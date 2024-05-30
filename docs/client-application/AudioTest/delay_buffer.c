/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : Delay buffer that will add delay to audio channel.
///
///					this module may be configure in the following modes
///				    In modes:
///					HD_DLY_MODE_IN_PTT_CD dsp will insert audio to 
///						buffer only if there is PTT+CD, and sending audio 
///						to IDEN after audio from IDEN detected (BIP)
///					HD_DLY_MODE_IN_PTT - dsp will insert audio to 
///						buffer only if there is PTT
///					Out modes:
///					HD_DLY_MODE_OUT_CD - DSP send audio to IDEN after
///					 	audio from IDEN detected (BIP)
///					HD_DLY_MODE_OUT_TRANSMIT - DSP send audio to IDEN 
///						after receiving transmit signal from arm
///					HD_DLY_MODE_OUT_FULL_BUFFER - send audio to IDEN 
///						after buffer is full (constant buffer delay) 
///					the module have diffrent 5 states:
///					IDLE - Buffer cleaning
///					WAIT IN- wait for PTT or PTT+CD signals depending 
///						on configured mode
///					IN - insert audio to buffer, move to next stage 
///						will be after receiving transmit signal from ARM 
///						or after receiving CD from input channel
///					IN_OUT - insert audio to buffer and output audio 
///						from buffer, move to next stage after receiving 
///						PTT RELEASE from ARM
///					OUT - output audio from buffer, move to IDLE 
///						after the buffer is empty.
///	@file:	delay_buffer.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	23/Sept/2012	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/
#include <stdint.h>
#include <c6x.h>


#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include <xdc/cfg/global.h>

#include <log.h>
#include <mem.h>
#include <string.h>
#include "DigitalCore_hpi.h"
#include "host_ctrl.h"
#include "delay_buffer.h"

//#define DLYBUF_TEST_ENABLE
#define DLYBUF_NSAMPLES 80


enum  STATES{DLYBUF_IDLE_STATE, DLYBUF_WAIT_VOICE_STATE, 
			DLYBUF_IN_STATE, DLYBUF_IN_OUT_STATE, DLYBUF_OUT_STATE};
 
extern Int ISRAM;
extern Int SDRAM;
//extern Int DLYBUFF;
//extern far LOG_Obj LOG_trace;

extern HD_host_dsp_ctrl  hd_host_ctrl;
extern unsigned short CD_State[HD_MAX_CHANNELS][2];



typedef struct delay_buff
{

	uint16_t in;
	uint16_t out;
	int16_t *ptr;
	
}delay_buff;


//internal struct define pointer to buffer (dynamic allocated), mode and state 
typedef struct dlyStruct
{	
	HD_dly_init_param init; //parameter that init the module
	HD_dly_ctrl_param ctrl; //parameter that control the module
	delay_buff buffer; //audio buffer pointer and indexes
	uint8_t state;	//delay buffer state
	uint16_t channel;	//associated audio channel
	uint16_t ch_in_out;	//associated audio ch_in_out
	uint16_t out_dly_count;//counter to make delay before sending audio out after 
							//transmit or cd was received	
}dlyStruct;

static dlyStruct dly[HD_MAX_DLY_BUFFER_CHANNELS];
//extension delay buffer, interface with Host 
#pragma DATA_SECTION	(DLYBUF_extension,".ISRAM_CODE")
HD_dly_ext DLYBUF_extension;

static int16_t audio_in[DLYBUF_NSAMPLES];
#ifdef DLYBUF_TEST_ENABLE
static int16_t test_audio_in[DLYBUF_NSAMPLES];
static int16_t test_audio_out[DLYBUF_NSAMPLES];
#endif
//static function declaration
/**************************************************************************/                     
//  Module		: dly_buff_in
//  Description : insert audio to delay buffer
/// @return 0 if succeed, DLYBUF_DELAY_BUFF_FULL if buffer is full
/// @param	ch - delay buffer channel
/// @param	audio - pointer to audio buffer
/**************************************************************************/
static int16_t dly_buff_in(uint16_t ch,int16_t *audio);
/**************************************************************************/                     
//  Module		: dly_buff_out
//  Description : get audio from delay buffer
/// @return 0 if succeed, DLYBUF_DELAY_BUFF_EMPTY if buffer is empty
/// @param	ch - delay buffer channel
/// @param	audio - pointer to audio buffer
/**************************************************************************/
static int16_t dly_buff_out(uint16_t ch,int16_t *audio);
/**************************************************************************/                     
//  Module		: find_channel
//  Description : check if channel number is associated with one of the 
///					delay buffer channel 
/// @return return the channel number and -1 if not found 
/// @param	channel - audio channel number 
/// @param	ch_in_out - HD_DSP_TO_HOST or HD_HOST_TO_DSP channel
/**************************************************************************/
static int find_channel(uint16_t channel, uint16_t ch_in_out);


