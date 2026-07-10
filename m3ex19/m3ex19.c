#include <msp430.h>
#include <stdint.h>

// buffer circular de 32 posicoes
volatile char buffer_rx[32];
volatile uint8_t idx_rx = 0;

void uartPrint(char * str);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; 

    // configura o botao no p1.1
    P1DIR &= ~BIT1; 
    P1REN |= BIT1;  // liga o resistor 
    P1OUT |= BIT1;  // pull up

    // pinos da uart conectados no usb da placa
    P4SEL |= BIT4 | BIT5;

    // configurando a porta serial
    UCA1CTL1 |= UCSWRST;      
    UCA1CTL1 |= UCSSEL_2;     // usa o smclk de 1 mhz

    // baud rate (9600 bps)
    // smclk = 1.048.576 Hz.
    // 1.048.576 / 9600 = 109,2266
    // A parte inteira (109) vai para os registradores BR0 e BR1.
    UCA1BR0 = 104;           
    UCA1BR1 = 0;           
    UCA1MCTL = UCBRS_2;       // ajuste do baudrate

    UCA1CTL1 &= ~UCSWRST;     

    // ativa interrupcao pra receber dados
    UCA1IE |= UCRXIE;

    __enable_interrupt(); 

    while(1) {
        // verifica se o botao foi apertado
        if (!(P1IN & BIT1)) {
            
            uartPrint("Davi e Valquiria\r\n");
            
            // debonce e evita flood
            __delay_cycles(300000); 
        }
    }
    
}

void uartPrint(char * str) {
    // roda ate achar o fim do texto
    while (*str != '\0') {
        // trava aqui ate o buffer ficar vazio e pronto pra enviar
        while (!(UCA1IFG & UCTXIFG)); 
        
        // manda a letra atual
        UCA1TXBUF = *str; 
        
        str++; 
    }
}

// interrupcao que roda quando chega algo do pc
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    // checa a flag direto pra nao dar conflito com o debugger
    if (UCA1IFG & UCRXIFG) { 
        
        // guarda a letra recebida no vetor e limpa a flag
        buffer_rx[idx_rx] = UCA1RXBUF; 
        
        idx_rx++;
        
        // zera o indice se bater o limite de 32
        if (idx_rx >= 32) {
            idx_rx = 0;
        }
    }
}
