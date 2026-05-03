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

  while(1)                               
  {
    while(!(TA0CCTL0 & CCIFG)); 
    //limpa flag
    TA0CCTL0 &= ~CCIFG; 
    P1OUT ^= BIT0;
  } 
}