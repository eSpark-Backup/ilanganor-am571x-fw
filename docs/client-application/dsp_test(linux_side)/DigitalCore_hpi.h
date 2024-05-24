/******************************************************************************/
//                         	TechMer Ltd.
//                  5, Hatzoref st., Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : Definition of HPI channel interface between Host CPU(ARM)
///					and DSP.	  
///		 
///	@file:	DigitalCore_hpi.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	19/May/2008	  Created	
//
/******************************************************************************/
#ifndef _DIGITAL_CORE_HPI__H_
#define _DIGITAL_CORE_HPI__H_

//version of DigitalCore HPI interface
#define HD_INTERFACE_VERSION 8

//address location of control struct in DSP memory space.
#define HD_HPI_STRUCT_ADDR	0//0x0002b000
//max number of channels received from Host	(must be even)
#define HD_MAX_HOST_DSP_CHANNELS	48	 
//max number of channels received from codecs (must be even)
#define HD_MAX_CODEC_CHANNELS	20	
//max number of internal generated tone channel
#define HD_MAX_TONE_CHANNELS		16	
//starting channel number for host channel
#define HD_HOST_CHANNELS_BASE	20



#define HD_MAX_CHANNELS	(HD_MAX_HOST_DSP_CHANNELS + HD_MAX_CODEC_CHANNELS) 


#define HD_MAX_INFO_CHAR	64
#define HD_GPIO_NUM	8

#define HD_MAX_CALLER_ID_CHANNELS	16
#define HD_MAX_DTMF_DETECT_CHANNELS	16
#define HD_MAX_DLY_BUFFER_CHANNELS	16

//dsp error code for HD_dsp_status.dsp_error, each error indicate by different bit 
#define HD_DSP_ERR_SYS_CONFIG		1
#define HD_DSP_ERR_MCBSP0_CODEC		2
#define HD_DSP_ERR_MCBSP1_CODEC		4
#define HD_DSP_ERR_NO_INPUT_CH 		8
#define HD_DSP_ERR_HOST_FRAME		0x10 //problem in host frame: ch number or frame size not correct or not enough space 
#define HD_DSP_ERR_MEMO_ALLOC		0x20
#define HD_DSP_ERR_MCBSP0_SYNC_CH	0x40
#define HD_DSP_ERR_MCBSP1_SYNC_CH	0x80
#define HD_DSP_ERR_SYS_STACK		0x100
#define HD_DSP_ERR_TASK_STACK		0x200
#define HD_DSP_ERR_HOST_CTRL_MEMO	0x400
#define HD_DSP_ERR_AUDIO_MEMO		0x800
#define HD_DSP_ERR_MCBSP0_NO_AUDIO	0x1000
#define HD_DSP_ERR_MCBSP1_NO_AUDIO	0x2000
#define HD_DSP_AUDIO_TEST			0x40000


//data_type field
#define HD_AUDIO_TYPE		0
#define HD_DTMF_TYPE		1
#define HD_TELEPHONY_TYPE	2

//tone mux define
#define HD_TONE_AUDIO_MUX	0
#define HD_TONE_ONLY_MUX	1

//mux
#define HD_MUX_VOL_MUTE		(-127)
#define HD_MUX_VOL_ON		(0)
#define HD_MUX_VOL_SIDETONE	(-6)

//put/set/clr codec register
#define HD_CLR_REG	0
#define HD_SET_REG	2
#define HD_PUT_REG	3
//macro to put, set or clr codec register. reg num-register address,
//data- for set and clr it is used as a mask that define the bits to set or clr, 
//for put it is the data to put in the register
#define HD_reg_clr(reg_num,data)	(data|((reg_num|(HD_CLR_REG<<6))<<8))
#define HD_reg_set(reg_num,data)	(data|((reg_num|(HD_SET_REG<<6))<<8))
#define HD_reg_put(reg_num,data)	(data|((reg_num|(HD_PUT_REG<<6))<<8)) 

//audio codec config
//aic24
#define HD_AIC24_SET_BAND_FILTER	0x8360
#define HD_AIC24_CLR_BAND_FILTER	0x0360
#define HD_AIC24_D_SIDETONE_9		0x0a07
#define HD_AIC24_D_SIDETONE_MUTE	0x8a07
//aic14
#define HD_AIC14_D_SIDETONE_6		0x0730
#define HD_AIC14_D_SIDETONE_MUTE	0x8738

#define HD_AC_CONF_NO_CHANGE		0x7fff  //any field in ac_configuration with 
											//this value will not be change
#define HD_TEL_EVENT_MASK_NO_CHANGE	0xffff  //no change in event_mask in HD_tel_ctrl_ext
#define HD_TEL_CTRL_NO_CHANGE		0xff  //no change in HD_tel_ctrl_ext (except event_mask)										

#define HD_FILTER_MAX_CHANNELS 10 //max number of filter channels
#define HD_FILTER_MAX_BQ 5 //max bi-quad sections
//#define HD_BOOL uint32_t
#define HD_BOOL int32_t //enum {HD_FALSE=0,HD_TRUE}HD_BOOL;
#define HD_REQ HD_BOOL
#define HD_CHANNEL_MASK uint32_t

#define HD_BOOL_BYTE int8_t

