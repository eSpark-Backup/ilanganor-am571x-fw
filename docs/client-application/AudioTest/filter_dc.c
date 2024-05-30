/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : filter dc - filter 0hz signal
///	@file:	filter_dc.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	20/Aug/2013	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/


#include <std.h>
#include <stdint.h>
#include <c6x.h>
#include <csl_gpio.h>
#include <csl.h>
#include <math.h>

#include "main.h"
#include "AudioCfg.h"
#include "filter_dc.h"

#define F_GAIN 21000//23000 //20000 	//0.64



static int32_t prev_x[HD_MAX_CHANNELS][2];
static int32_t prev_y[HD_MAX_CHANNELS][2]; 
static int32_t g_acc[HD_MAX_CHANNELS][2];
static int32_t coef;
static int32_t pole;
static int32_t acc;

						
/**************************************************************************/                     
//  Module		: filter_dc_init
//  Description :  clear filter delay line
/// @return none
/// @param	none
/**************************************************************************/
void filter_dc_init(uint32_t ch, uint32_t dir)
{
	int i;
	double pole_ = 0.95;//0.98;//0.95;
	prev_x[ch][dir] = 0;
	prev_y[ch][dir] = 0;
	g_acc[ch][dir] = 0;
	acc=0;

	coef=(int32_t)(32768.0*(1.0 - pole_));
	pole=(int32_t)(32768.0* pole_);
	//acc=0;
}
#if 0
/**************************************************************************/                     
//  Module		: filter_dc
//  Description :  Let:
//       x[n] = input to DC blocking filter (and to differentiator)
//       d[n] = output of differntiator and input to LPF
//       y[n] = output of LPF and of DC blocking filter
//       e[n] = quantization error of LPF (or leaky integrator)
//       d[n] = x[n] - x[n-1]   (no quantization error)
//       y[n] = Quantize{ pole*y[n-1] + d[n] - e[n-1] }
//       e[n] = y[n] - (pole*y[n-1] + d[n] - e[n-1])
// or
//       y[n] = (pole*y[n-1] + d[n] - e[n-1])            + e[n]
// or
//       y[n] = (pole*y[n-1] + x[n] - x[n-1] - e[n-1])   + e[n]
// or
//       y[n] = (y[n-1] - (1-pole)*y[n-1] + x[n] - x[n-1] - e[n-1]) + e[n]
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	dir- HD_DAC or HD_ADC
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_dc (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{
	unsigned int n;
	
	for (n=0; n<80; n++)
	{
		//acc=acc-x[n-1]+x[n]	
	    acc   -= prev_x[ch][dir];	
	    prev_x[ch][dir] = (int32_t)in[n]<<15;
	    acc   += prev_x[ch][dir];
	    //acc=acc-(1-pole)*y[n-1]
	    acc   -= coef*prev_y[ch][dir];
	    prev_y[ch][dir] = acc>>15;               // quantization happens here
	    out[n]   = (short)prev_y[ch][dir];
	    // acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
	}


}

/**************************************************************************/
//  Module		: filter_dc
//  Description :  implement one pole one zero filter
///	y[n]=x[n]-x[n-1]+pole*y[n-1]
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	dir- HD_DAC or HD_ADC
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_dc (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{
	unsigned int n;

	for (n=0; n<80; n++)
	{
		//acc=x[n]-x[n-1]
		acc=((int32_t)in[n]<<15)-prev_x[ch][dir];
	    prev_x[ch][dir] = (int32_t)in[n]<<15;
	    //acc=acc+pole*y[n-1]
	    acc   += pole*prev_y[ch][dir];
	    prev_y[ch][dir] = acc>>15;               // quantization happens here
	    out[n]   = (int16_t)prev_y[ch][dir];
	    // acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
	}


}
#endif
#if 0
/**************************************************************************/                     
//  Module		: filter_dc
//  Description :  implement one pole one zero filter
///	y[n]=x[n]-x[n-1]+pole*y[n-1]
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	dir- HD_DAC or HD_ADC
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_dc (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{
	unsigned int n;
	int32_t input;
	
	for (n=0; n<80; n++)
	{
		input =in[n]*F_GAIN;
		//acc=x[n]-x[n-1]
		acc=input-prev_x[ch][dir];
	    prev_x[ch][dir] = input;
	    //acc=acc+pole*y[n-1]
	    acc   += pole*prev_y[ch][dir];
	    prev_y[ch][dir] = acc>>15;               // quantization happens here
	    out[n]   = (int16_t)(prev_y[ch][dir]);
	    // acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
	}


}
#else
/**************************************************************************/
//  Module		: filter_dc
//  Description :  Let:
//       x[n] = input to DC blocking filter (and to differentiator)
//       d[n] = output of differntiator and input to LPF
//       y[n] = output of LPF and of DC blocking filter
//       e[n] = quantization error of LPF (or leaky integrator)
//       d[n] = x[n] - x[n-1]   (no quantization error)
//       y[n] = Quantize{ pole*y[n-1] + d[n] - e[n-1] }
//       e[n] = y[n] - (pole*y[n-1] + d[n] - e[n-1])
// or
//       y[n] = (pole*y[n-1] + d[n] - e[n-1])            + e[n]
// or
//       y[n] = (pole*y[n-1] + x[n] - x[n-1] - e[n-1])   + e[n]
// or
//       y[n] = (y[n-1] - (1-pole)*y[n-1] + x[n] - x[n-1] - e[n-1]) + e[n]
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	dir- HD_DAC or HD_ADC
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_dc (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{
	unsigned int n;

	for (n=0; n<80; n++)
	{
		//acc=acc-x[n-1]+x[n]
	    g_acc[ch][dir]   -= prev_x[ch][dir];
	    prev_x[ch][dir] = (int32_t)in[n]<<15;
	    g_acc[ch][dir]   += prev_x[ch][dir];
	    //acc=acc-(1-pole)*y[n-1]
	    g_acc[ch][dir]   -= coef*prev_y[ch][dir];
	    prev_y[ch][dir] = g_acc[ch][dir]>>15;               // quantization happens here
	    //clamping protection
	    if (prev_y[ch][dir]>32767)
	    	prev_y[ch][dir]=32767;
	    if (prev_y[ch][dir]<-32767)
	    	prev_y[ch][dir]=-32767;
	    out[n]   = (short)prev_y[ch][dir];
	    // acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
	}


}

#endif
