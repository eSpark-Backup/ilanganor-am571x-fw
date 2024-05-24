/*
 * Aic12.h
 *
 *  Created on: 10 Jan 2023
 *      Author: gili_k
 */

#ifndef CODEC_IF_H_
#define CODEC_IF_H_

//#define mcasptest
//#define NO_BIOS
#define MCASP_LIB
#define CODEC_NUM   2

void CodecRegWrite(unsigned int codecNum, unsigned char regAddr,
                   unsigned char regData);

unsigned char CodecRegRead(unsigned int codecNum, unsigned char regAddr);

void I2CCodecIfInit(unsigned int baseAddr, unsigned int codecNum,
                    unsigned int slaveAddr);

void CodecSwitch(unsigned int codecNum, unsigned int value);

void I2C_CodecClose(unsigned int codecNum);
#endif /* CODEC_IF_H_ */
