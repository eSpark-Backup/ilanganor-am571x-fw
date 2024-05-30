/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : caller id using MESI module
///	@file:	audio_mux.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	27/mars/2011	  Created	
//
/******************************************************************************/

/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/
//#define CID_TX_ENABLE
#define ENABLE_LOG_PRINT 1
#define STDIO	1

#include <std.h>
#include <stdio.h>
#include <stdint.h>
#include <c6x.h>
#include <csl_gpio.h>
#include <csl.h>
#include <log.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//#include "mesi_modules.h"
#include "caller_id.h"
//#include "BoardUtility\BoardUtility.h"
#include "host_ctrl.h"
#include "host_frame.h"
//#include "common.h"		/* MESi channel memory support */
//#include "rxtx.h"     /* MESi channel memory support */
#include "fsk.h"
#include "memory.h"     /* MESi channel memory support */
#include "target.h"		/* support for specific target hardware and tools */
#include "main.h"
#include "cid.h"


extern far LOG_Obj LOG_trace;
extern GPIO_Handle hGpio;


extern short Tx_log[],Rx_log[],Tx_log_counter,Rx_log_counter;

unsigned short old_Tx_ID=0, old_Rx_ID=0;

unsigned long ci_frame_counter=0,frame_limit;



     


CI_struct caller_id;
CI_struct caller_id_tmp[5];





extern HD_tel_ext	*tel;

extern mesi_ctrl_t MESI_ctrl;
				
/**************************************************************************/                     
//  Module		: caller_id_init
//  Description :  init caller id module
/// @return none
/// @param	none
/**************************************************************************/
int caller_id_init(int channel,uint16_t a_type,uint16_t a_detection_mode,uint16_t associated_ch)
{
    int ret;
    struct START_PTRS *ptrs;
    
    //init receive buffer 
	ret=mesi_init(channel, MESI_CALLER_ID_DET,associated_ch);	
    if (ret)
		return -1;

	//init receiver to detect caller id
	ptrs=(struct START_PTRS *)&start_ptrs[channel];	
	Rx_init_CID1(ptrs);			
		
	caller_id.type[channel]=a_type;
	caller_id.detection_mode[channel]=a_detection_mode;
	caller_id.channel[channel]=associated_ch;
	//caller_id.config[channel]=1;

	return 0;
}



/**************************************************************************/                     
//  Module		: caller_id_process
//  Description :  
//
/// @return none
/// @param	none
/**************************************************************************/
//#pragma CODE_SECTION(audio_mux,".ISRAM_CODE")

