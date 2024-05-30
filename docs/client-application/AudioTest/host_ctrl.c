/*******************************************************************************
  MER RAC DSP Software
  Copyright (c) 2006 DSPECIALISTS GmbH Berlin,Germany                                    
                                                                      
  SYSTEM:       TI-C64XX
  FILE:         $RCSfile$                                              
  RELEASE:      $Name$      
  VERSION:      $Revision: 11$
  DATE:         $Date: ?????? ?????????? 03 ???????????? 2013 10:34:54$                                                
  AUTHOR:       maxim;$Author: ilan_r$                                                               
                                                                      
  DESCRIPTION:  Host HPI interface

  ========                                                              
  HISTORY:                                                              
				$Log$
				ilan_r - ?????? ?????????? 03 ???????????? 2013 10:34:54
				ver 1.12.16
				init PLC module on powerup
				ilan_r - ?????? ?????????? 06 ?????????????? 2013 10:39:05
				VER 1.12.4 Add encription, add cd status reading, change dc_block filter gain
				ilan_r - Sunday, September 29, 2013 2:57:25 PM
				ver 1.12.3 add navy headset filter, dc block filter, separate LEC and EC modules
				ilan_r - Monday, September 16, 2013 4:31:25 PM
				ver 1.12.3
				ilan_r - Monday, September 16, 2013 2:57:50 PM
				VER 1.12.3 include navy headset filter (1.6khz)
				ilan_r - Monday, September 02, 2013 5:33:07 PM
				ver 1.12.2 fix warning
				ilan_r - Thursday, August 22, 2013 3:17:23 PM
				ver 1.12.1 add dc block filter
				ilan_r - ?????? ?????????? 12 ?????? 2013 09:08:08
				clear enable flag for delay buffer when open
				ilan_r - ?????? ?????????? 24 ?????????? 2013 10:27:42
				ilan_r - ?????? ?????????? 18 ?????????? 2013 08:21:50
				add delay buffer
				ilan_r - ?????? ?????? 18 ???????????? 2013 13:37:40
				ilan_r - ?????? ?????????? 18 ?????????? 2012 13:38:26
				yan_b - ?????? ?????????? 06 ?????? 2012 09:38:32
				ilan_r - ?????? ?????????? 06 ?????? 2012 09:32:39
				Ver 1.11.10 change padding in DigitalCore_hpi.h in tel status extension, in host_ctrl change init for dtmf and caller id module (they are not active yet)
				ilan_r - ?????? ?????????? 06 ?????? 2012 09:14:46
				changes for caller id and dtmf detect
				ilan_r - ?????? ?????????? 19 ?????????? 2012 14:43:16
				remove caller id and dtmf detection (not in use and not final yet)
				ilan_r - ?????? ?????????? 19 ?????????? 2012 14:32:32
				update caller id and dtmf module 
				TODO: check if possible to activate DTMF and CALLER ID on the same channel
				ilan_r - ?????? ?????????? 19 ?????????? 2012 09:31:20
				ver 1.11.9, add caller id and dtmf detect modules, fix bug in host_ctrl - clearing tel struct before pointer init
				ilan_r - ?????? ?????????? 17 ?????????? 2012 09:17:44
				ilan_r - ?????? ?????????? 11 ?????????? 2012 11:07:56
				ilan_r - ?????? ?????? 09 ?????????? 2012 09:00:42
				ilan_r - ?????? ?????????? 14 ?????????? 2011 07:24:33
				ver 1.11.5 event for error is sent every time there is a new error in the last 10msec, add stack error and separate no audio sync error and codec (register) error
				ilan_r - ?????? ?????????? 17 ?????? 2011 18:18:32
				VER 1.11.4 cpu load event is sent only after 5 contiues fault as the CLK_gethtime function may return fault value
				ilan_r - ?????? ?????? 01 ???????????? 2010 12:00:59
				ilan_r - ?????? ?????? 30 ???????????? 2010 15:43:22
				Ver 1.11.0, add LEC module (adaptive digital rev 10.30 demo), fix bug of sending tel event and cpu load on power up
				ilan_r - ?????? ?????????? 02 ???????? 2010 13:47:52
				VER 1.10.4 fix audio+tont mux by clearing output channels after sending them
				ilan_r - ?????? ?????? 12 ?????????? 2010 07:18:02
				update MTR driver, adding delay buffer
				ilan_r - ?????? ?????????? 07 ?????????? 2010 13:30:16
				add MTR driver
				ilan_r - Sunday, October 11, 2009 2:11:34 PM
				v1.10.2 add in tel driver ring state event- the ring event may be for state or for ring on and off during ringing host_ctrl debug for echo cancelling was added
				ilan_r - Sunday, September 13, 2009 1:47:59 PM
				fix bugs: send events with correct channel number, call event status check from audio processing and not from background
				ilan_r - Tuesday, September 08, 2009 10:52:10 AM
				ver 1.9.5 update dsp status and bit fields in DigitalCore_hpi.h
				ilan_r - Thursday, September 03, 2009 9:20:20 AM
				ver1.9.4 add dsp error events for dsp_error,host frame error and high cpu load
				ilan_r - Monday, August 24, 2009 10:35:22 AM
				ver 1.9.0 add Noise Reduction support (speex)
				ilan_r - ?????? ?????????? 26 ???????? 2009 11:44:41
				v1.8.1 Changes in si3056 driver
				ilan_r - Monday, May 04, 2009 7:58:34 AM
				v1.7.2 changes in host_frame regarding bit
				ilan_r - Monday, April 27, 2009 7:13:42 AM
				allocate size for host buffer was increase by 2, some how g729_encoder write one more byte then it should so it couse writing one byte to the rcv buffer that come right after the trnsmit buffer
				ilan_r - Sunday, April 26, 2009 4:01:58 PM
				in host_frame if no frame arrive from arm it will be consider as a lost frame (or vad 0 frame between vad2 frame in g729) 
				heap was increase to support PCM 30ch
				ilan_r - Wednesday, April 22, 2009 4:10:08 PM
				adding optimization -o2, reading gpio will read output as well
				ilan_r - Sunday, March 29, 2009 10:13:45 AM
				fix vocoder initialize in host ctrl
				ilan_r - Monday, March 23, 2009 2:34:30 PM
				host channel that are sent to arm are set with the type defined in vocoder init
				ilan_r - Thursday, March 05, 2009 3:48:15 PM
				ver 1.5.12, fix problem of configuration new configuration after reset, set unused value to edma handle to prevent un opend device to be read or write
				ilan_r - Wednesday, March 04, 2009 7:56:12 AM
				Ver 1.5.11 fix mux problem (when muxing 2 source and one of them was set to low digital volume)
				ilan_r - Thursday, February 26, 2009 1:51:20 PM
				ver 1.5.10, include mux in db, fix mux with negative volume problem
				ilan_r - Tuesday, February 24, 2009 8:33:27 AM
				g711 that was received from host with carrier detect=0 is cleared, it is assumed to be junk data
				ilan_r - Monday, February 09, 2009 9:09:31 AM
				update aic24 gain to be more accurate
				ilan_r - Tuesday, February 03, 2009 10:40:56 AM
				fix aic24 gain problem of value greater then 43db or less then -43db.
				in host_frame send a low,u low on stream to host
				ilan_r - Thursday, January 22, 2009 1:11:21 PM
				ver 1.5.6
				ilan_r - Sunday, January 04, 2009 8:25:20 AM
				support event message for telephone, allocate memory for event by using memo_size field in device function description struct
				ilan_r - Wednesday, December 31, 2008 7:57:08 AM
				change OSF in codec registers aic24/14 to 512 to reduce noise, add in g729 vad hadeling by clear vad2 value and insert demo vad 2 frame between vad 1 to vad0 frame reception, this fix problem of constant noise once in a while while ptt is off.
				ilan_r - Tuesday, December 02, 2008 4:50:26 PM
				v5.1.2 fix vad problem
				ilan_r - Monday, December 01, 2008 12:42:49 PM
				ver 1.5.1 include restart after receiving channel config
				ilan_r - Monday, November 24, 2008 1:55:01 PM
				changes in DigitalCore_hpi.h - add vocoder configuration insted of vad and fe enable, add vocoder sub_type, add codec, vocoder type, move audio link to be after version
				ilan_r - Sunday, November 23, 2008 4:07:42 PM
				VERSION 1.4.7
				ilan_r - Sunday, November 23, 2008 1:51:02 PM
				change CH names start from CH0 insterd of CH1, add debubing in host_ctrl for field telephone card, enable 2 tones muxing
				ilan_r - Wednesday, October 22, 2008 2:04:07 PM
				ver 1.5.0 working in task and background, include new audio protocol to host, include g729
				ilan_r - Tuesday, October 07, 2008 8:10:43 AM
				ilan_r - Tuesday, October 07, 2008 7:18:00 AM
				add mute functionality for CD module. add check for audio processing module.
				ilan_r - Thursday, October 02, 2008 2:36:45 PM
				ver 1.4.2 set host channel as output (fix problem of playback)
				ilan_r - Sunday, September 28, 2008 9:32:36 AM
				ilan_r - Thursday, September 18, 2008 10:47:57 AM
				include sending audio to host(not checked yet)
				ilan_r - Thursday, September 04, 2008 6:35:07 PM
				change in tone_generate and tone_mux clear hd_request in tone muxing
				ilan_r - Monday, September 01, 2008 4:53:17 PM
				ilan_r - Tuesday, August 26, 2008 7:47:39 AM
				ilan_r - Sunday, August 17, 2008 3:46:41 PM
				ilan_r - Wednesday, August 13, 2008 11:25:31 AM
				ilan_r - Thursday, July 24, 2008 5:58:55 PM
				ilan_r - ?????? ?????????? 29 ?????? 2008 18:48:10
				change names to the new HD_host_dsp_ctrl struct
				ilan_r - ?????? ?????????? 29 ?????? 2008 13:22:15
				ilan_r - Wednesday, July 11, 2007 1:19:19 PM
				version 0.7 working version with hpi
				ilan_r - Tuesday, July 03, 2007 7:06:23 PM
				Ilan Rosilio  14/jun/2007     0.7     adding audio link by using hpi channel insted of SSP. data from codec is passed to a pipe buffer to the host, and data from host is put in a pipe buffer.
				the current buffer address is passed to the host by writing data to host_dsp_audio_link in hd_host_ctrl
				ilan_r - Wednesday, May 02, 2007 5:05:43 PM
				change in memory definition the internal memory size is only 0x40000 and not as it was define, host_control start address changed from 0xffc00 to 0x3fc00
				ilan_r - Monday, January 22, 2007 2:13:24 PM
				Add test normal -normal operation, add version info in hpi, change leds name, configure cd, adding define that if set (not set in defult) will enable AGC
				ilan_r - ?????? ?????????? 15 ???????????? 2006 15:00:43
				ilan_r - ?????? ?????????? 02 ???????????? 2006 13:55:26
				ilan_r - ?????? ?????????? 11 ?????????????? 2006 11:04:27
			                                                                      
*******************************************************************************/
/*#include <std.h>
#include <stdint.h>
#include <stdio.h>
#include <c6x.h>
#include <csl_gpio.h>
#include <csl.h>
#include <tsk.h>
#include <log.h>
#include <string.h>
#include <clk.h>

//#include "rac_dsp.h"
#include "BoardUtility\BoardUtility.h"*/

#include "DigitalCore_hpi.h"
#include "plc_fp.h"
#include "main.h"
/*#include "dspe_math_lib.h"
#include "G729AB_user.h"
//#include "g711\lowcfe.h"

#include "hpf.h"
#include "agc.h"
#include "cd.h"


#include "device_tel_si3056.h"
#include "preprocessing_speex.h"
#include "G168_echo_cancel.h"
#include "caller_id.h"


#include "dtmf_det.h"



#include "anc_dspecialists.h"*/
#include "tone_generate.h"
#include "filter_dc.h"
#include "host_ctrl.h"
#include "system_config.h"
#include "AudioCfg.h"
#include "tone_generate.h"
#include "audio_processing.h"
#include "delay_buffer.h"
#include "filter_navy_headset.h"
#include "host_frame.h"
#include <ti/sysbios/hal/Cache.h>
/*
#define    DSP_VERSION_TAG "Version 1.1.1.1" //Version Tag
#define     DSP_VERSION_MAJOR 2 // major version number
#define     DSP_VERSION_MINOR 1 // minor version number
#define     DSP_VERSION_BUGFIX 0 // bugfix index
#define     HPI_INTERFACE_VERSION  8// DSP CPU interface version*/
 // Host shared memory space
#pragma DATA_SECTION	(hd_host_ctrl,".host")

