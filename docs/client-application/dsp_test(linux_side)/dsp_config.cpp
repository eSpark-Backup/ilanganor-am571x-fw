/* vim: sw=4:expandtab:sts=4:smartindent:formatoptions=tcqlron:cinoptions=l1,t0,\:0,g0
 */

//******************************************************************************/
//                         C. Mer Industries Ltd.
//                  5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright C.Mer Industries all rights reserved, confidential proprietary
//
//  Project		: ACS RAC
//  Author		: Ilan Rosilio		
//  Description : 
/// 	example how to use the hpi channel
///	@file:	hpi_main.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	20/aug/2006	  Created	
//Ilan Rosilio	27/aug/2006	  Working version
//
//******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>


#include "dsp_config.h"

//int error_check=1;
#define dly_time 500000
/**************************************************************************/                  
//  Module		: dsp_test
//  Description : 
///		 example of using dsp_control   		
/// @return none
/// @param	none
/**************************************************************************/
int main(int argc,char *argv[])
{
int board_name[4];
	
	printf("-----------dsp_config ver 1.11 ------------");
	if (argc<2)
	{
		printf("run this program with board name as parameter\n");
		printf("options are: dsp_config, acb3(fire consule), acb4\n");
        printf(",mvt, acb(same as mvt), macs, macs radio non\n");
		printf(", macs non radio, macs radio radio, macs tel field_tel, macs-2r-1t\n");
		printf(", racb2, acb11, svt, vt, infra, rcgw, acb14, acb12\n");
        printf("macs-r, macs-r radio non, macs-r non radio, macs-r radio radio \n");
                


		return(-1);

	}
	if (strcmp(argv[1],"acb3")==0)
		board_name[0]=ACB3_BOARD;
	else if (strcmp(argv[1],"acb4")==0)
		board_name[0]=ACB4_BOARD;
	else if (strcmp(argv[1],"mvt")==0)
		board_name[0]=MVT_BOARD;
	else if (strcmp(argv[1],"acb")==0)
		board_name[0]=MVT_BOARD;
                
	else if ((strcmp(argv[1],"macs")==0)||(strcmp(argv[1],"macs-r")==0))
	{
		if (argc<3)
			return (-1);
		if (strcmp(argv[1],"macs-r")==0)
		    board_name[0]=MACS_R_BOARD;
		else
		    board_name[0]=MACS_BOARD;
		if(strcmp(argv[2],"radio")==0)
			board_name[1]=MACS_RADIO_BOARD;
		else if(strcmp(argv[2],"field_tel")==0)
			board_name[1]=MACS_FIELD_TEL_BOARD;
		else if(strcmp(argv[2],"tel")==0)
			board_name[1]=MACS_TEL_BOARD;
		else
			board_name[1]=NON_BOARD;
		if (argc<4)
			board_name[2]=NON_BOARD;
		else
		{
			if(strcmp(argv[3],"radio")==0)
				board_name[2]=MACS_RADIO_BOARD;
			else if(strcmp(argv[3],"field_tel")==0)
				board_name[2]=MACS_FIELD_TEL_BOARD;
			else if(strcmp(argv[3],"tel")==0)
				board_name[2]=MACS_TEL_BOARD;
			else
				board_name[2]=NON_BOARD;
			if (argc==5)
			{
				if(strcmp(argv[4],"g711")==0)
					board_name[3]=VOCODER_G711;
			}
		}
	}
	else if (strcmp(argv[1],"macs-2r-1t")==0)
	{	
		board_name[0]=MACS_2R_1T_BOARD;
		if (argc>2)
		{
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
		}
	}

	else if (strcmp(argv[1],"racb2")==0)
		board_name[0]=RACB2_BOARD;

	else if (strcmp(argv[1],"acb11")==0)
		board_name[0]=ACB11_BOARD;

	else if (strcmp(argv[1],"svt")==0)
        {
		board_name[0]=SVT_BOARD;
                board_name[1]=VOCODER_G711;
		if (argc>2)
		{
			
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
			if(strcmp(argv[2],"pcm")==0)
				board_name[1]=VOCODER_PCM;
		}
        }

	else if (strcmp(argv[1],"vt")==0)
        {
		board_name[0]=VT_BOARD;
		if (argc>2)
		{
			board_name[1]=VOCODER_G711;
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
			if(strcmp(argv[2],"pcm")==0)
				board_name[1]=VOCODER_PCM;
		}
        }
	else if (strcmp(argv[1],"acb14")==0)
    {
		board_name[0]=ACB14_BOARD;
		if (argc>2)
		{
			board_name[1]=VOCODER_G711;
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
			if(strcmp(argv[2],"pcm")==0)
				board_name[1]=VOCODER_PCM;
		}
    }
	else if (strcmp(argv[1],"acb12")==0)
    {
		board_name[0]=ACB12_BOARD;
		if (argc>2)
		{
			board_name[1]=VOCODER_G711;
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
			if(strcmp(argv[2],"pcm")==0)
				board_name[1]=VOCODER_PCM;
		}
    }
	else if (strcmp(argv[1],"infra")==0)
		board_name[0]=INFRA_BOARD;
	else if (strcmp(argv[1],"rcgw")==0)
        {
		board_name[0]=RCGW_BOARD;
		if (argc>2)
		{
			board_name[1]=VOCODER_G711;
			if(strcmp(argv[2],"g729")==0)
				board_name[1]=VOCODER_G729;
			if(strcmp(argv[2],"pcm")==0)
				board_name[1]=VOCODER_PCM;
		}
        }        
	else
		return(-1);
        if (board_name[0]==MACS_R_BOARD)
        {
	    dsp_init(0);
	    dsp_init(1);
        }
        else
    	    dsp_init();

	print_dsp_version(board_name);
	set_gpio_config(board_name);
	set_gpio_set_state(board_name);
	set_channel_config(board_name);
	set_ac_init(board_name);
	usleep(dly_time);
	set_ac_config(board_name);
	set_vocoder_init(board_name);
	set_vocoder_enable(board_name);


	dsp_fini();
	return(0);
}
/**************************************************************************/                  
//  Module		: print_dsp_version
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int print_dsp_version(int *board_name)
{
   	int rv = DSPLIB_ERR_OK,dsp_num=1;
    	HD_version_info dsp_info;
	for(dsp_num=0;dsp_num<2;dsp_num++)
	{
		
		rv = dsp_version(&dsp_info,dsp_num);
		printf("---------------------------------------------------------------------\n");

		if(rv == DSPLIB_ERR_OK) {
			printf ("DSP number %d\n",dsp_num);
			printf("DSP: ver %d.%d.%d\n", dsp_info.MajorVersionNumber,
					dsp_info.MinorVersionNumber, 
					dsp_info.BugfixReleaseNumber);
			printf("DSP: %s\n", dsp_info.sVendorInfo);
			printf("DSP: %s\n", dsp_info.sDescription);
			printf("DSP: %s\n", dsp_info.sVersionTag);
		}
		printf("---------------------------------------------------------------------\n");
		if (board_name[0]!=MACS_R_BOARD)
			break;
				
	}
    return rv;
}


