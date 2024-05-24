/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : activate all audio process and synchronize the audio
///		capturing 
///	@file:	audio_processing.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	14/Jul/2008	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/

#include <ti/sysbios/knl/Task.h>
#include <std.h>
#include <stdint.h>

#include <c6x.h>
#include <c64.h>
#include <csl.h>
#include <csl_gpio.h>
//#include <csl_irq.h>
#include <tsk.h>
#include <clk.h>
#include <string.h>
/*#include "G729AB_user.h"
#include "hpf.h"
#include "hpf_dspe.h"
#include "agc.h"
#include "agc_dspe.h"
#include "cd.h"
#include "cd_dspe.h"
#include "BoardUtility\BoardUtility.h" *///ilan
#include "DigitalCore_hpi.h"
#include "main.h"
#include "system_config.h"
#include "tone_generate.h"
#include "audio_mux.h"
//#include "preprocessing_speex.h"
//#include "G168_echo_cancel.h"
#include "delay_buffer.h"
#include "filter_dc.h"
#include "filter_navy_headset.h"
//#include "filter_configured.h"
#include "host_ctrl.h"
#include "host_frame.h"
#include "caller_id.h"
#include "dtmf_det.h"
#include "plc_fp.h"
//#include "anc_dspecialists.h"
//#include "TI_aes.h"
#include <log.h>
#include "audio_processing.h"
#include "veuAlg.h"


//extern GPIO_Handle hGpio;
SEM_Obj SEM_audio_in;

//extern far LOG_Obj LOG_trace;

//G729ENC_ADT_Instance_t EncState[HD_MAX_HOST_DSP_CHANNELS];
//G729ENC_ADT_Scratch_t EncScratch;
//G729DEC_ADT_Instance_t DecState[HD_MAX_HOST_DSP_CHANNELS];
//G729DEC_ADT_Scratch_t DecScratch;
/*extern G729ENC_ADT_Instance_t g729_enc_state[HD_MAX_HOST_DSP_CHANNELS];
extern G729ENC_ADT_Scratch_t g729_enc_scratch;
extern G729DEC_ADT_Instance_t g729_dec_state[HD_MAX_HOST_DSP_CHANNELS];
extern G729DEC_ADT_Scratch_t g729_dec_scratch;



HPF_Handle		hHPF[HD_MAX_CHANNELS][2];
AGC_Handle		hAGC[HD_MAX_CHANNELS][2]; // ACG in both directions
CD_Handle		hCD[HD_MAX_CHANNELS][2];

extern SpeexPreprocessState *ps_den[HD_MAX_CHANNELS][2];
extern SpeexPreprocessState *ps_vad[HD_MAX_CHANNELS][2];

IHPF_Fxns   hpf_fxns;
IAGC_Fxns   agc_fxns;
ICD_Fxns    cd_fxns;*/

unsigned short CD_State[HD_MAX_CHANNELS][2];
unsigned short cd_mute[HD_MAX_CHANNELS][2];

uint32_t filter_type[HD_MAX_CHANNELS][2];

AP_input_channel_queue queue_in;
AP_audio_local_ctrl audio_local_ctrl;
AP_vocoder_config vocoder_config;

AP_channel	queue_out[HD_MAX_CHANNELS];
 
extern HD_host_dsp_ctrl  hd_host_ctrl;

//short int 	VADEnable[HD_MAX_CHANNELS];
//short int 	VADFlag[HD_MAX_CHANNELS];
//short int 	FrameErase[HD_MAX_CHANNELS];

//table of dev function, for every device there is a device function struct
AP_device_driver_func dev_func[MAX_DEV_FUNCTION];

//table of pointer to channel device function, connect channel to driver 
AP_device_channel_table dev_channel[MAX_CH_DEV_FUNCTION];


int16_t audio_in_buff[HD_MAX_CHANNELS][NSAMPLES];

int16_t audio_out_buff[HD_MAX_CHANNELS][NSAMPLES];

volatile uint16_t AP_frame_count = 0;
uint32_t debub_frame_count = 0;

extern SC_channel_config	ch_config;
volatile int temp_sem=0;
volatile uint16_t debug_ch_num;

//extern PS_nr_param nr_param[HD_MAX_CHANNELS][2];

uint32_t proc_time,proc_time_s,no_proc_time,no_proc_time_s,start_run_time;

