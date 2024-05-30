/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : host interface
//				  
///		capturing 
///	@file:	host_ctrl.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	10/Aug/2008   Created	
//
/******************************************************************************/
#ifndef HOST_CTRL_
#define HOST_CTRL_

typedef enum {
	  HC_SYS_CONFIG_STATE=0
	, HC_AC_INIT_STATE
	, HC_AC_INIT_STATE2
	, HC_AC_CONFIG_STATE1
	, HC_AC_CONFIG_STATE2
	, HC_AC_CONFIG_STATE3
	, HC_AC_CONFIG_STATE4
	, HC_TONE_CONFIG_STATE1
	, HC_TONE_CONFIG_STATE2
	, HC_TONE_CONFIG_STATE3
	, HC_TONE_CONFIG_STATE4
	, HC_TONE_CONFIG_STATE5
	, HC_TONE_CONFIG_STATE6
	, HC_AGC_CONFIG_STATE1
	, HC_AGC_CONFIG_STATE2
	, HC_AGC_CONFIG_STATE3
	, HC_FILTER_CONFIG_STATE1
	, HC_FILTER_CONFIG_STATE2
	, HC_CD_CONFIG_STATE1
	, HC_CD_CONFIG_STATE2
	, HC_CD_CONFIG_STATE3
	, HC_ECHO_CANCEL_CONFIG_STATE1
	, HC_ECHO_CANCEL_CONFIG_STATE2
	, HC_ECHO_CANCEL_CONFIG_STATE3
	, HC_MUX_CONFIG_STATE
	, HC_VOCODER_CONFIG_STATE1
	, HC_VOCODER_CONFIG_STATE2
	, HC_VOCODER_CONFIG_STATE3
	, HC_AC_INIT_STATE1
	, HC_CALLER_ID_CONFIG_STATE1
	, HC_CALLER_ID_CONFIG_STATE2
	, HC_CALLER_ID_CONFIG_STATE3
	, HC_DELAY_BUFFER_CONFIG_STATE1
	, HC_DELAY_BUFFER_CONFIG_STATE2
	, HC_DELAY_BUFFER_CONFIG_STATE3
	, HC_LEC_INIT_STATE
	, HC_LEC_CONFIG_STATE
	, HC_LEC_CONFIG_STATE1
	, HC_LEC_ENABLE_STATE
	, HC_STOP_STATE
}HC_STATE;


#define HC_WAIT_CODEC_RESET_STATE 7

#define HC_START_RUN_TIME	1000



#define HC_DBG_BUFF_SIZE	200
#define HC_DBG_START		1
#define HC_DBG_ROLLOVER		2

typedef enum {HC_DBG_DATA8,
			HC_DBG_DATA32
			}HC_DEBUG_TYPE;

//#define HPF_300HZ	1
#ifdef AUDIO_MUX_OLD_VOLUME
#define HC_MUX_VOL_MUTE	0
#define HC_MUX_VOL_ON	1
#else
#define HC_MUX_VOL_MUTE	HD_MUX_VOL_MUTE
#define HC_MUX_VOL_ON	HD_MUX_VOL_ON
#endif

			 


#define HD_EVENT_TEL_FRAME	3 //Telephone event type

#define HC_CPU_LOAD_MAX	90 
#define HC_CPU_LOAD_MAX_COUNT 5//number of frame we have cpu load befor reporting it
#define HC_CPU_LOAD_MAX_RESAND 1000//(10sec) number of frame before resending CPU_LOAD message


typedef struct channel_events_buff_type
{
	uint8_t	size; //size of event block  
	uint8_t* pointer; //pointer to events block
}channel_events_buff_type;

typedef struct dsp_error_status
{
	uint32_t dsp_error[2];
	uint16_t host_audio_error_count; //pointer to events block
}dsp_error_status;

typedef struct HC_encrypt_config
{
	uint32_t type;
	uint32_t key[4];
	uint32_t reserved[4];
} HC_encrypt_config;

extern HD_host_dsp_ctrl hd_host_ctrl;
/**************************************************************************/                     
//  Module		: background
//  Description : IDL routine, handle host request
/// @return none
/// @param	none
/**************************************************************************/
void background(void);

/**************************************************************************/                     
//  Module		: host_ctrl_audio_enable
//  Description : freeze current state of enabling fields in hd_host_ctrl 	
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_enable(void);

/**************************************************************************/                     
//  Module		: host_ctrl_audio_configuration
//  Description : init and configure audio process according to host request 	
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_configuration(void);

/**************************************************************************/                     
//  Module		: init_host_control
//  Description : initialize host control struct
/// @return none
/// @param	none
/**************************************************************************/
void init_host_ctrl(void);

/**************************************************************************/                     
//  Module		: host_ctrl_audio_update
//  Description : handle host request
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_update(void);

/**************************************************************************/
//  Module		: init_host_control
//  Description : initialize host control struct
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl(uint16_t board);

/**************************************************************************/                     
//  Module		: debug_host_ctrl
//  Description : for debug imitate host configuration in runtime
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl_audio_update(uint16_t board);

/**************************************************************************/                     
//  Module		: host_ctrl_gpio_update
//  Description : update host with gpio value or set gpio value according 
//		to host request
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_gpio_update(void);

/**************************************************************************/                     
//  Module		: debug_host_ctrl_init
//  Description : initialize static
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl_init(void);

/**************************************************************************/                     
//  Module		: bit_status_events
//  Description : check if new error flag were set if yes send event to cpu
/// @return none
/// @param	none
/**************************************************************************/
void bit_status_events(void);


/**************************************************************************/                     
//  Module		: print_debug
//  Description : print data to debug buffer, add data to debug buffer
///				data is in the format 32bit size, 
/// @return none
/// @param	data: contain data to send, type: 8 or 32 bit data, 
///		size: data size, operation: HC_DBG_START_OVER 
/**************************************************************************/
int print_debug(void *data,uint32_t type,uint32_t size,int operation);

#endif //end HOST_CTRL_

