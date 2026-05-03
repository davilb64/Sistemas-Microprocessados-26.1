#include <msp430.h>
#include <math.h> 

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    
    volatile unsigned int tempoInicio = 0;
    volatile unsigned int tempoFim = 0;
    volatile unsigned int ticksGastos = 0;

    //SMCLK 1MHz
    TA0CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR; 

    tempoInicio = TA0R; 
    
    volatile double hardVar = 128.43984610923f;
    hardVar = (sqrt(hardVar * 3.14159265359) + 30.3245)/1020.2331556 - 0.11923;
    
    tempoFim = TA0R;

    ticksGastos = tempoFim - tempoInicio; //o tempo vem em microsegundos :)

    while(1)
    {
        __no_operation(); 
    }
}