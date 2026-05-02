#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   
  P1DIR |= BIT0; //p1.0 para saida
  P2DIR &= ~BIT1;   //p2.1 para entrada
  P2REN |= BIT1;    //ativa resistor 2.1
  P2OUT |= BIT1;    //pullup

  while(1)                               
  {
    if ((P2IN & BIT1) == 0) { 
      P1OUT ^= BIT0;                  
    }
  } 
}
