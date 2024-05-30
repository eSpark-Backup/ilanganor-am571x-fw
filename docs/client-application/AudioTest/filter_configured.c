/*******************************************************************************
  MER RAC DSP Software
  Copyright (c) 2006 DSPECIALISTS GmbH Berlin,Germany                                    
                                                                      
  SYSTEM:       TI-C64XX
  FILE:         $RCSfile$                                              
  RELEASE:      $Name$      
  VERSION:      $Revision: 1$
  DATE:         $Date: ?????? ?????????? 04 ?????? 2010 09:14:28$                                                
  AUTHOR:       maxim;$Author: ilan_r$                                                               
                                                                      
  DESCRIPTION:  HPF HiPass Filter

  ========                                                              
  HISTORY:                                                              
				$Log$
				ilan_r - ?????? ?????????? 04 ?????? 2010 09:14:28
				ilan_r - Tuesday, April 07, 2009 7:57:43 AM
				ilan_r - ?????? ?????????? 11 ?????????????? 2006 11:03:44
			                                                                      
*******************************************************************************/
#include <stdint.h>
//#include "filter_iir32_coeffs.h"
#include <string.h>
#include "DigitalCore_hpi.h"
#include "filter.h"
#include "filter_configured.h"

extern HD_host_dsp_ctrl  hd_host_ctrl;

FILTER_Handle	hfilter[HD_FILTER_MAX_CHANNELS];
int32_t delay_a [HD_FILTER_MAX_CHANNELS][HD_FILTER_MAX_BQ*2+2];
int32_t filter_coef[HD_FILTER_MAX_CHANNELS][HD_FILTER_MAX_BQ*5];
#pragma DATA_SECTION	(filter_ext,".ISRAM_CODE")
HD_filter_ext	filter_ext;

/**************************************************************************/
//  Module		: filter_init
//  Description :  initialze and clean filter struct
/// @return none
/// @param	ch- filter channel number
/**************************************************************************/
void filter_init()
{
	memset(hfilter,0,sizeof(hfilter));
	memset(&filter_ext,0,sizeof(filter_ext));


}

/**************************************************************************/
//  Module		: filter_config
//  Description :  configure filter parameters
/// @return none
/// @param	ch- filter channel number
/**************************************************************************/
void filter_config(int ch)
{
	int i;
	if (ch>HD_FILTER_MAX_CHANNELS-1)
		return;
	if (filter_ext.number_of_bq>HD_FILTER_MAX_BQ)
		return;
	hfilter[ch].channel=filter_ext.data_channel;
	hfilter[ch].in_out=filter_ext.in_out_channel;

	hfilter[ch].coeffs=filter_coef[ch];
	for (i=0;i<filter_ext.number_of_bq;i++)
	{
		filter_coef[ch][i*5]=filter_ext.coeff[i*5];//b0
		filter_coef[ch][i*5+1]=filter_ext.coeff[i*5+1];//b1
		filter_coef[ch][i*5+2]=filter_ext.coeff[i*5+2];//b2
		filter_coef[ch][i*5+3]=-filter_ext.coeff[i*5+3];//-a1
		filter_coef[ch][i*5+4]=-filter_ext.coeff[i*5+4];//-a2
	}


	hfilter[ch].nbiq=filter_ext.number_of_bq;
	hfilter[ch].delayBuffer=delay_a[ch];
	hfilter[ch].coeff_q_format=filter_ext.coeff_q_format;
	DSPe_FILTER_Init(&hfilter[ch]);
	
}

/**************************************************************************/
//  Module		: filter_process
//  Description :  filter the audio channel
/// @return none
/// @param	ch- audio channel number
/// @param	in_out- audio channel in or out
/// @param	in- audio input buffer
/// @param	out- audio output buffer
/**************************************************************************/
void filter_process(int ch,int in_out,int16_t* in,int16_t* out)
{
	int i;
	//find filter channel for this audio channel
	for (i=0;i<HD_FILTER_MAX_CHANNELS;i++)
	{
		if ((hfilter[i].channel==ch)&&(hfilter[i].in_out==in_out))
		{
			if (hfilter[i].nbiq==0)
				return;
			DSPe_FILTER(&hfilter[i], in, out, FILT_NSAMPLE);
		}
	}
}
