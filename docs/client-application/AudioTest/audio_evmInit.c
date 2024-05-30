/*
 * audio_evmInit.c
 *
 * This file contains Audio application related EVM (platform) specific routines
 * implementation. 
 * This is part of the EVM Initialization library (evmInit) which contains pinmux
 * enabling routines for peripherals used by the application, and may contain 
 * related device pre-driver initialization routines.
 *
 * Copyright (C) 2009-2018 Texas Instruments Incorporated - http://www.ti.com/
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

/**
 *  \file   audio_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          audio driver.
 *
 *  (C) Copyright 2009-2018, Texas Instruments, Inc
 *
 */

#include <xdc/std.h>
#define SW_I2C
#ifndef SW_I2C
#include <i2c/include/I2c.h>
#endif

#include <Audio_evmInit.h>
#include "mcasp_cfg.h"
#include <ti/csl/cslr_device.h>
#include <ti/csl/soc/am571x/src/cslr_control_core_pad.h>
#include <ti/csl/soc.h>
#include <ti/drv/mcasp/soc/mcasp_soc.h>
//#include <board.h>
//#include "evmam572x_pinmux.h"
#include <ti/csl/hw_types.h>
#include <ti/drv/uart/UART_stdio.h>
//#include "MCASP_log.h"
#include <ti/csl/csl_i2c.h>
#include <ti/csl/csl_gpio.h>
#include "evmam571x_pinmux.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/

/**
 *  \brief    Initializes platform specific modules
 *
 *  This function initializes the modules like PLL, DDR, I2C etc
 *  which are required for audio processing. Need to call this function
 *  before doing any HW related operations.
 *
 *  \return    Platform_EOK on Success or error code
 */


//extern void PinmuxConfig(void);

extern Board_STATUS Board_init(Board_initCfg);

void Configure_XBAR(void);
void McASP3_Enable(void);

extern void AppGPIOInit(void);


void ResetGpio(unsigned int baseAddr, unsigned int pinNum, unsigned int value)
{
    int delay=0xFFFFF;
    GPIOPinWrite(baseAddr, pinNum, !value);
    while(delay-- > 0);
    GPIOPinWrite(baseAddr, pinNum, value);
}

/*
 * Function to enable the pinmux for the mcasp and i2c devices in the soc.
 *
 */
void configureAudio(void)
{
    //If run DSP without Linux
	/*Board_STATUS stat = BOARD_SOK;
	//Board_initCfg arg =  BOARD_INIT_MODULE_CLOCK | BOARD_INIT_UART_STDIO | BOARD_INIT_PINMUX_CONFIG;
	Board_initCfg arg =  BOARD_INIT_UART_STDIO ;

	stat = Board_init(arg);

    if( BOARD_SOK != stat)
    {
        return;
    }*/

	McASP3_Enable();

	Configure_XBAR();

}
#define GPIO(bus,pin) (bus*16+pin+1)

#define GPIO_CODEC_RST GPIO(5,9)