typedef enum {
HD_CH0 = 0,  HD_CH1,  HD_CH2,  HD_CH3,  HD_CH4,  HD_CH5,
HD_CH6,  HD_CH7,  HD_CH8,  HD_CH9,  HD_CH10,
HD_CH11,  HD_CH12,  HD_CH13,  HD_CH14,  HD_CH15,
HD_CH16,  HD_CH17,  HD_CH18,  HD_CH19,  HD_CH20,
HD_CH21,  HD_CH22,  HD_CH23,  HD_CH24,  HD_CH25,
HD_CH26,  HD_CH27,  HD_CH28,  HD_CH29,  HD_CH30,
HD_CH31,  HD_CH32,  HD_CH33,  HD_CH34,  HD_CH35,
HD_CH36,  HD_CH37,  HD_CH38,  HD_CH39,  HD_CH40,
HD_CH41,  HD_CH42,  HD_CH43,  HD_CH44,  HD_CH45,
HD_CH46,  HD_CH47,  HD_CH48,  HD_CH49,  HD_NO_CH=0xfffe,
HD_CH_ANY=0xffff 
} HD_CH_IDX;

typedef enum {HD_DAC=0, HD_ADC=1}HD_CH_DIR;
typedef enum {HD_DSP_TO_HOST=0, HD_HOST_TO_DSP=1}HD_HOST_DSP_DIR;
typedef enum {HD_OUT_OF_RESET=0,HD_POWER_UP_BIT, HD_SYSTEM_CONFIG, HD_READY,HD_INITIATE_BIT} HD_DSP_STATE;
typedef enum {HD_CH_IN_TYPE=0, HD_CH_OUT_TYPE,HD_CH_IN_OUT_TYPE}HD_CH_TYPE;
typedef enum {HD_PCM_FRAME=0, HD_G729_FRAME, HD_G711_FRAME, HD_AUTO_FRAME, HD_EVENT_FRAME,HD_UNKNOWN_FRAME = 0xFF } HD_FRAME_TYPE;
typedef enum {HD_CODEC_AIC14=1, HD_CODEC_AIC24, HD_CODEC_SI3056,HD_CODEC_MTR,HD_LED_A6812,HD_CODEC_AIC12}HD_CODEC_TYPE;
typedef enum {HD_VOCODER_PCM=0, HD_VOCODER_G729, HD_VOCODER_G711, HD_VOCODER_G726}HD_VOCODER_TYPE;
typedef enum {HD_FILTER_HP300HZ=1,HD_FILTER_DC_BLOCK=2,HD_FILTER_NAVY_HEADSET=4,HD_FILTER_CONFIGURED=8}HD_FILTER_TYPE;
typedef enum {HD_A_LAW=0, HD_U_LAW=1,HD_G726_16K,HD_G726_24K,HD_G726_32K,HD_G726_40K}HD_VOCODER_SUBTYPE;
typedef enum {HD_GPIO_IN=0, HD_GPIO_OUT=1}HD_GPIO_DIR;
typedef enum {HD_TEL_EVENT=0, HD_DSP_ERR_EVENT,HD_CALLER_ID_EVENT,HD_DTMF_DET_EVENT,HD_EVENT_TYPE_ALL=0xFFFF}HD_EVENT_TYPE;
typedef enum {HD_TEL_RING_EVENT=0,HD_TEL_STATUS_EVENT,HD_TEL_EVENT_SUB_TYPE_ALL=0xFFFF}HD_TEL_EVENT_SUB_TYPE;
typedef enum {HD_ERR_GLOBAL_EVENT=0,HD_ERR_HOST_EVENT,HD_ERR_LOAD_EVENT,HD_ERR_EVENT_SUB_TYPE_ALL=0xFFFF}HD_ERR_EVENT_SUB_TYPE;

typedef enum {HD_TEL_EXT=1,HD_LED_EXT, HD_DLY_EXT, HD_LEC_EXT,HD_FILTER_EXT}HD_EXTENSION_TYPE;
typedef enum {HD_TEL_EXT_POS=0,HD_LED_EXT_POS,HD_DLY_EXT_POS,HD_LEC_EXT_POS,HD_FILTER_EXT_POS, HD_MAX_EXT_POS=10}HD_EXTENSION_POSITION;
typedef enum {HD_VAD_NO_VOICE=0,HD_VAD_VOICE,HD_VAD_NOISE}HD_VAD_FLAG;
typedef enum {HD_DLY_MODE_IN_PTT_CD=0,HD_DLY_MODE_IN_PTT}HD_DLY_IN_MODE;
typedef enum {HD_DLY_MODE_OUT_CD=0,HD_DLY_MODE_OUT_TRANSMIT,HD_DLY_MODE_OUT_FULL}HD_DLY_OUT_MODE;
typedef enum {HD_ENCRYPT_AES128=0}HD_ENCRYPT_TYPE;
typedef enum {HD_LEC_NLP_OFF=0,HD_LEC_NLP_MUTE,HD_LEC_NLP_RAND_CNG,HD_LEC_NLP_HOTH,HD_LEC_NLP_SUPP,HD_LEC_NLP_SUPP_AUTO=6}HD_LEC_NLP_TYPE;

