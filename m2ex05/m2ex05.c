#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;        
  //led saida
  P1DIR |= BIT0;           

  while(1)                               
  {
    atraso(20000);
    P1OUT ^= BIT0;
  } 
}

volatile void atraso (volatile unsigned int tempo){
  volatile int i = 0;
  for (i; i <= tempo; i++);
  return;
}