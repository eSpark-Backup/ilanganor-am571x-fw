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
#ifndef __FILTER_TEST_H_
#define __FILTER_TEST_H_

#define FILT_NSAMPLE 80
/**************************************************************************/
//  Module		: filter_init
//  Description :  initialze and clean filter struct
/// @return none
/**************************************************************************/
void filter_init();

/**************************************************************************/
//  Module		: filter_config
//  Description :  configure filter parameters
/// @return none
/// @param	ch- filter channel number
/**************************************************************************/
void filter_config(int ch);

/**************************************************************************/
//  Module		: filter_process
//  Description :  filter the audio channel
/// @return none
/// @param	ch- audio channel number
/// @param	in_out- audio channel in or out
/// @param	in- audio input buffer
/// @param	out- audio output buffer
/**************************************************************************/
void filter_process(int ch,int in_out,int16_t* in,int16_t* out);
#endif ///__FILTER_TEST_H_