#define HD_DLY_STATUS_BUFFER_NOT_EMPTY	1
//telephone card definition
typedef enum {HD_TEL_POL=1,//lines polarity changed
			 HD_TEL_DLCS=2,//Delta Loop Current Sense changed 
			 HD_TEL_LCSO=4,//Loop Current Sense Overload changed
			 HD_TEL_DOD=8,//Drop Out Detect (line exist) changed
			 HD_TEL_BTD=0x10,//Billing Tone Detect changed
			 HD_TEL_FDT=0x20,//Frame Detect changed - if not set indicate a problem
			 HD_TEL_ROV=0x40,//Receive Overload changed
			 HD_TEL_RDTP=0x80,//Ring Detect bit changed
			 HD_TEL_CV=0x100,//Current/Voltage changed
			 HD_TEL_VOLT=0x200,//volt event(line exist)
			 HD_TEL_RING=0x400,//ring event
			 HD_TEL_REG=0x800,//defined register in si3056 changed
			 HD_TEL_RING_STATE=0x1000,//ring state (set constantly on tinging, 
			 						 //clear 5 sec after ring stop)
			 HD_TEL_ONHOOK_VOLT=0x2000,//onhook volt event set when parallel phone is onhook						 
			 HD_TEL_POL_CID=0x4000, //lines polarity changed and stable (may be use for caller id)
			 HD_TEL_RING_CID=0x8000, //RING pulse (may be use for caller id)
			 HD_TEL_AFTER_1RING_CID=0x10000 //set between first and second ring pulse(may be use for caller id)						 
			 						 
			 }TEL_SI3056_STATUS_TYPE;
//ring event info			 
#define HD_TEL_RING_ON			0x01 //this change on and off on ringing
#define HD_TEL_RING_STATE_ON	0x02 //this bit is constantly on when ringing

//type of caller id - depend on country 
typedef enum  { HD_NO_CALLER_ID,
				HD_CALLER_ID_ETSI_TYPE1, //v23 modem tone(europe), detect is made when telephone is on-hook
				HD_CALLER_ID_ETSI_TYPE2, //v23 modem tone(europe), detect is made when telephone is off-hook
				HD_CALLER_ID_BELL_TYPE1, //bell202 modem tone(usa), detect is made when telephone is on-hook
 				HD_CALLER_ID_BELL_TYPE2, //bell202 modem tone(usa), detect is made when telephone is off-hook
				HD_CALLER_ID_DTMF  //caller-id is dtmf signals (scandinavia)
			  }HD_CALLER_ID_TYPE;
//caller id detection mode. set the time to start checking for caller id signal			  
typedef enum  { HD_CALLER_ID_OFF,
				HD_CALLER_ID_ALLWAYS,
				HD_CALLER_ID_AFTER_FIRST_RING,				
				HD_CALLER_ID_AFTER_OFF_HOOK
			  }HD_CALLER_ID_DET_MODE;
			  
typedef enum  { HD_CALLER_ID_UNAVILABLE=1,
				HD_CALLER_ID_PRIVETE
			  }HD_CALLER_ID_ABSENCE;
			  
typedef enum  { HD_CALLER_ID_VOICE_CALL=1,
				HD_CALLER_ID_CLI_RING_BACK,
				HD_CALLER_ID_CALLING_NAME_DELIVER,
				HD_CALLER_ID_MESSAGE_CALL				
			  }HD_CALLER_ID_CALL_TYPE;
			  
typedef enum  { HD_DTMF_0=0,
				HD_DTMF_1,
				HD_DTMF_2,
				HD_DTMF_3,
				HD_DTMF_4,
				HD_DTMF_5,
				HD_DTMF_6,
				HD_DTMF_7,
				HD_DTMF_8,
				HD_DTMF_9,
				HD_DTMF_A,				
				HD_DTMF_B,
				HD_DTMF_C,
				HD_DTMF_D,
				HD_DTMF_STAR, 		//*
				HD_DTMF_NUM_SIGN	//#
																												
			  }HD_DTMF;			  				  			  				  			  
			  
typedef struct HD_version_info
{
	int8_t sVendorInfo[HD_MAX_INFO_CHAR];
	int8_t sDescription[HD_MAX_INFO_CHAR];
	int8_t sVersionTag[HD_MAX_INFO_CHAR];
	int16_t MajorVersionNumber;	
	int16_t MinorVersionNumber;
	int16_t BugfixReleaseNumber;
	int16_t hpi_interface_version;
}HD_version_info;

//host and dsp audio link via HPI channel
typedef struct HD_audio_link
{
	uint32_t dsp_host_buff_addr; 	//address of buffer send from DSP to ARM
	uint32_t host_read_flag_addr; //address of flag to indicate to DSP that 
								//ARM read the data	
	uint32_t host_read_flag; 	//content of the flag to signal that ARM 
								//read the data

	uint32_t host_dsp_buff_addr; 	//address of buffer send from ARM to DSP
	uint32_t host_write_flag_addr;  //address of flag to indicate to DSP that 
									//ARM finished writing data to DSP
	uint32_t host_write_flag;     	//content of the flag to signal that ARM 
									//write data to DSP	

	uint32_t dsp_buffer_size; //max size of audio buffer in DSP
		
}HD_audio_link;

////////////////////////////////////////////////////////////////////////////////
// system configuration - number of analog,digital and tone channel
typedef struct HD_channel_configuration
{
	uint16_t	number_of_mcbsp0_channel;		//number of codec in McBsp0 
	uint16_t	number_of_mcbsp1_channel;	//number of codec in McBsp1
	uint16_t	number_of_i2c0_channel;		//number of channel in i2c0
	uint16_t	number_of_i2c1_channel;		// number of channel in i2c1
	uint16_t	number_of_host_dsp_channel;	//number of channel transfer  between host and dsp
	uint16_t	number_of_tone_channel;		//number of internal generated tone channel
	uint32_t 	channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
	uint32_t 	channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
	HD_REQ HD_request;
}HD_channel_configuration;



