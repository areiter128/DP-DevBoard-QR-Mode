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
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _SOFT_START_H_
#define	_SOFT_START_H_

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>

#include "../common/syscfg_scaling.h"
#include "../common/syscfg_limits.h"

/* Controller Settings */
#define SOFT_START_V_REF_STEP_SIZE  7 // (uint16_t)((float)VOUT_FB_REF_ADC / (float)RAMP_UP_PERIOD_TICKS)     // Step-Granulatiry of soft-start ramp
//#define SOFT_START_I_REF_STEP_SIZE  1 // (uint16_t)((float)VOUT_FB_REF_ADC / (float)RAMP_UP_PERIOD_TICKS)     // Step-Granulatiry of soft-start ramp
#define SOFT_START_INIT_REFERENCE   100   // Minimum voltage feedback refrence
#define SOFT_START_POWER_ON_DELAY   5000  // Delay in [Number of System Ticks] until the output voltage is ramped up
#define SOFT_START_POWER_GOOD_DELAY 100    // Delay in [Number of System Ticks] until the POWER GOOD flag is set
#define SOFT_START_PRECHARGE_TICKS  10    // Number of bootstrap precharge pulses before ramp up
#define SOFT_START_PCRG_PULSE_DUTY  asm volatile ( "REPEAT #6 \n NOP \n" ) // pulse width ~100ns

/* Soft Start Sequence Definitions */
#define SOFT_START_STEP_INITIALIZE     0   // soft-start phase #0: initialize variables and hijack controller reference
#define SOFT_START_STEP_POWER_ON_DELAY 1   // soft-start phase #1: power on delay (no action)
#define SOFT_START_STEP_PRECHARGE      2   // soft-start phase #2: precharge bootstrap capacitor to ensure proper function of half-bridge switch node
#define SOFT_START_STEP_LAUNCH_V_RAMP  3   // soft-start phase #3: turn on PWM outputs and enable controller
#define SOFT_START_STEP_V_RAMP_UP      4   // soft-start phase #4: perform output voltage ramp up based on parameters and system response 
//#define SOFT_START_STEP_I_RAMP_UP      5   // soft-start phase #5: perform output current ramp up based on parameters and system response (average current mode only)
#define SOFT_START_WAIT_FOR_PWRGOOD    6   // soft-start phase #6: Output reached regulation point but waits until things have settled
#define SOFT_START_STEP_COMPLETE       7   // soft-start phase #7: Output in regulation and power is OK


extern void ExecuteVbatASoftStart(void);


#endif	/* _SOFT_START_H_ */

