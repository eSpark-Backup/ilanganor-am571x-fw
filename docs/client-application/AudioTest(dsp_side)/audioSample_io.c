/*
 * audioSample_io.c
 *
 * This file contains the test / demo code to demonstrate the Audio component
 * driver functionality on SYS/BIOS 6.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** \file     audioSample_io.c
 *
 *  \brief    sample application for demostration of audio playing
 *
 *  This file contains the implementation of the sample appliation for the
 *  demonstration of audio playing through the audio interface layer.
 *
 *             (C) Copyright 2009, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */


#include <xdc/std.h>
#include <ti/sysbios/io/IOM.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <mcasp_drv.h>
#include <ti/csl/csl_chip.h>

#if defined(SOC_AM65XX) || defined (SOC_J721E)
 #define MCASP_UDMA_ENABLED
#else
 #define MCASP_EDMA_ENABLED
#endif 


#if defined(MCASP_EDMA_ENABLED)
 #include <ti/sdo/edma3/drv/edma3_drv.h>
 #include <ti/sdo/edma3/rm/edma3_rm.h>
 #include <ti/sdo/edma3/drv/sample/bios6_edma3_drv_sample.h>
#elif defined(MCASP_UDMA_ENABLED)
 #include <ti/drv/udma/include/udma_types.h>
 #include <ti/drv/udma/udma.h>
#endif 

#include "mcasp_osal.h"
#include <ti/drv/mcasp/soc/mcasp_soc.h>
#include "ICodec.h"
#include "mcasp_cfg.h"
#include "MCASP_log.h"

#ifdef MCASP_ENABLE_DEBUG_LOG
#include "mcasp_debug.h"
#endif

#include "stdio.h"
#include "string.h"
#include "aic12.h"
#include "codec_if.h"
#include "Audio_evmInit.h"
#include "veusim.h"
#include "AudioCfg.h"
#include "host_ctrl.h"
#include "system_config.h"
#include "main.h"
#include "audio_mux.h"
#include "host_frame.h"

//#define SAVE_FILE
//#define TEST_FILE
//#define MEASURE_TIME
#include <ti/csl/cslr_mcasp.h>
#ifdef MEASURE_TIME
  #include "profiling.h"
#endif

#ifdef DEVICE_LOOPBACK
  #include "deviceloopback.h"
#endif

#ifdef AUDIO_EQ_DEMO
  #include "audioEQ.h"
#endif

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Cache.h>

extern short InBuff[];
int InBuffIndex=0;
#pragma DATA_SECTION(SaveInAudio,"RAM_CODE")
char SaveInAudio[100000];

extern int16_t* p_host_audio_buff_rcv;
extern int16_t* p_host_audio_buff_trn;

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */

extern const ti_sysbios_heaps_HeapMem_Handle myHeap;

#define max(x,y) ( ((x) > (y))?(x):(y) )
/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/*
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for
 * the cache work optimally on the C6x.
 */
#define BUFLEN                  (80) /* number of samples per serializer in the frame  */
#define BUFALIGN                (128) /* alignment of buffer for use of L2 cache */

/* This is the number of buffers used by the application to be issued and reclaimed 
   This number can be higher than 2 (Ping pong) also. The McASP driver puts them in 
   a queue internally and process them in order and give back to the application */
#define UDMA_FIFO_DEPTH      (2)
#define NUM_BUFS                (2) /* For ping pong */

#if defined (MCASP_UDMA_ENABLED)
  /* 
   * For UDMAP/PDMA the prime count needs to be 4
   * */
  #define PRIME_COUNT                (2)
#else 
  #define PRIME_COUNT                (2)
#endif
  
#define TOTAL_PACKETS_TO_TRANSFER  (98+2) /* Including priming */
#define NUM_APP_BUFS_QUEUE_ENTRIES  max(PRIME_COUNT,NUM_BUFS)
#define MCASP_TEST_MAX_SERIALIZERS  (TX_NUM_SERIALIZER) /* defined in mcasp_cfg.h */
#define MCASPFRAMES_RX_SIZE        (BUFLEN*sizeof(uint32_t)*MCASP_TEST_MAX_SERIALIZERS)

/* Putting it here so it can be changed through emulator */
volatile uint32_t test_num_bufs = NUM_BUFS;
volatile uint32_t test_prime_count = PRIME_COUNT;





#if defined(AIC_CODEC)
#include <Aic31.h>
Ptr  hAicDev;
Ptr  hAicChannel;
#endif