/**************************************************************************/                     
//  Module		: find_free_channel
//  Description : find the first free channel availeble 
/// @return first channel not in use 
/**************************************************************************/
static int find_free_channel(void);

/**************************************************************************/                     
//  Module		: close_all_channel
//  Description : close all opened channel 
/**************************************************************************/
static void close_all_channel(void);

/**************************************************************************/                     
//  Module		: DLYBUF_init()
//  Description : initialize internal struct
/// @return none
/// @param	none
/**************************************************************************/
void DLYBUF_init()
{
	int i;
	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
 		dly[i].init.buffer_size=0;
		dly[i].init.in_mode=0;
		dly[i].init.out_mode=0;
		dly[i].channel=HD_MAX_CHANNELS;
		dly[i].ch_in_out=0;
		dly[i].init.cd_channel=0;
		dly[i].init.cd_in_out=0;
		dly[i].init.out_delay=0;		
		dly[i].ctrl.ptt=0;
		dly[i].ctrl.transmit=0;
		dly[i].ctrl.enable=0;
		dly[i].state=DLYBUF_IDLE_STATE;
		dly[i].buffer.ptr=NULL;
		dly[i].buffer.in=0;
		dly[i].buffer.out=0;
		dly[i].out_dly_count=0;
		
	}	

}
/**************************************************************************/                     
//  Module		: DLYBUF_open
//  Description : 
/**************************************************************************/
int DLYBUF_open(uint16_t channel, uint16_t ch_in_out,HD_dly_init_param param)
{
	int ch;
	//find dly buffer channel from audio channel
	ch=find_channel(channel,ch_in_out);
	if (ch>=0)	
		return DLYBUF_CH_EXIST_OPEN_FAILED;		
	if (param.buffer_size>DLYBUF_MAX_BUFFER_SIZE)
		return DLYBUF_SIZE_REQ_FAILED;
	ch=find_free_channel();	
	if (ch>=HD_MAX_DLY_BUFFER_CHANNELS)
		return DLYBUF_NO_CH_AVAILEBLE_FAILED;		
	//copy init parameters 	
	dly[ch].init=param;
	dly[ch].channel=channel;
	dly[ch].ch_in_out=ch_in_out;	
	//allocate memory for delay buffer
	IHeap_Handle heap =(IHeap_Handle)HeapBuf_Handle_upCast(DLYBUFF);
	dly[ch].buffer.ptr=(int16_t*)Memory_alloc (heap,sizeof(int16_t)*
		dly[ch].init.buffer_size*DLYBUF_NSAMPLES,4, NULL);
	if ((int)dly[ch].buffer.ptr==0)
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_MEMO_ALLOC;
		dly[ch].channel=HD_MAX_CHANNELS;//init channel so it can be use later
		return (DLYBUF_MEMO_ALLOC_OPEN_FAILED);
	}
	//clear delay buffer extension memory
	memset(dly[ch].buffer.ptr,0,sizeof(int16_t)*dly[ch].init.buffer_size*DLYBUF_NSAMPLES);
	dly[ch].ctrl.ptt=0;
	dly[ch].ctrl.transmit=0;
	dly[ch].state=DLYBUF_IDLE_STATE;
	
	return 0;
}

/**************************************************************************/                     
//  Module		: DLYBUF_close
//  Description : 
/**************************************************************************/
void DLYBUF_close(uint16_t channel, uint16_t ch_in_out)
{
	int ch;
	//find dly buffer channel from audio channel
	ch=find_channel(channel,ch_in_out);
	if (ch>=0)
	{
	    IHeap_Handle heap =(IHeap_Handle)HeapBuf_Handle_upCast(DLYBUFF);
		Memory_free(heap,dly[ch].buffer.ptr,
			sizeof(int16_t)*dly[ch].init.buffer_size*DLYBUF_NSAMPLES);
		dly[ch].buffer.ptr=NULL;
		dly[ch].channel=HD_MAX_CHANNELS;//init channel so it can be use later
	}			
			
}			

