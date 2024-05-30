/*
 * aic12.c
 *
 *  Created on: 10 Jan 2023
 *      Author: gili_k
 */

/**
 * \file  aic12.c
 *
 * \brief APIs to configure the AIC12 codec.
 */

/*
* Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
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
*/

#include "codec_if.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/


#define AIC12_REG1  (1)
#define AIC12_REG2  (2)
#define AIC12_REG3  (3)
#define AIC12_REG4  (4)
#define AIC12_REG5  (5)
#define AIC12_REG6  (6)

//REG1– set continues mode (no control data on audio interface),
//use FIR filter and use 16bit audio data
//#define AIC12_REG1_DEFAULT (0X41)
//use IIR filter and use 16bit audio data
//#define AIC12_REG1_DEFAULT (0X61)
//use FIR filter and use 16bit audio data, the fir has more delay but
//the iir insert some times noise when no speak
#define AIC12_REG1_DEFAULT (0X41)

//REG2- set turbo mode, i2c address as default = 100 (0000)=0x40
#define AIC12_REG2_DEFAULT (0XA0)
//REG3- osr 512 (less noise)
#define AIC12_REG3_DEFAULT (0X09)
//REG4- fs=mclk/(16*N*P*M)=20.48M/(16*5*8*4)=8000
//REG4A- N=5, P=8
#define AIC12_REG4A_DEFAULT (0X28)//(0x1)//
//REG4A- M=4
#define AIC12_REG4B_DEFAULT (0X84) //(0xA0)  (0x8A)//
//REG5A-0db will be set later by application
#define AIC12_REG5A_DEFAULT (0X2A)
//REG5B-0db will be set later by application
#define AIC12_REG5B_DEFAULT (0X6A)
//REG5C-digital side tone gain=mute, 0 input buffer gain
#define AIC12_REG5C_DEFAULT (0XB8)
//REG5D-read version
#define AIC12_REG5D_DEFAULT (0XC0)
//REG6- out2,3 off
#define AIC12_REG6_DEFAULT (0X60)

#define AIC12_RESET (0X20)

#define AIC12_PREAMP_0DB 0
#define AIC12_PREAMP_6DB 1
#define AIC12_PREAMP_12DB 2
#define AIC12_PREAMP_24DB 3

#define AIC12_ADC_MUTE 0x3F
#define AIC12_DAC_MUTE 0x7F

#define CODEC_SWITCH_I2C_ADDR   0X70




#define AIC12_R1    1
#define AIC12_R2    2
#define AIC12_R3    3
#define AIC12_R4A   4
#define AIC12_R4B   5
#define AIC12_R5A   6
#define AIC12_R5B   7
#define AIC12_R5C   8
#define AIC12_R5D   9
#define AIC12_R6    10

#define AIC12_REG_CLR   0
#define AIC12_REG_SET   2
#define AIC12_REG_PUT   3

#define AIC12_REG5A  (0x00000000)
#define AIC12_REG5B  (0x00000040)
#define AIC12_REG5C  (0x00000080)
#define AIC12_REG5D  (0x000000C0)

//use this setting for 20.48m mclk
#define AIC12_R4A_PLL_P8        0x0
#define AIC12_R4A_PLL_N5        (0x5<<3)
//#define AIC12_REG4A_DEFAULT     (AIC12_R4A_PLL_N5|AIC12_R4A_PLL_P8)
#define AIC12_R4B_PLL_M4        0x04
//#define AIC12_REG4B_DEFAULT     (0x80|AIC12_R4B_PLL_M4)

//use this setting for AIC12 OUTPUT2
#define AIC12_R6_INP1_IN        0
#define AIC12_R6_OUT2           (1<<3)
//#define AIC12_REG6_DEFAULT      (AIC12_R6_INP1_IN|AIC12_R6_OUT2)



#define AIC12_R3_ASRF1          0x01
#define AIC12_R3_PWDN           0xc0
#define AIC12_R3_OSR512         0x18

extern Semaphore_Handle I2cSem;
/******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/
/**
 * \brief   Resets the AIC31 Codec
 *
 * \param   baseAddr     Base Address of the interface connected to AIC31
 *
 * \return  None.
 *
 **/