//sign that this is a board after reset
//int g_first_time=0xaa;
//extern PS_ec_param ec_param[HD_MAX_CHANNELS];
//extern LEC_param lec_param[HD_MAX_CHANNELS];
#ifdef DEBUG_STACK_MONITORING
TSK_Stat statbuf;
#endif

extern uint32_t dsp_error[];

//extension delay buffer, interface with Host
extern HD_dly_ext DLYBUF_extension;

extern void CD_init();
extern void HPF_init();
extern void AGC_init();
extern LowcFE_c	g711_lc[HD_MAX_HOST_DSP_CHANNELS];

/**************************************************************************/                     
//  Module		: audio_process
//  Description : activate audio process and synchronize audio capture 
/// @return none
/// @param	none
/**************************************************************************/
void audio_process(unsigned int ch_mask)
{

	int res,channel_index,i;
	uint64_t channel_processed=0;//,temp_in_conf,temp_out_conf;
	uint16_t ch_num;
	uint32_t mcbsp0_mask,mcbsp1_mask,i2c0_mask,i2c1_mask,ch_not_rcv;	
	debub_frame_count=0;
	//AP_frame_count=0;
	//start_devices();
    //initIrq();                 // Initialize interrupts
    //IRQ_globalEnable();        // Re-enable global interrupts
    //HWI_enable(); // Re-enable global interrupts
    ch_config.codec_channel_input=ch_config.channel_input_a & CODEC_CHANNEL_MASK;		

    //start_run_time=CLK_getltime();
    		
	//for(;;)
	{
		channel_processed=0;
		debub_frame_count++;	
		AP_frame_count++;
		
		//update audio control requested from host 
		#ifdef DEBUG_HOST_CTRL	
		debug_host_ctrl_audio_update(DEBUG_BOARD);
		#endif		
		host_ctrl_audio_update();
		
	#ifdef DEBUG_HOST_CTRL	
//		GPIO4_PIN_SET;
	#endif

	#ifdef DEBUG_STACK_MONITORING
		Task_stat(Task_self(),&statbuf);//call func to get status
		//LOG_printf(&LOG_trace,"stack_used %d from %d\n",statbuf.used,statbuf.attrs.stacksize);
		if(statbuf.used>(statbuf.attrs.stacksize*8/10)){
			//LOG_printf(&LOG_trace,"over %d%% of task stack is in use\n",((100*statbuf.used)/statbuf.attrs.stacksize));
			hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_TASK_STACK;
			dsp_error[0] |= HD_DSP_ERR_TASK_STACK;
		}
		//LOG_printf(&LOG_trace,"task stack is in use=%d\n",statbuf.used);
	#endif	


	//proc_time=CLK_gethtime()-proc_time_s-no_proc_time;
	//proc_time_s=CLK_gethtime();
	//GPIO0_PIN_CLR;
	no_proc_time=0;
	if (AP_frame_count>10)
		hd_host_ctrl.sHD_dsp_status.cpu_load=(proc_time*100)/COUNT_PER_10MS;


	//temp_sem=SEM_count(&SEM_audio_in);


		//process all channel till all channel defined was processed		
		//while(channel_processed!=(ch_config.codec_channel_input & CODEC_CHANNEL_MASK))
	    for (ch_num= 0; ch_num < HD_MAX_CODEC_CHANNELS;ch_num++)
		{	
			//check if there is new channel ready to process, if no wait
			//we had problem with CLK_gethtime some times there may be error of 61440 counts(1msec)
			//it seems that if we try to read value at the same time that the timer register 
			//change back to 0 we will have this problem. the CLK_gethtime is calculated by multiplay 
			//61440 by the low time count (time that timer registeris full and interrupt clk accured)
			//+the current register value. Chigh=Clow*61440+reg, so at the transaction from Clow old to
			//new we can have Chigh=Clow old*61440+reg new=~Clow old =~real time -61440
			/*no_proc_time_s=CLK_gethtime();
			ch_num=read_channel_queue_in(AP_BLOCK);
			no_proc_time+=CLK_gethtime()-no_proc_time_s;*/

			//read audio from codec
			//get_device(ch_num,&audio_in_buff[ch_num][0]);
			//do audio process on the channel
	        if(((1<<ch_num) && ch_mask)==0)
	            continue;

			res=audio_in_process(ch_num,audio_in_buff[ch_num]);

			//if (res==AP_PROCESS_FINISH)
				//channel_processed=channel_processed|(((uint64_t)1)<<ch_num);
			
			//ctrl_device(ch_num);

		
			//check if too match time passed 
			if(ch_config.no_audio==TRUE)
			{
			
				//find out the channel that was not received and set error bits
				ch_not_rcv=ch_config.codec_channel_input&(~channel_processed);
				mcbsp0_mask=0;
				for (i=0;i<ch_config.number_of_mcbsp0_channel;i++)
					mcbsp0_mask=(mcbsp0_mask<<1)|1;
				mcbsp1_mask=0;
				for (i=0;i<ch_config.number_of_mcbsp1_channel;i++)
					mcbsp1_mask=(mcbsp1_mask<<1)|(mcbsp0_mask+1);

//				i2c0_mask=0;
//				for (i=0;i<ch_config.number_of_i2c0_channel;i++)
//					i2c0_mask=(i2c0_mask|(mcbsp0_mask+mcbsp1_mask+1))<<1;
//				i2c1_mask=0;
//				for (i=0;i<ch_config.number_of_i2c1_channel;i++)
//					i2c1_mask=(i2c1_mask|(mcbsp0_mask+mcbsp1_mask+i2c0_mask+1))<<1;

					
				if (ch_not_rcv&mcbsp0_mask)
				{
					hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_MCBSP0_NO_AUDIO;
					dsp_error[0] |= HD_DSP_ERR_MCBSP0_NO_AUDIO;
				}

						
				if (ch_not_rcv&mcbsp1_mask)
				{
					hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_MCBSP1_NO_AUDIO;
					dsp_error[0] |= HD_DSP_ERR_MCBSP1_NO_AUDIO;
				}
				
//				if (ch_not_rcv&i2c0_mask)
//					hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_I2C0_CODEC;
//				if (ch_not_rcv&i2c1_mask)
//					hd_host_ctrl.sHD_dsp_status.dsp_error[0] |= HD_DSP_ERR_I2C1_CODEC;
					
							
				//too match time passed, set new input channel list
				//in this way in case of no received audio from input 
				//do not stuck other channels
				ch_config.codec_channel_input=channel_processed;
				ch_config.no_audio=0;
			}

			//if channel received is not in the codec channel input list insert
			//it to the list
			//if ((ch_config.codec_channel_input&(((uint64_t)1)<<ch_num))==0)
				//ch_config.codec_channel_input|=(((uint64_t)1)<<ch_num);
		}

		
	#ifdef DEBUG_HOST_CTRL	
//		GPIO4_PIN_CLR;
	#endif
		//GPIO0_PIN_CLR;
		//GPIO1_PIN_CLR;
		channel_processed=0;
		
		//get input buffer
		prepare_host_in_buffer();
		

		//send and receive audio from host
		//prepare_host_buffer();

		for (channel_index=0;
			channel_index<(HD_HOST_CHANNELS_BASE+ch_config.number_of_host_dsp_channel);
			channel_index++)
		{
			
			if (channel_index>=HD_HOST_CHANNELS_BASE)
			{
				//do audio process on host channel
				res=audio_in_process(channel_index,audio_in_buff[channel_index]);
			}
			//clear output buffer
			memset(audio_out_buff[channel_index],0,NSAMPLES*2);
			
					
		}			
	

		audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
			,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
			,NSAMPLES,AM_MODE_MAX_IN_ENERGY ); //AM_MODE_WEIGHTED_MEAN);AM_MODE_CONST_ENERGY);
	
		
		//process host channel
		for(channel_index=HD_HOST_CHANNELS_BASE;channel_index<HD_MAX_CHANNELS;channel_index++)
		{
			//process channel that was define as output
			if (channel_index<64)
			{
				if (ch_config.channel_output_a &(((uint64_t)1)<<channel_index))
				{
					res=audio_out_process(channel_index,audio_out_buff[channel_index]);
				}
			}
			else
			{
				if (ch_config.channel_output_b &(((uint64_t)1)<<(channel_index-64)))
				{
					res=audio_out_process(channel_index,audio_out_buff[channel_index]);
				}

			}
		}
		//send and receive audio from host
		prepare_host_buffer();


		//process codec channel
		for(channel_index=0;channel_index<HD_MAX_CODEC_CHANNELS;channel_index++)
		{
			//process channel that was define as output
			if (ch_config.channel_output_a &(((uint64_t)1)<<channel_index))
			{
				res=audio_out_process(channel_index,audio_out_buff[channel_index]);
				//send_device(channel_index,audio_out_buff[channel_index]);

			}

		}


/*

		//process all channel till all channel defined was processed
		for(channel_index=0;channel_index<HD_MAX_CHANNELS;channel_index++)
		{	
			//process channel that was define as output
			if (channel_index<64)
			{	
				if (ch_config.channel_output_a &(((uint64_t)1)<<channel_index))
				{	
					res=audio_out_process(channel_index,audio_out_buff[channel_index]);
					if (channel_index<HD_MAX_CODEC_CHANNELS)
					{
						send_device(channel_index,audio_out_buff[channel_index]);
					}	
				}
			}	
			else
			{
				if (ch_config.channel_output_b &(((uint64_t)1)<<(channel_index-64)))
				{
					res=audio_out_process(channel_index,audio_out_buff[channel_index]);
				}		
								
			}						
		}
*/
		
		//tone_mux((int16_t*)audio_out_buff);		
									
		///ctrl_devices();
		bit_status_events();
	}	

}