#ifndef OLD_AUDIO_INTERFACE
HD_host_dsp_ctrl	hd_host_ctrl =
{
	"-----",//Vendor Info
	"DigitalCore DSP SW",//description
	DSP_VERSION_TAG, //Version Tag
	DSP_VERSION_MAJOR, // major version number
	DSP_VERSION_MINOR, // minor version number
	DSP_VERSION_BUGFIX, // bugfix index
	HPI_INTERFACE_VERSION  // DSP CPU interface version
}; 
#else
HD_host_dsp_ctrl	hd_host_ctrl = 
{
	"TechMer Ltd., 5 Hatzoref str. Holon 58856 ISRAEL",//Vendor Info
	"DigitalCore DSP SW",//description
	"Changes in DigitalCore_hpi.h, include old audio protocol", //Version Tag
	1, // major version number
	4, // minor version number
	8, // bugfix index
	0  // reserved
};
#endif

//extern HD_tel_ext	*tel;
//table of pointer for channel events
channel_events_buff_type channel_events_buff[HD_MAX_CODEC_CHANNELS]={0};

static uint16_t state_host_ctrl=HC_SYS_CONFIG_STATE;
static uint16_t state_host_ctrl_update=HC_AC_CONFIG_STATE1;

extern HD_dly_ext DLYBUF_extension;;

uint32_t HC_vocoder_type[HD_MAX_CHANNELS][2];


uint16_t hc_start_run_time_flag=0;

HC_encrypt_config encrypt_config [HD_MAX_HOST_DSP_CHANNELS][2];

extern LowcFE_c g711_lc[HD_MAX_HOST_DSP_CHANNELS];

/*
extern G729ENC_ADT_Instance_t g729_enc_state[HD_MAX_HOST_DSP_CHANNELS];
extern G729ENC_ADT_Scratch_t g729_enc_scratch;
extern G729DEC_ADT_Instance_t g729_dec_state[HD_MAX_HOST_DSP_CHANNELS];
extern G729DEC_ADT_Scratch_t g729_dec_scratch;



extern HPF_Handle		hHPF[HD_MAX_CHANNELS];
extern AGC_Handle		hAGC[HD_MAX_CHANNELS][2];
extern CD_Handle		hCD[HD_MAX_CHANNELS][2];

extern SpeexPreprocessState *ps_den[HD_MAX_CHANNELS][2];
extern SpeexPreprocessState *ps_vad[HD_MAX_CHANNELS][2];
extern SpeexPreprocessState *ps_ec[HD_MAX_CHANNELS];

extern unsigned short cd_mute[HD_MAX_CHANNELS][2];

extern uint32_t filter_type[HD_MAX_CHANNELS][2];

extern GPIO_Handle hGpio;

extern AP_audio_local_ctrl audio_local_ctrl;
extern AP_vocoder_config vocoder_config;

extern uint16_t AP_frame_count;

extern uint32_t proc_time_s,start_run_time;




extern PS_nr_param nr_param[HD_MAX_CHANNELS][2];
extern PS_ec_param ec_param[HD_MAX_CHANNELS];
extern LEC_param lec_param[HD_MAX_CHANNELS];
extern HD_lec_ext	lec_ext;
extern HD_filter_ext	filter_ext;*/

extern SC_channel_config	ch_config;

//extern int g_first_time;

#define MUX_HDST_TO_RADIO1

//#define MUX_HDST_TO_RADIO2

//#define MUX_SPKR_TO_RADIO1	

//#define MUX_SPKR_TO_RADIO2

#define ENABLE_AUDIO_PROC


//volatile static int g_test1=0; 
int32_t g_test1=0; 

//static uint32_t dsp_error_0_old=0;
//static uint32_t dsp_error_1_old=0;
uint32_t HC_host_audio_error_count=0;

static dsp_error_status old_dsp_status;

HD_events_dsp_err dsp_err_event;
HD_events_dsp_host_err dsp_err_host_event;
HD_events_dsp_load_err dsp_err_load_event;

//extern far LOG_Obj LOG_trace;
#ifdef _DEBUG
#define SYS_STACK_ADDR	0x28840 //GBL_stackbeg
#else
#define SYS_STACK_ADDR	0x273b8	//GBL_stackbeg
#endif
//extern int16_t* p_host_audio_buff_rcv;
//extern int16_t* p_host_audio_buff_trn;

//unsigned char HC_leds_state=0;
//extern far int _stack;

uint32_t dsp_error[2];

#pragma DATA_SECTION	(HC_dbg_buffer,".ISRAM_CODE")
volatile uint32_t HC_dbg_buffer[HC_DBG_BUFF_SIZE];

//extern HD_dly_ext DLYBUF_extension;

//extern void c_int00(void);

//extern void CD_init(CD_Handle handle);
//extern void AGC_init(AGC_Handle handle);
//extern int HPF_init(HPF_Handle handle);

//extern int caller_id_config(int channel,uint16_t a_type,uint16_t a_detection_mode);


//volatile uint32_t HC_dbg_buffer[HC_DBG_BUFF_SIZE];
//char dbg_text[50];

/**************************************************************************/                     
//  Module		: background
//  Description : IDL routine, handle host request
/// @return none
/// @param	none
/**************************************************************************/
void background(void)
{
	volatile int i,ret;
	uint32_t stack_address;
	
	uint32_t background_time,proc_s_time;
	uint32_t background_time2;//,proc_s_time2;
	
	//check if enough time from start running passed 
	if (hc_start_run_time_flag)
	{
		//Check if too mach time passed without processing audio
	/*	proc_s_time=proc_time_s;
		//IRQ_globalDisable();
		background_time=CLK_gethtime();
		//IRQ_globalEnable();
		background_time2=CLK_gethtime();
		//GPIO0_PIN_SET;
		if (((background_time-proc_s_time)>(COUNT_PER_10MS*2))&&
			((background_time2-proc_s_time)>(COUNT_PER_10MS*2)))
		{
		    //ret=sprintf(dbg_text,"bg_time=%u, p_time=%u ,diff=%d\n",background_time,proc_s_time,background_time-proc_s_time);
		    //print_debug(dbg_text,HC_DBG_DATA8,ret,HC_DBG_START);
		    //ret=sprintf(dbg_text,"bg_time2=%u, p_time2=%u ,diff=%d\n",background_time2,proc_s_time2,background_time2-proc_s_time2);
		    //print_debug(dbg_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);
			//GPIO1_PIN_SET;
			//LOG_printf(&LOG_trace,"background_time=%u, proc_s_time=%u\n",background_time,proc_s_time);
			hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_NO_INPUT_CH;
			dsp_error[0] |= HD_DSP_ERR_NO_INPUT_CH;
			//ch_config.no_audio=TRUE;
		}*/

	}
	else
	{
		//background_time=CLK_getltime();
	//	if((background_time-start_run_time)>(HC_START_RUN_TIME))
		//	hc_start_run_time_flag=1;
	}		
	host_ctrl_gpio_update();
	
	//hd_host_ctrl.sHD_dsp_status.frame_count=AP_frame_count;
#ifdef DEBUG_STACK_MONITORING
/*
	HC_leds_state=hd_host_ctrl.sHD_dsp_status.dsp_error[0]>>6;

	if(AP_frame_count&0x40)
		HC_leds_state|=0x80;
	else
		HC_leds_state&(~0x80);
	
	leds_write(~HC_leds_state);
*/	
	//check if system stuck have less then 10 places
	stack_address=(uint32_t)&(_stack);
	
	//if ((*((uint32_t*)(SYS_STACK_ADDR+100))!=0xc0ffee)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&64)==0))
	if ((*((uint32_t*)(stack_address+100))!=0xc0ffee)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&HD_DSP_ERR_SYS_STACK)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_SYS_STACK;
		dsp_error[0] |= HD_DSP_ERR_SYS_STACK;
		//LOG_printf(&LOG_trace,"less then 100 places of system stack left\n");
	}

	//check hd_host_ctrl memory start is correct
	if ((strncmp((void*)&hd_host_ctrl.sHD_version_info,"-----",5))&&
		((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&HD_DSP_ERR_HOST_CTRL_MEMO)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_HOST_CTRL_MEMO;
		dsp_error[0] |= HD_DSP_ERR_HOST_CTRL_MEMO;
		//LOG_printf(&LOG_trace,"hd_host_ctrl memory start is not correct\n");	
			
	}
	
/*
	//check hd_host_ctrl memory debug is correct
	if ((hd_host_ctrl.sHD_debug.debug_buffer_addr!=0)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&512)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= 512;
		//LOG_printf(&LOG_trace,"hd_host_ctrl memory debug is not correct\n");	
			
	}
	
	//check start isram memory is correct
	if ((*((uint32_t*)(0x200))!=0)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&1024)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= 1024;
		//LOG_printf(&LOG_trace,"isram memory start is not correct\n");	
			
	}
*/	
	dsp_error[0] &= (~HD_DSP_ERR_AUDIO_MEMO);
	//check audio buffer1 end memory is correct
	if ((*((uint32_t*)((uint8_t*)p_host_audio_buff_rcv+(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)))!=0)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&HD_DSP_ERR_AUDIO_MEMO)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_AUDIO_MEMO;
		dsp_error[0] |= HD_DSP_ERR_AUDIO_MEMO;
		//LOG_printf(&LOG_trace,"audio rcv buffer end memory is not correct\n");	
			
	}
	//check audio buffer2 end memory is correct
	if ((*((uint32_t*)((uint8_t*)p_host_audio_buff_trn+(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)))!=0)&&((hd_host_ctrl.sHD_dsp_status.dsp_error[0]&HD_DSP_ERR_AUDIO_MEMO)==0))
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_AUDIO_MEMO;
		dsp_error[0] |= HD_DSP_ERR_AUDIO_MEMO;
		//LOG_printf(&LOG_trace,"audio trn buffer end memory is not correct\n");	
			
	}										
#endif	

		
}


/**************************************************************************/                     
//  Module		: host_ctrl_audio_enable
//  Description : freeze current state of enabling fields in hd_host_ctrl 	
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_enable(void)
{
	


	if(hd_host_ctrl.sHD_vocoder_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.vocoder_enable_mask,
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask,
				/*sizeof(audio_local_ctrl.vocoder_enable_mask)*/HD_MAX_CHANNELS*2*sizeof(HD_BOOL));

		hd_host_ctrl.sHD_vocoder_enabling.HD_request = 0;
	}



	if(hd_host_ctrl.sHD_cd_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.cd_enable_mask,
				hd_host_ctrl.sHD_cd_enabling.channels_mask,
				sizeof(audio_local_ctrl.cd_enable_mask));

		hd_host_ctrl.sHD_cd_enabling.HD_request = 0;
	}

	if(hd_host_ctrl.sHD_agc_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.agc_enable_mask,
				hd_host_ctrl.sHD_agc_enabling.channels_mask,
				sizeof(audio_local_ctrl.agc_enable_mask));

		hd_host_ctrl.sHD_agc_enabling.HD_request = 0;
	}

	if(hd_host_ctrl.sHD_filter_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.filter_enable_mask,
				hd_host_ctrl.sHD_filter_enabling.channels_mask,
				sizeof(audio_local_ctrl.filter_enable_mask));

		hd_host_ctrl.sHD_filter_enabling.HD_request = 0;
	}

	if(hd_host_ctrl.sHD_nr_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.nr_enable_mask,
				hd_host_ctrl.sHD_nr_enabling.channels_mask,
				sizeof(audio_local_ctrl.nr_enable_mask));

		hd_host_ctrl.sHD_nr_enabling.HD_request = 0;
	}
	
	if(hd_host_ctrl.sHD_echo_c_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.ec_enable_mask,
				hd_host_ctrl.sHD_echo_c_enabling.channels_mask,
				sizeof(audio_local_ctrl.ec_enable_mask));

		hd_host_ctrl.sHD_echo_c_enabling.HD_request = 0;
	}	

	if(hd_host_ctrl.sHD_line_echo_c_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.lec_enable_mask,
				hd_host_ctrl.sHD_line_echo_c_enabling.channels_mask,
				sizeof(audio_local_ctrl.lec_enable_mask));

		hd_host_ctrl.sHD_line_echo_c_enabling.HD_request = 0;
	}
	
	if(hd_host_ctrl.sHD_encrypt_enabling.HD_request)
	{
		memcpy(	audio_local_ctrl.encrypt_enable_mask,
				&hd_host_ctrl.sHD_encrypt_enabling.channels_mask[HD_HOST_CHANNELS_BASE],
				sizeof(audio_local_ctrl.encrypt_enable_mask));

		hd_host_ctrl.sHD_encrypt_enabling.HD_request = 0;
	}
}

