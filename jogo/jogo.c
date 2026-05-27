#include <msp430.h>
#include <stdint.h>
#include <string.h>

void inicializa_i2c_0_mestre_transmissor();
uint8_t i2cSend(uint8_t addr, uint8_t data);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWrite(char *str);
void lcdInit();
void lcdMarquee(char *str);
void lcdCreateChar(uint8_t slot, uint8_t *charmap);

int main()
{
    uint8_t char_coracao[8] = {
        0b00000,
        0b01010,
        0b11111, 
        0b11111, 
        0b01110, 
        0b00100, 
        0b00000, 
        0b00000 
    };
    
    WDTCTL = WDTPW + WDTHOLD; 

    inicializa_i2c_0_mestre_transmissor();
    lcdInit();
    
    // carrega coracao no slot 0
    lcdCreateChar(0, char_coracao);

    // pinos P1.2 (S2/Cura) e P1.3 (S1/Dano)
    P1DIR &= ~(BIT2 | BIT3); 
    P1REN |= (BIT2 | BIT3);  
    P1OUT |= (BIT2 | BIT3);  

    int vida = 3;       
    int vida_ant = -1;  
    int i;

    while (1) {
        // S1 (Dano)
        if (!(P1IN & BIT3)) { 
            __delay_cycles(200000); 
            if (vida > 0) {
                vida--;
            }
        }

        // S2 (Cura/Restart)
        if (!(P1IN & BIT2)) {
            __delay_cycles(200000); 
            
            if (vida == 0) {
                vida = 3; 
            } else if (vida < 5) {
                vida++;   
            }
        }

        // atualiza tela
        if (vida != vida_ant) {
            
            lcdWriteByte(0x01, 0); 
            __delay_cycles(2000);  
            
            if (vida == 0) {
                lcdWriteByte(0x80, 0); 
                lcdWrite("   GAME OVER!   ");
                lcdWriteByte(0xC0, 0); 
                lcdWrite(" B p/ reviver  ");
            } 
            else {
                lcdWriteByte(0x80, 0); 
                lcdWrite("Vida: ");
                
                for(i = 0; i < vida; i++){
                    lcdWriteByte(0, 1); 
                }
                
                lcdWriteByte(0xC0, 0); 
                lcdWrite("A:Dano B:Cura");
            }
            
            vida_ant = vida; 
        }
    }
}

void inicializa_i2c_0_mestre_transmissor(){
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 |= UCMODE_3 | UCMST | UCSYNC;
    UCB0CTL1 |= UCSSEL__SMCLK | UCTR | UCSWRST;

    P3SEL |= BIT0 | BIT1;
    P3REN &= ~(BIT0 | BIT1); 

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
    uint8_t addr = 0x27;
    uint8_t backlight = 0x08;
    uint8_t rs;
    
    if (isChar == 1) {
        rs = 0x01;
    } else {
        rs = 0x00;
    }
    
    uint8_t rw = 0x00; 
    uint8_t en = 0x04; 
    uint8_t data_bits = nibble & 0xF0;

    // en = 0
    uint8_t passo1 = data_bits | backlight | rs | rw; 
    i2cSend(addr, passo1);

    // en = 1
    uint8_t passo2 = passo1 | en; 
    i2cSend(addr, passo2);

    // en = 0
    i2cSend(addr, passo1);
}

void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    lcdWriteNibble(byte & 0xF0, isChar);
    lcdWriteNibble((byte << 4) & 0xF0, isChar);
}

void lcdInit() {
    __delay_cycles(50000); 

    lcdWriteNibble(0x30, 0); 
    __delay_cycles(5000);     
    
    lcdWriteNibble(0x30, 0);
    __delay_cycles(1000);     
    
    lcdWriteNibble(0x30, 0);
    __delay_cycles(1000);

    lcdWriteNibble(0x20, 0);
    __delay_cycles(1000);

    lcdWriteByte(0x28, 0);
    __delay_cycles(1000);

    lcdWriteByte(0x0C, 0);
    __delay_cycles(1000);

    lcdWriteByte(0x01, 0);
    __delay_cycles(2000);
}

void lcdWrite(char *str) {
    uint8_t count = 0; 

    while (*str != '\0') {
        if (*str == '\n') {
            lcdWriteByte(0xC0, 0); 
            count = 16;           
        } 
        else {
            if (count == 16) {
                lcdWriteByte(0xC0, 0); 
            } 
            else if (count == 32) {
                lcdWriteByte(0x80, 0); 
                count = 0;    
            }
            
            lcdWriteByte(*str, 1);
            count++;
        }
        str++;
    }
}

void lcdMarquee(char *str) {
    int len = strlen(str); 
    int offset;
    int i;

    if (len <= 16) {
        lcdWriteByte(0x80, 0);
        lcdWrite(str);
        return;
    }

    for (offset = 0; offset <= (len - 16); offset++) {
        lcdWriteByte(0x80, 0); 

        for (i = 0; i < 16; i++) {
            lcdWriteByte(str[offset + i], 1);
        }

        __delay_cycles(300000); 
    }
}

void lcdCreateChar(uint8_t slot, uint8_t *charmap) {
    int i;
    slot &= 0x07; 
    
    lcdWriteByte(0x40 | (slot << 3), 0);

    for (i = 0; i < 8; i++) {
        lcdWriteByte(charmap[i], 1);
    }

    lcdWriteByte(0x80, 0); 
}