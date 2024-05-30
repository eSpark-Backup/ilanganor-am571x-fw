/******************************************************************************
 * FILE PURPOSE: Main program and supporting routines for the simulation of VEU
 ******************************************************************************
 *
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                                                                                
 *              All rights reserved not granted herein.                           
 *                                                                                
 *                         Limited License.                                       
 *                                                                                
 *  Texas Instruments Incorporated grants a world-wide, royalty-free,             
 *  non-exclusive license under copyrights and patents it now or hereafter owns   
 *  or controls to make, have made, use, import, offer to sell and sell           
 *  ("Utilize") this software subject to the terms herein.  With respect to the   
 *  foregoing patent license, such license is granted solely to the extent that   
 *  any such patent is necessary to Utilize the software alone.  The patent       
 *  license shall not apply to any combinations which include this software,      
 *  other than combinations with devices manufactured by or for TI (“TI           
 *  Devices”).  No hardware patent is licensed hereunder.                         
 *                                                                                
 *  Redistributions must preserve existing copyright notices and reproduce this   
 *  license (including the above copyright notice and the disclaimer and (if      
 *  applicable) source code license limitations below) in the documentation       
 *  and/or other materials provided with the distribution                         
 *                                                                                
 *  Redistribution and use in binary form, without modification, are permitted    
 *  provided that the following conditions are met:                               
 *                                                                                
 *    *  No reverse engineering, decompilation, or disassembly of this software   
 *  is permitted with respect to any software provided in binary form.            
 *                                                                                
 *    *  any redistribution and use are licensed by TI for use only with TI       
 *  Devices.                                                                      
 *                                                                                
 *    *  Nothing shall obligate TI to provide you with source code for the        
 *  software licensed and provided to you in object code.                         
 *                                                                                
 *  If software source code is provided to you, modification and redistribution   
 *  of the source code are permitted provided that the following conditions are   
 *  met:                                                                          
 *                                                                                
 *    *  any redistribution and use of the source code, including any resulting   
 *  derivative works, are licensed by TI for use only with TI Devices.            
 *                                                                                
 *    *  any redistribution and use of any object code compiled from the source   
 *  code and any resulting derivative works, are licensed by TI for use only      
 *  with TI Devices.                                                              
 *                                                                                
 *  Neither the name of Texas Instruments Incorporated nor the names of its       
 *  suppliers may be used to endorse or promote products derived from this        
 *  software without specific prior written permission.                           
 *                                                                                
 *  DISCLAIMER.                                                                   
 *                                                                                
 *  THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS    
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED             
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        
 *  DISCLAIMED. IN NO EVENT SHALL TI AND TI’S LICENSORS BE LIABLE FOR ANY         
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR            
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER    
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT            
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY     
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   
 *  DAMAGE.                                                                       
*****************************************************************************/
/**
 *  @file   veusim.c
 *  @brief  Main program and supporting routines for the simulation of VEU
 *
 *  (C) Copyright 2009, Texas Instruments, Inc.
 */

/** @defgroup  VEUTEST */

/** @ingroup VEUTEST */
/* @{ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ti/mas/types/const.h>
#include <ti/mas/types/types.h>
#include <ti/mas/util/ecomem.h>
#include <ti/mas/fract/fract.h>
#include <ti/mas/util/utl.h>

#include <ti/mas/veu/veu.h>
#include <ti/mas/veu/test/src/veusim.h>
#include <ti/mas/veu/test/src/veusimport.h>

#include <ti/mas/veu/Version.h>

#include <ti/sysbios/knl/Task.h>

#define loop for(;;)

#define USE_CACHE

#ifdef USE_CACHE
#include <ti/mas/sdk/memArchCfg.h>
#endif

#ifdef PROFILE_CYCLES
#include <ti/mas/sdk/profile.h>
tlong     cyclesOverhead; /**< to measure fixed overhead in cycle measurement */
/** @brief for accumulation of cycle statistics */
typedef struct cycles_s {
  tlong     max; /**< running maximum of cycles */
  tlong     avg; /**< running average of cycles (using exponential averaging) */
} cycles_t;

