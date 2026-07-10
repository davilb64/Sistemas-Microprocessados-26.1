#include <msp430.h>
#include <stdint.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    // liberação do pmap
    PMAPKEYID = 0x02D52;
    
    // mapeamento das saídas do timer B pros pinos dos leds
    P4MAP3 = PM_TB0CCR1A; // joga o sinal pwm do timerB (canal 1) pro P4.3 (led vermelho - eixo X)
    P4MAP0 = PM_TB0CCR2A; // joga o sinal pwm do timerB (canal 2) pro P4.0 (led verde - eixo Y)
    
    // configura os pinos dos leds pra saída 
    P4DIR |= BIT3 | BIT0;
    P4SEL |= BIT3 | BIT0; // manda para pwm

    // prepara os pinos de leitura analógica do joystick - P6.0 (eixo X) e P6.1 (eixo Y)
    P6SEL |= BIT0 | BIT1; // liga a função de ADC nos pinos
    P6DIR &= ~(BIT0 | BIT1); // pinos para entradas

    // config timer B0 (PWM dos LEDs a 100Hz)
    // usando ACLK (32kHz): 32768 / 256 passos = ~128 Hz (100Hz do roteiro)
    TB0CCR0 = 255; // limite máximo do contador (combina com os 8 bits do adc)
    
    // modo reset/set
    TB0CCTL1 = OUTMOD_7; 
    TB0CCTL2 = OUTMOD_7;
    
    TB0CTL = TBSSEL_1 | MC_1; // clock ACLK, modo UP 

    // config timer A0 (amostragem do ADC)
    // frequência de 50Hz usando o SMCLK (1.048MHz): 1.048.000 / 50 = ~20960
    TA0CCR0 = 20000 - 1; 
    
    // calculo do capacitor (tempo de amostragem)
    // tcap = (Rs + Ri) * Ci * ln(2^(N+1)) + 800ns
    // Rs = 10k, Ri = 1.8k, Ci = 25pF, N = 12 bits
    // tcap = 11800 * 25e-12 * ln(8192) + 800ns = ~3.45 us
    // com o timer A0 a 1.048MHz (cada tick = 0.95us), 14 ticks = ~13.3 us
    TA0CCR1 = 14;        // gera um pulso rápido (14us) pro ADC carregar o capacitor (sample & hold)
    
    TA0CCTL1 = OUTMOD_3; // modo set/reset       
    TA0CTL = TASSEL_2 | MC_1; // clock SMCLK, Modo UP

    // config conversor A/D
    ADC12CTL0 &= ~ADC12ENC; // desliga o ADC para config
    ADC12CTL0 = ADC12ON;    // liga o núcleo do ADC
    
    // gatilho pelo pulso do timer A0.1 e sequência repetitiva 
    ADC12CTL1 = ADC12SHS_1 | ADC12CONSEQ_3; 
    ADC12CTL2 = ADC12RES_0; // resolução de 8 bits (vai dar um valor de 0 a 255, combina com pwm configurado acima)     
    
    ADC12MCTL0 = 0; // canal A0 (lê o eixo X do joystick)
    ADC12MCTL1 = 1 | ADC12EOS; // canal A1 (lê o eixo Y) + EOS avisa que é o Fim da Sequência (End Of Sequence)
    
    // ativa as interrupções de conversão
    ADC12IE |= ADC12IE0 | ADC12IE1;

    // habilita o conversor
    ADC12CTL0 |= ADC12ENC;

    __enable_interrupt(); // libera as interrupções globais
    while(1) {

    }
    
}

// Interrupção ADC
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    // se a flag do canal 0 subiu (converteu o eixo X)
    if (ADC12IFG & ADC12IFG0) {
        // pega a leitura do ADC e joga na largura do pulso PWM do led Vermelho
        TB0CCR1 = ADC12MEM0; 
    }
    
    // se a flag do canal 1 subiu (converteu o eixo Y)
    if (ADC12IFG & ADC12IFG1) {
        // eixo invertido: 255 menos a leitura  para corrigir a direção do brilho
        TB0CCR2 = 255 - ADC12MEM1; 
    }
}
