#include <msp430.h>
#include <stdint.h>

uint16_t adcRead(uint8_t pin);

int main()
{
    WDTCTL = WDTPW + WDTHOLD; 
     
    while (1) {
        adcRead(0);
    }

}

uint16_t adcRead(uint8_t pin) {
    // Configura o pino da porta P6 (0 a 7)
    P6SEL |= (1 << pin);
    P6DIR &= ~(1 << pin);

    // Desabilita o ADC
    ADC12CTL0 &= ~ADC12ENC;

    // Liga o ADC (ADC12ON) e ajusta Sample & Hold para 16 ciclos de clock
    ADC12CTL0 = ADC12ON | ADC12SHT0_2;

    // Usa o timer de amostragem interno (ADC12SHP) e seleciona o SMCLK (ADC12SSEL_3)
    ADC12CTL1 = ADC12SHP | ADC12SSEL_3;

    // Configura a resolução para 12 bits
    ADC12CTL2 = ADC12RES_2; 

    // Configura o multiplexador (ADC12MCTL0) para apontar para o canal INCHx desejado
    ADC12MCTL0 = pin;

    // Habilita o conversor (ENC) e dispara o gatilho da conversão (SC)
    ADC12CTL0 |= ADC12ENC | ADC12SC;

    // Trava o código esperando a flag de fim de conversão do canal 0
    while (!(ADC12IFG & ADC12IFG0));

    // Retorna o valor direto do registrador de memória
    return ADC12MEM0;
}
