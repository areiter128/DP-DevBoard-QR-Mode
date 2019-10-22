/*
 * File:   external_reference.c
 * Author: M91406
 *
 * Created on July 24, 2019, 12:45 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"
#include "task_external_reference.h"

volatile uint32_t vref_avg=0;   // local buffer variable of the oversampling filter of the external reference voltage signal
volatile uint16_t avg_cnt = 0;  // local buffer variable for the averaging filter counter steps

volatile uint16_t ext_reference_init(void) {

    converter.data.v_ref = 0;   // Reset power converter reference
    init_pot_adc();             // Initialize ADC input and interrupt
 
    return(1);
}

/*! _ADCAN6Interrupt
 * *************************************************************************************************
 * Summary:
 * ADC input interrupt service routine
 * 
 * Description:
 * Here the external reference voltage signal is sampled. The conversion of this ADC input is 
 * triggered by the PWM module. 
 * The reference voltage signal is oversampled 256 times before updating the controller reference
 * in the 'sepic' data structure. This reference is set to set the output voltage of the SEPIC.
 * 
 * *************************************************************************************************/

void __attribute__((__interrupt__, auto_psv, context)) _ADCAN6Interrupt(void)
{
    volatile uint16_t samp=0;   // local buffer variable for the most recent ADC result
    volatile uint32_t res=0;    // local buffer variable for the scaling range

DGBPIN_3_SET;
    
    samp = ADCBUF6; // read latest sample
    samp <<= 3;     // normalize to Q15
    res = (volatile uint32_t)samp * V_REF_DIFF; // Scale adjustable range by using Q15 number
    res >>= 15;     // normalize back into 16-bit
    
    vref_avg += (V_REF_MIN + (volatile uint16_t)res);   // Add most recent value to averaging buffer
    
    if(!(++avg_cnt & 0x00FF)) {     // After 256 samples, calculate average value
        converter.data.v_ref = (vref_avg >> 8);  // Copy averaged value into reference value
        vref_avg = 0;                       // Reset averaging buffer
    }
    
    _ADCAN6IF = 0;  // Clear the ADCANx interrupt flag 
    
DGBPIN_3_CLEAR;


}

