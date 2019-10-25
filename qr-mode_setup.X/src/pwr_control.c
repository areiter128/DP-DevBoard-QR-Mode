/*
 * File:   pwr_control.c
 * Author: M91406
 *
 * Created on July 9, 2019, 1:10 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"

volatile POWER_CONTROLLER_t converter;

volatile uint16_t init_pwr_control(void) {
    
    init_trig_pwm();   // Set up auxiliary PWM for power converter
    init_pwm();        // Set up power converter PWM
    init_acmp();       // Set up power converter peak current comparator/DAC
    init_adc();        // Set up power converter ADC (voltage feedback only)
    init_pot_adc();    // Set up ADC for sampling reference provided by external voltage divider        
    
    converter.soft_start.counter = 0;                             // Reset Soft-Start Counter
    converter.soft_start.pwr_on_delay = POWER_ON_DELAY;     // Soft-Start Power-On Delay = 500 ms
    converter.soft_start.ramp_period = RAMP_PERIOD;         // Soft-Start Ramp Period = 50 ms
    converter.soft_start.pwr_good_delay = POWER_GOOD_DELAY; // Soft-Start Power Good Delay = 200 ms
    converter.soft_start.reference = V_OUT_REF;             // Soft-Start Target Reference = 12V
    converter.soft_start.ramp_ref_increment = REF_STEP;     // Soft-Start Single Step Increment of Reference
    
    c2p2z_Init();
    
    c2p2z.ADCTriggerOffset = VOUT_ADCTRIG;
    c2p2z.ptrADCTriggerRegister = &REG_VOUT_ADCTRIG;
    c2p2z.InputOffset = VOUT_FEEDBACK_OFFSET;
    c2p2z.ptrControlReference = &converter.data.v_ref;
    c2p2z.ptrSource = &REG_VOUT_ADCBUF;
    c2p2z.ptrTarget = &DAC_VREF_REGISTER;
    c2p2z.MaxOutput = DAC_MAX;
    c2p2z.MinOutput = DAC_MIN;
    c2p2z.status.bits.enable = 0;
    
    converter.data.v_ref    = 0; // Reset power reference value (will be set via external potentiometer)
    
    return(1);
}

volatile uint16_t launch_pwr_control(void) {

    // Run enable-sequence of all peripherals used by this power controller
    launch_adc();         // Start ADC Module
    launch_acmp();        // Start analog comparator/DAC module
    launch_pwm();         // Start PWM
    
    c2p2z_Reset(&c2p2z);    // Reset control loop histories
    
    return(1);
}

volatile uint16_t exec_pwr_control(void) {
        
    switch (converter.soft_start.phase) {
        
        /*!SS_INIT
         * When power converter is in initialization mode, the basic 
         * peripherals needed for the operation of the power converter
         * will be set up. This step is only executed once. */
        case SS_INIT: // basic PWM, ADC, CMP, DAC configuration
            
            init_pwr_control();    // Initialize all peripherals and data structures of the controller
            
            converter.status.flags.op_status = STAT_OFF; // Set power status to OFF
            converter.soft_start.phase = SS_LAUNCH_PER;

        break;

        /*!SS_LAUNCH_PER
         * Once all required peripherals have been initialized, the peripheral blocks and
         * control related software modules are enabled in a dedicated sequence. The PWM 
         * outputs and control loop execution, however, are still disabled and the power
         * supply will not start up yet. This step is only executed once and will complete by 
         * switching into STANDBY  */
        case SS_LAUNCH_PER: // Enabling PWM, ADC, CMP, DAC 
            
            launch_pwr_control(); 
            
            converter.status.flags.op_status = STAT_OFF; // Set power status to OFF
            converter.soft_start.phase = SS_STANDBY;
            
        break;
        
        /*!SS_STANDBY
         * This state is entered once all required peripherals have been launched and the 
         * power supply is waiting to be powered up. This is also the standard fall-back 
         * state after a fault/restart condition.
         * To get the power supply to start, all faults status bits need to be cleared,
         * the ADC has to run and produce data, the power controller has to be enabled 
         * and the status bit "converter.status.flags.GO" has to be set.
         * 
         * Please note:
         * The data structure converter.status.flags also offers a setting called auto_start.
         * When this bit is set, the 'enable' and 'GO' bits are set automatically and only
         * the 'adc_active' and 'fault_active' bits are checked.
        */
        case SS_STANDBY: // Enabling PWM, ADC, CMP, DAC 

            converter.status.flags.op_status = STAT_STANDBY;  // Set converter status to STANDBY
            
            // Force PWM output and controller to OFF state
            PG1IOCONLbits.OVRENH = 1;           // Disable PWMxH output
            c2p2z.status.bits.enable = 0; // Disable the control loop
            converter.status.flags.pwm_active = false;   // Clear PWM_ACTIVE flag bit

            // wait for fault to be cleared, adc to run and the GO bit to be set
            if( (converter.status.flags.enabled == 1) && 
                (converter.status.flags.adc_active) &&
                (!converter.status.flags.fault_active) && 
                (converter.status.flags.GO) )
            {
                converter.soft_start.counter = 0;                   // Reset soft-start counter
                converter.soft_start.phase = SS_PWR_ON_DELAY; // Switch to Power On Delay mode
            }
            break;

        /*!SS_PWR_ON_DELAY
         * In this step the soft-start procedure is counting up call intervals until
         * the defined power-on delay period has expired. PWM and control loop are disabled.
         * At the end of this phase, the state automatically switches to RAMP_UP mode */     
        case SS_PWR_ON_DELAY:  

            converter.status.flags.op_status = STAT_START; // Set converter status to START-UP
            
            if(converter.soft_start.counter++ > converter.soft_start.pwr_on_delay)
            {
                converter.soft_start.reference = 0;  // Reset soft-start reference to minimum
                c2p2z.ptrControlReference = &converter.soft_start.reference; // Hijack controller reference

                converter.soft_start.counter = 0;                   // Reset soft-start counter
                converter.soft_start.phase   = SS_RAMP_UP;    // Switch to ramp-up mode
            }
            break;    
                 
        /*!SS_RAMP_UP
         * During ramp up, the PWM and control loop are forced ON while the control reference is 
         * incremented. Once the 'private' reference of the soft-start data structure equals the
         * reference level set in converter.data.v_ref, the ramp-up period ends and the state machine 
         * automatically switches to POWER GOOD DELAY mode */     
        case SS_RAMP_UP: // Increasing reference by 4 every scheduler cycle
            
            converter.status.flags.op_status = STAT_START; // Set converter status to START-UP

            // Force PWM output and controller to be active 
            PG1IOCONLbits.OVRENH = 0;           // User override disabled for PWMxH Pin =< PWM signal output starts
            c2p2z.status.bits.enable = 1; // Start the control loop 

            converter.soft_start.reference += 4;  // increment reference
            
            // check if ramp is complete
            if (converter.soft_start.reference >= converter.data.v_ref)
            {
                converter.soft_start.counter = 0;                       // Reset soft-start counter
                converter.soft_start.phase   = SS_PWR_GOOD_DELAY; // switch to Power Good Delay mode
            }
            break; 
            
        /*!SS_PWR_GOOD_DELAY
         * POWER GOOD DELAY is just like POWER ON DELAY a state in which the soft-start counter
         * is counting call intervals until the user defined period has expired. Then the state 
         * machine automatically switches to COMPLETE mode */     
        case SS_PWR_GOOD_DELAY:
            
            converter.status.flags.op_status = STAT_START; // Set converter status to START-UP
            
            if(converter.soft_start.counter++ > converter.soft_start.pwr_good_delay)
            {
                converter.soft_start.counter = 0;                 // Reset soft-start counter
                converter.soft_start.phase   = SS_COMPLETE; // switch to SOFT-START COMPLETE mode
            }
            break;
                
        /*!SS_COMPLETE
         * The COMPLETE phase is the default state of the power controller. Once entered, only a FAULT
         * condition or external modifications of the soft-start phase can trigger a change of state. */     
        case SS_COMPLETE: // Soft start is complete, system is running, output voltage reference is taken from external potentiometer
            
            converter.status.flags.op_status = STAT_ON; // Set converter status to ON mode
            c2p2z.ptrControlReference = &converter.data.v_ref; // hand reference control back
            break;

        /*!SS_FAULT or undefined state
         * If any controller state is set, different from the previous ones (e.g. FAULT),
         * the power controller sets the FAULT flag bit, enforces detection of the ADC activity 
         * by clearing the adc_active bit and switches the state machine into STANDBY, from
         * which the power controller may recover as soon as all startup conditions are met again. */
        default: // If something is going wrong, reset PWR controller to STANDBY

            converter.status.flags.op_status = STAT_FAULT; // Set converter status to FAULT mode
            converter.status.flags.fault_active = true;    // Set FAULT flag bit
            converter.status.flags.adc_active = false;     // Clear ADC_READY flag bit

            converter.soft_start.phase = SS_STANDBY;
            break;
            
    }
        
    /*!Power Converter Auto-Start Function
     * When the control bit converter.status.flags.auto_start is set, the status bits 'enabled' 
     * and 'GO' are automatically set and continuously enforced to ensure the power supply
     * will enter RAMP UP from STANDBY without the need for user code intervention. */
    // 
    if (converter.status.flags.auto_start == true) {
        converter.status.flags.enabled = true;  // Auto-Enable power converter
        converter.status.flags.GO = true;       // Auto-Kick-off power converter
    }
    else { 
        converter.status.flags.GO = false; // Always Auto-Clear GO bit
    }
        
    return(1);
}

/*!Power Converter Auto-Start Function
 * **************************************************************************************************
 * 
 * **************************************************************************************************/

void __attribute__((__interrupt__, auto_psv, context))_VOUT_ADCInterrupt(void)
{
    DGBPIN_2_SET;
    
    converter.status.flags.adc_active = true;
    converter.data.v_in = REG_VIN_ADCBUF;
    converter.data.v_out = REG_VOUT_ADCBUF;

    // here the control loop would be called
    //    c2p2z_Update(&c2p2z); // Control loop has been disabled
//    DAC1DATH = converter.data.v_ref;  // Copy averaged value into reference value
    DAC3DATH = converter.data.v_ref;
    _ADCAN16IF = 0;  // Clear the ADCANx interrupt flag 

    DGBPIN_2_CLEAR;
    
}

