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


#ifndef FILTER_DC_
#define FILTER_DC_


/**************************************************************************/                     
//  Module		: filter_dc_init
//  Description :  clear filter delay line
/// @return none
/// @param	none
/**************************************************************************/
void filter_dc_init(uint32_t ch, uint32_t dir);


/**************************************************************************/                     
//  Module		: filter_dc
//  Description :  implement low freq (dc) filter
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	dir- HD_DAC or HD_ADC
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_dc (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out);

#endif //FILTER_DC_
