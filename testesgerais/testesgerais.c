#include <msp430.h>

void debounce(unsigned long tempo);

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   
  P2DIR |= BIT5; //p2.5 para saida
  P2DIR |= BIT4; //p2.4 para saida

  P2DIR &= ~BIT1;   //p2.1 para entrada
  P2REN |= BIT1;    //ativa resistor 2.1
  P2OUT |= BIT1;    //pullup

  P8DIR &= ~BIT1;   //p8.1 para entrada
  P8REN |= BIT1;    //ativa resistor 8.1
  P8OUT |= BIT1;    //pullup

  P1DIR &= ~BIT1;   //p1.1 para entrada
  P1REN |= BIT1;    //ativa resistor 1.1
  P1OUT |= BIT1;    //pullup

  while(1)                               
  {
    if (((P2IN & BIT1) == 0)) { 
      P2OUT ^= BIT5; 
      debounce(10000);               
    }
    if ((P1IN & BIT1) == 0) {
      P2OUT ^= BIT4; 
      debounce(10000); 
    }
    if ((P8IN & BIT1) == 0) {
      P2OUT ^= BIT4; 
      debounce(10000); 
    }
  } 
}

void debounce(unsigned long tempo){
    volatile unsigned long i = 0; 
    for(i=0; i < tempo; i++);
    return;
}