/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : configure systems as defined by arm, open drivers as defined
//					by host cpu
///	@file:	system_config.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	29/Jul/2008	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/


#include <xdc/std.h>
#include <stdint.h>
/*#include <c6x.h>
#include <csl.h>
#include <csl_gpio.h>
#include <log.h>
#include <clk.h>
#include <string.h>
#include "G729AB_user.h"
#include "hpf.h"
#include "hpf_dspe.h"
#include "agc.h"
#include "agc_dspe.h"
#include "cd.h"
#include "cd_dspe.h"
#include "BoardUtility\BoardUtility.h" //ilan
#include "DigitalCore_hpi.h"
#include "main.h"
#include "audio_processing.h"*/
#include "host_frame.h"
#include "system_config.h"
#include <ti/sysbios/hal/Cache.h>

//extern far LOG_Obj LOG_trace;

//extern GPIO_Handle hGpio;

SC_channel_config	ch_config;

extern HD_host_dsp_ctrl	hd_host_ctrl;
/*extern AP_device_driver_func dev_func[];
extern AP_device_driver_func dev_func[];
extern AP_device_channel_table dev_channel[];*/

void (*reset_dsp)(void)=0;

//extern int g_first_time;

/**************************************************************************/                     
//  Module		: system_config
//  Description : open codec in the relevant port as configured by host cpu
/// @return none
/// @param	none
/**************************************************************************/
int system_config(uint16_t restart)
{
	uint16_t start_ch=0xff,stop_ch=0xff,count_ch=0,err=0,ret,i,j,ch_index;
	int device_index;
	static uint16_t sys_config_state=SC_WAIT_CH_CONFIG;	
	if (restart==SC_RESTART)
		sys_config_state=SC_WAIT_CH_CONFIG;

	Cache_wbInv(&hd_host_ctrl, sizeof(hd_host_ctrl),Cache_Type_ALL, TRUE);
	switch (sys_config_state)
	{
		case SC_WAIT_CH_CONFIG:
			if (hd_host_ctrl.sHD_channel_configuration.HD_request)
			{
				//close all open devices
				if (ch_config.codec_channel_opened)
				{				
					//for(ch_index=0;ch_index<HD_MAX_CODEC_CHANNELS;ch_index++)
					{
						//call close function for every opened channel
						//if(ch_config.codec_channel_opened & (((uint64_t)1)<<ch_index))
							//dev_channel[ch_index].func->dev_close(dev_channel[ch_index].handle);
					}				
				}
				ret=get_config();
				if (ret==0)
				{
					hd_host_ctrl.sHD_channel_configuration.HD_request=0;
					sys_config_state=SC_WAIT_AC_INIT;
					hd_host_ctrl.sHD_dsp_status.dsp_state=HD_SYSTEM_CONFIG;
				}
				else
				{
					err=SC_ERR_CH_CONFIG;
				}		
			}		
		break;	
		

				
		
		case SC_WAIT_AC_INIT:
		
			if (hd_host_ctrl.sHD_ac_initialization.HD_request)
			{
				start_ch=0xff;
				stop_ch=HD_MAX_CODEC_CHANNELS;
				count_ch=0;
				for (i=0;i<HD_MAX_CODEC_CHANNELS;i++)
				{
					if(hd_host_ctrl.sHD_ac_initialization.channels_mask[i]==TRUE)
					{
						//capture first channel
						if (start_ch==0xff)
							start_ch=i;
						//count	number of channel
						count_ch++;	
						
						hd_host_ctrl.sHD_ac_initialization.channels_mask[i]=FALSE;
					}
					else
					{
						//capture first channel not to configure
						if ((stop_ch==HD_MAX_CODEC_CHANNELS)&&(start_ch!=0xff))
						{
							stop_ch=i;
						}
					}
				}
				if ((stop_ch-start_ch)==count_ch)
				{
					//ok, we received a sequance of channel
					
					//find the device in the device id table
					//and return its index in the device function table
					//device_index=find_device_id(hd_host_ctrl.sHD_ac_initialization.codec_type);
				    device_index=0;
					if (device_index>=0)
					{
						//assign function struct to each channel
						for (j=start_ch;j<stop_ch;j++)
						{
							//dev_channel[j].func=&dev_func[device_index];
							//update ch_config with the channel type - input or output or both
							if ((hd_host_ctrl.sHD_ac_initialization.ch_type==HD_CH_IN_TYPE)||
								(hd_host_ctrl.sHD_ac_initialization.ch_type==HD_CH_IN_OUT_TYPE))
								ch_config.channel_input_a|=(((uint64_t)1)<<j);
							if ((hd_host_ctrl.sHD_ac_initialization.ch_type==HD_CH_OUT_TYPE)||
								(hd_host_ctrl.sHD_ac_initialization.ch_type==HD_CH_IN_OUT_TYPE))
								ch_config.channel_output_a|=(((uint64_t)1)<<j);
							
									
						}
						/*ret=open_driver_group(start_ch,stop_ch);
						if (ret!=0)
							err=SC_ERR_CODEC_OPEN;*/
						//indicate device is active
						ch_config.device_active|=(((uint32_t)1)<<device_index);
					}					
					else
					//codec id is not correct
					err=SC_ERR_CODEC_ID;		
					
					
				}
				else
					//we didnt receive a sequance of channels
					err=SC_ERR_CH_NON_SEQ;		
			
			if (err==0)
				//inform host we open the port
				hd_host_ctrl.sHD_ac_initialization.HD_request=0;
			else
			    break;

			//check if all radio was configured
			//if (ch_config.codec_channel_active==ch_config.codec_channel_opened)
			{
				sys_config_state=SC_HOST_CH_INIT;
				
			}
			}
			#ifdef ENABLE_SYSTEM_RESTART	
				if(hd_host_ctrl.sHD_channel_configuration.HD_request==TRUE)
				{			
				  	init_devices();				  	
				  	reset_dsp();
				
				}
			#endif
				
		break;

		case SC_HOST_CH_INIT:
			//initialize host audio channel
			init_host_audio();
			sys_config_state=SC_NORMAL;
			hd_host_ctrl.sHD_dsp_status.dsp_state=HD_READY;
		break;
				

		case SC_NORMAL:
			//system was configured and all drivers needed was opened
			
		break;
	
	
	}												
	
	return(err);

}



