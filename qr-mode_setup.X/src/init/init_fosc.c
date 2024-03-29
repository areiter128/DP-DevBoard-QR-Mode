/*
 * File:   init_fosc.c
 * Author: M91406
 *
 * Created on July 8, 2019, 1:55 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "init_fosc.h"   

#define TIMEOUT_LIMIT   5000    // timeout counter maximum

volatile uint16_t init_fosc(void) {
    
    volatile uint16_t timeout=0;
    
    //Temporarily switch to FRC (without PLL), so we can safely change the PLL settings,
    //in case we had previously been already running from the PLL.
    if(OSCCONbits.COSC != 0b000)
    {
        // NOSC = 0b000 = FRC without divider or PLL
        __builtin_write_OSCCONH(0b000);  // Fast RC Oscillator, no PLL 
        // Clear CLKLOCK and assert OSWEN = 1 to initiate switch-over
        __builtin_write_OSCCONL((OSCCON & 0x7E) | 0x01); 
        //Wait for switch over to complete.
        while((OSCCONbits.COSC != OSCCONbits.NOSC) && (timeout++ < TIMEOUT_LIMIT)); 
        if (timeout >= TIMEOUT_LIMIT){ return(0); }
    }
    
    // Set FRC tuning register to 8.000 MHz (default)
    OSCTUNbits.TUN = 0; 
    
    // Configure PLL prescaler, both PLL postscalers, and PLL feedback divider
    CLKDIVbits.PLLPRE = 1; // N1=1
    PLLFBDbits.PLLFBDIV = 200; // M = 200
    PLLDIVbits.POST1DIV = 4; // N2=4
    PLLDIVbits.POST2DIV = 1; // N3=1
    PLLDIVbits.VCODIV = 0; // VCO Output divider is set to Fvco/4
    
    // Initiate Clock Switch to FRC Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONH(0b001);  // Fast RC Oscillator with PLL 
    if(OSCCONbits.COSC != OSCCONbits.NOSC)
    {
        // Assert OSWEN and make sure CLKLOCK is clear, to initiate the switching operation
        __builtin_write_OSCCONL((OSCCON & 0x7F) | 0x01);    
        // Wait for clock switch to finish
        while((OSCCONbits.COSC != OSCCONbits.NOSC) && (timeout++ < TIMEOUT_LIMIT));  
        if ((OSCCONbits.COSC != OSCCONbits.NOSC) || (timeout >= TIMEOUT_LIMIT))
        { return(0); }
    }

    // Lock registers against accidental changes
    OSCCONbits.CLKLOCK = 1;
    
    while((OSCCONbits.LOCK != 1) && (timeout++ < TIMEOUT_LIMIT)); // Wait n while loops for PLL to Lock
	if ((OSCCONbits.LOCK != 1) || (timeout >= TIMEOUT_LIMIT)) // Error occurred? 
	{ return(0); } // => If so, return error code
   
    // Add enforced delay
    for(timeout=0xffff; timeout>0; timeout--);
    
    // Return Success/Failure
    return((1 - OSCCONbits.CF));					// Return oscillator fail status bit

}

volatile uint16_t init_aclk(void) {

    volatile uint16_t timeout=0;

    // Clear Enable-bit of Auxiliary PLL during configuration
    ACLKCON1bits.APLLEN = 0;

    // Select clock input source (either primary oscillator or internal FRC)
    ACLKCON1bits.FRCSEL = 1;        // FRC is the clock source for APLL
	
    // Configure APLL pre-scaler, APLL post-scaler, APLL divisor
    // APLL frequency limited to 400 MHz due to DAC counter limit
    ACLKCON1bits.APLLPRE   = 1;     // N1 (non zero)
	APLLFBD1bits.APLLFBDIV = 100;   // M  = APLLFBD 
    APLLDIV1bits.APOST1DIV = 2;     // N2 (non zero)
    APLLDIV1bits.APOST2DIV = 1;     // N3 (non zero)

    // Set AVCO divider of Auxiliary PLL 
    APLLDIV1bits.AVCODIV   = 0b11;  // AVCO Scaler = AFVCO

    // Set Enable-bit of Auxiliary PLL 
    ACLKCON1bits.APLLEN = 1;

    // if user has not enabled the APLL module, exit here
    if(!ACLKCON1bits.APLLEN)
    { return(0); }
        
    // Add enforced delay
    for(timeout=0xffff; timeout>0; timeout--);
    
    // Wait 5000 while loops for APLL to Lock
    while((ACLKCON1bits.APLLCK != 1) && (timeout++<TIMEOUT_LIMIT));		
	if ((ACLKCON1bits.APLLCK != 1) || (timeout++ >= TIMEOUT_LIMIT))	// PLL still not locked in? 
	{ return (0); } // => If so, return error code
    else
    { return(ACLKCON1bits.APLLCK); }

        
    return(1);
}