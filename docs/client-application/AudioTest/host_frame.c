/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : handle frame from and to host
///	@file:	host_frame.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	15/Oct/2008	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/


#include <ti/sysbios/hal/Cache.h>
#include <ti/ipc/MessageQ.h>

#include <ti/cmem.h>
#include <ti/ipc/Std.h>
#include <ti/ipc/MessageQ.h>
#include "HeapMem.h"
#include "SharedRegion.h"
#include "MemoryDefs.h"

#include "../shared/AppCommon.h"

#include <std.h>
#include <stdint.h>
#include <stdio.h>
/*#include <c6x.h>
#include <csl.h>
#include <csl_gpio.h>
#include <log.h>
#include <clk.h>*/
#include <string.h>
#include <mem.h>
#include "DigitalCore_hpi.h"
#include "main.h"
#include "audio_processing.h"
#include "system_config.h"
#include <g711.h>
#include "plc_fp.h"
#include "host_ctrl.h"
#include "main.h"
//#include "TI_aes.h"
#include "host_frame.h"
#include "AudioCfg.h"
//#include "g726.h"

#define BIG_DATA_POOL_SIZE 20000 // 20000 uint32_t values * 4 bytes each
#define ROUNDUP(size, align) \
    (UInt32)(((UInt32)(size) + ((UInt32)(align) - 1)) & ~((UInt32)(align) - 1))
//extern far LOG_Obj LOG_trace

typedef struct {
    MessageQ_Handle         hostQue;
    MessageQ_QueueId        slaveQue;
    UInt16                  heapId;
    UInt32                  msgSize;
} App_Module;

static App_Module Module;
;

Int IRAM = 0x8FE00000;
//extern Int IRAM;

extern HD_host_dsp_ctrl	hd_host_ctrl;
//extern AP_audio_local_ctrl audio_local_ctrl;
//extern AP_vocoder_config vocoder_config;
//extern unsigned short CD_State[HD_MAX_CHANNELS][2];
extern uint16_t AP_frame_count;
extern volatile uint16_t AP_frame_host_read ;
//extern AP_device_channel_table dev_channel[];

extern uint32_t HC_vocoder_type[HD_MAX_CHANNELS][2];

//extern GPIO_Handle hGpio;

//extern HD_tel_ext	*tel;//////////////////////////

/*G729ENC_ADT_Instance_t g729_enc_state[HD_MAX_HOST_DSP_CHANNELS];
G729ENC_ADT_Scratch_t g729_enc_scratch;
G729DEC_ADT_Instance_t g729_dec_state[HD_MAX_HOST_DSP_CHANNELS];
G729DEC_ADT_Scratch_t g729_dec_scratch;*/

LowcFE_c	g711_lc[HD_MAX_HOST_DSP_CHANNELS];		/* PLC simulation data */

//incaming and outgoing frame buffer (audio and events)
int16_t* p_host_audio_buff_rcv;
int16_t* p_host_audio_buff_trn;

//indicate were to put next data in outgoing buffer
uint16_t host_audio_buff_trn_index=0;
//indicate number of event frames
uint16_t host_audio_buff_frame_count=0;

uint32_t old_frame_count1[HD_MAX_HOST_DSP_CHANNELS]={0};
uint32_t no_frame_count[HD_MAX_HOST_DSP_CHANNELS]={0};

int16_t g726_buff_in[NSAMPLES];
int16_t g726_buff_out[NSAMPLES];
short g726_reset[HD_MAX_HOST_DSP_CHANNELS][2]={1};
//G726_state g726_state[HD_MAX_HOST_DSP_CHANNELS][2];

//can have value of 0 or 1 index for double buffer
static unsigned int active_buffer=0;

HF_host_frame_type hf_host_frame[HD_MAX_HOST_DSP_CHANNELS];

#pragma DATA_SECTION(host_read_flag,".host")
unsigned int host_read_flag[HOST_PIPE_LENGTH];
#pragma DATA_SECTION(host_write_flag,".host")
unsigned int host_write_flag[HOST_PIPE_LENGTH];

extern int16_t audio_in_buff[HD_MAX_CHANNELS][NSAMPLES];
extern int16_t audio_out_buff[HD_MAX_CHANNELS][NSAMPLES];

extern SC_channel_config	ch_config;

extern channel_events_buff_type channel_events_buff[];

//volatile uint16_t old_vad_flag[HD_MAX_HOST_DSP_CHANNELS]={0};

volatile uint16_t ring_count=0;

extern uint32_t HC_host_audio_error_count;

extern uint32_t dsp_error[];

extern HC_encrypt_config encrypt_config [HD_MAX_HOST_DSP_CHANNELS][2];
/*
typedef struct DEBUG_HOST_FRAME
{
	HD_channels_frame_size header;
	HD_channel_frame 	evt_ch;
	HD_event_frame		evt_data;
	HD_events_tel_ring 	ring;
	//HD_events_tel_status	status;
	HD_channel_frame 	ch0;
	HD_audio_g711_frame	audio0;
	HD_channel_frame	ch1;
	HD_audio_g711_frame	audio1;
	HD_channel_frame 	ch2;
	HD_audio_g711_frame	audio2;
	HD_channel_frame	ch3;
	HD_audio_g711_frame	audio3;
	HD_channel_frame 	ch4;
	HD_audio_g711_frame	audio4;
	HD_channel_frame	ch5;
	HD_audio_g711_frame	audio5;
	HD_channel_frame 	ch6;
	HD_audio_g711_frame	audio6;
	HD_channel_frame	ch7;
	HD_audio_g711_frame	audio7;


}DEBUG_HOST_FRAME;
DEBUG_HOST_FRAME debug_frame;
*/
//volatile int temp_debug=0;