cycles_t  cyclesVeuSendIn; /**< NMU Tx's cycles statistics */
cycles_t  cyclesVeuReceiveIn; /**< NMU Rx's cycles statistics */
#endif



/* the FRAME_SIZE is always set for the frame size used for narrowband(Fs=8KHz) 
    signals. For example, the bit_values 80 means 10ms for narrowband input. Once 
    wideband operation is specified, it will be automatically adjusted to 
    160-sample (same 10ms in wideband) by either veuOpen() or veuControl(); 
    Please see function veusim_open_veu() in this wrapper as an example.*/
#define FRAME_SIZE              80  

/* allocate enough memory for wideband operation */
#define MAX_NUM_SAMPLES_PER_FRAME (veu_INFRAME_MAXLENGTH*2) 

/* Handle CCS Project paths */
#ifdef __CCS_PROJECT_REL_PATH
#define INPUT_TX_LIST_BASE    "../../../"
#define INPUT_FILE_BASE       "../../../vectors/inp/"
#define OUTPUT_FILE_BASE      "../../../vectors/out/"
#else
#define INPUT_TX_LIST_BASE    ""
#define INPUT_FILE_BASE       "vectors/inp/"
#define OUTPUT_FILE_BASE      "vectors/out/"
#endif

/* Define name of the file which contains list of files to be processed */
#ifdef veu_WB_COMPILER
#define INPUT_TX_LIST_FILE "inp_list_file_16k.txt"
#else
#define INPUT_TX_LIST_FILE "inp_list_file_8k.txt"
#endif


tword heapBuf[VEUSIM_HEAP_SIZE];

/* Input & output files */
FILE *inputFile;
FILE *inputFile_Rx;
FILE *outputFile;
FILE *outputFile_Rx;
FILE *inputVEUFile;
FILE *inputTxlistFile, *inputRxlistFile, *outputTxlistFile;
FILE *outputRxlistFile, *inputVEUlistFile;
char inputFileName_Tx[100], inputFileName_Rx[100], outputFileName_Tx[100];
char outputFileName_Rx[100], inputVEUsetting[100];
char iofile_path[256];

/* Progress reporting */
#define VEU_SIM_SAMPLES_PER_SECOND  8000
tlong sample_cnt; 
tuint second_cnt; 

/* veu related parameters */
void *veuInst;

veuControl_t veuCtrl;

/* File reading parameters */
/**
 * @brief: Concatenate a filename and path
 *
 */
char *veusim_concat_filepath (char *base, char *fname) 
{
  strcpy (iofile_path, base);
  strcat (iofile_path, fname);
  return (iofile_path);
}

/**
 * @brief: Open the file which contains  list of files to be processed
 *
 */
void veusim_open_filelist () 
{
  char *list_fptr;
  
  list_fptr = veusim_concat_filepath (INPUT_TX_LIST_BASE, INPUT_TX_LIST_FILE);
  inputTxlistFile = fopen (list_fptr, "r");
}

/**
 * @brief: Get a set of file-names to be processed
 *
 */
void veusim_get_file () 
{
  fscanf (inputTxlistFile, "%s", inputFileName_Tx);
  fscanf (inputTxlistFile, "%s", inputFileName_Rx);
  fscanf (inputTxlistFile, "%s", outputFileName_Tx);
  fscanf (inputTxlistFile, "%s", outputFileName_Rx);
  fscanf (inputTxlistFile, "%s", inputVEUsetting);
}

/**
 * @brief: Initialize test vectors
 *  Open input files to be read, create output files to be write, 
 *  and open the corresponding VQE configuration file.
 %  All input files and output files need to be in little-endian binary format
 *
 */