/**************************************************************************/                     
//  Module		: DLYBUF_delay
//  Description : implement delay buffer 
/**************************************************************************/
int DLYBUF_delay(uint16_t channel,uint16_t ch_in_out ,int16_t *audio_input
				,int16_t *audio_out)
{
	int ret, ch, repeat;
	
	//find dly buffer channel from audio channel
	ch=find_channel(channel,ch_in_out );
	if (ch<0)	
		return DLYBUF_NO_CH_DELAY_FAILED;	
	memcpy(audio_in,audio_input,DLYBUF_NSAMPLES*sizeof(int16_t));
	do
	{
		
		repeat=0;
		switch(dly[ch].state)
		{
			case DLYBUF_IDLE_STATE:
				//clear buffer index, if ptt and audio(Carrier Detect) move to
				//in state
				dly[ch].buffer.in=0;
				dly[ch].buffer.out=0;
				dly[ch].out_dly_count=0;
				//inform host the buffer is empty
				DLYBUF_extension.status[channel][ch_in_out]=0;
				
				if (dly[ch].ctrl.ptt)
				{
					if (dly[ch].init.in_mode==HD_DLY_MODE_IN_PTT_CD)
						dly[ch].state=DLYBUF_WAIT_VOICE_STATE;
					else
					{
						dly[ch].state=DLYBUF_IN_STATE;
						//inform host the buffer is not empty
						DLYBUF_extension.status[channel][ch_in_out]|=HD_DLY_STATUS_BUFFER_NOT_EMPTY;	
					}	
					repeat=1;
				}
				else
					memset(audio_out,0,DLYBUF_NSAMPLES*sizeof(int16_t));
			break;
			case DLYBUF_WAIT_VOICE_STATE:			
				//check if voice detected
				if (CD_State[dly[ch].channel][dly[ch].ch_in_out])
				{
					dly[ch].state=DLYBUF_IN_STATE;
					repeat=1;//voice detected jump imidiatly to next 
							//state (reapet loop and do not exit)
					//inform host the buffer is not empty
					DLYBUF_extension.status[channel][ch_in_out]|=HD_DLY_STATUS_BUFFER_NOT_EMPTY;								
				}	
				//move to in out if radio is ready (transmit on)	
				if (dly[ch].ctrl.transmit)
				{
					dly[ch].state=DLYBUF_IN_OUT_STATE;
					repeat=1;
					//inform host the buffer is not empty
					DLYBUF_extension.status[channel][ch_in_out]|=HD_DLY_STATUS_BUFFER_NOT_EMPTY;	
				}	
				//if no ptt go back to idle (we do not have audio anyway)	
				if (dly[ch].ctrl.ptt==0)
					dly[ch].state=DLYBUF_IDLE_STATE;
				if (repeat==0)
					memset(audio_out,0,DLYBUF_NSAMPLES*sizeof(int16_t));	
						
						
			break;
			case DLYBUF_IN_STATE:
				//insert audio to buffer, 
				//if buffer full, return to idle state or to IN_OUT state in 
				//case we are in full mode 
				//if transmit go to in_out state 
				ret=dly_buff_in(ch, audio_in);			
				if (ret==DLYBUF_BUFF_FULL)
				{
					if (dly[ch].init.out_mode==HD_DLY_MODE_OUT_FULL)
						dly[ch].state=DLYBUF_IN_OUT_STATE;
					else
						dly[ch].state=DLYBUF_IDLE_STATE;
					repeat=1;						
				}
				if (dly[ch].init.out_mode==HD_DLY_MODE_OUT_TRANSMIT)
				{	
					if (dly[ch].ctrl.transmit)
					{	
						//after transmit signale was received start the delay counter					
						if (dly[ch].out_dly_count==0)
							dly[ch].out_dly_count++;	
					}
						
				}
				if (dly[ch].init.out_mode==HD_DLY_MODE_OUT_CD)
				{
					//check if voice detected
					if (CD_State[dly[ch].init.cd_channel][dly[ch].init.cd_in_out])
					{						
						//after carrier detected start the delay counter  
						if (dly[ch].out_dly_count==0)
							dly[ch].out_dly_count++;	
					}
		
				}
				//delay before in_out state after cd or transmit was received
				if (dly[ch].out_dly_count>0)
				{
					dly[ch].out_dly_count++;
					if (dly[ch].out_dly_count>dly[ch].init.out_delay)
						dly[ch].state=DLYBUF_IN_OUT_STATE;			
				}
				if (repeat==0)
					memset(audio_out,0,DLYBUF_NSAMPLES*sizeof(int16_t));
					
										
			break;
	
			case DLYBUF_IN_OUT_STATE:
						
				//insert audio and output audio, if no transmit signal, go back to in state
				//if no ptt, go to out state
				if ((dly[ch].init.out_mode==HD_DLY_MODE_OUT_TRANSMIT)&&
					(dly[ch].ctrl.transmit==0))
				{	
					dly[ch].state=DLYBUF_IN_STATE;
					repeat=1;
				}	
				if (dly[ch].ctrl.ptt==0)
				{
					dly[ch].state=DLYBUF_OUT_STATE;
					repeat=1;
				}
				if (repeat==0)
				{
					dly_buff_out(ch, audio_out);
					dly_buff_in(ch, audio_in);
				}	
				dly[ch].out_dly_count=0;					
			break;
	
			case DLYBUF_OUT_STATE:
				//output audio, if buffer is empty or no mic go to idle state 
				//if ptt go to in_out state		
				ret=dly_buff_out(ch, audio_out);
				if	((ret==DLYBUF_BUFF_EMPTY)||
					((dly[ch].init.out_mode==HD_DLY_MODE_OUT_TRANSMIT)&&
					(dly[ch].ctrl.transmit==0)))
				{	
					dly[ch].state=DLYBUF_IDLE_STATE;
					repeat=1;				
				}	
				if (dly[ch].ctrl.ptt)
					dly[ch].state=DLYBUF_IN_OUT_STATE;
			
			break;
		
		
		}
	}while (repeat);	
	return (0);
}

