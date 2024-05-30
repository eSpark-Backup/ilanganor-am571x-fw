/*
 * veu.c
 *
 *  Created on: 13 Feb 2023
 *      Author: gili_k
 */
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


#include "AudioCfg.h"

#define loop for(;;)

/* the FRAME_SIZE is always set for the frame size used for narrowband(Fs=8KHz)
    signals. For example, the bit_values 80 means 10ms for narrowband input. Once
    wideband operation is specified, it will be automatically adjusted to
    160-sample (same 10ms in wideband) by either veuOpen() or veuControl();
    Please see function veusim_open_veu() in this wrapper as an example.*/
#define FRAME_SIZE              80

/* allocate enough memory for wideband operation */
#define MAX_NUM_SAMPLES_PER_FRAME (veu_INFRAME_MAXLENGTH*2)

/* veu related parameters */
void *veuInst [HD_MAX_CODEC_CHANNELS];

veuControl_t veuCtrl [HD_MAX_CODEC_CHANNELS];

veuContext_t veuContext;

void open_veu (tint frame_size, uint32_t ch);

void veu_control (void *veuInst, veuControl_t *ctl);

void veu_init(uint32_t ch, uint32_t dir)
{
    if(veuInst[ch]!=NULL)
        return;

    int i;
    tint stat, veuNbufs;

    ecomemBuffer_t *bufs;
    ecomemBuffer_t *veuBufs;
    veuSizeConfig_t veuCfgSize;
    veuNewConfig_t  veuCfgNew;

    veuContext.max_frame_size= MAX_NUM_SAMPLES_PER_FRAME;

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
      veuBufs[i].volat       = TRUE;
      veuBufs[i].base        = (bufs[i].size)? malloc(bufs[i].size): NULL ;

      printf ("     %3d       %7d         %4d       ",
                   i, (int)bufs[i].size, (int)bufs[i].log2align);
      if(bufs[i].volat)
        printf ("    yes\n");
      else
        printf ("    no\n");
    }

    veuCfgNew.handle = NULL;
    veuInst[ch] = NULL;
    stat = veuNew ((void **)&veuInst[ch], veuNbufs, veuBufs, &veuCfgNew);
    if (stat != veu_NOERR) {
      printf ("Error in veuNew %d !", stat);
      exit (1);
    }

    printf ("veu instance is created successfully\n");

    open_veu(FRAME_SIZE, ch);

    veu_control(veuInst[ch], &veuCtrl[ch]);


}

/**
 * @brief: Main program in unit test wrapper
 *
 */
#pragma DATA_SECTION(inBufTx,"IRAM")
tint inBufTx[MAX_NUM_SAMPLES_PER_FRAME];

#pragma DATA_SECTION(inBufRx,"IRAM")
tint inBufRx[2][MAX_NUM_SAMPLES_PER_FRAME];

#pragma DATA_SECTION(outBufTx,"IRAM")
tint outBufTx[MAX_NUM_SAMPLES_PER_FRAME];

void veu_process(uint32_t ch, uint32_t dir, int16_t* in, int16_t* out)
{
    memcpy(inBufTx,in,FRAME_SIZE*2);

    /* process rin signals */
    veuReceiveIn (veuInst[ch], inBufRx[ch]);

    /* process sin signals */
    veuSendIn (veuInst[ch], inBufTx, inBufRx[ch], outBufTx, NULL);

    memcpy(inBufRx[ch], outBufTx, FRAME_SIZE*2);
    memcpy(out, outBufTx, FRAME_SIZE*2);
}

#define MAX_OPT_PARAMS 10
void veu_config_ch(uint32_t ch, uint32_t dir)
{
    veu_control(veuInst[ch], &veuCtrl[ch]);
   /* veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ANR_SEND, 0x1);
        veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_HLC_SEND, 0x1);
        veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_FGC_SEND, 0x1);
        veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_SEND, 0x1);
        veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_AEC, 0x1);
        veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_AGGRESS, 0x1);*/

}
void veu_config(uint32_t ch, uint32_t dir, ...)
{
   /* tint mode_mask, tint mode_value;

    va_list param;

    int _params[MAX_OPT_PARAMS] = {0};

    int i = 0;

    int optValue =(-1);

    va_start(param, dir);

    optValue = va_arg(param, int);

    while(optValue != (-1) && (i < MAX_OPT_PARAMS))
    {
        _params[i++] = optValue;

        optValue = va_arg(param, int);
    }

    mode_mask=veu_CTLM_VEU;

    mode_value=veu_CTLM_VEU;

    veu_configCtl(veuInst[ch], &veuCtrl[ch],0,0);*/
}


tint veu_configCtl(void *veuInst, veuControl_t *ctl, tint mode_mask, tint mode_value)
{
    tint veu_ret; /* for veu return value */

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

    return veu_ret;
}
/**
 * @brief: Configure a veu instance in unit test wrapper
 *
 * Description: Read veu configuration from the file and control the instance
 *
 */
void veu_control (void *veuInst, veuControl_t *ctl)
{

    //tint veu_ret; /* for veu return value */
     veu_configCtl(veuInst, ctl, veu_CTLM_VEU|veu_CTLM_ANR_SEND|veu_CTLM_ALC_SEND|veu_CTLM_AEC, veu_CTLM_VEU/*|veu_CTLM_ANR_SEND*/|veu_CTLM_ALC_SEND/*|veu_CTLM_AEC*/);
  /*  veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ANR_SEND, 0x1);
    veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_HLC_SEND, 0x1);
    veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_FGC_SEND, 0x1);
    veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_SEND, 0x1);
    veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_AEC, 0x1);
    veu_ret = veu_configCtl(veuInst, ctl, veu_CTLM_ALC_AGGRESS, 0x1);*/


} /* veusim_control_veu */

/**
 * @brief: open a veu instance in unit test wrapper
 *
 * Description: Set a veu instance to open and apply initial configuration
 *
 */
void open_veu (tint frame_size, uint32_t ch)
{
  veuConfig_t veuConfig;
  tint veu_ret; /* for veu return value */

  /* Initialize config params */

  veuConfig.frame_size      = frame_size;
  veuConfig.config_bitfield = veu_CTLM_VEU;
  veuConfig.anr_level       = 12;   /* set ANR max attenuation to 12dB */
  veuConfig.alc_send_level  = -9;  /* set ALC target level to -15dBm0 in Send path*/
  veuConfig.alc_recv_level  = -9;  /* set ALC target level to -15dBm0 in Recv path*/
  veuConfig.bulk_delay      = 162;  /* set bulk delay to 162ms */
  veuConfig.waepl           = 10;   /* set WAEPL to 36dB */
  veuConfig.fgc_send_gain   = 20;    /* set FGC gain to 0dB in Send path */
  veuConfig.fgc_recv_gain   = 0;    /* set FGC gain to 0dB in Recv path */

  /* Open VEU Instance */
  veu_ret = veuOpen(veuInst[ch], &veuConfig);

  /* In veu simulation, */
  if (veu_ret == veu_ERR_BAD_INST) {
    printf ("Error in open a VEU instance !\n");
    exit (1);
  }
} /* veusim_open_veu */
