#include <msp430.h>

void main(void) {
    // Para o Watchdog Timer
    WDTCTL = WDTPW | WDTHOLD; 

    // Configura o pino P4.1 para ser a saída do Timer B0 (Sinal PWM)
    P4DIR |= BIT1;
    P4SEL |= BIT1;

    // Configura o Timer B0 para gerar uma frequência de 50Hz (20ms)
    // O clock base (SMCLK) é de ~1.048 MHz. Logo: 1.048.576 * 0.02 = ~20971
    TB0CCR0 = 20971 - 1;
    TB0CCTL1 = OUTMOD_7; // Modo Reset/Set
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR;

    while(1) {
        // Manda o motor para uma ponta (Pulso de ~1.0 ms)
        TB0CCR1 = 1048;
        
        // Espera ~1 segundo (1 milhão de ciclos do clock de 1MHz)
        __delay_cycles(1048576); 

        // Manda o motor para a outra ponta (Pulso de ~2.0 ms)
        TB0CCR1 = 2096;
        
        // Espera ~1 segundo
        __delay_cycles(1048576); 
    }
}