/**************************************************************************/                     
//  Module		: audio_in_process
//  Description : receive an audio buffer from input channel and preform this
//					process: VOCODER, DTMF Detect, filter, Noise Reduction
//					Carrier Detect, AGC.	
/// @return none
/// @param	none
/**************************************************************************/
int audio_in_process(uint32_t channel,int16_t *speech_in)
{
	//uint16_t i=1;
	//unsigned char key_enc[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	//unsigned char key_dec[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	unsigned short cd_state;
	//process VOCODER

	//process DTMF detection
	if(audio_local_ctrl.filter_enable_mask[channel][HD_ADC])
	{
		if (filter_type[channel][HD_ADC]&HD_FILTER_DC_BLOCK)
			filter_dc (channel, HD_ADC, speech_in, speech_in);
	}
	//process Caller ID
	/*if (audio_local_ctrl.caller_id_enable[channel]!=HD_NO_CH)
	{
		caller_id_process(audio_local_ctrl.caller_id_enable[channel],speech_in);
	}
	//process DTMF Detection
	if (audio_local_ctrl.dtmf_det_enable[channel]!=HD_NO_CH)
	{
		dtmf_det_process(audio_local_ctrl.dtmf_det_enable[channel],speech_in);
	}*/

	//process echo cancel
	if(audio_local_ctrl.ec_enable_mask[channel])
	{
		/*if (ec_param[channel].echo_ch_dir==HD_DAC)
		{
			echo_cancel_process(channel,speech_in,
				&audio_out_buff[ec_param[channel].echo_ch][0]);

		}
		else
		{
			echo_cancel_process(channel,speech_in,
				&audio_in_buff[ec_param[channel].echo_ch][0]);

		}*/
	}

	//process line echo cancel
	if(audio_local_ctrl.lec_enable_mask[channel])
	{

		/*if (lec_param[channel].echo_ch_dir==HD_DAC)
		{
			LEC_process(channel,speech_in,
				&audio_out_buff[lec_param[channel].echo_ch][0],
				lec_param[channel].started);
		}
		else
		{

			LEC_process(channel,speech_in,
				&audio_in_buff[lec_param[channel].echo_ch][0],
				lec_param[channel].started);

		}
		lec_param[channel].started=1;*/
	}
	else
	{
		//lec_param[channel].started=0;
	}

	 // process filters
	if(audio_local_ctrl.filter_enable_mask[channel][HD_ADC])
	{
		//if (filter_type[channel][HD_ADC]&HD_FILTER_DC_BLOCK)
		//	filter_dc (channel, HD_ADC, speech_in, speech_in);
		/*if (filter_type[channel][HD_ADC]&HD_FILTER_HP300HZ)
			HPF_process(hHPF[channel][HD_ADC], speech_in, speech_in);*/
		if (filter_type[channel][HD_ADC]&HD_FILTER_NAVY_HEADSET)
			filter_navy_headset (channel,HD_ADC,speech_in, speech_in);
		if (filter_type[channel][HD_ADC]&HD_FILTER_CONFIGURED)
			filter_process(channel,HD_ADC, speech_in, speech_in);


	}

	if(audio_local_ctrl.nr_enable_mask[channel][HD_ADC]||audio_local_ctrl.agc_enable_mask[channel][HD_ADC]||audio_local_ctrl.lec_enable_mask[channel])
	    veu_process(channel, HD_ADC, speech_in, speech_in);
	//process Noise Reduction
	if(audio_local_ctrl.nr_enable_mask[channel][HD_ADC])
	{
		/*if (anc_isInitialized(channel, HD_ADC))
				anc_process(channel,HD_ADC,speech_in, speech_in);
		else
			denoise_process(channel,HD_ADC,speech_in);
*/

	}
///	gHostBufferProcXmt[channel].SoF.CC = CD_State[channel];

// process AGC

	/*if(audio_local_ctrl.agc_enable_mask[channel][HD_ADC])
	{

		AGC_process(hAGC[channel][HD_ADC], speech_in, speech_in);
		
	}

	// process CD


	if(audio_local_ctrl.cd_enable_mask[channel][HD_ADC])
	{
		cd_state=CD_State[channel][HD_ADC];
		CD_process(hCD[channel][HD_ADC], speech_in, &CD_State[channel][HD_ADC]);

		if ((cd_mute[channel][HD_ADC]==TRUE)&&
			(CD_State[channel][HD_ADC]==FALSE))
			//clear channel audio buffer if mute is requiered and no audio was detected
			memset (speech_in,0,NSAMPLES*2);

		hd_host_ctrl.sHD_cd_status.status[channel][HD_ADC]=(uint16_t)CD_State[channel][HD_ADC];
		if ((cd_state!=CD_State[channel][HD_ADC])&&(hd_host_ctrl.sHD_cd_status.changed==0))
			hd_host_ctrl.sHD_cd_status.changed=1;
	}
	else
	{
		CD_State[channel][HD_ADC] = CD_ACTIVE; // Allways active if CD disabled
		hd_host_ctrl.sHD_cd_status.status[channel][HD_ADC]=CD_ACTIVE;
	}*/

	 // process delay buffer
	if((channel<HD_MAX_CODEC_CHANNELS)&&(audio_local_ctrl.dly_buffer_enable_mask[channel][HD_ADC]))
	{
		DLYBUF_delay(channel,HD_ADC ,speech_in,speech_in);
	}




	return (AP_PROCESS_FINISH);
}



