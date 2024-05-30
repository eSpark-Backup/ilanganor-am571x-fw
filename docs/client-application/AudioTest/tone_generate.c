/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : configure systems as defined by arm, open drivers as defined
//					by host cpu
///	@file:	tone_generate.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	14/Aug/2008	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/


#include <std.h>
#include <stdint.h>
#include <c6x.h>
#include <csl.h>
#include <csl_gpio.h>
#include <math.h>
#include <string.h>
//#include "BoardUtility\BoardUtility.h"
#include "main.h"
#include "DigitalCore_hpi.h"
#include "tone_generate.h"


TG_tone_table_type TG_tone_table[HD_MAX_TONE_CHANNELS];
extern HD_host_dsp_ctrl  hd_host_ctrl;
//extern GPIO_Handle hGpio;

TG_tone_mux_type TG_tone_mux;




static int16_t sin_table[66] =
{
	0,804,1608,2410,3212,4011,4808,5602,
	6393,7179,7962,8739,9512,10278,11039,11793,
	12539,13279,14010,14732,15446,16151,16846,17530,
	18204,18868,19519,20159,20787,21403,22005,22594,
	23170,23731,24279,24811,25329,25832,26319,26790,
	27245,27683,28105,28510,28898,29268,29621,29956,
	30273,30571,30852,31113,31356,31580,31785,31971,
	32137,32285,32412,32521,32609,32678,32728,32757,
	32767,32757
};


/**************************************************************************/                     
//  Module		: tone_gen
//  Description :  generate single tone
/// @return none
/// @param	none
/**************************************************************************/
void tone_gen(int16_t* in,uint16_t size_in,uint16_t ch)
{
//	static double arg=0;
//	uint16_t arg_inc=0;
	int i;
	
	//arg_inc=2*pi*f/sampling_rate
//	arg_inc=((uint32_t)TG_tone_table[ch].freq_hz*32767)/4000;
	//arg_inc=0.00078539816339*(double)TG_tone_table[ch].freq_hz;	
	for(i=0;i<size_in;i++)
	{		
		TG_tone_table[ch].arg+=TG_tone_table[ch].arg_inc;
	//	if (TG_tone_table[ch].arg>16383)
	//		TG_tone_table[ch].arg=TG_tone_table[ch].arg-16383;		
	//	in[i]=32767*sin(TG_tone_table[ch].arg);
	//	in[i]=32767*sin_func(TG_tone_table[ch].arg);
		in[i]=sin_func(TG_tone_table[ch].arg);
		if (TG_tone_table[ch].arg>65535)
			TG_tone_table[ch].arg=TG_tone_table[ch].arg-65535;
	
	}

}




/**************************************************************************/                     
//  Module		: tone_command
//  Description :  generate tone command
/// @return none
/// @param	none
/**************************************************************************/
/*
void tone_command(int16_t command,int16_t* in,int16_t mode)
{
	static uint16_t tone_state=TG_HIGH_GUARD_STATE,duration=0;
	
	

	if (mode==TG_DISABLE)
	{
		tone_state=TG_HIGH_GUARD_STATE;	
		duration=0;		
		return;
	}	
	switch(tone_state)
	{

		case TG_HIGH_GUARD_STATE:

			if (duration==0)
			{
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
			}


			tone_gen(in,NSAMPLES,TG_GAURD_TONE_FREQ,TG_HIGH_GAURD_TONE_VOLUME,TG_ONLY_TONE);		
			if (duration>(TG_HIGH_GAURD_DURATION-2))
			{
				tone_state=TG_FUNCTION_STATE;
	
			}
				

		break;

		case TG_FUNCTION_STATE:
		
			if (duration==TG_HIGH_GAURD_DURATION)
			{	
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
			}
		
			tone_gen(in,NSAMPLES,command,TG_FUNC_TONE_VOLUME,TG_ONLY_TONE);		
			if (duration>(TG_HIGH_GAURD_DURATION+TG_FUNC_DURATION-2))
				tone_state=TG_LOW_GUARD_STATE;

		break;

		case TG_LOW_GUARD_STATE:
			if (duration>1)
			{	
						//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-11;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
			}		

			if (mode==TG_ENABLE)
				tone_gen(in,NSAMPLES,TG_GAURD_TONE_FREQ,TG_LOW_GAURD_TONE_VOLUME,TG_ADDING_TONE);				
			duration=0;
		break;	
	
	
	}
	duration++;

}
*/

