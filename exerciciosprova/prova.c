#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;           
  
  // CONFIG BOTÃO (P1.2)        
  P1DIR &= ~BIT2;  // P1.2 ENTRADA
  P1REN |= BIT2;   // LIGA RESISTOR
  P1OUT |= BIT2;   // PULL UP
  P1SEL &= ~BIT2;  // GPIO
  
  P1IES |= BIT2;   // FLANCO DE DESCIDA 
  P1IFG &= ~BIT2;  // ZERA IFG 
  P1IE |= BIT2;    // LIGA INTERRUPÇÃO
  
  // CONFIG LED (P3.0)
  P3DIR |= BIT0;   // P3.0 SAIDA
  P3OUT &= ~BIT0;  // APAGADO 

  // CONFIG TIMER PARA DEBOUNCE
  TA0CTL = TASSEL__SMCLK | ID__8 | MC__STOP | TACLR;
  
  TA0CCR0 = 6250; //50ms
  TA0CCTL0 = CCIE; 

  __enable_interrupt();
  
  while (1) {
    __no_operation();
  }
}

#pragma vector = PORT1_VECTOR 
__interrupt void botao(){
  switch (P1IV) {
    case 0x06:
      P1IE &= ~BIT2;            // 1. Fica "surdo" para novos cliques
      P1IFG &= ~BIT2;           // 2. Limpa a sujeira inicial
      TA0CTL |= MC__UP | TACLR; // 3. Dispara o cronômetro do Timer
      break;
    default: 
      break;
  }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timer(){
  
  TA0CTL &= ~(MC0 | MC1);
  if ((P1IN & BIT2) == 0) { 
      P3OUT ^= BIT0;
  }
  P1IFG &= ~BIT2;
  P1IE |= BIT2;  
}
