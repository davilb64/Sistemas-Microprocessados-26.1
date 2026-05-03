#include <msp430.h>

#define MAX_REBOTES 50

unsigned int rebotes[MAX_REBOTES];
unsigned int indice_rebote = 0;
unsigned int tempo_anterior = 0;
unsigned char primeiro_aperto = 1;
volatile unsigned char fim_janela = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    // Botão S1 PARA MODO CAPTURA
    P2DIR &= ~BIT1; // P2.1 como entrada
    P2SEL |= BIT1;  // s1 para periferico
    P2REN |= BIT1;  // resistor
    P2OUT |= BIT1;  // pullup

    // TIMER 1 
    // SMCLK 1MHz Modo Contínuo
    TA1CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR; 
    
    // Configura o Capture/Compare Register 1 (CCR1) do Timer 1:
    // CM_3   -> Captura na subida e na descida (para pegar todos os rebotes)
    // CCIS_0 -> Seleciona a entrada CCI1A (que é fisicamente o P2.1)
    // SCS    -> Sincroniza com o clock 
    // CAP    -> Ativa o modo de Captura
    // CCIE   -> Habilita a interrupção da captura
    TA1CCTL2 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    // TIMER 0
    // ACLK no Modo STOP 
    TA0CTL = TASSEL__ACLK | MC__STOP | TACLR;
    TA0CCR0 = 32768; 
    TA0CCTL0 = CCIE; // Habilita a interrupção do fim do tempo

    __enable_interrupt();

    // CPU vai dormir no Low Power Mode 0 enquanto a janela estiver aberta (entender dps)
    while(!fim_janela)
    {
        __bis_SR_register(LPM0_bits); 
    }

    while(1)
    {
        __no_operation(); 
    }
}

// INTERRUPÇÃO DA CAPTURA (Timer 1 - P2.1)
// Roda toda vez que a mola do botão quica
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR(void)
{
    switch(TA1IV) 
    {
        case 0x04:
            
            if (primeiro_aperto == 1) 
            {
                primeiro_aperto = 0;
                tempo_anterior = TA1CCR2; // Lê do CCR2
                TA0CTL |= MC__UP; 
            } 
            else 
            {
                if (indice_rebote < MAX_REBOTES) 
                {
                    rebotes[indice_rebote] = TA1CCR2 - tempo_anterior;
                    tempo_anterior = TA1CCR2; 
                    indice_rebote++; 
                }
            }
            break;
            
        default: break;
    }
}

// INTERRUPÇÃO DO FIM DE 1 SEGUNDO (Timer 0)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void)
{
    // para tudo
    TA1CTL &= ~MC__CONTINUOUS; 
    TA0CTL &= ~MC__UP;         
    TA1CCTL2 &= ~CCIE;  

    fim_janela = 1; 

    __bic_SR_register_on_exit(LPM0_bits); //entender dps
}