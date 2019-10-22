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
 * File:   pwr_control.h
 * Author: M91406
 * Comments: power controller functions for buck converter
 * Revision history: 
 * 1.0  initial version
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INITIALIZE_POWER_CONTROL_H
#define	INITIALIZE_POWER_CONTROL_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>

#include "c2p2z.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

// ==============================================================================================
// Power converter operation status bits data structure and defines
// ==============================================================================================

 typedef enum {
    STAT_OFF     = 0b000,  // Converter Status Off: Everything is inactive incl. peripherals
    STAT_STANDBY = 0b001,  // Converter Status Standby: Peripherals are running but controller and PWM outputs are off
    STAT_START   = 0b010,  // Converter Status Startup: Converter is executing its startup procedure
    STAT_ON      = 0b011,  // Converter Status Active and Running
    STAT_FAULT   = 0b100   // Converter Status FAULT: Power supply has been shut down waiting for restart attempt
}CONVERTER_OP_STATUS_e;

typedef struct {
    volatile CONVERTER_OP_STATUS_e op_status :3;  // Bit <0:2> operation status
    volatile unsigned : 7;                              // Bit <3:9> (reserved)
    volatile bool pwm_active  :1;                       // Bit 10: Status bit indicating that the PWM outputs have been enabled
    volatile bool adc_active  :1;                       // Bit 11: Status bit indicating that the ADC has been started and is sampling data
    volatile bool fault_active  :1;                     // Bit 12: Status bit indicating that a critical fault condition has been detected
    volatile bool GO :1;                                // Bit 13: POWER SUPPLY START bit (will trigger startup procedure when set)
    volatile bool auto_start :1;                        // Bit 14: Auto-Start will automatically enable the converter and set the GO bit when ready
    volatile bool enabled :1;                           // Bit 15: Enable-bit (when disabled, power supply will reset in STANDBY mode)
}__attribute__((packed))CONVERTER_STATUS_FLAGS_t;

typedef union {
	volatile uint16_t value;                 // buffer for 16-bit word read/write operations
	volatile CONVERTER_STATUS_FLAGS_t flags; // data structure for single bit addressing operations
} CONVERTER_STATUS_t;                  // Power converter operation status bits

// ==============================================================================================
// Power converter soft-start settings data structure and defines
// ==============================================================================================
typedef enum {
    SS_INIT            = 0,  // Soft-Start Phase Initialization
    SS_LAUNCH_PER      = 1,  // Soft-Start Phase Peripheral Launch
    SS_STANDBY         = 2,  // Soft-Start Phase Standby (wait for GO command)
    SS_PWR_ON_DELAY    = 3,  // Soft-Start Phase Power On Delay
    SS_RAMP_UP         = 4,  // Soft-Start Phase Output Ramp Up 
    SS_PWR_GOOD_DELAY  = 5,  // Soft-Start Phase Power Good Delay
    SS_COMPLETE        = 6   // Soft-Start Phase Complete
}SOFT_START_STATUS_e;

typedef struct {
    volatile uint16_t reference;            // Soft-Start target reference value
    volatile uint16_t pwr_on_delay;         // Soft-Start POwer On Delay
    volatile uint16_t precharge_delay;      // Soft-Start Bootstrap Capacitor pre-charge delay
    volatile uint16_t ramp_period;          // Soft-Start Ramp-Up Duration
    volatile uint16_t ramp_ref_increment;   // Soft-Start Single Reference Increment per Step
    volatile uint16_t pwr_good_delay;       // Soft-Start Power Good Delay
    volatile uint16_t counter;              // Soft-Start Execution Counter
    volatile uint16_t phase;                // Soft-Start Phase Index
}SOFT_START_t;                              // Power converter soft-start settings and variables

// ==============================================================================================
// Power converter soft-start settings data structure and defines
// ==============================================================================================

typedef struct {
    volatile uint16_t i_out;    // Power converter output current
    volatile uint16_t v_in;     // Power converter input voltage
    volatile uint16_t v_out;    // Power converter output voltage
    volatile uint16_t v_ref;    // Power converter reference voltage
}CONVERTER_DATA_t;              // Power converter runtime data

typedef struct {
    volatile CONVERTER_STATUS_t status; // Power converter operation status bits
    volatile SOFT_START_t soft_start;   // Power converter soft-start settings and variables
    volatile CONVERTER_DATA_t data;     // Power converter runtime data
}POWER_CONTROLLER_t;                    // Power converter control & monitoring data structure


// ==============================================================================================
// Power converter public function prototypes
// ==============================================================================================

extern volatile uint16_t init_pwr_control(void);
extern volatile uint16_t launch_pwr_control(void);
extern volatile uint16_t exec_pwr_control(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* INITIALIZE_POWER_CONTROL_H */