/**************************************************************************/                     
//  Module		: host_ctrl_audio_configuration
//  Description : init and configure audio process according to host request 	
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_configuration(void)
{
	short int 	iChannel = 0;

	//AGC_Params  agc_params;
	//CD_Params   cd_params;


	/////////////////// Module Initialization ///////////////////

	if(hd_host_ctrl.sHD_vocoder_initialization.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_HOST_DSP_CHANNELS; iChannel++)
		{
			// process request
			if (hd_host_ctrl.sHD_vocoder_initialization.type==HD_VOCODER_G729)
			{

				if((hd_host_ctrl.sHD_vocoder_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_ADC])&&
					(hd_host_ctrl.sHD_vocoder_initialization.type==HD_G729_FRAME))
				{
					//G729AB_ADT_EncodeInit( &g729_enc_state[iChannel], &g729_enc_scratch);
				}
	
				if((hd_host_ctrl.sHD_vocoder_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_DAC])&&
					(hd_host_ctrl.sHD_vocoder_initialization.type==HD_G729_FRAME))
				{
					//G729AB_ADT_DecodeInit( &g729_dec_state[iChannel], &g729_dec_scratch);
				}
			}

			if ((hd_host_ctrl.sHD_vocoder_initialization.type==HD_VOCODER_G711)||
				(hd_host_ctrl.sHD_vocoder_initialization.type==HD_VOCODER_PCM)||
				(hd_host_ctrl.sHD_vocoder_initialization.type==HD_VOCODER_G726))
			{

				if(hd_host_ctrl.sHD_vocoder_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_ADC])
					
				{
					//constructor for the packet lost concealment(PLC)
					g711plc_construct(&g711_lc[iChannel]);
				}
	
			}
						
			if(hd_host_ctrl.sHD_vocoder_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_ADC])
				HC_vocoder_type[iChannel][HD_ADC]=hd_host_ctrl.sHD_vocoder_initialization.type;
				
			if(hd_host_ctrl.sHD_vocoder_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_DAC])
				HC_vocoder_type[iChannel][HD_DAC]=hd_host_ctrl.sHD_vocoder_initialization.type;
	
		}
		
		
		
		// reset request
		hd_host_ctrl.sHD_vocoder_initialization.HD_request = 0; 
	}	

	if(hd_host_ctrl.sHD_cd_initialization.HD_request)
	{
		/*
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_cd_initialization.channels_mask[iChannel][HD_ADC])
			{
				CD_init(hCD[iChannel][HD_ADC]);
			}

			if(hd_host_ctrl.sHD_cd_initialization.channels_mask[iChannel][HD_DAC])
			{
				CD_init(hCD[iChannel][HD_DAC]);
			}				
			
		}
		 */
		hd_host_ctrl.sHD_cd_initialization.HD_request = 0;
	}


	if(hd_host_ctrl.sHD_filter_initialization.HD_request)
	{
		// process request
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			if(hd_host_ctrl.sHD_filter_initialization.channels_mask[iChannel][HD_ADC])
			{
				if (hd_host_ctrl.sHD_filter_initialization.type&HD_FILTER_DC_BLOCK)
					filter_dc_init(iChannel, HD_ADC);
				if (hd_host_ctrl.sHD_filter_initialization.type&HD_FILTER_NAVY_HEADSET)
					filter_navy_headset_init(iChannel,HD_ADC);

				//if (hd_host_ctrl.sHD_filter_initialization.type&HD_FILTER_HP300HZ)
				//	HPF_init(hHPF[iChannel][HD_ADC]);
			//	filter_type[iChannel][HD_ADC]=hd_host_ctrl.sHD_filter_initialization.type;
			}

			if(hd_host_ctrl.sHD_filter_initialization.channels_mask[iChannel][HD_DAC])
			{
				if (hd_host_ctrl.sHD_filter_initialization.type&HD_FILTER_DC_BLOCK)
					filter_dc_init(iChannel, HD_DAC);
				//if (hd_host_ctrl.sHD_filter_initialization.type&HD_FILTER_HP300HZ)
				//	HPF_init(hHPF[iChannel][HD_DAC]);
			//	filter_type[iChannel][HD_DAC]=hd_host_ctrl.sHD_filter_initialization.type;
			}

		}

		hd_host_ctrl.sHD_filter_initialization.HD_request = 0;
	}


	if(hd_host_ctrl.sHD_agc_initialization.HD_request)
	{
	    veu_init(iChannel, HD_ADC);
/*

		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			if(hd_host_ctrl.sHD_agc_initialization.channels_mask[iChannel][HD_ADC])
			{
				AGC_init(hAGC[iChannel][HD_ADC]);
			}

			if(hd_host_ctrl.sHD_agc_initialization.channels_mask[iChannel][HD_DAC])
			{
				AGC_init(hAGC[iChannel][HD_DAC]);			
			}
		}
*/
		// reset request
		hd_host_ctrl.sHD_agc_initialization.HD_request = 0; 
	}
	
	if(hd_host_ctrl.sHD_mux_initialization.HD_request)
	{
		// reset request
		hd_host_ctrl.sHD_mux_initialization.HD_request = 0; 
	}			

	if(hd_host_ctrl.sHD_nr_initialization.HD_request)
	{
	    veu_init(iChannel, HD_ADC);
		// process request
		//init will be done in the configuration state, so we can work with speex or dspecialists anc
/*
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			if(hd_host_ctrl.sHD_nr_initialization.channels_mask[iChannel][HD_ADC])
			{
#ifdef	 ANC_DSPESIALISTS_MODULE
				anc_init(iChannel,HD_ADC);
#else
				denoise_init(iChannel,HD_ADC);
#endif
			}

			if(hd_host_ctrl.sHD_nr_initialization.channels_mask[iChannel][HD_DAC])
			{
#ifdef	 ANC_DSPESIALISTS_MODULE
				anc_init(iChannel,HD_DAC);
#else
				denoise_init(iChannel,HD_DAC);
#endif
			}
		}
*/
		// reset request
		hd_host_ctrl.sHD_nr_initialization.HD_request = 0; 
	}
	
	if(hd_host_ctrl.sHD_echo_c_initialization.HD_request)
	{
		// process request

		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			if(hd_host_ctrl.sHD_echo_c_initialization.channels_mask[iChannel])
			{
				//ec_param[iChannel].tail_length=hd_host_ctrl.sHD_echo_c_initialization.tail_length;
				//echo_cancel_init(iChannel);
			}
		}
		// reset request
		hd_host_ctrl.sHD_echo_c_initialization.HD_request = 0;
	}

	if(hd_host_ctrl.sHD_line_echo_c_initialization.HD_request)
	{
		// process request

		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			if(hd_host_ctrl.sHD_line_echo_c_initialization.channels_mask[iChannel])
			{
				//lec_param[iChannel].tail_length=hd_host_ctrl.sHD_line_echo_c_initialization.tail_length;
				//LEC_init();
			    veu_init(iChannel, HD_ADC);
			}

		}

		// reset request
		hd_host_ctrl.sHD_line_echo_c_initialization.HD_request = 0;
	}


	//delay_buffer configuration
	if(DLYBUF_extension.init.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CODEC_CHANNELS; iChannel++)
		{
			if (DLYBUF_extension.init.channels_mask [iChannel][HD_DAC])
			{
				if (DLYBUF_extension.init.param.buffer_size>0)
					DLYBUF_open(iChannel,HD_DAC
							,DLYBUF_extension.init.param);
				else
				{
					audio_local_ctrl.dly_buffer_enable_mask[iChannel][HD_DAC]=0;
					DLYBUF_close(iChannel,HD_DAC);
				}
			}
			if (DLYBUF_extension.init.channels_mask [iChannel][HD_ADC])
			{
				if (DLYBUF_extension.init.param.buffer_size>0)
					DLYBUF_open(iChannel,HD_ADC
						,DLYBUF_extension.init.param);
				else
				{
					audio_local_ctrl.dly_buffer_enable_mask[iChannel][HD_ADC]=0;
					DLYBUF_close(iChannel,HD_ADC);
				}
			}
		}
		// reset request
		DLYBUF_extension.init.HD_request = 0;

	}

	if(hd_host_ctrl.sHD_encrypt_initialization.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_HOST_DSP_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_encrypt_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_ADC])
				encrypt_config[iChannel][HD_ADC].type=hd_host_ctrl.sHD_encrypt_initialization.type;

			if(hd_host_ctrl.sHD_encrypt_initialization.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_DAC])
				encrypt_config[iChannel][HD_DAC].type=hd_host_ctrl.sHD_encrypt_initialization.type;

		}
		// reset request
		hd_host_ctrl.sHD_encrypt_initialization.HD_request = 0;
	}
	/////////////////// Module Configuration ///////////////////

	// AGC Configuration
	if(hd_host_ctrl.sHD_agc_configuration.HD_request)
	{
		// process request

	/*	agc_params.HD_AGC_cutoff_input_level		=
					hd_host_ctrl.sHD_agc_configuration.cutoff_input_level; 

		agc_params.HD_AGC_cutoff_offset			=  
					hd_host_ctrl.sHD_agc_configuration.cutoff_offset; 

		agc_params.HD_AGC_rotation_input_level  	=  
					hd_host_ctrl.sHD_agc_configuration.rotation_input_level; 

		agc_params.HD_AGC_rotation_output_level 	=  
					hd_host_ctrl.sHD_agc_configuration.rotation_output_level; */


		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			/*if(hd_host_ctrl.sHD_agc_configuration.channels_mask[iChannel][HD_ADC])
			{
				AGC_setup(hAGC[iChannel][HD_ADC],&agc_params);
			}

			if(hd_host_ctrl.sHD_agc_configuration.channels_mask[iChannel][HD_DAC])
			{
				AGC_setup(hAGC[iChannel][HD_DAC],&agc_params);			
			}*/
		}

		// reset request
		hd_host_ctrl.sHD_agc_configuration.HD_request = 0; 
	}	



		// CD Configuration
	if(hd_host_ctrl.sHD_cd_configuration.HD_request)
	{
		// process request
		
	/*	cd_params.HD_CD_start_duration = // not used
				hd_host_ctrl.sHD_cd_configuration.start_duration;
		cd_params.HD_CD_stop_duration = 
				hd_host_ctrl.sHD_cd_configuration.stop_duration;
		cd_params.HD_CD_threshold_level =  //dB 
				hd_host_ctrl.sHD_cd_configuration.threshold_level;
*/
	
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

		/*	if(hd_host_ctrl.sHD_cd_configuration.channels_mask[iChannel][HD_ADC])
			{
				CD_setup(hCD[iChannel][HD_ADC], &cd_params);
				cd_mute[iChannel][HD_ADC]=hd_host_ctrl.sHD_cd_configuration.mute;	
			}
			if(hd_host_ctrl.sHD_cd_configuration.channels_mask[iChannel][HD_DAC])
			{
				//debug for MTR
		//		if (iChannel>7)
		//		{
		//			cd_params.HD_CD_threshold_level =-35;
		//			cd_params.HD_CD_stop_duration=5;
					
		//		}	
				//END DEBUG	
				CD_setup(hCD[iChannel][HD_DAC], &cd_params);
				cd_mute[iChannel][HD_DAC]=hd_host_ctrl.sHD_cd_configuration.mute;		
			}	*/
		}

		hd_host_ctrl.sHD_cd_configuration.HD_request = 0; 
	}
	
		// VOCODER Configuration
	if(hd_host_ctrl.sHD_vocoder_configuration.HD_request)
	{

	
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_vocoder_configuration.channels_mask[iChannel][HD_ADC])
			{
				vocoder_config.vad_enable[iChannel][HD_ADC]=
					hd_host_ctrl.sHD_vocoder_configuration.vad_enable;
				vocoder_config.frame_erase_enable[iChannel][HD_ADC]=
					hd_host_ctrl.sHD_vocoder_configuration.frame_erase_enable;
				vocoder_config.sub_type[iChannel][HD_ADC]=
					hd_host_ctrl.sHD_vocoder_configuration.sub_type;

			}
			if(hd_host_ctrl.sHD_vocoder_configuration.channels_mask[iChannel][HD_DAC])
			{
				vocoder_config.vad_enable[iChannel][HD_DAC]=
					hd_host_ctrl.sHD_vocoder_configuration.vad_enable;
				vocoder_config.frame_erase_enable[iChannel][HD_DAC]=
					hd_host_ctrl.sHD_vocoder_configuration.frame_erase_enable;
				vocoder_config.sub_type[iChannel][HD_DAC]=
					hd_host_ctrl.sHD_vocoder_configuration.sub_type;
			}
		}

		hd_host_ctrl.sHD_vocoder_configuration.HD_request = 0; 
	}
	
		// NR Configuration
	if(hd_host_ctrl.sHD_nr_configuration.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
			//vox_delay - indicate we are using DSPECIALISTS ANC module and not the speex module
			if(hd_host_ctrl.sHD_nr_configuration.vox_delay)
			{

				if(hd_host_ctrl.sHD_nr_configuration.channels_mask[iChannel][HD_ADC])
				{
					//anc_init(iChannel,HD_ADC);
					//anc_config(iChannel,HD_ADC);
				}
				if(hd_host_ctrl.sHD_nr_configuration.channels_mask[iChannel][HD_DAC])
				{
					//anc_init(iChannel,HD_DAC);
					//anc_config(iChannel,HD_DAC);
				}
			}
			else
			{

		/*		if(hd_host_ctrl.sHD_nr_configuration.channels_mask[iChannel][HD_ADC])
				{
						denoise_init(iChannel,HD_ADC);
						nr_param[iChannel][HD_ADC].attenuation=hd_host_ctrl.sHD_nr_configuration.attenuation;
						nr_param[iChannel][HD_ADC].vad_enable=hd_host_ctrl.sHD_nr_configuration.vox_on_level;
						speex_preprocess_ctl(ps_den[iChannel][HD_ADC], SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &nr_param[iChannel][HD_ADC].attenuation);
				}
				if(hd_host_ctrl.sHD_nr_configuration.channels_mask[iChannel][HD_DAC])
				{
						denoise_init(iChannel,HD_DAC);
						nr_param[iChannel][HD_DAC].attenuation=hd_host_ctrl.sHD_nr_configuration.attenuation;
						nr_param[iChannel][HD_DAC].vad_enable=hd_host_ctrl.sHD_nr_configuration.vox_on_level;
						speex_preprocess_ctl(ps_den[iChannel][HD_DAC], SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &nr_param[iChannel][HD_DAC].attenuation);
				}*/
			}

		}

		hd_host_ctrl.sHD_nr_configuration.HD_request = 0; 
	}
	
		// Echo Cancel Configuration
	if(hd_host_ctrl.sHD_echo_c_configuration.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_echo_c_configuration.channels_mask[iChannel])
			{
	/*			if(ec_param[iChannel].init)
				{
					ec_param[iChannel].attenuation=hd_host_ctrl.sHD_echo_c_configuration.attenuation;   	
	   				ec_param[iChannel].att_near_active=hd_host_ctrl.sHD_echo_c_configuration.att_near_active; 
	   				ec_param[iChannel].echo_ch=hd_host_ctrl.sHD_echo_c_configuration.echo_ch; 
	   				ec_param[iChannel].echo_ch_dir=hd_host_ctrl.sHD_echo_c_configuration.echo_ch_dir;

					//configure EC module (speex)
   					speex_preprocess_ctl(ps_ec[iChannel], SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &ec_param[iChannel].attenuation);
   					speex_preprocess_ctl(ps_ec[iChannel], SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &ec_param[iChannel].att_near_active);
				}*/
			}
					
		}

		hd_host_ctrl.sHD_echo_c_configuration.HD_request = 0; 
	}

	// Echo Cancel Configuration
	if(hd_host_ctrl.sHD_line_echo_c_configuration.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_line_echo_c_configuration.channels_mask[iChannel])
			{

			    veu_config_ch(iChannel, HD_ADC);
			/*	if(lec_param[iChannel].init)
				{
					lec_param[iChannel].echo_ch=hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch;
					lec_param[iChannel].echo_ch_dir=hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch_dir;

					//configure lec module (adaptive digital init)
					LEC_config(iChannel);
				}*/
			}

		}

		hd_host_ctrl.sHD_line_echo_c_configuration.HD_request = 0;
	}