/* Function prototype */
static Void createStreams();
static Void prime();

uintptr_t rxbuf[NUM_BUFS][BUFLEN*MCASP_NUM_SLOTS*RX_NUM_SERIALIZER/2];
uintptr_t txbuf[NUM_BUFS][BUFLEN*MCASP_NUM_SLOTS*TX_NUM_SERIALIZER/2];

/* McASP Device handles */
Ptr  hMcaspDev;


/* McASP Device parameters */
Mcasp_Params mcaspParams;


/* Channel Handles */
Ptr hMcaspTxChan;
Ptr hMcaspRxChan;

volatile int RxFlag=0,TxFlag=0;
Semaphore_Handle semR,semT;
Semaphore_Params params;
int mcaspApiTest = FALSE;

typedef struct McASP_App_Buffer_s {
	Osal_Queue_Elem  link;
	intptr_t *buf;
	uint32_t index;
}McASP_App_BufferInfo_t;

/* For TX and RX */
typedef enum {
 APP_BUFFER_RX=0,
 APP_BUFFER_TX=1
}McASP_App_Buffer_Direction_e;

Osal_Queue_Elem McASP_App_Buffer_FreeList[2];
Osal_Queue_Elem McASP_App_Buffer_TransitList[2];

McASP_App_BufferInfo_t McASP_App_bufs[2][NUM_APP_BUFS_QUEUE_ENTRIES];


/* Initialize the queue of buffer info  and push them in to the free queues */
void McASP_App_BufferInfo_Init()
{
    uint32_t i,bufno;
	Osal_Queue_construct(&McASP_App_Buffer_FreeList[APP_BUFFER_RX], (void *)NULL);
	Osal_Queue_construct(&McASP_App_Buffer_TransitList[APP_BUFFER_RX], (void *)NULL);

    Osal_Queue_construct(&McASP_App_Buffer_FreeList[APP_BUFFER_TX], (void *)NULL);
    Osal_Queue_construct(&McASP_App_Buffer_TransitList[APP_BUFFER_TX], (void *)NULL);

	
    /* Put buffers in the queue */
	for(i=0;i<test_prime_count;i++) {
      /* For priming, we would need to put 'prime_count' number of buffers in to the Free List to begin with.
	   * During priming process, these will be popped and submitted to the McASP */
	    bufno= i%test_num_bufs;	/* There are only NUM_BUFS buffers allocated from memory. If more buffers are needed for priming, say 4, these 
	                                       buffers tx/rxBufs[NUM_BUFS]are to be re-used. */
	    McASP_App_bufs[APP_BUFFER_RX][i].buf =(uintptr_t*) rxbuf[bufno];
	    McASP_App_bufs[APP_BUFFER_RX][i].index = bufno;
        Osal_Queue_put((Osal_Queue_handle(&McASP_App_Buffer_FreeList[APP_BUFFER_RX])),(Osal_Queue_Elem *)&McASP_App_bufs[APP_BUFFER_RX][i]);

	    McASP_App_bufs[APP_BUFFER_TX][i].buf = (uintptr_t*)txbuf[bufno];
	    McASP_App_bufs[APP_BUFFER_TX][i].index = bufno;
        Osal_Queue_put((Osal_Queue_handle(&McASP_App_Buffer_FreeList[APP_BUFFER_TX])),(Osal_Queue_Elem *)&McASP_App_bufs[APP_BUFFER_TX][i]);

      
	}  
}
/* Pop a buffer not in use */
McASP_App_BufferInfo_t * McASP_App_Buffers_PopFree(McASP_App_Buffer_Direction_e dir)
{
  McASP_App_BufferInfo_t *ptr;
  ptr = Osal_Queue_get(&McASP_App_Buffer_FreeList[dir]);
  if(ptr!=NULL) {
    Osal_Queue_put((Osal_Queue_handle(&McASP_App_Buffer_TransitList[dir])),(Osal_Queue_Elem *)ptr);
  }
  return(ptr);
}

/* Get from the transit list */
McASP_App_BufferInfo_t * McASP_App_Buffers_PopTransitBuf(McASP_App_Buffer_Direction_e dir)
{
  McASP_App_BufferInfo_t *ptr;
  ptr = Osal_Queue_get(&McASP_App_Buffer_TransitList[dir]);
  return(ptr);
}
/* Release to free list */
void McASP_App_Buffers_Release(McASP_App_BufferInfo_t *bufQueueEntry,McASP_App_Buffer_Direction_e dir)
{
  Osal_Queue_put((Osal_Queue_handle(&McASP_App_Buffer_FreeList[dir])),(Osal_Queue_Elem *)bufQueueEntry);
}


