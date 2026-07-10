#include <msp430.h>
#include <stdint.h>
#include "/home/davi/ti/ccs2050/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include/RTC Calendar/headers/RTC.h"

//-----------------------------------------------------
// CONFIGURAÇÕES DE TEMPO (Em Segundos)
//-----------------------------------------------------
#define TEMPO_ESTABILIDADE      10 // 10 ticks = 5 segs

#define INTERVALO_OXIGENADOR    60  
#define DURACAO_OXIGENADOR      10  

#define INTERVALO_ALIMENTADOR   50 
#define DURACAO_ALIMENTADOR     2   

//-----------------------------------------------------
// ATUADORES (Relés ativos em nível baixo)
//-----------------------------------------------------
#define LIGA_BOMBA          P1OUT &= ~BIT2
#define DESLIGA_BOMBA       P1OUT |= BIT2

#define LIGA_OXIGENADOR     P1OUT &= ~BIT4
#define DESLIGA_OXIGENADOR  P1OUT |= BIT4

//-----------------------------------------------------
// Variáveis Globais
//-----------------------------------------------------
volatile uint8_t bomba_ligada = 0;
volatile uint8_t contador_vazio = 0;
volatile uint8_t contador_cheio = 0;

volatile char buffer_rx[32];
volatile uint8_t idx_rx = 0;

uint8_t ultimo_segundo = 99;

volatile int alimentacao = INTERVALO_ALIMENTADOR;
volatile int oxigenar = INTERVALO_OXIGENADOR;
volatile int tempo_oxigenando = 0;
volatile int tempo_alimentando = 0;
volatile uint8_t oxigenador_ligado = 0;

//-----------------------------------------------------
// Protótipos
//-----------------------------------------------------
void iniciaBoia(void);
void iniciaBomba(void);
void iniciaOxigenador(void);
void iniciaAlimentador(void);
void abreAlimentador(void);
void fechaAlimentador(void);
void configTimerBoia(void);
void atualizaTerminal(void);
void inicializaUARTPC(void);
void uartPrint(char * str);
void uartPrintBCD(uint8_t bcd);
void uartPrintDec3(int num);
void configRTC(void);

//-----------------------------------------------------
// MAIN
//-----------------------------------------------------
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    iniciaBoia();
    iniciaBomba();
    iniciaOxigenador();
    iniciaAlimentador();
    
    configTimerBoia();
    inicializaUARTPC();
    configRTC();

    __enable_interrupt(); 

    uartPrint("\033[2J"); // Limpa a tela na inicialização

    while (1)
    {
        if (TI_second != ultimo_segundo) {
            ultimo_segundo = TI_second;
            atualizaTerminal();
        }
    }
}

//-----------------------------------------------------
// FUNÇÕES DE IMPRESSÃO LEVES (Substituem o sprintf)
//-----------------------------------------------------
void uartPrint(char * str) {
    while (*str != '\0') {
        while (!(UCA1IFG & UCTXIFG)); 
        UCA1TXBUF = *str; 
        str++; 
    }
}

void uartPrintBCD(uint8_t bcd) {
    char d1 = (bcd >> 4) + '0';
    char d2 = (bcd & 0x0F) + '0';
    while (!(UCA1IFG & UCTXIFG)); UCA1TXBUF = d1;
    while (!(UCA1IFG & UCTXIFG)); UCA1TXBUF = d2;
}

void uartPrintDec3(int num) {
    char d1 = (num / 100) + '0';
    char d2 = ((num / 10) % 10) + '0';
    char d3 = (num % 10) + '0';
    while (!(UCA1IFG & UCTXIFG)); UCA1TXBUF = d1;
    while (!(UCA1IFG & UCTXIFG)); UCA1TXBUF = d2;
    while (!(UCA1IFG & UCTXIFG)); UCA1TXBUF = d3;
}

//-----------------------------------------------------
// TERMINAL 
//-----------------------------------------------------
void atualizaTerminal(void){
    get24Hour(); // Apenas para atualizar as variaveis internas, se necessário
    char *AMPM = (TI_PM == 1) ? "PM" : "AM";

    uartPrint("\033[H"); 
    uartPrintBCD(TI_hour); uartPrint(":");
    uartPrintBCD(TI_minute); uartPrint(":");
    uartPrintBCD(TI_second); uartPrint(" ");
    uartPrint(AMPM); uartPrint("\r\n");
    
    if (bomba_ligada == 1) uartPrint("Bomba de Nivel: LIGADA   \r\n");
    else uartPrint("Bomba de Nivel: Desligada\r\n");
    
    if (oxigenador_ligado == 1) {
        uartPrint("Oxigenacao:     LIGADA (Oxigenando...) \r\n");
    } else {
        uartPrint("Oxigenacao em:  ");
        uartPrintDec3(oxigenar);
        uartPrint(" segundos          \r\n");
    }
    
    if (tempo_alimentando > 0) {
        uartPrint("Alimentador:    ABERTO (Alimentando...) \r\n");
    } else {
        uartPrint("Alimentacao em: ");
        uartPrintDec3(alimentacao);
        uartPrint(" segundos               \r\n");
    }
}

//-----------------------------------------------------
// CONFIGURAÇÕES DE HARDWARE E ATUADORES
//-----------------------------------------------------
void iniciaOxigenador(void) {
    P1DIR |= BIT4;         
    DESLIGA_OXIGENADOR;    
    oxigenador_ligado = 0;
}

