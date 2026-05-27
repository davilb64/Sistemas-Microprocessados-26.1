#include <msp430.h>
#include <stdint.h>

void inicializa_i2c_0_mestre_transmissor();
uint8_t i2cSend(uint8_t addr, uint8_t data);
void i2cScan(uint8_t *addrs);
void i2cScan_otimizado(uint8_t *addrs);

int main()
{
    WDTCTL = WDTPW + WDTHOLD; 

    inicializa_i2c_0_mestre_transmissor();
    uint8_t dispositivos_encontrados[10];
    //i2cScan(dispositivos_encontrados);
    i2cScan_otimizado(dispositivos_encontrados);
    
    
    while (1) {
        _no_operation();
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

void i2cScan(uint8_t *addrs) {
    uint8_t count = 0;
    uint8_t addr;
    
    for (addr = 0x00; addr <= 0x7F; addr++) {
        
        uint8_t status = i2cSend(addr, 0x00);
        
        if (status == 0) {
            addrs[count] = addr; 
            count++;          
        }
    }
    
    // marca o fim do vetor com 0xFF
    addrs[count] = 0xFF; 
}

void i2cScan_otimizado(uint8_t *addrs) { //start stop simultaneos
    uint8_t count = 0;
    uint8_t addr;

    for (addr = 0x00; addr <= 0x7F; addr++) {
        UCB0I2CSA = addr; 

        // start stop juntos
        UCB0CTL1 |= UCTR | UCTXSTT | UCTXSTP;

        // Espera o pulso de STOP terminar
        while (UCB0CTL1 & UCTXSTP);

        if (!(UCB0IFG & UCNACKIFG)) {
            addrs[count] = addr;
            count++;
        } else {
            UCB0IFG &= ~UCNACKIFG; 
        }
    }
    
    addrs[count] = 0xFF; // marcador
}