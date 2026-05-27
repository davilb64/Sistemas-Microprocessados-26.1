#include <msp430.h>
#include <stdint.h>

void inicializa_i2c_0_mestre_transmissor();
uint8_t i2cSend(uint8_t addr, uint8_t data);
uint8_t i2cSend_nBytes(uint8_t addr, uint8_t *data, uint8_t nBytes);

int main()
{
    WDTCTL = WDTPW + WDTHOLD; 

    inicializa_i2c_0_mestre_transmissor();
    uint8_t liga_backlight = 0x08;
    uint8_t desliga_backlight = 0x00;
    
    
    while (1) {

        i2cSend_nBytes(0x27, &liga_backlight, 1);
        __delay_cycles(1000000);
        i2cSend_nBytes(0x27, &desliga_backlight, 1);
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
        // a. Espere o byte data ser transmitido (TXIFG=1)
        while (!(UCB0IFG & UCTXIFG));
    }

    // 6
    UCB0CTL1 |= UCTXSTP;

    // 7
    while (UCB0CTL1 & UCTXSTP);

    // 8
    return (UCB0IFG & UCNACKIFG); 
}

uint8_t i2cSend_nBytes(uint8_t addr, uint8_t *data, uint8_t nBytes) {
    uint8_t status = 0;
    uint8_t i; // Variável declarada fora do loop
    
    for (i = 0; i < nBytes; i++) {
        status = i2cSend(addr, data[i]);
        
        // Se a função retornar erro por causa de um NACK, aborta o loop
        if (status != 0) {
            break; 
        }
    }
    
    return status;
}
