#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   
  //BOTAO ENTRADA
  P2DIR &= ~BIT1;
  P2REN |= BIT1;
  P2OUT |= BIT1;

  //LED SAIDA
  P1DIR |= BIT0;

  while(1)                               
  {
    if ((P2IN & BIT1) == 0) {
      P1OUT ^= BIT0;
    }
  } 
}
