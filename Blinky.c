/* Blinky --- blink an LED on the ATSAME51J20A               2024-12-01 */


#include <sam.h>


void oneLedOn(const int led)
{
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA00;
    PORT_REGS->GROUP[1].PORT_OUTCLR = PORT_PB31;
    PORT_REGS->GROUP[1].PORT_OUTCLR = PORT_PB30;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA27;
    PORT_REGS->GROUP[1].PORT_OUTCLR = PORT_PB23;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA25;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA20;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA18;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA15;
    
    switch (led)
    {
        case 0:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA00;
            break;
        case 1:
            PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB31;
            break;
        case 2:
            PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB30;
            break;
        case 3:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA27;
            break;
        case 4:
            PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB23;
            break;
        case 5:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA25;
            break;
        case 6:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA20;
            break;
        case 7:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA18;
            break;
        case 8:
            PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;
            break;
    }
}


void main(void)
{
    volatile int dally;
    int i;
    
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA00;    // LED pins to outputs
    PORT_REGS->GROUP[1].PORT_DIRSET = PORT_PB30;
    PORT_REGS->GROUP[1].PORT_DIRSET = PORT_PB31;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA27;
    PORT_REGS->GROUP[1].PORT_DIRSET = PORT_PB23;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA25;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA20;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA18;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA15;
    
    while (1)
    {
        for (i = 0; i < 9; i++)
        {
            oneLedOn(i);
            
            for (dally = 0; dally < 500000; dally++)
                ;
        }
    }
}

