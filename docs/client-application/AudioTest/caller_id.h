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

#ifndef CALLER_ID_
#define CALLER_ID_

#include "DigitalCore_hpi.h"


typedef struct CI_struct	
{
	uint16_t channel[HD_MAX_CALLER_ID_CHANNELS];
	int config[HD_MAX_CALLER_ID_CHANNELS];
	uint16_t type[HD_MAX_CALLER_ID_CHANNELS];
	uint16_t detection_mode[HD_MAX_CALLER_ID_CHANNELS];

}CI_struct;

/**************************************************************************/                     
//  Module		: caller_id_init
//  Description :  init caller id module
/// @return none
/// @param	none
/**************************************************************************/
int caller_id_init(int channel,uint16_t a_type,uint16_t a_detection_mode,uint16_t associated_ch);

/**************************************************************************/                     
//  Module		: caller_id_process
//  Description :  
//
/// @return none
/// @param	none
/**************************************************************************/
void caller_id_process(int channel,int16_t *in);

/**************************************************************************/                     
//  Module		: printf_CID
//  Description :  print caller id info
/// @return none
/// @param	none
/**************************************************************************/
void printf_CID(short X[], short chan_num);

/**************************************************************************/                     
//  Module		: test_caller_id
//  Description :  test caller id module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_caller_id(void);


void restart_caller_id(int channel);


/**************************************************************************/                     
//  Module		: rx_block_initialize
//  Description :  initialize caller id block (replace exist function) 
/// @return none
/// @param	none
/**************************************************************************/
//void rx_block_initialize(struct START_PTRS *rx );
#endif //CALLER_ID_
