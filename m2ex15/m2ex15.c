#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;        
  
  // LEDS como saída
  P1OUT &= ~BIT0 ; 
  P4OUT &= ~BIT7;
  P1DIR |= BIT0;
  P4DIR |= BIT7;

  // ccr0 (valor qualquer visto q estamos só buscando uma porcentagem de luminosidade, não marcando tempo especificamente)
  TA0CCR0 = 1000;

  // deuty cycle
  TA0CCR1 = 300; // 30% led vermeklho
  TA0CCR2 = 700; // 70% led verde

  // liga interrupções
  TA0CCTL0 = CCIE; // CCR0
  TA0CCTL1 = CCIE; // CCR1
  TA0CCTL2 = CCIE; // CCR2

  // timer
  TA0CTL = TASSEL__SMCLK | MC__UP | TACLR; 
  
  __enable_interrupt();

  while(1)                               
  {
      
  } 
}

//liga a cada ciclo (ccr0 estoura)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void iniciaCiclo(){
  P1OUT |= BIT0;
  P4OUT |= BIT7;
}

//ativa quando counter bate ccr1 ou ccr2
#pragma vector = TIMER0_A1_VECTOR
__interrupt void controlaDutyCycle(){
  switch(TA0IV) {
      
      case TA0IV_TACCR1:
          P1OUT &= ~BIT0; //apaga vermelho
          break;
          
      case TA0IV_TACCR2: 
          P4OUT &= ~BIT7; // Apaga verde
          break;
          
      default:
          break;
  }
}