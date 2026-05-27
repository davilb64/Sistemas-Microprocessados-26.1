#include <msp430.h>
#include <stdint.h>

void inicializa_i2c_0_mestre_transmissor();
uint8_t i2cSend(uint8_t addr, uint8_t data);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWrite(char *str);
void lcdInit();


int main()
{
    WDTCTL = WDTPW + WDTHOLD; 

    inicializa_i2c_0_mestre_transmissor();
    
    lcdInit();

    
    
    
    while (1) {
        lcdWrite("Hello World!!");
        __delay_cycles(1000000);
        lcdWrite("Davi Lopes\nBrito");
        __delay_cycles(1000000);
    }

}

void inicializa_i2c_0_mestre_transmissor(){
    // interface resetada
    UCB0CTL1 |= UCSWRST;
    // i2c | mestre | modo sincrono
    UCB0CTL0 |= UCMODE_3 | UCMST | UCSYNC;
    // smclk como clock | transmissora | reset interface
    UCB0CTL1 |= UCSSEL__SMCLK | UCTR | UCSWRST;

    // portas para periferico
    P3SEL |= BIT0 | BIT1;
    P3REN &= ~(BIT0 | BIT1);

    // divisor de clock para atingir ~100 kHz (Modo I2C padrão)
    // 1.04 MHz / 10 = ~104 kHz
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    // tira restet
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE = 0; // sem interrup

}

uint8_t i2cSend(uint8_t addr, uint8_t data) {
    // 1
    UCB0I2CSA = addr;

    // 2
    UCB0CTL1 |= UCTR | UCTXSTT;

    // 3
    while (!(UCB0IFG & UCTXIFG)); // Trava até o buffer estar livre
    UCB0TXBUF = data;             // Joga o dado no buffer

    // 4
    while (UCB0CTL1 & UCTXSTT);

    // 5
    if (!(UCB0IFG & UCNACKIFG)) {
        // espera o byte data ser transmitido (TXIFG=1)
        while (!(UCB0IFG & UCTXIFG));
    }

    // 6
    UCB0CTL1 |= UCTXSTP;

    // 7
    while (UCB0CTL1 & UCTXSTP);

    // 8
    return (UCB0IFG & UCNACKIFG); 
}

void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    uint8_t addr = 0x27;
    
    // bits de controle
    uint8_t backlight = 0x08;
    uint8_t rs;
    if (isChar == 1) {
        rs = 0x01;
    } else {
        rs = 0x00;
    }
    uint8_t rw = 0x00; // escrita
    uint8_t en = 0x04; // liga apenas o bit 2 (0000 0100)

    // zera a parte baixa do byte
    uint8_t data_bits = nibble & 0xF0;

    // passo 1 (en = 0)
    uint8_t passo1 = data_bits | backlight | rs | rw; 
    i2cSend(addr, passo1);

    // passo 2 (en = 1)
    uint8_t passo2 = passo1 | en; 
    i2cSend(addr, passo2);

    // passo 3 é o passo 1 denovo
    i2cSend(addr, passo1);
}

void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    // Envia a metade mais significativa 
    lcdWriteNibble(byte & 0xF0, isChar);
    
    // Envia a metade menos significativa deslocada
    lcdWriteNibble((byte << 4) & 0xF0, isChar);
}

void lcdInit() {
    __delay_cycles(50000); // Delay energia

    // reset para o modo 8-bits
    // envia o nibble 0x3 três vezes (isChar = 0)
    lcdWriteNibble(0x30, 0); 
    __delay_cycles(5000);     // Espera pelo menos 4.1ms
    
    lcdWriteNibble(0x30, 0);
    __delay_cycles(1000);     // Espera pelo menos 100us
    
    lcdWriteNibble(0x30, 0);
    __delay_cycles(1000);

    // entrar no modo 4-bits
    // Envia o nibble 0x2
    lcdWriteNibble(0x20, 0);
    __delay_cycles(1000);

    // agora podemos usar bytes
    
    // Configura a interface: Modo 4 bits, 2 Linhas, Fonte 5x8
    lcdWriteByte(0x28, 0);
    __delay_cycles(1000);

    // Configura o display: Display LIGADO, Cursor DESLIGADO, Blink DESLIGADO
    lcdWriteByte(0x0C, 0);
    __delay_cycles(1000);

    // Limpar o display 
    lcdWriteByte(0x01, 0);
    
    // espera o clear
    __delay_cycles(2000);
}

void lcdWrite(char *str) {
    lcdWriteByte(0x01, 0); //limpa display
    __delay_cycles(2000); 
    uint8_t count = 0; // pos do cursor

    while (*str != '\0') {
        
        if (*str == '\n') {
            lcdWriteByte(0xC0, 0); // comando que pula para linha 2
            count = 16;           
        } 
        else {
            // limite linha 1
            if (count == 16) {
                lcdWriteByte(0xC0, 0); // pula
            } 
            // volta a escrever na linha 1 se estourou tudo
            else if (count == 32) {
                lcdWriteByte(0x80, 0); // inicio linha 1
                count = 0;    
            }
            
            // escreve char
            lcdWriteByte(*str, 1);
            count++;
        }
        
        str++;
    }
}