void veusim_init_testvector ()
{
  char *fpath_ptr;
  
  /* open Tx(Sin) input files */
  fpath_ptr = veusim_concat_filepath (INPUT_FILE_BASE, inputFileName_Tx);
  inputFile = fopen (fpath_ptr, "rb");
  if (inputFile != NULL) {
    printf ("input file %s is opened!\n", inputFileName_Tx);
  }
  else {
    printf ("File Open Error %s\n",inputFileName_Tx);
    exit (1);
  }

  /* open Rx(Rin) input files */
  fpath_ptr = veusim_concat_filepath (INPUT_FILE_BASE, inputFileName_Rx);
  inputFile_Rx = fopen (fpath_ptr, "rb");
  if (inputFile_Rx != NULL) {
    printf ("input file %s is opened!\n", inputFileName_Rx);
  }
  else {
    printf ("File Open Error %s\n",inputFileName_Rx);
    exit (1);
  }

  /* open Tx(Sout) output files */
  fpath_ptr = veusim_concat_filepath (OUTPUT_FILE_BASE, outputFileName_Tx);
  outputFile = fopen (fpath_ptr,"wb");
  if (outputFile!= NULL) {
    printf ("output file %s is opened!\n", outputFileName_Tx);
  }  
  else {
    printf ("File Open Error %s\n",outputFileName_Tx);
    exit (1);
  }

  /* open Rx(Rout) output files */
  fpath_ptr = veusim_concat_filepath (OUTPUT_FILE_BASE, outputFileName_Rx);
  outputFile_Rx = fopen (fpath_ptr,"wb");
  if (outputFile_Rx!= NULL) {
    printf ("output file %s is opened!\n", outputFileName_Rx);
  }  
  else {
    printf ("File Open Error %s\n",outputFileName_Rx);
    exit (1);
  }

  /* open veu setting file */
  fpath_ptr = veusim_concat_filepath (INPUT_FILE_BASE, inputVEUsetting);
  inputVEUFile = fopen (fpath_ptr, "rb");
  if (inputVEUFile!= NULL) {
    printf ("veu setting file %s is opened!\n", inputVEUsetting);
  }  
  else {
    printf ("File Open Error %s\n",inputVEUsetting);
    exit (1);
  }
} /* veusim_init_testvector */


/**
 * @brief: release test vectors / close file handles
 *
 */
void veusim_release_testvector ()
{
  fclose (inputFile);
  fclose (inputFile_Rx);
  fclose (outputFile);
  fclose (outputFile_Rx);
  fclose (inputVEUFile);
} /* veusim_release_testvector */


/**
 * @brief: Read a frame of signal from the input file
 *
 * CALLING FORMAT:
 *   tint veusim_read_frame (tint *inBuf, FILE *inFile, tint frmSize)
 *
 * RETURN VALUE:
 *   flag:   0 - end of file (error)
 *           1 - read successfully
*/
tint veusim_read_frame (linSample *inBuf, FILE *inFile, tint frmSize)
{
  int i;

  if (inFile) {
    for (i = 0; i < frmSize; i++) {
      int lsb, msb;
      lsb = fgetc(inFile);
      msb = fgetc(inFile);
      inBuf[i] = (linSample)((msb << 8) | lsb);
    }

    if (feof(inFile)) return (0);
  }
  return (1);
}


/**
 * @brief: Write a frame of signal to the output file
 *
 * CALLING FORMAT:
 *   tint veusim_write_frame (tint *inBuf, FILE *inFile, tint frmSize)
 *
 * RETURN VALUE:
 *   flag:   0 - end of file (error)
 *           1 - read successfully
*/
tint veusim_write_frame (tint *outBuf, FILE *outFile, tint frmSize)
{
  int k;
  tword buf[2*MAX_NUM_SAMPLES_PER_FRAME];

  if (outFile) {
    for (k = 0; k < frmSize; k++) {
      buf[2*k+0] = (outBuf[k] >> 0) & 0xff;
      buf[2*k+1] = (outBuf[k] >> 8) & 0xff;
    }

    fwrite (buf, sizeof(tword), 2*frmSize, outFile);
  }
  
  return (1); 
}


/**
 * @brief: Read frame size setting (in sample unit for 8KHz sampling rate)
 %          from the VQE configuration file
 *
 */
