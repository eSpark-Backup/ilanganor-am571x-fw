/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : handle frame from and to host
///	@file:	host_frame.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	15/Oct/2008   Created	
//
/******************************************************************************/
#ifndef HOST_FRAME_
#define HOST_FRAME_

typedef struct HF_host_frame_type
{
	uint16_t	last_frame_type; //last frame type. example: HD_G711_FRAME  
	uint8_t		last_vad; //last vad. example:0/1/2		
	uint8_t		done_decoding; //indicate if decoding was done. value - true/false
}HF_host_frame_type;



/**************************************************************************/                     
//  Module		: host_frame_in
//  Description : handle host channel frames, call vocoder to handel each
//					frame and pass resault to audio buffer.
//				host frames buffer format:
//				total size, number of frames
//				frame size,	data type,	ch index
//				frame size,	data type,	ch index
//				...				
/// @return none
/// @param	none
/**************************************************************************/
int host_frame_in(int16_t *speech_in,int8_t *host_in);

/**************************************************************************/                     
//  Module		: host_frame_out
//  Description : build message to be send to host, call encoder to handel each
//					frame and pass resault to host frames buffer.
//				host frames buffer format:
//				total size, number of frames
//				frame size,	data type,	ch index
//				frame size,	data type,	ch index
//				...				
/// @return none
/// @param	none host_out-frames buffer pointer, speech_out - audio buffer pointer
//		ch_num-number of channel, buff_size- size left in frame buffer		
/**************************************************************************/
int host_frame_out(int8_t *host_out,int16_t *speech_out,uint16_t ch_num,uint16_t buff_size);

/**************************************************************************/                     
//  Module		: init_host_audio
//  Description : 
///					allocate buffer for audio received and send to host
/// @return none
/// @param	none
/**************************************************************************/
int init_host_audio(void);

/**************************************************************************/                     
//  Module		: prepare_host_buffer
//  Description : send and receive buffers to host
///					
/// @return none
/// @param	none
/**************************************************************************/
void prepare_host_buffer(void);

void prepare_host_in_buffer(void);

/**************************************************************************/                     
//  Module		: send_event_frame
//  Description : inset an event frame to frame memory
///					
/// @return none
/// @param	none
/**************************************************************************/
int send_event_frame(void* event, uint16_t size,uint16_t channel,
	uint16_t event_type,uint16_t event_sub_type);
#endif //end HOST_FRAME_

