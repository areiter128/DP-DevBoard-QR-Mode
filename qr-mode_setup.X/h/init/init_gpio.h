/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   init_gpio.h
 * Author: M91406
 * Comments: header file of the GPIO initialization
 * Revision history: 
 * 1.0  initial version
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INITIALIZE_GPIO_H
#define	INITIALIZE_GPIO_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"

#ifdef __DPDB_MA330048__

    // Red LED on CK_DP_PIM
    #define DBGLED_SET          { _LATD15 = 1; }
    #define DBGLED_CLEAR        { _LATD15 = 0; }
    #define DBGLED_TOGGLE       { _LATD15 ^= 1; }
    #define DBGLED_INIT         { _LATD15 = 0; _TRISD15 = 0; }

    // TP4 on CK_DP_PIM
    #define DBGPIN_1_SET        { _LATD12 = 1; }
    #define DBGPIN_1_CLEAR      { _LATD12 = 0; }
    #define DBGPIN_1_TOGGLE     { _LATD12 ^= 1; }
    #define DBGPIN_1_INIT       { _LATD12 = 0; _TRISD12 = 0; }

    // TP52 on Digital Power Development Board
    #define DBGLED_RD_SET       { _LATD8 = 1; }
    #define DBGLED_RD_CLEAR     { _LATD8 = 0; }
    #define DBGLED_RD_TOGGLE	{ _LATD8 ^= 1; }
    #define DBGLED_RD_INIT      { _LATD8 = 0; _TRISD8 = 0; }

    // TP54 on Digital Power Development Board
    #define DBGLED_GN_SET       { _LATD13 = 1; }
    #define DBGLED_GN_CLEAR     { _LATD13 = 0; }
    #define DBGLED_GN_TOGGLE	{ _LATD13 ^= 1; }
    #define DBGLED_GN_INIT      { _LATD13 = 0; _TRISD13 = 0; }

    // TP34 on Digital Power Development Board
    #define DGBPIN_2_SET        { _LATD5 = 1; }
    #define DGBPIN_2_CLEAR     { _LATD5 = 0; }
    #define DGBPIN_2_TOGGLE	   { _LATD5 ^= 1; }
    #define DGBPIN_2_INIT      { _LATD5 = 0; _TRISD5 = 0; }

    // TP36 on Digital Power Development Board
    #define DGBPIN_3_SET        { _LATD6 = 1; }
    #define DGBPIN_3_CLEAR     { _LATD6 = 0; }
    #define DGBPIN_3_TOGGLE	   { _LATD6 ^= 1; }
    #define DGBPIN_3_INIT      { _LATD6 = 0; _TRISD6 = 0; }

#endif


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

extern volatile uint16_t init_gpio(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* INITIALIZE_GPIO_H */
