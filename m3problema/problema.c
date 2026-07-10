#include <msp430.h>
#include <stdint.h>
#include <stdio.h> 

// comandos do terminal serial
#define TERM_CLR "\033[2J"
#define TERM_HOME "\033[0;0H"

// end. e pinos do lcd
uint8_t lcd_addr = 0x27; 
#define LCD_BL 0x08 // backlight
#define LCD_EN 0x04 // enable
#define LCD_RW 0x02 // read/write
#define LCD_RS 0x01 // register select

// variaveis globais
volatile uint8_t n_canais = 1;         
volatile uint8_t novo_n_canais = 0;    
volatile uint16_t adc_dados[8];        
volatile uint8_t adc_pronto = 0;       

// prototipos das funcoes
void delay_ms(uint16_t ms);
void i2cInit(void);
uint8_t i2cSend(uint8_t addr, uint8_t data);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdInit(void);
void uartInit(void);
void uartSendChar(char c);
void uartSendString(char *str);
void configuraADC_Timer(uint8_t canais);
void converte_hex(uint16_t valor, char *saida); 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    uint8_t i;

    // inicializa perifericos
    i2cInit();
    lcdInit();
    uartInit();
    
    // config a primeira rodada do adc (1 canal)
    configuraADC_Timer(n_canais);
    
    // limpa terminal do pc a primeira vez
    uartSendString(TERM_CLR);

    __enable_interrupt(); // libera interrupcoes globais

    char buffer_texto[32]; // string temporaria pra montar as mensagens
    char valor_hex[4];     // buffer pros 3 digitos hexadecimais

    while(1) {
        
        // se a uart avisou que o usuario quer mudar os canais
        if (novo_n_canais > 0) {
            n_canais = novo_n_canais;
            novo_n_canais = 0;            // consome a flag
            configuraADC_Timer(n_canais); // reconfigura o hardware
            uartSendString(TERM_CLR);     // limpa a tela pra tirar lixo antigo
        }

        // se a interrupcao do adc avisou que leu
        if (adc_pronto) {
            adc_pronto = 0; // consome a flag

            // atualiza terminal do pc
            uartSendString(TERM_HOME); // volta cursor pro inicio
            for (i = 0; i < n_canais; i++) {
                
                // converte leitura bruta num texto hex de 3 digitos
                converte_hex(adc_dados[i], valor_hex);
                
                sprintf(buffer_texto, "%d: %s\r\n", i, valor_hex); 
                uartSendString(buffer_texto); 
            }

            // atualiza o lcd (canal a0)
            lcdWriteByte(0x80, 0); // forca cursor do lcd pra linha 1, col 0
            
            converte_hex(adc_dados[0], valor_hex); // converte o a0
            sprintf(buffer_texto, "A0: %s    ", valor_hex); // espacos no final pra apagar lixo
            
            for (i = 0; buffer_texto[i] != '\0'; i++) {
                lcdWriteByte(buffer_texto[i], 1); // escreve char por char
            }
        }
    }
}

// conversao hexadecimal (corrige bug do sprintf)
void converte_hex(uint16_t valor, char *saida) {
    const char mapa[] = "0123456789ABCDEF"; // dicionario
    
    // 12 bits = 3 digitos hexadecimais (4 bits cada)
    saida[0] = mapa[(valor >> 8) & 0x0F]; // pega os 4 bits mais altos
    saida[1] = mapa[(valor >> 4) & 0x0F]; // pega os 4 bits do meio 
    saida[2] = mapa[valor & 0x0F];        // pega os 4 bits mais baixos
    saida[3] = '\0';                      // finaliza string
}

