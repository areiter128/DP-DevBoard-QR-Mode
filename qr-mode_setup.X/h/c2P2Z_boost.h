/* ***************************************************************************************
 * z-Domain Control Loop Designer Version 0.9.0.61.
 * ***************************************************************************************
 * 2p2z compensation filter coefficients derived for following operating conditions:
 * ***************************************************************************************
 *
 * 	Controller Type:	2P2Z - Basic Current Mode Compensator
 * 	Sampling Frequency:	250000 Hz 
 * 	Fixed Point Format:	15
 * 	Scaling Mode:		3 - Dual Bit-Shift Scaling
 * 	Input Gain:			0.125
 * 
 * ***************************************************************************************/

#ifndef __SPECIAL_FUNCTION_LAYER_C2P2Z_BOOST_H__
#define __SPECIAL_FUNCTION_LAYER_C2P2Z_BOOST_H__

#include <xc.h>
#include <dsp.h>
#include <stdint.h>

#include "npnz16b.h"

/* ***************************************************************************************
 * Data Arrays:
 * The cNPNZ_t data structure contains a pointer to derived coefficients in X-space and
 * other pointers to controller and error history in Y-space.
 * This header file holds public declarations for variables and arrays defined in 
 * c2p2z_boost.c
 * 
 * Type definition for A- and B- coefficient arrays and error- and control-history arrays, 
 * which are aligned in memory for optimized addressing during DSP computations.           
 * These data structures need to be placed in specific memory locations to allow direct    
 * X/Y-access from the DSP. (coefficients in x-space, histories in y-space)                
 * ***************************************************************************************/

	typedef struct
	{
		volatile fractional ACoefficients[2]; // A-Coefficients
		volatile fractional BCoefficients[3]; // B-Coefficients
	} __attribute__((packed))C2P2Z_BOOST_CONTROL_LOOP_COEFFICIENTS_t;

	typedef struct
	{
		volatile fractional ControlHistory[2];  // Control History
		volatile fractional ErrorHistory[3];    // Error History
	} __attribute__((packed))C2P2Z_BOOST_CONTROL_LOOP_HISTORIES_t;


	extern volatile cNPNZ16b_t c2P2Z_boost; // user-controller data object

/* ***************************************************************************************/

// Function call prototypes for initialization routines and control loops

extern inline uint16_t c2P2Z_boost_Init(void); // Loads default coefficients into 2P2Z controller and resets histories to zero

extern inline void c2P2Z_boost_Reset( // Resets the 2P2Z controller histories
	volatile cNPNZ16b_t* controller // Pointer to nPnZ data structure
	);

extern inline void c2P2Z_boost_Precharge( // Pre-charges histories of the 2P2Z with defined steady-state data
	volatile cNPNZ16b_t* controller, // Pointer to nPnZ data structure
	volatile uint16_t ctrl_input, // user-defined, constant error history value
	volatile uint16_t ctrl_output // user-defined, constant control output history value
	);

extern inline void c2P2Z_boost_Update( // Calls the 2P2Z controller
	volatile cNPNZ16b_t* controller // Pointer to nPnZ data structure
	);

#endif	// end of __SPECIAL_FUNCTION_LAYER_C2P2Z_BOOST_H__ header file section
