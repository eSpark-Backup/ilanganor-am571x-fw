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
#include "filter_navy_headset.h"

#define NV_FILTER_SIZE 11

//static int16_t x[HD_MAX_CHANNELS][6];
//static int16_t x_old[HD_MAX_CHANNELS][6];
static int16_t x[HD_MAX_CHANNELS][NV_FILTER_SIZE-1];
static int16_t x_old[HD_MAX_CHANNELS][NV_FILTER_SIZE-1];

//static int32_t coef[7]={3439,7321,-3439,30310,-3439,7321,3439};
//to adjast gain we multiply by 0.8488
//static int32_t coef[7]={2919,6214,-2919,25727,-2919,6214,2919};

//static int32_t coef[NV_FILTER_SIZE]={-2330,-817,2353,2515,-999,29491,-999,2515,2353,-817,-2330};

//static int32_t coef[NV_FILTER_SIZE]={-4989,-729,3323,3590,-1047,28666,-1047,3590,3323,-729,-4989};
//ADJUST GAIN *0.77
//static int32_t coef[NV_FILTER_SIZE]={-3841,-561,2558,2764,-806,22072,-806,2764,2558,-561,-3841};
//ADJUST GAIN *0.81
static int32_t coef[NV_FILTER_SIZE]={-4041,-590,2691,2907,-848,23219,-848,2907,2691,-590,-4041};


static int32_t acc;

/**************************************************************************/                     
//  Module		: filter_dc_init
//  Description :  clear filter delay line
/// @return none
/// @param	none
/**************************************************************************/
void filter_navy_headset_init(uint32_t ch ,uint32_t dir)
{

	int i;
	for (i=0;i<6;i++)
	{
		x[ch][i] = 0;
		x_old[ch][i] = 0;

	}

}
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
void filter_navy_headset (uint32_t ch, int16_t* in, int16_t* out)
{

	unsigned int n;
	//save last 6 samples for the next block
	memcpy (&x_old[ch][0],&in[80-6],6*sizeof(int16_t));

	for (n=79; n>5; n--)
	{
	    
	    //acc=b0*x[n]+b0*x[n-1]+b0*x[n-2]+b0*x[n-3]+b0*x[n-4]+b0*x[n-5]+b0*x[n-6]
	    acc=coef[0]*in[n]+coef[1]*in[n-1]+coef[2]*in[n-2]+coef[3]*in[n-3]+coef[4]*in[n-4]+coef[5]*in[n-5]+coef[6]*in[n-6];

	    out[n]   = (int16_t)(acc>>15);
	    
	}





	acc=coef[0]*in[5]+coef[1]*in[4]+coef[2]*in[3]+coef[3]*in[2]+coef[4]*in[1]+coef[5]*in[0]+coef[6]*x[ch][5];
	out[5]=(int16_t)(acc>>15);
	acc=coef[0]*in[4]+coef[1]*in[3]+coef[2]*in[2]+coef[3]*in[1]+coef[4]*in[0]+coef[5]*x[ch][5]+coef[6]*x[ch][4];
	out[4]=(int16_t)(acc>>15);
	acc=coef[0]*in[3]+coef[1]*in[2]+coef[2]*in[1]+coef[3]*in[0]+coef[4]*x[ch][5]+coef[5]*x[ch][4]+coef[6]*x[ch][3];
	out[3]=(int16_t)(acc>>15);
	acc=coef[0]*in[2]+coef[1]*in[1]+coef[2]*in[0]+coef[3]*x[ch][5]+coef[4]*x[ch][4]+coef[5]*x[ch][3]+coef[6]*x[ch][2];
	out[2]=(int16_t)(acc>>15);
	acc=coef[0]*in[1]+coef[1]*in[0]+coef[2]*x[ch][5]+coef[3]*x[ch][4]+coef[4]*x[ch][3]+coef[5]*x[ch][2]+coef[6]*x[ch][1];
	out[1]=(int16_t)(acc>>15);
	acc=coef[0]*in[0]+coef[1]*x[ch][5]+coef[2]*x[ch][4]+coef[3]*x[ch][3]+coef[4]*x[ch][2]+coef[5]*x[ch][1]+coef[6]*x[ch][0];
	out[0]=(int16_t)(acc>>15);

	//save last 6 samples for the next block
	memcpy (&x[ch][0],&x_old[ch][0],6*sizeof(int16_t));

}
#else
void filter_navy_headset (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{

	int n,i;
	//save last samples for the next block
	memcpy (&x_old[ch][0],&in[80-(NV_FILTER_SIZE-1)],(NV_FILTER_SIZE-1)*sizeof(int16_t));

	for (n=79; n>NV_FILTER_SIZE-2; n--)
	{
	    acc=0;
	    //acc=b0*x[n]+b0*x[n-1]+b0*x[n-2]+b0*x[n-3]+b0*x[n-4]+b0*x[n-5]+b0*x[n-6]
		for (i=0;i<NV_FILTER_SIZE;i++)
			acc+=coef[i]*in[n-i];
	    out[n]   = (int16_t)(acc>>15);

	}

	for (n=NV_FILTER_SIZE-2;n>=0;n--)
	{
		acc=0;
		for (i=0;i<NV_FILTER_SIZE;i++)
		{
			if ((n-i)>=0)
				acc+=coef[i]*in[n-i];
			else
				acc+=coef[i]*x[ch][(NV_FILTER_SIZE-1-(i-n))];
			out[n]=(int16_t)(acc>>15);
		}
	}


	//save last 6 samples for the next block
	memcpy (&x[ch][0],&x_old[ch][0],(NV_FILTER_SIZE-1)*sizeof(int16_t));

}

#endif