Error_Block eb;

int mcaspControlChanTest(void * mcaspChan);

/**************************************************************************************/
/*   FUNCTION DESCRIPTION: This utility function converts local GEM L2 address in to global
    memory addresses used by the EDMA inside McASP
*/
/**************************************************************************************/
static uintptr_t getGlobalAddr (uintptr_t addr)
{
    if ((addr >= 0x800000) && (addr < 0x1000000))
    {
#ifdef _TMS320C6X
        uint32_t coreNum;

        /* Get the core number. */
        coreNum = CSL_chipReadReg(CSL_CHIP_DNUM);

#if defined(SOC_AM572x) || defined(SOC_AM571x)
        /* Compute the global address. */
        return ((1 << 30) | (coreNum << 24) | (addr & 0x00ffffff));

#else
  /* Compute the global address. */
        return ((1 << 28) | (coreNum << 24) | (addr & 0x00ffffff));
#endif
#else
        return addr;
#endif
    }
    else
    {
        /* non-L2 address range */
        return addr;
    }
}
/*********************** APPLICATION DEFINED FUNCTIONS: Begin ****************************/
/* The below functions need to be defined by the application and are registered to the
   McASP driver during instantiation 
 */
/*
 * This call back function is provided to the McASP driver during mcaspCreateChan()
 * and is called at the end of a transaction. This example uses the same call back function
 * for both TX and RX transfers and the call back argument is not being used in this
 * application and hence we pass NULL during mcaspCreateChan() as the call back argument.
 * This need not be the case for other applications where they could use a seperate
 * call back function for TX and RX. Also they could provide a non-NULL argument as
 * the call back argument and use it in their implementation of the call back function(s).
 */
void mcaspAppCallback(void* arg, MCASP_Packet *ioBuf)
{

   McASP_App_BufferInfo_t *bufInfo_transit; /* The buffer in transit */


	if(ioBuf->cmd == MCASP_READ)
	{   
		RxFlag++;
		/* Get the buffer which is supposed to be the next. It should be in the transit queue */
        bufInfo_transit = McASP_App_Buffers_PopTransitBuf(APP_BUFFER_RX);
		if(ioBuf->addr != (uintptr_t *)getGlobalAddr((uintptr_t)bufInfo_transit->buf)) {
#if !defined(AUDIO_DC_DIGITAL_TEST)
		   MCASP_log("Rx Buf Address mismatch on packet [%d] Expected %p, got %p with size %d\n",RxFlag,(uintptr_t *)getGlobalAddr((uintptr_t)bufInfo_transit->buf),ioBuf->addr,ioBuf->size);
#endif

		}

        
        /* Release it back to the Free list */ 
        McASP_App_Buffers_Release(bufInfo_transit,APP_BUFFER_RX);

	    /* post semaphore */
	    Semaphore_post(semR);
	
	} else if(ioBuf->cmd == MCASP_WRITE)
		{
		 TxFlag++;
        
          /* Get the buffer which is supposed to be in the top of the list */
          bufInfo_transit = McASP_App_Buffers_PopTransitBuf(APP_BUFFER_TX);
		  if(ioBuf->addr != (uintptr_t *)getGlobalAddr((uintptr_t)bufInfo_transit->buf)) {
#if !defined(AUDIO_DC_DIGITAL_TEST)
		     MCASP_log("Tx Buf Address mismatch on packet[%d] Expected %p, got %p with size %d\n",TxFlag,(uintptr_t *)getGlobalAddr((uintptr_t)bufInfo_transit->buf),ioBuf->addr,ioBuf->size);
#endif
		  }

          /* Release it back to the Free list */ 
          McASP_App_Buffers_Release(bufInfo_transit,APP_BUFFER_TX);
        
		  /* post semaphore */
		  Semaphore_post(semT);
		} else {
  		  MCASP_log("Callback: Spurious packet ! Buff addr = %p with size %d",ioBuf->addr,ioBuf->size);
		}
	

}
/*
 * This call back is used during interrupt processing and is defined by the
 * application for error handling. These functions are called back from within the
 * mcasp driver when an error interrupt happens and macspIsr() is being called.
 * The sample error handling functions just records these errors which
 * are later used for analyzing the errors seen.
 */