/**************************************************************************/                     
//  Module		: dly_buff_in
//  Description : insert audio to delay buffer
/// @return 0 if succeed, DLYBUF_DELAY_BUFF_FULL if buffer is full
/// @param	ch - delay buffer channel
/// @param	audio - pointer to audio buffer
/**************************************************************************/
int16_t dly_buff_in(uint16_t ch,int16_t *audio)
{

	uint32_t diff,size=dly[ch].init.buffer_size;
	//check if buffer is full
	if (dly[ch].buffer.out>dly[ch].buffer.in)
		diff=size-(dly[ch].buffer.out-dly[ch].buffer.in);
	else
		diff=dly[ch].buffer.in-dly[ch].buffer.out;

	if (diff>=(size-1))
		return(DLYBUF_BUFF_FULL);

	if (dly[ch].buffer.in>=size)
		dly[ch].buffer.in=0;
	//insert audio to buffer
	memcpy(&dly[ch].buffer.ptr[DLYBUF_NSAMPLES*dly[ch].buffer.in],audio,DLYBUF_NSAMPLES*sizeof(int16_t));

	//increment index
	dly[ch].buffer.in++;

	return(0);
}

/**************************************************************************/                     
//  Module		: dly_buff_out
//  Description : get audio from delay buffer
/// @return 0 if succeed, DLYBUF_DELAY_BUFF_EMPTY if buffer is empty
/// @param	ch - delay buffer channel
/// @param	audio - pointer to audio buffer
/**************************************************************************/
int16_t dly_buff_out(uint16_t ch,int16_t *audio)
{

	//check if buffer is empty	
	if (dly[ch].buffer.in==dly[ch].buffer.out)
		return(DLYBUF_BUFF_EMPTY);
	if (dly[ch].buffer.out>=dly[ch].init.buffer_size)
		dly[ch].buffer.out=0;
	
	//get audio from buffer
	memcpy(audio,&dly[ch].buffer.ptr[DLYBUF_NSAMPLES*dly[ch].buffer.out],DLYBUF_NSAMPLES*sizeof(int16_t));

	//increment index
	dly[ch].buffer.out++;

	return(0);
}

/**************************************************************************/                     
//  Module		: find_channel
//  Description : check if channel number is associated with one of the 
///					delay buffer channel 
/// @return return the channel number and -1 if not found 
/// @param	channel - audio channel number 
/**************************************************************************/
int find_channel(uint16_t channel, uint16_t ch_in_out)
{
	int i;
	//serch for the delay bufer
	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
		if ((dly[i].channel==channel)&&(dly[i].ch_in_out==ch_in_out))
			break;	
	}
	//TODO verify for i=HD_MAX_DLY_BUFFER_CHANNELS
	//check if the channel was already in use
	if (i>=HD_MAX_DLY_BUFFER_CHANNELS)
		return -1;
	return i;	
}