/**************************************************************************/                     
//  Module		: tone_command
//  Description :  generate tone command
/// @return none
/// @param	none
/**************************************************************************/
void tone_command(int16_t command,int16_t ch,int16_t mode)
{
	static uint16_t tone_state=TG_HIGH_GUARD_STATE,duration=0;	

	if (mode==TG_DISABLE)
	{
		tone_state=TG_HIGH_GUARD_STATE;	
		duration=0;	
		hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[ch]=0;
		hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;	
		return;
	}	
	switch(tone_state)
	{

		case TG_HIGH_GUARD_STATE:

			if (duration==0)
			{
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[ch]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
			}
			hd_host_ctrl.sHD_tone_mux_configuration.sum_to_speech[ch]=HD_TONE_ONLY_MUX;
			hd_host_ctrl.sHD_tone_mux_configuration.volume[ch]=TG_HIGH_GAURD_TONE_VOLUME;
			hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[ch]=TG_GUARD_TONE_NUM;
			hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;
			//tone_gen(in,NSAMPLES,TG_GAURD_TONE_FREQ,TG_HIGH_GAURD_TONE_VOLUME,TG_ONLY_TONE);		
			if (duration>(TG_HIGH_GAURD_DURATION-2))
			{
				tone_state=TG_FUNCTION_STATE;
	
			}
				

		break;

		case TG_FUNCTION_STATE:
		
			if (duration==TG_HIGH_GAURD_DURATION)
			{	
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[ch]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
			}
			hd_host_ctrl.sHD_tone_mux_configuration.sum_to_speech[ch]=HD_TONE_ONLY_MUX;
			hd_host_ctrl.sHD_tone_mux_configuration.volume[ch]=TG_FUNC_TONE_VOLUME;
			hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[ch]=command;
			hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;
			//tone_gen(in,NSAMPLES,command,TG_FUNC_TONE_VOLUME,TG_ONLY_TONE);		
			if (duration>(TG_HIGH_GAURD_DURATION+TG_FUNC_DURATION-2))
				tone_state=TG_LOW_GUARD_STATE;

		break;

		case TG_LOW_GUARD_STATE:
			if (duration>1)
			{	
						//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[ch]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=9;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-11;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
			}		

			if (mode==TG_ENABLE)
			{
				hd_host_ctrl.sHD_tone_mux_configuration.sum_to_speech[ch]=HD_TONE_AUDIO_MUX;
				hd_host_ctrl.sHD_tone_mux_configuration.volume[ch]=TG_LOW_GAURD_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[ch]=TG_GUARD_TONE_NUM;
				hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;
			}	
				//tone_gen(in,NSAMPLES,TG_GAURD_TONE_FREQ,TG_LOW_GAURD_TONE_VOLUME,TG_ADDING_TONE);				
			duration=0;
		break;	
	
	
	}
	duration++;

}

