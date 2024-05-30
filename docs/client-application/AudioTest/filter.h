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
				ilan_r - ?????? ?????????? 11 ?????????????? 2006 11:03:45
			                                                                      
*******************************************************************************/
#ifndef __FILTER_H_
#define __FILTER_H_


// define HPF Instance  
typedef struct filter_str 
{
	int channel;//the channel we need to filter
	int in_out; //is the channel we filter is in or out
   	int *delayBuffer;//[2*FILTER_NBIQ+2];
	int *coeffs;//pointer to bi-quad coefficient array b2_0,b1_0,b0_0,a2_0,a1_0,b2_1 ...
	int nbiq;//number of bi-quad sections
	int coeff_q_format;//q format for example 30 for q30
} FILTER_Handle;

void DSPe_FILTER_MAC(const int *x, const int *y, long long *r, short Qx);
void DSPe_FILTER_Init(FILTER_Handle* hpf);
void DSPe_FILTER(FILTER_Handle* filter, short *in, short *out, short N);

#endif ///__FILTER_H_
