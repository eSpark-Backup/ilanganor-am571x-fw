/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : activate all audio process and synchronize the audio
///		capturing 
///	@file:	audio_processing.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	14/Jul/2008   Created	
//
/******************************************************************************/
#ifndef AUDIO_PROC_
#define AUDIO_PROC_
#include "DigitalCore_hpi.h"

#define AP_INDEX_IN		0
#define AP_INDEX_OUT	1

#define AP_QUEUE_IN		0
#define AP_QUEUE_OUT	1



#define AP_PROCESS_FINISH	0

#define AP_BLOCK		0
#define AP_NON_BLOCK	1

//max number of diffrent codec
#define MAX_DEV_FUNCTION	6
#define MAX_CH_DEV_FUNCTION	HD_MAX_CODEC_CHANNELS

#define HC_LEC_TYPE	1
#define HC_EC_TYPE	2



#define DEV_MCBSP0	0
#define DEV_MCBSP1	1
#define DEV_I2C0	2
#define DEV_I2C1	3

#define CODEC_CHANNEL_MASK	0xfffff
#define COUNT_PER_10MS	614400

typedef struct AP_channel
{
	uint32_t num; 	
	int16_t *audio_pointer;
} AP_channel;

typedef struct AP_input_channel_queue
{
	uint16_t ch_num[HD_MAX_CHANNELS]; 	
	uint32_t index_in;
	uint32_t index_out;	
} AP_input_channel_queue;




typedef struct AP_audio_local_ctrl	
{
	HD_BOOL	loopback_enable[HD_MAX_CODEC_CHANNELS];

	HD_BOOL vocoder_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL vocoder_vad_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL vocoder_fe_enable_mask[HD_MAX_CHANNELS][2];
	
	HD_BOOL	cd_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL	agc_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL filter_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL nr_enable_mask[HD_MAX_CHANNELS][2];
	HD_BOOL ec_enable_mask[HD_MAX_CHANNELS];
	HD_BOOL lec_enable_mask[HD_MAX_CHANNELS];
	HD_BOOL caller_id_enable[HD_MAX_CHANNELS];//contain the caller id channel number for every audio channel 
	HD_BOOL dtmf_det_enable[HD_MAX_CHANNELS];//contain the dtmf channel number for every audio channel
	HD_BOOL dly_buffer_enable_mask[HD_MAX_CODEC_CHANNELS][2];//enabled in config struct
	HD_BOOL encrypt_enable_mask[HD_MAX_HOST_DSP_CHANNELS][2];
} AP_audio_local_ctrl;

typedef struct AP_vocoder_config	
{
	int16_t vad_enable[HD_MAX_CHANNELS][2];
	int16_t frame_erase_enable[HD_MAX_CHANNELS][2];
	int16_t sub_type[HD_MAX_CHANNELS][2];
	int16_t reserved[HD_MAX_CHANNELS][2];
} AP_vocoder_config;



//parameter that need to be send to eny device open function
typedef struct AP_device_open_param	
{
	//port name (mcbsp0/1 i^c etc...)
	uint16_t port;
	//start channel
	uint16_t start_ch;
	//size - number of channel
	uint16_t size_ch;
	
}AP_device_open_param;

typedef struct AP_device_driver_func	
{
	uint16_t codec_id;
	uint16_t memo_size;//device memory size (normaly event memory)
	//pointer to open function
	int (*dev_open)(uint16_t port,uint16_t start_ch,uint16_t size_ch);
	//pointer to close function
	void (*dev_close)(uint16_t handle);
	//pointer to init function
	void (*dev_init)(void);
	//pointer to receive function
	int (*dev_get)(uint16_t,uint16_t,int16_t *);
	//pointer to send function
	int (*dev_send)(uint16_t,uint16_t,int16_t *);
	//pointer to control function
	int (*dev_ctrl)(uint16_t,uint16_t);
	//pointer to interrupt function
	void (*dev_isr)(void);
	//pointer to start function
	void (*dev_start)(void);	
	
}AP_device_driver_func;



typedef struct AP_device_channel_table	
{
	//port name (mcbsp0/1 i^c etc...)
	uint16_t handle;
	//start channel
	AP_device_driver_func* func;	
}AP_device_channel_table;


/**************************************************************************/                     
//  Module		: audio_process
//  Description : activate audio process and synchronize audio capture 
/// @return none
/// @param	none
/**************************************************************************/
void audio_process(unsigned int ch_mask);

/**************************************************************************/                     
//  Module		: audio_in_process
//  Description : receive an audio buffer from input channel and preform this
//					process: VOCODER, DTMF Detect, filter, Noise Reduction
//					Carrier Detect, AGC.	
/// @return none
/// @param	none
/**************************************************************************/
int audio_in_process(uint32_t channel,int16_t *speech_in);

/**************************************************************************/                     
//  Module		: audio_out_process
//  Description : receive an audio buffer from mux output and preform this
//					process: AGC, tone generation, VOCODER.	
/// @return none
/// @param	none
/**************************************************************************/
int audio_out_process(uint32_t channel,int16_t *speech_out);



/**************************************************************************/                     
//  Module		: inc_channel_queue
//  Description : increment channel queue
/// @return none
/// @param	none
/**************************************************************************/
int inc_channel_queue_in(int index_in_out);

/**************************************************************************/                     
//  Module		: read_channel_queue
//  Description : read channel queue
/// @return AP_channel (num, audio pointer) 	
/// @param	none
/**************************************************************************/
uint16_t read_channel_queue_in(int block);

/**************************************************************************/                     
//  Module		: write_channel_queue
//  Description : write channel queue
/// @return none
/// @param	none
/**************************************************************************/
int write_channel_queue_in(uint16_t ch_num);

/**************************************************************************/                     
//  Module		: init_audio_processing
//  Description : initialize and create audio processing modules
/// @return none
/// @param	none
/**************************************************************************/
void init_audio_processing(void);

/**************************************************************************/
//  Module		: init_devices
//  Description : initialize all devices
/// @return none
/// @param	none
/**************************************************************************/
void init_devices(void);

/**************************************************************************/                     
//  Module		: system_config
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
//void system_config(void);

/**************************************************************************/                     
//  Module		: get_device
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
int get_device(uint16_t ch_num, int16_t *audio);

/**************************************************************************/                     
//  Module		: send_device
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
int send_device(uint16_t ch_num,int16_t *audio);

/**************************************************************************/                     
//  Module		: ctrl_devices
//  Description : call ctrl device function for every opened device
/// @return none
/// @param	none
/**************************************************************************/
int ctrl_devices(void);

/**************************************************************************/                     
//  Module		: ctrl_device
//  Description : call ctrl device function 
/// @return none
/// @param	none
/**************************************************************************/
int ctrl_device(uint16_t ch_num);

/**************************************************************************/                     
//  Module		: edma_isr
//  Description : EDMA interrupt routine, call all registered device driver 
//					isr function
/// @return none
/// @param	none
/**************************************************************************/
void edma_isr(void);

/**************************************************************************/                     
//  Module		: start_devices
//  Description : start devices operation
/// @return none
/// @param	none
/**************************************************************************/
void start_devices(void);


extern AP_audio_local_ctrl audio_local_ctrl;
extern AP_vocoder_config vocoder_config;
#endif //end AUDIO_PROC_