/**************************************************************************/                  
//  Module		: set_channel_config
//  Description : config number of host and codec channels
///		    		
/// @return none
/// @param	board_name 
/**************************************************************************/
int set_channel_config(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,start_ch,dsp_num=0,dsp_max_num=1;
	HD_channel_configuration ch_config;

	

        if (board_name[0]==MACS_R_BOARD)
	    dsp_max_num=2;
        for (dsp_num=0;dsp_num<dsp_max_num;dsp_num++)
        {
            memset(&ch_config,0,sizeof(ch_config));

	    switch (board_name[0])
	    {
		    //fire
		    case ACB3_BOARD:
			    ch_config.number_of_mcbsp0_channel=2;
			    ch_config.number_of_mcbsp1_channel=4;
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=sizeof(HD_channel_frame)+160;
			    ch_config.channel_frame_type[0]=HD_PCM_FRAME;

		    break;
		
		    //ots
		    case ACB4_BOARD:
			    ch_config.number_of_mcbsp0_channel=3;
			    ch_config.number_of_mcbsp1_channel=0;
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=
				    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
			    ch_config.channel_frame_type[0]=HD_G711_FRAME;

		    break;

		
		    case MVT_BOARD:
			    ch_config.number_of_mcbsp0_channel=4;
			    ch_config.number_of_mcbsp1_channel=0;
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=
				    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
			    ch_config.channel_frame_type[0]=HD_G711_FRAME;

		    break;

		    //macs board
		    case MACS_BOARD:
			    switch (board_name[1])
			    {
				    case MACS_RADIO_BOARD:
				    case MACS_FIELD_TEL_BOARD:
				    case MACS_TEL_BOARD:
					    ch_config.number_of_mcbsp0_channel=8;
					    ch_config.number_of_mcbsp1_channel=0;
					    ch_config.number_of_host_dsp_channel=8;
					    ch_config.number_of_tone_channel=0;
					    for(i=0;i<8;i++)
					    {
						    if (board_name[3]==VOCODER_G711)
						    {
						    ch_config.channel_frame_size[i]=
							    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
						    ch_config.channel_frame_type[i]=HD_G711_FRAME;
						    }
						    else
						    {
						    ch_config.channel_frame_size[i]=
							    sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
						    ch_config.channel_frame_type[i]=HD_G729_FRAME;
						    }
					    }
					    start_ch=8;
				    break;
				    default:
					    start_ch=0;
				    break;
			    }
			    switch (board_name[2])
			    {
				    case MACS_RADIO_BOARD:
				    case MACS_FIELD_TEL_BOARD:
				    case MACS_TEL_BOARD:
					    ch_config.number_of_mcbsp0_channel=start_ch;
					    ch_config.number_of_mcbsp1_channel=8;
					    ch_config.number_of_host_dsp_channel+=8;
					    ch_config.number_of_tone_channel=0;
					    for(i=start_ch;i<(start_ch+8);i++)
					    {
						    if (board_name[3]==VOCODER_G711)
						    {
							    ch_config.channel_frame_size[i]=
								    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
							    ch_config.channel_frame_type[i]=HD_G711_FRAME;
						    }
						    else
						    {
							    ch_config.channel_frame_size[i]=
								    sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
							    ch_config.channel_frame_type[i]=HD_G729_FRAME;
					
						    }
					    }
					
				    break;
				    default:
				    break;
			    }
		    break;
		    //macs-2r-1t board
		    case MACS_2R_1T_BOARD:
			    ch_config.number_of_mcbsp0_channel=2;
			    ch_config.number_of_mcbsp1_channel=1;
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=0;
			    for(i=0;i<3;i++)
			    {
				    if (board_name[1]==VOCODER_G729)
				    {
				    ch_config.channel_frame_size[i]=
					    sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
				    ch_config.channel_frame_type[i]=HD_G729_FRAME;

				    }
				    else
				    {
				    ch_config.channel_frame_size[i]=
					    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
				    ch_config.channel_frame_type[i]=HD_G711_FRAME;
				    }
			    }
		    break;
                    //nMDIS_RACB2
		    case RACB2_BOARD:
			    ch_config.number_of_mcbsp0_channel=1;
			    ch_config.number_of_mcbsp1_channel=1;
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=
				    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
			    ch_config.channel_frame_type[0]=HD_G711_FRAME;

		    break;
                    
                   	case ACB11_BOARD:
			    ch_config.number_of_mcbsp0_channel=8;
			    ch_config.number_of_mcbsp1_channel=0; //todo define chanelles for communication with arm
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=
				    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
			    ch_config.channel_frame_type[0]=HD_G711_FRAME;

		    break;

		    case SVT_BOARD:
			    ch_config.number_of_mcbsp0_channel=2;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=4;
			    for(i=0;i<3;i++)
			    {

		                if (board_name[1]==VOCODER_G729)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
		                    ch_config.channel_frame_type[i]=HD_G729_FRAME;

		                }
		                else if (board_name[1]==VOCODER_G711)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_G711_FRAME;
		                }
		                else 
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);
		                    ch_config.channel_frame_type[i]=HD_PCM_FRAME;
		                }

                            }

		    break;
		    case SVT2_BOARD:
                ch_config.number_of_mcbsp0_channel=2;
                ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
                ch_config.number_of_host_dsp_channel=2;
                ch_config.number_of_tone_channel=4;
                for(i=0;i<3;i++)
                {

                        if (board_name[1]==VOCODER_G729)
                        {
                            ch_config.channel_frame_size[i]=
                            sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
                            ch_config.channel_frame_type[i]=HD_G729_FRAME;

                        }
                        else if (board_name[1]==VOCODER_G711)
                        {
                            ch_config.channel_frame_size[i]=
                            sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
                            ch_config.channel_frame_type[i]=HD_G711_FRAME;
                        }
                        else
                        {
                            ch_config.channel_frame_size[i]=
                            sizeof(HD_channel_frame)+sizeof(HD_audio_pcm_frame);
                            ch_config.channel_frame_type[i]=HD_PCM_FRAME;
                        }

                            }

            break;
		    case VT_BOARD:
			    ch_config.number_of_mcbsp0_channel=3;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=4;
			    for(i=0;i<3;i++)
			    {

		                if (board_name[1]==VOCODER_G729)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
		                    ch_config.channel_frame_type[i]=HD_G729_FRAME;

		                }
		                else if (board_name[1]==VOCODER_G711)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_G711_FRAME;
		                }
		                else 
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_PCM_FRAME;
		                }

                 }
		    break;
		    case ACB14_BOARD:
			    ch_config.number_of_mcbsp0_channel=6;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=4;
			    for(i=0;i<3;i++)
			    {

		                if (board_name[1]==VOCODER_G729)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
		                    ch_config.channel_frame_type[i]=HD_G729_FRAME;

		                }
		                else if (board_name[1]==VOCODER_G711)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_G711_FRAME;
		                }
		                else
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_PCM_FRAME;
		                }

                 }
		    break;
		    case ACB12_BOARD:
			    ch_config.number_of_mcbsp0_channel=4;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=4;
			    for(i=0;i<3;i++)
			    {

		                if (board_name[1]==VOCODER_G729)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
		                    ch_config.channel_frame_type[i]=HD_G729_FRAME;

		                }
		                else if (board_name[1]==VOCODER_G711)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_G711_FRAME;
		                }
		                else
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_PCM_FRAME;
		                }

                 }
		    break;

		    case INFRA_BOARD:
			    ch_config.number_of_mcbsp0_channel=4;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=1;
			    ch_config.number_of_tone_channel=4;
			    ch_config.channel_frame_size[0]=
				    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
			    ch_config.channel_frame_type[0]=HD_G711_FRAME;

		    break;

		    case RCGW_BOARD:
			    ch_config.number_of_mcbsp0_channel=16;
			    ch_config.number_of_mcbsp1_channel=0;//TBD, arm communication
			    ch_config.number_of_host_dsp_channel=3;
			    ch_config.number_of_tone_channel=4;
			    for(i=0;i<3;i++)
			    {

		                if (board_name[1]==VOCODER_G729)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
		                    ch_config.channel_frame_type[i]=HD_G729_FRAME;

		                }
		                else if (board_name[1]==VOCODER_G711)
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_G711_FRAME;
		                }
		                else 
		                {
		                    ch_config.channel_frame_size[i]=
			                sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
		                    ch_config.channel_frame_type[i]=HD_PCM_FRAME;
		                }

                            }

		    break;

		    //macs-r board
		    case MACS_R_BOARD:
			    switch (board_name[1+dsp_num])
			    {
				    case MACS_RADIO_BOARD:
				    case MACS_FIELD_TEL_BOARD:
				    case MACS_TEL_BOARD:
					    ch_config.number_of_mcbsp0_channel=8;
					    //TODO add support to mcbsp communication with ARM
					    ch_config.number_of_mcbsp1_channel=0;
					    ch_config.number_of_host_dsp_channel=8;
					    ch_config.number_of_tone_channel=0;
					    for(i=0;i<8;i++)
					    {
						    if (board_name[3]==VOCODER_G711)
						    {
						    ch_config.channel_frame_size[i]=
							    sizeof(HD_channel_frame)+sizeof(HD_audio_g711_frame);
						    ch_config.channel_frame_type[i]=HD_G711_FRAME;
						    }
						    else
						    {
						    ch_config.channel_frame_size[i]=
							    sizeof(HD_channel_frame)+sizeof(HD_audio_g729_frame);
						    ch_config.channel_frame_type[i]=HD_G729_FRAME;
						    }
					    }
					    //printf ("Configure DSP %d",state+1);
					
				    break;

				    default:
				    break;

					
			    }
			       
		    break;
     
	    }
	    printf("---------------------------------------------------------------------\n");
        printf ("DSP number %d\n",dsp_num);
	    printf("ch_config.number_of_mcbsp0_channeil=%d\n",ch_config.number_of_mcbsp0_channel);
	    printf("ch_config.number_of_mcbsp1_channel=%d\n",ch_config.number_of_mcbsp1_channel);
	    printf("ch_config.number_of_i2c0_channeil=%d\n",ch_config.number_of_i2c0_channel);
	    printf("ch_config.number_of_i2c1_channeil=%d\n",ch_config.number_of_i2c1_channel);
	    printf("ch_config.number_of_host_dsp_channel=%d\n",
		    ch_config.number_of_host_dsp_channel);
	    printf("ch_config.number_of_tone_channeil=%d\n",ch_config.number_of_tone_channel);

	    rv=hpi_write_HD_channel_configuration ( &ch_config,dsp_num );
        	if(rv != DSPLIB_ERR_OK) 
	    {
		    PRINT_ERR_START;
		    printf("fail to set DSP channel_configuration\n");
		    PRINT_ERR_STOP;
        	}
	    else
	    {
		    PRINT_OK_START;
		    printf("channel_configuration succeed\n");
		    PRINT_OK_STOP;
     	    }
	    printf("---------------------------------------------------------------------\n");
        }
    	return rv;
}