//int16_t temp_frame_c[500];
char dbg_text[50],print_data=1;
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
uint32_t prevFrame=0;
int prevError=0;
int host_frame_in(int16_t *speech_in,int8_t *host_in)
{
	uint16_t host_frames_count=0,i;
	HD_channels_frame_size *header=(HD_channels_frame_size*)host_in;
	HD_channel_frame *frame_header=(HD_channel_frame*)(header+1);
	HD_audio_g729_frame *g729_frame;
	HD_audio_g711_frame *g711_frame;
	HD_audio_pcm_frame *pcm_frame;
	uint32_t temp24bit;
	uint64_t temp40bit;
	int16_t	g729_vad_flag;
	int16_t	g729_frame_erase=0;
	int status=0;
	uint32_t diff_count;
	uint32_t frame_count;
	char g726_law=0;//use ulow for g726
	int ret;
	short g726_rate,byte_count;
	//static uint32_t HF_frame_count=0,HF_count=0;
	//HF_frame_count++;
	//volatile static int tempp,tempp1,tempp2;
//	static int16_t old_c_noise[2]={0};
		
	//check buffer size parameter and num_of_frames parameter is ok
	if (((header->size) <= ch_config.frames_buffer_size)&&
		((header->num_of_frames) <= ch_config.number_of_host_dsp_channel))
	{		
		//read data from every frame
		while (host_frames_count<(header->num_of_frames))			
		{
			host_frames_count++;
	
			//check channel index parameter is ok
			if (((frame_header->channel_index)>(HD_HOST_CHANNELS_BASE+ch_config.number_of_host_dsp_channel))||
				((frame_header->channel_index)<HD_HOST_CHANNELS_BASE))
			{
				status=-1;
				break;
			}
				
			
			//check we didnt pass buffer memory size		
			if ((((int8_t*)frame_header)+(frame_header->size))>(host_in+header->size))
			{
				status=-1;
				hd_host_ctrl.sHD_dsp_status.host_audio_error[frame_header->channel_index] = 1;
				hd_host_ctrl.sHD_dsp_status.host_audio_error_count[frame_header->channel_index]++;
				HC_host_audio_error_count++;
				break;
			}	
		
			switch(frame_header->data_type)
			{
				case HD_PCM_FRAME:
					//uncompressed audio 
					//check frame size is correct
					if((frame_header->size)!=(sizeof(HD_audio_pcm_frame)+sizeof(HD_channel_frame)))
					{
						status=-2;
						break;
					}
					pcm_frame=(HD_audio_pcm_frame*)(frame_header+1);
					
					//process aes128 encryptions
			/*		if((audio_local_ctrl.encrypt_enable_mask[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_ADC])&&
							(encrypt_config[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_ADC].type==HD_ENCRYPT_AES128))
					{
						for (i=0;i<10;i++)
							aes_decrypt(&(((unsigned char*)pcm_frame->audio)[i*16]),
									(unsigned char *)encrypt_config[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_ADC].key);
					}*/

					memcpy (&speech_in[NSAMPLES*(frame_header->channel_index)],
						&(pcm_frame->audio[0]),NSAMPLES*2);
					
			if (vocoder_config.frame_erase_enable[frame_header->channel_index][HD_HOST_TO_DSP])
					{
						//insert frame to history buffer										
					//	g711plc_addtohistory(&g711_lc[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE], &speech_in[NSAMPLES*(frame_header->channel_index)]); /* frame is not erased */
					}
					else
						memcpy (&speech_in[NSAMPLES*(frame_header->channel_index)],
							&(pcm_frame->audio[0]),NSAMPLES*2);
					
					
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_frame_type=HD_PCM_FRAME;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_vad=pcm_frame->carrier_detect;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].done_decoding=1;

					frame_count=pcm_frame->frame_count;
				break;
	
				case HD_G729_FRAME:
					//check frame size is correct
					if((frame_header->size)!=(sizeof(HD_audio_g729_frame)+sizeof(HD_channel_frame)))
					{
						status=-2;
						break;
					}				
					g729_frame=(HD_audio_g729_frame*)(frame_header+1);
					
					//process aes128 encryptions
					//TODO

				   // VAD Control
					g729_vad_flag = g729_frame->vad_flag;				
		
					//if we receive no audio consider it like we didn't receive any frame 
					if (g729_vad_flag==0)
						break;
		           // Frame Erase 
		           //g729_frame_erase=vocoder_config.frame_erase_enable[frame_header->channel_index][HD_DAC];
	
					
					//clean audio because of problems in confort noise value received  
					if (g729_vad_flag==2)
					{
						g729_frame->audio[0]=0;//0x80;//old_c_noise[0];
						g729_frame->audio[1]=0;				
					}
					/*
					//if we received unregular sequence of vad=1 and then 0 
					//without vad 2 as needed 
					if ((old_vad_flag[(frame_header->channel_index)-HD_MAX_CODEC_CHANNELS]==1)
						&&(g729_vad_flag==0))
					{	
						if (vocoder_config.frame_erase_enable[frame_header->channel_index][HD_DAC])
						{
							//frame was lost, set frame erase so the decoder will try to fill 
							// the missing frame 
							g729_frame_erase=1;
						}
						else
						{	
							//simulate confort noise state (vad=2)		
							g729_vad_flag=2;
							g729_frame->audio[0]=0;//0x80;//old_c_noise[0];
							g729_frame->audio[1]=0;//0x70;//old_c_noise[1];	
						}	
					}
	
					old_vad_flag[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE]=g729_vad_flag;		
					*/
					/*
					if ((HF_frame_count%100)==0)
						
					{
						HF_count++;
						if (HF_count>2)
							HF_count=0;
						else
							HF_frame_count-=1;	
						g729_vad_flag=2;//indicate we dont have audio
						g729_frame_erase=1;
						g729_frame->audio[0]=0;//0x80;//old_c_noise[0];
						g729_frame->audio[1]=0;//0x70;//old_c_noise[1];	
						
					}
	
					*/
					g729_frame_erase=0;
				/*	G729AB_ADT_Decode(
						&g729_dec_state[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE],
						(char*)&(g729_frame->audio[0]),
						&speech_in[NSAMPLES*(frame_header->channel_index)], 
						g729_frame_erase,
						g729_vad_flag);*/
	
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_frame_type=HD_G729_FRAME;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_vad=g729_vad_flag;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].done_decoding=1;					
		
					frame_count=g729_frame->frame_count;

				break;
				case HD_G711_FRAME:
					//ret=sprintf(dbg_text,"g711 type %d",g711_frame->a_law_u_law);
					//print_debug(dbg_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);
					//check frame size is correct
					if((frame_header->size)!=(sizeof(HD_audio_g711_frame)+sizeof(HD_channel_frame)))
					{
						status=-2;
						break;
					}				
					g711_frame=(HD_audio_g711_frame*)(frame_header+1);

					//process aes128 decryptions
				/*	if((audio_local_ctrl.encrypt_enable_mask[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_ADC])&&
							(encrypt_config[frame_header->channel_index - HD_MAX_CODEC_CHANNELS][HD_ADC].type==HD_ENCRYPT_AES128))
					{
						for (i=0;i<5;i++)
							aes_decrypt(&(((unsigned char*)g711_frame->audio)[i*16]),
									(unsigned char *)encrypt_config[frame_header->channel_index - HD_MAX_CODEC_CHANNELS][HD_ADC].key);
					}*/
					/*
					//for packet lost test
					if ((HF_frame_count%100)==0)					
					{
						HF_count++;
						if (HF_count>0) //change 0 to the number of packet loss you want to chack
							HF_count=0;
						else
							HF_frame_count-=1;	
						g711_frame->carrier_detect=0;//indicate we dont have audio
					}
					*/
					//check if carrier detect is not 0 - there is an audio packet
				//	if(g711_frame->carrier_detect!=0)
				//	{
						if(g711_frame->a_law_u_law==HD_A_LAW)
							g711_alaw_to_linear(&speech_in[NSAMPLES*(frame_header->channel_index)],(char*)&(g711_frame->audio[0]),NSAMPLES);
						else if(g711_frame->a_law_u_law==HD_U_LAW)
							g711_ulaw_to_linear(&speech_in[NSAMPLES*(frame_header->channel_index)],(char*)&(g711_frame->audio[0]),NSAMPLES);
						else
						{
							if(g711_frame->a_law_u_law==HD_G726_16K)
							{
								g726_rate=2;
								for (i=0;i<NSAMPLES;i++)
								{
									g726_buff_in[i]=(g711_frame->audio[i/4]>>((i%4)*2))& 3;
								}
							}

							else if(g711_frame->a_law_u_law==HD_G726_24K)
							{
								g726_rate=3;
								byte_count=0;
								for (i=0;i<NSAMPLES;i++)
								{
									if (i%8==0)
									{
										temp24bit=g711_frame->audio[byte_count]+(g711_frame->audio[byte_count+1]<<8)+(g711_frame->audio[byte_count+2]<<16);
										byte_count+=3;
									}
									g726_buff_in[i]=temp24bit& 0x7;
									temp24bit>>=3;
								}
							}
							else if(g711_frame->a_law_u_law==HD_G726_32K)
							{
								g726_rate=4;
								for (i=0;i<NSAMPLES;i++)
								{
									g726_buff_in[i]=(g711_frame->audio[i/2]>>((i%2)*4))& 0xf;
								}
							}
							else if(g711_frame->a_law_u_law==HD_G726_40K)
							{
								g726_rate=5;
								byte_count=0;
								for (i=0;i<NSAMPLES;i++)
								{
									if (i%8==0)
									{
										temp40bit=g711_frame->audio[byte_count]+(g711_frame->audio[byte_count+1]<<8)+(g711_frame->audio[byte_count+2]<<16)+
												((uint64_t)g711_frame->audio[byte_count+3]<<24)+((uint64_t)g711_frame->audio[byte_count+4]<<32);
										byte_count+=5;
									}
									g726_buff_in[i]=temp40bit& 0x1f;
									temp40bit>>=5;
								}
							}
							//g726_buff_in[g726]->g726_buff_out[g711]
					//		G726_decode (g726_buff_in, g726_buff_out, NSAMPLES, &g726_law, g726_rate,
					//				g726_reset[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_HOST_TO_DSP], &g726_state[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_HOST_TO_DSP]);
							for (i=0;i<NSAMPLES;i++)
							{
								g711_frame->audio[i]=g726_buff_out[i];
								if (print_data)
								{
									ret=sprintf(dbg_text,"i=%d,audio=0x%x\n",i,g711_frame->audio[i]);
									print_debug(dbg_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);

								}

							}
							print_data=0;
							//g711_frame[g711]->speech_in[pcm]
							g711_ulaw_to_linear(&speech_in[NSAMPLES*(frame_header->channel_index)],(char*)&(g711_frame->audio[0]),NSAMPLES);
						}
						if (vocoder_config.frame_erase_enable[frame_header->channel_index][HD_HOST_TO_DSP])
						{
							//insert frame to history buffer										
							g711plc_addtohistory(&g711_lc[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE], &speech_in[NSAMPLES*(frame_header->channel_index)]); /* frame is not erased */
/*							temp_frame_c[aaa]=g711_frame->frame_count;
							aaa++;
							if (aaa==500)
								aaa=0;
*/								
						}
				//	}
			//		else
			//		{
						//clear buffer when carrier detect is not active
			//			memset(&speech_in[NSAMPLES*(frame_header->channel_index)],0,NSAMPLES*2);
							
			//		}
					//if ((frame_header->channel_index==20)&&(temp_debug))
					//	temp_debug=0;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_frame_type=HD_G711_FRAME;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].last_vad=g711_frame->carrier_detect;
					hf_host_frame[(frame_header->channel_index)-HD_HOST_CHANNELS_BASE].done_decoding=1;

					frame_count=g711_frame->frame_count;
					if(frame_count-1!=prevFrame)
					{
					    prevError++;
					}
					prevFrame=frame_count;
				break;
				
				default:							
					status=-2;
				break;
				
						
			}	
			
			//check frame size parameter is ok
			if(status==-2 )
			{				
				hd_host_ctrl.sHD_dsp_status.host_audio_error[frame_header->channel_index] = 1;
				hd_host_ctrl.sHD_dsp_status.host_audio_error_count[frame_header->channel_index]++;
				HC_host_audio_error_count++;
				break;
			}
			else
			{
				//check if frame_count is incrementing, if not inform missed count, do not inform missed count if counter stoped
				diff_count=frame_count-old_frame_count1[frame_header->channel_index-HD_MAX_CODEC_CHANNELS];

				if ((frame_count)&&(old_frame_count1[frame_header->channel_index-HD_MAX_CODEC_CHANNELS])&&(diff_count!=1))
					hd_host_ctrl.sHD_dsp_status.audio_frame_missed_count++;

				old_frame_count1[frame_header->channel_index-HD_MAX_CODEC_CHANNELS]=frame_count;

			}
						
			//increment frame pointer
			//check frame_header is not bigger then defined buffer size	
			frame_header=(HD_channel_frame*)(((int8_t*)frame_header)+frame_header->size);	
//			if (((int8_t*)frame_header)>((host_in+header->size)-(frame_header->size)))
//			{
//				status=-1;
//				break;
//			}	

				
		
		}
	}
	else
	{
		status=-1;
		
	}
	//signal error if problem was found with input frame
	if (status!=0)
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_HOST_FRAME;
		dsp_error[0] |= HD_DSP_ERR_HOST_FRAME;
	}
	
		
	for(i = 0; i<ch_config.number_of_host_dsp_channel; i++)
	{
		if(hf_host_frame[i].done_decoding==0)
		{
			//no frame was arrived for this channel
			switch(hf_host_frame[i].last_frame_type)
			{
				case HD_PCM_FRAME:
					//frame was lost or no audio frame exist
					if (vocoder_config.frame_erase_enable[i+HD_MAX_CODEC_CHANNELS][HD_HOST_TO_DSP])
					{						
						//frame was lost, activate Packet Lost Concealment(PLC)
						g711plc_dofe(&g711_lc[i], &speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)]);
						
				}
					else
					{
						//clear buffer if no audio received
						memset(&speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)],0,NSAMPLES*2);
					}					
				break;
				
				case HD_G729_FRAME:
					/*if ((vocoder_config.frame_erase_enable[i+HD_HOST_CHANNELS_BASE][HD_HOST_TO_DSP])&&
						(hf_host_frame[i].last_vad==1))
					{
						//frame was lost, set frame erase so the decoder will try to fill 
						// the missing frame 
						g729_frame_erase=1;						
					}
					else*/
					{						
						//no frame arrived probebly we are in between vad 2 frames							
						g729_frame_erase=0;						
					}
					g729_vad_flag=0;
											
				/*	G729AB_ADT_Decode(
						&g729_dec_state[i],
						(char*)(&speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)]),
						&speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)], 
						g729_frame_erase,
						g729_vad_flag);	*/
				
				break;
				
				case HD_G711_FRAME:
					//frame was lost or no audio frame exist
					if (vocoder_config.frame_erase_enable[i+HD_HOST_CHANNELS_BASE][HD_HOST_TO_DSP])
					{						
						//frame was lost, activate Packet Lost Concealment(PLC)
						g711plc_dofe(&g711_lc[i], &speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)]);
						//temp_debug++;
					/*	temp_frame_c[aaa]=0;
							aaa++;
							if (aaa==500)
								aaa=0;						
					*/			
					}
					else
					{
						//clear buffer if no audio received
						memset(&speech_in[NSAMPLES*(i+HD_HOST_CHANNELS_BASE)],0,NSAMPLES*2);
					}				
				break;				
			}
			//inform if we do not receive frames, if we do not receive frame for more then 5 frame ignore (probably stop)
			if (no_frame_count[i]<5)
			{
				hd_host_ctrl.sHD_dsp_status.audio_frame_missed_count++;
				no_frame_count[i]++;
			}
			else if (no_frame_count[i]==5)
			{
				//delete last 5 count as they are because of stop transmitting and not frame lost
				hd_host_ctrl.sHD_dsp_status.audio_frame_missed_count=hd_host_ctrl.sHD_dsp_status.audio_frame_missed_count-5;
				no_frame_count[i]=6;
			}

		}
		else
		{
			no_frame_count[i]=0;
		}
		hf_host_frame[i].done_decoding=0;
	}
	//clear header, so we do not receive same frames again if no frames were sent
	header->size=0;
	header->num_of_frames=0;
	
	return(0);
}