void McASP3_Enable(void)
{

		// Choose SYS_CLK2 (22.5792 MHZ) as source for ABE_PLL REF CLK
		HW_WR_FIELD32(CSL_DSP_CKGEN_PRM_REGS+CSL_CKGEN_PRM_CM_CLKSEL_ABE_PLL_SYS_REG, \
				CSL_CKGEN_PRM_CM_CLKSEL_ABE_PLL_SYS_REG_CLKSEL, \
				CSL_CKGEN_PRM_CM_CLKSEL_ABE_PLL_SYS_REG_CLKSEL_SEL_SYS_CLK1);

		/* Reprogram ABE DPLL for 451.584 MHz output on PER_ABE_X1_GFCLK line */

		// step 1: disable the PLL, if enabled (ex: via GEL)
		while(HW_RD_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN) == CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN_DPLL_LOCK_MODE)
			HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG, \
					CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN, \
					CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN_DPLL_FR_BYP_MODE);

		// step 2: modify Synthesized Clock Parameters - DPLL MULT & DIV
		HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKSEL_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_CLKSEL_DPLL_ABE_REG_DPLL_MULT, \
				0xC8);

		HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKSEL_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_CLKSEL_DPLL_ABE_REG_DPLL_DIV, \
				0x09);

		// step 3: Configure output clocks parameters - M2 = 1  M3 = 1
		HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_DIV_M2_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_DIV_M2_DPLL_ABE_REG_DIVHS, \
				0x1);
		HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_DIV_M3_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_DIV_M3_DPLL_ABE_REG_DIVHS, \
				0x1);

		// step 4: Confirm that the PLL has locked
		while(HW_RD_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG, \
				CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN) != CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN_DPLL_LOCK_MODE)
			HW_WR_FIELD32(CSL_DSP_CKGEN_CM_CORE_AON_REGS+CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG, \
					CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN, \
					CSL_CKGEN_CM_CORE_AON_CM_CLKMODE_DPLL_ABE_REG_DPLL_EN_DPLL_LOCK_MODE);

	    HW_WR_REG32(SOC_IPU_CM_CORE_AON_BASE + CM_IPU_MCASP1_CLKCTRL, 0x09000002);
	       while ((HW_RD_REG32(SOC_IPU_CM_CORE_AON_BASE + CM_IPU_MCASP1_CLKCTRL)) != 0x09000002)
	           {

	           }


	       //HW_WR_REG32(SOC_IPU_CM_CORE_AON_BASE + CM_IPU_MCASP1_CLKCTRL, CSL_IPU_CM_CORE_AON_CM_IPU_MCASP1_CLKCTRL_REG_CLKSEL_AHCLKX_SEL_XREF_CLK1);
	       //HW_WR_REG32(SOC_IPU_CM_CORE_AON_BASE + CM_IPU_MCASP1_CLKCTRL,0x0000000);
	       //HW_WR_REG32(SOC_IPU_CM_CORE_AON_BASE + CM_L4PER2_MCASP2_CLKCTRL,0x0000000);

	       /* Pad mux configuration for McASP instance 1 */
	       HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_ACLKX,0xc0000);
	       HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_XREF_CLK0,0x40003);
	       HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_FSX,0xc0000);
	       HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_AXR0,0xc0000);
	       HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_AXR1,0xc0000);
	        HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_AXR2,0xc0000);
	           HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_AXR3,0xc0000);

	        HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_XREF_CLK1,0x40000);


	    //HW_WR_REG32(0x4AE06160, 0x1); // CM_CLKSEL_CLKOUT2: 0x1: Selects SYS_CLK2
	    HW_WR_FIELD32(CSL_DSP_CKGEN_PRM_REGS+CSL_CKGEN_PRM_CM_CLKSEL_CLKOUTMUX1_REG, \
	    		CSL_CKGEN_PRM_CM_CLKSEL_CLKOUTMUX1_REG_CLKSEL, \
				CSL_CKGEN_PRM_CM_CLKSEL_CLKOUTMUX1_REG_CLKSEL_SEL_SYS_CLK1);



	    HW_WR_FIELD32(CSL_DSP_COREAON_CM_CORE_REGS+CSL_COREAON_CM_CORE_CM_COREAON_DUMMY_MODULE1_CLKCTRL_REG, \
	    		CSL_COREAON_CM_CORE_CM_COREAON_DUMMY_MODULE1_CLKCTRL_REG_OPTFCLKEN_CLKOUTMUX1_CLK, \
	    		CSL_COREAON_CM_CORE_CM_COREAON_DUMMY_MODULE1_CLKCTRL_REG_OPTFCLKEN_CLKOUTMUX1_CLK_FCLK_EN);

	    HW_WR_REG32(CSL_DSP_I2C1_REGS + 0x94,0x00);


	    /********** Configure GPIO pinmux **************/

	    //Codec reset GPIO5_9
	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP1_AXR7,0x4000E);

	    //Audio amlifiers GPIO6: pin: 14- left, 15- speaker, 10- right
	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_GPIO6_10,0x0000E);

	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_GPIO6_14,0x0000E);

	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_GPIO6_15,0x0000E);

	    //For ptt test
	/*    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP3_ACLKX,0x4000E);
	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP2_AXR2,0x4000E);
	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP3_FSX,0x4000E);
	    HW_WR_REG32(CSL_MPU_CORE_PAD_IO_REGISTERS_REGS+CSL_CONTROL_CORE_PAD_IO_PAD_MCASP2_AXR3,0x4000E);*/

	    /********** Enable GPIO  **************/
	    //Codec reset GPIO5_9
	    unsigned int gpio5_reg=HW_RD_REG32(SOC_GPIO5_BASE+CSL_GPIO_OE);
	    gpio5_reg = ~(1<<9);

	    //For ptt test
	  /*  gpio5_reg &= ~(1<<9);
	    gpio5_reg |= (1<<13|1<<14);*/
	    HW_WR_REG32(SOC_GPIO5_BASE+CSL_GPIO_OE,gpio5_reg);
	    while (HW_RD_REG32(SOC_GPIO5_BASE+CSL_GPIO_OE)!= gpio5_reg) { ; }

	    unsigned int gpio6_reg=HW_RD_REG32(SOC_GPIO6_BASE+CSL_GPIO_OE);
	    gpio6_reg = ~((1<<14)|(1<<15)|(1<<10));
	    //For ptt test
	  /*  gpio6_reg &= ~((1<<14)|(1<<15));
	    gpio6_reg |= ((1<<10)|(1<<8)|(1<<9));*/

	    //Audio amlifiers GPIO6: pin: 14- left, 15- speaker, 10- right
	    HW_WR_REG32(SOC_GPIO6_BASE+CSL_GPIO_OE,gpio6_reg);
	    while (HW_RD_REG32(SOC_GPIO6_BASE+CSL_GPIO_OE)!= gpio6_reg) {; }

	    /********** Reset and set default values GPIO  **************/

	    //Codec reset GPIO5_9
	    ResetGpio(SOC_GPIO5_BASE , 9 , 1);

	    //Audio amlifiers GPIO6: pin: 14- left, 15- speaker, 10- right
	    ResetGpio(SOC_GPIO6_BASE , 10 , 0);

	    ResetGpio(SOC_GPIO6_BASE , 14 , 0);

	    ResetGpio(SOC_GPIO6_BASE , 15 , 0);

	    //For ptt test