/**************************************************************************/                     
//  Module		: get_config
//  Description : fill ch_config struct with value 
/// @return none
/// @param	none
/**************************************************************************/
int get_config(void)
{
	uint32_t temp,i;
	if (hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel>
			HD_MAX_TONE_CHANNELS) 
		return(-1);
	if (hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel>
			HD_MAX_HOST_DSP_CHANNELS) 
		return(-1);
		
	ch_config.frames_buffer_size=0;
	
	for (i=0;i<hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel;i++)
	{
		if ((hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]>SC_MAX_FRAME_SIZE)||
			(hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i]<SC_MIN_FRAME_SIZE))
			return(-1);
		ch_config.frames_buffer_size+=hd_host_ctrl.sHD_channel_configuration.channel_frame_size[i];
	}	
	ch_config.frames_buffer_size+=sizeof(HD_channels_frame_size);
	if (ch_config.frames_buffer_size>SC_MAX_FRAME_BUFFER_SIZE)
		return(-1);	
				
	temp=hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel+
		hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel+
		hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel+
		hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel;
	if (temp>HD_MAX_CODEC_CHANNELS) 
		return(-1);

	memcpy(&ch_config,&hd_host_ctrl.sHD_channel_configuration,
		sizeof(hd_host_ctrl.sHD_channel_configuration)-4);  
	
	//set bits for active codec channels, 
	//codecs channles start from channel 0 		
	ch_config.codec_channel_active=0xffffffffffffffff>>(64-temp);
	
	//ch_config.codec_channel_opened=0;
	ch_config.channel_input_a=0;
	ch_config.channel_input_b=0;
	ch_config.channel_output_a=0;
	ch_config.channel_output_b=0;	
	ch_config.device_active=0;
	ch_config.no_audio=0;	
	return (0);		

}	

#ifdef TEST_DSP
/**************************************************************************/                     
//  Module		: find_device_id
//  Description : find the device in the device id table 
/// @return none
/// @param	none
/**************************************************************************/
int find_device_id(uint16_t codec_type)
{
	int i=0;
	while (dev_func[i].codec_id!=0)
	{
		if (dev_func[i].codec_id==codec_type)
			return(i);	
		i++;	
	}
	//we didnt found the codec
	return(-1);

}

/**************************************************************************/                     
//  Module		: open_driver_group
//  Description : call open function for the driver, may be for several port
//					- mcbsp0, mcbsp1, i2c0, i2c1. channel number start and 
//					stop may be from diffrent port in this case several driver
//					will be open
/// @return none
/// @param	start channel number, stop channel number, 
/**************************************************************************/