void caller_id_process(int channel,int16_t *in)
{
	int i,p,ret;
	struct START_PTRS *ptrs;
	struct TX_CID_BLOCK *Tx;
	struct RX_CID_BLOCK *Rx;
	char ci_text[20];

	//check if channel was configured
//	if (caller_id.config[channel]!=1)
//		return;
	if (MESI_ctrl.mode[channel]!=MESI_CALLER_ID_DET)
		return;		
	//check if process need to be done this time				
	if (caller_id.detection_mode[channel]==HD_CALLER_ID_OFF)
		return;	
	//check this only if channel is codec channel - status received from telephon driver	
	if (channel<HD_MAX_CODEC_CHANNELS)
	{	
		//check there was 1 ring in this channel 
		if ((caller_id.detection_mode[channel]==HD_CALLER_ID_AFTER_FIRST_RING)&&
			((tel!=0)&&(tel->status[channel].general_status&(~HD_TEL_AFTER_1RING_CID)==0)))
			return;	
		if ((caller_id.detection_mode[channel]==HD_CALLER_ID_AFTER_OFF_HOOK)&&
			((tel!=0)&&(tel->status[channel].general_status&(~HD_TEL_AFTER_1RING_CID)==0)))
			return;	
	}		
	ptrs=(struct START_PTRS *)&start_ptrs[channel];
	Tx=(struct TX_CID_BLOCK *)ptrs->Tx_block_start;
	Rx=(struct RX_CID_BLOCK *)ptrs->Rx_block_start;
			
	for (i=0;i<NSAMPLES;i++)
	{	

		//insert data to buffer	
		*Rx->sample_head=*(in+i);
		//*Rx->sample_head=*Tx->sample_tail;		
		//inc header
		if (++Rx->sample_head>=ptrs->Rx_sample_start+Rx->sample_len)
			Rx->sample_head=ptrs->Rx_sample_start;
#ifdef CID_TX_ENABLE
		if (++Tx->sample_tail>=ptrs->Tx_sample_start+Tx->sample_len)
			Tx->sample_tail=ptrs->Tx_sample_start;
#endif			
			ret=receiver(ptrs);
//			if (ret!=0)
//				LOG_printf(&LOG_trace,"start decode\n");
#ifdef CID_TX_ENABLE
			if (transmitter(ptrs) != 0)
				if (i == 0)				/* if channel 0 ... */
					ci_frame_counter++;	/* ...increment the frame */
#endif													
			if (Rx->status == CID_DETECTED)
				{

				printf_CID(ptrs->Rx_fir_start,channel);

				Rx->status=STATUS_OK;
				frame_limit=0;
				restart_caller_id(channel);
				//Rx_block_init(ptrs);
				//Rx_init_CID(ptrs);
				return;
				}

			/**** look for bad checksum ****/

			if (Rx->status == CHECKSUM_FAILURE)
			{

				ret=sprintf(ci_text,"cid[%d]:Bad checksum \n ",channel);
                print_debug(ci_text,HC_DBG_DATA8,ret,HC_DBG_START);
				Rx->status=STATUS_OK;
				frame_limit=0;
				restart_caller_id(channel);
				//Rx_block_init(ptrs);
				//Rx_init_CID(ptrs);
				return;
				}
			
			p=0;
			//ptrs=&start_ptrs[0];
			Tx=(struct TX_CID_BLOCK *)ptrs->Tx_block_start;
			Rx=(struct RX_CID_BLOCK *)ptrs->Rx_block_start;
#ifdef CID_TX_ENABLE
			if (Tx->state_ID!=old_Tx_ID)
				{
				if (Tx_log_counter<STATE_LOG_LEN)
					{
					p++;
					Tx_log[Tx_log_counter++]=Tx->state_ID;
					Tx_log[Tx_log_counter++]=(short)ci_frame_counter;
					}
				}
			old_Tx_ID=Tx->state_ID;
#endif
			/**** log Rx state transitions ****/

			if (Rx->state_ID!=old_Rx_ID)
			{
				if (Rx_log_counter<STATE_LOG_LEN)
					{
					p++;
					//Rx_log[Rx_log_counter++]=Rx->state_ID;
					//Rx_log[Rx_log_counter++]=(short)ci_frame_counter;
					}
			}
			old_Rx_ID=Rx->state_ID;	
			if (p != 0)
			{
		        ret=sprintf(ci_text,"cid[%d] state=%x ",channel,Rx->state_ID);
                print_debug(ci_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);				
			}			
	}										
}


