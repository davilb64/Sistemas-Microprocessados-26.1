#include <msp430.h>
#include <stdint.h>

uint8_t lcd_addr = 0x27; // end. lcd

// pinos de controle da pcf 
#define LCD_BL 0x08 // backlight - 3
#define LCD_EN 0x04 // enable - 2
#define LCD_RW 0x02 // read/write - 1
#define LCD_RS 0x01 // register select - 0

// funcoes
void delay_ms(uint16_t ms);
void i2cInit(void);
uint8_t i2cSend(uint8_t addr, uint8_t data);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWrite(char *str);
void lcdInit(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    i2cInit();
    lcdInit();
    
    while (1) {
        lcdWrite("Hello World!!");
        delay_ms(1000); 
        
        lcdWrite("Davi\nValquiria");
        delay_ms(1000); 
    }
}

// função de atraso recomendada pelo professor Peixoto (Delay Cylcles é irregular)
void delay_ms(uint16_t ms) {
    TA0CTL = TASSEL_2 | MC_0 | TACLR; // SMCLK, timer parado, limpa o timer
    TA0CCR0 = 1048 - 1;               // conta 1048 ticks pra dar 1 ms
    TA0CCTL0 &= ~CCIFG;               // limpa a flag de interrupção do canal 0
    TA0CTL |= MC_1;                   // liga o Timer no modo up

    uint16_t  i;
    
    for (i = 0; i < ms; i++) {
        while (!(TA0CCTL0 & CCIFG));  // fica travado até o Timer contar 1ms
        TA0CCTL0 &= ~CCIFG;           // limpa a flag para o próximo
    }
    
    TA0CTL = MC_0;                    // para o Timer
}

// I²C
void i2cInit(void) {
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 |= UCMODE_3 | UCMST | UCSYNC;      // I²C Mestre
    UCB0CTL1 |= UCSSEL__SMCLK | UCTR | UCSWRST; // SMCLK, transmissor

    P3SEL |= BIT0 | BIT1;                       // pinos P3.0 e P3.1 para I2C
    P3REN |= BIT0 | BIT1;
    P3OUT &= ~(BIT0 | BIT1);

    UCB0BR0 = 10;                               // divisor para ~100kHz
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;                       // libera interface
    UCB0IE = 0;                                 // interrupções desativadas
}

uint8_t i2cSend(uint8_t addr, uint8_t data) {
    uint16_t timeout;
    
    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;                 

    // 1. Aguarda o buffer TX estar livre 
    timeout = 10000;
    while (!(UCB0IFG & UCTXIFG) && --timeout);  
    if (timeout == 0) return 1; 
                 
    UCB0TXBUF = data;                           

    // 2. Aguarda o START terminar
    timeout = 10000;
    while ((UCB0CTL1 & UCTXSTT) && --timeout);                 
    if (timeout == 0) return 1;

    // 3. Verifica NACK e manda STOP
    if (UCB0IFG & UCNACKIFG) {                  
        UCB0CTL1 |= UCTXSTP;                    
        timeout = 10000;
        while ((UCB0CTL1 & UCTXSTP) && --timeout); // Timeout adicionado!
        return 1;                               
    }

    // 4. Aguarda o dado terminar
    timeout = 10000;
    while (!(UCB0IFG & UCTXIFG) && --timeout);               
    if (timeout == 0) return 1;

    // 5. Finaliza enviando STOP
    UCB0CTL1 |= UCTXSTP;                        
    timeout = 10000;
    while ((UCB0CTL1 & UCTXSTP) && --timeout);     // Timeout adicionado!

    return 0;                                   
}

// Display
void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    // configura o pino rs dependendo se é caractere ou instrução
    uint8_t rs;
    if (isChar == 1) {
        rs = LCD_RS;
    } else {
        rs = 0x00;
    }
    
    // zera a parte baixa do byte
    uint8_t data_bits = nibble & 0xF0;

    // passo 1: prepara os dados, backlight ativo, rs configurado e en = 0
    uint8_t passo1 = data_bits | LCD_BL | rs; 
    i2cSend(lcd_addr, passo1);

    // passo 2: pulso de enable (en = 1)
    uint8_t passo2 = passo1 | LCD_EN; 
    i2cSend(lcd_addr, passo2);

    // passo 3: finaliza o pulso voltando pro en = 0
    i2cSend(lcd_addr, passo1);
}

void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    lcdWriteNibble(byte & 0xF0, isChar);          // envia nibble alto
    lcdWriteNibble((byte << 4) & 0xF0, isChar);   // envia nibble baixo deslocado
}

void lcdInit(void) {
    delay_ms(50); // tempo de estabilização da alimentação

    // força o reset para o estado de 8-bits
    lcdWriteNibble(0x30, 0); 
    delay_ms(5);     
    
    lcdWriteNibble(0x30, 0);
    delay_ms(1);     
    
    lcdWriteNibble(0x30, 0);
    delay_ms(1);

    // entra  no modo 4 bits
    lcdWriteNibble(0x20, 0);
    delay_ms(1);

    // configura interface: modo 4 bits, 2 linhas, Fonte 5x8
    lcdWriteByte(0x28, 0);
    delay_ms(1);

    // configura display: ligado, cursor desligado, blink desligadp
    lcdWriteByte(0x0C, 0);
    delay_ms(1);

    // limpa o display 
    lcdWriteByte(0x01, 0);
    delay_ms(2);
}

void lcdWrite(char *str) {
    lcdWriteByte(0x01, 0); // limpa display
    delay_ms(2);           
    
    uint8_t count = 0;     // posição do cursor

    while (*str != '\0') {
        if (*str == '\n') {
            lcdWriteByte(0xC0, 0); // pula para a linha 2
            count = 16;           
        } 
        else {
            if (count == 16) {
                lcdWriteByte(0xC0, 0); // estourou linha 1, pula para a 2
            } 
            else if (count == 32) {
                lcdWriteByte(0x80, 0); // estourou linha 2, volta para a 1
                count = 0;    
            }
            
            lcdWriteByte(*str, 1);
            count++;
        }
        str++;
    }
}