#include <msp430.h>
#include <stdint.h>

uint8_t dados_X[50]; 
uint8_t dados_Y[50]; 
uint8_t idx_X = 0;
uint8_t idx_Y = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    // 6.0 X e P6.1 Y
    P6SEL |= BIT0 | BIT1;
    P6DIR &= ~(BIT0 | BIT1);

    ADC12CTL0 &= ~ADC12ENC; 
    
    ADC12CTL0 = ADC12ON;  
    
    ADC12CTL1 = ADC12SHS_1 | ADC12CONSEQ_3; 
    
    ADC12CTL2 = ADC12RES_0; // Resolução de 8 bits               
    
    // Configuração dos buffers de memória (MCTL)
    ADC12MCTL0 = 0;                   // MCTL0 lê o Canal A0 (Eixo X)
    ADC12MCTL1 = 1 | ADC12EOS;        // MCTL1 lê o Canal A1 (Eixo Y) e sinaliza Fim da Sequência (EOS)
    
    // Ativa a interrupção para os dois registos de memória
    ADC12IE |= ADC12IE0 | ADC12IE1;

    // Habilita o conversor
    ADC12CTL0 |= ADC12ENC;

    TA0CCR0 = 50000 - 1; // Período de 100ms
    TA0CCR1 = 7;         // ~14us para tempo de amostragem
    TA0CCTL1 = OUTMOD_3;                 
    TA0CTL = TASSEL_2 | ID_1 | MC_1; 

    __enable_interrupt(); 

    while(1) {
    }
    
    return 0;
}


#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    // Verifica se a interrupção foi do registo 0 (Eixo X)
    if (ADC12IFG & ADC12IFG0) {
        dados_X[idx_X++] = ADC12MEM0; // Limpa a flag automaticamente ao ler
        if (idx_X >= 50) idx_X = 0;
    }
    
    // Verifica se a interrupção foi do registo 1 (Eixo Y)
    if (ADC12IFG & ADC12IFG1) {
        dados_Y[idx_Y++] = ADC12MEM1; 
        if (idx_Y >= 50) idx_Y = 0;
    }
}