void AIC12Reset(unsigned int baseAddr)
{
    volatile unsigned int delay=0xffff;
    /* Reset the codec */
    CodecRegWrite(baseAddr, AIC12_REG3, AIC12_REG3_DEFAULT|AIC12_RESET);
    //delay
    while (delay--);
    /* Stop Reset*/
    CodecRegWrite(baseAddr, AIC12_REG3, AIC12_REG3_DEFAULT);
}

int AIC12Init(unsigned int codecNum)
{

    unsigned int regRead;
    int ret=0;

   // Semaphore_pend(I2cSem,BIOS_WAIT_FOREVER);

    //REG3– put in reset
    CodecRegWrite(codecNum, AIC12_REG3, AIC12_REG3_DEFAULT | AIC12_RESET);

   //read re1 to clear overflow flag
    regRead=CodecRegRead(codecNum, AIC12_REG1);

    //REG2- set turbo mode, i2c address as default = 100 (0000)=0x40
    CodecRegWrite(codecNum, AIC12_REG2, AIC12_REG2_DEFAULT);

    //REG4- fs=mclk/(16*N*P*M)=20.48M/(16*5*8*4)=8000
    //REG4A- N=5, P=8
    CodecRegWrite(codecNum, AIC12_REG4, AIC12_REG4A_DEFAULT);


    //REG4B- M=4
    CodecRegWrite(codecNum, AIC12_REG4, AIC12_REG4B_DEFAULT);

    //REG5A-ADC GAIN, TBD
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_ADC_MUTE);

    //REG5B-DAC GAIN, TBD
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_DAC_MUTE);

    //REG5C-digital side tone gain=mute, 12 input buffer gain (Default, TBD)
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5C_DEFAULT);

    //CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5D_DEFAULT);

    //REG6- out2,3 off
    CodecRegWrite(codecNum, AIC12_REG6, AIC12_REG6_DEFAULT);

    //REG3- osr 512 (less noise)
    CodecRegWrite(codecNum, AIC12_REG3, AIC12_REG3_DEFAULT);



    //REG1– set continues mode (no control data on audio interface),
    //use FIR filter and use 16bit audio data
    CodecRegWrite(codecNum, AIC12_REG1, AIC12_REG1_DEFAULT);


    //read registe back to check configuration OK
    regRead=CodecRegRead(codecNum, AIC12_REG1);
    if ((regRead&(~(0x90)))!=AIC12_REG1_DEFAULT) //do not check overflow flags
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG2);
    if (regRead!=AIC12_REG2_DEFAULT)
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG3);
    if (regRead!=AIC12_REG3_DEFAULT)
        ret=-1;
    //read reg4 2 times, return B, A
    regRead=CodecRegRead(codecNum, AIC12_REG4);
    if (regRead!=AIC12_REG4B_DEFAULT)
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG4);
    if (regRead!=AIC12_REG4A_DEFAULT)
        ret=-1;
    //read reg5 4 times, return C, D, A, B
    regRead=CodecRegRead(codecNum, AIC12_REG5);
    if (regRead!=AIC12_REG5C_DEFAULT)
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG5);
    //if (regRead!=AIC12_REG5D_DEFAULT) //Do not check codec ID
    //    ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG5);
    if (regRead!=AIC12_ADC_MUTE)
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG5);
    if (regRead!=AIC12_DAC_MUTE)
        ret=-1;
    regRead=CodecRegRead(codecNum, AIC12_REG6);
    if (regRead!=AIC12_REG6_DEFAULT)
        ret=-1;

   // Semaphore_post(I2cSem);
    return ret;
}
char g_adcGain[2]={0,0};
char g_dacGain[2]={0,0};

