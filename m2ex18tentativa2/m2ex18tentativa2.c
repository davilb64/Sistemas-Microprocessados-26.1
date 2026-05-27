#include <msp430.h>
#include <stdint.h> 

// variaveis pro infravermelho
volatile unsigned int tempoAnterior = 0;
volatile unsigned int contadorBits = 0;
volatile uint32_t codigoRecebido = 0;    
volatile uint8_t codigoPronto = 0; 
volatile unsigned int ultimoDelta = 0;

// brilho dos leds (começa no máximo (duty cycle 100% -> 0%brilho))
unsigned int brilhoRed = 20000;
unsigned int brilhoGreen = 20000;
unsigned int brilhoBlue = 20000;

#define PASSO 2000 //quanto aumenta ou diminui brilho

int main()
{
    WDTCTL = WDTPW + WDTHOLD; 
    
    //infravermelho
    P1DIR &= ~BIT2; //IR COMO ENTRADA
    P1REN |= BIT2;  //RESISTOR
    P1OUT |= BIT2;  //PULLUP
    P1SEL |= BIT2;  //Seleciona para o timer Timer (P1.2 -> TA0.1)

    TA0CCTL1 = CM_2 | CCIS_0 | SCS | CAP | CCIE; //CONF CAPTURA DO INFRAVERMELHO
    TA0CTL = TASSEL__SMCLK | MC__CONTINOUS | TACLR; //CONF TIMER A 0

    //leds
    // P2.0 (Vermelho) P2.4 (Verde) P2.5 (Azul)
    P2OUT |= (BIT0 + BIT4 + BIT5);
    P2DIR |= (BIT0 + BIT4 + BIT5); 
    P2SEL |= (BIT0 + BIT4 + BIT5); //LEDS PRO PWM
    

    // TIMER A1: vermelho
    TA1CCR0 = 20000; //50 hz
    TA1CCTL1 = OUTMOD_7; // canal vermelho (1)
    TA1CCR1 = brilhoRed;
    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR; // inicio timer

    // TIMER A2: led verde (P2.4 -> TA2.1) e azul (P2.5 -> TA2.2)
    TA2CCR0 = 20000; //50hz
    TA2CCTL1 = OUTMOD_7; //canal verde (1)
    TA2CCTL2 = OUTMOD_7; //canal azul (2)
    TA2CCR1 = brilhoGreen;      
    TA2CCR2 = brilhoBlue;       
    TA2CTL = TASSEL__SMCLK | MC__UP | TACLR; // inicio timer

    __enable_interrupt();

    while(1)
    {
        if(codigoPronto == 1){
            switch (codigoRecebido) {
                // controle do Vermelho
                case 0xBA45FF00: // aumenta brilho
                {
                    if (brilhoRed >= PASSO) {
                        brilhoRed -= PASSO;
                    } else {
                        brilhoRed = 0;
                    }
                    break;
                }
                case 0xBB44FF00: // diminui brilho
                {
                    if (brilhoRed <= (20000 - PASSO)) {
                        brilhoRed += PASSO;
                    } else {
                        brilhoRed = 20000;
                    }
                    break;
                }

                // controle do Verde
                case 0xB946FF00: // aumenta brilho
                {
                    if (brilhoGreen >= PASSO) {
                        brilhoGreen -= PASSO;
                    } else {
                        brilhoGreen = 0;
                    }
                    break;
                }
                case 0xBF40FF00: // diminui brilho
                {
                    if (brilhoGreen <= (20000 - PASSO)) {
                        brilhoGreen += PASSO;
                    } else {
                        brilhoGreen = 20000;
                    }
                    break;
                }

                // controle do Azul
                case 0xB847FF00: // aumenta brilho
                {
                    if (brilhoBlue >= PASSO) {
                        brilhoBlue -= PASSO;
                    } else {
                        brilhoBlue = 0;
                    }
                    break;
                }
                case 0xBC43FF00: // diminui brilho
                {
                    if (brilhoBlue <= (20000 - PASSO)) {
                        brilhoBlue += PASSO;
                    } else {
                        brilhoBlue = 20000;
                    }
                    break;
                }

                //max vermelho no 7
                case 0xF807FF00:
                {
                    brilhoRed = 0;
                    break;
                }

                //max VERDE no 8
                case 0xEA15FF00:
                {
                    brilhoGreen = 0;
                    break;
                }

                //max AZUL no 9
                case 0xF609FF00:
                {
                    brilhoBlue = 0;
                    break;
                }

                 //ZERA TUDO no *
                case 0xE916FF00:
                {
                    brilhoBlue = 20000;
                    brilhoGreen = 20000;
                    brilhoRed = 20000;
                    break;
                }

                 //max TUDO NO #
                case 0xF20DFF00:
                {
                    brilhoBlue = 0;
                    brilhoGreen = 0;
                    brilhoRed = 0;
                    break;
                }

                default: break;
            }
            
            // atualiza brilhos
            TA1CCR1 = brilhoRed;  
            TA2CCR1 = brilhoGreen; 
            TA2CCR2 = brilhoBlue;  

            codigoPronto = 0; // espera próximo aperto
        }
    }
}

// Interrupção do Infravermelho (Timer A0)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void interruptInfra(){
    switch (TA0IV) {
        case 0x02: //CCR1
        {
            unsigned int tempoAtual = TA0CCR1;
            unsigned int delta;
            
            delta = tempoAtual - tempoAnterior;
            tempoAnterior = tempoAtual;
            ultimoDelta = delta;

            // caso start bit (11000 a 15000 us)
            if (delta > 11000 && delta < 15000) 
            {
                contadorBits = 0; 
                codigoRecebido = 0;
                codigoPronto = 0;
            }
            // verifica BIT 1 (1800 a 2800 us)
            else if (delta > 1800 && delta < 2800) 
            {
                codigoRecebido |= ((uint32_t)1 << contadorBits); 
                contadorBits++;
            }
            // verifica BIT 0 (700 a 1600 us)
            else if (delta > 700 && delta < 1600)
            {
                contadorBits++;
            }

            // confere se chegou nos 32 bits
            if (contadorBits >= 32) 
            {
                codigoPronto = 1; 
                contadorBits = 0; 
            }
            break;
        }
        default:
        {
             break;
        }
    }
}