/**************************************************************************/                  
//  Module		: ac_init
//  Description : configure type of hw codecs
///		    		
/// @return none
/// @param	board_name
/**************************************************************************/
int set_ac_init(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,state=0,start_ch=0,dsp_num=0;
	HD_ac_initialization	ac_init;
	
	while(state<10)
	{
		memset(&ac_init,0,sizeof(ac_init));

		switch (board_name[0])
		{
			//fire
			case ACB3_BOARD:
				if (state==0)
				{
					ac_init.channels_mask[0]=1;
					ac_init.channels_mask[1]=1;
					ac_init.codec_type=HD_CODEC_AIC14;
					ac_init.ch_type=HD_CH_IN_OUT_TYPE;
					state=1;
					break;
				}
				if (state==1)
				{
					ac_init.channels_mask[2]=1;
					ac_init.channels_mask[3]=1;
					ac_init.channels_mask[4]=1;
					ac_init.channels_mask[5]=1;	
					ac_init.codec_type=HD_CODEC_AIC24;
					ac_init.ch_type=HD_CH_IN_OUT_TYPE;
					state=10;
					break;
				}
			break;

			//ots
			case ACB4_BOARD:
				ac_init.channels_mask[0]=1;
				ac_init.channels_mask[1]=1;
				ac_init.channels_mask[2]=1;
				ac_init.codec_type=HD_CODEC_AIC14;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
			break;

			case MVT_BOARD:
			case ACB12_BOARD:
				ac_init.channels_mask[0]=1;
				ac_init.channels_mask[1]=1;
				ac_init.channels_mask[2]=1;
				ac_init.channels_mask[3]=1;
				ac_init.codec_type=HD_CODEC_AIC24;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
			break;


			//macs board
			case MACS_BOARD:
			case MACS_R_BOARD:
		
				if(state==0)
				{
					if (board_name[0]== MACS_R_BOARD)
					{	
						ac_init.codec_type=HD_CODEC_AIC12;
						dsp_num=0;
					}
					else
						ac_init.codec_type=HD_CODEC_AIC24;

					switch (board_name[1])
					{
						case MACS_TEL_BOARD:											
						ac_init.codec_type=HD_CODEC_SI3056;
							
						case MACS_RADIO_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=0;i<8;i++)
							{
								ac_init.channels_mask[i]=1;
							}
							ac_init.ch_type=HD_CH_IN_OUT_TYPE;
							if (board_name[0]== MACS_BOARD)	
								start_ch=8;
							state=1;

						break;

						default:
							start_ch=0;
							state=1;
						break;
					}
					break;
				}

				if(state==1)
				{
				
					if (board_name[0]== MACS_R_BOARD)
					{	
						ac_init.codec_type=HD_CODEC_AIC12;
						dsp_num=1;
					}
					else
						ac_init.codec_type=HD_CODEC_AIC24;
					switch (board_name[2])
					{
						case MACS_TEL_BOARD:
							printf("tel");

							ac_init.codec_type=HD_CODEC_SI3056;
						case MACS_RADIO_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=start_ch;i<(start_ch+8);i++)
							{
								ac_init.channels_mask[i]=1;
							}
							ac_init.ch_type=HD_CH_IN_OUT_TYPE;
							state=10;

						break;

						default:
							state=10;
						break;
					}
					break;
				}

			break;

			case MACS_2R_1T_BOARD:
				if (state==0)
				{
					ac_init.channels_mask[0]=1;
					ac_init.channels_mask[1]=1;
					ac_init.codec_type=HD_CODEC_AIC24;
					ac_init.ch_type=HD_CH_IN_OUT_TYPE;
					state=1;
					break;
				}
				if (state==1)
				{
					ac_init.channels_mask[2]=1;
					ac_init.codec_type=HD_CODEC_SI3056;
					ac_init.ch_type=HD_CH_IN_OUT_TYPE;
					state=10;
					break;
				}
			break;

			//nMDIS_RACB2
			case RACB2_BOARD:
				if (state==0)
				{
					ac_init.channels_mask[0]=1;
					ac_init.codec_type=HD_CODEC_AIC14;
					ac_init.ch_type=HD_CH_IN_OUT_TYPE;
					state=1;
					break;
				}
                                
				if (state==1)
				{
					ac_init.channels_mask[1]=1;
					ac_init.codec_type=HD_LED_A6812;
					ac_init.ch_type=HD_CH_OUT_TYPE;
					state=10;
					break;
				}
                                
			break;

			case ACB11_BOARD:
				for (i=0;i<8;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC24;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break;
				
			case SVT_BOARD:
				for (i=0;i<2;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC12;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break;

            case SVT2_BOARD:
                for (i=0;i<2;i++)
                    ac_init.channels_mask[i]=1;
                ac_init.codec_type=HD_CODEC_AIC12;
                ac_init.ch_type=HD_CH_IN_OUT_TYPE;
                state=10;
                break;

			case VT_BOARD:
				for (i=0;i<3;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC12;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break;

			case ACB14_BOARD:
				for (i=0;i<6;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC24;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break;

			case INFRA_BOARD:
				for (i=0;i<4;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC24;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break; 
			case RCGW_BOARD:
				for (i=0;i<16;i++)
					ac_init.channels_mask[i]=1;
				ac_init.codec_type=HD_CODEC_AIC12;
				ac_init.ch_type=HD_CH_IN_OUT_TYPE;
				state=10;
				break;                               
			default:
				state=10;
			break;
			

		}

		printf("---------------------------------------------------------------------\n"
			);
        printf ("DSP number %d\n",dsp_num);
		printf("ac_init.channel_mask=");
		for(i=0;i<HD_MAX_CODEC_CHANNELS;i++)
		{
			printf("%d",ac_init.channels_mask[(HD_MAX_CODEC_CHANNELS-1)-i]);
		}
		printf("\n");

		printf("ac_init.codec_type=%d\n",ac_init.codec_type);
		printf("ac_init.channel_type=%d\n",ac_init.ch_type);


		if (ac_init.ch_type)
		{
			rv=hpi_write_HD_ac_initialization ( &ac_init,dsp_num );
			if(rv != DSPLIB_ERR_OK) 
			{
				PRINT_ERR_START;
				printf("fail to set DSP ac_initialization\n");
				PRINT_ERR_STOP;
			}
			else
			{
				PRINT_OK_START;
				printf("ac_initialization succeed\n");
				PRINT_OK_STOP;
			}
		}
		printf("---------------------------------------------------------------------\n"			);
	}
    	return rv;
}

/**************************************************************************/                  
//  Module		: ac_config
//  Description : configure hw codec parameters (gain, etc..)
///		    		
/// @return none
/// @param	board_name
/**************************************************************************/
int set_ac_config(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,state=0,start_ch=0,dsp_num=0;
	HD_ac_configuration	ac_config;
	
	while(state<10)
	{
		memset(&ac_config,0,sizeof(ac_config));

		switch (board_name[0])
		{
			//fire
			case ACB3_BOARD:
				if (state==0)
				{
					ac_config.channels_mask[0]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=-6;
					state=1;
					break;
				}
				if (state==1)
				{
					ac_config.channels_mask[1]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=3;
					ac_config.dac_gain=-6;
					state=2;
					break;
				}

				if (state==2)
				{
					ac_config.channels_mask[2]=1;
					ac_config.channels_mask[3]=1;
					ac_config.channels_mask[4]=1;
					ac_config.channels_mask[5]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=9;
					ac_config.dac_gain=-5;
					state=10;
					break;
				}
			break;

			//ots
			case ACB4_BOARD:
				if (state==0)
				{
					ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=9;
					ac_config.dac_gain=-5;
					state=1;
					break;
				}

				if (state==1)
				{
					ac_config.channels_mask[2]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=0;
					state=10;
					break;
				}

			break;

			case MVT_BOARD:
				if (state==0)
				{
					//HS Left and right
					ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=5;
					ac_config.dac_gain=0;
					state=1;
					break;
				}

				if (state==1)
				{
					//line in and out
					ac_config.channels_mask[2]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=0;
					state=2;
					break;
				}
				if (state==2)
				{
					//speaker
					ac_config.channels_mask[3]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=-6;
					state=10;
					break;
				}

			break;


			//macs board
			case MACS_BOARD:
				if(state==0)
				{
			
					switch (board_name[1])
					{
						case MACS_TEL_BOARD:
						case MACS_RADIO_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=0;i<8;i++)
							{
								ac_config.channels_mask[i]=1;
							}
							ac_config.analog_loopback=0;
							ac_config.digital_loopback=0;
							ac_config.powerdown=0;
							if(board_name[1]==MACS_TEL_BOARD)
							{
								ac_config.adc_gain=0;
								ac_config.dac_gain=0;
							}
							if(board_name[1]==MACS_RADIO_BOARD)
							{
								ac_config.adc_gain=17;
								ac_config.dac_gain=0;
							}
							if(board_name[1]==MACS_FIELD_TEL_BOARD)
							{
								ac_config.adc_gain=25;
								ac_config.dac_gain=0;
							}

							start_ch=8;
							state=1;

						break;

						default:
							start_ch=0;
							state=1;
						break;
					}
					break;
				}
				if(state==1)
				{
			
					switch (board_name[2])
					{
						case MACS_TEL_BOARD:
						case MACS_RADIO_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=start_ch;i<(start_ch+8);i++)
							{
								ac_config.channels_mask[i]=1;
							}
							ac_config.analog_loopback=0;
							ac_config.digital_loopback=0;
							ac_config.powerdown=0;
							if(board_name[2]==MACS_TEL_BOARD)
							{
								ac_config.adc_gain=0;
								ac_config.dac_gain=0;
							}
							if(board_name[2]==MACS_RADIO_BOARD)
							{
								ac_config.adc_gain=17;
								ac_config.dac_gain=0;
							}
							if(board_name[2]==MACS_FIELD_TEL_BOARD)
							{
								ac_config.adc_gain=25;
								ac_config.dac_gain=0;
							}

							state=10;

						break;

						default:
							state=10;
						break;
					}
					break;
				}


			break;


			//macs-r board
			case MACS_R_BOARD:
				if(state==0)
				{
					dsp_num=0;
					switch (board_name[1])
					{
						case MACS_RADIO_BOARD:
						case MACS_TEL_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=0;i<8;i++)
							{
								ac_config.channels_mask[i]=1;
							}
							ac_config.analog_loopback=0;
							ac_config.digital_loopback=0;
							ac_config.adc_gain=11;
							ac_config.dac_gain=-14;
							if(board_name[1]==MACS_TEL_BOARD)
							{
							    ac_config.adc_gain=0;
							    ac_config.dac_gain=0;
							
							}
							if(board_name[1]==MACS_FIELD_TEL_BOARD)
							{
								ac_config.adc_gain=25;
								ac_config.dac_gain=0;
							}
							
							state=1;

						break;

						default:
							state=1;
						break;
					}
					break;
				}
				if(state==1)
				{
					dsp_num=1;
					switch (board_name[2])
					{
						case MACS_RADIO_BOARD:
						case MACS_TEL_BOARD:
						case MACS_FIELD_TEL_BOARD:
							for(i=0;i<8;i++)
							{
								ac_config.channels_mask[i]=1;
							}
							ac_config.analog_loopback=0;
							ac_config.digital_loopback=0;
							ac_config.powerdown=0;
							ac_config.adc_gain=11;
							ac_config.dac_gain=-14;
							if(board_name[2]==MACS_TEL_BOARD)
							{
							    ac_config.adc_gain=0;
							    ac_config.dac_gain=0;							
							}
							if(board_name[2]==MACS_FIELD_TEL_BOARD)
							{
								ac_config.adc_gain=25;
								ac_config.dac_gain=0;
							}

							
							state=10;

						break;

						default:
							state=10;
						break;
					}
					break;
				}


			break;

			case MACS_2R_1T_BOARD:
				if (state==0)
				{
					ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=21;
					ac_config.dac_gain=0;
					state=1;
					break;
				}

				if (state==1)
				{
					ac_config.channels_mask[2]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=0;
					state=10;
					break;
				}
			break;
			
			
			case RACB2_BOARD:
				if (state==0)
				{
					ac_config.channels_mask[0]=1;								       
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=9;
					ac_config.dac_gain=-17;
					state=10;
					break;
				}
                                /*
				if (state==1)
				{
					ac_config.channels_mask[2]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=0;
					state=10;
					break;
				}
                                */

			break;

			case ACB11_BOARD:
				if (state==0)
				{
				     	ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
					ac_config.channels_mask[2]=1;
					ac_config.channels_mask[3]=1;
					ac_config.channels_mask[4]=1;

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=9;
				     	ac_config.dac_gain=0;
				     	state=1;
				     	break;
				}
				if (state==1)
				{
				     	ac_config.channels_mask[5]=1;

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=20;
				     	ac_config.dac_gain=-4;
				     	state=2;
				     	break;
				}
				if (state==2)
				{
				     	ac_config.channels_mask[6]=1;
					ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=0;
				     	ac_config.dac_gain=-20;
				     	state=3;
				     	break;
				}
				if (state==3)
				{
     					ac_config.channels_mask[7]=1;
				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=35;
				     	ac_config.dac_gain=0;
				     	state=10;
				     	break;
				}

			break;

			case SVT_BOARD:
				if (state==0)
				{
				     	ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=31;
				     	ac_config.dac_gain=0;
                                        ac_config.codec_reg=0x8904;//set register 6 (addr 9) to mic input (psedo diffrential)
				     	state=10;
				     	break;
				}

                        break;

			case SVT2_BOARD:
                if (state==0)
                {
                        ac_config.channels_mask[0]=1;
                    ac_config.channels_mask[1]=1;

                        ac_config.analog_loopback=0;
                        ac_config.digital_loopback=0;
                        ac_config.powerdown=0;
                        ac_config.adc_gain=31;
                        ac_config.dac_gain=0;
                                        ac_config.codec_reg=0x8904;//set register 6 (addr 9) to mic input (psedo diffrential)
                        state=10;
                        break;
                }

                        break;

			case VT_BOARD:
				if (state==0)
				{
				     	ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=31;
				     	ac_config.dac_gain=0;
                                        ac_config.codec_reg=0x8904;//set register 6 (addr 9) to mic input (psedo diffrential)
				     	state=1;
				     	break;
				}

				if (state==1)
				{
				     	ac_config.channels_mask[0]=0;
					ac_config.channels_mask[1]=0;
                                        ac_config.channels_mask[2]=1;

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=0;
				     	ac_config.dac_gain=-18;
                                        ac_config.codec_reg=0;
				     	state=10;
				     	break;
				}
                        break;

			case ACB14_BOARD:
				if (state==0)
				{
				     	ac_config.channels_mask[0]=1;//HSL
				     	ac_config.channels_mask[1]=1;//HSL
				     	ac_config.channels_mask[2]=1;//HSM
				     	ac_config.channels_mask[3]=1;//HSM

				     	ac_config.analog_loopback=0;
				     	ac_config.digital_loopback=0;
				     	ac_config.powerdown=0;
				     	ac_config.adc_gain=12;
				     	ac_config.dac_gain=-6;
                                        ac_config.codec_reg=0;
				     	state=1;
				     	break;
				}
				if (state==1)
				{
			            ac_config.channels_mask[0]=0;//HSL
			            ac_config.channels_mask[1]=0;//HSL
			            ac_config.channels_mask[2]=0;//HSM
			            ac_config.channels_mask[3]=0;//HSM

				    ac_config.channels_mask[4]=1;//oxygen mic+spkr

				    ac_config.analog_loopback=0;
				    ac_config.digital_loopback=0;
				    ac_config.powerdown=0;
				    ac_config.adc_gain=7;
				    ac_config.dac_gain=-22;
				    ac_config.codec_reg=0xcc04;//set reg 6A(12) to mic input(4) 
				    state=2;
				    break;
				}
				if (state==2)
				{
			     	    ac_config.channels_mask[0]=0;//HSL
			     	    ac_config.channels_mask[1]=0;//HSL
			     	    ac_config.channels_mask[2]=0;//HSM
			     	    ac_config.channels_mask[3]=0;//HSM
			     	    ac_config.channels_mask[4]=0;//HSL

				    ac_config.channels_mask[5]=1;//line
				    ac_config.analog_loopback=0;
				    ac_config.digital_loopback=0;
				    ac_config.powerdown=0;
				    ac_config.adc_gain=0;
				    ac_config.dac_gain=0;
				    ac_config.codec_reg=0;
				    state=10;
				    break;
				}
            break;

			case ACB12_BOARD:
				if (state==0)
				{
					//HS Left and right
					ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=6;
					ac_config.dac_gain=-6;
					state=1;
					break;
				}

				if (state==1)
				{
					//line in and out
					ac_config.channels_mask[2]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=17;
					ac_config.dac_gain=0;
					state=2;
					break;
				}
				if (state==2)
				{
					//speaker
					ac_config.channels_mask[3]=1;
					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=-11;
					state=10;
					break;
				}

			break;

			case INFRA_BOARD:
				if (state==0)
				{
				    ac_config.channels_mask[0]=1;
					ac_config.channels_mask[1]=1;
				    ac_config.channels_mask[2]=1;
					ac_config.channels_mask[3]=1;

					ac_config.analog_loopback=0;
					ac_config.digital_loopback=0;
					ac_config.powerdown=0;
					ac_config.adc_gain=0;
					ac_config.dac_gain=0;
					state=10;
					break;
				}
                        break;
			case RCGW_BOARD:
				if (state==0)
				{
				        for(i=0;i<16;i++)
				        {
					        ac_config.channels_mask[i]=1;
				        }
				        ac_config.analog_loopback=0;
				        ac_config.digital_loopback=0;
				        ac_config.adc_gain=11;
				        ac_config.dac_gain=-14;
				
				        state=10;
                                        break;
				}
                        break;




			break;	
		}

	
		printf("---------------------------------------------------------------------\n");
                printf ("DSP number %d\n",dsp_num);
		printf("ac_config.channel_mask=");
		for(i=0;i<HD_MAX_CODEC_CHANNELS;i++)
		{
			printf("%d",ac_config.channels_mask[(HD_MAX_CODEC_CHANNELS-1)-i]);
		}
		printf("\n");	
		printf("ac_config.analog_loopback=%d\n",ac_config.analog_loopback);
		printf("ac_config.digital_loopback=%d\n",ac_config.digital_loopback);
		printf("ac_config.powerdown=%d\n",ac_config.powerdown);
		printf("ac_config.adc_gain=%d\n",ac_config.adc_gain);
		printf("ac_config.dac_gain=%d\n",ac_config.dac_gain);
                printf("ac_config.codec_reg=0x%x\n",ac_config.codec_reg);





		rv=hpi_write_HD_ac_configuration ( &ac_config,dsp_num );
		if(rv != DSPLIB_ERR_OK) 
		{
			PRINT_ERR_START;
			printf("fail to set DSP ac_configuration!!!\n");
			PRINT_ERR_STOP;
		}
		else
		{
			PRINT_OK_START;
			printf("ac_configuration succeed!!!\n");
			PRINT_OK_STOP;
		}
		printf("---------------------------------------------------------------------\n");

	}
    	return rv;
}