/**************************************************************************/                     
//  Module		: find_free_channel
//  Description : find the first free channel availeble 
/// @return first channel not in use 
/**************************************************************************/
int find_free_channel(void)
{
	int i;
	//serch for the delay bufer
	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
		if (dly[i].buffer.ptr==NULL)
			break;	
	}
	//TODO verify for i=HD_MAX_DLY_BUFFER_CHANNELS
	return i;
}

/**************************************************************************/                     
//  Module		: close_all_channel
//  Description : close all opened channel 
/**************************************************************************/
void close_all_channel(void)
{
	int i;
	//serch for the delay bufer
	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
		if (dly[i].buffer.ptr!=NULL)
		{
			DLYBUF_close(dly[i].channel,dly[i].ch_in_out);
		}
			
	}

}

/**************************************************************************/                     
//  Module		: DLYBUF_config
//  Description : update control parameters
/// @return 0 if succeed
/// @param	channel - audio cahnnel number
/// @param	param - parameter to init the module
/**************************************************************************/
int DLYBUF_config(uint16_t channel, uint16_t ch_in_out,HD_dly_ctrl_param param)
{
	int ch;
	//find dly buffer channel from audio channel
	ch=find_channel(channel,ch_in_out);
	if (ch<0)
		return -1;
	dly[ch].ctrl=param;
	if (param.enable==0)
		dly[ch].state=DLYBUF_IDLE_STATE;

	return 0;
}		