/**************************************************************************/                     
//  Module		: audio_out_process
//  Description : receive an audio buffer from mux output and preform this
//					process: AGC, tone generation, VOCODER.	
/// @return none
/// @param	none
/**************************************************************************/
int audio_out_process(uint32_t channel,int16_t *speech_out)
{
	unsigned short cd_state;

	// process HPF
	if(audio_local_ctrl.filter_enable_mask[channel][HD_DAC])
	{
		if (filter_type[channel][HD_DAC]&HD_FILTER_DC_BLOCK)
			filter_dc (channel, HD_DAC, speech_out, speech_out);
		/*if (filter_type[channel][HD_DAC]&HD_FILTER_HP300HZ)
			HPF_process(hHPF[channel][HD_DAC], speech_out, speech_out);*/
		if (filter_type[channel][HD_DAC]&HD_FILTER_CONFIGURED)
			filter_process(channel,HD_DAC, speech_out, speech_out);

	}
				
	//process Noise Reduction
	if(audio_local_ctrl.nr_enable_mask[channel][HD_DAC])
	{
		/*if (anc_isInitialized(channel, HD_DAC))
			anc_process(channel,HD_DAC,speech_out, speech_out);
		else
			denoise_process(channel,HD_DAC,speech_out);*/

	}
			
	/*	// process AGC
if(audio_local_ctrl.agc_enable_mask[channel][HD_DAC])
	{
		AGC_process(hAGC[channel][HD_DAC], speech_out, speech_out);
	}

 	// process CD
	if(audio_local_ctrl.cd_enable_mask[channel][HD_DAC])
	{
		cd_state=CD_State[channel][HD_DAC];
		CD_process(hCD[channel][HD_DAC], speech_out, &CD_State[channel][HD_DAC]);
		
		if ((cd_mute[channel][HD_DAC]==TRUE)&&
			(CD_State[channel][HD_DAC]==FALSE))
			//clear channel audio buffer if mute is requiered and no audio was detected 
			memset (speech_out,0,NSAMPLES*2);

		hd_host_ctrl.sHD_cd_status.status[channel][HD_DAC]=(uint16_t)CD_State[channel][HD_DAC];
		if ((cd_state!=CD_State[channel][HD_DAC])&&(hd_host_ctrl.sHD_cd_status.changed==0))
			hd_host_ctrl.sHD_cd_status.changed=1;
	}
	else
	{
		CD_State[channel][HD_DAC] = CD_ACTIVE; // Allways active if CD disabled
		hd_host_ctrl.sHD_cd_status.status[channel][HD_DAC]=CD_ACTIVE;
	}*/
		
	//process Tone Generator
	tone_mux(speech_out,channel);
	
	//process VOCODER
	
	 // process delay buffer
	if((channel<HD_MAX_CODEC_CHANNELS)&&(audio_local_ctrl.dly_buffer_enable_mask[channel][HD_DAC]))
	{
		DLYBUF_delay(channel,HD_DAC ,speech_out,speech_out);
	}

	
	return (0);

}