void veusim_get_frame_size(tint *frmSize)
{
  /* for frame size setting at sample unit (sampled at 8KHz) */
  veu_fileio_read_int (inputVEUFile, "%d\n", frmSize);
  if ((*frmSize<veu_INFRAME_MINLENGTH) || (*frmSize>veu_INFRAME_MAXLENGTH)) {
    printf ("Frame Size Error: frame size should be between 5 to 30ms");
    exit (1);
  }

  /* for frame size setting at ms unit */
  /*  
  fscanf(inputVEUFile, "%d", frmSize);
  if ((*frmSize<5) || (*frmSize>30)) {
    printf ("Frame Size Error: frame size should be between 5 to 30ms");
    exit (1);
  }

  *frmSize *= 8; // default sample rate is 8KHz
  */
}

/**
 * @brief: create a veu instance in unit test wrapper
 *
 * Description: Allocate memory for a veu instance and set it to close initially
 *
 */
void veusim_new_veu ()
{
  int i;
  tint stat, veuNbufs;

  ecomemBuffer_t *bufs;
  ecomemBuffer_t *veuBufs;
  veuSizeConfig_t veuCfgSize;
  veuNewConfig_t  veuCfgNew;
  
  /* VEU get sizes */
  veuCfgSize.max_frame_size = MAX_NUM_SAMPLES_PER_FRAME;
  stat = veuGetSizes (&veuNbufs, &bufs, &veuCfgSize);
  if (stat != veu_NOERR) {
    printf ("Error in veuGetSizes !");
    exit (1);
  }

  /* Allocate veu memory buffer descriptors & increment auxiliary heap size */
  veuBufs = (ecomemBuffer_t*) malloc (veuNbufs * sizeof(ecomemBuffer_t));

  /* Allocate veu memory buffers & increment veu heap size. Data from the (global)
     variable veuBufs is copied into the (local) variable veuLocalBufs in order to
     correctly set veuLocalBufs' size parameter */ 

  /* assume all buffers are in external memory, no alignment requests */
  /* NOTE: This is not true for target platform!!! */
  printf ("VEU memory buffers:\n");
  printf ("    Buffer    Size(twords)    Alignment    Volatile\n");
  for (i = 0; i < veuNbufs; i++) {
    veuBufs[i]             = bufs[i];
    veuBufs[i].mclass      = ecomem_CLASS_EXTERNAL;
    veuBufs[i].log2align   = 0;
    veuBufs[i].volat       = FALSE;
    veuBufs[i].base        = (bufs[i].size)? malloc(bufs[i].size): NULL ;

    printf ("     %3d       %7d         %4d       ", 
                 i, (int)bufs[i].size, (int)bufs[i].log2align);
    if(bufs[i].volat)
      printf ("    yes\n");
    else  
      printf ("    no\n");
  }

  veuCfgNew.handle = NULL;
  veuInst = NULL;
  stat = veuNew ((void **)&veuInst, veuNbufs, veuBufs, &veuCfgNew);
  if (stat != veu_NOERR) {
    printf ("Error in veuNew %d !", stat);
    exit (1);
  }
  
  printf ("veu instance is created successfully\n");
} /* veusim_new_veu */

/**
 * @brief: open a veu instance in unit test wrapper
 *
 * Description: Set a veu instance to open and apply initial configuration 
 *
 */
void veusim_open_veu (tint frame_size)
{
  veuConfig_t veuConfig;
  tint veu_ret; /* for veu return value */

  /* Initialize config params */

  veuConfig.frame_size      = frame_size;
  veuConfig.config_bitfield = veu_CTLM_VEU;
  veuConfig.anr_level       = 12;   /* set ANR max attenuation to 12dB */
  veuConfig.alc_send_level  = -15;  /* set ALC target level to -15dBm0 in Send path*/
  veuConfig.alc_recv_level  = -15;  /* set ALC target level to -15dBm0 in Recv path*/
  veuConfig.bulk_delay      = 162;  /* set bulk delay to 162ms */
  veuConfig.waepl           = 36;   /* set WAEPL to 36dB */
  veuConfig.fgc_send_gain   = 0;    /* set FGC gain to 0dB in Send path */
  veuConfig.fgc_recv_gain   = 0;    /* set FGC gain to 0dB in Recv path */
  
  /* Open VEU Instance */
  veu_ret = veuOpen(veuInst, &veuConfig);
  
  /* In veu simulation, */
  if (veu_ret == veu_ERR_BAD_INST) {
    printf ("Error in open a VEU instance !\n");
    exit (1);
  }
} /* veusim_open_veu */