/*	    int val=0;
	    while(true)
	    {
	        val=GPIOPinRead2(SOC_GPIO6_BASE, 8);
	        if(val==0)
	        {
	            val=8;
	            break;
	        }
	        val= GPIOPinRead2(SOC_GPIO6_BASE, 9);
            if(val==0)
            {
                val=9;
                break;
            }
	        val=GPIOPinRead2(SOC_GPIO5_BASE, 13);
            if(val==0)
            {
                val=13;
                break;
            }
	        val= GPIOPinRead2(SOC_GPIO5_BASE, 14);
            if(val==0)
            {
                val=14;
                break;
            }
	    }*/
}

void Configure_XBAR(void)
{
    /* Mapping the DMA crossbar for McASP DMA_DREQ_(128-120) in to EDMA3_DREQ_0-15.
     * The EDMA3_DREQ_0-15 correspond to (CSL_EDMA3_CHA_MCASP'n'_RX) which are
     * used inside mcasp_soc.c to register EDMA. Please note that the API below takes
     * the mapped DMAReq  with 1 as the base, whereas CSL_EDMA3_CHA_MCASP'n'
     * are with base 0 */
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,128,1+CSL_EDMA3_CHA_MCASP0_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,129,1+CSL_EDMA3_CHA_MCASP0_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,130,1+CSL_EDMA3_CHA_MCASP1_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,131,1+CSL_EDMA3_CHA_MCASP1_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,132,1+CSL_EDMA3_CHA_MCASP2_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,133,1+CSL_EDMA3_CHA_MCASP2_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,134,1+CSL_EDMA3_CHA_MCASP3_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,135,1+CSL_EDMA3_CHA_MCASP3_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,136,1+CSL_EDMA3_CHA_MCASP4_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,137,1+CSL_EDMA3_CHA_MCASP4_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,138,1+CSL_EDMA3_CHA_MCASP5_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,139,1+CSL_EDMA3_CHA_MCASP5_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,140,1+CSL_EDMA3_CHA_MCASP6_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,141,1+CSL_EDMA3_CHA_MCASP6_TX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,142,1+CSL_EDMA3_CHA_MCASP7_RX);
	CSL_xbarDmaConfigure(CSL_XBAR_DMA_CPU_ID_EDMA,143,1+CSL_EDMA3_CHA_MCASP7_RX);
}