/**************************************************************************/                     
//  Module		: inc_channel_queue
//  Description : increment channel queue
/// @return none
/// @param	none
/**************************************************************************/
int inc_channel_queue_in(int index_in_out)
{

	if (index_in_out==AP_INDEX_OUT)
	{
		if (queue_in.index_out==queue_in.index_in)
			return(-1);
		queue_in.index_out++;
		if (queue_in.index_out>(HD_MAX_CHANNELS-1))
			queue_in.index_out=0;
	}		
	if (index_in_out==AP_INDEX_IN)
	{
		queue_in.index_in++;
		if (queue_in.index_in>(HD_MAX_CHANNELS-1))
			queue_in.index_in=0;		
		//check if queue is	full, if yes roll back to old index
		if (queue_in.index_out==queue_in.index_in)
		{
			if (queue_in.index_in==0)
				queue_in.index_in=HD_MAX_CHANNELS-1;
			else
				queue_in.index_in--;
			return(-1);								
		}
		
	}
	return(0);		
}

/**************************************************************************/                     
//  Module		: read_channel_queue
//  Description : read channel queue
/// @return AP_channel (num, audio pointer) 	
/// @param	none
/**************************************************************************/
uint16_t read_channel_queue_in(int block)
{
	uint16_t	res;
	
	//on blocking wait till queue have data
	while (queue_in.index_out==queue_in.index_in)
	{
		if (block==AP_NON_BLOCK)
			return res;
		//pend task untill edma interrupt accured		
		//SEM_pend(&SEM_audio_in, SYS_FOREVER);
	}
	res=queue_in.ch_num[queue_in.index_out];	
	//queue_in.ch_num[queue_in.index_out]=0;
	inc_channel_queue_in(AP_INDEX_OUT);	
	return res;
}