/* The below variables are used to quit the frame processing loop if an error occurs */
int gblErrFlagXmt=0;
int gblErrFlagRcv=0;
/* The below variables are used to analyze the errors if an error interrupt happens */
Mcasp_errCbStatus errCbStatusXmt;
Mcasp_errCbStatus errCbStatusRcv;

/* Error handler for Transmit side */
void GblErrXmt(Mcasp_errCbStatus errCbStat)
{
	gblErrFlagXmt=1;
	errCbStatusXmt=errCbStat;
}
/* Error handler for Rcv side */
void GblErrRcv(Mcasp_errCbStatus errCbStat)
{
    gblErrFlagRcv=1;
    errCbStatusRcv=errCbStat;
}
/*********************** APPLICATION DEFINED FUNCTIONS: End ****************************/




/**************************************************************************************/
/* FUNCTION DESCRIPTION: This function analyzes the result of error interrupts, if it
 * happened
*/
/**************************************************************************************/	 
void mcaspAnalyzeErrors(Mcasp_errCbStatus *errCbStat)
{
    MCASP_log("\n------------ Error stats --------------\n");
    MCASP_log("*****  isClkFailErr : %d\n",errCbStat->isClkFailErr);
    MCASP_log("*****  isDMAErr    : %d\n",errCbStat->isDMAErr);
    MCASP_log("*****  isSyncErr   : %d\n",errCbStat->isSyncErr);
    MCASP_log("*****  retVal      : %d \n",errCbStat->retVal);
    MCASP_log("*****  isRcvOvrRunOrTxUndRunErr : %d \n",errCbStat->isRcvOvrRunOrTxUndRunErr);
}

/**************************************************************************************/
/* Sample Watchdog routine for parsing errors */
/**************************************************************************************/
void ErrorWatchDogRoutine()
{
    Mcasp_errCbStatus errCbStat_Xmt;
    Mcasp_errCbStatus errCbStat_Rcv;

    /* Query the transmit channel's error stat */
    mcaspControlChan(hMcaspTxChan,Mcasp_IOCTL_CHAN_QUERY_ERROR_STATS,&errCbStat_Xmt);
    mcaspControlChan(hMcaspTxChan,Mcasp_IOCTL_CHAN_QUERY_ERROR_STATS,&errCbStat_Rcv);

    /* For testing purpose, just print the contents. Application may choose to run this in
     * a background task and take action. In this example we are just printing the error */
    MCASP_log("\n\n******************* Transmit Watch dog stats ****************");
    mcaspAnalyzeErrors(&errCbStat_Xmt);

    MCASP_log("\n\n******************* Receive Watch dog stats ****************");
    mcaspAnalyzeErrors(&errCbStat_Rcv);
}

/**************************************************************************************/
/*   FUNCTION DESCRIPTION: This function creates the McASP channels for Tx and Rx 
     This function also creates the codec channels (if any)
*/
/**************************************************************************************/	 
static Void createStreams()
{
	int status;



	/* Create Mcasp channel for Rx */
	status = mcaspCreateChan(&hMcaspRxChan, hMcaspDev,
	                         MCASP_INPUT,
	                         &mcasp_chanparam[0],
	                         mcaspAppCallback, NULL);
	if((status != MCASP_COMPLETED) || (hMcaspRxChan == NULL))
	{
		MCASP_log("mcaspCreateChan for McASP2 Rx Failed\n");
		BIOS_exit(0);
	}

    /* Create Mcasp channel for Tx */
    status = mcaspCreateChan(&hMcaspTxChan, hMcaspDev,
                             MCASP_OUTPUT,
                             &mcasp_chanparam[1],
                             mcaspAppCallback, NULL);


    if((status != MCASP_COMPLETED) || (hMcaspTxChan == NULL))
    {
        MCASP_log("mcaspCreateChan for McASP2 Tx Failed\n");
        BIOS_exit(0);
    }


    CodecReset();

	Task_sleep(10);

    I2CCodecIfInit (SOC_I2C2_BASE, 1, AIC12_INST0_ADDRESS);
   AIC12Init(1);
   AIC12Gain(1,15,20);
   I2C_CodecClose(1);

   I2CCodecIfInit (SOC_I2C1_BASE, 0, AIC12_INST0_ADDRESS);
   AIC12Init(0);
   AIC12Gain(0,15,20);
   I2C_CodecClose(0);

   HW_WR_REG32(CSL_DSP_I2C1_REGS + 0x94,0x01);


}

/*
 * ======== prime ========
 */
