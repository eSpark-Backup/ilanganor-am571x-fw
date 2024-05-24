/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : device driver for the MTR board
///		capturing 
///	@file:	device_mtr4ch.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	15/Jul/2008   Created	
//
/******************************************************************************/
#ifndef DELAY_BUFFER_H_
#define DELAY_BUFFER_H_


#define DLYBUF_MAX_BUFFER_SIZE (100*7)		//max delay buffer size 7sec(10msec steps)

enum DLYBUF_ERRORS {DLYBUF_CH_EXIST_OPEN_FAILED=1, DLYBUF_SIZE_REQ_FAILED, 
	DLYBUF_MEMO_ALLOC_OPEN_FAILED,DLYBUF_NO_CH_DELAY_FAILED,
	DLYBUF_NO_CH_AVAILEBLE_FAILED, 
	DLYBUF_BUFF_FULL,DLYBUF_BUFF_EMPTY};




/**************************************************************************/                     
//  Module		: DLYBUF_init()
//  Description : initialize internal struct
/**************************************************************************/
void DLYBUF_init();

/**************************************************************************/                     
//  Module		: DLYBUF_open
//  Description : allocate memory for delay buffer and set init parameters
//					values, 
//					REMARKS: for carrier detect mode you need to configure
//							and enable cd module preior using this module
/// @return 0 if succeed
/// @param	channel - audio cahnnel number
/// @param	ch_in_out - input or output from dsp HD_ADC(in) or HD_DAC(out)
/// @param	param - parameter to init the module
/**************************************************************************/
int DLYBUF_open(uint16_t channel, uint16_t ch_in_out,HD_dly_init_param param);

/**************************************************************************/                     
//  Module		: DLYBUF_close
//  Description : close delay channel so it can be use later
//					free allocated memory 
/// @param	channel - audio cahnnel number
/// @param	ch_in_out - input or output from dsp HD_ADC(in) or HD_DAC(out)
/**************************************************************************/
void DLYBUF_close(uint16_t channel, uint16_t ch_in_out);

/**************************************************************************/                     
//  Module		: DLYBUF_delay
//  Description : implement delay buffer 
/// @return 0 if succeed
/// @param	channel-audio cahnnel number
/**************************************************************************/
int DLYBUF_delay(uint16_t channel,uint16_t ch_in_out,int16_t *audio_in
				,int16_t *audio_out);

/**************************************************************************/                     
//  Module		: DLYBUF_config
//  Description : update control parameters
/// @return 0 if succeed
/// @param	channel - audio cahnnel number
/// @param	param - parameter to init the module
/**************************************************************************/
int DLYBUF_config(uint16_t channel, uint16_t ch_in_out,HD_dly_ctrl_param param);

/**************************************************************************/
//  Module		: DLYBUF_test
//  Description : implement unit tests for delay buffer module
/// @return 0 if succeed
/// @param	none
/**************************************************************************/
int DLYBUF_test();

#endif //end DELAY_BUFFER_H_