/**************************************************************************/                     
//  Module		: write_channel_queue
//  Description : write channel queue
/// @return none
/// @param	none
/**************************************************************************/
int write_channel_queue_in(uint16_t ch_num)
{
	int res;
	queue_in.ch_num[queue_in.index_in]=ch_num;
	res=inc_channel_queue_in(AP_INDEX_IN);	
	return res;
}

/**************************************************************************/                     
//  Module		: init_audio_processing
//  Description : initialize and create audio processing modules
/// @return none
/// @param	none
/**************************************************************************/
Void init_audio_processing(void)
{
	short int 	iChannel = 0;
	/*HPF_Params  hpf_params;
	AGC_Params  agc_params;
	CD_Params   cd_params;*/


/*	CD_init();
	HPF_init();
    AGC_init();

	anc_initialize();

	hpf_fxns   = HPF_DSPE_IHPF;
    hpf_params = HPF_PARAMS;

    agc_fxns   = AGC_DSPE_IAGC;
    agc_params = AGC_PARAMS;

	cd_fxns    = CD_DSPE_ICD;
    cd_params  = CD_PARAMS;*/


	for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
	{
		if (iChannel<HD_MAX_HOST_DSP_CHANNELS)
		{
			//G729AB_ADT_EncodeInit( &g729_enc_state[iChannel], &g729_enc_scratch);
			//G729AB_ADT_DecodeInit( &g729_dec_state[iChannel], &g729_dec_scratch);
	
			//VADEnable[iChannel] = 0;
			//FrameErase[iChannel] = 0;
			g711plc_construct(&g711_lc[iChannel]);

		}
		
	// Create HPFs
		
	/*	hHPF[iChannel][0] = HPF_create(&hpf_fxns, &hpf_params);
		hHPF[iChannel][1] = HPF_create(&hpf_fxns, &hpf_params);


	// Create AGCs

		hAGC[iChannel][0] = AGC_create(&agc_fxns, &agc_params);
		hAGC[iChannel][1] = AGC_create(&agc_fxns, &agc_params);


	// Create CDs
		hCD[iChannel][0] = CD_create(&cd_fxns, &cd_params);
		hCD[iChannel][1] = CD_create(&cd_fxns, &cd_params);

		//CD
		//initialize module for the first time
		//CD_init(hCD[iChannel][HD_ADC]);
		//CD_init(hCD[iChannel][HD_DAC]);
		//configure module for the first time
		cd_params.HD_CD_start_duration = 0;
		cd_params.HD_CD_stop_duration = 80;
		cd_params.HD_CD_threshold_level = -35;
		CD_setup(hCD[iChannel][HD_ADC], &cd_params);
		CD_setup(hCD[iChannel][HD_DAC], &cd_params);
		cd_mute[iChannel][HD_ADC]=0;
		cd_mute[iChannel][HD_DAC]=0;

		//AGC
		//AGC_init(hAGC[iChannel][HD_ADC]);
		//AGC_init(hAGC[iChannel][HD_DAC]);
		//initialize module for the first time
		//configure module for the first time
		agc_params.HD_AGC_cutoff_input_level=-50;
		agc_params.HD_AGC_cutoff_offset=0;
		agc_params.HD_AGC_rotation_input_level=-23;
		agc_params.HD_AGC_rotation_output_level=-12;
		AGC_setup(hAGC[iChannel][HD_ADC],&agc_params);
		AGC_setup(hAGC[iChannel][HD_DAC],&agc_params);*/

		//Filters Init
		//HPF_init(hHPF[iChannel][HD_ADC]);
		filter_dc_init(iChannel, HD_ADC);
		filter_navy_headset_init(iChannel, HD_ADC);
		//HPF_init(hHPF[iChannel][HD_DAC]);
		filter_dc_init(iChannel, HD_DAC);

		//nr&ec&lec init will be done by user (as it allocate memory only if module is needed)
		//nr&ec&lec modules will not be allowed to run if not previously initialized




	}
	// Disable all module 
	memset(&audio_local_ctrl,0,sizeof(audio_local_ctrl));
	for(iChannel=0; iChannel<HD_MAX_CHANNELS; iChannel++)
	{
		audio_local_ctrl.caller_id_enable[iChannel]=HD_NO_CH;
		audio_local_ctrl.dtmf_det_enable[iChannel]=HD_NO_CH;
	}
	memset(&vocoder_config,0,sizeof(vocoder_config));
			
	memset(audio_in_buff,0,sizeof(audio_in_buff)); 
	memset(audio_out_buff,0,sizeof(audio_out_buff)); 
	
	//init mux module
	mux_init();
	
	//init tone generation module
	tone_init();
	
	//init dev_channel table
	memset(dev_channel,0,sizeof(dev_channel));
	
	//init nr module
	//preprocess_create();
	
	//LEC_clean();

	queue_in.index_out=0;
	queue_in.index_in=0;	

	//init delay buffer
	DLYBUF_init();

    memset((void*)&DLYBUF_extension,0,sizeof(DLYBUF_extension));

    filter_init();
}



