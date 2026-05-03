#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    //CONFIGURAÇÃO DO PINO 
    P1DIR |= BIT2; // P1.2 como saída
    P1SEL |= BIT2; // joga pro timer

    //frequencia
    TA0CCR0 = 128 - 1; 

    TA0CCTL1 = OUTMOD_7; //Reset/Set 
    TA0CCR1 = 64;       

    //TIMER
    //SMCLK 1MHz
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;


    while(1)
    {
        
    }
}