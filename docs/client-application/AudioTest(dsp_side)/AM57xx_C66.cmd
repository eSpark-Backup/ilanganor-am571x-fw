/****************************************************************************/
/*  AM57xx_C66.cmd                                                          */
/*  Copyright (c) 2015  Texas Instruments Incorporated                      */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on the C66x DSP core of  */
/*                 an AM57xx device.                                        */
/*                 Use it as a guideline.  You will want to                 */
/*                 change the memory layout to match your specific          */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/
MEMORY
{
	//SR_1: o = 0x8D000000 l = 0x01000000   /*  */
	_DLYBUFF: o = 0x8D000000 l = 0x00200000
	//OCMC_RAM3: o = 0x40500000 l=0x0c000000
	//DSP1_PROG: o =0x8C000000 l = 0x01000000
}

SECTIONS
{
    .text          >  DSP1_PROG
    .stack         >  DSP1_PROG
    .bss           >  DSP1_PROG
    .cio           >  DSP1_PROG
    .const         >  DSP1_PROG
    .data          >  DSP1_PROG
    .switch        >  DSP1_PROG
    .sysmem        >  DSP1_PROG
    .far           >  DSP1_PROG
    .args          >  DSP1_PROG
    .ppinfo        >  DSP1_PROG
    .ppdata        >  DSP1_PROG
  
    /* COFF sections */
    .pinit         >  DSP1_PROG
    .cinit         >  DSP1_PROG
  
    /* EABI sections */
    .binit         >  DSP1_PROG
    .init_array    >  DSP1_PROG
    .neardata      >  DSP1_PROG
    .fardata       >  DSP1_PROG
    .rodata        >  DSP1_PROG
    .c6xabi.exidx  >  DSP1_PROG
    .c6xabi.extab  >  DSP1_PROG
    .vects		   >  DSP1_PROG
    .l2_int		   >  DSP1_PROG

    IRAM		   >  DSP1_PROG

    RAM_CODE	   > DSP1_PROG

    DDR			   > DSP1_PROG



    .host		   > 0xa0000000// L2SRAM//OCMC_RAM1//L2SRAM//OCMC_RAM1//SR_1

	.ISRAM_CODE    > DSP1_PROG//OCMC_RAM1//SR_1//L2SRAM
	 DLYBUFF	   > _DLYBUFF

}


