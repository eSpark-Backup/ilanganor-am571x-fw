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
//#include "filter_iir32_coeffs.h" 

#include "filter.h"


void DSPe_FILTER_MAC(const int *x, const int *y, long long *r, short Qx)
{
	
	int a,b,c,d;
	int e;
	Qx = 32-Qx;
	a=*x;
	b=*y;
	c=_mpyluhs(a,b); // Xlow*Yhigh 
	d=_mpyhslu(a,b); // Xhigh*Ylow 
	e =_mpyh(a,b); 	// Xhigh*Yhigh 
	d+=c; 			// Xhigh*Ylow+Xlow*Yhigh 

	d=d>>16; 		// (Xhigh*Ylow+Xlow*Yhigh)>>16 
	
	e+=d; 			// Xhigh*Yhigh + 
					// (Xhigh*Ylow+Xlow*Yhigh)>>16 
	e = _sshl(e,Qx); // Normalise
	*r+=e;
}


void DSPe_FILTER_Init(	FILTER_Handle* filter)
{
	// cleat the delay line
	memset(filter->delayBuffer,0,((filter->nbiq)*2+2)*4);
}

void DSPe_FILTER(FILTER_Handle* filter, short *in, short *out, short N)
{
	 short idx,tap;
	 short  dlidx;
	 int data;
	 long long acc;
 	 int* coeffs;
  	 int* dline;
	 short Qx = filter->coeff_q_format; //Q30
	 for(idx = 0; idx<N; idx++)
	 {
		data = ((int)in[idx])<<12;
		//data = ((int)in[idx])<<15;

		acc = 0;
		coeffs = filter->coeffs;
		dline = filter->delayBuffer;
		dlidx = 0;
		for(tap = 0; tap<filter->nbiq; tap++)
		{
			acc = 0;
			DSPe_FILTER_MAC(&data, 		  (coeffs++), &acc, Qx); 
			DSPe_FILTER_MAC(&dline[dlidx+0], (coeffs++), &acc, Qx);
			DSPe_FILTER_MAC(&dline[dlidx+1], (coeffs++), &acc, Qx);
			DSPe_FILTER_MAC(&dline[dlidx+2], (coeffs++), &acc, Qx);
			DSPe_FILTER_MAC(&dline[dlidx+3], (coeffs++), &acc, Qx);

			dline[dlidx+1] = dline[dlidx+0];
			dline[dlidx+0] = data; 

			data = acc;
			dlidx +=2;
		}

		dline[dlidx+1] = dline[dlidx+0];
		dline[dlidx+0] = data; 

		
		acc = acc>>12;
		//acc = acc>>15;
//		acc = min(acc,0x00007FFF); // saturate
//  	acc = max(acc,0xFFFF8000); // saturate
		out[idx] =acc;

	}
}