MCASP_Packet rxFrame[NUM_APP_BUFS_QUEUE_ENTRIES];
MCASP_Packet txFrame[NUM_APP_BUFS_QUEUE_ENTRIES];

#include <ti/sysbios/family/c64p/Hwi.h>

Hwi_Handle myHwi;

static Void prime()
{
	//Error_Block  eb;
    int32_t        count = 0, status;
    //IHeap_Handle iheap;
    uint32_t tx_bytes_per_sample=(mcasp_chanparam[1].wordWidth/8);
    uint32_t rx_bytes_per_sample=(mcasp_chanparam[0].wordWidth/8);
    /* This represents the actual  number of bytes being transferred by the
     * DMA to/from the Host memory to the McASP peripheral. This include all serializers and timeslots.
     * BUFLEN contains the samples per serializer (inclusive of its timeslots) */
    uint32_t tx_frame_size = BUFLEN*MCASP_NUM_SLOTS*mcasp_chanparam[1].noOfSerRequested*tx_bytes_per_sample;
    uint32_t rx_frame_size = BUFLEN*MCASP_NUM_SLOTS*mcasp_chanparam[0].noOfSerRequested*rx_bytes_per_sample;
    McASP_App_BufferInfo_t *appBuf_ptr;
    
    McASP_App_BufferInfo_Init();

    for(count = 0; count < test_prime_count; count ++)
    {
            /* Pop a free application buffer */
            appBuf_ptr=McASP_App_Buffers_PopFree(APP_BUFFER_RX);
            /* Issue the first & second empty buffers to the input stream         */
	        memset((uint8_t *)appBuf_ptr->buf, (0xB0+count), rx_frame_size);
    
			/* RX frame processing */
			rxFrame[count].cmd = MCASP_READ;
			rxFrame[count].addr = (void*)(getGlobalAddr((uintptr_t)appBuf_ptr->buf));
 		    rxFrame[count].size = rx_frame_size;
			rxFrame[count].arg = (uintptr_t) hMcaspRxChan;
			rxFrame[count].status = 0;
			rxFrame[count].misc = 1;   /* reserved - used in callback to indicate asynch packet */
		   status = mcaspSubmitChan(hMcaspRxChan, &(rxFrame[count]));


	
		if((status != MCASP_PENDING))
			MCASP_log ("Debug: Error McASP2 RX : Prime  buffer  #%d submission FAILED\n", count);


    }
    for(count = 0; count < (test_prime_count); count ++)
    {
         /* Pop a free application buffer */
         appBuf_ptr=McASP_App_Buffers_PopFree(APP_BUFFER_TX);
	     memset((uint8_t *)appBuf_ptr->buf, (0xA0+count), tx_frame_size);

   			/* TX frame processing */
		 txFrame[count].cmd = MCASP_WRITE;

         txFrame[count].addr = (void*)(getGlobalAddr((uintptr_t)appBuf_ptr->buf));

		 txFrame[count].size = tx_frame_size;
	 
   		 txFrame[count].arg = (uintptr_t) hMcaspTxChan;
   		 txFrame[count].status = 0;
   		 txFrame[count].misc = 1;   /* reserved - used in callback to indicate asynch packet */

   		status = mcaspSubmitChan(hMcaspTxChan, &(txFrame[count]));
   		if((status != MCASP_PENDING))
   			MCASP_log ("Debug: Error McASP2 TX : Prime  buffer  #%d submission FAILED\n", count);
     }

}

extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[];
/* EnableEDMA event in the SampleCfg*/
static void enableEDMAHwEvent(uint32_t edmaNum, uint32_t eventNo) {
  sampleEdma3GblCfgParams[edmaNum].dmaChannelHwEvtMap[eventNo/32] |= (1 << (eventNo%32));
}

EDMA3_DRV_Handle McaspApp_edmaInit(Mcasp_HwInfo *cfg)
{
    EDMA3_DRV_Handle hEdma;
    EDMA3_DRV_Result edmaResult = 0;

    enableEDMAHwEvent(EDMACC_NUM,MCASP_RX_DMA_CH);
    enableEDMAHwEvent(EDMACC_NUM,MCASP_TX_DMA_CH);

    hEdma = edma3init(EDMACC_NUM, &edmaResult);

    if (edmaResult != EDMA3_DRV_SOK)
    {
        /* Report EDMA Error
         */
        MCASP_log("\nEDMA driver initialization unsuccessful\n");
    }
    else
    {
        MCASP_log("\nEDMA driver initialization successful.\n");
    }

    return hEdma;
}


