/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : multiplex audio channels according to muxing matrix
///	@file:	audio_mux.c	
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


#include <std.h>
#include <stdint.h>
#include <c6x.h>
#include <csl_gpio.h>
#include <csl.h>
#include <log.h>
#include <math.h>
#include "DigitalCore_hpi.h"
#include "system_config.h"
#include "main.h"
#include "string.h"
#include "audio_mux.h"


#if	 (TEST_TYPE == TEST_AUDIO_MUX)

extern int16_t audio_in_buff[HD_MAX_CHANNELS][NSAMPLES];
extern int16_t audio_out_buff[HD_MAX_CHANNELS][NSAMPLES];
#endif

//static int32_t sample_sum[HD_MAX_CHANNELS];
static int32_t in_energy[HD_MAX_CHANNELS];
static int8_t input_mux_channel[HD_MAX_CHANNELS];
//static int32_t max_energy[HD_MAX_CHANNELS];

static AM_channel_mux ch_mux[HD_MAX_CHANNELS];
extern HD_host_dsp_ctrl	hd_host_ctrl;
extern SC_channel_config	ch_config;

//int16_t vol_db[16][2]={{1,1},{9,8},{5,4},{7,5},{8,5},{7,4},{2,1},{9,4},{5,2},
//						{11,4},{3,1},{7,2},{4,1},{9,2},{5,1},{11,2}};

//define the volume first value is the multiply value and the secound is the shift value
int16_t vol_db[16][2]={{1,0},{9,3},{5,2},{11,3},{3,1},{7,2},{2,0},{9,2},{5,1},
						{11,2},{3,0},{7,1},{4,0},{9,1},{5,0},{11,1}};
//int16_t vol_db[16][2]={{1,0},{1,0},{5,2},{11,3},{3,1},{7,2},{2,0},{9,2},{5,1},
//						{11,2},{3,0},{7,1},{4,0},{9,1},{5,0},{11,1}};
						


int16_t vol_db_h[128-15]={6,7,8,9,10,11,12,14,16,18,20,22,25,28,30,35,39,44,50,56,
						63,70,79,89,100,112,126,141,158,178,200,224,251,
						282,316,355,398,447,501,562,631,708,794,891,1000,1122,
						1259,1413,1585,1778,1995,2239,2512,2818,3162,3548,3981,
						4467,5012,5623,6310,7079,7943,8913,10000,11220,12589,
						14125,15849,17783,19953,22387,25119,28184,31623,32767,
						32767,32767,32767,32767,32767,32767,32767,32767,32767,
						32767,32767,32767,32767,32767,32767,32767,32767,32767,
						32767,32767,32767,32767,32767,32767,32767,32767,32767,
						32767,32767,32767,32767,32767,32767,32767,32767,32767,
						32767};

						
/**************************************************************************/                     
//  Module		: mux_init
//  Description :  init mux module
/// @return none
/// @param	none
/**************************************************************************/
void mux_init(void)
{

	memset(in_energy,0,sizeof(in_energy));
//	memset(max_energy,0,sizeof(max_energy));
	memset(ch_mux,0,sizeof(ch_mux));	
	memset(input_mux_channel,0,sizeof(input_mux_channel));

}




/**************************************************************************/                     
//  Module		: mux
//  Description :  mux audio, sum input channel to one output channel
///					according a volume matrix (in the volume matrix there 
///					are weiget value for each input)
///					preform the operator in MUX_ENERGY_MODE:
///					(out)=(vol_matrix)*(in)*max_energy/(normalize_factor)
///					normalize_factor=sum(vol_matrix*in_energy)
///					preform the operator in MUX_WEIGHT_MODE:
///					(out)=(vol_matrix)*(in)/(normalize_factor)
///					normalize_factor=sum(vol_matrix)
/// @return none
/// @param	none
/**************************************************************************/
//#pragma CODE_SECTION(audio_mux,".ISRAM_CODE")