/**************************************************************************/                     
//  Module		: host_frame_out
//  Description : build message to be send to host, call encoder to handel each
//					frame and pass resault to host frames buffer.
//				host frames buffer format:
//				total size(32bit), number of frames(32bit)
//				frame size(32bit),	data type(16bit),	ch index(16bit)
//				frame size(32bit),	data type(16bit),	ch index(16bit)
//				...				
/// @return none
/// @param	none host_out-frames buffer pointer, speech_out - audio buffer pointer
//		ch_num-number of channel, buff_size- size left in frame buffer		
/**************************************************************************/
int host_frame_out(int8_t *host_out,int16_t *speech_out,uint16_t ch_num,uint16_t buff_size)
{
	uint16_t i_channel,frame_type,frame_size,i;
	HD_channels_frame_size *header=(HD_channels_frame_size*)host_out;
	HD_channel_frame *frame_header;
	HD_audio_g729_frame *g729_frame;
	HD_audio_g711_frame *g711_frame;
	HD_audio_pcm_frame *pcm_frame;
	int16_t	g729_vad_enable;
	int16_t	g729_vad_flag;
	//int8_t *events_pointer;
	uint32_t frames_buffer_size_count,num_of_frames_count=0;
	uint32_t temp24bit;
	uint64_t temp40bit;
	char g726_law=0;//use ulow for g726
	short g726_rate,byte_count,j;
	//add event size to the total buffer size
	frames_buffer_size_count=host_audio_buff_trn_index;
	//audio frame start after event frames	
	frame_header=(HD_channel_frame*)(host_out+frames_buffer_size_count);
	//update buffer size
	buff_size-=frames_buffer_size_count;
	
	for(i_channel = 0; i_channel<ch_num; i_channel++)
	{
		frame_type=HC_vocoder_type[i_channel][HD_DAC];
		switch (frame_type)
		{
			case HD_PCM_FRAME:
				//set frame size
				frame_size=sizeof(HD_audio_pcm_frame)+sizeof(HD_channel_frame);
				if ((frames_buffer_size_count+frame_size)>buff_size)
				{
					//we pass frame buffer size
					frame_size=0;
					break;
				}
				frames_buffer_size_count+=frame_size;
				num_of_frames_count++;
					
				//set frame header value:size,type,channel no.	
				frame_header->size=frame_size;
				frame_header->data_type=HD_PCM_FRAME;
				frame_header->channel_index=HD_MAX_CODEC_CHANNELS+i_channel;
				pcm_frame=(HD_audio_pcm_frame*)(frame_header+1);
				pcm_frame->frame_count=AP_frame_count;
				//pcm_frame->carrier_detect=CD_State[HD_MAX_CODEC_CHANNELS+i_channel][HD_DAC];
				memcpy (&(pcm_frame->audio[0]),&speech_out[NSAMPLES*(i_channel+HD_MAX_CODEC_CHANNELS)],NSAMPLES*2);
				//process aes128 encryptions
			/*	if((audio_local_ctrl.encrypt_enable_mask[i_channel][HD_DAC])&&(encrypt_config[i_channel][HD_DAC].type==HD_ENCRYPT_AES128))
				{
					for (i=0;i<10;i++)
						aes_encrypt(&(((unsigned char*)pcm_frame->audio)[i*16]),(unsigned char *)encrypt_config[i_channel][HD_DAC].key);
				}*/
			break;

			case HD_G729_FRAME:	
				//set frame size			
				frame_size=sizeof(HD_audio_g729_frame)+sizeof(HD_channel_frame);
				if ((frames_buffer_size_count+frame_size)>buff_size)
				{
					//we pass frame buffer size
					frame_size=0;
					break;
				}
				frames_buffer_size_count+=frame_size;
				num_of_frames_count++;
									
				//set frame header value:size,type,channel no.
				frame_header->size=frame_size;
				frame_header->data_type=HD_G729_FRAME;
				frame_header->channel_index=HD_MAX_CODEC_CHANNELS+i_channel;

			//	g729_frame=(HD_audio_g729_frame*)(frame_header+1);
				// apply VAD enable mask
			//	g729_vad_enable = vocoder_config.vad_enable[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC];
		//		g729_vad_enable=1;//////////////////
		//		memset(speech_out,0,160);////////////
			//	g729_frame->frame_count=AP_frame_count;
			//	g729_frame->carrier_detect=CD_State[i_channel+HD_MAX_CODEC_CHANNELS][HD_DAC];
		
			/*	if(audio_local_ctrl.vocoder_enable_mask[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC])
				{
					G729AB_ADT_Encode(
						&g729_enc_state[i_channel],
						&speech_out[NSAMPLES*(i_channel+HD_MAX_CODEC_CHANNELS)], 
						(char*)&(g729_frame->audio[0]),
				    	g729_vad_enable,
						&g729_vad_flag);
		
					// VAD Control
					g729_frame->vad_flag=g729_vad_flag;				
				}
				else
				{
					memset(&(g729_frame->audio[0]),0,10);
					g729_frame->vad_flag = 0;
				
				}*/
				//process aes128 encryptions
				//TODO
			break;
			
			case HD_G711_FRAME:
				//set frame size			
				frame_size=sizeof(HD_audio_g711_frame)+sizeof(HD_channel_frame);
				if ((frames_buffer_size_count+frame_size)>buff_size)
				{
					//we pass frame buffer size
					frame_size=0;
					break;
				}
				frames_buffer_size_count+=frame_size;
				num_of_frames_count++;
									
				//set fraem header value:size,type,channel no.
				frame_header->size=frame_size;
				frame_header->data_type=HD_G711_FRAME;
				frame_header->channel_index=HD_MAX_CODEC_CHANNELS+i_channel;


				g711_frame=(HD_audio_g711_frame*)(frame_header+1);
				g711_frame->frame_count=AP_frame_count;
				//g711_frame->carrier_detect=CD_State[HD_MAX_CODEC_CHANNELS+i_channel][HD_DAC];
				
				if(audio_local_ctrl.vocoder_enable_mask[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC])
				{
					
					if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_A_LAW)
					{
						g711_frame->a_law_u_law=HD_A_LAW;						
						g711_linear_to_alaw((char*)&(g711_frame->audio[0]),&speech_out[NSAMPLES*(i_channel+HD_MAX_CODEC_CHANNELS)],NSAMPLES);						
					}
					else if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_U_LAW)
					{
						g711_frame->a_law_u_law=HD_U_LAW;
						g711_linear_to_ulaw((char*)&(g711_frame->audio[0]),&speech_out[NSAMPLES*(i_channel+HD_MAX_CODEC_CHANNELS)],NSAMPLES);
					}
					else
					{
						if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_G726_16K)
						{
							g711_frame->a_law_u_law=HD_G726_16K;
							g726_rate=2;
						}
						else if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_G726_24K)
						{
							g711_frame->a_law_u_law=HD_G726_24K;
							g726_rate=3;
						}
						else if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_G726_32K)
						{
							g711_frame->a_law_u_law=HD_G726_32K;
							g726_rate=4;
						}
						else if(vocoder_config.sub_type[HD_MAX_CODEC_CHANNELS+i_channel][HD_ADC]==HD_G726_40K)
						{
							g711_frame->a_law_u_law=HD_G726_40K;
							g726_rate=5;
						}
						//speech_out[pcm]-> g711_frame[g711]
						g711_linear_to_ulaw((char*)&(g711_frame->audio[0]),&speech_out[NSAMPLES*(i_channel+HD_MAX_CODEC_CHANNELS)],NSAMPLES);
						for (i=0;i<NSAMPLES;i++)
						{
							g726_buff_in[i]=g711_frame->audio[i];
						}
						//g726_buff_in[g711]-> g726_buff_out[g726]
						//G726_encode (g726_buff_in,g726_buff_out, NSAMPLES, &g726_law, g726_rate, g726_reset[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_DSP_TO_HOST]
						//                    , &g726_state[frame_header->channel_index-HD_MAX_CODEC_CHANNELS][HD_DSP_TO_HOST]);