/**************************************************************************/                  
//  Module		: set_gpio_config
//  Description : set i/o direction 
///		    		
/// @return none
/// @param	board_name
/**************************************************************************/
int set_gpio_config(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,dsp_num=0,dsp_max_num=1;
	HD_gpio_configuration	gpio_config;

	memset(&gpio_config,0,sizeof(gpio_config));
    if (board_name[0]==MACS_R_BOARD)
    	dsp_max_num=2;
    for (dsp_num=0;dsp_num<dsp_max_num;dsp_num++)
    {
       	switch (board_name[0])
       	{
			//fire
			case ACB3_BOARD:
				//ots
			case ACB4_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;
				gpio_config.in_out[1]=DC_GPIO_IN;
				gpio_config.in_out[2]=DC_GPIO_IN;
				gpio_config.in_out[3]=DC_GPIO_IN;
				gpio_config.in_out[4]=DC_GPIO_OUT;
				gpio_config.in_out[5]=DC_GPIO_OUT;
				gpio_config.in_out[6]=DC_GPIO_OUT;
				gpio_config.in_out[7]=DC_GPIO_OUT;
				break;

			case MVT_BOARD:
			case ACB12_BOARD:

				gpio_config.in_out[0]=DC_GPIO_OUT;
				gpio_config.in_out[1]=DC_GPIO_OUT;
				gpio_config.in_out[2]=DC_GPIO_OUT;
				gpio_config.in_out[3]=DC_GPIO_OUT;
				gpio_config.in_out[4]=DC_GPIO_IN;
				gpio_config.in_out[5]=DC_GPIO_IN;
				gpio_config.in_out[6]=DC_GPIO_OUT;
				gpio_config.in_out[7]=DC_GPIO_OUT;
				break;

			case MACS_BOARD:
				gpio_config.in_out[0]=DC_GPIO_OUT;
				gpio_config.in_out[1]=DC_GPIO_IN;
				gpio_config.in_out[2]=DC_GPIO_IN;
				gpio_config.in_out[3]=DC_GPIO_OUT;
				gpio_config.in_out[4]=DC_GPIO_IN;
				gpio_config.in_out[5]=DC_GPIO_IN;
				gpio_config.in_out[6]=DC_GPIO_IN;
				gpio_config.in_out[7]=DC_GPIO_IN;
			break;

			case MACS_2R_1T_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;
				gpio_config.in_out[1]=DC_GPIO_IN;
				gpio_config.in_out[2]=DC_GPIO_IN;
				gpio_config.in_out[3]=DC_GPIO_IN;
				gpio_config.in_out[4]=DC_GPIO_IN;
				gpio_config.in_out[5]=DC_GPIO_IN;
				gpio_config.in_out[6]=DC_GPIO_OUT;//RESET TEL CODEC
				gpio_config.in_out[7]=DC_GPIO_OUT;//RESET RADIO CODEC
			break;

			case RACB2_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;//PTT INT
				gpio_config.in_out[1]=DC_GPIO_IN;//PTT EXT
				gpio_config.in_out[2]=DC_GPIO_OUT;//NOT USE
				gpio_config.in_out[3]=DC_GPIO_OUT;//HSAMPL_STBY
				gpio_config.in_out[4]=DC_GPIO_OUT;//MIC_BAIS_EN
				gpio_config.in_out[5]=DC_GPIO_OUT;//LED_EN
				gpio_config.in_out[6]=DC_GPIO_OUT;//CODEC_PWDN
				gpio_config.in_out[7]=DC_GPIO_OUT;//CODEC_RST
			break;
			case ACB11_BOARD:
				gpio_config.in_out[0]=DC_GPIO_OUT;//headset mono mute, active high
				gpio_config.in_out[1]=DC_GPIO_OUT;//headset mono shutdown, active low
				gpio_config.in_out[2]=DC_GPIO_OUT;//headset left mute, active high
				gpio_config.in_out[3]=DC_GPIO_OUT;//headset left shutdown, active low
				gpio_config.in_out[4]=DC_GPIO_OUT;//headset right mute, active high
				gpio_config.in_out[5]=DC_GPIO_OUT;//headset right shutdown, active low
				gpio_config.in_out[6]=DC_GPIO_OUT;//speaker shutdown, active high
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low
			break;
			case MACS_R_BOARD:				
				gpio_config.in_out[0]=DC_GPIO_OUT;//FS direction,
				gpio_config.in_out[1]=DC_GPIO_IN;//NC
				gpio_config.in_out[2]=DC_GPIO_IN;//NC
				gpio_config.in_out[3]=DC_GPIO_OUT;//CLK direction
				gpio_config.in_out[4]=DC_GPIO_IN;//SW INIT
				gpio_config.in_out[5]=DC_GPIO_IN;//NC
				gpio_config.in_out[6]=DC_GPIO_IN;//NC
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low
			break;
			case SVT_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;//spare
				gpio_config.in_out[1]=DC_GPIO_IN;//spare
		        gpio_config.in_out[2]=DC_GPIO_IN;//spare
				gpio_config.in_out[3]=DC_GPIO_OUT;//headset left shutdown, active low
				gpio_config.in_out[4]=DC_GPIO_IN;//spare
				gpio_config.in_out[5]=DC_GPIO_OUT;//headset right shutdown, active low
				gpio_config.in_out[6]=DC_GPIO_IN;//spare
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low
				
			break;

            case SVT2_BOARD:
                gpio_config.in_out[0]=DC_GPIO_IN;//spare
                gpio_config.in_out[1]=DC_GPIO_IN;//spare
                gpio_config.in_out[2]=DC_GPIO_IN;//spare
                gpio_config.in_out[3]=DC_GPIO_OUT;//headset left shutdown, active low
                gpio_config.in_out[4]=DC_GPIO_IN;//spare
                gpio_config.in_out[5]=DC_GPIO_OUT;//headset right shutdown, active low
                gpio_config.in_out[6]=DC_GPIO_IN;//spare
                gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low

            break;

			case VT_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;//n.c
				gpio_config.in_out[1]=DC_GPIO_IN;//n.c
		                    gpio_config.in_out[2]=DC_GPIO_IN;//n.c
				gpio_config.in_out[3]=DC_GPIO_OUT;//headset left shutdown, active low
				gpio_config.in_out[4]=DC_GPIO_IN;//n.c
				gpio_config.in_out[5]=DC_GPIO_OUT;//headset right shutdown, active low
				gpio_config.in_out[6]=DC_GPIO_OUT;//speaker shutdown, active high
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low
				
			break;
			case ACB14_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;//n.c
				gpio_config.in_out[1]=DC_GPIO_IN;//n.c
		        gpio_config.in_out[2]=DC_GPIO_IN;//n.c
				gpio_config.in_out[3]=DC_GPIO_IN;//n.c
				gpio_config.in_out[4]=DC_GPIO_IN;//n.c
				gpio_config.in_out[5]=DC_GPIO_IN;//n.c
				gpio_config.in_out[6]=DC_GPIO_OUT;//speaker shutdown, active high
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low

			break;
			case INFRA_BOARD:
				gpio_config.in_out[0]=DC_GPIO_IN;//n.c
				gpio_config.in_out[1]=DC_GPIO_IN;//n.c
		                    gpio_config.in_out[2]=DC_GPIO_IN;//n.c
				gpio_config.in_out[3]=DC_GPIO_IN;//n.c
				gpio_config.in_out[4]=DC_GPIO_IN;//UHF_RSSI_IRQ_N, active low
				gpio_config.in_out[5]=DC_GPIO_IN;//AUX_UHF_RSSI_IRQ_N, active low
				gpio_config.in_out[6]=DC_GPIO_IN;//UHF receiver CD
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low
				
			break;


			case RCGW_BOARD:
					
				gpio_config.in_out[0]=DC_GPIO_IN;//NC
				gpio_config.in_out[1]=DC_GPIO_IN;//NC
				gpio_config.in_out[2]=DC_GPIO_IN;//NC
				gpio_config.in_out[3]=DC_GPIO_IN;//NC
				gpio_config.in_out[4]=DC_GPIO_IN;//SW INIT
				gpio_config.in_out[5]=DC_GPIO_IN;//NC
				gpio_config.in_out[6]=DC_GPIO_IN;//NC
				gpio_config.in_out[7]=DC_GPIO_OUT;//codec reset, active low

			break;
		}

	

		printf("---------------------------------------------------------------------\n");
	        printf ("DSP number %d\n",dsp_num);

		printf("gpio_config.in_out=");
		for(i=0;i<HD_GPIO_NUM;i++)
		{
			printf("%d",gpio_config.in_out[(HD_GPIO_NUM-1)-i]);
		}
		printf("\n");	

		rv=hpi_write_HD_gpio_configuration ( &gpio_config,dsp_num );
		if(rv != DSPLIB_ERR_OK) 
		{
			PRINT_ERR_START;
			printf("fail to set DSP gpio_configuration!!!\n");
			PRINT_ERR_STOP;
		}
		else
		{
			PRINT_OK_START;
			printf("gpio_configuration succeed!!!\n");
			PRINT_OK_STOP;
	 	}
		printf("---------------------------------------------------------------------\n");
	}
    	return rv;
}