/**
 * @brief: Configure a veu instance in unit test wrapper
 *
 * Description: Read veu configuration from the file and control the instance 
 *
 */
void veusim_control_veu (void *veuInst, veuControl_t *ctl, tint *frmSize_send, 
                         tint *frmSize_recv)
{
  tint ctrl_code, u_value, mode_mask, mode_value, num_read;
  tint veu_ret; /* for veu return value */

  while (!feof(inputVEUFile)) {
    num_read = veu_fileio_read_int (inputVEUFile, "%d\n", &ctrl_code);
    if (num_read > 0) {
      ctl->ctl_code = ctrl_code;
      if (ctrl_code == veu_CTL_MODE) {
        num_read  = veu_fileio_read_int (inputVEUFile, "%x\n", &mode_mask);
        num_read += veu_fileio_read_int (inputVEUFile, "%x\n", &mode_value);
        if (num_read > 0) {
          ctl->u.mode_ctl.bit_masks = mode_mask;
          ctl->u.mode_ctl.bit_values = mode_value;
          veu_ret = veuControl(veuInst, ctl);
          if (veu_ret == veu_ERR_BAD_INST) {
            printf ("Error: The VEU instance is not open!\n");
            exit (1);
          }
          else if (veu_ret == veu_ERR_BAD_PARAM) {
            printf ("Warning: Invalid VEU configuration parameters!\n");
          }
          else if (veu_ret == veu_ERR_BAD_BUILD) {
            printf ("Error: Incompatible build! ");
            printf ("Can't run wideband operation in narrowband build.\n");
            exit (1);
          }
          /* adjust frame_size in Send and Recv Path, the code is for
              simulation only. The codes need to modify is this 
              function is called during a test-vector is running */
          if ((mode_mask&veu_CTLM_WB_SEND) && (mode_value&veu_CTLM_WB_SEND)) {
            (*frmSize_send) <<= 1; 
          }
          if ((mode_mask&veu_CTLM_WB_RECV) && (mode_value&veu_CTLM_WB_RECV)) {
            (*frmSize_recv) <<= 1; 
          }
        }
        else {
          printf ("Error in veu setting file: %s", inputVEUsetting);
          exit (1);
        }
      }
      else {
        num_read = veu_fileio_read_int (inputVEUFile, "%d\n", &u_value);
        if (num_read > 0) {
          switch (ctl->ctl_code) {
            case veu_CTL_ANR_LEVEL: 
                ctl->u.anr_level = u_value; 
                break;
            case veu_CTL_ALC_SEND_LEVEL: 
                ctl->u.alc_level = u_value; 
                break;
            case veu_CTL_ALC_RECV_LEVEL: 
                ctl->u.alc_level = u_value; 
                break;
            case veu_CTL_WAEPL: 
                ctl->u.waepl = u_value; 
                break;
            case veu_CTL_BULK_DELAY: 
                ctl->u.bulk_delay = u_value; 
                break;
            case veu_CTL_FGC_SEND_GAIN: 
                ctl->u.fgc_gain = u_value; 
                break;
            case veu_CTL_FGC_RECV_GAIN: 
                ctl->u.fgc_gain = u_value; 
                break;
            default: 
                printf ("Error in veu setting file: %s", inputVEUsetting);
                exit (1);
                break;
          }
          veu_ret = veuControl(veuInst, ctl);
          if (veu_ret == veu_ERR_BAD_INST) {
            printf ("Error: The VEU instance is not open!\n");
            exit (1);
          }
          else if (veu_ret == veu_ERR_BAD_PARAM) {
            printf ("Warning: Invalid VEU configuration parameters!\n");
          }
        }
        else {
          printf ("Error in veu setting file: %s", inputVEUsetting);
          exit (1);
        }
      }
    }
    else {
      printf ("Error in veu setting file: %s", inputVEUsetting);
      exit (1);
    }
  }
} /* veusim_control_veu */


