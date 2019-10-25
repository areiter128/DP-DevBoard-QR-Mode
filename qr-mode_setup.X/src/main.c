/*
 * File:   main.c
 * Author: M91406
 *
 * Created on July 8, 2019, 1:52 PM
 */

// Hello Quentin
// Hello Andy

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "init/init_inputcap.h"

volatile uint16_t tgl_cnt = 0;  // local counter of LED toggle loops
#define TGL_INTERVAL    2999    // LED toggle interval of (2999 + 1) x 100usec = 100ms
#define TMR_TIMEOUT     30000   // Timeout protection for Timer1 interrupt flag bit

volatile uint32_t dac_cnt = 0;  // local counter of LED toggle loops
#define DACMOD_COUNT    1999    // DAC increment interval of (1999 + 1) x 100usec = 200ms

volatile bool btn_push = false;

uint16_t InputCap_Counter = 0;
uint16_t InputCapData[1024];
uint32_t InputCap_Value = 0;
uint32_t InputCap_Value1 = 0, InputCap_ValueH1 = 0, InputCap_ValueL1 = 0;
uint32_t InputCap_Value2 = 0, InputCap_ValueH2 = 0, InputCap_ValueL2 = 0;

int main(void) {

    volatile uint16_t timeout = 0;
  
    
    init_fosc();        // Set up system oscillator for 100 MIPS operation
    init_aclk();        // Set up Auxiliary PLL for 500 MHz (source clock to PWM module)
    init_timer1();      // Set up Timer1 as scheduler time base
    init_gpio();        // Initialize common device GPIOs
    
    // Basic setup of common power controller peripheral modules
    init_pwm_module();  // Set up PWM module (basic module configuration)
    init_acmp_module(); // Set up analog comparator/DAC module
    init_adc_module();  // Set up Analog-To-Digital converter module
    init_vin_adc();     // Initialize ADC Channel to measure input voltage
    
    init_inputcap_module();
    
    ext_reference_init();   // initialize external reference input
    
    // Reset Soft-Start Phase to Initialization
    converter.soft_start.phase = SS_INIT;   
    converter.status.flags.auto_start = true;
    
// ===========================================
 // ToDo: FOR DEBUGGING ONLY! REMOVE WHEN DONE
// ===========================================
    // Comparator output = RB11(RP43) = TP41 on DP DevBoard
    __builtin_write_RPCON(0x0000);
    _TRISB11 = 0;
    _LATB11 = 0;
    
//    RPINR3bits.ICM1R = 71;     //RP71, RD7 as the Input capture input IC1
//    RPOR5bits.RP43R = 23;      // Assign COMP1 (=23) output to pin RB11 = RP43  (DSP_GPIO3) 
    RPOR5bits.RP43R = 25;      // Assign COMP3 (=25) output to pin RB11 = RP43  (DSP_GPIO3)
    __builtin_write_RPCON(0x0800);

// ===========================================
    
    // Enable Timer1
    T1CONbits.TON = 1; 
    
//    IEC6bits.ADCAN6IE  = 0;    // Disable ADCAN6 Interrupt, just for Input Capture testing
//    IEC6bits.ADCAN16IE = 0;    // Disable ADCAN16 Interrupt just for Input Capture testing
//    ADIELbits.IE6 = 0; // Common Interrupt Disable:
//    ADIELbits.IE12 = 0; // Common Interrupt Disable
    while (1) {

        // wait for timer1 to overrun
        while ((!_T1IF) && (timeout++ < TMR_TIMEOUT));
        timeout = 0;    // Reset timeout counter
        _T1IF = 0; // reset Timer1 interrupt flag bit
        DBGPIN_1_TOGGLE; // Toggle DEBUG-PIN

        exec_pwr_control();
               
        if (tgl_cnt++ > TGL_INTERVAL) // Count 100 usec loops until LED toggle interval is exceeded
        {
            DBGLED_TOGGLE;
            tgl_cnt = 0;
        } // Toggle LED and reset toggle counter
        Nop();
        
        
        if((!_RC11) && (!btn_push)) {
        // If SW1 on the development board is pressed...
        // Starting at a slope of 100mV/usec (=8), the slew rate is incremented in single steps
        // up to 1V/usec (=80) and then resets to 100mV/usec (=8)

            btn_push = true; 
            DBGLED_RD_SET;
            DBGLED_GN_CLEAR;

            if(SLP1DAT < 120) {
                SLP1DAT += 8;   // Increment slope slew rate by 100mV/usec up to 1.5V
            }
            else {
                SLP1DAT = DAC_SLOPE_RATE;
            }

        }

        if((_RC11) && (btn_push)) { 
            btn_push = false; 
            DBGLED_RD_CLEAR;
            DBGLED_GN_SET;
        }
        
        if(IFS0bits.CCP1IF){
//            if(CCP1STATLbits.ICOV){
//                
//                InputCap_ValueL1 = CCP1BUFL;              
//                InputCap_ValueL2 = CCP1BUFL;
//                IFS0bits.CCP1IF = 0;
//                InputCap_Value = InputCap_ValueL2 - InputCap_ValueL1;
//
//                Nop();
//                Nop();
//                Nop();
//            }

//            if(CCP1STATLbits.ICOV){
                    
                
                IFS0bits.CCP1IF = 0;
                if(CCP1STATLbits.ICBNE)
                {
                    InputCap_Value1 = CCP1BUFL; 
                    InputCap_Value2 = CCP1BUFL; 
                }
                
                                              
                if((InputCap_Value2 > InputCap_Value1)&&(InputCap_Value1 != 0))
                {                   
                    InputCap_Value = InputCap_Value2 - InputCap_Value1;
                    InputCapData[InputCap_Counter] = InputCap_Value;
//                    InputCap_Value1 = InputCap_Value2;
                    InputCap_Counter ++;
                    Nop();
                    Nop();
                    Nop();

                    if(InputCap_Counter == 1024)
                    {
                        InputCap_Counter = 0;
                        Nop();
                        Nop();
                        Nop();
                    }
                
                }
                Nop();
                Nop();
                Nop();
//            }
        }
        

        
    }


    return (0);
}



//void __attribute__((__interrupt__, auto_psv, context)) _CCP1Interrupt(void){
//    
//    
//    IFS0bits.CCP1IF = 0;
//
//    ccpbuffer2 = ccpbuffer1;
//    ccpbuffer1 = CCP1BUFL;
//    ccpbuffer = (ccpbuffer1 - ccpbuffer2);
//    Nop();
//    Nop();
//    Nop();
//            
//   
//    
//}