//						//ddddddddddddddddddebug
//						for (i=0;i<NSAMPLES;i++)
//						{
//							g726_buff_out[i]=i & (0x1f>>(5-g726_rate));
//						}
//						g726_buff_out[0]=0xaa;
						//debug end
						//g726_buff_out[g726] -> g711_frame->audio[g726]
						if(g711_frame->a_law_u_law==HD_G726_16K)
						{
							for (i=0;i<(NSAMPLES/4);i++)
							{
								g711_frame->audio[i]=g726_buff_out[i*4]+(g726_buff_out[i*4+1]<<2) + (g726_buff_out[i*4+2]<<4) + (g726_buff_out[i*4+3]<<6);
							}
						}

						else if(g711_frame->a_law_u_law==HD_G726_24K)
						{
							byte_count=0;
							for (i=0;i<NSAMPLES;i++)
							{
								if (i%8==0)
								{
									temp24bit=0;
									for (j=0;j<8;j++)
										temp24bit+=g726_buff_out[i*8+j]<<(j*3);
									g711_frame->audio[byte_count++]=(temp24bit)&0xff;
									g711_frame->audio[byte_count++]=(temp24bit>>8)&0xff;
									g711_frame->audio[byte_count++]=(temp24bit>>16)&0xff;
								}
							}
						}
						else if(g711_frame->a_law_u_law==HD_G726_32K)
						{
							for (i=0;i<(NSAMPLES/2);i++)
							{
								g711_frame->audio[i]=g726_buff_out[i*2]+(g726_buff_out[i*2+1]<<4);
							}
						}
						else if(g711_frame->a_law_u_law==HD_G726_40K)
						{
							byte_count=0;
							for (i=0;i<NSAMPLES;i++)
							{
								if (i%8==0)
								{
									temp40bit=0;
									for (j=0;j<8;j++)
										temp40bit+=g726_buff_out[i*8+j]<<(j*5);
									g711_frame->audio[byte_count++]=(temp40bit)&0xff;
									g711_frame->audio[byte_count++]=(temp40bit>>8)&0xff;
									g711_frame->audio[byte_count++]=(temp40bit>>16)&0xff;
									g711_frame->audio[byte_count++]=(temp40bit>>24)&0xff;
									g711_frame->audio[byte_count++]=(temp40bit>>32)&0xff;

								}
							}
						}

					}
				}
				else
				{
					memset(&(g711_frame->audio[0]),0,NSAMPLES);
				
				}
				//process aes128 encryptions
			/*	if((audio_local_ctrl.encrypt_enable_mask[i_channel][HD_DAC])&&(encrypt_config[i_channel][HD_DAC].type==HD_ENCRYPT_AES128))
				{
					for (i=0;i<5;i++)
						aes_encrypt(&(((unsigned char*)g711_frame->audio)[i*16]),(unsigned char *)encrypt_config[i_channel][HD_DAC].key);
				}*/
			
			break;
			default:
				frame_size=0;			
			break;	
					
											
		
		}
		//increment frame pointer	
		frame_header=(HD_channel_frame*)(((int8_t*)frame_header)+frame_size);	

	}
