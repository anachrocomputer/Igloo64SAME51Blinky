/* Blinky --- blink an LED on the ATSAME51J20A               2024-12-01 */


#include <sam.h>


/* initMCU --- initialise the microcontroller in general */

static void initMCU(void)
{
    // Set up wait states
    NVMCTRL_REGS->NVMCTRL_CTRLA = NVMCTRL_CTRLA_RWS(0) | NVMCTRL_CTRLA_AUTOWS_Msk;
    
    /* GCLK2 initialisation: divide DFLL48 by 48 to get 1MHz */
    GCLK_REGS->GCLK_GENCTRL[2] = GCLK_GENCTRL_DIV(48) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK2) == GCLK_SYNCBUSY_GENCTRL_GCLK2)
    {
        /* Wait for the Generator 2 synchronisation */
    }
    
    /* Connect GCLK to GCLK2 (1MHz) */
    GCLK_REGS->GCLK_PCHCTRL[1] = GCLK_PCHCTRL_GEN(0x2)  | GCLK_PCHCTRL_CHEN_Msk;
    while ((GCLK_REGS->GCLK_PCHCTRL[1] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronisation */
    }
    
    /* Configure DPLL    */
    // FILTER = 0
    // LTIME = 0    No time-out
    // REFCLK = 0   GCLK (1MHz)
    // DIV = 0
    OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_FILTER(0) | OSCCTRL_DPLLCTRLB_LTIME(0x0)| OSCCTRL_DPLLCTRLB_REFCLK(0);

    // LDRFRAC = 0
    // LDR = 119    Divide-by-120
    OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDRFRAC(0) | OSCCTRL_DPLLRATIO_LDR(119);

    while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk) == OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk)
    {
        /* Wait for synchronisation */
    }
    
    /* Enable DPLL */
    OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_ENABLE_Msk;

    while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk) == OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk)
    {
        /* Wait for DPLL enable synchronisation */
    }

    while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSTATUS & (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk)) !=
                (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk))
    {
        /* Wait for Ready state */
    }
    
    /* Initialise CPU clock */
    // CPUDIV = 1           Divide-by-1
    MCLK_REGS->MCLK_CPUDIV = MCLK_CPUDIV_DIV(0x01);

    while((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY_Msk) != MCLK_INTFLAG_CKRDY_Msk)
    {
        /* Wait for Main Clock to be Ready */
    }
    
    /* Initialise GCLK0 */
    // GENCTRL0.DIV = 1     Divide-by-1
    // GENCTRL0.DIVSEL = 0  Divide-by-GENTRL0.DIV
    // GENCTRL0.SRC = 1     DPLL0 (120MHz)
    // GENCTRL0.GENEN = 1   Enabled
    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK0) == GCLK_SYNCBUSY_GENCTRL_GCLK0)
    {
        /* Wait for Generator 0 synchronisation */
    }
    
    /* GCLK1 initialisation DFLL (48MHz) divide-by-1 */
    // GENCTRL1.DIV = 1     Divide-by-1
    // GENCTRL1.DIVSEL = 0  Divide-by-GENTRL1.DIV
    // GENCTRL1.SRC = 1     DFLL
    // GENCTRL1.GENEN = 1   Enabled
    GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK1) == GCLK_SYNCBUSY_GENCTRL_GCLK1)
    {
        /* Wait for Generator 1 synchronisation */
    }
}


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
    
    initMCU();
    
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
        for (i = 0; i < 8; i++)
        {
            oneLedOn(i);
            
            for (dally = 0; dally < 500000; dally++)
                ;
        }
        
        for (i = 8; i > 0; i--)
        {
            oneLedOn(i);
            
            for (dally = 0; dally < 500000; dally++)
                ;
        }
    }
}