/**************************************************************************/                     
//  Module		: tone_generate
//  Description :  update tone configuration and generate tone if needed
/// @return none
/// @param	none
/**************************************************************************/
/*
void tone_generate(void)
{
	uint16_t i;
	//update tone table
	
	for (i=0;i<HD_MAX_TONE_CHANNELS;i++)
	{
		TG_tone_table[i].enable=0;
		if (hd_host_ctrl.sHD_tone_configuration.HD_request==TRUE)
		{
			if	(hd_host_ctrl.sHD_tone_configuration.channels_mask[i]==TRUE)
			{
				TG_tone_table[i].freq_hz=hd_host_ctrl.sHD_tone_configuration.freq_hz;
//				TG_tone_table[i].duration=hd_host_ctrl.sHD_tone_configuration.duration;
//				TG_tone_table[i].volume=hd_host_ctrl.sHD_tone_configuration.volume;
				hd_host_ctrl.sHD_tone_configuration.channels_mask[i]=FALSE;
			}
			hd_host_ctrl.sHD_tone_configuration.HD_request=FALSE;
		}
		
	}
	
	
	//check freq mux demand
	for (i=0;i<HD_MAX_CHANNELS;i++)
	{
		if ((hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[i]<HD_MAX_TONE_CHANNELS)&&
			(hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[i]!=0))
		
			TG_tone_table[hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[i]-1].enable=1;			


		if ((hd_host_ctrl.sHD_tone_mux_configuration.tones_number_b[i]<HD_MAX_TONE_CHANNELS)&&
			(hd_host_ctrl.sHD_tone_mux_configuration.tones_number_b[i]!=0))
		
			TG_tone_table[hd_host_ctrl.sHD_tone_mux_configuration.tones_number_b[i]-1].enable=1;			
		
	}
	
	//call tone to generate the needed tone sample
	for (i=0;i<HD_MAX_TONE_CHANNELS;i++)
	{
		if (TG_tone_table[i].enable==TRUE)
			tone_gen(TG_tone_table[i].tone_samples,NSAMPLES,i);
	}

}		
*/
/**************************************************************************/                     
//  Module		: tone_generate
//  Description :  update tone configuration and generate tone if needed
/// @return none
/// @param	none
/**************************************************************************/
void tone_generate(void)
{
	uint16_t i;
	
	//update tone table
	if (hd_host_ctrl.sHD_tone_configuration.HD_request==TRUE)
	{
		for (i=0;i<HD_MAX_TONE_CHANNELS;i++)
		{

			if	(hd_host_ctrl.sHD_tone_configuration.freq_hz[i])
			{
				TG_tone_table[i].freq_hz=hd_host_ctrl.sHD_tone_configuration.freq_hz[i];
//				TG_tone_table[i].duration=hd_host_ctrl.sHD_tone_configuration.duration;
//				TG_tone_table[i].volume=hd_host_ctrl.sHD_tone_configuration.volume;
				//arg_inc=2*pi*f/sampling_rate  (new arg_inc=2*f/sampling_rate)
				TG_tone_table[i].arg_inc=((uint32_t)TG_tone_table[i].freq_hz*32767)/4000;				
			}
			
		}
		hd_host_ctrl.sHD_tone_configuration.HD_request=FALSE;	
		
	}
	

	
	//call tone to generate the needed tone sample
	for (i=0;i<HD_MAX_TONE_CHANNELS;i++)
	{
		if (TG_tone_table[i].freq_hz!=0)
			tone_gen(TG_tone_table[i].tone_samples,NSAMPLES,i);
	}
	
}		
	 




/**************************************************************************/                     
//  Module		: tone_mux
//  Description :  
/// @return none
/// @param	none
/**************************************************************************/
void tone_mux(int16_t *out,int16_t ch)
{	
	uint16_t sample;


	//mux tone
	if ((TG_tone_mux.tones_number_a[ch]<HD_MAX_TONE_CHANNELS)&&
		(TG_tone_mux.tones_number_a[ch]!=0)&&
		(TG_tone_mux.volume[ch]!=0)&&
		(TG_tone_mux.tones_number_b[ch]<HD_MAX_TONE_CHANNELS))
	{	
 			
		for (sample=0;sample<NSAMPLES;sample++)	
		{
			if (TG_tone_mux.tones_number_b[ch]==0)
			{
				if (TG_tone_mux.sum_to_speech[ch]==HD_TONE_AUDIO_MUX)	
					(*(out+sample))=(*(out+sample))+((int16_t)(/*(double)*/TG_tone_table[TG_tone_mux.tones_number_a[ch]-1].tone_samples[sample]
					/ (TG_tone_mux.volume[ch])));
				else
					(*(out+sample))=(int16_t)(/*(double)*/TG_tone_table[TG_tone_mux.tones_number_a[ch]-1].tone_samples[sample]
					/ /*(double)*/(TG_tone_mux.volume[ch]));
			}
			else
			{		
				if (TG_tone_mux.sum_to_speech[ch]==HD_TONE_AUDIO_MUX)	
					((*(out+sample))=(*(out+sample))+((int16_t)((/*(double)*/TG_tone_table[TG_tone_mux.tones_number_a[ch]-1].tone_samples[sample]
					+/*(double)*/TG_tone_table[TG_tone_mux.tones_number_b[ch]-1].tone_samples[sample])/(TG_tone_mux.volume[ch]))));
				else
					(*(out+sample))=(int16_t)((/*(double)*/TG_tone_table[TG_tone_mux.tones_number_a[ch]-1].tone_samples[sample]
					+/*(double)*/TG_tone_table[TG_tone_mux.tones_number_b[ch]-1].tone_samples[sample])/ /*(double)*/(TG_tone_mux.volume[ch]));
			}				
		}
		
	}
	//check if this is tone disable command
	if (TG_tone_mux.tones_number_a[ch]==0)
	{
		//if no audio need to be added clear the output buffer
		if (TG_tone_mux.sum_to_speech[ch]==HD_TONE_ONLY_MUX)
		{
			//clear out buffer (one time only)
			TG_tone_mux.sum_to_speech[ch]=HD_TONE_AUDIO_MUX;
			memset(out, 0, NSAMPLES*2);
		}	
	}	

}