/*
 * ======== echo ========
 * This function copies from the input SIO to the output SIO. You could
 * easily replace the copy function with a signal processing algorithm.
 */
extern Int aic31MdBindDev(Ptr *, Int, Ptr);

int gtxFrameIndexCount=0;
int grxFrameIndexCount=0;
int itemp;
int result, pwr_status, fs_status, bck_status;
int total_frames_sent=0;

#define IO_CONSOLE
McASP_App_BufferInfo_t *appBuf_ptr_tx = NULL;
McASP_App_BufferInfo_t *appBuf_ptr_rx = NULL;
char AudioBuff[BUFLEN*2];
int16_t ProcessAudioBuff[2][BUFLEN];
volatile unsigned int processAudio=0;
extern short _audio[];
unsigned int _index=0;

extern int16_t audio_in_buff[HD_MAX_CHANNELS][NSAMPLES];

extern int16_t audio_out_buff[HD_MAX_CHANNELS][NSAMPLES];

//volatile uint16_t AP_frame_count;
volatile uint16_t AP_frame_host_read =0 ;

unsigned int SendAudio(char * _audioBuff, unsigned int len)
{
    if(processAudio)
    {
        memcpy(AudioBuff,_audioBuff,len);
        processAudio=0;
        return 1;
    }
    return 0;
}

#pragma DATA_SECTION(tmpbuff,"DDR")
short tmpbuff[100000];

//AP_audio_local_ctrl audio_local_ctrl;
//AP_vocoder_config vocoder_config;



