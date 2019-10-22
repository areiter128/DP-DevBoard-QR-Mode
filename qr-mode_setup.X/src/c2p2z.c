/* ***************************************************************************************
 * z-Domain Control Loop Designer Version 0.9.0.61.
 * ***************************************************************************************
 * 2p2z compensation filter coefficients derived for following operating conditions:
 * ***************************************************************************************
 *
 * 	Controller Type:	2P2Z - Basic Current Mode Compensator
 * 	Sampling Frequency:	350000 Hz 
 * 	Fixed Point Format:	15
 * 	Scaling Mode:		2 - Single Bit-Shift with Output Factor Scaling
 * 	Input Gain:			0.148
 * 
 * ***************************************************************************************/

#include "c2p2z.h"

/* ***************************************************************************************
 * Data Arrays:
 * The cNPNZ_t data structure contains a pointer to derived coefficients in X-space and
 * other pointers to controller and error history in Y-space.
 * This source file declares the default parameters of the z-domain compensation filter.
 * These declarations are made publicly accessible through defines in c2p2z.h
 * ***************************************************************************************/

	volatile C2P2Z_CONTROL_LOOP_COEFFICIENTS_t __attribute__((space(xmemory), near)) c2p2z_coefficients; // A/B-Coefficients 
	volatile uint16_t c2p2z_ACoefficients_size = (sizeof(c2p2z_coefficients.ACoefficients)/sizeof(c2p2z_coefficients.ACoefficients[0])); // A-coefficient array size
	volatile uint16_t c2p2z_BCoefficients_size = (sizeof(c2p2z_coefficients.BCoefficients)/sizeof(c2p2z_coefficients.BCoefficients[0])); // B-coefficient array size

	volatile C2P2Z_CONTROL_LOOP_HISTORIES_t __attribute__((space(ymemory), far)) c2p2z_histories; // Control/Error Histories 
	volatile uint16_t c2p2z_ControlHistory_size = (sizeof(c2p2z_histories.ControlHistory)/sizeof(c2p2z_histories.ControlHistory[0])); // Control history array size
	volatile uint16_t c2p2z_ErrorHistory_size = (sizeof(c2p2z_histories.ErrorHistory)/sizeof(c2p2z_histories.ErrorHistory[0])); // Error history array size

/* ***************************************************************************************
 * 	Pole&Zero Placement:
 * ***************************************************************************************
 *
 * 	fP0:	300 Hz 
 * 	fP1:	60000 Hz 
 * 	fZ1:	300 Hz 
 *
 * ***************************************************************************************
 * 	Filter Coefficients and Parameters:
 * ***************************************************************************************/

	volatile fractional c2p2z_ACoefficients [2] = 
	{
		0x4629,	// Coefficient A1 will be multiplied with controller output u(n-1)
		0xEFD1	// Coefficient A2 will be multiplied with controller output u(n-2)
	};

	volatile fractional c2p2z_BCoefficients [3] = 
	{
		0x7FFF,	// Coefficient B0 will be multiplied with error input e(n)
		0x00B0,	// Coefficient B1 will be multiplied with error input e(n-1)
		0x80B1	// Coefficient B2 will be multiplied with error input e(n-2)
	};


	volatile int16_t c2p2z_pre_scaler = 3;
	volatile int16_t c2p2z_post_shift_A = -2;
	volatile int16_t c2p2z_post_shift_B = 0;
	volatile fractional c2p2z_post_scaler = 0x4BE5;

	volatile cNPNZ16b_t c2p2z; // user-controller data object

/* ***************************************************************************************/

uint16_t c2p2z_Init(void)
{
	volatile uint16_t i = 0;

	// Initialize controller data structure at runtime with pre-defined default values
	c2p2z.status.value = CONTROLLER_STATUS_CLEAR;  // clear all status flag bits (will turn off execution))

	c2p2z.ptrACoefficients = &c2p2z_coefficients.ACoefficients[0]; // initialize pointer to A-coefficients array
	c2p2z.ptrBCoefficients = &c2p2z_coefficients.BCoefficients[0]; // initialize pointer to B-coefficients array
	c2p2z.ptrControlHistory = &c2p2z_histories.ControlHistory[0]; // initialize pointer to control history array
	c2p2z.ptrErrorHistory = &c2p2z_histories.ErrorHistory[0]; // initialize pointer to error history array
	c2p2z.normPostShiftA = c2p2z_post_shift_A; // initialize A-coefficients/single bit-shift scaler
	c2p2z.normPostShiftB = c2p2z_post_shift_B; // initialize B-coefficients/dual/post scale factor bit-shift scaler
	c2p2z.normPostScaler = c2p2z_post_scaler; // initialize control output value normalization scaling factor
	c2p2z.normPreShift = c2p2z_pre_scaler; // initialize A-coefficients/single bit-shift scaler

	c2p2z.ACoefficientsArraySize = c2p2z_ACoefficients_size; // initialize A-coefficients array size
	c2p2z.BCoefficientsArraySize = c2p2z_BCoefficients_size; // initialize A-coefficients array size
	c2p2z.ControlHistoryArraySize = c2p2z_ControlHistory_size; // initialize control history array size
	c2p2z.ErrorHistoryArraySize = c2p2z_ErrorHistory_size; // initialize error history array size


	// Load default set of A-coefficients from user RAM into X-Space controller A-array
	for(i=0; i<c2p2z.ACoefficientsArraySize; i++)
	{
		c2p2z_coefficients.ACoefficients[i] = c2p2z_ACoefficients[i];
	}

	// Load default set of B-coefficients from user RAM into X-Space controller B-array
	for(i=0; i<c2p2z.BCoefficientsArraySize; i++)
	{
		c2p2z_coefficients.BCoefficients[i] = c2p2z_BCoefficients[i];
	}

	// Clear error and control histories of the 2P2Z controller
	c2p2z_Reset(&c2p2z);

	return(1);
}