int open_driver_group(uint16_t start_ch,uint16_t stop_ch)
{

	uint16_t mcbsp0_count_ch,mcbsp1_count_ch,i2c0_count_ch,i2c1_count_ch,
		mcbsp0_start_ch,mcbsp1_start_ch,i2c0_start_ch,i2c1_start_ch;
	int handle,j,err=0;
	
	mcbsp0_count_ch=0; //number of channel in mcbsp0
	mcbsp1_count_ch=0; //number of channel in mcbsp1
	i2c0_count_ch=0; //number of channel in i2c0
	i2c1_count_ch=0; //number of channel in i2c1
	mcbsp0_start_ch=0;
	mcbsp1_start_ch=ch_config.number_of_mcbsp0_channel;
	i2c0_start_ch=ch_config.number_of_mcbsp0_channel+
		ch_config.number_of_mcbsp1_channel;
	i2c1_start_ch=ch_config.number_of_mcbsp0_channel+
		ch_config.number_of_mcbsp1_channel+
		ch_config.number_of_i2c0_channel;
		
	if (start_ch<mcbsp1_start_ch)
	{
		//channel start in mcbsp0
		mcbsp0_start_ch=start_ch;
		if (stop_ch<=mcbsp1_start_ch)
		{
			//all codec are in mcbsp0									
			mcbsp0_count_ch=stop_ch-start_ch;
		}
		else if(stop_ch<=i2c0_start_ch)
		{
			//codec are in mcbsp0 and mcbsp1 
			mcbsp0_count_ch=ch_config.number_of_mcbsp0_channel;	
			mcbsp1_count_ch=stop_ch-ch_config.number_of_mcbsp0_channel;
		}	
		else if(stop_ch<=i2c1_start_ch)
		{
			//codec are in mcbsp0, mcbsp1 and i2c0 
			mcbsp0_count_ch=ch_config.number_of_mcbsp0_channel;
			mcbsp1_count_ch=ch_config.number_of_mcbsp1_channel;	
			i2c0_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel);
						
		}						
		else if(stop_ch<=(i2c0_start_ch+ch_config.number_of_i2c1_channel))
		{
			//codec are in mcbsp0, mcbsp1, i2c0 and i2c1 
			mcbsp0_count_ch=ch_config.number_of_mcbsp0_channel;
			mcbsp1_count_ch=ch_config.number_of_mcbsp1_channel;
			i2c0_count_ch=ch_config.number_of_i2c0_channel;	
			i2c1_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel+
				ch_config.number_of_i2c0_channel);
		}						
		
	}
	
	else if (start_ch<i2c0_start_ch)
	{
		//channel start in mcbsp1
		mcbsp1_start_ch=start_ch;
		if(stop_ch<=i2c0_start_ch)
		{
			//codec are in mcbsp1 									
			mcbsp1_count_ch=stop_ch-ch_config.number_of_mcbsp0_channel;
		}	
		else if(stop_ch<=i2c1_start_ch)
		{
			//codec are in mcbsp1 and i2c0 								
			mcbsp1_count_ch=ch_config.number_of_mcbsp1_channel;	
			i2c0_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel);
						
		}						
		else if(stop_ch<=(i2c1_start_ch+ch_config.number_of_i2c1_channel))
		{
			//codec are in mcbsp1, i2c0 and i2c1 								
			mcbsp1_count_ch=ch_config.number_of_mcbsp1_channel;
			i2c0_count_ch=ch_config.number_of_i2c0_channel;	
			i2c1_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel+
				ch_config.number_of_i2c0_channel);
							
		}						
		
	}
	
	else if (start_ch<i2c1_start_ch)
	{
		//channel start in i2c0
		i2c0_start_ch=start_ch;
		if(stop_ch<=i2c1_start_ch)
		{
			//codec are in i2c0 								
			i2c0_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel);											
		}						
		else if(stop_ch<=(i2c1_start_ch+ch_config.number_of_i2c1_channel))
		{
			//codec are in i2c0 and i2c1 								
			i2c0_count_ch=ch_config.number_of_i2c0_channel;	
			i2c1_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel+
				ch_config.number_of_i2c0_channel);												
		}						
		
	}


	else if (start_ch<(i2c1_start_ch+ch_config.number_of_i2c1_channel))
	{
		//channel start in i2c1
		i2c1_start_ch=start_ch;
		if(stop_ch<=(i2c1_start_ch+ch_config.number_of_i2c1_channel))
		{
			//codec are in i2c1 								
			i2c1_count_ch=stop_ch-(ch_config.number_of_mcbsp0_channel+
				ch_config.number_of_mcbsp1_channel+
				ch_config.number_of_i2c0_channel);											
		}
	}						
							
	//open device with the correct parameters:port, start channel, number of channel	
	if (mcbsp0_count_ch)
	{									
		handle=dev_channel[mcbsp0_start_ch].func->dev_open(DEV_MCBSP0,mcbsp0_start_ch,mcbsp0_count_ch);
		if (handle>=0)
		{										
			for (j=mcbsp0_start_ch;j<(mcbsp0_start_ch+mcbsp0_count_ch);j++)
			{
				dev_channel[j].handle=handle;
				ch_config.codec_channel_opened=ch_config.codec_channel_opened|(((uint64_t)1)<<j);	
			}						
		}
		else
			//port was not opned
			err=1;
	}
	if (mcbsp1_count_ch)
	{									
		handle=dev_channel[mcbsp1_start_ch].func->dev_open(DEV_MCBSP1,mcbsp1_start_ch,mcbsp1_count_ch);
		if (handle>=0)
		{										
			for (j=mcbsp1_start_ch;j<(mcbsp1_start_ch+mcbsp1_count_ch);j++)
			{
				dev_channel[j].handle=handle;
				ch_config.codec_channel_opened=ch_config.codec_channel_opened|(((uint64_t)1)<<j);	
			}						
		}
		else
			//port was not opned
			err=1;
	}
	if (i2c0_count_ch)
	{									
		handle=dev_channel[i2c0_start_ch].func->dev_open(DEV_I2C0,i2c0_start_ch,i2c0_count_ch);
		if (handle>=0)
		{										
			for (j=i2c0_start_ch;j<(i2c0_start_ch+i2c0_count_ch);j++)
			{
				dev_channel[j].handle=handle;
				ch_config.codec_channel_opened=ch_config.codec_channel_opened|(((uint64_t)1)<<j);	
			}						
		}
		else
			//port was not opned
			err=1;
	}
	if (i2c1_count_ch)
	{									
		handle=dev_channel[i2c1_start_ch].func->dev_open(DEV_I2C1,i2c1_start_ch,i2c1_count_ch);
		if (handle>=0)
		{										
			for (j=i2c1_start_ch;j<(i2c1_start_ch+i2c1_count_ch);j++)
			{
				dev_channel[j].handle=handle;
				ch_config.codec_channel_opened=ch_config.codec_channel_opened|(((uint64_t)1)<<j);	
			}						
		}
		else
			//port was not opned
			err=1;
	}
	
	if((err==1) ||((mcbsp0_count_ch+mcbsp1_count_ch+i2c0_count_ch+i2c1_count_ch)==0))
	
		return(-1);
	
	return(0);
}