//#pragma CODE_SECTION(Audio_echo_Task,"IRAM")
Void Audio_echo_Task()
{
    Mcasp_HwInfo hwInfo;
    volatile int32_t i32Count, status = 0;
	hMcaspDev  = NULL;
    uint32_t semTimeout;
    int32_t txSemStatus, rxSemStatus;
    uint32_t rx_frames = 0, tx_frames = 0;

    uint32_t tx_bytes_per_sample=(mcasp_chanparam[1].wordWidth/8);
    uint32_t rx_bytes_per_sample=(mcasp_chanparam[0].wordWidth/8);
    /* This represents the actual  number of bytes being transferred by the
     * DMA to/from the Host memory to the McASP peripheral. This include all serializers and timeslots.
     * BUFLEN contains the samples per serializer (inclusive of its timeslots) */
    uint32_t tx_frame_size = BUFLEN*TX_NUM_SERIALIZER*MCASP_NUM_SLOTS*tx_bytes_per_sample;
    uint32_t rx_frame_size = BUFLEN*RX_NUM_SERIALIZER*MCASP_NUM_SLOTS*rx_bytes_per_sample;





#ifdef MEASURE_TIME
    profiling_init();
#endif

    uint32_t tx_ser_size=BUFLEN*MCASP_NUM_SLOTS*tx_bytes_per_sample;

    //veuInit();
    AlgInitAll();

    MCASP_log("\n******** Audio Loopback demo ********\n");
    MCASP_log("Send audio signals in to the EVM's audio-in port and hear the same audio in the audio-out port\n");

  MCASP_log("NUM_BUFS=%d \n",test_num_bufs);
  MCASP_log("PRIME_COUNT=%d \n",test_prime_count);


      Mcasp_socGetInitCfg(MCASP_NUM, &hwInfo);

    hwInfo.dmaHandle = McaspApp_edmaInit(&hwInfo);


      Mcasp_socSetInitCfg(MCASP_NUM, &hwInfo);

	/* 2. SEM Initializations */
    Semaphore_Params_init(&params);

	/* Create semaphores to wait for buffer reclaiming */
    semR = Semaphore_create(0, &params, &eb);
    semT = Semaphore_create(0, &params, &eb);

	/* 3. McASP Initializations */
	/* Initialize McASP Tx and Rx parameters */

	mcaspParams    = Mcasp_PARAMS;
	mcaspParams.enablecache=1;

	status = mcaspBindDev(&hMcaspDev, MCASP_NUM, &mcaspParams);
	if((status != MCASP_COMPLETED) || (hMcaspDev == NULL))
	{
		MCASP_log("mcaspBindDev for McASP Failed\n");
		abort();
	}



    /* Call createStream function to create I/O streams                       */
    createStreams();



    MCASP_log("Initialization complete. priming about to begin \n");
    



    /* Call prime function to do priming                                      */
    prime();

    /* Since Rx data arrival due to Tx buffering in non device-loopback cases,
     * and because we need data from Rx before submitting to the Tx stream, we
     * wait indefinitely for both to complete.
     */
    semTimeout = BIOS_WAIT_FOREVER;
    Bool initCodecs=FALSE;
    /* Forever loop to continuously receive and transmit audio data           */
    for (i32Count = 0; i32Count >= 0; i32Count++)
    {



    	if(gblErrFlagXmt || gblErrFlagRcv)
    		break;

    	//AP_frame_count++;

    	//host_ctrl_audio_update();

        rxSemStatus = Semaphore_pend(semR, semTimeout);
        txSemStatus = Semaphore_pend(semT, semTimeout);

        if(rxSemStatus == TRUE)
        {
            /* Reclaim full buffer from the input stream if ready */
            appBuf_ptr_rx = McASP_App_Buffers_PopFree(APP_BUFFER_RX);
            grxFrameIndexCount = appBuf_ptr_rx->index;

            Cache_inv((void *)((uint8_t *)appBuf_ptr_rx->buf),rx_frame_size,Cache_Type_ALL, TRUE);
        }

        if(txSemStatus == TRUE)
        {
            /* Reclaim full buffer from the output stream if ready */
            appBuf_ptr_tx = McASP_App_Buffers_PopFree(APP_BUFFER_TX);
            gtxFrameIndexCount = appBuf_ptr_tx->index;
            processAudio=1;
        }

#ifdef MEASURE_TIME
    profiling_end();
#endif

        /******************************* Sample Processing Begins ***************************/
	    /* (BUFLEN* RX_NUM_SERIALIZER) 32-bit samples samples have been accumulated in rxbuf[grxFrameIndexCount] now.
	       Application specific processing on these samples, before sending it back to McASP via 
	       txbuf[grxFrameIndexCount].
		   APPLICATION SPECIFIC PROCESSING could be done here. Below are the few audio demos and their
		   application specific processing shown below.
	    */

        /* DEFAULT CASE: Copy the frame received and send it back to Tx buffer.
		   This way the audio received by McASP from the remote device, is loopbacked and sent back
		   to the device here.
		*/

#ifdef TEST_FILE
    int i;
            for(i=0;i<BUFLEN;i++)
            {
                ProcessAudioBuff[0][i]=(short)(_audio[_index+i]*5);
                ProcessAudioBuff[1][i]=(short)(_audio[_index+i]*5);
            }
        //memcpy((void *)(ProcessAudioBuff),(void *)((uint8_t *)appBuf_ptr_rx->buf),(rx_frame_size));
#else

        memcpy((void *)(audio_in_buff[0]),(void *)((uint8_t *)appBuf_ptr_rx->buf),(rx_frame_size));

#endif
       // ProcessChannels(HD_ADC, audio_in_buff, 0x3);
        audio_process(0x3);



        memcpy((void *)((uint8_t *)appBuf_ptr_tx->buf),(void *)((uint8_t *)&audio_out_buff[0][0]),(rx_frame_size));

#ifdef SAVE_FILE
        memcpy(&tmpbuff[_index],(void *)(ProcessAudioBuff),BUFLEN*2);

#endif
        _index+=BUFLEN;
        if((_index+(BUFLEN))>100000)
        {
            _index=0;

        }
        if((_index%50000)==0)
        {
#ifdef MEASURE_TIME
      profiling_display_results();
#endif
        }
        /******************************* Sample Processing End ***************************/
        if(rxSemStatus == TRUE)
        {
            /* Issue an empty buffer to the input stream                          */
            rxFrame[grxFrameIndexCount].cmd = MCASP_READ;
            rxFrame[grxFrameIndexCount].addr = (void*)getGlobalAddr((uintptr_t)rxbuf[grxFrameIndexCount]);
            rxFrame[grxFrameIndexCount].size = rx_frame_size;
            rxFrame[grxFrameIndexCount].arg = (uintptr_t) hMcaspRxChan;
            rxFrame[grxFrameIndexCount].status = 0;
            rxFrame[grxFrameIndexCount].misc = 1;   /* reserved - used in callback to indicate asynch packet */

            status = mcaspSubmitChan(hMcaspRxChan, &(rxFrame[grxFrameIndexCount]));
            if((status != MCASP_PENDING))
                MCASP_log ("Debug: Error McASP RX :  buffer  #%d submission FAILED\n", i32Count);
        }

        if(txSemStatus == TRUE)
        {
            //Cache_wbInv((void *)((uint8_t *)appBuf_ptr_tx->buf),tx_frame_size,Cache_Type_ALL, TRUE);

            /* Issue full buffer to the output stream                             */
            /* TX frame processing */
            txFrame[gtxFrameIndexCount].cmd = MCASP_WRITE;

            txFrame[gtxFrameIndexCount].addr = (void*)getGlobalAddr((uintptr_t)appBuf_ptr_tx->buf);

            txFrame[gtxFrameIndexCount].size = tx_frame_size;
            txFrame[gtxFrameIndexCount].arg = (uintptr_t) hMcaspTxChan;
            txFrame[gtxFrameIndexCount].status = 0;
            txFrame[gtxFrameIndexCount].misc = 1;   /* reserved - used in callback to indicate asynch packet */

            status = mcaspSubmitChan(hMcaspTxChan, &(txFrame[gtxFrameIndexCount]));
            if((status != MCASP_PENDING))
                MCASP_log ("Debug: Error McASP TX : Prime  buffer  #%d submission FAILED\n", i32Count);
        }
		
#ifdef MEASURE_TIME
        profiling_start();
#endif
		/* Test Mcasp control API */
		status = mcaspControlChanTest(hMcaspRxChan);
		if(status != MCASP_COMPLETED) {
		    MCASP_log("mcaspControlChanTest Failed\n");
		}

      /*  if(!initCodecs)
        {
            I2CCodecIfInit (SOC_I2C2_BASE, 1, AIC12_INST0_ADDRESS);
            AIC12Init(1);
            AIC12Gain(1,3,3);
            I2C_CodecClose(1);

            I2CCodecIfInit (SOC_I2C1_BASE, 0, AIC12_INST0_ADDRESS);
            AIC12Init(0);
            AIC12Gain(0,3,3);
            I2C_CodecClose(0);

            initCodecs=TRUE;
        }*/

    } /* end of for (i32Count = 0; i32Count >= 0; i32Count++) */
       
        MCASP_log("\nTotal frames sent:     %d", tx_frames);
        MCASP_log("\nTotal frames received: %d", rx_frames);
  	    ErrorWatchDogRoutine();
  	    if(gblErrFlagXmt) {
  	       MCASP_log("\n Transmit ERROR occured\n");
  	       mcaspAnalyzeErrors(&errCbStatusXmt);
    	}

        if(gblErrFlagRcv) {
            MCASP_log("\n Receive ERROR occured\n");
            mcaspAnalyzeErrors(&errCbStatusRcv);
    	}

        MCASP_log("\nDeleting Rx channel");
        status = mcaspDeleteChan(hMcaspRxChan);
        MCASP_log("\nDeleting Tx channel");
        status = mcaspDeleteChan(hMcaspTxChan);
        MCASP_log("\nUnBinding Mcasp");
    	status = mcaspUnBindDev(hMcaspDev);

		{
			IHeap_Handle iheap;

			iheap = HeapMem_Handle_to_xdc_runtime_IHeap(myHeap);
			Error_init(&eb);
			for(i32Count = 0; i32Count < (test_num_bufs); i32Count ++)
				{
					Memory_free(iheap,rxbuf[i32Count],rx_frame_size);
					Memory_free(iheap,txbuf[i32Count],tx_frame_size);
				}
		}
	  /* Display profiling results */	


/* Application specific report(if any) */

    BIOS_exit(0);
}

/*
 * This function tests the mcaspControlChan API.
 */
int mcaspControlChanTest(void * mcaspChan)
{
    static unsigned int testCount = 0;
    int status;

    status = MCASP_COMPLETED;

    /* Test this API every 100 frames to make sure it doesn't cause any audio glitch. */
    if((testCount % 100) == 0) {
        /* Testing the reconfiguration of wordWidth: first change it to 16. */
        mcasp_chanparam[0].wordWidth = Mcasp_WordLength_16;
        status = mcaspControlChan(mcaspChan, Mcasp_IOCTL_CHAN_PARAMS_WORD_WIDTH, &mcasp_chanparam[0]);

        /* If there is no error, change wordWidth back to 32. This shouldn't impact audio playing out. */
        if(status == MCASP_COMPLETED) {
           mcasp_chanparam[0].wordWidth = Mcasp_WordLength_16;
           status = mcaspControlChan(mcaspChan, Mcasp_IOCTL_CHAN_PARAMS_WORD_WIDTH, &mcasp_chanparam[0]);
        }
    }

    testCount++;

    return status;
}

/* ========================================================================== */
/*                             END OF FILE                                    */
/* ========================================================================== */