// config do adc e timer
void configuraADC_Timer(uint8_t canais) {
    uint8_t i;

    TA0CTL &= ~MC_3;            // para o timerA0 pra evitar disparo
    ADC12CTL0 &= ~ADC12ENC;     // desliga o adc

    // calc freq de amostragem de 200/N Hz:
    // smclk = 1.048.576 Hz. freq = 200/N
    // periodo = 1048576 / (200/canais) = 5242 * canais
    uint16_t periodo = 5242 * canais;
    TA0CCR0 = periodo - 1;                   
    
    // calc tempo de carga do capacitor (tcap):
    // tcap = (Rs + Ri) * Ci * ln(2^(N+1)) + 800ns
    // exigencia do roteiro: Rs=10k. dados do msp: Ri=1.8k, Ci=25pF, N=12 bits
    // tcap = 11800 * 25e-12 * ln(8192) + 800ns = ~3.45 us
    // o smclk bate a cada ~0.95us. entao 14 ticks = ~13.3 us
    // usamos 14 ticks pra sobrar margem de seguranca pro hardware
    TA0CCR1 = 14; 
    
    TA0CCTL1 = OUTMOD_3;              // modo set/reset (pulso pro adc)
    TA0CTL = TASSEL_2 | MC_1 | TACLR; // smclk, modo up, limpa

    // config adc 12 bits modo sequencia
    ADC12CTL0 = ADC12ON | ADC12SHT0_2 | ADC12MSC; 
    ADC12CTL1 = ADC12SHP | ADC12SHS_1 | ADC12CONSEQ_1; 
    ADC12CTL2 = ADC12RES_2; // 12 bits

    // limpa canais pra nao dar conflito
    ADC12MCTL0 = 0; ADC12MCTL1 = 0; ADC12MCTL2 = 0; ADC12MCTL3 = 0;
    ADC12MCTL4 = 0; ADC12MCTL5 = 0; ADC12MCTL6 = 0; ADC12MCTL7 = 0;

    // prepara pinos p6 de acordo com n de canais
    P6SEL |= ((1 << canais) - 1); 
    P6DIR &= ~((1 << canais) - 1);

    // mapeia registradores de leitura
    for (i = 0; i < canais; i++) {       
        uint16_t config_canal = i; // le de a0 ate a7
        if (i == canais - 1) {
            config_canal |= ADC12EOS; // bota flag de fim de sequencia no ultimo
        }
        
        switch(i) {
            case 0: ADC12MCTL0 = config_canal; break;
            case 1: ADC12MCTL1 = config_canal; break;
            case 2: ADC12MCTL2 = config_canal; break;
            case 3: ADC12MCTL3 = config_canal; break;
            case 4: ADC12MCTL4 = config_canal; break;
            case 5: ADC12MCTL5 = config_canal; break;
            case 6: ADC12MCTL6 = config_canal; break;
            case 7: ADC12MCTL7 = config_canal; break;
        }
    }

    ADC12IE = (1 << (canais - 1)); // ativa a interrupcao so quando o ultimo canal converter
    ADC12IFG = 0;                  // limpa sujeira
    ADC12CTL0 |= ADC12ENC;         // religa adc
}

// interrupcao adc
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void) {
    uint8_t i;
    // guarda resultados de todos os canais convertidos
    for (i = 0; i < n_canais; i++) {
        switch(i) {
            case 0: adc_dados[0] = ADC12MEM0; break;
            case 1: adc_dados[1] = ADC12MEM1; break;
            case 2: adc_dados[2] = ADC12MEM2; break;
            case 3: adc_dados[3] = ADC12MEM3; break;
            case 4: adc_dados[4] = ADC12MEM4; break;
            case 5: adc_dados[5] = ADC12MEM5; break;
            case 6: adc_dados[6] = ADC12MEM6; break;
            case 7: adc_dados[7] = ADC12MEM7; break;
        }
    }
    
    adc_pronto = 1; // avisa a main
    ADC12IFG = 0;   // limpa flag
    
    // religa o enc (enable conversion), pq a flag eos desliga ele no hardware
    ADC12CTL0 |= ADC12ENC; 
}

