#include <msp430.h>
#include <stdint.h> 

// Variáveis globais voláteis
volatile unsigned int tempoAnterior = 0;
volatile unsigned int contadorBits = 0;
volatile uint32_t codigoRecebido = 0;    
volatile uint8_t codigoPronto = 0; 
volatile uint8_t recebendo = 0; 

// Variável apenas para facilitar a sua visualização no Debug
volatile uint32_t ultimoBotaoApertado = 0; 

int main(void)
{
    // Para o Watchdog Timer
    WDTCTL = WDTPW + WDTHOLD; 

    // Configuração do pino do RECEPTOR IR (P1.3 -> TA0.2)
    P1DIR &= ~BIT3; // Configura como entrada
    P1REN |= BIT3;  // Habilita resistor
    P1OUT |= BIT3;  // Resistor de Pull-up
    P1SEL |= BIT3;  // Passa o controle do pino para o Timer0

    // Configuração do Timer0 (Base de tempo de 20ms)
    TA0CCR0 = 20000; 
    
    // Configura o Canal 2 para modo Captura (Borda de descida)
    TA0CCTL2 = CM_2 | CCIS_0 | SCS | CAP | CCIE;
    
    // Inicia o Timer (SMCLK, Modo UP, limpa o timer)
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR; 

    // Habilita as interrupções globais
    __enable_interrupt();

    while(1)
    {
        // Se a interrupção terminou de ler 32 bits válidos
        if (codigoPronto == 1) 
        {
            ultimoBotaoApertado = codigoRecebido; 

            // Limpa a flag para esperar o próximo botão
            codigoPronto = 0; 
        }
    }
}

// Interrupção do Timer0
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
    switch(TA0IV) 
    {
        case 0x04: // Interrupção do CCR2 (Pino P1.3)
        {
            unsigned int tempoAtual = TA0CCR2;
            unsigned int delta;
            
            // Calcula o tempo desde a última borda de descida
            if (tempoAtual >= tempoAnterior) {
                delta = tempoAtual - tempoAnterior;
            } else {
                delta = (20000 - tempoAnterior) + tempoAtual;
            }
            
            tempoAnterior = tempoAtual;
            
            // 1. Verifica START BIT (Pulso longo de ~13.5ms)
            if (delta > 11000 && delta < 15000) 
            {
                contadorBits = 0;
                codigoRecebido = 0;
                codigoPronto = 0;
                recebendo = 1; // Libera a captura dos próximos bits
            }
            // 2. Só analisa se já recebeu um Start Bit válido
            else if (recebendo == 1) 
            {
                // Verifica BIT 1 (Pulso de ~2.25ms)
                if (delta > 1800 && delta < 2800) 
                {
                    codigoRecebido |= ((uint32_t)1 << contadorBits); 
                    contadorBits++;
                }
                // Verifica BIT 0 (Pulso de ~1.12ms)
                else if (delta > 700 && delta < 1600)
                {
                    contadorBits++;
                }
                // Se o tempo não for nem 0 nem 1, é ruído. Aborta a leitura.
                else 
                {
                    recebendo = 0; 
                    contadorBits = 0;
                }

                // 3. Verifica se completou os 32 bits
                if (contadorBits >= 32) 
                {
                    // FILTRO ANTI-FANTASMA: Ignora se for puro zero ou puro um
                    if (codigoRecebido != 0x00000000 && codigoRecebido != 0xFFFFFFFF) 
                    {
                        codigoPronto = 1; // Avisa a main que tem código pronto
                    }
                    
                    recebendo = 0; // Trava leitura esperando novo Start Bit
                    contadorBits = 0; 
                }
            }
            break;
        }
        default: break;
    }
}