/**************************************************************************/                     
//  Module		: system_config
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
/*
void system_config(void)
{	
	uint16_t i,handle;
	//static channel configuration for test 
	//channlel 0-2 work with AIC14 codec
	//in normal operation the channel_dev table is set by checking contents
	//of struct host_dsp_ctrl in DigitalCore_hpi.h and according to AC_initialize
	//codec type set the fuction of this codec by serching func_Dev table for
	//the match device id
	dev_channel[0].func=&dev_func[0];
	dev_channel[1].func=&dev_func[0];
	dev_channel[2].func=&dev_func[0];

	handle=dev_channel[0].func->dev_open(DEV_MCBSP0,0,3);
	for (i=0;i<3;i++)
	{
		dev_channel[i].handle=handle;	
	}
	
}

*/
/**************************************************************************/                     
//  Module		: dev_get
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
int get_device(uint16_t ch_num, int16_t *audio)
{
//	int16_t *audio;
	//call send function for every opened channel
	if(ch_config.codec_channel_opened & (((uint64_t)1)<<ch_num))
		dev_channel[ch_num].func->dev_get(dev_channel[ch_num].handle,ch_num,audio);
//	return (audio);
	return(0);
}


/**************************************************************************/                     
//  Module		: send_device
//  Description : 
/// @return none
/// @param	none
/**************************************************************************/
int send_device(uint16_t ch_num,int16_t *audio)
{
	//call send function for every opened channel
	if(ch_config.codec_channel_opened & (((uint64_t)1)<<ch_num))
		dev_channel[ch_num].func->dev_send(dev_channel[ch_num].handle,ch_num,audio);
	return (0);
}