/**************************************************************************/                     
//  Module		: tone_init
//  Description :  init mux module
/// @return none
/// @param	none
/**************************************************************************/
void tone_init(void)
{

	memset((void*)&TG_tone_mux,0,sizeof(TG_tone_mux));
	memset(TG_tone_table,0,sizeof(TG_tone_table));

}

/**************************************************************************/                     
//  Module		: tone_mux_config
//  Description :  config tone mux according to host control
/// @return none
/// @param	none
/**************************************************************************/
void tone_mux_config(void)
{
	if (hd_host_ctrl.sHD_tone_mux_configuration.HD_request==TRUE)
	{
		memcpy(&TG_tone_mux,&(hd_host_ctrl.sHD_tone_mux_configuration),sizeof(TG_tone_mux));
		hd_host_ctrl.sHD_tone_mux_configuration.HD_request=FALSE;
	}

}


/**************************************************************************/                     
//  Module		: test_tone
//  Description :  mux tone for testing (assume host ctrl is in debug and
//				 tone was configured
/// @return none
/// @param	none
/**************************************************************************/
void test_tone(void)
{
	static unsigned int test_count;
	
	test_count++;
//	if (test_count&0x01)
	{
		hd_host_ctrl.sHD_tone_mux_configuration.sum_to_speech[0]=HD_TONE_ONLY_MUX;
		hd_host_ctrl.sHD_tone_mux_configuration.volume[0]=TG_HIGH_GAURD_TONE_VOLUME;
		hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[0]=1;//TG_GUARD_TONE_NUM;
	//	hd_host_ctrl.sHD_tone_mux_configuration.tones_number_b[3]=TG_REPETER_ENABLE_TONE_NUM;

		hd_host_ctrl.sHD_tone_mux_configuration.sum_to_speech[8]=HD_TONE_ONLY_MUX;
		hd_host_ctrl.sHD_tone_mux_configuration.volume[8]=TG_HIGH_GAURD_TONE_VOLUME;
		hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[8]=1;//TG_GUARD_TONE_NUM;

		hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;
	}
//	else
//	{
//		hd_host_ctrl.sHD_tone_mux_configuration.tones_number_a[3]=0;
//		hd_host_ctrl.sHD_tone_mux_configuration.HD_request=TRUE;
//	}		
	

}



/**************************************************************************/                     
//  Module		: sin_func
//  Description :  implementation of sin function usinf look up table
//				 argument receive is in pi - 16383(q15 - 0.5) is pi/2 32767 is pi
/// @return none
/// @param	none
/**************************************************************************/
int16_t sin_func(uint16_t phase)
{

	int16_t s0;
	uint16_t tmp_phase, tmp_phase_hi;
	
	tmp_phase = phase&0x7fff;
	
	if(tmp_phase&0x4000) tmp_phase = 0x8000 - tmp_phase;
	
	tmp_phase_hi = tmp_phase>>8; // 0...64
	
	s0  = sin_table[tmp_phase_hi];
	
	s0 += ((int16_t)((((int32_t)(sin_table[tmp_phase_hi+1] - 
	s0))*(tmp_phase&0xff))>>8));
	
	if(phase&0x8000) s0 = -s0;
	
	return s0;
}

int16_t cos_func(uint16_t phase)
{
return sin_func(phase + 0x4000);
}
