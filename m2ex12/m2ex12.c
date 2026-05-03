#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    //CONFIGURA LED
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;

    // CONFIGURA BOTÃO
    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;
    P2IES |= BIT1;  // Interrupção na descida
    P2IFG &= ~BIT1; // Limpa a flag inicialmente
    P2IE |= BIT1;   // Liga a interrupção da porta 2

    // CONFIGURA TIMER
    // SMCLK 1MHz 
    TA0CCR0 = 30000; 
    //interrupção do timer
    TA0CCTL0 = CCIE; 
    TA0CTL = TASSEL__SMCLK | MC__STOP | TACLR;

    __enable_interrupt();

    while(1)
    {

    }
}

// INTERRUPÇÃO DO BOTÃO
#pragma vector = PORT2_VECTOR
__interrupt void apertaS1(void){
    
    switch (P2IV) {
        case 0x04: 
            //Desliga a interrupção do botão
            P2IE &= ~BIT1; 
            // Liga o Timer 
            TA0CTL |= MC__UP; 
            break;
            
        default: 
            break;
    }
}

// INTERRUPÇÃO DO TIMER
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void){
    
    //para o Timer 
    TA0CTL &= ~MC__UP; 
    TA0CTL |= TACLR; // zera o contador
    
    if ((P2IN & BIT1) == 0) {
        P1OUT ^= BIT0; // inverte 
    }
    
    P2IFG &= ~BIT1; 
    
    //religa a interrupção do botão
    P2IE |= BIT1; 
}