/*	
	//insert events to buffer
	events_pointer=(int8_t*)frame_header;
	for(i_channel = 0; i_channel<HD_MAX_CODEC_CHANNELS; i_channel++)
	{
		//check that we dont exceed frame buffer memory space
		if((events_pointer+channel_events_buff[i_channel].size)>(host_out+ch_config.frames_buffer_size))
			break;
		if(channel_events_buff[i_channel].size)
		{
			memcpy(events_pointer,channel_events_buff[i_channel].pointer,channel_events_buff[i_channel].size);			
			//increment events pointer	
			events_pointer=events_pointer+channel_events_buff[i_channel].size;
			frames_buffer_size_count+=channel_events_buff[i_channel].size;
			channel_events_buff[i_channel].size=0;
			ring_count++;					
		}

	}
	//clear event buffer 
	memset(events_pointer,0,ch_config.frames_buffer_size-(events_pointer-host_out));
*/
	//set buffer global header include audio and event frame
	header->size=frames_buffer_size_count;
	header->num_of_frames=num_of_frames_count+host_audio_buff_frame_count;
				
	
	return(0);	
}


/**************************************************************************/                     
//  Module		: init_host_audio
//  Description : allocate buffer for audio received and send to host
///					
/// @return none
/// @param	none
/**************************************************************************/

