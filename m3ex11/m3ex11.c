#include <msp430.h>
#include <stdint.h>

uint8_t dados_joystick[50]; 
uint8_t idx = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    // P6.1 
    P6SEL |= BIT1;
    P6DIR &= ~BIT1;

    // Configuração do ADC
    ADC12CTL0 &= ~ADC12ENC; 
    
    // ADC ligado, S&H = 16 clocks, e o segredo: ADC12MSC para rodar em loop infinito
    ADC12CTL0 = ADC12ON | ADC12SHT0_2 | ADC12MSC;  
    
    // Timer interno, Repeated Single Channel (CONSEQ_2) e Clock SMCLK (SSEL_3)
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_2 | ADC12SSEL_3; 
    
    ADC12CTL2 = ADC12RES_0; // 8 bits               
    ADC12MCTL0 = 1;         // Canal A1
    
    // Habilita o conversor e DÁ O GATILHO INICIAL AQUI!
    ADC12CTL0 |= ADC12ENC | ADC12SC;

    // Timer A0 (100ms)
    TA0CCR0 = 12500 - 1; 
    TA0CCTL0 = CCIE;                 
    TA0CTL = TASSEL_2 | ID_3 | MC_1; 

    __enable_interrupt(); 

    while(1) {
        // CPU totalmente livre, o hardware trabalha por você
    }
    
    return 0;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    dados_joystick[idx] = ADC12MEM0;
    
    idx++;
    if(idx >= 50) {
        idx = 0; 
    }
}