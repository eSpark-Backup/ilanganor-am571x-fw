/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : configure systems as defined by arm, open drivers as defined
//					by arm
//				  
///		capturing 
///	@file:	system_config.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	29/Jul/2008   Created	
//
/******************************************************************************/
#ifndef SYS_CONFIG_
#define SYS_CONFIG_
#include "DigitalCore_hpi.h"

#define SC_WAIT_CH_CONFIG	1
#define SC_HOST_CH_INIT		2
#define SC_WAIT_AC_INIT		3
#define SC_NORMAL			4

#define SC_RUN		0
#define SC_RESTART	1




//Error codes
#define SC_ERR_CH_CONFIG	1
#define SC_ERR_CODEC_ID		2
#define SC_ERR_CH_NON_SEQ	3
#define SC_ERR_CODEC_OPEN	4


//need to be move to host_ctrl struct
#define SC_MAX_FRAME_SIZE (sizeof(HD_audio_pcm_frame)+sizeof(HD_channel_frame))
#define SC_MIN_FRAME_SIZE (sizeof(HD_channel_frame))
#define SC_MAX_FRAME_BUFFER_SIZE ((SC_MAX_FRAME_SIZE*HD_MAX_HOST_DSP_CHANNELS)+sizeof(HD_channels_frame_size))

#define SC_INTERNAL_EVENT_DSP_ERR_SIZE (sizeof(HD_events_dsp_err)+sizeof(HD_event_frame)+sizeof(HD_channel_frame))
#define SC_INTERNAL_EVENTS_HOST_ERR_SIZE (sizeof(HD_events_dsp_host_err)+sizeof(HD_event_frame)+sizeof(HD_channel_frame))
#define SC_INTERNAL_EVENTS_LOAD_ERR_SIZE (sizeof(HD_events_dsp_load_err)+sizeof(HD_event_frame)+sizeof(HD_channel_frame))
#define SC_INTERNAL_EVENTS_CALLER_ID_SIZE (sizeof(HD_events_caller_id)+sizeof(HD_event_frame)+sizeof(HD_channel_frame))
#define SC_INTERNAL_EVENTS_DTMF_DET_SIZE (sizeof(HD_events_dtmf_detect)+sizeof(HD_event_frame)+sizeof(HD_channel_frame))

#define SC_INTERNAL_EVENTS_SIZE (SC_INTERNAL_EVENT_DSP_ERR_SIZE+SC_INTERNAL_EVENTS_HOST_ERR_SIZE+ \
SC_INTERNAL_EVENTS_LOAD_ERR_SIZE+HD_MAX_CALLER_ID_CHANNELS*SC_INTERNAL_EVENTS_CALLER_ID_SIZE+\
HD_MAX_DTMF_DETECT_CHANNELS*SC_INTERNAL_EVENTS_DTMF_DET_SIZE) 

typedef struct SC_channel_config
{
	uint16_t	number_of_mcbsp0_channel;		//number of channel in McBsp0 
	uint16_t	number_of_mcbsp1_channel;	//number of channel in McBsp1
	uint16_t	number_of_i2c0_channel;		//number of channel in i2c0
	uint16_t	number_of_i2c1_channel;		// number of channel in i2c1
	uint16_t	number_of_host_dsp_channel;	//number of channel transfer  between host and dsp
	uint16_t	number_of_tone_channel;		//number of internal generated tone channel
	uint32_t 	channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
	uint32_t 	channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
	
	uint32_t	frames_buffer_size;
	uint64_t	codec_channel_active; //every bit set indicate the codec channel is active
	uint64_t	codec_channel_opened; //every bit set indicate the codec channel is opened (device driver is opened)
	uint64_t	channel_input_a; //every bit set indicate this channel is an input
	uint64_t	channel_input_b; //every bit set indicate this channel is an input

	uint64_t	channel_output_a; //every bit set indicate this channel is an output(0-63)
	uint64_t	channel_output_b; //every bit set indicate this channel is an output(64-end)
	uint32_t	device_active; //every bit set indicate this device is active (number are accurding to dev_function order) 
	uint32_t	no_audio; //flag that indicate too match time passed without audio
	uint64_t	codec_channel_input; //list of input channel, this list is update if no audio recived from codecs
} SC_channel_config;


/**************************************************************************/                     
//  Module		: system_config
//  Description : open codec in the relevant port as configured by host cpu
/// @return none
/// @param	none
/**************************************************************************/
int system_config(uint16_t restart);

/**************************************************************************/                     
//  Module		: get_config
//  Description : fill ch_config struct with value 
/// @return none
/// @param	none
/**************************************************************************/
int get_config(void);

/**************************************************************************/                     
//  Module		: find_device_id
//  Description : find the device in the device id table 
/// @return none
/// @param	none
/**************************************************************************/
int find_device_id(uint16_t codec_type);


/**************************************************************************/                     
//  Module		: find_device_id
//  Description : find the device in the device id table 
/// @return none
/// @param	none
/**************************************************************************/
int open_driver_group(uint16_t start_ch,uint16_t stop_ch);


/**************************************************************************/                     
//  Module		: test_system_config
//  Description : module that check system_config functionality,
//					it call system config with diffrent input and check 
//					result is correct
/// @return none
/// @param	none
/**************************************************************************/
void test_system_config(void);

/**************************************************************************/                     
//  Module		: system_config_init
//  Description : initiate system control module
/// @return none
/// @param	none
/**************************************************************************/
void system_config_init(void);

extern SC_channel_config    ch_config;

#endif //end SYS_CONFIG_

