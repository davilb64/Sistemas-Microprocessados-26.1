#include <msp430.h>

void debounce(unsigned long tempo);

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   
  P1DIR |= BIT0; //p1.0 para saida

  P2DIR &= ~BIT1;   //p2.1 para entrada
  P2REN |= BIT1;    //ativa resistor 2.1
  P2OUT |= BIT1;    //pullup

  P1DIR &= ~BIT1;   //p1.1 para entrada
  P1REN |= BIT1;    //ativa resistor 1.1
  P1OUT |= BIT1;    //pullup

  while(1)                               
  {
    if (((P2IN & BIT1) == 0)||(P1IN & BIT1) == 0) { 
      P1OUT ^= BIT0; 
      debounce(10000);               
    }
  } 
}

void debounce(unsigned long tempo){
    volatile unsigned long i = 0; 
    for(i=0; i < tempo; i++);
    return;
}