/**************************************************************************/                     
//  Module		: test_system_config
//  Description : module that check system_config functionality,
//					it call system config with diffrent input and check 
//					result is correct
/// @return none
/// @param	none
/**************************************************************************/
#if	 (TEST_TYPE == TEST_SYS_CONFIG)
void test_system_config(void)
{
	int ret;

	//channel configuration
	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=3;		//number of codec in McBsp0 
	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=0;	//number of codec in McBsp1
	hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
	hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
	hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=3;	//number of channel transfer  between host and dsp
	hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	
	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;

	ret=system_config(SC_RESTART);
	if (hd_host_ctrl.sHD_dsp_status.dsp_state!=HD_SYSTEM_CONFIG)
		LOG_printf(&LOG_trace,"system_config fail in channel comnfiguration %d",ret);		
		

	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=-3;		//number of codec in McBsp0 
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	
	hd_host_ctrl.sHD_dsp_status.dsp_state=HD_OUT_OF_RESET;
	ret=system_config(SC_RESTART);
	if (hd_host_ctrl.sHD_dsp_status.dsp_state==HD_SYSTEM_CONFIG)
		LOG_printf(&LOG_trace,"system_config fail in channel comnfiguration %d",ret); 


	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=3;		//number of codec in McBsp0 
	hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=100;		// number of channel in i2c1
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;

	hd_host_ctrl.sHD_dsp_status.dsp_state=HD_OUT_OF_RESET;
	ret=system_config(SC_RESTART);
	if (hd_host_ctrl.sHD_dsp_status.dsp_state==HD_SYSTEM_CONFIG)
		LOG_printf(&LOG_trace,"system_config fail in channel comnfiguration %d",ret); 


	hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
	hd_host_ctrl.sHD_channel_configuration.HD_request=0;

	hd_host_ctrl.sHD_dsp_status.dsp_state=HD_OUT_OF_RESET;
	ret=system_config(SC_RESTART);
	if (hd_host_ctrl.sHD_dsp_status.dsp_state==HD_SYSTEM_CONFIG)
		LOG_printf(&LOG_trace,"system_config fail in channel comnfiguration %d",ret); 
		
	
	hd_host_ctrl.sHD_dsp_status.dsp_state=HD_OUT_OF_RESET;	

	//  **********non sequential channel initialization
	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=FALSE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	ret=system_config(SC_RESTART);
	ret=system_config(SC_RUN);		
	if(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY)
		LOG_printf(&LOG_trace,"system_config fail in ac initialization %d",ret); 
	//********wrong codec type
	//ac initialize, 
	hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14+10;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	ret=system_config(SC_RESTART);
	ret=system_config(SC_RUN);		
	if(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY)
		LOG_printf(&LOG_trace,"system_config fail in ac initialization %d",ret); 	
				
	//******** config codec not existed codec number	
	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	ret=system_config(SC_RESTART);
	ret=system_config(SC_RUN);		
	if(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY)
		LOG_printf(&LOG_trace,"system_config fail in ac initialization %d",ret); 	

	//********config codec twice, not supported by driver
	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[0]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.channels_mask[1]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	ret=system_config(SC_RESTART);
	ret=system_config(SC_RUN);	

	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	
	ret=system_config(SC_RUN);		
		
	if(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY)
		LOG_printf(&LOG_trace,"system_config fail in ac initialization %d",ret); 	
		
	// ***********config 2 codecs in mcbsp0 and 1					
	//channel configuration
	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp0_channel=3;		//number of codec in McBsp0 
	hd_host_ctrl.sHD_channel_configuration.number_of_mcbsp1_channel=3;	//number of codec in McBsp1
	hd_host_ctrl.sHD_channel_configuration.number_of_i2c0_channel=0;		//number of channel in i2c0
	hd_host_ctrl.sHD_channel_configuration.number_of_i2c1_channel=0;		// number of channel in i2c1
	hd_host_ctrl.sHD_channel_configuration.number_of_host_dsp_channel=3;	//number of channel transfer  between host and dsp
	hd_host_ctrl.sHD_channel_configuration.number_of_tone_channel=0;		//number of internal generated tone channel
//	hd_host_ctrl.sHD_channel_configuration.channel_frame_size[HD_MAX_HOST_DSP_CHANNELS]; 	//size of data frame for each channel(in byte)
//	hd_host_ctrl.sHD_channel_configuration.channel_frame_type[HD_MAX_HOST_DSP_CHANNELS]; 	//data frame type for each channel. 
	hd_host_ctrl.sHD_channel_configuration.HD_request=TRUE;
	
	ret=system_config(SC_RESTART);	

	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[2]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	
	ret=system_config(SC_RUN);		

	//ac initialize
	hd_host_ctrl.sHD_ac_initialization.channels_mask[3]=TRUE;
	hd_host_ctrl.sHD_ac_initialization.codec_type=HD_CODEC_AIC14;
	hd_host_ctrl.sHD_ac_initialization.ch_type=HD_CH_IN_OUT_TYPE;
	hd_host_ctrl.sHD_ac_initialization.HD_request=TRUE;
	
	ret=system_config(SC_RUN);		
	//check receiving correct handle and operation succeed
	if ((dev_channel[2].handle!=0)||
		(dev_channel[3].handle!=1)||
		(hd_host_ctrl.sHD_dsp_status.dsp_state==HD_READY))
		LOG_printf(&LOG_trace,"system_config fail in ac initialization %d",ret);
		
}
#endif
#endif
/**************************************************************************/                     
//  Module		: system_config_init
//  Description : initiate system control module
/// @return none
/// @param	none
/**************************************************************************/
void system_config_init(void)
{
	
	//clear ch_config
	memset(&ch_config,0,sizeof(ch_config));
	
	hd_host_ctrl.sHD_audio_link.dsp_buffer_size=SC_MAX_FRAME_SIZE*HD_MAX_HOST_DSP_CHANNELS;


}
