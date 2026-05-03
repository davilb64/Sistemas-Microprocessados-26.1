#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;    
  //CONFIG BOTÃO
    P2DIR &= ~BIT1; //BOTÃO PARA ENTRADA
    P2REN |= BIT1; //RESISTOR
    P2OUT |= BIT1; //PULL UP

  //CONFIG LED
    P4DIR |= BIT7;

  while(1)                               
  {
    if ((P2IN & BIT1) == 0) {
      P4OUT |= BIT7;
    }
    else {
      P4OUT &= ~BIT7;
    }
  }
  return 0;   
}