/**************************************************************************/                     
//  Module		: printf_CID
//  Description :  print caller id info
/// @return none
/// @param	none
/**************************************************************************/
void printf_CID(short X[], short channel)
{

	short i,k,PM,*ptr;
    short presentation_len;
	short parameter_type;
	short parameter_len;
	short invalid_parameter=0;
	char ci_text[50];
	char tmp_chr[3];
	HD_events_caller_id caller_id_event;
	
	int cid_index=0,ret;
	ci_frame_counter++;
	memset(&caller_id_event,0,sizeof(caller_id_event));

	presentation_len=X[1];
	ptr=&X[2];
	while (ptr < &X[presentation_len+1])
		{
		parameter_type=*ptr++;
		parameter_len=*ptr++;
		
		switch (parameter_type)
			{
			case 0x01: 	/* Date and Time*/
				

                tmp_chr[0]=(char)*ptr++;
                tmp_chr[1]=(char)*ptr++;
                tmp_chr[2]=0;
                caller_id_event.month=atoi(tmp_chr);

                tmp_chr[0]=(char)*ptr++;
                tmp_chr[1]=(char)*ptr++;
                tmp_chr[2]=0;
                caller_id_event.day=atoi(tmp_chr);				
				tmp_chr[0]=(char)*ptr++;
                tmp_chr[1]=(char)*ptr++;
                tmp_chr[2]=0;
                caller_id_event.hour=atoi(tmp_chr);
				tmp_chr[0]=(char)*ptr++;
                tmp_chr[1]=(char)*ptr++;
                tmp_chr[2]=0;
                caller_id_event.minute=atoi(tmp_chr);
				break;
			case 0x02: 	/* Calling Line Identity */
                if (parameter_len<=20)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.calling_id[i]=(char)(*ptr++);
                }
			break;
			case 0x03: 	/* Called Line Identity */
               if (parameter_len<=20)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.called_id[i]=(char)(*ptr++);
                }                
			break;
			case 0x04: 	/* Reason for Absence of Called Line Identity */
				i=*ptr++;
				switch (i)
					{
					case 0x4f:
						caller_id_event.absense_calling_id=HD_CALLER_ID_UNAVILABLE;
					break;
					case 0x50:
						caller_id_event.absense_calling_id=HD_CALLER_ID_PRIVETE;
					break;
					}
				break;
			case 0x05: 	/* undefined */
				for (i=0;i<parameter_len;i++)
					ptr++;
				break;
			case 0x06: 	/* undefined */
				for (i=0;i<parameter_len;i++)
					ptr++;
				break;
			case 0x07: 	/* Calling Party Name */
                if (parameter_len<=50)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.calling_name[i]=(char)(*ptr++);
                }
			break;
			case 0x08: 	/* Reason for Absence of Calling Party Name */
				i=*ptr++;
				switch (i)
					{
					case 0x4f:
						caller_id_event.absense_calling_name=HD_CALLER_ID_UNAVILABLE;
					break;
					case 0x50:
						caller_id_event.absense_calling_name=HD_CALLER_ID_PRIVETE;
					break;
					}
				break;
			case 0x11: 	/* Call Type */
				i=*ptr++;
				switch (i)
					{
					case 0x01:
						caller_id_event.call_type=HD_CALLER_ID_VOICE_CALL;
					break;
					case 0x02:
						caller_id_event.call_type=HD_CALLER_ID_CLI_RING_BACK;
					break;
					case 0x03:
						caller_id_event.call_type=HD_CALLER_ID_CALLING_NAME_DELIVER;
					break;
					case 0x81:
						caller_id_event.call_type=HD_CALLER_ID_MESSAGE_CALL;
					break;
					}
				break;
			default:
				invalid_parameter++;
				break;
			} /* switch (parameter_type) */

		}

	if (invalid_parameter != 0)
	{
		caller_id_event.error=X[0];
		caller_id_event.error_count=invalid_parameter;    
	}
	//Send caller id event and put it in the frame memory	
	send_event_frame(&caller_id_event,sizeof(HD_events_caller_id),caller_id.channel[channel],
		HD_CALLER_ID_EVENT,0);
	
//	print_debug(&caller_id_event,HC_DBG_DATA32,sizeof(HD_events_caller_id),0);	
	sprintf(ci_text,"cid[%d]:month=%d,day=%d,hour=%d,min=%d ",channel,caller_id_event.month,
		caller_id_event.day,caller_id_event.hour,caller_id_event.minute);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_START);		
	sprintf(ci_text,"calling id=%s ",caller_id_event.calling_id);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);
	sprintf(ci_text,"called id=%s ",caller_id_event.called_id);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);	
	sprintf(ci_text,"absense_calling_id=%d ",caller_id_event.absense_calling_id);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);
	sprintf(ci_text,"calling name=%s ",caller_id_event.calling_name);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);	
	sprintf(ci_text,"absense_calling_name=%d ",caller_id_event.absense_calling_name);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);
	sprintf(ci_text,"call_type=%d ",caller_id_event.call_type);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);
	sprintf(ci_text,"error=%d,error_count=%d ",caller_id_event.error,caller_id_event.error_count);
	print_debug(ci_text,HC_DBG_DATA8,strlen(ci_text),HC_DBG_ROLLOVER);						
	return;
}

