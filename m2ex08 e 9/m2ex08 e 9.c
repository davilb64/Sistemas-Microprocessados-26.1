#include <msp430.h>

volatile void debouce(volatile unsigned int tempo);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    //CONFIGURAÇÃO DO PINO 
    P1DIR |= BIT2; // P1.2 como saída (colocar led externo pra ver)
    P1SEL |= BIT2; // joga pro timer

    //CONFIGURAÇÃO DOS BOTÕES
    //S1
    P2DIR &= ~BIT1; //ENTRADA
    P2REN |= BIT1; //RESISTOR
    P2OUT |= BIT1; //PULL UP

    //S2
    P1DIR &= ~BIT1; //ENTRADA
    P1REN |= BIT1; //RESISTOR
    P1OUT |= BIT1; //PULL UP


    //frequencia
    TA0CCR0 = 128 - 1; 

    TA0CCTL1 = OUTMOD_7; //Reset/Set 
    TA0CCR1 = 64;       

    //TIMER
    //SMCLK 1MHz
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;


    while(1)
    {
        if (((P1IN & BIT1) == 0) && (TA0CCR1 <= 128)) {
            TA0CCR1 += 16;
            debouce(20000);
        }
        if (((P2IN & BIT1) == 0) && (TA0CCR1 >= 16)) {
            TA0CCR1 -= 16;
            debouce(20000);
        }
    }
}

volatile void debouce(volatile unsigned int tempo){
  volatile int i;
  for (i = 0; i <= tempo; i++);
  return;
}