/**************************************************************************/                  
//  Module		: set_gpio_set_state
//  Description : set i/o state
///		    		
/// @return none
/// @param	board_name
/**************************************************************************/
int set_gpio_set_state(int *board_name)
{
	int rv = DSPLIB_ERR_OK,state=0,dsp_num=0;
	HD_gpio_status	gpio_status;
	while(state<10)
	{
		memset(&gpio_status,0,sizeof(gpio_status));


		switch (board_name[0])
		{
			//fire
			case ACB3_BOARD:
			//ots
			case ACB4_BOARD:
				//reset codec
				if (state==0)
				{
					gpio_status.data_out=0xc0;
					state=1;
					break;
				}
				if (state==1)
				{
					gpio_status.data_out=0x40;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);					
					gpio_status.data_out=0xc0;
					state=10;
					break;
				}

			break;

			case MVT_BOARD:
			case ACB12_BOARD:
				//reset codec, and disable codec mute and shutdown
				if (state==0)
				{
					//make sure amplifier are off and mute
					gpio_status.data_out=0xc5;
					state=1;
					break;
				}
				if (state==1)
				{
					//reset codecs
					gpio_status.data_out=0x45;
					state=2;
					break;
				}
/*
				if (state==2)
				{
					//stop codec reset
					sleep(1);					
					gpio_status.data_out=0xc5;
					state=3;
					break;
				}
*/

				if (state==2)
				{
					//stop reset and enable headset amplifier
					sleep(1);					
					gpio_status.data_out=0xc2;
					state=3;
					break;
				}
				if (state==3)
				{
					//enable speaker amplifier
					sleep(1);					
					gpio_status.data_out=0xca;
					state=10;
					break;
				}

			break;


			case MACS_BOARD:
				state=10;
			break;
	
			case MACS_2R_1T_BOARD:
				//reset codec
				if (state==0)
				{
					gpio_status.data_out=0xc0;
					state=1;
					break;
				}
				if (state==1)
				{
					gpio_status.data_out=0x00;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);					
					gpio_status.data_out=0xc0;
					state=10;
					break;
				}

			break;
			case RACB2_BOARD:
				//reset codec
				if (state==0)
				{
					gpio_status.data_out=0xc0;
					state=1;
					break;
				}
				if (state==1)
				{
					gpio_status.data_out=0x40;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);					
					gpio_status.data_out=0xc0;
					state=10;
					break;
				}

			break;
			case ACB11_BOARD:
				//reset codec
				if (state==0)
				{
					//codec running, all audio amplifier mute and shtdown
					gpio_status.data_out=0xd5;
					state=1;
					break;
				}
				if (state==1)
				{
					//reset codec, all audio amplifier mute and shtdown

					gpio_status.data_out=0x55;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);
					//codec running, all audio amplifier active					
					gpio_status.data_out=0xaa;
					state=10;
					break;
				}
                        break;

			case MACS_R_BOARD:
				//reset codec
				if (state==0)
				{
					dsp_num=0;
					gpio_status.data_out=0x80;
					state=1;
					break;
				}
				if (state==1)
				{
					gpio_status.data_out=0x00;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);					
					gpio_status.data_out=0x80;
					state=3;
					break;
				}
				//reset codec
				if (state==3)
				{
					dsp_num=1;
					gpio_status.data_out=0x80;
					state=4;
					break;
				}
				if (state==4)
				{
					gpio_status.data_out=0x00;
					state=5;
					break;
				}
				if (state==5)
				{
					sleep(1);					
					gpio_status.data_out=0x80;
					state=10;
					break;
				}

			break;

			case SVT2_BOARD:
			case SVT_BOARD:
			case VT_BOARD:
			case ACB14_BOARD:
            case INFRA_BOARD:
				//reset codec
				if (state==0)
				{
					//codec running, all audio amplifier mute and shtdown
					gpio_status.data_out=0xc0;
					state=1;
					break;
				}
				if (state==1)
				{
					//reset codec, all audio amplifier mute and shtdown

					gpio_status.data_out=0x40;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);
					//codec running, all audio amplifier active					
					gpio_status.data_out=0xa9;
					state=10;
					break;
				}
                        break;


           case RCGW_BOARD:
				//reset codec
				if (state==0)
				{					
					gpio_status.data_out=0x80;
					state=1;
					break;
				}
				if (state==1)
				{
					gpio_status.data_out=0x00;
					state=2;
					break;
				}
				if (state==2)
				{
					sleep(1);					
					gpio_status.data_out=0x80;
					state=10;
					break;
				}
            break;


		}

		printf("---------------------------------------------------------------------\n"			);
                printf ("DSP number %d\n",dsp_num);
		printf("gpio_status.data_out=0x%x\n",gpio_status.data_out);

		rv=dsp_write_gpio( gpio_status.data_out,dsp_num );
		if(rv != DSPLIB_ERR_OK) 
		{
			PRINT_ERR_START;
			printf("fail to set DSP gpio_configuration!!!\n");
			PRINT_ERR_STOP;
		}
		else
		{
			PRINT_OK_START;
			printf("gpio_configuration succeed!!!\n");
			PRINT_OK_STOP;
		}
		printf("---------------------------------------------------------------------\n"			);
	}
    	return rv;
}