////////////////////////////////////////////////////////////////////////////////
//
//
typedef struct HD_ac_initialization
{
	HD_BOOL channels_mask[HD_MAX_CODEC_CHANNELS];
	uint32_t codec_type;
	HD_CH_TYPE	ch_type;
	HD_REQ HD_request;
} HD_ac_initialization;


////////////////////////////////////////////////////////////////////////////////
// requests to configure an AC channel with the configuration parameters
// this configuration data structure used for all AC channels
typedef struct HD_ac_configuration 
{
	HD_BOOL channels_mask[HD_MAX_CODEC_CHANNELS];
	HD_BOOL analog_loopback;
	HD_BOOL digital_loopback;
	uint16_t powerdown;
	int16_t adc_gain;
	int16_t dac_gain;
	uint16_t codec_reg;
	HD_REQ HD_request;
} HD_ac_configuration;




////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the CD instance(s)
// TRUE value requests to initialize the given instance.
typedef struct HD_cd_initialization
{
	HD_BOOL channels_mask[HD_MAX_CHANNELS][2];
	HD_REQ 	HD_request;
} HD_cd_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling of the appropriate CD instances
// TRUE value enables the given instance
typedef struct HD_cd_enabling 
{
	HD_BOOL channels_mask[HD_MAX_CHANNELS][2];
	HD_REQ HD_request; 
} HD_cd_enabling;


////////////////////////////////////////////////////////////////////////////////
// requests to configure a CD instance with the configuration parameters
// this configuration data structures used for all CD instances
typedef struct HD_cd_configuration
{
	HD_BOOL channels_mask[HD_MAX_CHANNELS][2];
	int16_t threshold_level;
	int16_t start_duration;
	int16_t stop_duration;
	int16_t mute;
	HD_REQ HD_request;
} HD_cd_configuration;

////////////////////////////////////////////////////////////////////////////////
// update current channel CD status
typedef struct HD_cd_status
{
	uint16_t status[HD_MAX_CHANNELS][2];//current channel status
	uint32_t changed;//set by DSP if any cd status changed, clear by host
	HD_REQ HD_request;
}HD_cd_status;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the AGC instance(s)
// TRUE value requests to initialize the given instance
typedef struct HD_agc_initialization
{
	HD_BOOL channels_mask[HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_agc_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling of the appropriate AGC instances
// TRUE value enables the given instance,
typedef struct HD_agc_enabling
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_agc_enabling;

////////////////////////////////////////////////////////////////////////////////
// requests to configure an AGC instance with the configuration parameters
// this configuration data structures used for all AGC instances
typedef struct HD_agc_configuration 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	int16_t	cutoff_input_level;
	int16_t cutoff_offset;
	int16_t rotation_input_level;
	int16_t rotation_output_level;
	HD_REQ HD_request;
} HD_agc_configuration;

////////////////////////////////////////////////////////////////////////////////
// requests to configure a new AGC instance with the configuration parameters
// this configuration data structures used for all AGC instances
typedef struct HD_aec_agc_configuration
{
	HD_CHANNEL_MASK channels_mask [3][2];
	int16_t	txAgcMaxGainDB;
	int16_t txAgcMaxLossDB;
	int16_t txAgcTargetLeveldBm;
	int16_t txAgcLowSigThreshdBm;
	HD_REQ HD_request;
} HD_aec_agc_configuration;

////////////////////////////////////////////////////////////////////////////////
// requests to configure a new AGC instance with the configuration parameters
// this configuration data structures used for all AGC instances
typedef struct HD_aec_configuration
{
	HD_CHANNEL_MASK channels_mask [3][2];
	int32_t	maxAudioFreq;
		int16_t fixedBulkDelayMSec;
		//int16_t		variableBulkDelayMSec;	/*6*/
		//int16_t		initialBulkDelayMSec;	/*7*/
		int16_t 		activeTailLengthMSec;	/*8*/
		//int16_t 		totalTailLengthMSec;	/*9*/
		int16_t		txNLPAggressiveness;	/*10*/
		int16_t      maxTxLossSTdB;			/*11 */
		int16_t		maxTxLossDTdB;			/*12*/
		int16_t      maxRxLossdB;			/*13   ?????*/
		//int16_t		initialRxOutAttendB;	/*14*/
		//int16_t      targetResidualLeveldBm;	/*15  ???????*/
		//int16_t      maxRxNoiseLeveldBm;		/*16  ??????*/
		int16_t		worstExpectedERLdB;		/*17  ??????*/
		int16_t		rxSaturateLeveldBm;		/*18*/
	//	int16_t		cngEnable;				/*21*/
		//int16_t		fixedGaindB10;			/*22  ??????????*/
	//	int16_t		rxBypassEnable;			/*33*/
		//int16_t		maxTrainingTimeMSec;	/*34*/
		//int16_t		trainingRxNoiseLeveldBm;/*35*/
	HD_REQ HD_request;
} HD_aec_configuration;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the HPF instance(s)
// a TRUE value requests to initialize the given instance

typedef struct HD_filter_initialization 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	uint32_t type; //may be several separated by or
	HD_REQ HD_request;
} HD_filter_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling of the appropriate HPF instances
// TRUE value enables the given instance
typedef struct HD_filter_enabling 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_filter_enabling;



////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the G.729AB instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_vocoder_initialization 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	uint32_t type;	//vocoder type - VOCODER_G729, VOCODER_PCM, VOCODER_PCM
	HD_REQ HD_request;
} HD_vocoder_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling of the appropriate vocoder instances
// a TRUE value enables the given instance,
typedef struct HD_vocoder_enabling 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_vocoder_enabling;