void iniciaAlimentador(void) {
    // Configura P2.5 para a saída do PWM (TA2.2)
    P2DIR |= BIT5;
    P2SEL |= BIT5;
    
    // Configura o Timer A2 para 50Hz (PWM do Servo)
    TA2CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA2CCR0 = 20971 - 1;      
    TA2CCTL2 = OUTMOD_7; 
    
    fechaAlimentador();  
}

void abreAlimentador(void) {
    // Tenta ir para o limite máximo real (180°)
    TA2CCR2 = 2500; 
}

void fechaAlimentador(void) {
    // Tenta ir para o limite mínimo real (0°)
    TA2CCR2 = 600; 
}

void inicializaUARTPC(void){
    P4SEL |= BIT4 | BIT5; 
    UCA1CTL1 |= UCSWRST;  
    UCA1CTL1 |= UCSSEL_2; 
    UCA1BR0 = 54;                           
    UCA1BR1 = 0;
    UCA1MCTL = UCBRS_5; 
    UCA1CTL1 &= ~UCSWRST; 
    UCA1IE |= UCRXIE;     
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    if (UCA1IFG & UCRXIFG) { 
        buffer_rx[idx_rx] = UCA1RXBUF; 
        idx_rx++;
        if (idx_rx >= 32) idx_rx = 0;
    }
}

//-----------------------------------------------------
// BOIA - 100% POLLING (Imune a ruídos e EMI)
//-----------------------------------------------------
void iniciaBoia(void) {
    P1DIR &= ~BIT3;      
    P1SEL &= ~BIT3;      
    P1REN |= BIT3;       
    P1OUT &= ~BIT3;      
}

void iniciaBomba(void) {
    P1DIR |= BIT2;
    P1SEL &= ~BIT2;
    DESLIGA_BOMBA;
    bomba_ligada = 0;
}

void configTimerBoia(void) {
    // Agora o Timer roda ININTERRUPTAMENTE
    TA1CTL = TASSEL__SMCLK | ID__8 | MC__UP | TACLR;
    TA1CCR0 = 62500 - 1;      
    TA1CCTL0 = CCIE;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void) {
    if (bomba_ligada == 0) {
        if ((P1IN & BIT3) == 0) { // Água Vazia
            contador_vazio++;
            contador_cheio = 0; 
            if (contador_vazio >= TEMPO_ESTABILIDADE) {
                LIGA_BOMBA;
                bomba_ligada = 1;
                contador_vazio = 0;
            }
        } else {
            contador_vazio = 0; // Marola? Zera o contador!
        }
    } else {
        if ((P1IN & BIT3) == BIT3) { // Água Cheia
            contador_cheio++;
            contador_vazio = 0;
            if (contador_cheio >= TEMPO_ESTABILIDADE) {
                DESLIGA_BOMBA;
                bomba_ligada = 0;
                contador_cheio = 0;
            }
        } else {
            contador_cheio = 0; // Marola? Zera o contador!
        }
    }
}

//-----------------------------------------------------
// RTC E GERENCIADOR DE EVENTOS
//-----------------------------------------------------
void sincronizaRTC_PC(void) {
    uint8_t h_dec = (__TIME__[0] - '0') * 10 + (__TIME__[1] - '0');
    uint8_t m_dec = (__TIME__[3] - '0') * 10 + (__TIME__[4] - '0');
    uint8_t s_dec = (__TIME__[6] - '0') * 10 + (__TIME__[7] - '0');
    
    uint8_t is_pm = 0;
    
    if (h_dec >= 12) {
        is_pm = 1; 
        if (h_dec > 12) h_dec -= 12;
    }
    if (h_dec == 0) h_dec = 12; 
    
    uint8_t h_bcd = ((h_dec / 10) << 4) | (h_dec % 10);
    uint8_t m_bcd = ((m_dec / 10) << 4) | (m_dec % 10);
    uint8_t s_bcd = ((s_dec / 10) << 4) | (s_dec % 10);
    
    setTime(h_bcd, m_bcd, s_bcd, is_pm);
}

void configRTC(void){
    sincronizaRTC_PC();         

    TB0CCR0 = 32768 - 1;      
    TB0CTL = TBSSEL_1 | MC__UP; // TBSSEL_1 seleciona o ACLK (~32kHz)             
    TB0CCTL0 |= CCIE; 
}

#pragma vector=TIMER0_B0_VECTOR 
__interrupt void Timer_B0_ISR (void) 
{
    incrementSeconds();
    
    // --- Lógica do Alimentador ---
    if (tempo_alimentando > 0) {
        tempo_alimentando--;
        if (tempo_alimentando == 0) {
            fechaAlimentador(); 
            alimentacao = INTERVALO_ALIMENTADOR;  
        }
    } else {
        if (alimentacao == 0) {
            abreAlimentador();      
            tempo_alimentando = DURACAO_ALIMENTADOR;  
        } else {
            alimentacao--;
        }
    }

    // --- Lógica do Oxigenador ---
    if (tempo_oxigenando > 0) {
        tempo_oxigenando--;
        if (tempo_oxigenando == 0) {
            DESLIGA_OXIGENADOR;
            oxigenador_ligado = 0;
            oxigenar = INTERVALO_OXIGENADOR; 
        }
    } else {
        if (oxigenar == 0) {
            LIGA_OXIGENADOR;
            oxigenador_ligado = 1;
            tempo_oxigenando = DURACAO_OXIGENADOR; 
        } else {
            oxigenar--;
        }
    }
}