/*	if(lec_ext.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{

			if(lec_ext.channels_mask[iChannel])
				LEC_set_param(iChannel);
		}

		lec_ext.HD_request=0;//lec params is update using extension data on lec configuration
	}

	if(filter_ext.HD_request)
	{
		filter_config(filter_ext.filter_channel);
		filter_ext.HD_request=0;
	}*/

		// Caller ID Configuration
	if(hd_host_ctrl.sHD_caller_id_configuration.HD_request)
	{
		//clear enable struct for all channel
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{
		//	audio_local_ctrl.caller_id_enable[iChannel]=HD_NO_CH;
		}			
		for(iChannel=0; iChannel<HD_MAX_CALLER_ID_CHANNELS; iChannel++)
		{	
			//init caller_id
			/*caller_id_init(iChannel,hd_host_ctrl.sHD_caller_id_configuration.type[iChannel],
				hd_host_ctrl.sHD_caller_id_configuration.detection_mode[iChannel],
				hd_host_ctrl.sHD_caller_id_configuration.channel[iChannel]);	*/
			
			if (hd_host_ctrl.sHD_caller_id_configuration.channel[iChannel]<HD_MAX_CHANNELS)
			{
			//	audio_local_ctrl.caller_id_enable[hd_host_ctrl.sHD_caller_id_configuration.channel[iChannel]]
				//	=iChannel;
			}		
					
		}

		hd_host_ctrl.sHD_caller_id_configuration.HD_request = 0; 
	}
	
		// DTMF detection Configuration
	if(hd_host_ctrl.sHD_dtmf_det_enabling.HD_request)
	{
		//clear enable struct for all channel
		for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
		{			
	//		audio_local_ctrl.dtmf_det_enable[iChannel]=HD_NO_CH;
		}			
		for(iChannel=0; iChannel<HD_MAX_DTMF_DETECT_CHANNELS; iChannel++)
		{
			//init dtmf
			//dtmf_det_init(iChannel,hd_host_ctrl.sHD_dtmf_det_enabling.channel[iChannel]);
			if (hd_host_ctrl.sHD_dtmf_det_enabling.channel[iChannel]<HD_MAX_CHANNELS)
			{
	//			audio_local_ctrl.dtmf_det_enable[hd_host_ctrl.sHD_dtmf_det_enabling.channel[iChannel]]
		//			=iChannel;
			}		
					
		}

		hd_host_ctrl.sHD_dtmf_det_enabling.HD_request = 0; 
	}

	//delay_buffer configuration
	if(DLYBUF_extension.ctrl.HD_request)
	{
		for(iChannel=0; iChannel<HD_MAX_CODEC_CHANNELS; iChannel++)
		{
			//Update delay buffer param and enable struct
			if (DLYBUF_extension.ctrl.channels_mask [iChannel][HD_DAC])
			{
				DLYBUF_config(iChannel,HD_DAC,DLYBUF_extension.ctrl.param);
					audio_local_ctrl.dly_buffer_enable_mask[iChannel][HD_DAC]=
						DLYBUF_extension.ctrl.param.enable;
			}
			if (DLYBUF_extension.ctrl.channels_mask [iChannel][HD_ADC])
			{
				DLYBUF_config(iChannel,HD_ADC,DLYBUF_extension.ctrl.param);
				audio_local_ctrl.dly_buffer_enable_mask[iChannel][HD_ADC]=
						DLYBUF_extension.ctrl.param.enable;
			}
		}
		// reset request
		DLYBUF_extension.ctrl.HD_request = 0;
	}


	// Encrypt Configuration
	if(hd_host_ctrl.sHD_encrypt_configuration.HD_request)
	{


		for(iChannel=0; iChannel<HD_MAX_HOST_DSP_CHANNELS; iChannel++)
		{

			if(hd_host_ctrl.sHD_encrypt_configuration.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_ADC])
			{
				memcpy(encrypt_config[iChannel][HD_ADC].key,
					hd_host_ctrl.sHD_encrypt_configuration.key,sizeof(encrypt_config[iChannel][HD_ADC].key));
			}
			if(hd_host_ctrl.sHD_encrypt_configuration.channels_mask[iChannel+HD_HOST_CHANNELS_BASE][HD_DAC])
			{
				memcpy(encrypt_config[iChannel][HD_DAC].key,
					hd_host_ctrl.sHD_encrypt_configuration.key,sizeof(encrypt_config[iChannel][HD_DAC].key));
			}
		}

		hd_host_ctrl.sHD_encrypt_configuration.HD_request = 0;
	}

	if(hd_host_ctrl.sHD_cd_status.HD_request)
	{
		hd_host_ctrl.sHD_cd_status.changed=0;
		hd_host_ctrl.sHD_cd_status.HD_request=0;
	}

	if(hd_host_ctrl.sHD_ac_configuration.HD_request)
	{
	    hd_host_ctrl.sHD_ac_configuration.HD_request = 0;
	}
}

/**************************************************************************/                     
//  Module		: init_host_control
//  Description : initialize host control struct
/// @return none
/// @param	none
/**************************************************************************/
void init_host_ctrl(void)
{
	memset((void*)&hd_host_ctrl.sHD_audio_link,0,
		sizeof(hd_host_ctrl)-sizeof(HD_version_info));

	memset(channel_events_buff,0,sizeof(channel_events_buff));
	
	memset(HC_vocoder_type,0xff,sizeof(HC_vocoder_type));	
	
	
#ifndef AUDIO_MUX_OLD_VOLUME
	//set all volume to mute
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif
	hc_start_run_time_flag=0;
	
	memset((void*)&old_dsp_status,0,sizeof(old_dsp_status));
	HC_host_audio_error_count=0;
	
	memset((void*)&HC_dbg_buffer,0,sizeof(HC_dbg_buffer));
	hd_host_ctrl.sHD_debug.debug_buffer_addr=(uint32_t)&HC_dbg_buffer;
	
	
	dsp_error[0]=0;
	dsp_error[1]=0;
	
	//update the location of buffer in extension struct
//	hd_host_ctrl.sHD_extension.buffer_addr[HD_DLY_EXT_POS]=(uint32_t)&DLYBUF_extension;
	hd_host_ctrl.sHD_extension.type[HD_DLY_EXT_POS]=HD_DLY_EXT;

	//hd_host_ctrl.sHD_extension.buffer_addr[HD_FILTER_EXT_POS]=(uint32_t)&filter_ext;
	hd_host_ctrl.sHD_extension.type[HD_FILTER_EXT_POS]=HD_FILTER_EXT;
	
}



#ifdef DEBUG_HOST_CTRL
//#pragma CODE_SECTION(debug_host_ctrl,".ISRAM_CODE")
/**************************************************************************/                     
//  Module		: debug_host_ctrl_init
//  Description : initialize static
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl_init(void)
{
	state_host_ctrl=HC_SYS_CONFIG_STATE;
	state_host_ctrl_update=HC_AC_CONFIG_STATE1;


}