/**************************************************************************/                  
//  Module		: set_vocoder_init
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_vocoder_init(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,start_ch=20,dsp_num=0;
	HD_vocoder_initialization	vocoder_init;
	

	memset(&vocoder_init,0,sizeof(vocoder_init));

	switch (board_name[0])
	{


		//macs board
		case MACS_BOARD:
		case MACS_2R_1T_BOARD:
			if (board_name[0]==MACS_2R_1T_BOARD)
			{
				for(i=start_ch;i<(start_ch+3);i++)
				{
				vocoder_init.channels_mask[i][1]=1;
				}

				if (board_name[1]==VOCODER_G711)
					vocoder_init.type=HD_VOCODER_G711;
				else if (board_name[1]==VOCODER_G729)
					vocoder_init.type=HD_VOCODER_G729;
			        else 
			            vocoder_init.type=HD_VOCODER_PCM;

			}
			else
			{
				for(i=start_ch;i<(start_ch+8);i++)
				{
					vocoder_init.channels_mask[i][1]=1;
				}

				if (board_name[3]==VOCODER_G711)
					vocoder_init.type=HD_VOCODER_G711;
				else if (board_name[3]==VOCODER_G729)
					vocoder_init.type=HD_VOCODER_G729;
			        else 
			            vocoder_init.type=HD_VOCODER_PCM;
			}
			
			break;			

		break;

		case SVT2_BOARD:
        case SVT_BOARD:
		case VT_BOARD:
		case RCGW_BOARD:
		case ACB14_BOARD:
		case ACB12_BOARD:
			for(i=start_ch;i<(start_ch+3);i++)
					{
					vocoder_init.channels_mask[i][HD_DSP_TO_HOST]=1;
					vocoder_init.channels_mask[i][HD_HOST_TO_DSP]=1;
					}
			if (board_name[1]==VOCODER_G711)
				vocoder_init.type=HD_VOCODER_G711;
			else if (board_name[1]==VOCODER_G729)
				vocoder_init.type=HD_VOCODER_G729;
			else 
			    vocoder_init.type=HD_VOCODER_PCM;
                            
        break;

		default:
                        return 0;
                    break;

	}
	printf("---------------------------------------------------------------------\n");

	printf("vocoder_init.channel_mask in=");
	for(i=0;i<HD_MAX_CHANNELS;i++)
	{
	printf("%d",vocoder_init.channels_mask[(HD_MAX_CHANNELS-1)-i][1]);

	}
	printf("\n");
	printf("vocoder_init.channel_mask out=");
	for(i=0;i<HD_MAX_CHANNELS;i++)
	{
	printf("%d",vocoder_init.channels_mask[(HD_MAX_CHANNELS-1)-i][0]);

	}
	printf("\n");

	printf("vocoder_init.type=%d\n",vocoder_init.type);

	rv=hpi_write_HD_vocoder_initialization ( &vocoder_init,dsp_num );
	if(rv != DSPLIB_ERR_OK) 
	{
		PRINT_ERR_START;
		printf("fail to set DSP vocoder_initialization\n");
		PRINT_ERR_STOP;
	}
	else
	{
		PRINT_OK_START;
		printf("vocoder_initialization succeed\n");
		PRINT_OK_STOP;
	}
	printf("---------------------------------------------------------------------\n");
	
    	return rv;
}