////////////////////////////////////////////////////////////////////////////////
// requests to configure an VOCODER instance with the configuration parameters
// this configuration data structures used for all VOCODER instances
typedef struct HD_vocoder_configuration 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	int16_t	vad_enable;
	int16_t frame_erase_enable;
	int16_t sub_type;
	int16_t reserved;
	HD_REQ HD_request;
} HD_vocoder_configuration;






////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the NR instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_nr_initialization 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_nr_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling noise reduction functionality.
// TRUE value enables, FALSE disables
typedef struct HD_nr_enabling 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_nr_enabling;

typedef struct HD_nr_configuration 
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	int16_t attenuation; // attenuation of noise in dB; range [0;-14]
	int16_t hpf;		// switch on high pass filter [0/1]
	int16_t lpf; 		// switch on low pass filter [0/1]
	int16_t pause_attenuation; // attenuation of signal during pause in dB
	int16_t pause_roll_off; // pause roll off slope in dB/s; range [0;60]
	int16_t vox_off_level_high; // switch off pause when pause attenuation is on; range [0;70] -> -4 to +10 dB pseudo SNR
	int16_t vox_off_level_low; // switch off pause during countdown for pause attenuation; //range [0;70] -> -4 to +10 dB pseudo SNR
	int16_t vox_on_level; // switch on pause; range [0;70] -> -4 to +10 dB     pseudo SNR
	int16_t vox_delay; // countdown before pause attenuation starts; range [0;32768] -> 0 to 68.8 s [1 block is about 2.1 ms]
	int16_t reserved;
	HD_REQ HD_request;
} HD_nr_configuration;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the MUX instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_mux_initialization 
{
	HD_REQ HD_request;
} HD_mux_initialization;

typedef struct HD_mux_configuration 
{
	int8_t channel_level[HD_MAX_CHANNELS] [HD_MAX_CHANNELS]; // routing table each byte contain volume level of the input channel to be sum
	uint32_t reserved;
	HD_REQ HD_request;
} HD_mux_configuration;

typedef struct HD_tone_configuration{	
	uint32_t freq_hz[HD_MAX_TONE_CHANNELS]; // frequency of generated signal in Hz
	uint32_t reserved;
	HD_REQ HD_request;
} HD_tone_configuration;


typedef struct HD_tone_mux_configuration 
{
	uint8_t tones_number_a[HD_MAX_CHANNELS]; // number of one tone to sum to the output
	uint8_t tones_number_b[HD_MAX_CHANNELS]; // number of second tone to sum to the output
											
	uint8_t sum_to_speech[HD_MAX_CHANNELS];//set to true to sum tone and speech
											//set to false to set tone only	 
	uint8_t volume[HD_MAX_CHANNELS];//volume of tone
//	uint32_t duration[HD_MAX_CHANNELS];//tone duration in msec, 0 mean forever	 											
	HD_REQ HD_request;
} HD_tone_mux_configuration;


////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the echo cancellation instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_echo_c_initialization
{
	HD_BOOL_BYTE channels_mask [HD_MAX_CHANNELS];
	uint32_t tail_length; //length in samples of the filter
	HD_REQ HD_request;
} HD_echo_c_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling echo cancellation functionality.
// TRUE value enables, FALSE disables
typedef struct HD_echo_c_enabling
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS];
	HD_REQ HD_request;
} HD_echo_c_enabling;

typedef struct HD_echo_c_configuration{
	HD_BOOL_BYTE channels_mask [HD_MAX_CHANNELS];
	int16_t attenuation; //attenuation in db(negative)
	int16_t att_near_active; //attenuation in db(negative) when near end active
	int16_t echo_ch; //output channel that cause the echo (speaker)
	int16_t echo_ch_dir;//echo channel direction in or out
	int16_t reserved[4];
	HD_REQ HD_request;
} HD_echo_c_configuration;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the line echo cancellation instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_line_echo_c_initialization
{
	HD_BOOL_BYTE channels_mask [HD_MAX_CHANNELS];
	uint32_t tail_length; //length in samples of the filter, (tail =< 256)
	HD_REQ HD_request;
} HD_line_echo_c_initialization;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the line echo cancellation instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_aec_initialization
{
	HD_CHANNEL_MASK channels_mask [3];
	HD_REQ HD_request;
} HD_aec_initialization;

////////////////////////////////////////////////////////////////////////////////
// configure line echo cancellation functionality.
typedef struct HD_line_echo_c_configuration{
	HD_BOOL_BYTE channels_mask [HD_MAX_CHANNELS];
	int16_t echo_ch; //output channel that cause the echo
	int16_t echo_ch_dir;//echo channel direction in or out
	HD_REQ HD_request;
} HD_line_echo_c_configuration;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling line echo cancellation functionality.
// TRUE value enables, FALSE disables
typedef struct HD_line_echo_c_enabling
{
	HD_BOOL_BYTE channels_mask [HD_MAX_CHANNELS];
	HD_REQ HD_request;
} HD_line_echo_c_enabling;


typedef struct HD_caller_id_configuration{
	uint16_t channel[HD_MAX_CALLER_ID_CHANNELS]; //input channel associated to caller id channel, 
												//put HD_NO_CH for not using this channel
	uint8_t type[HD_MAX_CALLER_ID_CHANNELS]; //type of caller ID - HD_CALLER_ID_TYPE
	uint8_t detection_mode[HD_MAX_CALLER_ID_CHANNELS]; //when to activate the caller id
	HD_REQ HD_request;
} HD_caller_id_configuration;