/**************************************************************************/                     
//  Module		: debug_host_ctrl
//  Description : for debug imitate host configuration
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl(uint16_t board)
{

	
	volatile Uint32 i; 
	
	
	switch(state_host_ctrl)
	{
		case HC_SYS_CONFIG_STATE:
		
			if (board==mACB_BOARD)
			{	
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=3;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=DEUBAG_HOST_CHANNEL;//3;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=4;//0;		//number of internal generated tone channel
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[0]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);//sizeof(HD_audio_g729_frame);//(NSAMPLES*2); 	//size of data frame for each channel(in byte)
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[1]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame); 	//size of data frame for each channel(in byte)
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[2]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame); 	//size of data frame for each channel(in byte)
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[0]=HD_G711_FRAME;//HD_G729_FRAME;//HD_PCM_FRAME;
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[1]=HD_PCM_FRAME;
//				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[2]=HD_PCM_FRAME;
				
				for (i=0;i<DEUBAG_HOST_CHANNEL;i++)
				{					
					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=DEBUG_HOST_CTRL_VOCODER;
					if (hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]==HD_PCM_FRAME)
						hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);
					else if (hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]==HD_G729_FRAME)
						hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
					else if (hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]==HD_G711_FRAME)
						hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);

				}
				
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=1;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				
				//Reset codec on power up
				hd_host_ctrl.sHD_gpio_status.data_out=0x70;
				hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;
	
				state_host_ctrl=HC_WAIT_CODEC_RESET_STATE;				
				
			}
			else if	(board==FIRE_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=2;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=4;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=2;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[0]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);//sizeof(HD_audio_g729_frame);//(NSAMPLES*2); 	//size of data frame for each channel(in byte)
				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[1]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame); 	//size of data frame for each channel(in byte)

				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[0]=HD_PCM_FRAME;
				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[1]=HD_PCM_FRAME;
	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=1;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				
				//Reset codec on power up
				hd_host_ctrl.sHD_gpio_status.data_out=0x70;
				hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;
	
				state_host_ctrl=HC_WAIT_CODEC_RESET_STATE;				
				
			}

			else if	(board==FIELD_TELEPHONY_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=1;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[0]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);//sizeof(HD_audio_g729_frame);//(NSAMPLES*2); 	//size of data frame for each channel(in byte)
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[1]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame); 	//size of data frame for each channel(in byte)

				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[0]=HD_PCM_FRAME;
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[1]=HD_PCM_FRAME;
	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}

			else if	(board==TELEPHONY_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=8;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<8;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G711_FRAME;
				}	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}	

			else if	(board==TELEPHONY_2_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=8;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=8;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<8;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G711_FRAME;
				}	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}	

			else if	((board==RADIO_BOARD)||(board==RADIO2_BOARD))
			{
				//channel configuration
				if	(board==RADIO2_BOARD)
				{
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=0;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=8;	//number of codec in McBsp1				
				
				}
				else
				{
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				}
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=8;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<8;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);

					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G729_FRAME;
				}	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}
			
			else if	(board==TELEPHONY_FIELD_TELEPHONY_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=8;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=1;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
				hd_host_ctrl.sHD_channel_configuration.channel_frame_size[0]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);//sizeof(HD_audio_g729_frame);//(NSAMPLES*2); 	//size of data frame for each channel(in byte)
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[1]=sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame); 	//size of data frame for each channel(in byte)

				hd_host_ctrl.sHD_channel_configuration.channel_frame_type[0]=HD_PCM_FRAME;
			//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[1]=HD_PCM_FRAME;
	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}
			
			else if	(board==MTR_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=4;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=4;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<4;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G711_FRAME;
				}	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=0;				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

	
				state_host_ctrl=HC_AC_INIT_STATE;				
				
			}	
			else if	(board==MACS_2R_1T_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=2;		//number of codec in McBsp0 
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=1;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=3;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<3;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G711_FRAME;
				}	
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
				
			
				
				
				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=HD_GPIO_IN;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=HD_GPIO_OUT;//RESET TEL CODEC
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=HD_GPIO_OUT;//RESET RADIO CODEC				
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				

				//Reset codec on power up
				hd_host_ctrl.sHD_gpio_status.data_out=0x40;
				hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;	
				state_host_ctrl=HC_WAIT_CODEC_RESET_STATE;				
				
			}
			else if (board==MACS_R_RADIO_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=8;		//number of codec in McBsp0
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=8;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
				for(i=0;i<8;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);

					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G729_FRAME;
				}
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;




				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=1;//Out, FS direction 0- codec generate the FS
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=1;//out, CLK direction 0- codec generate the CLK
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=0;
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=1;//reset codec
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				//Reset codec on power up
				hd_host_ctrl.sHD_gpio_status.data_out=0;
				hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;


				state_host_ctrl=HC_WAIT_CODEC_RESET_STATE;

			}
			else if (board==VT_BOARD)
			{
				//channel configuration
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=3;		//number of codec in McBsp0
				hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
				hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
				hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=3;	//number of channel transfer  between host and dsp
				hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=4;		//number of internal generated tone channel
				for(i=0;i<3;i++)
				{
					hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]=sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);

					hd_host_ctrl.sHD_channel_configuration.channel_frame_type[i]=HD_G711_FRAME;
				}
				hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;




				//configure GPIO
				hd_host_ctrl.sHD_gpio_configuration.in_out[0]=HD_GPIO_IN;//NC
				hd_host_ctrl.sHD_gpio_configuration.in_out[1]=HD_GPIO_IN;//NC
				hd_host_ctrl.sHD_gpio_configuration.in_out[2]=HD_GPIO_IN;//NC
				hd_host_ctrl.sHD_gpio_configuration.in_out[3]=HD_GPIO_OUT;//headset left shutdown, active low
				hd_host_ctrl.sHD_gpio_configuration.in_out[4]=HD_GPIO_IN;//NC
				hd_host_ctrl.sHD_gpio_configuration.in_out[5]=HD_GPIO_OUT;//headset right shutdown, active low
				hd_host_ctrl.sHD_gpio_configuration.in_out[6]=HD_GPIO_OUT;//speaker shutdown, active high
				hd_host_ctrl.sHD_gpio_configuration.in_out[7]=HD_GPIO_OUT;//codec reset, active low
				hd_host_ctrl.sHD_gpio_configuration.HD_request=TRUE;
				//Reset codec on power up
				hd_host_ctrl.sHD_gpio_status.data_out=0x40;
				hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;


				state_host_ctrl=HC_WAIT_CODEC_RESET_STATE;

			}
		break;	
		
		case HC_WAIT_CODEC_RESET_STATE:
			for (i=0;i<10000000;i++);
			hd_host_ctrl.sHD_gpio_status.data_out=0xf0;
			if (board==VT_BOARD)
				hd_host_ctrl.sHD_gpio_status.data_out=0xa9;
			hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;
			state_host_ctrl=HC_AC_INIT_STATE;
		break;

		case HC_AC_INIT_STATE:
			if (board==mACB_BOARD)
			{					
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	(board==FIRE_BOARD)
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_AC_INIT_STATE1;
			}
			else if	(board==FIELD_TELEPHONY_BOARD)
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[7]=TRUE;
				
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC24;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	((board==TELEPHONY_BOARD)||(board==TELEPHONY_FIELD_TELEPHONY_BOARD)||(board==TELEPHONY_2_BOARD))
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[7]=TRUE;
				
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_SI3056;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				if	(board==TELEPHONY_BOARD)
					state_host_ctrl=HC_STOP_STATE;
				else
					state_host_ctrl=HC_AC_INIT_STATE1;
			}

			else if	((board==RADIO_BOARD)||(board==RADIO2_BOARD))
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[7]=TRUE;
				
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC24;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	(board==MTR_BOARD)
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_MTR;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	(board==MACS_2R_1T_BOARD)
			{
					hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=1;
					hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=1;
					hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC24;
					hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
					hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;			
					state_host_ctrl=HC_AC_INIT_STATE1;				
			}
			else if	(board==MACS_R_RADIO_BOARD)
			{
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[7]=TRUE;

				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC12;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	(board==VT_BOARD)
			{
					hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=1;
					hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=1;
					hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=1;
					hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC12;
					hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
					hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
					state_host_ctrl=HC_STOP_STATE;
			}
		break;
		
		case HC_AC_INIT_STATE1:
			state_host_ctrl=HC_AC_INIT_STATE2;
		break;
		
		case HC_AC_INIT_STATE2:
			if	(board==FIRE_BOARD)
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC24;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	((board==TELEPHONY_FIELD_TELEPHONY_BOARD)||(board==TELEPHONY_2_BOARD))
			{				
				//ac initialize
				hd_host_ctrl.sHD_ac_initialization.channels_mask[8]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[9]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[10]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[11]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[12]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[13]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[14]=TRUE;
				hd_host_ctrl.sHD_ac_initialization.channels_mask[15]=TRUE;
				
				if(board==TELEPHONY_2_BOARD)
					hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_SI3056;
				else	
					hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC24;
				hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
				hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
				state_host_ctrl=HC_STOP_STATE;
			}
			else if	(board==MACS_2R_1T_BOARD)
			{
					hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=1;					
					hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_SI3056;
					hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
					hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;			
					state_host_ctrl=HC_STOP_STATE;				
			}

		break;
		
		case HC_STOP_STATE:
		
		break;

	}		
				
}

/**************************************************************************/                     
//  Module		: debug_host_ctrl
//  Description : for debug imitate host configuration in runtime
/// @return none
/// @param	none
/**************************************************************************/
void debug_host_ctrl_audio_update(uint16_t board)
{
	int i;
	static int j;
	switch(state_host_ctrl_update)
	{
		
		case HC_AC_CONFIG_STATE1:
			if (board==mACB_BOARD)
			{
				//ac configuration
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
			else if (board==FIRE_BOARD)
			{
				//ac configuration hdst codec aic14
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;//for hdst mic
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;	//boost
				//hd_host_ctrl.sHD_ac_configuration.dac_gain=-6;	//regular
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
				state_host_ctrl_update=HC_AC_CONFIG_STATE2;		
			}
			if (board==FIELD_TELEPHONY_BOARD)
			{
				//ac configuration
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[7]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=25;//15;//29;//32;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;//-3;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
			if ((board==TELEPHONY_BOARD)||
				(board==TELEPHONY_FIELD_TELEPHONY_BOARD))
				
			{
				//ac configuration
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[7]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE2;
			}
			if (board==TELEPHONY_2_BOARD)
			{
				//ac configuration
				for (i=0;i<16;i++)
					hd_host_ctrl.sHD_ac_configuration.channels_mask[i]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE2;
			}			
			if ((board==RADIO_BOARD)||(board==RADIO2_BOARD)||(board==MACS_R_RADIO_BOARD))
			{
				//ac configuration
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[6]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[7]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=17;//39;//16;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
			if (board==MTR_BOARD)
			{
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
			else if	(board==MACS_2R_1T_BOARD)
			{
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=1;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=1;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.powerdown=0;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=21;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.codec_reg=33632;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE2;
			}
			else if	(board==VT_BOARD)
			{
				hd_host_ctrl.sHD_ac_configuration.channels_mask[0]=1;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=1;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.powerdown=0;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=33;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-6;
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0x8904;//set register 6 (addr 9) to mic input (psedo diffrential)
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE2;
			}
		break;

		case HC_AC_CONFIG_STATE2:
			if (board==FIRE_BOARD)
			{
				//ac configuration spkr codec aic 14
				hd_host_ctrl.sHD_ac_configuration.channels_mask[1]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=3;//10; //for spkr mic
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-6;////3;//-7;//-7;	
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	

				state_host_ctrl_update=HC_AC_CONFIG_STATE3;
			}
			if (board==TELEPHONY_FIELD_TELEPHONY_BOARD)
			{
				//ac configuration
				hd_host_ctrl.sHD_ac_configuration.channels_mask[8]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[9]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[10]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[11]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[12]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[13]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[14]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[15]=TRUE;

				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=24;//32;//21;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;//-3;//0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE3;
			}
			if ((board==TELEPHONY_BOARD)||
				(board==TELEPHONY_FIELD_TELEPHONY_BOARD))
			{
				//extension configuration
				tel->ctrl.channels_mask[0]=TRUE;
				tel->ctrl.channels_mask[1]=TRUE;
				tel->ctrl.channels_mask[2]=TRUE;
				tel->ctrl.channels_mask[3]=TRUE;
				tel->ctrl.channels_mask[4]=TRUE;
				tel->ctrl.channels_mask[5]=TRUE;
				tel->ctrl.channels_mask[6]=TRUE;
				tel->ctrl.channels_mask[7]=TRUE;

				
				
				//ACT2,OHS,ACT,IIRE,res,res,RZ,RT (israel=10000000)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1080;
				//tel->ctrl.register_addr=16;
				//tel->ctrl.register_data=0x80;
				
				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=0;
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE3;
			}
			if (board==TELEPHONY_2_BOARD)
			{
				//extension configuration
				for (i=0;i<16;i++)
					tel->ctrl.channels_mask[i]=TRUE;
				
				//ACT2,OHS,ACT,IIRE,res,res,RZ,RT (israel=10000000)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1080;
				//tel->ctrl.register_addr=16;
				//tel->ctrl.register_data=0x80;
				
				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=0;
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE3;
			}				
			else if	(board==MACS_2R_1T_BOARD)
			{
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=1;				
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.powerdown=0;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
			else if	(board==VT_BOARD)
			{
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=1;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=0;
				hd_host_ctrl.sHD_ac_configuration.powerdown=0;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=0;
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-26;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
			}
		break;		
		
		case HC_AC_CONFIG_STATE3:
			if (board==FIRE_BOARD)
			{
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=15;//19;//9;//0;//20;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=-11;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
				state_host_ctrl_update=HC_AC_CONFIG_STATE4;//HC_MUX_CONFIG_STATE;		
			}
			if ((board==TELEPHONY_BOARD)||
				(board==TELEPHONY_FIELD_TELEPHONY_BOARD))
			{
				//extension configuration
				tel->ctrl.channels_mask[0]=TRUE;
				tel->ctrl.channels_mask[1]=TRUE;
				tel->ctrl.channels_mask[2]=TRUE;
				tel->ctrl.channels_mask[3]=TRUE;
				tel->ctrl.channels_mask[4]=TRUE;
				tel->ctrl.channels_mask[5]=TRUE;
				tel->ctrl.channels_mask[6]=TRUE;
				tel->ctrl.channels_mask[7]=TRUE;

				
				//DVC,DVC,MINI,MINI,res,res,ILIM,DCR (israel=11000010)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1ac2;
				//tel->ctrl.register_addr=26;
				//tel->ctrl.register_data=0xc2;

				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=0;
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE4;
			}
			
			if (board==TELEPHONY_2_BOARD)
			{
				//extension configuration
				for (i=0;i<16;i++)
					tel->ctrl.channels_mask[i]=TRUE;
				
				//DVC,DVC,MINI,MINI,res,res,ILIM,DCR (israel=11000010)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1ac2;
				//tel->ctrl.register_addr=26;
				//tel->ctrl.register_data=0xc2;

				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=0;
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_AC_CONFIG_STATE4;
			}									
		break;
		
		case HC_AC_CONFIG_STATE4:
			if (board==FIRE_BOARD)
			{
				//ac configuration radio codec aic24
				hd_host_ctrl.sHD_ac_configuration.channels_mask[2]=TRUE;
			//	hd_host_ctrl.sHD_ac_configuration.channels_mask[3]=TRUE;
			//	hd_host_ctrl.sHD_ac_configuration.channels_mask[4]=TRUE;
			//	hd_host_ctrl.sHD_ac_configuration.channels_mask[5]=TRUE;
				hd_host_ctrl.sHD_ac_configuration.analog_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.digital_loopback=FALSE;
				hd_host_ctrl.sHD_ac_configuration.powerdown=FALSE;
				hd_host_ctrl.sHD_ac_configuration.adc_gain=18;//19;//9;//0;//20;//assumed -10dbm(600ohm)
				hd_host_ctrl.sHD_ac_configuration.dac_gain=0;
				hd_host_ctrl.sHD_ac_configuration.HD_request=TRUE;	
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;		
			}
			if ((board==TELEPHONY_BOARD)||
				(board==TELEPHONY_FIELD_TELEPHONY_BOARD))
			{
				//extension configuration
				tel->ctrl.channels_mask[0]=TRUE;
				tel->ctrl.channels_mask[1]=TRUE;
				tel->ctrl.channels_mask[2]=TRUE;
				tel->ctrl.channels_mask[3]=TRUE;
				tel->ctrl.channels_mask[4]=TRUE;
				tel->ctrl.channels_mask[5]=TRUE;
				tel->ctrl.channels_mask[6]=TRUE;
				tel->ctrl.channels_mask[7]=TRUE;

				//res,res,res,res,ACIM,ACIM,ACIM,ACIM (israel=00000010)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1e02;
				//tel->ctrl.register_addr=30;				 
				//tel->ctrl.register_data=0x2;
				
				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=HD_TEL_RING;//HD_TEL_DOD;//
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_MUX_CONFIG_STATE;
				//clear ring status
				tel->status[0].ring_status=0;
				tel->status[1].ring_status=0;
				tel->status[2].ring_status=0;
				tel->status[3].ring_status=0;
				tel->status[4].ring_status=0;
				tel->status[5].ring_status=0;
				tel->status[6].ring_status=0;
				tel->status[7].ring_status=0;
			}
			if (board==TELEPHONY_2_BOARD)
			{
				//extension configuration
				for (i=0;i<16;i++)
				{
					tel->ctrl.channels_mask[i]=TRUE;
					//clear ring status
					tel->status[i].ring_status=0;
				}	

				//res,res,res,res,ACIM,ACIM,ACIM,ACIM (israel=00000010)
				hd_host_ctrl.sHD_ac_configuration.codec_reg=0xc000|0x1e02;
				//tel->ctrl.register_addr=30;				 
				//tel->ctrl.register_data=0x2;
				
				tel->ctrl.read_register_addr=0;
				tel->ctrl.off_hook_state=0;
				tel->ctrl.ring_validate_off=0;
				tel->ctrl.event_mask=HD_TEL_RING;//HD_TEL_DOD;//
				tel->ctrl.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE1;
				
			}						
		break;
		
			

		case HC_MUX_CONFIG_STATE:
			if (board==mACB_BOARD)
			{
				//mux initialize
				//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]
				//make loop
			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=1;
				//-hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=HC_MUX_VOL_ON;	

				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH20]=HC_MUX_VOL_ON;	
				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][HD_CH0]=HC_MUX_VOL_ON;	

			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH20]=1;
			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][HD_CH0]=1;
				/*
				for (i=0;i<12;i++)
				{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH3][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH5][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH6][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH7][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH8][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH9][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH10][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH11][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH12][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH13][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH14][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH15][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH16][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH17][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH18][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH19][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH21][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH22][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH23][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH24][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH25][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH26][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH27][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH28][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH29][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH30][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH31][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH32][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH33][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH34][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH35][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH36][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH37][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH38][i]=31;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH39][i]=31;				
				}
				*/	
				/*
				for (i=0;i<2;i++)
				{
					for (j=0;j<40;j++)
					{
						hd_host_ctrl.sHD_mux_configuration.channel_level[i][j]=31;
					}
				}
				*/										
				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH0]=7;
				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				#ifdef ENABLE_AUDIO_PROC
				state_host_ctrl_update=HC_AGC_CONFIG_STATE1;		
				#else
				state_host_ctrl_update=HC_STOP_STATE;
				#endif
			}
			else if (board==FIRE_BOARD)
			{
				//mux initialize
				//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]
				//make loop
				#ifdef MUX_HDST_TO_RADIO1
				//hdst to radio1				
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH3]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH0]=HC_MUX_VOL_ON;	
				#endif
				#ifdef MUX_HDST_TO_RADIO2
				//hdst to radio2
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH5]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][HD_CH0]=HC_MUX_VOL_ON;
				#endif
				#ifdef MUX_SPKR_TO_RADIO1	
				//spkr to radio1
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH3]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=HC_MUX_VOL_ON;
				#endif
				#ifdef MUX_SPKR_TO_RADIO2
				//spkr to radio2
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH5]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][HD_CH1]=HC_MUX_VOL_ON;
				#endif

			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH20]=1;
			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][HD_CH0]=1;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=HC_MUX_VOL_ON;


				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;
			}
			else if (board==FIELD_TELEPHONY_BOARD)
			{
				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH2]=HC_MUX_VOL_ON;