void hw_init(void)
{
 #ifdef PROFILE_CYCLES
  if (!profile_initCycleProfiling()) {
    printf ("Could not acquire profiling resources..Exiting\n");
    exit (0);
  }
  /* Initialize the Stats */
  cyclesVeuSendIn.avg    = 0;
  cyclesVeuSendIn.max    = 0; 
  cyclesVeuReceiveIn.avg = 0;
  cyclesVeuReceiveIn.max = 0;
 #endif

 #ifdef USE_CACHE
   memarchcfg_cacheEnable ();
 #endif
}

#ifdef PROFILE_CYCLES
static inline void siu_print_cycles_util (cycles_t * cyclesPtr, char *name,
                                         tlong overhead)
{
  printf ("%s MaxCycles = %ld AvgCycles = %ld\n", name,
         (long) cyclesPtr->max - (long) overhead,
         (long) cyclesPtr->avg - (long) overhead);
}

void siu_print_cycles (void)
{
  siu_print_cycles_util (&cyclesVeuSendIn, "VEU Send IN", cyclesOverhead);
  siu_print_cycles_util (&cyclesVeuReceiveIn, "VEU Receive In", cyclesOverhead);
}

static inline void siu_process_cycles (tlong cycles, cycles_t * cyclesPtr)
{
  if (cycles > cyclesPtr->max) {
    cyclesPtr->max = cycles;
  }

  cyclesPtr->avg = ((cyclesPtr->avg) * 255 + cycles) >> 8;
}
#endif

veuContext_t veuContext;


/*-----------------------------------------------------------------
 * Function:  veu_sim_progress
 *							  
 * Description: Update sample counters 
 *-----------------------------------------------------------------*/
void veu_sim_progress (tint frame_size)
{
  /* Update sample counter */
  sample_cnt += frame_size;
  if (sample_cnt >= (second_cnt + 1) * VEU_SIM_SAMPLES_PER_SECOND) {
    second_cnt++;
    fprintf (stdout, " - Progress: %d seconds\n", second_cnt);
  }
}

tint frmSize = FRAME_SIZE;
tint frmSize_send, frmSize_recv;
tint frame_counter = 0;
#ifdef PROFILE_CYCLES
volatile tlong cycles;
#endif


extern unsigned int SendAudio(char * _audioBuff, unsigned int len);
void veuInit()
{
    veuInst = NULL;

    printf ("Version Information of modules tested:\n");
    printf ("VEU %d.%d.%d.%d\n",
          ti_mas_veu_Version_MAJOR, ti_mas_veu_Version_MINOR,
          ti_mas_veu_Version_PATCH, ti_mas_veu_Version_BUILD);

  //  hw_init ();

    #ifdef PROFILE_CYCLES
    cycles = profile_getCPUcycleCount ();

    cycles = (profile_getCPUcycleCount () - cycles);
    cyclesOverhead = cycles;
    #endif

    /* create a new veu instance */
    veusim_new_veu ();

    veuContext.max_frame_size = 160;

    veusim_open_filelist ();

    if (!feof(inputTxlistFile))
    {
    /* get input file list */
    veusim_get_file ();

    /* open test-vectors and veu setting file */
    veusim_init_testvector ();

    /* read frame size */
    veusim_get_frame_size (&frmSize);
    
    /* set veu instance to be ready */
    veusim_open_veu (frmSize); 

    /* setup parameters for veu instance */
    frmSize_send = frmSize;
    frmSize_recv = frmSize;
    veusim_control_veu (veuInst, &veuCtrl, &frmSize_send, &frmSize_recv);

    /* Reset progress reporting */
    sample_cnt = 0L;
    second_cnt = 0;
    }
}

tint inBufTx[MAX_NUM_SAMPLES_PER_FRAME];
/**
 * @brief: Main program in unit test wrapper
 *
 */