int AIC12AdcGain(unsigned int codecNum, char gain)
{
    unsigned int regReadA,regReadC;
    unsigned char preAmpGain=AIC12_PREAMP_0DB;
    g_adcGain[codecNum]=gain;
    if (gain>24)
    {
        preAmpGain=AIC12_PREAMP_24DB;
        gain-=24;
    }
    else if (gain>12)
    {
        preAmpGain=AIC12_PREAMP_12DB;
        gain-=12;
    }
    if (gain>20)
        gain=20;
    if (gain<-42)
        gain=AIC12_ADC_MUTE;
    //Semaphore_pend(I2cSem,BIOS_WAIT_FOREVER);

    //REG5A-ADC GAIN
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5A_DEFAULT + gain);
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5B_DEFAULT);
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5C_DEFAULT | preAmpGain);
    regReadC=CodecRegRead(codecNum, AIC12_REG5);//reg 5c
    CodecRegRead(codecNum, AIC12_REG5);//reg 5d
    regReadA=CodecRegRead(codecNum, AIC12_REG5);//reg 5a
    CodecRegRead(codecNum, AIC12_REG5);//reg 5b

   // Semaphore_post(I2cSem);
    if (regReadA!=(AIC12_REG5A_DEFAULT+gain))
        return 0;
    if (regReadC!=(AIC12_REG5C_DEFAULT|preAmpGain))
        return 0;
    return 1;
}
int AIC12DacGain(unsigned int codecNum, char gain)
{

    unsigned int regRead;

    g_dacGain[codecNum]=gain;
    if (gain>20)
        gain=20;
    if (gain<-42)
        gain=AIC12_DAC_MUTE;//mute
   // Semaphore_pend(I2cSem,BIOS_WAIT_FOREVER);
    //REG5B-DAC GAIN
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5B_DEFAULT + gain);

    regRead=CodecRegRead(codecNum, AIC12_REG5);//reg 5c
    regRead=CodecRegRead(codecNum, AIC12_REG5);//reg 5d
    regRead=CodecRegRead(codecNum, AIC12_REG5);//reg 5a
    regRead=CodecRegRead(codecNum, AIC12_REG5);//reg 5b

   // Semaphore_post(I2cSem);
    if (regRead!=(AIC12_REG5B_DEFAULT+gain))
        return 0;

    return 1;
}

int AIC12Gain(unsigned int codecNum,char gainAdc, char gainDac)
{
    unsigned int regReadA,regReadC;
    unsigned int regRead;

    unsigned char preAmpGain=AIC12_PREAMP_0DB;
    g_adcGain[codecNum]=gainAdc;
    if (gainAdc>24)
    {
        preAmpGain=AIC12_PREAMP_24DB;
        gainAdc-=24;
    }
    else if (gainAdc>12)
    {
        preAmpGain=AIC12_PREAMP_12DB;
        gainAdc-=12;
    }
    else if (gainAdc>6)
     {
         preAmpGain=AIC12_PREAMP_6DB;
         gainAdc-=6;
     }

    if (gainAdc>20)
        gainAdc=20;
    if (gainAdc<-42)
        gainAdc=AIC12_ADC_MUTE;

    g_dacGain[codecNum]=gainDac;
    if (gainDac>20)
        gainDac=20;
    if (gainDac<-42)
        gainDac=AIC12_DAC_MUTE;//mute

    //Semaphore_pend(I2cSem,BIOS_WAIT_FOREVER);
  //  CodecSwitch(codecNum,1);

    //REG5A-ADC GAIN
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5A_DEFAULT + gainAdc);
    //REG5B-DAC GAIN
    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5B_DEFAULT + gainDac);

    CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5C_DEFAULT | preAmpGain);
    //REG5D
    //CodecRegWrite(codecNum, AIC12_REG5, AIC12_REG5D_DEFAULT + gainAdc);

    regReadC=CodecRegRead(codecNum, AIC12_REG5);//reg 5c
    CodecRegRead(codecNum, AIC12_REG5);//reg 5d
    regReadA=CodecRegRead(codecNum, AIC12_REG5);//reg 5a
    regRead=CodecRegRead(codecNum, AIC12_REG5);//reg 5b

   // CodecSwitch(codecNum,0);
    if (regReadA!=(AIC12_REG5A_DEFAULT+gainAdc))
        return 0;
    if (regReadC!=(AIC12_REG5C_DEFAULT|preAmpGain))
        return 0;
    if (regRead!=(AIC12_REG5B_DEFAULT+gainDac))
        return 0;
    return 1;
}
void AIC12SetCodecReg(unsigned int codecNum, unsigned char a_addr, unsigned char a_data)
{
    if (a_addr>6)
        return;
   // Semaphore_pend(I2cSem,BIOS_WAIT_FOREVER);
   // CodecSwitch(baseAddr,codecNumber);
    CodecRegWrite(codecNum, a_addr, a_data);
   // CodecSwitch(baseAddr,0);
   // Semaphore_post(I2cSem);
}
/***************************** End Of File ***********************************/