//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=HC_MUX_VOL_ON;
//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=HC_MUX_VOL_ON;
				
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][HD_CH5]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH5][HD_CH6]=HC_MUX_VOL_ON;
				
				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
				#ifdef ENABLE_AUDIO_PROC
				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;		
				#else
				state_host_ctrl_update=HC_STOP_STATE;
				#endif				
			}

			else if (board==TELEPHONY_BOARD)
			{
				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
	//			hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
	//			hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH2]=HC_MUX_VOL_ON;

//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=HC_MUX_VOL_ON;
//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=HC_MUX_VOL_ON;
				
//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][HD_CH5]=HC_MUX_VOL_ON;
//				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH5][HD_CH6]=HC_MUX_VOL_ON;
				
				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
//				#ifdef ENABLE_AUDIO_PROC
//				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;		
//				#else
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE1;//HC_STOP_STATE;
//				#endif	
				//state_host_ctrl_update=HC_VOCODER_CONFIG_STATE1;			
			}										
			else if ((board==RADIO_BOARD)||(board==RADIO2_BOARD))
			{
/*
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][HD_CH0]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH20]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH21][HD_CH1]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH21]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH22][HD_CH2]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH22]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH23][HD_CH3]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH3][HD_CH23]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH24][HD_CH4]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH4][HD_CH24]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH25][HD_CH5]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH5][HD_CH25]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH26][HD_CH6]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH6][HD_CH26]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH27][HD_CH7]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH7][HD_CH27]=HC_MUX_VOL_ON;
*/
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH20][HD_CH0]=HC_MUX_VOL_ON;
				
				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
//				#ifdef ENABLE_AUDIO_PROC
//				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;		
//				#else
				state_host_ctrl_update=HC_STOP_STATE;
				//state_host_ctrl_update=HC_TONE_CONFIG_STATE1;
//				#endif		
				//state_host_ctrl_update=HC_VOCODER_CONFIG_STATE1;