#if 0
/**************************************************************************/                     
//  Module		: printf_CID
//  Description :  print caller id info
/// @return none
/// @param	none
/**************************************************************************/
void printf_CID(short X[], short chan_num)
{
//#if defined(STDIO)
	short i,k,PM,*ptr;
    short presentation_len;
	short parameter_type;
	short parameter_len;
	short invalid_parameter=0;
	char *text_ptr;
	char tmp_chr[2];
	HD_events_caller_id caller_id_event;
	//char text2[128];
	int cid_index=0,ret;
	
	memset(caller_id_event,0,sizeof(caller_id_event));
#ifdef ENABLE_LOG_PRINT	
	LOG_printf(&LOG_trace,"[%02d] CID checksum OK, burst completed\n\n",chan_num);
#endif	
	//ret=sprintf(text2,"[%02d] CID checksum OK, burst completed\n\n");
    //print_debug(text2,HC_DBG_DATA8,ret,0);	
	presentation_len=X[1];
	ptr=&X[2];
	while (ptr < &X[presentation_len+1])
		{
		parameter_type=*ptr++;
		parameter_len=*ptr++;
		text_ptr=text;
		switch (parameter_type)
			{
			case 0x01: 	/* Date and Time*/
				
				
				#ifdef ENABLE_LOG_PRINT
                LOG_printf(&LOG_trace,"[%02d] Date and Time:           ",chan_num);
                #endif
                ret=sprintf(text2,"[%02d] Date and Time:           ",chan_num);
                print_debug(text2,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);//HC_DBG_START);
                tmp_chr[0]=(char)*ptr;
                tmp_chr[1]=(char)*(ptr+1);
                tmp_chr[2]=0;
                caller_id_event.month=atoi(tmp_chr);
				*text_ptr++=(char)*ptr++;
				*text_ptr++=(char)*ptr++;
				*text_ptr++='/';
                tmp_chr[0]=(char)*ptr;
                tmp_chr[1]=(char)*(ptr+1);
                tmp_chr[2]=0;
                caller_id_event.day=atoi(tmp_chr);				
				*text_ptr++=(char)*ptr++;
				*text_ptr++=(char)*ptr++;
				*text_ptr++=' ';
				tmp_chr[0]=(char)*ptr;
                tmp_chr[1]=(char)*(ptr+1);
                tmp_chr[2]=0;
                caller_id_event.hour=atoi(tmp_chr);
				PM=0;
				k=(*ptr++ - 0x30)*10;
				k+=(*ptr++ - 0x30);
				if (k>12)
					{
					k-=12;
					PM=1;
					}
				*text_ptr++=(char)(k/10+0x30);
				*text_ptr++=(char)((k%10)+0x30);
				*text_ptr++=':';
				tmp_chr[0]=(char)*ptr;
                tmp_chr[1]=(char)*(ptr+1);
                tmp_chr[2]=0;
                caller_id_event.minute=atoi(tmp_chr);
				*text_ptr++=(char)*ptr++;
				*text_ptr++=(char)*ptr++;
				*text_ptr++=' ';
				if (PM==0)
					*text_ptr++='A';
				else
					*text_ptr++='P';
				*text_ptr++='M';
				break;
			case 0x02: 	/* Calling Line Identity */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Calling Line Identity:   ",chan_num);
				#endif
                //ret=sprintf(text2,"[%02d] Calling Line Identity:   ",chan_num);
                //print_debug(text2,HC_DBG_DATA8,ret,0);
                if (parameter_len<=20)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.calling_id[i]=(char)(*(ptr+i));
                }		
				if (parameter_len == 10)
					{
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++='-';
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++='-';
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					}
				else
					{
					for (i=0;i<parameter_len;i++)
						*text_ptr++=(char)*ptr++;
					}
				break;
			case 0x03: 	/* Called Line Identity */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Called Line Identity:   ",chan_num);
				#endif
                //ret=sprintf(text2,"[%02d] Calling Line Identity:   ",chan_num);
                //print_debug(text2,HC_DBG_DATA8,ret,0);	
                if (parameter_len<=20)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.called_id[i]=(char)(*(ptr+i));
                }                
				if (parameter_len == 10)
					{
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++='-';
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++='-';
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					*text_ptr++=(char)*ptr++;
					}
				else
					{
					for (i=0;i<parameter_len;i++)
						*text_ptr++=(char)*ptr++;
					}
				break;
			case 0x04: 	/* Reason for Absence of Called Line Identity */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Reason for Absence of Called Line Identity:   ",chan_num);
				#endif
                //ret=sprintf(text2,"[%02d] Reason for Absence of Called Line Identity:   ",chan_num);
                //print_debug(text2,HC_DBG_DATA8,ret,0);					
				i=*ptr++;
				switch (i)
					{
					case 0x4f:
						#ifdef ENABLE_LOG_PRINT
						LOG_printf(&LOG_trace,"Unavailable");						
						#endif
						caller_id_event.absense_calling_id=HD_CALLER_ID_UNAVILABLE;
						//ret=sprintf(text2,"Unavailable");
                		//print_debug(text2,HC_DBG_DATA8,ret,0);						
						break;
					case 0x50:
						#ifdef ENABLE_LOG_PRINT
						LOG_printf(&LOG_trace,"Private");						
						#endif
						caller_id_event.absense_calling_id=HD_CALLER_ID_PRIVETE;
						//ret=sprintf(text2,"Private");
                		//print_debug(text2,HC_DBG_DATA8,ret,0);
						break;
					}
				break;
			case 0x05: 	/* undefined */
				for (i=0;i<parameter_len;i++)
					ptr++;
				break;
			case 0x06: 	/* undefined */
				for (i=0;i<parameter_len;i++)
					ptr++;
				break;
			case 0x07: 	/* Calling Party Name */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Calling Party Name:      ",chan_num);
				#endif
                //ret=sprintf(text2,"[%02d] Calling Party Name:      ",chan_num);
                //print_debug(text2,HC_DBG_DATA8,ret,0);	
                if (parameter_len<=50)
                {
                	for(i=0;i<parameter_len;i++)
                		caller_id_event.calling_name[i]=(char)(*(ptr+i));
                }                				
				for (i=0;i<parameter_len;i++)
					*text_ptr++=(char)*ptr++;
				break;
			case 0x08: 	/* Reason for Absence of Calling Party Name */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Calling Party Name:      ",chan_num);
				#endif
                //ret=sprintf(text2,"[%02d] Calling Party Name:      ",chan_num);
                //print_debug(text2,HC_DBG_DATA8,ret,0);				
				i=*ptr++;
				switch (i)
					{
					case 0x4f:
						#ifdef ENABLE_LOG_PRINT
						LOG_printf(&LOG_trace,"Unavailable");
						#endif
						caller_id_event.absense_calling_name=HD_CALLER_ID_UNAVILABLE;
						//ret=sprintf(text2,"Unavailable");
                		//print_debug(text2,HC_DBG_DATA8,ret,0);
						break;
					case 0x50:
						#ifdef ENABLE_LOG_PRINT
						LOG_printf(&LOG_trace,"Private");
						#endif
						caller_id_event.absense_calling_name=HD_CALLER_ID_PRIVETE;
						//ret=sprintf(text2,"Private");
                		//print_debug(text2,HC_DBG_DATA8,ret,0);
						break;
					}
				break;
			case 0x11: 	/* Call Type */
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] Call Type:   ",chan_num);
				#endif
				i=*ptr++;
				#ifdef ENABLE_LOG_PRINT
				switch (i)
					{
					case 0x01:
						
						LOG_printf(&LOG_trace,"Voice Call");
						caller_id_event.call_type=HD_CALLER_ID_VOICE_CALL;
						break;
					case 0x02:
						LOG_printf(&LOG_trace,"CLI Ring Back when free call");
						caller_id_event.call_type=HD_CALLER_ID_CLI_RING_BACK;
						break;
					case 0x03:
						LOG_printf(&LOG_trace,"Calling Name Delivery");
						caller_id_event.call_type=HD_CALLER_ID_CALLING_NAME_DELIVER;
						break;
					case 0x81:
						LOG_printf(&LOG_trace,"Message Waiting Call");
						caller_id_event.call_type=HD_CALLER_ID_MESSAGE_CALL;
						break;
					}
				break;
				#endif
			default:
				invalid_parameter++;
				#ifdef ENABLE_LOG_PRINT
				LOG_printf(&LOG_trace,"[%02d] unknown/invalid parameter:0x%02x",chan_num,parameter_type);
				#endif
				break;
			} /* switch (parameter_type) */
		*text_ptr='\0';
		/*
		if ((cid_index+parameter_len+1)<1000)		
		{
			cid_test_info[cid_index]=0x30+parameter_type;
			cid_index++;
			for (i=0;i<parameter_len;i++)
				//LOG_printf(&LOG_trace,"%c",text[i]);
				cid_test_info[cid_index+i]=text[i];
			cid_index+=parameter_len;	
		}
		*/
		#ifdef ENABLE_LOG_PRINT	
		for (i=0;i<strlen(text);i++)
			LOG_printf(&LOG_trace,"%c",text[i]);
		#endif	
		print_debug(text,HC_DBG_DATA8,strlen(text),0);
		//LOG_printf(&LOG_trace,"%s\n",text);	
		}
	//LOG_printf(&LOG_trace,"\n");

	if (invalid_parameter != 0)
	{
		#ifdef ENABLE_LOG_PRINT
		if (X[0]==0x80)
			LOG_printf(&LOG_trace,"Type: Call Setup, %d invalid parameter(s)\n",invalid_parameter);
			//ret=sprintf(text2,"Type: Call Setup, %d invalid parameter(s)\n");
            //print_debug(text2,HC_DBG_DATA8,ret,0);
		if (X[0]==0x82)
			LOG_printf(&LOG_trace,"Type: Visual Message Waiting Indicator\n");
			//ret=sprintf(text2,"Type: Visual Message Waiting Indicator\n");
            //print_debug(text2,HC_DBG_DATA8,ret,0);
		for (i=0;i<presentation_len+3;i++)
			LOG_printf(&LOG_trace,"Rx_fir[%2d]: %2x\n",i,X[i]);
			//ret=sprintf(text2,"Rx_fir[%2d]: %2x\n",i,X[i]);
            //print_debug(text2,HC_DBG_DATA8,ret,0);
        #endif    
	}
	//Send caller id event and put it in the frame memory	
	send_event_frame(caller_id_event,sizeof(HD_events_caller_id),chan_num,
		HD_CALLER_ID_EVENT,0);
	print_debug(&caller_id_event,HC_DBG_DATA32,sizeof(HD_events_caller_id),0);	
	strncpy(text,"hey",3);
	print_debug(text,HC_DBG_DATA8,strlen(text),0);	
//#endif /* STDIO */
	return;
}
#endif
/**************************************************************************/                     
//  Module		: test_caller_id
//  Description :  test caller id module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void restart_caller_id(int channel)
{
	int ret;
	struct START_PTRS *ptrs;
	char ci_text[30];
	//channel=0;
	ptrs=(struct START_PTRS *)&start_ptrs[channel];
	Rx_block_init(ptrs);
	Rx_init_CID(ptrs);	
    ret=sprintf(ci_text,"cid[%d] restart",channel);
    print_debug(ci_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);												
													
}


				
//#if	 (TEST_TYPE == TEST_AUDIO_MUX)
/**************************************************************************/                     
//  Module		: test_caller_id
//  Description :  test caller id module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_caller_id(void)
{
							
													
}
//#endif