/**************************************************************************/                  
//  Module		: set_vocoder_enable
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_vocoder_enable(int *board_name)
{
	int rv = DSPLIB_ERR_OK,i,start_ch=20,ch_num=8,dsp_num=0;
	HD_vocoder_enabling vocoder_enabling;
	

	memset(&vocoder_enabling,0,sizeof(vocoder_enabling));

	switch (board_name[0])
	{

		//macs board
		case MACS_BOARD:
		case MACS_2R_1T_BOARD:	
			if(board_name[0]==MACS_2R_1T_BOARD)
				ch_num=3;
			for(i=start_ch;i<(start_ch+ch_num);i++)
			{
				vocoder_enabling.channels_mask[i][1]=1;
			}

			


						

		break;

		case SVT2_BOARD:
		case SVT_BOARD:
		case VT_BOARD:
		case RCGW_BOARD:
		case ACB14_BOARD:
		case ACB12_BOARD:
			for(i=start_ch;i<(start_ch+3);i++)
			{
				vocoder_enabling.channels_mask[i][HD_DSP_TO_HOST]=1;
                                    vocoder_enabling.channels_mask[i][HD_HOST_TO_DSP]=1;
			}
                            
                        
        break;

		default:
                        return 0;
                    break;
		

	}

	printf("---------------------------------------------------------------------\n"
		);

	printf("vocoder_enabling.channel_mask in=");
	for(i=0;i<HD_MAX_CHANNELS;i++)
	{
	printf("%d",vocoder_enabling.channels_mask[(HD_MAX_CHANNELS-1)-i][1]);
	}
	printf("\n");

	printf("vocoder_enabling.channel_mask out=");
	for(i=0;i<HD_MAX_CHANNELS;i++)
	{
	printf("%d",vocoder_enabling.channels_mask[(HD_MAX_CHANNELS-1)-i][0]);
	}
	printf("\n");

	rv=hpi_write_HD_vocoder_enabling ( &vocoder_enabling,dsp_num );
	if(rv != DSPLIB_ERR_OK) 
	{
		PRINT_ERR_START;
		printf("fail to set DSP vocoder_enabling\n");
		PRINT_ERR_STOP;
	}
	else
	{
		PRINT_OK_START;
		printf("vocoder_enabling succeed\n");
		PRINT_OK_STOP;
	}
	printf("---------------------------------------------------------------------\n"			);


    	return rv;
}