void VeuProcess (char * rxBuff, char * txBuff, unsigned int size)
{
  tint read_flag_Tx, read_flag_Rx;
  tint inBufRx[MAX_NUM_SAMPLES_PER_FRAME];

  tint outBufTx[MAX_NUM_SAMPLES_PER_FRAME];




 // while (!feof(inputTxlistFile)) {

#ifdef FILES_TSET
    /* process the test-vectors */
    if (!feof(inputFile)) {


      /* write singals from files */ 
      read_flag_Tx = veusim_read_frame (inBufTx, inputFile,    frmSize_send);
      read_flag_Rx = veusim_read_frame (inBufRx, inputFile_Rx, frmSize_recv);

      if (read_flag_Tx && read_flag_Rx) {
        frame_counter++;
        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif
        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif
        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif
        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif

        /* process rin signals */
        veuReceiveIn (veuInst, inBufRx);

        #ifdef PROFILE_CYCLES
        cycles = (profile_getCPUcycleCount () - cycles);
        siu_process_cycles (cycles, &cyclesVeuReceiveIn);
        #endif

        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif

        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif

        /* process sin signals */
        veuSendIn (veuInst, inBufTx, inBufRx, outBufTx, NULL);
        
        #ifdef PROFILE_CYCLES
        cycles = (profile_getCPUcycleCount () - cycles);
        siu_process_cycles (cycles, &cyclesVeuSendIn);
        #endif

        while(SendAudio((char *)outBufTx,frmSize_send)==0)
        {
            Task_sleep(1);
        }

        /* write outputs to files */
        veusim_write_frame (outBufTx, outputFile, frmSize_send);
        veusim_write_frame (inBufRx, outputFile_Rx, frmSize_recv);

        /* Sample count and Breakpoint check */
        veu_sim_progress (frmSize);
      } /* if */
    } /* while */
#else
    /* process the test-vectors */
    if (!feof(inputFile))
    {


      /* write singals from files */
      read_flag_Tx = veusim_read_frame (inBufTx, inputFile,    frmSize_send);
      read_flag_Rx = veusim_read_frame (inBufRx, inputFile_Rx, frmSize_recv);

      if (read_flag_Tx && read_flag_Rx)
      {
        frame_counter++;
        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif
        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif
        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif
        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif
       // memcpy(inBufRx,rxBuff,MAX_NUM_SAMPLES_PER_FRAME*2);

        /* process rin signals */
        veuReceiveIn (veuInst, inBufRx);

        #ifdef PROFILE_CYCLES
        cycles = (profile_getCPUcycleCount () - cycles);
        siu_process_cycles (cycles, &cyclesVeuReceiveIn);
        #endif

        #ifdef USE_CACHE
        memarchcfg_cacheFlush ();
        #endif

        #ifdef PROFILE_CYCLES
        cycles = profile_getCPUcycleCount ();
        #endif

        /* process sin signals */
        veuSendIn (veuInst, inBufTx, inBufRx, outBufTx, NULL);

        #ifdef PROFILE_CYCLES
        cycles = (profile_getCPUcycleCount () - cycles);
        siu_process_cycles (cycles, &cyclesVeuSendIn);
        #endif

       /* while(SendAudio((char *)outBufTx,frmSize_send*2)==0)
        {
            Task_sleep(1);
        }
*/
        /* write outputs to files */
        veusim_write_frame (outBufTx, outputFile, frmSize_send);
        veusim_write_frame (inBufRx, outputFile_Rx, frmSize_recv);

        /* Sample count and Breakpoint check */
        veu_sim_progress (frmSize);
        //memcpy(inBufTx,outBufTx,MAX_NUM_SAMPLES_PER_FRAME*2);
        memcpy(txBuff,outBufTx,MAX_NUM_SAMPLES_PER_FRAME*2);
      } /* if */
    } /* while */
#endif
    /* close veu instance */
  //  veuClose (veuInst);

    /* close test-vectors */
  //  veusim_release_testvector ();
    #ifdef PROFILE_CYCLES
    siu_print_cycles ();
    profile_closeCycleProfiling ();
    #endif

    #ifdef PROFILE_CYCLES
    siu_print_cycles ();
    profile_closeCycleProfiling ();
    #endif
 // }

  /* Loop */
 // printf ("\n\nSimulation Complete\n\n");
  
} /* main */
/* @} */ /* ingroup */
/* nothing past this point */
