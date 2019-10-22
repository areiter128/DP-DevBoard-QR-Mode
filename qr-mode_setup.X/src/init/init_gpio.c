/*
 * File:   init_gpio.c
 * Author: M91406
 *
 * Created on July 8, 2019, 6:26 PM
 */


#include <xc.h>
#include "init_gpio.h"

volatile uint16_t init_gpio(void) {
    
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
    ANSELD = 0x0000;
    
    DBGLED_INIT;    // On-PIM Debug LED
    DBGPIN_1_INIT;  // On-PIM Debug Pin
    
    DBGLED_RD_INIT; // DevBoard Red LED 
    DBGLED_GN_INIT; // DevBoard Green LED 
    
    DGBPIN_2_INIT;  // DevBoard Debug Pin TP34
    DGBPIN_3_INIT;  // DevBoard Debug Pin TP36
    
    return(1);
}
