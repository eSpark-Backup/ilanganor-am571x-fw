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
//#define DTMF_TX_ENABLE
#define ENABLE_LOG_PRINT 1
//#define STDIO	1


#include <std.h>
#include <stdio.h>
#include <stdint.h>
#include <c6x.h>
#include <csl_gpio.h>
#include <csl.h>
#include <log.h>
#include <math.h>
#include "mesi_modules.h"
#include "DigitalCore_hpi.h"
#include "BoardUtility\BoardUtility.h"
#include "host_ctrl.h"
#include "host_frame.h"
#include "common.h"		/* MESi channel memory support */
#include "rxtx.h"     /* MESi channel memory support */
#include "memory.h"     /* MESi channel memory support */
#include "target.h"		/* support for specific target hardware and tools */
#include "gendet.h"
#include "dtmf.h"
#include "dtmf_det.h"
#include "main.h"
extern far LOG_Obj LOG_trace;
extern GPIO_Handle hGpio;

extern short Tx_log[],Rx_log[],Tx_log_counter,Rx_log_counter;

unsigned short old_Tx_dtmf=0, old_Rx_dtmf=0;

unsigned long dtmf_frame_counter=0;
short errors=0,digit_counter=0;
short Tx_digit[NUM_CHANNELS];

//struct START_PTRS *ptrs;

     






//int *memo_tst1,*memo_tst2;	
//extern Int ISRAM;
//extern Int SDRAM;	
int dtmf_config=0;
DTMF_DET_struct dtmf_det;

extern mesi_ctrl_t MESI_ctrl;
			
/**************************************************************************/                     
//  Module		: dtmf_det_init
//  Description :  init dtmf detector module
/// @return none
/// @param	none
/**************************************************************************/
int dtmf_det_init(int channel,uint16_t associated_ch)
{
    int ret;
    struct START_PTRS *ptrs;
    struct TX_GEN_BLOCK *Tx;
	struct RX_DET_BLOCK *Rx;

    //init receive buffer 
	ret=mesi_init(channel,MESI_DTMF_DET,associated_ch);

	if (ret)
		return -1;
		
	ptrs=(struct START_PTRS *)&start_ptrs[channel];

    
#ifdef DTMF_TX_ENABLE
	Tx=(struct TX_GEN_BLOCK *)ptrs->Tx_block_start;

	Tx_init_DTMF(ptrs);					/* start up Tx_DTMF */
	Tx_digit[channel]=i&0xf;
	*Tx->data_head=Tx_digit[channel];			/* start with digit "0" */
	
	if (++Tx->data_head >= ptrs->Tx_data_start+Tx->data_len)
		Tx->data_head=ptrs->Tx_data_start;
	Tx->cad_period=5000;				/* set cadence period to 625 msec. */
	Tx->on_time=4000;					/* set digit ON time to 500 msec. */
#endif
	//init receiver to detect DTMF	
	set_Rx_digit_CP_mask(DTMF_MASK, ptrs);
	set_Rx_detector_mask(0, ptrs);		/* mask off all GenDet detectors */
	Rx_init_detector(ptrs);	
	
	dtmf_det.channel[channel]=associated_ch;
//	dtmf_det.config[channel]=1;
	return 0;	

}




/**************************************************************************/                     
//  Module		: dtmf_det_process
//  Description :  
//
/// @return none
/// @param	none
/**************************************************************************/
//#pragma CODE_SECTION(audio_mux,".ISRAM_CODE")