////////////////////////////////////////////////////////////////////////////////
// enabling and disabling acoustic echo cancellation functionality.
// TRUE value enables, FALSE disables
typedef struct HD_aec_enabling
{
	HD_CHANNEL_MASK channels_mask[3];
	HD_REQ HD_request;
} HD_aec_enabling;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling dtmf detection functionality.
// TRUE value enables, FALSE disables
typedef struct HD_dtmf_det_enabling
{
	HD_BOOL channel [HD_MAX_DTMF_DETECT_CHANNELS];//input channel associated to dtmf channel,
												//put HD_NO_CH for not using this channel
	HD_REQ HD_request;
} HD_dtmf_det_enabling;


typedef struct HD_gpio_configuration 
{
	HD_BOOL in_out[HD_GPIO_NUM];// dsp GPIO direction setting TRUE- output
	HD_REQ HD_request;
} HD_gpio_configuration;

typedef struct HD_gpio_status 
{
	uint32_t data_out; // host set output 
	uint32_t data_in; //host read input
	
	HD_REQ HD_request;	
} HD_gpio_status;

////////////////////////////////////////////////////////////////////////////////
// invokes the initialization function of the G.729AB instance(s)
// a TRUE value requests to initialize the given instance
typedef struct HD_encrypt_initialization
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	uint32_t type;	//Encryption type - ENCRYPT_AES128
	HD_REQ HD_request;
} HD_encrypt_initialization;

////////////////////////////////////////////////////////////////////////////////
// enabling and disabling of the appropriate vocoder instances
// a TRUE value enables the given instance,
typedef struct HD_encrypt_enabling
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	HD_REQ HD_request;
} HD_encrypt_enabling;


////////////////////////////////////////////////////////////////////////////////
// requests to configure an encrypt/decrypt instance with the configuration parameters
// this configuration data structures used for all encrypt/decrypt instances
typedef struct HD_encrypt_configuration
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS][2];
	uint32_t key[4];
	uint32_t reserver[4];
	HD_REQ HD_request;
} HD_encrypt_configuration;


typedef struct HD_dsp_status
{
	uint16_t	host_audio_error[HD_MAX_HOST_DSP_CHANNELS]; 	// channel id in audio buffer is not correct, host_audio_error must be cleared by host
	uint16_t	host_audio_error_count [HD_MAX_HOST_DSP_CHANNELS]; 	// count of host_audio_error
	uint16_t	codec_sync_lost_count [HD_MAX_CODEC_CHANNELS]; // Codec ID-Error counters
	uint16_t	codec_sync_lost; 			// Error on Codec-TDM -bus
	uint16_t	frame_count;		//frame number - stop if problem on reading audio
	uint16_t	codec_detected_mcbsp0; //number of codec detected in McBsp0	
	uint16_t	codec_detected_mcbsp1; //number of codec detected in McBsp1
	uint16_t	codec_detected_i2c0; //number of codec detected in i2c0
	uint16_t	codec_detected_i2c1; //number of codec detected in i2c1	 
	HD_DSP_STATE dsp_state;  //dsp state
	uint32_t dsp_error[2]; 	//64bits each bit indicate different error code
	uint32_t McBsp0_status; //Mcbsp0 status: 0-ok,1-fail 
	uint32_t McBsp1_status; //Mcbsp1 status: 0-ok,1-fail
	uint32_t memory_status;	//memory status: 0-ok,1-fail
	uint32_t i2c_0_status; //i2c 0 status: 0-ok,1-fail
	uint32_t i2c_1_status; //i2c 1 status: 0-ok,1-fail
	uint32_t cpu_load; //cpu load 0-100%
	uint16_t frame_buffer_size; //size of frame buffer (audio+events)
	uint16_t audio_frame_missed_count; //count of frame with frame_count difference not 1. do not increment counter if frame stops.

}HD_dsp_status;

typedef struct HD_dsp_bit
{
	uint32_t reserved[4]; //TBD will be use in initiated bit
	HD_REQ HD_request;
} HD_dsp_bit;






typedef struct HD_debug
{
	uint32_t debug_buffer_addr; 	//address of debug buffer
} HD_debug;

typedef struct HD_extension
{
	uint32_t buffer_addr[HD_MAX_EXT_POS]; 	//address of extension buffers
	uint32_t type[HD_MAX_EXT_POS];		 	//type of extension (HD_EXTENSION_TYPE)
	
} HD_extension;