int init_host_audio(void)
{
	uint16_t i,memory_size=0;
	
	p_host_audio_buff_rcv=0;
	p_host_audio_buff_trn=0;
	
	//find the event memory size of every codec
	for(i=0;i<HD_MAX_CODEC_CHANNELS;i++)
	{
		//run on open channel 
		if (ch_config.codec_channel_opened &(((uint64_t)1)<<i))
		{
			//memory_size=memory_size+dev_channel[i].func->memo_size;
		    //memory_size += 0;
		}		
	}
	
	memory_size+=SC_INTERNAL_EVENTS_SIZE;
	//add the audio frame size to the event memory size
	ch_config.frames_buffer_size=ch_config.frames_buffer_size+memory_size;

	//allocate memory for audio received and send to host
	//p_host_audio_buff_rcv=(int16_t*)malloc (ch_config.frames_buffer_size*HOST_PIPE_LENGTH);
//	p_host_audio_buff_rcv=(int16_t*)MEM_alloc (IRAM,(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)+4,4);
	p_host_audio_buff_rcv=(int16_t*)malloc ((ch_config.frames_buffer_size*HOST_PIPE_LENGTH)+4,4);

	if ((int)p_host_audio_buff_rcv==0)
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_MEMO_ALLOC;
		dsp_error[0] |= HD_DSP_ERR_MEMO_ALLOC;
		return (-1);
	}	
	p_host_audio_buff_trn=(int16_t*)malloc (ch_config.frames_buffer_size*HOST_PIPE_LENGTH+4,4);
	//p_host_audio_buff_trn=(int16_t*)MEM_alloc (IRAM,(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)+4,4);

	if ((int)p_host_audio_buff_trn==0)
	{
		hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_MEMO_ALLOC;
		dsp_error[0] |= HD_DSP_ERR_MEMO_ALLOC;
		return (-1);
	}	
	//clear frames buffer	
	memset(p_host_audio_buff_rcv,0,(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)+4);
	memset(p_host_audio_buff_trn,0,(ch_config.frames_buffer_size*HOST_PIPE_LENGTH)+4);


	//signal channel input and output for every host channel
	for (i=HD_MAX_CODEC_CHANNELS;i<(HD_MAX_CODEC_CHANNELS+ch_config.number_of_host_dsp_channel);i++)
	{
		
		if (i<64)
		{
			ch_config.channel_input_a|=(((uint64_t)1)<<i);
			ch_config.channel_output_a|=(((uint64_t)1)<<i);
		}	
		else
		{
			ch_config.channel_input_b|=(((uint64_t)1)<<(i-64));
			ch_config.channel_output_b|=(((uint64_t)1)<<(i-64));
		}
	}
	
	memset (hf_host_frame,0,sizeof(hf_host_frame));
	for (i=0;i<HD_MAX_HOST_DSP_CHANNELS;i++)
	{
		hf_host_frame[i].last_frame_type=0xffff;
	}
	
	host_audio_buff_trn_index=sizeof(HD_channels_frame_size);
	host_audio_buff_frame_count=0;
	
	//inform cpu the size of the frame buffer;
	hd_host_ctrl.sHD_dsp_status.frame_buffer_size=ch_config.frames_buffer_size;

	Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, TRUE);
	return (0);

}



