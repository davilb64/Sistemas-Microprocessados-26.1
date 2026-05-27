#include <msp430.h> 

// =========================================================================
// CONFIGURAÇÃO DO ENDEREÇO DO LCD
// Módulos I2C (PCF8574) geralmente usam 0x27 ou 0x3F.
// Se o LED Vermelho da placa acender constante, altere esse valor para 0x3F.
// =========================================================================
#define LCD_I2C_ADDRESS     0x27

// Definições dos LEDs de Debug da LaunchPad (F5529)
#define LED_RED_ON        (P1OUT |= BIT0)
#define LED_RED_OFF       (P1OUT &= ~BIT0)
#define LED_GREEN_ON      (P4OUT |= BIT7)
#define LED_GREEN_OFF     (P4OUT &= ~BIT7)

// Protótipos das funções
void initialize_I2C_UCB0_MasterTransmitter(void);
void master_TransmitOneByte(unsigned char address, unsigned char data);

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Para o Watchdog Timer

    // Configuração do LED VERMELHO (P1.0)
    P1SEL &= ~BIT0;
    P1DIR |= BIT0;
    LED_RED_OFF;

    // Configuração do LED VERDE (P4.7)
    P4SEL &= ~BIT7;
    P4DIR |= BIT7;
    LED_GREEN_OFF;

    // Inicializa o módulo I2C
    initialize_I2C_UCB0_MasterTransmitter();

    while(1)
    {
        // Transmite 0xFF: Ativa todos os pinos do expansor (Liga o Backlight do LCD)
        master_TransmitOneByte(LCD_I2C_ADDRESS, 0xFF);
        
        // Atraso de 500ms utilizando os ciclos de clock internos (DCO padrão de ~1.04 MHz)
        __delay_cycles(500000);

        // Transmite 0x00: Desativa todos os pinos do expansor (Desliga o Backlight do LCD)
        master_TransmitOneByte(LCD_I2C_ADDRESS, 0x00);
        
        __delay_cycles(500000);
    }

    return 0;
}

/*
 * Inicialização do barramento I2C no módulo USCI_B0
 * P3.0 - SDA
 * P3.1 - SCL
 */
void initialize_I2C_UCB0_MasterTransmitter(void)
{
    // Coloca o módulo em estado de Reset para poder configurar
    UCB0CTL1 |= UCSWRST;

    // Configura os pinos P3.0 e P3.1 para a função nativa do módulo I2C
    P3SEL |= BIT0 | BIT1;     
    
    // Garante que os resistores internos estão desligados (usando os seus da protoboard)
    P3REN &= ~(BIT0 | BIT1);

    // Configura: Modo Mestre (UCMST), Modo I2C (UCMODE_3) e Síncrono (UCSYNC)
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    // Fonte de Clock: SMCLK (Sub-Main Clock, roda a aproximadamente 1.04 MHz por padrão)
    // Transmissor ativo (UCTR) e mantém o módulo em Reset
    UCB0CTL1 = UCSSEL__SMCLK | UCTR | UCSWRST;

    // Divisor de clock para atingir ~100 kHz (Modo I2C padrão)
    // 1.04 MHz / 10 = ~104 kHz
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    // Retira o módulo do Reset para iniciar a operação
    UCB0CTL1 &= ~UCSWRST;

    // Garante interrupções desligadas (trabalhamos checando as flags manualmente)
    UCB0IE = 0;
}

/*
 * Transmite um único byte para o endereço fornecido
 */
void master_TransmitOneByte(unsigned char address, unsigned char data)
{
    // Define o endereço do periférico escravo
    UCB0I2CSA = address;

    // Gera a condição de START na linha
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Aguarda o buffer de transmissão ficar pronto para aceitar o dado
    while (!(UCB0IFG & UCTXIFG))
    {
        // Se o escravo responder com NACK (endereço errado ou desligado), saímos para não travar
        if (UCB0IFG & UCNACKIFG)
        {
            UCB0CTL1 |= UCTXSTP;             // Envia STOP para liberar a linha
            while (UCB0CTL1 & UCTXSTP);      // Espera o STOP concluir
            UCB0IFG &= ~UCNACKIFG;           // Limpa o erro de NACK
            LED_RED_ON;                      // Debug visual de ERRO
            LED_GREEN_OFF;
            return;                          // Cancela o envio
        }
    }

    // Coloca o dado a ser transmitido no registrador de saída
    UCB0TXBUF = data;

    // Aguarda o dado ser totalmente enviado/descarregado pela linha física
    while (!(UCB0IFG & UCTXIFG))
    {
        if (UCB0IFG & UCNACKIFG)
        {
            UCB0CTL1 |= UCTXSTP;
            while (UCB0CTL1 & UCTXSTP);
            UCB0IFG &= ~UCNACKIFG;
            LED_RED_ON;
            LED_GREEN_OFF;
            return;
        }
    }

    // Envia a condição de STOP para encerrar o frame I2C com sucesso
    UCB0CTL1 |= UCTXSTP;

    // Aguarda o hardware liberar fisicamente o barramento antes de sair da função
    while (UCB0CTL1 & UCTXSTP);

    // Se passou por tudo isso, recebemos um ACK do LCD!
    LED_GREEN_ON; 
    LED_RED_OFF;
}