// HOST shared structure
typedef struct HD_host_dsp_ctrl
{
	HD_version_info					sHD_version_info;
	HD_audio_link					sHD_audio_link;
	HD_channel_configuration		sHD_channel_configuration;
	HD_ac_initialization			sHD_ac_initialization;
	HD_ac_configuration				sHD_ac_configuration;
	HD_cd_initialization			sHD_cd_initialization;
	HD_cd_enabling					sHD_cd_enabling;
	HD_cd_configuration				sHD_cd_configuration;
	HD_cd_status					sHD_cd_status;
	HD_agc_initialization			sHD_agc_initialization;
	HD_agc_enabling					sHD_agc_enabling;
	HD_agc_configuration			sHD_agc_configuration;
	HD_filter_initialization		sHD_filter_initialization;
	HD_filter_enabling				sHD_filter_enabling;
	HD_vocoder_initialization		sHD_vocoder_initialization;
	HD_vocoder_enabling				sHD_vocoder_enabling;
	HD_vocoder_configuration		sHD_vocoder_configuration;
	HD_nr_initialization			sHD_nr_initialization;
	HD_nr_enabling					sHD_nr_enabling;
	HD_nr_configuration				sHD_nr_configuration;
	HD_mux_initialization			sHD_mux_initialization;
	HD_mux_configuration			sHD_mux_configuration;
	HD_tone_configuration			sHD_tone_configuration;
	HD_tone_mux_configuration		sHD_tone_mux_configuration;
	HD_echo_c_initialization		sHD_echo_c_initialization;
	HD_echo_c_enabling				sHD_echo_c_enabling;
	HD_echo_c_configuration			sHD_echo_c_configuration;
	HD_line_echo_c_initialization	sHD_line_echo_c_initialization;
	HD_line_echo_c_enabling			sHD_line_echo_c_enabling;
	HD_line_echo_c_configuration	sHD_line_echo_c_configuration;
	HD_caller_id_configuration		sHD_caller_id_configuration;
	HD_dtmf_det_enabling			sHD_dtmf_det_enabling;
	HD_gpio_configuration			sHD_gpio_configuration;
	HD_gpio_status					sHD_gpio_status;
	HD_encrypt_initialization		sHD_encrypt_initialization;
	HD_encrypt_enabling				sHD_encrypt_enabling;
	HD_encrypt_configuration		sHD_encrypt_configuration;
	HD_dsp_status					sHD_dsp_status;
	HD_dsp_bit						sHD_dsp_bit;
	HD_debug						sHD_debug;
	HD_extension					sHD_extension;
	HD_aec_enabling					sHD_aec_enabling;
	HD_aec_initialization 			sHD_aec_initialization;
	HD_aec_agc_configuration		sHD_aec_agc_configuration;
	HD_aec_configuration			sHD_aec_configuration;
	//uint8_t  sHD_padding[100];

}HD_host_dsp_ctrl;



//#pragma STRUCT_ALIGN (Host_Ctrl,HOST_ALIGN);

typedef struct HD_channels_frame_size
{
	uint32_t size;	//total size of data block (all channel struct including this one in byte) 
	uint32_t num_of_frames;	//number of data frames
} HD_channels_frame_size;

typedef struct HD_channel_frame
{
	uint32_t size;	//size of  channel frame 
	uint16_t data_type;	//data type for the channel
	uint16_t channel_index; //index of channel
} HD_channel_frame;

typedef struct HD_audio_g729_frame
{
	uint32_t frame_count; //count frames, running number
	uint16_t vad_flag;	//voice activity detect
	uint16_t carrier_detect;	//voice detected by CD module
	uint16_t reserved;
	uint8_t audio[10];	//audio content, 32bit alignment 

} HD_audio_g729_frame;

typedef struct HD_audio_g711_frame
{
	uint32_t frame_count; //count frames, running number
	uint16_t carrier_detect;	//voice detected by CD module
	uint16_t a_law_u_law;	//0=A-law format, 1=u-law format
	uint8_t audio[80];	//data type for the channel
} HD_audio_g711_frame;

typedef struct HD_audio_pcm_frame
{
	uint32_t frame_count; //count frames, running number
	uint16_t carrier_detect;	//voice detected by CD module
	uint16_t reserved;	
	uint8_t audio[160];	//data type for the channel
} HD_audio_pcm_frame;

typedef struct HD_tel_ctrl_ext
{
	HD_BOOL channels_mask[HD_MAX_CODEC_CHANNELS];
	uint8_t read_register_addr; //address of register to be read,  >60 no change
	uint8_t read_register_mask; //specified bit to be read in the register to reads
	uint8_t off_hook_state;	//set off hook state 0-on hook, 1-off hook, other-no change
	uint8_t ring_validate_off; //set ring validate (with default value). 0-enable, 1-disable, other-no change
	uint32_t event_mask; //define the bits that when changed an event message will be generated
						//HD_TEL_AFTER_1RING_CID,HD_TEL_RING_CID,HD_TEL_POL_CID,ONHOOK_VOLT,
						//ring_state,reg,ring,VOLT,CV,RDTP,ROV,FDT,BTD,DOD,LCSO,DLCSO,POL
						//put HD_TEL_EVENT_MASK_NO_CHANGE for no change
	
	HD_REQ HD_request;
}HD_tel_ctrl_ext;

typedef struct HD_tel_status_ext
{
	uint8_t	ring_status; //returned by dsp, ring detection status.  
	uint8_t read_register_data; //returned by dsp, last data from requester register
	uint16_t padding;
	uint32_t general_status; //HD_TEL_AFTER_1RING_CID,HD_TEL_RING_CID,HD_TEL_POL_CID,ONHOOK_VOLT,
						//ring_state,reg,ring,VOLT,CVI,RDTP,ROV,FDT,BTD,DOD,LCSO,DLCSO,POL
	
}HD_tel_status_ext;

//HD_TEL_EXT type definition
typedef struct HD_tel_ext
{
	HD_tel_ctrl_ext		ctrl;  
	HD_tel_status_ext 	status[HD_MAX_CODEC_CHANNELS];
}HD_tel_ext;


typedef struct HD_led_ctrl_ext
{
	HD_BOOL channels_mask[2];
	uint32_t led_state; //led status every bit define one led state
	uint32_t intensity; //led dimming low value low intensity 
	
	HD_REQ HD_request;
}HD_led_ctrl_ext;