// comunicacao pc
void uartInit(void) {
    P4SEL |= BIT4 | BIT5; // rx e tx
    
    UCA1CTL1 |= UCSWRST;  // reseta pra configurar
    UCA1CTL1 |= UCSSEL_2; // smclk 
    
    // calc baudrate 19200:
    // smclk = 1.048.576 Hz
    // divisor = 1048576 / 19200 = 54.61
    // a fracao 0.61 eh compensada no modulador (UCBRS_5)
    UCA1BR0 = 54;                           
    UCA1BR1 = 0;
    UCA1MCTL = UCBRS_5; // modulador
    
    UCA1CTL1 &= ~UCSWRST; // libera
    UCA1IE |= UCRXIE;     // int so pra rx
}

void uartSendChar(char c) {
    while (!(UCA1IFG & UCTXIFG)); // trava ate buffer liberar
    UCA1TXBUF = c;                          
}

void uartSendString(char *str) {
    while (*str) {
        uartSendChar(*str++);
    }
}

// interrupcao de rx da uart
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    if (UCA1IFG & UCRXIFG) {
        char recebido = UCA1RXBUF;
        
        // verifica se digitou entre 1 e 8
        if (recebido >= '1' && recebido <= '8') {
            novo_n_canais = recebido - '0'; // sinaliza pra main trocar canais
        }
    }
}

// delay com timer b0 (nao conflita com timer a0 do adc)
void delay_ms(uint16_t ms) {
    // calc do delay em ms:
    // usando smclk de ~.048MHz, precisamos de 1048 ticks pra fechar 1ms exato
    TB0CTL = TBSSEL_2 | MC_0 | TBCLR; 
    TB0CCR0 = 1048 - 1;               // conta 1048 ticks pra 1 ms
    TB0CCTL0 &= ~CCIFG;               // limpa flag
    TB0CTL |= MC_1;                   // liga modo up

    uint16_t i;
    for (i = 0; i < ms; i++) {
        while (!(TB0CCTL0 & CCIFG));  // trava ate 1ms
        TB0CCTL0 &= ~CCIFG;           
    }
    
    TB0CTL = MC_0;                    // desliga timer
}

// i2c e lcd
void i2cInit(void) {
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 |= UCMODE_3 | UCMST | UCSYNC;      
    UCB0CTL1 |= UCSSEL__SMCLK | UCTR | UCSWRST; 

    P3SEL |= BIT0 | BIT1;                       
    P3REN &= ~(BIT0 | BIT1);

    // divisor do i2c: 1.048MHz / 10 = ~104 kHz
    UCB0BR0 = 10;                               
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;                       
    UCB0IE = 0;                                 
}

uint8_t i2cSend(uint8_t addr, uint8_t data) {
    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;                 

    while (!(UCB0IFG & UCTXIFG));               
    UCB0TXBUF = data;                           

    while (UCB0CTL1 & UCTXSTT);                 

    if (!(UCB0IFG & UCNACKIFG)) {
        while (!(UCB0IFG & UCTXIFG));           
    }

    UCB0CTL1 |= UCTXSTP;                        
    while (UCB0CTL1 & UCTXSTP);                 

    return (UCB0IFG & UCNACKIFG);               
}

void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    uint8_t rs;
    if (isChar == 1) {
        rs = LCD_RS;
    } else {
        rs = 0x00;
    }
    
    uint8_t data_bits = nibble & 0xF0;

    uint8_t passo1 = data_bits | LCD_BL | rs; 
    i2cSend(lcd_addr, passo1);

    uint8_t passo2 = passo1 | LCD_EN; 
    i2cSend(lcd_addr, passo2);

    i2cSend(lcd_addr, passo1);
}

void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    lcdWriteNibble(byte & 0xF0, isChar);          
    lcdWriteNibble((byte << 4) & 0xF0, isChar);   
}

void lcdInit(void) {
    delay_ms(50); 

    lcdWriteNibble(0x30, 0); 
    delay_ms(5);     
    
    lcdWriteNibble(0x30, 0);
    delay_ms(1);     
    
    lcdWriteNibble(0x30, 0);
    delay_ms(1);

    lcdWriteNibble(0x20, 0);
    delay_ms(1);

    lcdWriteByte(0x28, 0);
    delay_ms(1);

    lcdWriteByte(0x0C, 0);
    delay_ms(1);

    lcdWriteByte(0x01, 0);
    delay_ms(2);
}