void audio_mux(int8_t* vol_matrix,int16_t* in,int16_t* out,uint16_t size_in
,uint16_t size_out,uint16_t sample_num,uint16_t mode)
{
	int32_t sum,norm,max_energy,max_energy_index,out32bit,temp_energy;
	uint16_t sample_indx,in_indx,out_indx,i,j,energy_div,const_energy=AM_CONST_ENERGY;
	static uint16_t out_count=0;
	
	if ((mode==AM_MODE_MAX_IN_ENERGY)||(mode==AM_MODE_CONST_ENERGY))
		//calculate energy of the block of sample
		//in_energy[ch]=sum of all input samples		
		calc_energy(in,in_energy,size_in,sample_num);		
	
	//Check if mux configuration have been changed
	if (hd_host_ctrl.sHD_mux_configuration.HD_request==TRUE)
	{

		out_count=0;
		memset(input_mux_channel,0,sizeof(input_mux_channel));
		for (out_indx = 0; out_indx < size_out; out_indx++)
		{
			i=0;
			//run on all input, and build a table of input building up current output
			for (in_indx = 0; in_indx < size_in; in_indx++)
			{
#ifdef AUDIO_MUX_OLD_VOLUME
				if(mux_matrix(in_indx,out_indx)!=0)
#else				
				//Check if volume is not mute
				if(mux_matrix(in_indx,out_indx)>(-AM_VOL_LIMIT))
					//&&(mux_matrix(in_indx,out_indx)!=0))
#endif					
				{
					ch_mux[out_count].out=out_indx;
					ch_mux[out_count].inputs[i]=in_indx;
					i++;
					ch_mux[out_count].size=i;
					
					input_mux_channel[in_indx]=1;					 										 					
					
				}
				if (i>=AM_MAX_INPUT_SUM)
					break;	
			}
			if (i)
				out_count++; 
				
			if(i==0)
			{
				if (out_indx<64)
				{
					if((ch_config.channel_output_a>>out_indx)&1)
						memset((out+(out_indx*sample_num)),0,sample_num*2);
				}		
				else
				{		
					if((ch_config.channel_output_b>>(out_indx-64))&1)
						memset((out+(out_indx*sample_num)),0,sample_num*2);
				}		
			}			
//			if ((i==0)&&((ch_config.channel_output>>out_indx)&1))
//			{
//				//clear output without any input
//				memset((out+(out_indx*sample_num)),0,sample_num*2);
//			}
		}
		hd_host_ctrl.sHD_mux_configuration.HD_request=FALSE;	
	}
	
	
	//run on all outputs
	for (i = 0; i < out_count; i++)
	{				
		out_indx=ch_mux[i].out;
		
		norm=0;
		max_energy=0;
		max_energy_index=0;
		//run on every sample
		for (sample_indx = 0; sample_indx < sample_num; sample_indx++)
		{					
			sum=0;
			//run on all inputs
			for (j = 0; j < ch_mux[i].size; j++)
			{	
				in_indx=ch_mux[i].inputs[j];

				if (ch_mux[i].size>1)	
				{
				
					if (sample_indx==0)
					{
						if (mode==AM_MODE_WEIGHTED_MEAN)
						{
							//normalize_factor+=volume
#ifdef AUDIO_MUX_OLD_VOLUME
							norm+=(int32_t)mux_matrix(in_indx,out_indx);
#else							
							if (mux_matrix(in_indx,out_indx)<0)
							{
								if ((-mux_matrix(in_indx,out_indx))<AM_VOL_HIGH)
									//low volume multiply and devide
									norm+=(1<<(int32_t)vol_db[-mux_matrix(in_indx,out_indx)][1])/
									((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][0]);								
								else	
									norm+=(int32_t)vol_db_h[(-mux_matrix(in_indx,out_indx))-AM_VOL_HIGH];
							}
							else
							{
								if (mux_matrix(in_indx,out_indx)<AM_VOL_HIGH)
									//low volume multiply and devide
									norm+=((int32_t)vol_db[mux_matrix(in_indx,out_indx)][0])>>
									((int32_t)vol_db[mux_matrix(in_indx,out_indx)][1]);
								else	
									norm+=(int32_t)vol_db_h[(mux_matrix(in_indx,out_indx))-AM_VOL_HIGH];
							}
#endif							
						}							
						else
						{
							//calculate norm and max energy of all input					
							//normalize_factor+=in_energy*volume
#ifdef AUDIO_MUX_OLD_VOLUME
							
							if (mux_matrix(in_indx,out_indx)<0)
								temp_energy=in_energy[in_indx]/(-(int32_t)mux_matrix(in_indx,out_indx));
							else
								temp_energy=in_energy[in_indx]*((int32_t)mux_matrix(in_indx,out_indx));
#else							
							if (mux_matrix(in_indx,out_indx)<0)
							{
								if ((-mux_matrix(in_indx,out_indx))<AM_VOL_HIGH)								
									//low volume multiply and devide
									temp_energy=(in_energy[in_indx]<<
									((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][1]))/
									((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][0]);								
								else	
									temp_energy=in_energy[in_indx]/
									((int32_t)vol_db_h[(-mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
							}
							else
							{
								if (mux_matrix(in_indx,out_indx)<AM_VOL_HIGH)								
									//low volume multiply and devide
									temp_energy=(in_energy[in_indx]*
									((int32_t)vol_db[mux_matrix(in_indx,out_indx)][0]))>>
									((int32_t)vol_db[mux_matrix(in_indx,out_indx)][1]);								
								else	
									temp_energy=in_energy[in_indx]*
									((int32_t)vol_db_h[(mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
							}
#endif
							norm+=temp_energy;

	
							
							//find out the input with max energy, from all input in the sum

							
							if (max_energy<temp_energy)
							{
								max_energy=temp_energy;
								max_energy_index=in_indx;
							}
							//check if last input
							if(j == (ch_mux[i].size-1))
							{
								//work with 6bit energy
								energy_div=0;
								if (mode==AM_MODE_MAX_IN_ENERGY)
								{
									while (max_energy&0xffffffc0)
									{
										energy_div++;
										max_energy=max_energy>>1;								
									}
									norm=norm>>energy_div;
								}
								else
								{
									norm=norm>>5;								

								}								
									
							}
						}				
					}

					//sum in+=in*volume
#ifdef AUDIO_MUX_OLD_VOLUME
					if (mux_matrix(in_indx,out_indx)<0)
						sum+=(int32_t)inp(in_indx,sample_indx)/(int32_t)mux_matrix(in_indx,out_indx);
					else
						sum+=(int32_t)inp(in_indx,sample_indx)*(int32_t)mux_matrix(in_indx,out_indx);

#else

					if (mux_matrix(in_indx,out_indx)<0)
					{
						if ((-mux_matrix(in_indx,out_indx))<AM_VOL_HIGH)						
							//low volume multiply and devide
							sum+=((int32_t)inp(in_indx,sample_indx)<<
							((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][1]))/
							((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][0]);						
						else	
							sum+=((int32_t)inp(in_indx,sample_indx))
							/((int32_t)vol_db_h[(-mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
					}
					else
					{
						if (mux_matrix(in_indx,out_indx)<AM_VOL_HIGH)						
							//low volume multiply and devide
							sum+=((int32_t)inp(in_indx,sample_indx)*
							((int32_t)vol_db[mux_matrix(in_indx,out_indx)][0]))>>
							((int32_t)vol_db[mux_matrix(in_indx,out_indx)][1]);
						else	
							sum+=((int32_t)inp(in_indx,sample_indx))*
							((int32_t)vol_db_h[(mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
					}
#endif					
					
				}	
				else
				{
					//there is only one input 
#ifdef AUDIO_MUX_OLD_VOLUME
					if (mux_matrix(in_indx,out_indx)<0)
					outp(out_indx,sample_indx)=inp(in_indx,sample_indx)/(int32_t)mux_matrix(in_indx,out_indx);	
					else
					outp(out_indx,sample_indx)=inp(in_indx,sample_indx)*(int32_t)mux_matrix(in_indx,out_indx);	

#else					
					if (mux_matrix(in_indx,out_indx)<0)
					{
						if ((-mux_matrix(in_indx,out_indx))<AM_VOL_HIGH)
						{
							//low volume multiply and devide
							outp(out_indx,sample_indx)=(((int32_t)inp(in_indx,sample_indx))<<
							((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][1]))/
							((int32_t)vol_db[-mux_matrix(in_indx,out_indx)][0]);									
						}
						else	
							outp(out_indx,sample_indx)=((int32_t)inp(in_indx,sample_indx))/
							((int32_t)vol_db_h[(-mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
					}
					else
					{
						if (mux_matrix(in_indx,out_indx)<AM_VOL_HIGH)
						{
							//low volume multiply and devide
							outp(out_indx,sample_indx)=(((int32_t)inp(in_indx,sample_indx))*
							((int32_t)vol_db[mux_matrix(in_indx,out_indx)][0]))>>
							((int32_t)vol_db[mux_matrix(in_indx,out_indx)][1]);									
							
						}
						else	
							outp(out_indx,sample_indx)=((int32_t)inp(in_indx,sample_indx))*
							((int32_t)vol_db_h[(mux_matrix(in_indx,out_indx))-AM_VOL_HIGH]);
								
					}
#endif					
					
				}
				
			}
			if (ch_mux[i].size>1)	
			{	
				if (norm<1) norm=1;		
				//normalize output
				if (mode==AM_MODE_MAX_IN_ENERGY)
				{						
					out32bit=(sum*max_energy)/norm;
					//check we are not over max allawed value
					if (out32bit>32765)
						out32bit=32765;
					if (out32bit<(-32765))
						out32bit=-32765;
					outp(out_indx,sample_indx)=(int16_t)(out32bit);	
					//outp(out_indx,sample_indx)=(int16_t)((sum*max_energy)/norm);
				}	
				else if (mode==AM_MODE_WEIGHTED_MEAN)
					outp(out_indx,sample_indx)=(int16_t)((sum)/norm);
				else if (mode==AM_MODE_CONST_ENERGY)
				{
					if ((energy_div==0)&&(max_energy<AM_MIN_IN_ENERGY))
						outp(out_indx,sample_indx)=0;
					else
						outp(out_indx,sample_indx)=(int16_t)((sum*(const_energy))/norm);
				
				}
			}																																																	
		}
	
		
	}
																																																
									
}

/**************************************************************************/                     
//  Module		: calc_energy
//  Description :  sum input samples and divide by the number of samples 
/// @return none
/// @param	none
/**************************************************************************/
#if 1
int32_t calc_energy(int16_t* in,int32_t* in_energy,uint16_t ch_num,uint16_t sample_num)
{
	uint32_t i,j;
	uint64_t energy_ch;	
	//GPIO4_PIN_SET;
	for (i = 0; i < ch_num; i++)
	{
		in_energy[i]=0;
		energy_ch=0;
		//calculate energy on defined channel 
		//if ((ch_config.channel_input>>i)&1)
		if (input_mux_channel[i])
		{			
			for (j = 0; j < sample_num; j++)
			{
				energy_ch+=(((int32_t)((in+(i*sample_num))[j]))*((int32_t)((in+(i*sample_num))[j])))/16;				
			}
			in_energy[i]=sqrt(energy_ch/sample_num);///(((int32_t)sample_num)*4);

		}
	}
	//GPIO4_PIN_CLR;		
	return (0);	
}
#endif
#if 0
int32_t calc_energy(int16_t* in,int32_t* in_energy,uint16_t ch_num,uint16_t sample_num)
{
	uint32_t i,j;
	
	for (i = 0; i < ch_num; i++)
	{
		in_energy[i]=0;
		//calculate energy on defined channel 
		if ((ch_config.channel_input>>i)&1)
		{			
			for (j = 0; j < sample_num; j++)
			{
			
				if ((in+(i*sample_num))[j]<0)
					in_energy[i]-=(int32_t)((in+(i*sample_num))[j]);
				else		
					in_energy[i]+=(int32_t)((in+(i*sample_num))[j]);
								
			}
			in_energy[i]=in_energy[i]/(((int32_t)sample_num)*4);
		}
	}
			
	return (0);	
}
#endif

#if	 (TEST_TYPE == TEST_AUDIO_MUX)
/**************************************************************************/                     
//  Module		: test_audio_mux
//  Description :  test audio mux module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_audio_mux(uint16_t mode)
{
	uint16_t test_count=0,i;
	int16_t temp_res;
	
	ch_config.channel_input=0x7;
	ch_config.channel_output=0x7;

	//********audio mux 0 to 1 and 1 to 0 with no signal
	//mux initialize
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=1;	
		
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=0;	
		
#endif
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

	memset(audio_in_buff,0,sizeof(audio_in_buff));
	TRN_ACB_PTT_LINE_SET;
	//audio_mux(matrix,in buff,out buff,num of inputs,num of outputs,num of sample in block)
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	
	for(i=0;i<NSAMPLES;i++)
	{
		if (audio_out_buff[0][i]!=0)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if (audio_out_buff[1][i]!=0)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	

	}
	
	//********audio mux ch0 to ch1 and 2*ch1 to ch0 with const signal
	//********ch1 in=0x5050, ch2=0x5050, ch3=0x5050
	test_count++;
	//mux initialize
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]	
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif	

#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=2;		
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=6;
#endif	
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

	memset(&audio_in_buff[0][0],0x50,sizeof(audio_in_buff));
	TRN_ACB_PTT_LINE_SET;
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	for(i=0;i<NSAMPLES;i++)
	{
		if (audio_out_buff[0][i]!=(int16_t)(0x5050*2))
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if (audio_out_buff[1][i]!=(int16_t)0x5050)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	

	}

	//********audio mux ch0=ch0+2ch1 with no signal
	test_count++;
	//mux initialize
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif


	
#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=2;	
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=6;
#endif	
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

	memset(audio_in_buff,0,sizeof(audio_in_buff));
	TRN_ACB_PTT_LINE_SET;
	//audio_mux(matrix,in buff,out buff,num of inputs,num of outputs,num of sample in block)
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;	
	for(i=0;i<NSAMPLES;i++)
	{
		if (audio_out_buff[0][i]!=0)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if (audio_out_buff[1][i]!=0)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	

	}

	//********audio mux ch0=ch0+2ch1 with constant signal
	//********ch0 in=0x3030, ch1=0x3030
	test_count++;
	//mux initialize
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]	
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif

	
#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=2;	
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=6;
#endif	
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

	memset(audio_in_buff,0x30,sizeof(audio_in_buff));
	TRN_ACB_PTT_LINE_SET;
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	if (mode==AM_MODE_MAX_IN_ENERGY)
		temp_res=0x5050;
	else if (mode==AM_MODE_CONST_ENERGY)
		temp_res=0x3fff;
	else
		temp_res=0x5050;	
	for(i=0;i<NSAMPLES;i++)
	{
		if (audio_out_buff[0][i]!=(int16_t)(0x6060))
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if (audio_out_buff[1][i]!=0)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	

	}
			



	//********audio mux ch1=ch0+2ch1+5ch2 with constant signal
	//********ch1 in=0x5050, ch2=0x2020, ch3=0x0101
	test_count++;
	//mux initialize
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif		

	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]
	
#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH1]=2;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=5;	
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH1]=6;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=14;
#endif	
	
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;

	memset(&audio_in_buff[0][0],0x50,NSAMPLES*2);
	memset(&audio_in_buff[1][0],0x20,NSAMPLES*2);
	memset(&audio_in_buff[2][0],0x1,NSAMPLES*2);
	TRN_ACB_PTT_LINE_SET;
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	
	for(i=0;i<NSAMPLES;i++)
	{
	
		if (audio_out_buff[0][i]!=0)
		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}
		if (mode==AM_MODE_MAX_IN_ENERGY)
			temp_res=0x5050;
		else if (mode==AM_MODE_CONST_ENERGY)
			temp_res=0x3fff;
		else
			temp_res=0x12b2;
					
		if ((audio_out_buff[1][i]<(temp_res-(temp_res/20)))||(audio_out_buff[1][i]>(temp_res+(temp_res/20))))		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if (audio_out_buff[2][i]!=0)
		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}

	}


	//********audio mux ch1=ch0+2ch1+5ch2 with constant signal
	//********audio mux ch2=ch0+100ch1 with constant signal
	//********ch1=0x7f7f, ch2=0x7070, ch3=0x6060
	test_count++;
	//mux initialize
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif		
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]


#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH2]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH2]=100;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH1]=2;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=5;
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH2]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH1]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH2]=40;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH1]=6;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH2][HD_CH1]=14;
#endif	
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;	
	
	memset(&audio_in_buff[0][0],0x7f,NSAMPLES*2);
	memset(&audio_in_buff[1][0],0x70,NSAMPLES*2);
	memset(&audio_in_buff[2][0],0x60,NSAMPLES*2);
	TRN_ACB_PTT_LINE_SET;
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	
	for(i=0;i<NSAMPLES;i++)
	{
	
		if (audio_out_buff[0][i]!=0)
		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}
		if (mode==AM_MODE_MAX_IN_ENERGY)
			temp_res=0x7f7f;
		else if (mode==AM_MODE_CONST_ENERGY)
			temp_res=0x3fff;
		else
			temp_res=0x12b2;			

		
		if ((audio_out_buff[1][i]<(temp_res-(temp_res/20)))&&(audio_out_buff[1][i]>(temp_res+(temp_res/20))))		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}	
		if ((audio_out_buff[2][i]<(temp_res-(temp_res/20)))&&(audio_out_buff[2][i]>(temp_res+(temp_res/20))))		
		
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}

	}
	
	//********audio mux ch0=ch0+ch1
	//********ch0 in=0x7fff, ch1=30
	test_count++;
	//mux initialize
	//hd_host_ctrl.sHD_mux_configuration.channel_level[in][out]	
#ifdef AUDIO_MUX_OLD_VOLUME	
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,0
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#else
	memset(hd_host_ctrl.sHD_mux_configuration.channel_level,HD_MUX_VOL_MUTE
		,sizeof(hd_host_ctrl.sHD_mux_configuration.channel_level));
#endif
#ifdef AUDIO_MUX_OLD_VOLUME	
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=1;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=1;	
#else
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH0][HD_CH0]=0;
	hd_host_ctrl.sHD_mux_configuration.channel_level[HD_CH1][HD_CH0]=0;	
#endif
	hd_host_ctrl.sHD_mux_configuration.HD_request=TRUE;
	for (i=0;i<NSAMPLES;i++)
	{
		audio_in_buff[0][i]=0x7fff;
		audio_in_buff[1][i]=30;		
	}
	TRN_ACB_PTT_LINE_SET;
	audio_mux(&hd_host_ctrl.sHD_mux_configuration.channel_level[0][0]
		,&audio_in_buff[0][0],&audio_out_buff[0][0],HD_MAX_CHANNELS,HD_MAX_CHANNELS
		,NSAMPLES,mode);
	TRN_ACB_PTT_LINE_CLR;
	for(i=0;i<NSAMPLES;i++)
	{
		if (audio_out_buff[0][i]<32000)
		{
			LOG_printf(&LOG_trace,"audio_mux fail  %d",test_count);		
			break;
		}

	}				
							
													
}
#endif