typedef struct HD_dly_init_param
{
	uint16_t buffer_size; //size in 10msec step of the max audio buffer
						//set to 0 to close the delay channel and free dsp memory
	uint8_t in_mode;	//input working mode HD_DLY_IN_MODE
	uint8_t out_mode;	//output working mode HD_DLY_OUT_MODE
	uint8_t cd_channel; //channel to detect return signal (cd out mode only)
	uint8_t cd_in_out; //HD_DSP_TO_HOST or HD_HOST_TO_DSP
	uint16_t out_delay; //delay (10msec step) before sending audio out after
							//transmit or cd was received
}HD_dly_init_param;

typedef struct HD_dly_init_ext
{
	HD_BOOL channels_mask [HD_MAX_CODEC_CHANNELS][2];
	HD_dly_init_param param;
	HD_REQ HD_request;
}HD_dly_init_ext;

typedef struct HD_dly_ctrl_param
{

	uint8_t ptt; //ptt signal from host indicating start receiving,
				// audio and put in buffer(depend on mode)
	uint8_t transmit; //set by host to signal start sending audio
	uint16_t enable;
}HD_dly_ctrl_param;

typedef struct HD_dly_ctrl_ext
{
	HD_BOOL channels_mask [HD_MAX_CODEC_CHANNELS][2];
	HD_dly_ctrl_param param;
	HD_REQ HD_request;
}HD_dly_ctrl_ext;

//HD_dly_ext type definition
typedef struct HD_dly_ext
{
	HD_dly_init_ext init; //initialize delay buffer module
	HD_dly_ctrl_ext ctrl; //control delay buffer module
	uint16_t status[HD_MAX_CODEC_CHANNELS][2];//status of delay buffer module (see HD_DLY_STATUS_)
}HD_dly_ext;

typedef struct HD_lec_ext
{
	HD_BOOL channels_mask [HD_MAX_CHANNELS];

	int8_t nlp_type; //non linear process type
	int8_t adapt_enable;//lec will adapt or not
	int8_t bypass_enable;//enable or disable processing
	int8_t g165_enable;//enable tone detection g165

	int8_t double_talk_thres;//threshold for double talk condition
	int8_t max_double_talk_thres;//max threshold for double talk condition
	int8_t saturation_threshold;//
	int8_t nlp_threshold;//non linear process threshold

	int8_t nlp_upper_limit_thres_conv;//
	int8_t nlp_upper_limit_thres_unconv;//
	int8_t nlp_saturation_threshold;//
	int8_t nlp_max_supp;//max supresion for non linear process

	int8_t cng_noise_threshold;
	int8_t adapt_limit;
	int8_t rec_check_enable;//2 enable 0 disable
	int8_t reserved;

	HD_REQ HD_request;
}HD_lec_ext;

typedef struct HD_filter_ext
{
	uint16_t filter_channel;
	uint16_t data_channel;
	uint16_t in_out_channel;
	uint16_t reserved;
	uint16_t number_of_bq; //number of bi-quad sections
	uint16_t coeff_q_format; //q format ex: 30 for q30
	uint32_t coeff[HD_FILTER_MAX_BQ*5];//BQ coefficients b2,b1,b0,a1,a2

	HD_REQ HD_request;
}HD_filter_ext;

typedef struct HD_events_tel_ring
{
	uint8_t	ring_status; //returned by dsp, ring detection status.  
	uint8_t reserved[3];

}HD_events_tel_ring;

typedef struct HD_events_tel_status
{
	uint32_t general_status;//HD_TEL_AFTER_1RING_CID,HD_TEL_RING_CID,HD_TEL_POL_CID,ONHOOK_VOLT,
						//ring_state,reg,ring,VOLT,CVI,RDTP,ROV,FDT,BTD,DOD,LCSO,DLCSO,POL
	uint8_t read_register_data; //returned by dsp, last data from requester register
	uint8_t reserved[3];
}HD_events_tel_status;

typedef struct HD_event_frame
{
	uint16_t type; //HD_EVENT_TYPE, event type example: tel
	uint16_t sub_type;//example ring, status, fault (HD_TEL_EVENT_SUB_TYPE)	
} HD_event_frame;

typedef struct HD_events_dsp_err
{
	uint32_t dsp_error[2]; //dsp error bits 
}HD_events_dsp_err;
typedef struct HD_events_dsp_host_err
{
	uint32_t host_audio_error_count;
}HD_events_dsp_host_err;

typedef struct HD_events_dsp_load_err
{
	uint32_t cpu_load;
}HD_events_dsp_load_err;

typedef struct HD_events_caller_id
{
	char month;
	char day;
	char hour;
	char minute;
	char calling_id[20];//number of calling line (origin)
	char called_id[20];//number of the called party of a call(destination)
	char absense_calling_id;//Reason for Absence of Called Line Identity:unavaileble or private
	char calling_name[50];
	char absense_calling_name;//Reason for Absence of Called Line name:unavaileble or private
	char call_type;//may be Voice Call, CLI Ring Back when free call, Calling Name Delivery,
						//Message Waiting Call
	uint8_t error;	//error in caller id info
	uint8_t error_count;				
	
}HD_events_caller_id;

typedef struct HD_events_dtmf_detect
{
	uint32_t dtmf;//HD_DTMF type
	
}HD_events_dtmf_detect;

//maximum structure size
#define HD_MAX_STRUCT_SIZE	(sizeof(HD_mux_configuration))

#endif	//_DIGITAL_CORE_HPI__H_
