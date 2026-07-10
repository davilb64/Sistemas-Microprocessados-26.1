#include <msp430.h>
#include <stdint.h> 

// variaveis pro infravermelho
volatile unsigned int tempoAnterior = 0;
volatile unsigned int contadorBits = 0;
volatile uint32_t codigoRecebido = 0;  
volatile uint8_t codigoPronto = 0; 
volatile unsigned int ultimoDelta = 0;
volatile uint8_t recebendo = 0; 

// brilho dos leds
unsigned int brilhoRed = 20000;
unsigned int brilhoGreen = 20000;
unsigned int brilhoBlue = 20000;

#define PASSO 2000// quanto brilho aumenta/diminui cada vez que apertar

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    //leds
    // RED (P2.5 -> TA2.2) e 
    P2DIR |= BIT4;
    P2SEL |= BIT4; // manda o pino pro Timer 2

    //GREEN (P2.4 -> TA2.1)
    P2DIR |= BIT5;
    P2SEL |= BIT5; // manda o pino pro Timer 2

    // BLUE (P1.5 -> TA0.4)
    P1DIR |= BIT5;
    P1SEL |= BIT5; // manda o pino pro Timer 0

    // timeR 2 (RED e GREEN)
    TA2CCR0 = 20000; // 50hz
    TA2CCTL2 = OUTMOD_7; // canal red (2)
    TA2CCTL1 = OUTMOD_7; // canal green (1)
    TA2CCR2 = brilhoRed;
    TA2CCR1 = brilhoGreen;
    TA2CTL = TASSEL__SMCLK | MC__UP | TACLR;

    // RECEPTOR (P1.3 -> TA0.2)
    P1DIR &= ~BIT3;
    P1REN |= BIT3;  
    P1OUT |= BIT3;  
    P1SEL |= BIT3;  

    // timer 0 (IR e Blue)
    TA0CCR0 = 20000; // 50hz
    TA0CCTL4 = OUTMOD_7; // canal blue (4)
    TA0CCR4 = brilhoBlue;
    
    // canal IR (2)
    TA0CCTL2 = CM_2 | CCIS_0 | SCS | CAP | CCIE;
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR; 

    __enable_interrupt();

    while(1)
    {
        if (codigoPronto == 1) 
        {
            switch(codigoRecebido) 
            {
                // vermelho
                case 0xBA45FF00: // bot 1: aumenta vermelho
                    if (brilhoRed >= PASSO){
                        brilhoRed -= PASSO;
                    }  
                    else {
                        brilhoRed = 0; // brilho max se já estourou
                    }
                    codigoPronto = 0;
                    break;
                    
                case 0xBB44FF00: // bot 4: diminui vermelho
                    if (brilhoRed <= (20000 - PASSO)) {
                        brilhoRed += PASSO;
                    }
                    else {
                        brilhoRed = 20000; // brilho min se já estourou
                    }
                    codigoPronto = 0;
                    break;

                // verde
                case 0xB946FF00: // bot 2: aumenta verde
                    if (brilhoGreen >= PASSO){
                        brilhoGreen -= PASSO;
                    }
                    else {
                        brilhoGreen = 0;
                    } 
                    codigoPronto = 0;
                    break;
                    
                case 0xBF40FF00: // bot 5: diminui verde
                    if (brilhoGreen <= (20000 - PASSO)){
                        brilhoGreen += PASSO;
                    } 
                    else {
                        brilhoGreen = 20000;
                    }
                    codigoPronto = 0; 
                    break;

                // azul
                case 0xB847FF00: // bot 3: aumenta azul
                    if (brilhoBlue >= PASSO){
                        brilhoBlue -= PASSO;
                    } 
                    else {
                        brilhoBlue = 0;
                    }
                    codigoPronto = 0; 
                    break;
                    
                case 0xBC43FF00: // bot 6: diminui azul
                    if (brilhoBlue <= (20000 - PASSO)){
                        brilhoBlue += PASSO;
                    } 
                    else {
                        brilhoBlue = 20000;
                    } 
                    codigoPronto = 0; 
                    break;

                case 0xF20DFF00: //bot #: zera tudo
                    brilhoRed = 20000;
                    brilhoGreen = 20000;
                    brilhoBlue = 20000;
                    codigoPronto = 0; 
                    break;

                case 0xE916FF00: //bot *: estoura tudo
                    brilhoRed = 0;
                    brilhoGreen = 0;
                    brilhoBlue = 0;
                    codigoPronto = 0; 
                    break;

                default:
                    codigoPronto = 0;
                    break;
            }

            // atualiza as cores
            TA2CCR2 = brilhoRed;
            TA2CCR1 = brilhoGreen;
            TA0CCR4 = brilhoBlue;

            codigoPronto = 0; // espera aperto
        }
    }
}

// interrupção do infravermelho
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
    switch(TA0IV) 
    {
        case 0x04: // CCR2
        {
            unsigned int tempoAtual = TA0CCR2;
            unsigned int delta;
            
            if (tempoAtual >= tempoAnterior) {
                delta = tempoAtual - tempoAnterior;
            } else {
                delta = (20000 - tempoAnterior) + tempoAtual;
            }
            
            tempoAnterior = tempoAtual;
            ultimoDelta = delta;
            
            // verifica start bit
            if (delta > 11000 && delta < 15000) 
            {
                contadorBits = 0;
                codigoRecebido = 0;
                codigoPronto = 0;
                recebendo = 1; 
            }
            // Só analisa os bits SE tiver recebido um start bit antes
            else if (recebendo == 1) 
            {
                // verifica BIT 1
                if (delta > 1800 && delta < 2800) 
                {
                    codigoRecebido |= ((uint32_t)1 << contadorBits); 
                    contadorBits++;
                }
                // verifica BIT 0
                else if (delta > 700 && delta < 1600)
                {
                    contadorBits++;
                }

                else 
                {
                    recebendo = 0; 
                    contadorBits = 0;
                }

                // confere se chegou nos 32 bits
                if (contadorBits >= 32) 
                {
                    if (codigoRecebido != 0x00000000 && codigoRecebido != 0xFFFFFFFF) 
                    {
                        codigoPronto = 1; 
                    }
                    
                    recebendo = 0;
                    contadorBits = 0; 
                }
            }
            break;
        }
        default: break;
    }
}