#ifdef DLYBUF_TEST_ENABLE
/**************************************************************************/                     
//  Module		: DLYBUF_test
//  Description : implement unit tests for delay buffer module
/// @return 0 if succeed
/// @param	none
/**************************************************************************/
int DLYBUF_test()
{
	int i,j,ret,size;
	LOG_printf(&LOG_trace,"delay-buffer test");
	DLYBUF_init();
	//---------test open module-------------
	DLYBUF_extension.init.param.buffer_size=100*5;
	DLYBUF_extension.init.param.in_mode=HD_DLY_MODE_IN_PTT_CD;
	DLYBUF_extension.init.param.out_mode=HD_DLY_MODE_OUT_CD;


	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
		DLYBUF_extension.init.param.cd_channel=i;
		DLYBUF_extension.init.param.cd_in_out=HD_ADC;
		ret=DLYBUF_open(i,HD_DAC,DLYBUF_extension.init.param);
		if (ret)
			LOG_printf(&LOG_trace,"dly_buff_test:error opening file");
	}
	//test overload opend channels
	ret=DLYBUF_open(i+1,HD_DAC,DLYBUF_extension.init.param);
	if (ret==0)
		LOG_printf(&LOG_trace,"dly_buff_test: error overload open channel");
	
	//test allready opened channel				
	ret=DLYBUF_open(0,HD_DAC,DLYBUF_extension.init.param);
	if (ret==0)
		LOG_printf(&LOG_trace,"dly_buff_test: error allready opened channel");
	
	ret=DLYBUF_open(i-1,HD_DAC,DLYBUF_extension.init.param);	
	if (ret==0)
		LOG_printf(&LOG_trace,"dly_buff_test: error allready opened channel");

	for (i=0;i<HD_MAX_DLY_BUFFER_CHANNELS;i++)
	{
	//close and open channel
		DLYBUF_close(i,HD_DAC);					
		ret=DLYBUF_open(i,HD_DAC,DLYBUF_extension.init.param);
		if (ret!=0)
			LOG_printf(&LOG_trace,"dly_buff_test: error close and open channel");
	}		
	
	close_all_channel();
										
	//---------test in/out-------------
	//test B1:test operation in mode input when cd+ptt, and output on cd 
	DLYBUF_extension.init.param.cd_channel=0;
	DLYBUF_extension.init.param.cd_in_out=HD_ADC;
	DLYBUF_extension.init.param.out_delay=50;//500msec after bip
	CD_State[0][HD_DAC]=0;
	CD_State[0][HD_ADC]=0;
	ret=DLYBUF_open(0,HD_DAC,DLYBUF_extension.init.param);
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B1: error in open");	
	//insert data to buffer
	for (i=0;i<DLYBUF_NSAMPLES;i++)
	{
		test_audio_in[i]=i;
		test_audio_out[i]=0;
	}
	for (i=0;i<100;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		if (ret!=0)
			LOG_printf(&LOG_trace,"DLYBUF test B1: error in delay(idle)");
		for (j=0;j<DLYBUF_NSAMPLES;j++)
		{
			if (test_audio_out[j]!=0)
			{
				LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(idle)");	
				break;
			}
		}				
	}		
	//send ptt
	DLYBUF_extension.ctrl.param.ptt=1;
	DLYBUF_extension.ctrl.param.transmit=0;
	DLYBUF_extension.ctrl.param.enable=1;
	ret=DLYBUF_config(0,HD_DAC,DLYBUF_extension.ctrl.param);
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B1: error in config");	
	//send audio for 1sec
	for (i=0;i<100;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		if (ret!=0)
			LOG_printf(&LOG_trace,"DLYBUF test B1: error in delay(start ptt)");
		for (j=0;j<DLYBUF_NSAMPLES;j++)
		{
			if (test_audio_out[j]!=0)
			{
				LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(start ptt)");	
				break;
			}
		}				
	}	
	//send cd indication (out ch)-audio start
	CD_State[0][HD_DAC]=1;
	//send audio for 1sec
	for (i=0;i<100;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);	
		if (ret!=0)
			LOG_printf(&LOG_trace,"DLYBUF test B1: error in delay (start audio)");
		for (j=0;j<DLYBUF_NSAMPLES;j++)
		{
			if (test_audio_out[j]!=0)
			{
				LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(start audio)");	
				break;
			}
		}				
	}	
	//send cd indication (in ch)-start of transmit
	CD_State[0][HD_ADC]=1;
	//send audio for 10 sec
	for (i=0;i<1000;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		if (ret!=0)
			LOG_printf(&LOG_trace,"DLYBUF test B1: error in delay (start transmit)");
		if (i>=50)
		{				
			ret=memcmp(test_audio_in,test_audio_out,sizeof(test_audio_in));	
			if (ret!=0)
				LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(start transmit)");			
		}
		else
		{
			for (j=0;j<DLYBUF_NSAMPLES;j++)
			{
				if (test_audio_out[j]!=0)
				{
					LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(start transmit)");	
					break;
				}
			}		
		}
	}
	//stop sending ptt
	DLYBUF_extension.ctrl.param.ptt=0;
	ret=DLYBUF_config(0,HD_DAC,DLYBUF_extension.ctrl.param);	
	//send audio for 2sec and check audio comes out for 1sec
	for (i=0;i<200;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		if (ret!=0)
			LOG_printf(&LOG_trace,"DLYBUF test B1: error in delay(stop ptt)");
		if (i<150)
		{				
			ret=memcmp(test_audio_in,test_audio_out,sizeof(test_audio_in));	
			if (ret!=0)
				LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(stop ptt)");
		}
		else
		{		
			for (j=0;j<DLYBUF_NSAMPLES;j++)
			{
				if (test_audio_out[j]!=0)
				{
					LOG_printf(&LOG_trace,"DLYBUF test B1: error in audio in(stop ptt)");	
					break;
				}
			}
		}								
	
	} 
	close_all_channel();


	//------------------------------------------------------------
	//check no loss of data
	//ptt for 2 sec, stop ptt, bip signal (cd on input from transmitter)
	//we will send samples only on start and stop, and 0 in the middle
	//we will check start and stop frame are correct
	DLYBUF_extension.init.param.cd_channel=0;
	DLYBUF_extension.init.param.cd_in_out=HD_ADC;
	DLYBUF_extension.init.param.out_delay=50;//500msec after bip
	CD_State[0][HD_DAC]=0;
	CD_State[0][HD_ADC]=0;
	ret=DLYBUF_open(0,HD_DAC,DLYBUF_extension.init.param);
	//insert data to buffer
	for (i=0;i<DLYBUF_NSAMPLES;i++)
	{
		test_audio_in[i]=i;
		test_audio_out[i]=0;
	}

	//ptt+cd , ptt for 2sec
	//send ptt
	DLYBUF_extension.ctrl.param.ptt=1;
	DLYBUF_extension.ctrl.param.transmit=0;
	DLYBUF_extension.ctrl.param.enable=1;
	ret=DLYBUF_config(0,HD_DAC,DLYBUF_extension.ctrl.param);
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B1: error in config");
	//set cd
	CD_State[0][HD_DAC]=1;
	//insert first non zero data
	ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	//set cd
	CD_State[0][HD_DAC]=0;
	//insert 2 sec of 0
	memset(test_audio_in,0,sizeof(test_audio_in));
	for (i=0;i<200;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	}
	//insert LLAST non zero data
	for (i=0;i<DLYBUF_NSAMPLES;i++)
		test_audio_in[i]=i;
	ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	//stop ptt for 1 sec
	DLYBUF_extension.ctrl.param.ptt=0;
	DLYBUF_extension.ctrl.param.transmit=0;
	DLYBUF_extension.ctrl.param.enable=1;
	ret=DLYBUF_config(0,HD_DAC,DLYBUF_extension.ctrl.param);
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B1: error in config");
	for (i=0;i<100;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	}
	//set cd for bip and wait it will pass
	CD_State[0][HD_ADC]=1;
	for (i=0;i<50;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		//check out is 0
		for (j=0;j<DLYBUF_NSAMPLES;j++)
		{
			if (test_audio_out[j]!=0)
			{
				LOG_printf(&LOG_trace,"DLYBUF test B2: error in middle frames");
				break;
			}
		}
	}
	ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	//check first frame
	ret=memcmp(test_audio_in,test_audio_out,sizeof(test_audio_in));
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B2: error in start frame");

	for (i=0;i<200;i++)
	{
		ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
		for (j=0;j<DLYBUF_NSAMPLES;j++)
		{
			if (test_audio_out[j]!=0)
			{
				LOG_printf(&LOG_trace,"DLYBUF test B2: error in middle frames");
				break;
			}
		}
	}

	ret=DLYBUF_delay(0,HD_DAC ,test_audio_in,test_audio_out);
	//check last frame
	ret=memcmp(test_audio_in,test_audio_out,sizeof(test_audio_in));
	if (ret!=0)
		LOG_printf(&LOG_trace,"DLYBUF test B2: error in last frame");
	close_all_channel();


	//------------------------------------------------------------
	test in and out buffer function

	DLYBUF_extension.init.param.buffer_size=10;
	DLYBUF_extension.init.param.cd_channel=0;
	DLYBUF_extension.init.param.cd_in_out=HD_ADC;
	DLYBUF_extension.init.param.out_delay=0;//0msec after bip
	ret=DLYBUF_open(0,HD_DAC,DLYBUF_extension.init.param);
	size=DLYBUF_extension.init.param.buffer_size;
	dly[0].buffer.in=0;
	dly[0].buffer.out=0;
	dly[0].out_dly_count=0;

	//insert audio more then possible
	for (i=0;i<200;i++)
	{
		ret=dly_buff_in(0,test_audio_in);
		if (((ret==0)&&(i>=size-1))||((ret!=0)&&(i<size-1)))
			LOG_printf(&LOG_trace,"DLYBUF test C1: error in dly_buff_in");
	}
	//get more data then exist
	for (i=0;i<200;i++)
	{
		ret=dly_buff_out(0,test_audio_in);
		if (((ret==0)&&(i>=size-1))||((ret!=0)&&(i<size-1)))
			LOG_printf(&LOG_trace,"DLYBUF test C2: error in dly_buff_out");
	}

	//insert audio more then possible
	for (i=0;i<200;i++)
	{
		ret=dly_buff_in(0,test_audio_in);
		if (i<5)
			dly_buff_out(0,test_audio_in);
		if (((ret==0)&&(i>=14))||((ret!=0)&&(i<14)))
			LOG_printf(&LOG_trace,"DLYBUF test C3: error in dly_buff_in or out");
	}
	dly[0].buffer.in=0;
	dly[0].buffer.out=0;
	dly[0].out_dly_count=0;

	//get audio more then possible
	for (i=0;i<200;i++)
	{
		if (i<14)
			dly_buff_in(0,test_audio_in);
		ret=dly_buff_out(0,test_audio_in);
		if (((ret==0)&&(i>=14))||((ret!=0)&&(i<14)))
			LOG_printf(&LOG_trace,"DLYBUF test C4: error in dly_buff_in or out");
	}
	//test in without out

	while (1){};
	return 0;
}
#endif