/**************************************************************************/                     
//  Module		: prepare_host_buffer
//  Description : send and receive buffers to host
///					
/// @return none
/// @param	none
/**************************************************************************/
void prepare_host_buffer(void)
{
	//unsigned int iChannel=0;
//	static unsigned int active_buffer=0;
	static unsigned int ready_buffer=0;
	int i=0;
//	uint16_t host_ch_num;
	static uint32_t HF_frame_count=0,HF_count=0;
	HF_frame_count++;

	Int status = 0;
    MessageQ_Params msgqParams;
    char msgqName[32];

    printf("--> Message Queue setup :\n");

    Module.hostQue = NULL;
    Module.slaveQue = MessageQ_INVALIDMESSAGEQ;
    Module.heapId = App_MsgHeapId;
    Module.msgSize = sizeof(App_Msg);

    MessageQ_Params_init(&msgqParams);
    Module.hostQue = MessageQ_create(App_HostMsgQueName, &msgqParams);

    if (Module.hostQue == NULL) {
        printf("Host_frame: Failed creating MessageQ\n");
        status = -1;
        return;
    }

    sprintf(msgqName, App_SlaveMsgQueName, MultiProc_getName(remoteProcId));

    do {
        status = MessageQ_open(msgqName, &Module.slaveQue);
        sleep(1);
    } while (status == MessageQ_E_NOTFOUND);

    if (status < 0) {
        printf("Host_frame: Failed opening MessageQ\n");
        return;
    }

    printf("Host_frame: Host is ready\n");

    MessageQ_Params_init(&msgqParams);

    /* Variables for bigdata setup */

    App_Msg *msg;
//    UInt32 *bigDataLocalPtr;
    UInt16 regionId = 1;
    SharedRegion_Entry *pSrEntryTrn;
    void *sharedRegionAllocPtr = NULL;
    CMEM_AllocParams cmemAttrs;
    UInt32 errorCount = 0;
    SharedRegion_Config sharedRegionConfig;
    SharedRegion_Entry srEntry;

    /* Similar setup for recieve buffer shared region allocation
    UInt16 regionId = 1;
    SharedRegion_Entry *pSrEntryRcv;
    void *sharedRegionAllocPtrRcv = NULL;
    CMEM_AllocParams cmemAttrsRcv;
    SharedRegion_Config sharedRegionConfigRcv;
    SharedRegion_Entry srEntryRcv;
    */

    /* Get the memory allocation using CMEM*/

    printf("--> Host_frame:\n");

    status = CMEM_init();
    if (status < 0) {
        printf("CMEM_init failed\n");
        goto leave;
    }
    printf("CMEM_init success\n");

    int pool_id = CMEM_getPool(BIG_DATA_POOL_SIZE);
    if (pool_id < 0) {
        printf("CMEM_getPool failed\n");
        goto leave;
    }
    printf("CMEM_getPool success\n");

    cmemAttrs.type = CMEM_HEAP;
    cmemAttrs.flags = CMEM_CACHED;
    cmemAttrs.alignment = 0;
    sharedRegionAllocPtr = CMEM_allocPool(pool_id, &cmemAttrs);
    if (sharedRegionAllocPtr == NULL) {
        printf("CMEM_allocPool failed\n");
        return;
    }
    printf("CMEM_allocPool success: Allocated buffer %p\n", sharedRegionAllocPtr);

    SharedRegion_getConfig(&sharedRegionConfig);
    status = SharedRegion_setup(&sharedRegionConfig);
    if (status < 0) {
        printf("SharedRegion_setup failed\n");
        return;
    }
    printf("SharedRegion_setup success\n");

    srEntry.base = sharedRegionAllocPtr;
    srEntry.len = BIG_DATA_POOL_SIZE;
    srEntry.ownerProcId = MultiProc_self();
    srEntry.isValid = TRUE;
    srEntry.cacheEnable = TRUE;
    srEntry.createHeap = FALSE;
    srEntry.cacheLineSize = 128;
    srEntry.name = "SR1";

    /* Will using this pSrEntry to be asigned to the HD_host_dsp_ctrl transfer and recieve buffer */
    status = SharedRegion_setEntry(regionId, &srEntry);
    pSrEntryTrn = SharedRegion_getEntryPtr(regionId);  //pointer for shared region for transferring

    /* TODO: Same for the pSrEntryRcv so that it can be assigned to the recv buffer */
//    pSrEntryRcv = SharedRegion_getEntryPtr(regionId2);  //pointer for shared region for transferring

    printf("Host_frame: SR_1, base 0x%x, len=%x\n", (UInt32)pSrEntry->base, pSrEntry->len);

//	host_ch_num=hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel;
/*
	//Check which buffer is ready from host
	//start from the last buffer if its not ready check the previues one 
	//and so on till you find one that is ready or take the last one if 
	//no ready one have been found
	ready_buffer=active_buffer;
	do
	{
		if (ready_buffer==0)
			ready_buffer=HOST_PIPE_LENGTH-1;
		else	
			ready_buffer=ready_buffer-1;
		i++;
		if (i>HOST_PIPE_LENGTH)
			break;				
	}		
	while (host_write_flag[ready_buffer]!=1);
*/
	//copy buffer from host pipe to process buffer and from process buffer 
	//to host pipe
	
//	for(iChannel = 0; iChannel<host_ch_num; iChannel++)
//	{
//		memcpy(p_host_audio_buff_trn+(iChannel*NSAMPLES)+(active_buffer* host_ch_num*NSAMPLES),&audio_out_buff[HD_MAX_CODEC_CHANNELS+iChannel],NSAMPLES*2);
//		memcpy(&audio_in_buff[HD_MAX_CODEC_CHANNELS+iChannel],p_host_audio_buff_rcv+(iChannel*NSAMPLES)+(active_buffer* host_ch_num*NSAMPLES),NSAMPLES*2);
//	}
	//memset(&audio_out_buff[20][0],500,50);///////////dddddddebug
	//memset(&audio_out_buff[20][50],5000,30);///////////dddddddebug
	
	//build a frames buffer to be send to host

	Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, TRUE);
	host_frame_out(((int8_t *)p_host_audio_buff_trn)+
		(active_buffer* ch_config.frames_buffer_size),
		&audio_out_buff[0][0],ch_config.number_of_host_dsp_channel,
		ch_config.frames_buffer_size);	
	
//	host_frame_out(((int8_t *)p_host_audio_buff_trn)+
//		(active_buffer* ch_config.frames_buffer_size),
//		//&audio_out_buff[0][0],ch_config.number_of_host_dsp_channel,
//		&audio_in_buff[0][0],ch_config.number_of_host_dsp_channel, //debuggggggggggggggggggg
//		ch_config.frames_buffer_size);


	//get a frames buffer from host	
	
//	host_frame_in(&audio_in_buff[0][0],((int8_t *)p_host_audio_buff_rcv)+(active_buffer* ch_config.frames_buffer_size));
	