void dtmf_det_process(int channel,int16_t *in)
{

	int i,p,ret,j;
	struct START_PTRS *ptrs;
	struct TX_GEN_BLOCK *Tx;
	struct RX_DET_BLOCK *Rx;
	HD_events_dtmf_detect dtmf_det_event;
	char dtmf_text[50];

	//if (dtmf_det.config[channel]!=1)
	//	return;
	if (MESI_ctrl.mode[channel]!=MESI_DTMF_DET)
		return;	
		
	ptrs=(struct START_PTRS *)&start_ptrs[channel];
	Tx=(struct TX_GEN_BLOCK *)ptrs->Tx_block_start;
	Rx=(struct RX_DET_BLOCK *)ptrs->Rx_block_start;
			
	for (i=0;i<80;i++)
	{	

		//insert data to buffer			
		*Rx->sample_head=*(in+i);
				
		//inc header
		if (++Rx->sample_head>=ptrs->Rx_sample_start+Rx->sample_len)
			Rx->sample_head=ptrs->Rx_sample_start;
			
		ret=receiver(ptrs);


		/**** look for received digits and count errors ****/

		if (Rx->data_head != Rx->data_tail)
		{
			//if (*Rx->data_tail != Tx_digit[0])
			//	errors++;
			dtmf_frame_counter++;
			//Send caller id event and put it in the frame memory
			dtmf_det_event.dtmf=*Rx->data_tail;	
			send_event_frame(&dtmf_det_event,sizeof(HD_events_dtmf_detect),dtmf_det.channel[channel],
				HD_DTMF_DET_EVENT,0);
            ret=sprintf(dtmf_text,"dtmf[%d] Rx digit: %x \n",
				channel,*Rx->data_tail);
            print_debug(dtmf_text,HC_DBG_DATA8,ret,HC_DBG_START);
			LOG_printf(&LOG_trace,"dtmf[%d] Rx digit: %x \n",
				channel,*Rx->data_tail);		
			if (++Rx->data_tail >= ptrs->Rx_data_start+Rx->data_len)
				Rx->data_tail=ptrs->Rx_data_start;
		}


		/**** log Tx state transitions for first channel ****/
		p=0;
		//ptrs=&start_ptrs[0];
		Tx=(struct TX_GEN_BLOCK *)ptrs->Tx_block_start;
		Rx=(struct RX_DET_BLOCK *)ptrs->Rx_block_start;
#ifdef DTMF_TX_ENABLE

		if (Tx->state_ID != old_Tx_dtmf)
		{
			if (Tx_log_counter < STATE_LOG_LEN)
			{
				p++;
				Tx_log[Tx_log_counter++]=Tx->state_ID;
				Tx_log[Tx_log_counter++]=(short)dtmf_frame_counter;
			}
		}
		old_Tx_dtmf=Tx->state_ID;
#endif
		/**** log Rx state transitions for first channel  ****/

		if (Rx->state_ID != old_Rx_dtmf)
		{
			if ((Rx_log_counter < STATE_LOG_LEN) && (Rx->state_ID > 0x1100))
			{
				p++;
				Rx_log[Rx_log_counter++]=Rx->state_ID;
				Rx_log[Rx_log_counter++]=(short)dtmf_frame_counter;
			}
		}
		old_Rx_dtmf=Rx->state_ID;
																																																

		if (p != 0)
		{
			dtmf_frame_counter++;
            ret=sprintf(dtmf_text,"dtmf[%d] state=%x",channel,Rx->state_ID);
            print_debug(dtmf_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);
            //LOG_printf(&LOG_trace,"dtmf[%d] state=%x",channel,Rx->state_ID);				
		}
	}	
										
}


/**************************************************************************/                     
//  Module		: restart_dtmf_det
//  Description :   
/// @return none
/// @param	none
/**************************************************************************/
void restart_dtmf_det(int channel)
{
	int ret;
	char dtmf_text[30];
	struct START_PTRS *ptrs;
	ptrs=(struct START_PTRS *)&start_ptrs[channel];
	Rx_block_init(ptrs);
	Rx_init_detector(ptrs);	
    ret=sprintf(dtmf_text,"dtmf[%d]:dtmf restart",channel);
    print_debug(dtmf_text,HC_DBG_DATA8,ret,HC_DBG_ROLLOVER);												
													
}


				
//#if	 (TEST_TYPE == TEST_AUDIO_MUX)
/**************************************************************************/                     
//  Module		: test_caller_id
//  Description :  test caller id module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_dtmf_det(void)
{
							
													
}
//#endif
