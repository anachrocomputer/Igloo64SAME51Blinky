/* Blinky --- blink an LED on the ATSAME51J20A               2024-12-01 */


#include <sam.h>
#include <stdint.h>
#include <stdbool.h>


volatile uint32_t Milliseconds = 0L;
volatile bool Tick = false;


void TC3_Handler(void)
{
    Milliseconds++;
    Tick = true;
    
    //PORT_REGS->GROUP[1].PORT_OUTTGL = PORT_PB06;    // 500Hz on PB06
    //PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA12;    // 500Hz on PA12

    // Acknowledge interrupt
    TC3_REGS->COUNT16.TC_INTFLAG |= TC_INTFLAG_MC0(1);
}


/* millis --- return milliseconds since reset */

uint32_t millis(void)
{
    return (Milliseconds);
}


/* initMillisecondTimer --- set up a timer to interrupt every millisecond */

static void initMillisecondTimer(void)
{
    // Connect GCLK1 to TC3
    GCLK_REGS->GCLK_PCHCTRL[26] = GCLK_PCHCTRL_GEN(0x1U) | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[26] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
        ;
    
    // Main Clock setup to supply clock to TC3
    MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_TC3(1);
    
    // Set up TC3 for regular 1ms interrupt
    TC3_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV64;
    TC3_REGS->COUNT16.TC_COUNT = 0;
    TC3_REGS->COUNT16.TC_WAVE = TC_WAVE_WAVEGEN_MFRQ;
    TC3_REGS->COUNT16.TC_CC[0] = ((48000000 / 64) / 1000) - 1;
    TC3_REGS->COUNT16.TC_INTENSET = TC_INTENSET_MC0(1);
    
    /* Set TC3 Interrupt Priority to Level 3 */
    NVIC_SetPriority(TC3_IRQn, 3);
    
    /* Enable TC3 NVIC Interrupt Line */
    NVIC_EnableIRQ(TC3_IRQn);
    
    TC3_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE(1);
    
    // Wait until TC3 is enabled
    //while(TC3->COUNT16.STATUS.bit.SYNCBUSY == 1)
    //    ;
}


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
    uint32_t t;
    int i;
    
    initMCU();
    initMillisecondTimer();
    
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
            
            t = millis();
            while((millis() - t) < 200)
                ;
        }
        
        for (i = 8; i > 0; i--)
        {
            oneLedOn(i);
            
            t = millis();
            while((millis() - t) < 200)
                ;
        }
    }
}

