/*
 *  Copyright (C) 2013-2017 Texas Instruments Incorporated - http://www.ti.com/
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
 /**
 *  \file   mcaspTransmit.c
 *
 *  \brief  This file contains the McASP application, designed to meet specific
 *           requirements -
 *           1. Showcase bit clock of 10MHz
 *           2. Data output on two data lines
 *           3. The McASP to output the data on the lines and observe the same on the CRO.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include <ti/csl/hw_types.h>
#include <ti/csl/csl_mcasp.h>
#include <ti/csl/csl_edma.h>
#include <ti/csl/csl_i2c.h>
#include <ti/csl/soc.h>
#include <ti/csl/csl_gpio.h>
#include <ti/csl/cslr_mcasp.h>
#include <ti/drv/mcasp/soc/mcasp_soc.h>

#include <ti/csl/csl_types.h>

#include <ti/csl/arch/csl_arch.h>

#include <ti/csl/cslr_device.h>
#include <ti/board/board.h>

#include <ti/csl/soc/am571x/src/cslr_control_core_pad.h>
#include <ti/csl/soc/tda3xx/hw_ctrl_core_pad_io.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "codec_if.h"
#include "Audio_evmInit.h"
#include "AudioCfg.h"
#include "audio_mux.h"
#include "system_config.h"
#include "host_ctrl.h"
#include "plc_fp.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

//extern char ti_trace_SysMin_Module_State_0_outbuf__A;

/* ========================================================================== */
/*                          Function prototypes                              */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern LowcFE_c g711_lc[HD_MAX_HOST_DSP_CHANNELS];
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


//extern char ti_trace_SysMin_Module_State_0_outbuf__A;


/*
** The main function. Application starts here.
*/
int main(void)
{

    configureAudio();
    /* Initializing McASP HwInfo parameters */
    McaspDevice_init();
    configMcASP_SocHwInfo();
   //Set memory section non cache non pre-fetch
    *((unsigned *) 0x01848280) &= (~((unsigned)0x9));

    mux_init();
    init_host_ctrl();
    system_config_init();
    init_audio_processing();
    veu_init();

    system_config(SC_RESTART);

    ch_config.codec_channel_opened = (1<<1) | (1<<0);

    unsigned int frame_count=0;

    //HW_WR_REG32( SOC_DSP1_CM_CORE_AON_BASE , 0x102);
    //HW_WR_REG32( SOC_DSP1_CM_CORE_AON_BASE+CM_DSP1_DSP1_CLKCTRL , 0x1);

    while(hd_host_ctrl.sHD_dsp_status.dsp_state!=HD_READY)
    {
      /*  if(frame_count/10%2==0)
            HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_GPIO6_16,0xFFFF);
        else
            HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_GPIO6_16,0x0000);*/
        frame_count++;
        hd_host_ctrl.sHD_dsp_status.frame_count= frame_count;
       system_config(SC_RUN);
    }

    BIOS_start();

    return 0;
}


/***************************** End Of File ***********************************/
