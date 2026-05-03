#include <msp430.h>

volatile int counter = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;

    // Configura LED VERMELHO
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;

    // Configura LED VERDE
    P4OUT &= ~BIT7;
    P4DIR |= BIT7;

    // Configura BOTÃO S1
    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;
    P2IES |= BIT1; //INTERRUPÇÃO NA DESCIDA
    P2IE |= BIT1; //LIGA INTERRUPÇÃO
    P2IFG &= ~BIT1; //LIMPA A FLAG INICIALMENTE

    // Configura BOTÃO S2
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;
    P1IES |= BIT1; //INTERRUPÇÃO NA DESCIDA
    P1IE |= BIT1; //LIGA INTERRUPÇÃO
    P1IFG &= ~BIT1; //LIMPA A FLAG INICIALMENTE

    __enable_interrupt();

    while(1)
    {

    }

}

volatile void debouce(volatile unsigned int tempo){
  volatile int i;
  for (i = 0; i <= tempo; i++);
  return;
}

void acendeCounter(){
    if ((counter >= 0) && (counter <= 3)) {
        switch (counter) {
            case 0:
                P1OUT &= ~BIT0;
                P4OUT &= ~BIT7;
                break;

            case 1:
                P1OUT &= ~BIT0;
                P4OUT |= BIT7;
                break;

            case 2:
                P1OUT |= BIT0;
                P4OUT &= ~BIT7;
                break;

            case 3:
                P1OUT |= BIT0;
                P4OUT |= BIT7;
                break;
            default: break;
        }   
    }
    return;
}

//INTERRUPÇÃO S1
#pragma vector = PORT2_VECTOR
__interrupt void apertaS1(void){
    switch (P2IV) {
        case 0x04:
        if ((counter > 0) && (counter <= 3)){
            counter--;
            debouce(20000);
            acendeCounter();
            P2IFG &= ~BIT1; //LIMPA FLAG
            break;
        }
        default:
            break;
    }

}

//INTERRUPÇÃO S2
#pragma vector = PORT1_VECTOR
__interrupt void apertaS2(void){
    switch (P1IV) {
        case 0x04:
        if ((counter >= 0) && (counter < 3)){
            counter++;
            debouce(20000);
            acendeCounter();
            P1IFG &= ~BIT1; //LIMPA FLAG
            break;
        }
        default:
            break;
    }

}

