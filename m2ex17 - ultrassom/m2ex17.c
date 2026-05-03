#include <msp430.h>

unsigned int tempo_inicial = 0;
unsigned int tempo_final = 0;
unsigned int distancia_cm = 0;

void dispara_sensor(void);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 

    //config portas
    P4DIR |= BIT0; 
    P3DIR |= BIT7; 
    P8DIR |= BIT2; 
    P4OUT &= ~BIT0; 
    P3OUT &= ~BIT7; 
    P8OUT &= ~BIT2; 

    // CONFIGURA O TRIGGER (P2.4 como saída)
    P2DIR |= BIT4; 
    P2OUT &= ~BIT4; //0 trigger


    // CONFIGURA O ECHO (P2.3 conectado ao TA2.0)
    P2DIR &= ~BIT3; 
    P2SEL |= BIT3;  // manda pro ta2.0
    
    // Configura o Timer 2 no Modo Contínuo (SMCLK ~1MHz)
    TA2CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR; 
    
    // Configura o Canal 0 (TA2CCR0) para Captura:
    // CM_3   -> Captura em AMBOS os flancos (Subida e Descida)
    // CCIS_0 -> Lê o pino P2.3 (CCI0A)
    // SCS    -> Sincroniza
    // CAP    -> Liga o Modo Captura
    // CCIE   -> Habilita interrupção VIP do CCR0
    TA2CCTL0 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    __enable_interrupt();

    while(1)
    {
        dispara_sensor();
        
        // Espera 60 milissegundos antes de disparar de novo.
        __delay_cycles(60000); 
    }
}

void dispara_sensor(void)
{
    P2OUT |= BIT4;         // Sobe o Trigger para HIGH
    __delay_cycles(10);    // Espera exatos 10 microssegundos (se SMCLK for 1MHz)
    P2OUT &= ~BIT4;        // Desce o Trigger para LOW
}


// INTERRUPÇÃO DA CAPTURA DO ECHO (Porta VIP do Timer 2 - CCR0)
#pragma vector = TIMER2_A0_VECTOR
__interrupt void Timer2_A0_ISR(void)
{
    
    if (TA2CCTL0 & CCI) 
    {
        tempo_inicial = TA2CCR0; // Guarda a hora que começou
    } 
    else 
    {
        tempo_final = TA2CCR0; 
        
        unsigned int duracao = tempo_final - tempo_inicial;
        
        // Transforma o tempo (microssegundos) em Distância (centímetros)
        distancia_cm = duracao / 58;
        
        if (distancia_cm < 15) {
            P4OUT |= BIT0;  
            P3OUT &= ~BIT7; 
            P8OUT &= ~BIT2;
        } 
        else if (distancia_cm > 30) {
            P4OUT &= ~BIT0;  
            P3OUT |= BIT7; 
            P8OUT &= ~BIT2;
        } 
        else {
            P4OUT &= ~BIT0;  
            P3OUT &= ~BIT7; 
            P8OUT |= BIT2;
        }
    }
}