/*
 * Function to configure SOC specific parameters. This includes SOC
 * specific parameters such as interrupt configuration using Muxes,
 * interrupt configuration etc.
 */

void configMcASP_SocHwInfo()
{
  /* Configure the interrupts for the McASP Instance MCASP_NUM */
  /* ON Keystone, it involves CIC programming as well.
   * The McASP does that internally, if configured with the correct parameters.
   * Such as muxNum, muxInEvent, muxOutEvent,
   * cpuInEventNum, cpuIntNum
  */
    Mcasp_HwInfo hwInfo;

    Mcasp_socGetInitCfg(MCASP_NUM, &hwInfo);

    if((hwInfo.rxMuxOutEvent==MCASP_UNASSIGNED_MUX_EVENTNUM) ||
       (hwInfo.txMuxOutEvent==MCASP_UNASSIGNED_MUX_EVENTNUM) ) {
#ifdef _TMS320C6X
       /* Choosing a free Crossbar Instance number from Table 17-3. DSP1_INTC Default Interrupt Mapping
        * in the AM572x TRM . Please note that this is fore Core 0 , i.e DSP1 only*/
       /* Freely available cross bar instance numbers for DSP1 */
       hwInfo.txMuxOutEvent=CSL_XBAR_INST_DSP1_IRQ_74;
       hwInfo.rxMuxOutEvent=CSL_XBAR_INST_DSP1_IRQ_75;
       /* The CPU event numbers corresponding to the above  */
       hwInfo.cpuTxEventNumber = 74;
       hwInfo.cpuRxEventNumber = 75;

#elif defined(__TI_ARM_V7M4__)
      /* M4 */

       /* Choosing a free Crossbar Instance number from IPU_INTC Default Interrupt Mapping
        * in the AM572x TRM. Please note that this is fore Core 0 , i.e IPU1 */
       hwInfo.txMuxOutEvent=CSL_XBAR_INST_IPU1_IRQ_24;
       hwInfo.rxMuxOutEvent=CSL_XBAR_INST_IPU1_IRQ_25;

      /* The corresponding interrupts for MPU  */
       hwInfo.txIntNum = 24;
       hwInfo.rxIntNum = 25;

       hwInfo.cpuTxEventNumber =  24;
       hwInfo.cpuRxEventNumber =  25;
#else
       /* A15 */
       /* Choosing a free Crossbar Instance number from Table 17-2. MPU_INTC Default Interrupt Mapping
        * in the AM572x TRM */
       hwInfo.txMuxOutEvent=CSL_XBAR_INST_MPU_IRQ_141;
       hwInfo.rxMuxOutEvent=CSL_XBAR_INST_MPU_IRQ_142;

       /* The corresponding interrupts for MPU  */
       hwInfo.txIntNum = 141+32;
       hwInfo.rxIntNum = 142+32;

       hwInfo.cpuTxEventNumber =  141+32;
       hwInfo.cpuRxEventNumber =  142+32;
#endif
    }
    /* Write back */
    Mcasp_socSetInitCfg(MCASP_NUM,&hwInfo);
}

void CodecReset()
{
    int delay=0xFFFFF;
    GPIOPinWrite(SOC_GPIO5_BASE, 9, 0);
    while(delay-- > 0);
    GPIOPinWrite(SOC_GPIO5_BASE, 9, 1);
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