/*	
	//for debug loopback host channel///////////dddddddebug	
	if ((HF_frame_count%100)==0)
	{
		HF_count++;
		if (HF_count>0)
			HF_count=0;
		else
			HF_frame_count-=1;
		memset(((int8_t *)p_host_audio_buff_rcv)+((active_buffer)* ch_config.frames_buffer_size),0,ch_config.frames_buffer_size);
				
	}
	else
	
		memcpy(((int8_t *)p_host_audio_buff_rcv)+((active_buffer)* ch_config.frames_buffer_size),((int8_t *)p_host_audio_buff_trn)+(active_buffer* ch_config.frames_buffer_size),ch_config.frames_buffer_size);
*/
	//DEBUG!!!!!!use this line to make a loop between in and out
	//memcpy(((int8_t *)p_host_audio_buff_rcv)+((active_buffer)* ch_config.frames_buffer_size),((int8_t *)p_host_audio_buff_trn)+(active_buffer* ch_config.frames_buffer_size),ch_config.frames_buffer_size);

	///host_frame_in(&audio_in_buff[0][0],((int8_t *)p_host_audio_buff_rcv)+(active_buffer* ch_config.frames_buffer_size));
	
	host_write_flag[active_buffer]=0;
	host_read_flag[active_buffer]=0;
		
	if(hd_host_ctrl.sHD_audio_link.host_read_flag==0)
	    AP_frame_host_read++;
	//Update host audio hpi info	

	/*
	hd_host_ctrl.sHD_audio_link.dsp_host_buff_addr=(Uint32)(((Uint32)p_host_audio_buff_trn)+(active_buffer*ch_config.frames_buffer_size)); 	//address of buffer send from DSP to ARM
	hd_host_ctrl.sHD_audio_link.host_read_flag_addr=(Uint32)&host_read_flag[active_buffer]; 	//address of flag to indicate to DSP that ARM read the data
	hd_host_ctrl.sHD_audio_link.host_read_flag=1;	  	//content of the flag to signal that ARM read the data
	hd_host_ctrl.sHD_audio_link.host_dsp_buff_addr=(Uint32)(((Uint32)p_host_audio_buff_rcv)+(active_buffer*ch_config.frames_buffer_size)); 	//address of buffer send from ARM to DSP
	hd_host_ctrl.sHD_audio_link.host_write_flag_addr=(Uint32)&host_write_flag[active_buffer]; //address of flag to indicate to DSP that ARM finished writing data to DSP
	hd_host_ctrl.sHD_audio_link.host_write_flag=1;     	//content of the flag to signal that ARM write data to DSP
    */

    hd_host_ctrl.sHD_audio_link.dsp_host_buff_addr=(Uint32)(pSrEntryTrn->base);  //address of buffer send from DSP to ARM
    hd_host_ctrl.sHD_audio_link.host_read_flag_addr=(Uint32)&host_read_flag[active_buffer];     //address of flag to indicate to DSP that ARM read the data
    hd_host_ctrl.sHD_audio_link.host_read_flag=1;       //content of the flag to signal that ARM read the data

    // TODO: Assiging the pSrEntryRcv to the recieve buffer.
//    hd_host_ctrl.sHD_audio_link.host_dsp_buff_addr=(Uint32)(pSrEntryRcv->base);   //address of buffer send from ARM to DSP
//    hd_host_ctrl.sHD_audio_link.host_write_flag_addr=(Uint32)&host_write_flag[active_buffer]; //address of flag to indicate to DSP that ARM finished writing data to DSP
//    hd_host_ctrl.sHD_audio_link.host_write_flag=1;      //content of the flag to signal that ARM write data to DSP


	//for debug loopback host channel///////////dddddddebug	
//	if ((HF_frame_count%100)==0)
//		hd_host_ctrl.sHD_audio_link.host_dsp_buff_addr=
    //set interrupt to host that data is ready
   // SET_HPI_INTTERUPT_TO_HOST;
    							  	

	active_buffer++;
	if (active_buffer>(HOST_PIPE_LENGTH-1))
		active_buffer=0;

	//debug only_________________________________
	//if(((tel->status[0].ring_status)==1)&&(host_audio_buff_frame_count))
//	if(host_audio_buff_frame_count)
//		memcpy(&debug_frame,(uint8_t *)hd_host_ctrl.sHD_audio_link.dsp_host_buff_addr,sizeof(debug_frame));							
	//__________________________________________	
	host_audio_buff_trn_index=sizeof(HD_channels_frame_size);
	host_audio_buff_frame_count=0;	
	Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, TRUE);
	Cache_wbInv(p_host_audio_buff_trn, ch_config.frames_buffer_size*2,Cache_Type_ALL, TRUE);
				
}

void prepare_host_in_buffer(void)
{
	static unsigned int ready_buffer=0;
	int i=0;
	//Check which buffer is ready from host
	//start from the last buffer if its not ready check the previues one
	//and so on till you find one that is ready or take the last one if
	//no ready one have been found

	ready_buffer=active_buffer;
	do
	{
	    Cache_wbInv(host_write_flag, sizeof(unsigned int)*HOST_PIPE_LENGTH,Cache_Type_ALL, TRUE);
		if (ready_buffer==0)
			ready_buffer=HOST_PIPE_LENGTH-1;
		else
			ready_buffer=ready_buffer-1;
		i++;
		if (i>HOST_PIPE_LENGTH)
			break;
	}
	while (host_write_flag[ready_buffer]!=1);
	Cache_wbInv(p_host_audio_buff_rcv, ch_config.frames_buffer_size*2,Cache_Type_ALL, TRUE);
	host_frame_in(&audio_in_buff[0][0],((int8_t *)p_host_audio_buff_rcv)+(ready_buffer* ch_config.frames_buffer_size));
	//memcpy(&audio_in_buff[HD_MAX_CODEC_CHANNELS][0],&audio_out_buff[HD_MAX_CODEC_CHANNELS][0],HD_MAX_CHANNELS*NSAMPLES*2);
	//host_frame_in(&audio_in_buff[0][0],((int8_t *)p_host_audio_buff_trn)+((!ready_buffer)* ch_config.frames_buffer_size));

}
/**************************************************************************/                     
//  Module		: send_event_frame
//  Description : inset an event frame to frame memory
///					
/// @return none
/// @param	none
/**************************************************************************/
int send_event_frame(void* event, uint16_t size,uint16_t channel,
	uint16_t event_type,uint16_t event_sub_type)
{
	int8_t* pframe_buff;
	HD_channel_frame* ch_header_frame;
	HD_event_frame* event_header_frame;
	uint16_t total_size;
	
	total_size=size+sizeof(HD_channel_frame)+sizeof(HD_event_frame);
	
	//check if we have inufe room for this event frame
	if((ch_config.frames_buffer_size-host_audio_buff_trn_index)<total_size)		
		return (-1);		
	pframe_buff=((int8_t *)p_host_audio_buff_trn)+
		(active_buffer* ch_config.frames_buffer_size)+host_audio_buff_trn_index;
	
	//fill channel header 
	ch_header_frame=(HD_channel_frame*)pframe_buff;
	ch_header_frame->size=total_size;
	ch_header_frame->data_type=HD_EVENT_FRAME;	
	ch_header_frame->channel_index=channel;	
	pframe_buff+=sizeof(HD_channel_frame);
	
	//fill event header
	event_header_frame=(HD_event_frame*)pframe_buff;
	event_header_frame->type=event_type;
	event_header_frame->sub_type=event_sub_type;
	pframe_buff+=sizeof(HD_event_frame);

					
	//copy event frame to frames buffer 
	memcpy(	pframe_buff,event,size);

	//debug only_________________________________
//		memcpy(&debug_frame,((int8_t *)p_host_audio_buff_trn)+
//		(active_buffer* ch_config.frames_buffer_size),sizeof(debug_frame));							
	//__________________________________________	

	host_audio_buff_trn_index+=total_size;
	host_audio_buff_frame_count+=1;
	
	return(0);
}
