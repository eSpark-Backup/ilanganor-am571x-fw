/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : filter dc - filter around 1.6KHZ
///	@file:	filter_navy_headset.h
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	20/Aug/2013	  Created	
//
/******************************************************************************/


#ifndef FILTER_NAVY_HEADSET_
#define FILTER_NAVY_HEADSET_


/**************************************************************************/                     
//  Module		: filter_navy_headset_init
//  Description :  clear filter delay line
/// @return none
/// @param	none
/**************************************************************************/
void filter_navy_headset_init(uint32_t ch ,uint32_t dir);


/**************************************************************************/                     
//  Module		: filter_navy_headset
//  Description :  implement low freq (dc) filter
/// @return none
/// @param	channel- channel number <HD_MAX_CHANNELS
/// @param	in- input samples buffer
/// @param	out- output samples buffer
/**************************************************************************/
void filter_navy_headset (uint32_t ch, uint32_t dir, int16_t* in, int16_t* out);

#endif //FILTER_NAVY_HEADSET_
