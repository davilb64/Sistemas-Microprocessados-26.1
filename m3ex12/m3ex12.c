#include <msp430.h>
#include <stdint.h>

uint8_t dados_joystick[50]; 
uint8_t idx = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    // Configura o P6.1 (Canal A1)
    P6SEL |= BIT1;
    P6DIR &= ~BIT1;

    ADC12CTL0 &= ~ADC12ENC; 
    
    // Liga o ADC.
    ADC12CTL0 = ADC12ON;  
    
    // SHP = 0 (Gatilho pelo sinal de PWM e não pelo timer interno)
    // SHS_1 = Seleciona o canal 1 do Timer A0 como fonte de gatilho
    // CONSEQ_2 = Modo Single Channel Repetitivo
    ADC12CTL1 = ADC12SHS_1 | ADC12CONSEQ_2; 
    
    ADC12CTL2 = ADC12RES_0; // Resolução de 8 bits               
    ADC12MCTL0 = 1;         // Canal A1 (Pino 6.1)
    
    // Ativa a interrupção do ADC para o canal 0 
    ADC12IE |= ADC12IE0;

    // Habilita o conversor (NÃO usamos ADC12SC, pois o gatilho virá do Timer!)
    ADC12CTL0 |= ADC12ENC;

    // Período total do sinal: 100ms (Clock 500kHz)
    TA0CCR0 = 50000 - 1; 
    
    // Tempo em que o sinal fica em nível ALTO (Tempo de amostragem): ~14us
    TA0CCR1 = 7;         
    
    // Configura o Canal 1 do Timer A0 no modo Set/Reset (Gera o sinal PWM interno)
    TA0CCTL1 = OUTMOD_3;                 
    
    // Configura o Timer: Usa o SMCLK (TASSEL_2), Divisor por 2 (ID_1), Modo UP (MC_1)
    TA0CTL = TASSEL_2 | ID_1 | MC_1; 

    __enable_interrupt(); // Ativa as interrupções globais

    while(1) {

    }
    
    return 0;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    // Lê o valor recém-convertido
    dados_joystick[idx] = ADC12MEM0;
    
    idx++;
    if(idx >= 50) {
        idx = 0;
    }
}
