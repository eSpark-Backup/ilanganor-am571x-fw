/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : caller id using MESI module
///	@file:	caller_id.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	29/Jul/2008	  Created	
//
/******************************************************************************/

#ifndef DTMF_DET_
#define DTMF_DET_

typedef struct DTMF_DET_struct	
{
	uint16_t channel[HD_MAX_DTMF_DETECT_CHANNELS];
	int config[HD_MAX_DTMF_DETECT_CHANNELS];
}DTMF_DET_struct;

/**************************************************************************/                     
//  Module		: dtmf_det_init
//  Description :  init dtmf detector module
/// @return none
/// @param	none
/**************************************************************************/
int dtmf_det_init(int channel,uint16_t associated_ch);


/**************************************************************************/                     
//  Module		: dtmf_det_process
//  Description :  
//
/// @return none
/// @param	none
/**************************************************************************/
void dtmf_det_process(int channel,int16_t *in);


/**************************************************************************/                     
//  Module		: test_caller_id
//  Description :  test caller id module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_dtmf_det(void);

void restart_dtmf_det(int channel);
#endif //DTMF_DET_
