/******************************************************************************
                         	TechMer Ltd.
                  5, Hatzoref st., Holon 58856 ISRAEL
  Copyright TechMer all rights reserved, confidential proprietary

  Project		: DigitalCore
  Author		: Ilan Rosilio		
  Description : Definition of HPI channel interface between Host CPU(ARM)
					and DSP.	  
		 
	@file:	DigitalCore_hpi.h	
  
 Revision History:
																
 Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
------------	-----------  ---------  ---------------------------------------
Ilan Rosilio	19/May/2008	  Created	

  ========                                                              
  HISTORY:                                                              
				$Log$
				ilan_r - ?????? ?????????? 03 ???????????? 2013 10:34:54
				ver 1.12.16
				init PLC module on powerup
				ilan_r - Tuesday, October 15, 2013 12:03:41 PM
				ver 1.12.5 update dc and navy filters
				ilan_r - ?????? ?????????? 06 ?????????????? 2013 10:39:06
				VER 1.12.4 Add encription, add cd status reading, change dc_block filter gain
				ilan_r - Sunday, September 29, 2013 2:57:25 PM
				ver 1.12.3 add navy headset filter, dc block filter, separate LEC and EC modules
				ilan_r - Monday, September 16, 2013 2:57:50 PM
				VER 1.12.3 include navy headset filter (1.6khz)
				ilan_r - Monday, September 02, 2013 5:33:08 PM
				ver 1.12.2 fix warning
				ilan_r - Thursday, August 22, 2013 3:17:23 PM
				ver 1.12.1 add dc block filter
				ilan_r - ?????? ?????????? 18 ?????????? 2013 08:21:50
				add delay buffer
				ilan_r - ?????? ?????? 18 ???????????? 2013 13:37:40
				ilan_r - ?????? ?????????? 22 ?????? 2012 07:31:21
				Ver 1.11.13 fix channel number index in caller id and dtmf events
				ilan_r - ?????? ?????????? 18 ?????????? 2012 13:38:26
				ilan_r - ?????? ?????????? 20 ?????? 2012 09:38:25
				Ver 1.11.11 fix probelm in telephony driver of not clearing control buffer that couse not sending off hook (as it was overwrite by updating reg 5 for caller id)
				yan_b - ?????? ?????????? 06 ?????? 2012 09:37:30
				ilan_r - ?????? ?????????? 06 ?????? 2012 09:32:39
				Ver 1.11.10 change padding in DigitalCore_hpi.h in tel status extension, in host_ctrl change init for dtmf and caller id module (they are not active yet)
				ilan_r - ?????? ?????????? 19 ?????????? 2012 14:43:16
				remove caller id and dtmf detection (not in use and not final yet)
				ilan_r - ?????? ?????????? 19 ?????????? 2012 09:31:20
				ver 1.11.9, add caller id and dtmf detect modules, fix bug in host_ctrl - clearing tel struct before pointer init
				ilan_r - ?????? ?????????? 17 ?????????? 2012 09:17:44
				ilan_r - ?????? ?????????? 17 ???????????? 2011 16:50:54
				ver 1.11.8 add telephone status and event for parallel phone on hook state detection. fix HD_AC_CONF_NO_CAHNGE to HD_AC_CONF_NO_CHANGE
				ilan_r - ?????? ?????? 31 ?????????????? 2011 12:41:17
				Ver 1.11.7 fix bug in si3056 tel driver to enable register setting (change in codec_si3056_praivete.h)
				ilan_r - ?????? ?????????? 17 ???????? 2011 10:26:36
				Ver 1.11.6 add possibility to not change fields in ac config by using HD_AC_CONF_NO_CAHNGE, update led driver
				ilan_r - ?????? ?????????? 17 ???????? 2011 08:52:33
				led driver
				ilan_r - ?????? ?????????? 14 ?????????? 2011 07:24:34
				ver 1.11.5 event for error is sent every time there is a new error in the last 10msec, add stack error and separate no audio sync error and codec (register) error
				ilan_r - ?????? ?????????? 17 ?????? 2011 18:18:33
				VER 1.11.4 cpu load event is sent only after 5 contiues fault as the CLK_gethtime function may return fault value
				ilan_r - ?????? ?????????? 16 ?????????? 2010 13:06:33
				VER 1.11.3 fix problem in MTR driver, add PCM carrier_detect info
				ilan_r - ?????? ?????? 01 ???????????? 2010 12:39:21
				Ver 1.11.2 update MTR driver:activate vad before buffering audio
				ilan_r - ?????? ?????? 01 ???????????? 2010 12:01:00
				ilan_r - ?????? ?????????? 13 ?????????????? 2010 18:40:27
				Ver1.11.1 include Adaptive Digital line echo cancller v10.30 (not demo)
				ilan_r - ?????? ?????????? 03 ?????????????? 2010 18:22:24
				ver 1.11.0 add lec adaptive digital demo lib
				ilan_r - ?????? ?????????? 02 ???????? 2010 13:47:52
				VER 1.10.4 fix audio+tont mux by clearing output channels after sending them
				ilan_r - ?????? ?????????? 07 ?????????? 2010 13:30:16
				add MTR driver
				ilan_r - Sunday, November 22, 2009 2:51:23 PM
				ver 1.10.3 in aic24 driver fix reg6 with fixed value - this was done to enable working with sidetone in codec
				ilan_r - Sunday, October 11, 2009 2:11:35 PM
				v1.10.2 add in tel driver ring state event- the ring event may be for state or for ring on and off during ringing host_ctrl debug for echo cancelling was added
				ilan_r - Thursday, September 17, 2009 3:55:24 PM
				v1.10.1
				ilan_r - Wednesday, September 09, 2009 10:51:24 AM
				ver 1.10.0 change host channel to 48 and tone channel to 16, in CDB file change malloc/free to be from SDRAM memory,(agc,cd,filter heap memory)
				ilan_r - Tuesday, September 08, 2009 10:52:10 AM
				ver 1.9.5 update dsp status and bit fields in DigitalCore_hpi.h
				ilan_r - Thursday, September 03, 2009 9:20:20 AM
				ver1.9.4 add dsp error events for dsp_error,host frame error and high cpu load
				ilan_r - Wednesday, September 02, 2009 11:54:13 AM
				ver 1.9.3 change in audio processing if some channel are not received the system will continue to work without waiting for them in the next loop.
				ilan_r - Tuesday, September 01, 2009 3:34:28 PM
				ver 1.9.2 include echo canceling
				ilan_r - Sunday, August 30, 2009 4:14:35 PM
				ver 1.9.1 change audio_processing, device control is made for every channel in this way all received channel are being configured without connection to other channels, add start function to start all codec in one time. change volt event in si3056 driver
				ilan_r - Monday, August 24, 2009 10:35:22 AM
				ver 1.9.0 add Noise Reduction support (speex)
				ilan_r - ?????? ?????? 03 ???????????? 2009 18:13:49
				change in tel driver dac gain set reg38 and not 40
				ilan_r - ?????? ?????????? 26 ???????? 2009 11:44:42
				v1.8.1 Changes in si3056 driver
				ilan_r - ?????? ?????? 06 ???????? 2009 14:29:23
				v1.7.5 change aic14/24 driver add posibility to modify internal codec register
				ilan_r - Sunday, June 07, 2009 3:28:34 PM
				version 1.7.4 heap size in ISRAM changed to 0x16000 (insted of 0x11000) and memory allocation failure is reported to arm
				ilan_r - Sunday, May 10, 2009 1:11:04 PM
				v1.7.3 complie AGC library with realese slope parameter changed to 30db insted of 6db, this gives faster gain decrease after stop speaking
				ilan_r - Monday, May 04, 2009 7:58:34 AM
				v1.7.2 changes in host_frame regarding bit
				ilan_r - Sunday, April 26, 2009 4:01:58 PM
				in host_frame if no frame arrive from arm it will be consider as a lost frame (or vad 0 frame between vad2 frame in g729) 
				heap was increase to support PCM 30ch
				ilan_r - Wednesday, April 22, 2009 4:10:08 PM
				adding optimization -o2, reading gpio will read output as well
				ilan_r - Thursday, March 26, 2009 2:40:36 PM
				V1.6.1 change host channel number to 30
				ilan_r - Tuesday, March 24, 2009 1:51:19 PM
				ver 1.6.0, change host channels to 80, change tone generation to take less cpu time, type of audio frame to host is set by vocoder init
				ilan_r - Thursday, March 05, 2009 3:48:15 PM
				ver 1.5.12, fix problem of configuration new configuration after reset, set unused value to edma handle to prevent un opend device to be read or write
				ilan_r - Wednesday, March 04, 2009 7:56:13 AM
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

#ifndef _MAIN__H_
#define _MAIN__H_

//#include <csl_hpi.h>
#define DSP_VERSION_TAG "20/02/23 09:38"	//"48 host,16 tone,dly_buf,CCS5,encrypt. Tehila"
#define DSP_VERSION_MAJOR	2
#define DSP_VERSION_MINOR	0
#define DSP_VERSION_BUGFIX	0
#define HPI_INTERFACE_VERSION	HD_INTERFACE_VERSION	

//number of word(16bit) in the uncompressed speech block(audio samples in 10ms)
#define 	NSAMPLES	80
//number of bytes in the compressed speech block(audio samples in 10ms) 
#define SPEECH_SIZE		10

#define VV_STATE 		0 //VADFlag
#define CC_STATE 		1 //Carrier Detect State


//#define FILES
////////////////////////////////////////////////////////////////////////////////
#define	DSP_ERR_OK				0x0000		// no error
#define	DSP_ERR_RX_FRAME_LOSS	0x0001		// rx frame loss

// Constants for the buffered audio ping-pong transfer 
#define BUFFSIZE 160	//audio buffer size, in every 10ms there are
						//80 audio words and 80 control(for codec) words
#define PING 0
#define PONG 1

#define mACB_BOARD						1
#define FIRE_BOARD						2
#define FIELD_TELEPHONY_BOARD			3
#define TELEPHONY_BOARD					4
#define RADIO_BOARD						5
#define TELEPHONY_FIELD_TELEPHONY_BOARD 6
#define RADIO2_BOARD					7
#define MTR_BOARD						8
#define MACS_2R_1T_BOARD				9
#define TELEPHONY_2_BOARD				10
#define MACS_R_RADIO_BOARD				11
#define VT_BOARD						12
#define SVT2_BOARD						15

#define DEBUG_BOARD VT_BOARD//MACS_R_RADIO_BOARD //RADIO_BOARD	//FIRE_BOARD	//mACB_BOARD


#define HOST_PIPE_LENGTH	2
//#define SET_HPI_INTTERUPT_TO_HOST	(HPI_setHint(1))


// define test cases
#define AGC_ADC_TEST	0
#define AGC_DAC_TEST	1
#define HPF_TEST		2
#define CD_TEST			3
#define G729_TEST		4
#define TEST_ALL		5
#define TEST_EN_ALL_CH	6
#define TEST_PCM_LOOP	7
#define TEST_G729_GAIN_16	8
#define TEST_G729_LB_AND_HPF 9
#define TEST_G729_VAD_CD_HPF_AGC_NO_LB	10
#define TEST_NORMAL	11
#define TEST_SYS_CONFIG	12
#define TEST_AUDIO_MUX	13


//#define TEST_TYPE  CD_TEST
//#define TEST_TYPE  HPF_TEST
//#define TEST_TYPE  AGC_DAC_TEST
//#define TEST_TYPE  AGC_ADC_TEST
//#define TEST_TYPE  G729_TEST
//#define TEST_TYPE  TEST_ALL
//#define TEST_TYPE  TEST_EN_ALL_CH
//#define TEST_TYPE  TEST_PCM_LOOP
//#define TEST_TYPE  TEST_G729_GAIN_16
//#define TEST_TYPE  TEST_G729_LB_AND_HPF
//#define TEST_TYPE  TEST_G729_VAD_CD_HPF_AGC_NO_LB
//#define TEST_TYPE  TEST_SYS_CONFIG
//#define TEST_TYPE  TEST_AUDIO_MUX
#define TEST_TYPE  TEST_NORMAL

//#define DEBUG_HOST_CTRL	1	//remove comment to activate debug mode- configure board as if it was done from arm

//#define OLD_AUDIO_INTERFACE

#define ENABLE_SYSTEM_RESTART	1

//#define AUDIO_MUX_OLD_VOLUME	1

#define	CD_MONITOR_CHANNEL	0//ilan
#define	VAD_MONITOR_CHANNEL	0//ilan

#define DEUBAG_HOST_CHANNEL	30
#define DEBUG_HOST_CTRL_VOCODER HD_G711_FRAME

//#define DEBUG_STACK_MONITORING	1



/**************************************************************************/                     
//  Module		: initIrq
//  Description : Initialize and enable the DMA receive interrupt using the CSL.
//	              The interrupt service routine for this interrupt is edmaHwi.
///					
/// @return none
/// @param	none
/**************************************************************************/
void initIrq(void);

/**************************************************************************/                     
//  Module		: McBsp_powerup_bit
//  Description : check McBsp serial channel are working 
///					
/// @return none
/// @param	none
/**************************************************************************/
void McBsp_powerup_bit(void);


/**************************************************************************/                     
//  Module		: McBsp_loop_bit
//  Description : check McBsp serial channel are working by sending and 
//					receiving data (as default McBsp are set in loop mode)
///					
/// @return none
/// @param	none
/**************************************************************************/
int McBsp_loop_test(int port);
#endif	//_MAIN__H_
