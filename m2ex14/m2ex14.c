#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;        
  
  // LED saída
  P1OUT &= ~BIT0;
  P1DIR |= BIT0; 

  // TIMER ACLK = 32768 Hz
  TA0CTL = TASSEL__ACLK | MC__UP; 
  // 16384 ticks = 0.5 segundos
  TA0CCR0 = 16383;
  TA0CCTL0 = CCIE; //interrupção timer
  __enable_interrupt();
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void piscaLed(){
  P1OUT ^= BIT0;
}