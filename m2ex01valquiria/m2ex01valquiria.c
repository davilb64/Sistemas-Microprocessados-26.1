#include <msp430.h>

#define liga_led_verde P4OUT |= BIT7

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   
  //PORTA 4.7 SAIDA
  P4DIR |= BIT7;

  //PORTA 2.1 ENTRADA
  P2DIR &= ~BIT1;
  P2REN |= BIT1;
  P2OUT |= BIT1;


  while(1)                               
  {
    if ((P2IN & BIT1) == 0) {
      liga_led_verde;
    } else {
      P4OUT &= ~BIT7;
    }
  }

}

