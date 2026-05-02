#include <msp430.h>

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;  

    P1DIR |= BIT2;      
    P1SEL |= BIT2;            
    
    TA0CCR0 = 32768;    
    
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 16384;          

    TA0CTL = TASSEL_1 | MC_1 | TACLR; 


    __bis_SR_register(LPM3_bits); 
}