/**************************************************************************/                     
//  Module		: ctrl_devices
//  Description : call ctrl device function for every opened device
/// @return none
/// @param	none
/**************************************************************************/
int ctrl_devices(void)
{
	int ch_index=0;
//	if ((hd_host_ctrl.sHD_ac_configuration.HD_request==1)&&
//		(hd_host_ctrl.sHD_ac_configuration.channels_mask[3])==1)	
//		hd_host_ctrl.sHD_ac_configuration.adc_gain=20;
	//if((debub_frame_count=10000)==0)	
	
	for(ch_index=0;ch_index<HD_MAX_CODEC_CHANNELS;ch_index++)
	{
		//call ctrl function for every opened channel
		if(ch_config.codec_channel_opened & (((uint64_t)1)<<ch_index))		
			dev_channel[ch_index].func->dev_ctrl(dev_channel[ch_index].handle,ch_index);
	}
	// reset initialization request
	hd_host_ctrl.sHD_ac_initialization.HD_request = FALSE;

	hd_host_ctrl.sHD_ac_configuration.HD_request = FALSE;
//	if ((hd_host_ctrl.sHD_ac_configuration.HD_request==1)&&
//		(hd_host_ctrl.sHD_ac_configuration.channels_mask[3])==1)	
	return (0);
}

/**************************************************************************/                     
//  Module		: ctrl_device
//  Description : call ctrl device function 
/// @return none
/// @param	none
/**************************************************************************/
int ctrl_device(uint16_t ch_num)
{
	int ch_index=0,init=0,conf=0;
	

	//call ctrl function for every opened channel
	if(ch_config.codec_channel_opened & (((uint64_t)1)<<ch_num))		
		dev_channel[ch_num].func->dev_ctrl(dev_channel[ch_num].handle,ch_num);
	if ((hd_host_ctrl.sHD_ac_initialization.HD_request)||
		(hd_host_ctrl.sHD_ac_configuration.HD_request))
	{	
		//check if all channels were configured		
		for(ch_index=0;ch_index<HD_MAX_CODEC_CHANNELS;ch_index++)
		{
			if (hd_host_ctrl.sHD_ac_initialization.channels_mask[ch_index])
				init++;	
			if (hd_host_ctrl.sHD_ac_configuration.channels_mask[ch_index])
				conf++;	
		
		}
		// reset initialization request
		if (init==0)
			hd_host_ctrl.sHD_ac_initialization.HD_request = FALSE;
		if (conf==0)
			hd_host_ctrl.sHD_ac_configuration.HD_request = FALSE;
	}		
	return (0);
}


/**************************************************************************/                     
//  Module		: edma_isr
//  Description : EDMA interrupt routine, call all registered device driver 
//					isr function
/// @return none
/// @param	none
/**************************************************************************/
void edma_isr(void)
{
	int device_index=0;
	
	for (device_index=0;device_index<MAX_DEV_FUNCTION;device_index++)
	{
		if(ch_config.device_active & (((uint32_t)1)<<device_index))
			dev_func[device_index].dev_isr();
	}
	
	//post semafor to indicate auido_process task that data is ready
	//SEM_post(&SEM_audio_in);


}


/**************************************************************************/                     
//  Module		: init_devices
//  Description : initialize all devices
/// @return none
/// @param	none
/**************************************************************************/
void init_devices(void)
{
	int device_index=0;
	
	for (device_index=0;device_index<MAX_DEV_FUNCTION;device_index++)
	{
		if(ch_config.device_active & (((uint32_t)1)<<device_index))
			dev_func[device_index].dev_init();
	}	

}

/**************************************************************************/                     
//  Module		: start_devices
//  Description : start devices operation
/// @return none
/// @param	none
/**************************************************************************/
void start_devices(void)
{
	int device_index=0;

	for (device_index=0;device_index<MAX_DEV_FUNCTION;device_index++)
	{
		if(ch_config.device_active & (((uint32_t)1)<<device_index))
			dev_func[device_index].dev_start();
	}	

}