//				state_host_ctrl_update=HC_ECHO_CANCEL_CONFIG_STATE1;		
			}
			else if (board==TELEPHONY_FIELD_TELEPHONY_BOARD)
			{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH8]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH8][HD_CH0]=HC_MUX_VOL_ON;

			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH8][HD_CH9]=HC_MUX_VOL_ON;
			//	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH9][HD_CH10]=HC_MUX_VOL_ON;

				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
				#ifdef ENABLE_AUDIO_PROC
				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;		
				#else
				state_host_ctrl_update=HC_STOP_STATE;
				#endif				
			}
			
			else if (board==MTR_BOARD)
			{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=HC_MUX_VOL_ON;

				//hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=HC_MUX_VOL_ON;
				
				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
//				#ifdef ENABLE_AUDIO_PROC
//				state_host_ctrl_update=HC_TONE_CONFIG_STATE1;		
//				#else
				state_host_ctrl_update=HC_CD_CONFIG_STATE1;
//				#endif	
//				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE1;			
			}		
			else if (board==MACS_2R_1T_BOARD)
			{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=HC_MUX_VOL_ON;

				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
				
				state_host_ctrl_update=HC_DELAY_BUFFER_CONFIG_STATE1;//HC_STOP_STATE;
			
			}
			else if (board==MACS_R_RADIO_BOARD)
			{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH1]=HC_MUX_VOL_ON;

				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

				state_host_ctrl_update=HC_STOP_STATE;//HC_STOP_STATE;

			}
			else if (board==VT_BOARD)
			{
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=HC_MUX_VOL_ON;
				hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=HC_MUX_VOL_ON;

				hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

				state_host_ctrl_update=HC_AGC_CONFIG_STATE1;//HC_STOP_STATE;

			}
		break;
		/*
		case HC_TONE_CONFIG_STATE1:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//high gaurd tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_GUARD_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_GAURD_TONE_FREQ;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_HIGH_GAURD_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE2;		
			}
			if (board==FIELD_TELEPHONY_BOARD)
			{
				//high gaurd tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_GUARD_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_GAURD_TONE_FREQ;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_HIGH_GAURD_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_STOP_STATE;		
			}
			if (board==TELEPHONY_FIELD_TELEPHONY_BOARD)
			{
				//high gaurd tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[0]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_1000HZ_TONE_FREQ;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_STOP_STATE;		
			}						
		break;

		case HC_TONE_CONFIG_STATE2:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//low gaurd tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_LOW_GUARD_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_GAURD_TONE_FREQ;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_LOW_GAURD_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE3;		
			}
		break;

		case HC_TONE_CONFIG_STATE3:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//PTT F1 tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_PTT_F1_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_PTT_F1_COMAND;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_FUNC_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE4;		
			}
		break;

		case HC_TONE_CONFIG_STATE4:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//PTT F2 tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_PTT_F2_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_PTT_F2_COMAND;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_FUNC_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE5;		
			}
		break;

		case HC_TONE_CONFIG_STATE5:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//REPETER_DISABLE tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_REPETER_DISABLE_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_REPETER_DISABLE_COMAND;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_FUNC_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_TONE_CONFIG_STATE6;		
			}
		break;

		case HC_TONE_CONFIG_STATE6:
			if ((board==FIRE_BOARD)||(board==RADIO_BOARD))
			{
				//REPETER_DISABLE tone configuration
				hd_host_ctrl.sHD_tone_configuration.channels_mask[TG_REPETER_ENABLE_TONE_NUM-1]=TRUE;
				hd_host_ctrl.sHD_tone_configuration.freq_hz=TG_REPETER_ENABLE_COMAND;
				//hd_host_ctrl.sHD_tone_configuration.volume=TG_FUNC_TONE_VOLUME;
				hd_host_ctrl.sHD_tone_configuration.HD_request=TRUE;
				#ifdef ENABLE_AUDIO_PROC
				state_host_ctrl_update=HC_AGC_CONFIG_STATE1;		
				#else
				state_host_ctrl_update=HC_STOP_STATE;
				#endif
				if (board==FIRE_BOARD)
					state_host_ctrl_update=HC_STOP_STATE;
			}
		break;
		*/
		case HC_AGC_CONFIG_STATE1:
			if (board==mACB_BOARD)
			{
				//agc init OBSERVER
				hd_host_ctrl.sHD_agc_initialization.channels_mask[0][HD_ADC]=1;
			//	hd_host_ctrl.sHD_agc_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE2;		
			}		
			if (board==FIRE_BOARD)
			{
				//agc init on radio channel
				hd_host_ctrl.sHD_agc_initialization.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_initialization.channels_mask[4][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE2;		
			}
			if (board==VT_BOARD)
			{
				//agc init on radio channel
				hd_host_ctrl.sHD_agc_initialization.channels_mask[0][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_initialization.channels_mask[1][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE2;
			}
		break;
		
		case HC_AGC_CONFIG_STATE2:
			if (board==mACB_BOARD)
			{
				//agc config on OBSERVER
				hd_host_ctrl.sHD_agc_configuration.channels_mask[0][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_configuration.cutoff_input_level		=  -30;//-25; // -90..-30 dB
				hd_host_ctrl.sHD_agc_configuration.cutoff_offset			=   0; //
				hd_host_ctrl.sHD_agc_configuration.rotation_input_level  	=  -20;//15; // -40..+40 dB
				hd_host_ctrl.sHD_agc_configuration.rotation_output_level 	=  -7; // -20..-3 dB
				
		//		hd_host_ctrl.sHD_agc_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE3;		
			}


			if (board==FIRE_BOARD)
			{
				//agc config on radio channel
				hd_host_ctrl.sHD_agc_configuration.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_configuration.channels_mask[4][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_configuration.cutoff_input_level		=  -30;//-25; // -90..-30 dB
				hd_host_ctrl.sHD_agc_configuration.cutoff_offset			=   0; //
				hd_host_ctrl.sHD_agc_configuration.rotation_input_level  	=  -20;//15; // -40..+40 dB
				hd_host_ctrl.sHD_agc_configuration.rotation_output_level 	=  -7; // -20..-3 dB
				
				hd_host_ctrl.sHD_agc_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE3;		
			}
			if (board==VT_BOARD)
			{
				//agc config on radio channel
				hd_host_ctrl.sHD_agc_configuration.channels_mask[0][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_configuration.channels_mask[1][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_configuration.cutoff_input_level		=  -45;//-25; // -90..-30 dB
				hd_host_ctrl.sHD_agc_configuration.cutoff_offset			=   10; //
				hd_host_ctrl.sHD_agc_configuration.rotation_input_level  	=  -35;//15; // -40..+40 dB
				hd_host_ctrl.sHD_agc_configuration.rotation_output_level 	=  -12; // -20..-3 dB

				hd_host_ctrl.sHD_agc_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_AGC_CONFIG_STATE3;
			}
		break;

		case HC_AGC_CONFIG_STATE3:
			if (board==mACB_BOARD)
			{
				//agc enable on OBSERVER
				hd_host_ctrl.sHD_agc_enabling.channels_mask[0][HD_ADC]=1;
				
	//			hd_host_ctrl.sHD_agc_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE1;		
			}

			if (board==FIRE_BOARD)
			{
				//agc enable on radio channel
				hd_host_ctrl.sHD_agc_enabling.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_enabling.channels_mask[4][HD_ADC]=1;
				
	//-			hd_host_ctrl.sHD_agc_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE1;		
			}
			if (board==VT_BOARD)
			{
				//agc enable on radio channel
				hd_host_ctrl.sHD_agc_enabling.channels_mask[0][HD_ADC]=1;
				hd_host_ctrl.sHD_agc_enabling.channels_mask[1][HD_ADC]=1;

				hd_host_ctrl.sHD_agc_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_LEC_INIT_STATE;
			}
		break;

		case HC_FILTER_CONFIG_STATE1:
			if (board==mACB_BOARD)
			{
				//hpf init on OBSERVER
				hd_host_ctrl.sHD_filter_initialization.channels_mask[0][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.type=HD_FILTER_HP300HZ;
				
			//	hd_host_ctrl.sHD_filter_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE2;		
			}

			if (board==FIRE_BOARD)
			{
				//hpf init on radio channel
				hd_host_ctrl.sHD_filter_initialization.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.channels_mask[4][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.type=HD_FILTER_HP300HZ;
				
				hd_host_ctrl.sHD_filter_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE2;		
			}
			else if (board==MACS_2R_1T_BOARD)
			{
				//hpf init on telephone channel
				hd_host_ctrl.sHD_filter_initialization.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.type=HD_FILTER_HP300HZ;				
				hd_host_ctrl.sHD_filter_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE2;												
			}			
			else if (board==TELEPHONY_BOARD)
			{
				//hpf init on telephone channel
				for(i=0;i<8;i++)
					hd_host_ctrl.sHD_filter_initialization.channels_mask[i][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.type=HD_FILTER_HP300HZ;				
				hd_host_ctrl.sHD_filter_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE2;										
			}
			else if (board==TELEPHONY_2_BOARD)
			{
				//hpf init on telephone channel
				for(i=0;i<16;i++)
					hd_host_ctrl.sHD_filter_initialization.channels_mask[i][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_initialization.type=HD_FILTER_HP300HZ;				
				hd_host_ctrl.sHD_filter_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_FILTER_CONFIG_STATE2;										
			}																			
		break;	
		
		case HC_FILTER_CONFIG_STATE2:
			if (board==mACB_BOARD)
			{
				//hpf enable on OBSERVER
				hd_host_ctrl.sHD_filter_enabling.channels_mask[0][HD_ADC]=1;
				
		//		hd_host_ctrl.sHD_filter_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE1;		
			}

			if (board==FIRE_BOARD)
			{
				//hpf enable on radio channel
				hd_host_ctrl.sHD_filter_enabling.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_enabling.channels_mask[4][HD_ADC]=1;
				
	//-			hd_host_ctrl.sHD_filter_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE1;		
			}
			else if (board==MACS_2R_1T_BOARD)
			{
				//hpf enable on telephone channel
				hd_host_ctrl.sHD_filter_enabling.channels_mask[2][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_CALLER_ID_CONFIG_STATE1;												
			}			
			else if (board==TELEPHONY_BOARD)
			{
				//hpf enable on telephone channel
				for(i=0;i<8;i++)
					hd_host_ctrl.sHD_filter_enabling.channels_mask[i][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_enabling.HD_request=TRUE;	
				state_host_ctrl_update=HC_CALLER_ID_CONFIG_STATE1;										
			}
			else if (board==TELEPHONY_2_BOARD)
			{
				//hpf enable on telephone channel
				for(i=0;i<16;i++)
					hd_host_ctrl.sHD_filter_enabling.channels_mask[i][HD_ADC]=1;
				hd_host_ctrl.sHD_filter_enabling.HD_request=TRUE;	
				state_host_ctrl_update=HC_CALLER_ID_CONFIG_STATE1;										
			}						
		break;
		
		case HC_CD_CONFIG_STATE1:
			if (board==mACB_BOARD)
			{
				//cd enable on observer channel
				hd_host_ctrl.sHD_cd_initialization.channels_mask[0][HD_ADC]=1;
				
				hd_host_ctrl.sHD_cd_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE2;		
			}
			else if (board==MTR_BOARD)
			{
				
				hd_host_ctrl.sHD_cd_initialization.channels_mask[0][HD_DAC]=1;
				hd_host_ctrl.sHD_cd_initialization.channels_mask[1][HD_DAC]=1;
				
				hd_host_ctrl.sHD_cd_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE2;			
			}
		break;
		
		case HC_CD_CONFIG_STATE2:
			if (board==mACB_BOARD)
			{
				//cd enable on observer channel
				hd_host_ctrl.sHD_cd_configuration.channels_mask[20][HD_DAC]=1;
				hd_host_ctrl.sHD_cd_configuration.threshold_level=-30;
				hd_host_ctrl.sHD_cd_configuration.start_duration=0;
				hd_host_ctrl.sHD_cd_configuration.stop_duration=200;
				hd_host_ctrl.sHD_cd_configuration.mute=TRUE;				
				hd_host_ctrl.sHD_cd_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE3;		
			}
			if (board==MTR_BOARD)
			{
				//cd enable on observer channel
				hd_host_ctrl.sHD_cd_configuration.channels_mask[0][HD_DAC]=1;
				hd_host_ctrl.sHD_cd_configuration.channels_mask[1][HD_DAC]=1;

				hd_host_ctrl.sHD_cd_configuration.threshold_level=-35;
				hd_host_ctrl.sHD_cd_configuration.start_duration=0;
				hd_host_ctrl.sHD_cd_configuration.stop_duration=0;//200;
				hd_host_ctrl.sHD_cd_configuration.mute=FALSE;				
				hd_host_ctrl.sHD_cd_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_CD_CONFIG_STATE3;		
			}			
		break;
		
		case HC_CD_CONFIG_STATE3:
			if (board==mACB_BOARD)
			{
				//cd enable on observer channel
				hd_host_ctrl.sHD_cd_enabling.channels_mask[20][HD_DAC]=1;
				
			//	hd_host_ctrl.sHD_cd_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_ECHO_CANCEL_CONFIG_STATE1;		
			}
			if (board==MTR_BOARD)
			{
				hd_host_ctrl.sHD_cd_enabling.channels_mask[0][HD_DAC]=1;
				hd_host_ctrl.sHD_cd_enabling.channels_mask[1][HD_DAC]=1;
				
				hd_host_ctrl.sHD_cd_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_STOP_STATE;		
			}			
		break;

		case HC_ECHO_CANCEL_CONFIG_STATE1:
			if(board==RADIO_BOARD)
			{
				hd_host_ctrl.sHD_echo_c_initialization.channels_mask[0]=1;
				hd_host_ctrl.sHD_echo_c_initialization.tail_length=256;
				hd_host_ctrl.sHD_echo_c_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_ECHO_CANCEL_CONFIG_STATE2;		
			}
		break;	
		
		case HC_ECHO_CANCEL_CONFIG_STATE2:
			if(board==RADIO_BOARD)
			{

				hd_host_ctrl.sHD_echo_c_configuration.channels_mask[0]=1;
				hd_host_ctrl.sHD_echo_c_configuration.attenuation=-50;
				hd_host_ctrl.sHD_echo_c_configuration.att_near_active=-15;
				hd_host_ctrl.sHD_echo_c_configuration.echo_ch=0;
				hd_host_ctrl.sHD_echo_c_configuration.echo_ch_dir=HD_DAC;
				
				hd_host_ctrl.sHD_echo_c_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_ECHO_CANCEL_CONFIG_STATE3;		
			}
		break;
			
		case HC_ECHO_CANCEL_CONFIG_STATE3:
			if(board==RADIO_BOARD)
			{

				hd_host_ctrl.sHD_echo_c_enabling.channels_mask[0]=1;
				
				hd_host_ctrl.sHD_echo_c_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE1;		
			}
		break;	
								
		case HC_VOCODER_CONFIG_STATE1:
			if (board==mACB_BOARD)
			{
				//VOCODER INIT
				for(i=20;i<20+DEUBAG_HOST_CHANNEL;i++)
				{
					hd_host_ctrl.sHD_vocoder_initialization.channels_mask[i][HD_ADC]=1;
					hd_host_ctrl.sHD_vocoder_initialization.channels_mask[i][HD_DAC]=1;
				}	
				hd_host_ctrl.sHD_vocoder_initialization.type=DEBUG_HOST_CTRL_VOCODER;
				hd_host_ctrl.sHD_vocoder_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE2;		
			}
			if ((board==RADIO_BOARD)||(board==RADIO2_BOARD)||(board==TELEPHONY_BOARD))
			{
				//VOCODER INIT
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[20][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[20][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[21][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[21][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[22][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[22][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[23][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[23][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[24][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[24][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[25][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[25][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[26][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[26][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[27][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_initialization.channels_mask[27][HD_DAC]=1;


				hd_host_ctrl.sHD_vocoder_initialization.type=DEBUG_HOST_CTRL_VOCODER;
				hd_host_ctrl.sHD_vocoder_initialization.HD_request=TRUE;
				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE2;		
			}			
		break;

		case HC_VOCODER_CONFIG_STATE2:
			if (board==mACB_BOARD)
			{
				//vocoder config
				for(i=20;i<20+DEUBAG_HOST_CHANNEL;i++)
				{
					hd_host_ctrl.sHD_vocoder_configuration.channels_mask[i][HD_ADC]=1;
					hd_host_ctrl.sHD_vocoder_configuration.channels_mask[i][HD_DAC]=1;
				}
				hd_host_ctrl.sHD_vocoder_configuration.vad_enable=1;
				hd_host_ctrl.sHD_vocoder_configuration.frame_erase_enable=1;
				hd_host_ctrl.sHD_vocoder_configuration.sub_type=0;
				hd_host_ctrl.sHD_vocoder_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE3;		
			}
			if ((board==RADIO_BOARD)||(board==RADIO2_BOARD)||(board==TELEPHONY_BOARD))
			{
				//vocoder config
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[20][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[20][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[21][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[21][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[22][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[22][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[23][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[23][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[24][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[24][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[25][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[25][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[26][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[26][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[27][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_configuration.channels_mask[27][HD_DAC]=1;
				
				hd_host_ctrl.sHD_vocoder_configuration.sub_type=0;
				hd_host_ctrl.sHD_vocoder_configuration.vad_enable=1;
				
				
				hd_host_ctrl.sHD_vocoder_configuration.HD_request=TRUE;
				state_host_ctrl_update=HC_VOCODER_CONFIG_STATE3;		
			}			
		break;
		
		case HC_VOCODER_CONFIG_STATE3:
			if (board==mACB_BOARD)
			{
				//vocoder enable
				for(i=20;i<20+DEUBAG_HOST_CHANNEL;i++)
				{
					hd_host_ctrl.sHD_vocoder_enabling.channels_mask[i][HD_ADC]=1;
					hd_host_ctrl.sHD_vocoder_enabling.channels_mask[i][HD_DAC]=1;
				}
				
				hd_host_ctrl.sHD_vocoder_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_STOP_STATE;		
			}
			if ((board==RADIO_BOARD)||(board==RADIO2_BOARD)||(board==TELEPHONY_BOARD))
			{
				//vocoder enable
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[20][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[20][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[21][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[21][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[22][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[22][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[23][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[23][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[24][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[24][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[25][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[25][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[26][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[26][HD_DAC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[27][HD_ADC]=1;
				hd_host_ctrl.sHD_vocoder_enabling.channels_mask[27][HD_DAC]=1;
				
				hd_host_ctrl.sHD_vocoder_enabling.HD_request=TRUE;
				state_host_ctrl_update=HC_STOP_STATE;		
			}			
		break;	
			
																																																												
		case HC_CALLER_ID_CONFIG_STATE1:
			if (board==MACS_2R_1T_BOARD)
			{
				hd_host_ctrl.sHD_caller_id_configuration.channel[0]=2;
				hd_host_ctrl.sHD_caller_id_configuration.type[2]=HD_CALLER_ID_ETSI_TYPE1;
				hd_host_ctrl.sHD_caller_id_configuration.detection_mode[2]=HD_CALLER_ID_ALLWAYS;
				hd_host_ctrl.sHD_caller_id_configuration.HD_request=1;
				state_host_ctrl_update=HC_STOP_STATE;
			}
			else if (board==TELEPHONY_BOARD)
			{
				for(i=0;i<8;i++)
				{
					hd_host_ctrl.sHD_caller_id_configuration.channel[i]=i;
					hd_host_ctrl.sHD_caller_id_configuration.type[i]=HD_CALLER_ID_ETSI_TYPE1;
					hd_host_ctrl.sHD_caller_id_configuration.detection_mode[i]=HD_CALLER_ID_ALLWAYS;
					hd_host_ctrl.sHD_caller_id_configuration.HD_request=1;
					state_host_ctrl_update=HC_STOP_STATE;
				}	
			
			
			}
			else if (board==TELEPHONY_2_BOARD)
			{
				for(i=0;i<16;i++)
				{
					hd_host_ctrl.sHD_caller_id_configuration.channel[i]=i;
					hd_host_ctrl.sHD_caller_id_configuration.type[i]=HD_CALLER_ID_ETSI_TYPE1;
					hd_host_ctrl.sHD_caller_id_configuration.detection_mode[i]=HD_CALLER_ID_ALLWAYS;
					hd_host_ctrl.sHD_caller_id_configuration.HD_request=1;
					state_host_ctrl_update=HC_STOP_STATE;
				}
			
			}			
		break;	

		case HC_DELAY_BUFFER_CONFIG_STATE1:

			if (board==MACS_2R_1T_BOARD)
			{
				DLYBUF_extension.init.channels_mask[0][HD_DAC]=1;//enable delay on output ch 0
				DLYBUF_extension.init.channels_mask[1][HD_DAC]=1;//enable delay on output ch 1
				DLYBUF_extension.init.param.buffer_size=500; //mqx 5sec delay
				DLYBUF_extension.init.param.in_mode=HD_DLY_MODE_IN_PTT_CD; //start in on cd and ptt
				DLYBUF_extension.init.param.out_mode=HD_DLY_MODE_OUT_CD;//start transmitting after cd (bip detection)
				DLYBUF_extension.init.param.cd_channel=0;
				DLYBUF_extension.init.param.cd_in_out=HD_ADC;
				DLYBUF_extension.init.param.out_delay=50; //500mse delay duration of the bip signal
				DLYBUF_extension.init.HD_request=1;
				state_host_ctrl_update=HC_DELAY_BUFFER_CONFIG_STATE2;
				j=0;
			}
		break;

		case HC_DELAY_BUFFER_CONFIG_STATE2:

			j++;
			if (j>50)
			{
				j=0;
				//activate ptt
				if (board==MACS_2R_1T_BOARD)
				{
					DLYBUF_extension.ctrl.channels_mask[0][HD_DAC]=1;//enable delay on output ch 0
					DLYBUF_extension.ctrl.channels_mask[1][HD_DAC]=1;//enable delay on output ch 1
					DLYBUF_extension.ctrl.param.ptt=1; //activate ptt
					DLYBUF_extension.ctrl.param.transmit=0;
					DLYBUF_extension.ctrl.param.enable=1;//enable
					DLYBUF_extension.ctrl.HD_request=1;
					state_host_ctrl_update=HC_DELAY_BUFFER_CONFIG_STATE3;
				}
			}
		break;
		case HC_DELAY_BUFFER_CONFIG_STATE3:

			j++;
			if (j>5000)
			{
				j=0;
				//release ptt
				if (board==MACS_2R_1T_BOARD)
				{
					DLYBUF_extension.ctrl.channels_mask[0][HD_DAC]=1;//enable delay on output ch 0
					DLYBUF_extension.ctrl.channels_mask[1][HD_DAC]=1;//enable delay on output ch 1
					DLYBUF_extension.ctrl.param.ptt=0; //activate ptt
					DLYBUF_extension.ctrl.param.transmit=0;
					DLYBUF_extension.ctrl.param.enable=1;//enable
					DLYBUF_extension.ctrl.HD_request=1;
					state_host_ctrl_update=HC_STOP_STATE;
				}
			}
		break;
		case HC_LEC_INIT_STATE:
			if (board==VT_BOARD)
			{
				hd_host_ctrl.sHD_line_echo_c_initialization.channels_mask[0]=1;
				hd_host_ctrl.sHD_line_echo_c_initialization.channels_mask[1]=1;
				hd_host_ctrl.sHD_line_echo_c_initialization.tail_length=256;
				hd_host_ctrl.sHD_line_echo_c_initialization.HD_request=1;
				state_host_ctrl_update=HC_LEC_CONFIG_STATE;
			}
		break;
		case HC_LEC_CONFIG_STATE:
			if (board==VT_BOARD)
			{
				hd_host_ctrl.sHD_line_echo_c_configuration.channels_mask[0]=1;
				hd_host_ctrl.sHD_line_echo_c_configuration.channels_mask[1]=0;
				hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch=0;
				hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch_dir=HD_DAC;
				hd_host_ctrl.sHD_line_echo_c_configuration.HD_request=1;
				state_host_ctrl_update=HC_LEC_CONFIG_STATE1;
			}
		break;
		case HC_LEC_CONFIG_STATE1:
			if (board==VT_BOARD)
			{
				hd_host_ctrl.sHD_line_echo_c_configuration.channels_mask[0]=0;
				hd_host_ctrl.sHD_line_echo_c_configuration.channels_mask[1]=1;
				hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch=1;
				hd_host_ctrl.sHD_line_echo_c_configuration.echo_ch_dir=HD_DAC;
				hd_host_ctrl.sHD_line_echo_c_configuration.HD_request=1;
				state_host_ctrl_update=HC_LEC_ENABLE_STATE;
			}
		break;
		case HC_LEC_ENABLE_STATE:
			if (board==VT_BOARD)
			{
				hd_host_ctrl.sHD_line_echo_c_enabling.channels_mask[0]=1;
				hd_host_ctrl.sHD_line_echo_c_enabling.channels_mask[1]=1;
				hd_host_ctrl.sHD_line_echo_c_enabling.HD_request=1;
				state_host_ctrl_update=HC_STOP_STATE;
			}
		break;
		case HC_STOP_STATE:
		
			//read and write i/o
//			if (hd_host_ctrl.sHD_gpio_status.data_out==0xe0)
//				hd_host_ctrl.sHD_gpio_status.data_out=0xd0;
//			else
//				hd_host_ctrl.sHD_gpio_status.data_out=0xe0;	
//			hd_host_ctrl.sHD_gpio_status.HD_request=TRUE;
/*
			//simulate detection of ring and onhook
			if ((board==TELEPHONY_BOARD)||
				(board==TELEPHONY_FIELD_TELEPHONY_BOARD))
			{
				for (i=0;i<8;i++)
				{
					//check if a telephone is ringing
				///	if (tel->status.ring_status[i]==1)
					{
						//set off hook
						tel->ctrl.channels_mask[i]=TRUE;	
						tel->ctrl.register_addr=0;
						tel->ctrl.register_data=0;
						tel->ctrl.read_register_addr=0;
						tel->ctrl.off_hook_state=1;
						tel->ctrl.ring_validate_off=0;
						tel->ctrl.event_mode=0;
						tel->ctrl.HD_request=TRUE;					
					}
				}	
			}
*/			
			
		break;
	
	}
	
}
#endif  //DEBUG_HOST_CTRL



/**************************************************************************/                     
//  Module		: host_ctrl_audio_update
//  Description : handle host request
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_audio_update(void)
{
    Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, true);
	//update audio process enable state from hpi 
	host_ctrl_audio_enable();
	
	host_ctrl_audio_configuration();
	
	//update and generate tones
	tone_generate();

//	test_tone	();
	//update tone muxing
	tone_mux_config();
	Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, true);
	
}


/**************************************************************************/                     
//  Module		: host_ctrl_gpio_update
//  Description : update host with gpio value or set gpio value according 
//		to host request
/// @return none
/// @param	none
/**************************************************************************/
void host_ctrl_gpio_update(void)
{
	static uint8_t HC_gpio_dir;
	int i;
	
	//gpio handle
	//configure direction
	/*if(hd_host_ctrl.sHD_gpio_configuration.HD_request==TRUE)
	{
	
		HC_gpio_dir=0;
		for (i=0;i<HD_GPIO_NUM;i++)
		{
			if (hd_host_ctrl.sHD_gpio_configuration.in_out[i])
				HC_gpio_dir|=(1<<i);
		}
		
		//GPIO_RSETH(hGpio,GPDIR,HC_gpio_dir);
		hd_host_ctrl.sHD_gpio_configuration.HD_request=FALSE;
	
	}
	//read GPIO
	//hd_host_ctrl.sHD_gpio_status.data_in=GPIO_read(hGpio,0xff);

	if(hd_host_ctrl.sHD_gpio_status.HD_request==TRUE)
	{
		//write GPIO
		//GPIO_write(hGpio,HC_gpio_dir,(uint8_t)hd_host_ctrl.sHD_gpio_status.data_out);
		hd_host_ctrl.sHD_gpio_status.HD_request=FALSE;
	}*/
	
#ifdef ENABLE_SYSTEM_RESTART	
	if((hd_host_ctrl.sHD_channel_configuration.HD_request==TRUE)&&
		(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY))
	{

	  	/*init_devices();
	  	
	  	//reset_dsp();
	  	c_int00();*/
	
	}
#endif		
	
}


/**************************************************************************/                     
//  Module		: bit_status_events
//  Description : check if new error flag were set if yes send event to cpu
/// @return none
/// @param	none
/**************************************************************************/
void bit_status_events(void)
{
	uint32_t dsp_new_error=0;
	static uint32_t cpu_load_count=0,cpu_load_resend=HC_CPU_LOAD_MAX_RESAND;

	//check if dsp error bits were set  
	if 	(dsp_error[0]||dsp_error[1])
	{
		dsp_err_event.dsp_error[0]=dsp_error[0];
		dsp_err_event.dsp_error[1]=dsp_error[1];
		//send_event_frame((void*)&dsp_err_event,sizeof(HD_events_dsp_err),HD_CH_ANY,
			//HD_DSP_ERR_EVENT,HD_ERR_GLOBAL_EVENT);
		//clear error flag	
		dsp_error[0]=0;
		dsp_error[1]=0;	
	}
			
	//check if host frame error accured
	if (HC_host_audio_error_count!=old_dsp_status.host_audio_error_count)
	{
		dsp_err_host_event.host_audio_error_count=HC_host_audio_error_count;
		old_dsp_status.host_audio_error_count=HC_host_audio_error_count;
		//send_event_frame((void*)&dsp_err_host_event,sizeof(HD_events_dsp_host_err),HD_CH_ANY,
			//HD_DSP_ERR_EVENT,HD_ERR_HOST_EVENT);
	}

	if (cpu_load_resend<=HC_CPU_LOAD_MAX_RESAND)
		cpu_load_resend++;
	//check if cpu_load is too high
	if (hd_host_ctrl.sHD_dsp_status.cpu_load>HC_CPU_LOAD_MAX)
	{
		cpu_load_count++;

		if (cpu_load_count>HC_CPU_LOAD_MAX_COUNT)
		{
			if (cpu_load_resend>HC_CPU_LOAD_MAX_RESAND)
			{
				dsp_err_load_event.cpu_load=hd_host_ctrl.sHD_dsp_status.cpu_load;
				//send_event_frame((void*)&dsp_err_load_event,sizeof(HD_events_dsp_load_err),HD_CH_ANY,
					//	HD_DSP_ERR_EVENT,HD_ERR_LOAD_EVENT);
				cpu_load_resend=0;
			}

			cpu_load_count=0;	
		}		
	}
	else
		cpu_load_count=0;	
}

/**************************************************************************/                     
//  Module		: print_debug
//  Description : print data to debug buffer, add data to debug buffer
///				data is in the format 32bit size, 
/// @return none
/// @param	data: contain data to send, type: 8 or 32 bit data, 
///		size: data size, operation: HC_DBG_START, HC_DBG_ROLLOVER 
/**************************************************************************/
int print_debug(void *data,uint32_t type,uint32_t size,int operation)
{
	static uint32_t index=0,count=0;
	uint32_t size_32bit;

	if (size==0)
		return -1;
	//if operation HC_DBG_STARTOVER start buffer from index 0
	if (operation==HC_DBG_START)
	{
		index=0;
	}
	//if size is not devide in 4 add number of bytes to change size to be a multiply of 4
	size_32bit=(size+(4-(size%4)))/4;	

	//check if we have more place in buffer	
	if (index>(HC_DBG_BUFF_SIZE-size-4))
	{
		if((operation==HC_DBG_ROLLOVER)&&(size<HC_DBG_BUFF_SIZE-3))
			index=0;	
		else
			return -1;	
				
	}		
	
	HC_dbg_buffer[index+3+size_32bit]=0;//clear next buffer size
	HC_dbg_buffer[index]=size_32bit;//size (number of words(32bit) in data field)
	index++;
	HC_dbg_buffer[index]=type;//way of presentation char or 32bit
	index++;
	HC_dbg_buffer[index]=count;//way of presentation char or 32bit
	index++;		
	HC_dbg_buffer[index+size_32bit-1]=0;
	memcpy ((uint8_t*)&HC_dbg_buffer[index],data,size);
	index+=size_32bit;	
	count++;	

	return 0;			

}
