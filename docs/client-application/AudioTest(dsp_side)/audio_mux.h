/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : multiplex audio channels according to muxing matrix
///	@file:	audio_mux.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	29/Jul/2008	  Created	
//
/******************************************************************************/

#ifndef AUDIO_MUX_
#define AUDIO_MUX_



#define ENERGY_BUFFER_SIZE	100
#define MUX_ENERGY_NORM	1 //enable mux normalizing by calculate input energy

#define mux_matrix(in_indx,out_indx)	((int8_t)((vol_matrix+(in_indx*size_out))[out_indx]))
#define inp(in_indx,sample_indx)	((in+(in_indx*sample_num))[sample_indx])
#define outp(out_indx,sample_indx)	((out+(out_indx*sample_num))[sample_indx])
//#define volume(in_index,out_index) (vol_log_table[mux_matrix(in_indx,out_indx)&0x9f]) 
#define AM_MAX_INPUT_SUM	HD_MAX_CHANNELS //40	//10

#define AM_MODE_WEIGHTED_MEAN	0
#define AM_MODE_MAX_IN_ENERGY	1
#define AM_MODE_CONST_ENERGY	2	

//max sample value/2 and divide by 4 as it is calculated in calc_energy
#define AM_CONST_ENERGY	((0x5a00/2.82)/(4*32)) //(0x7fff/8)
#define AM_MIN_IN_ENERGY	100	//used in const energy mode to mux only if input
								// energy is bigger then this value	

#define AM_VOL_HIGH		(16)
#define AM_VOL_LIMIT	(91)



typedef struct AM_channel_mux
{
	uint8_t	out;
	uint8_t	size;
	uint8_t	inputs[AM_MAX_INPUT_SUM];	
}AM_channel_mux;
/**************************************************************************/                     
//  Module		: mux
//  Description :  mux audio, sum input channel to one output channel
///					according a volume matrix (in the volume matrix there 
///					are weiget value for each input)
///					preform the operator in MUX_ENERGY_MODE:
///					(out)=(vol_matrix)*(in)*(in_energy)/(normalize_factor)
///					normalize_factor=sum(vol_matrix*in_energy)
///					preform the operator in MUX_WEIGHT_MODE:
///					(out)=(vol_matrix)*(in)/(normalize_factor)
///					normalize_factor=sum(vol_matrix)
/// @return none
/// @param	none
/**************************************************************************/
void audio_mux(int8_t* vol_matrix,int16_t* in,int16_t* out,uint16_t size_in
,uint16_t size_out,uint16_t sample_num,uint16_t mode);

/**************************************************************************/                     
//  Module		: mux_init
//  Description :  init mux module
/// @return none
/// @param	none
/**************************************************************************/
void mux_init(void);

/**************************************************************************/
//  Module		: calc_energy
//  Description :  sum input samples and divide by the number of samples 
/// @return none
/// @param	none
/**************************************************************************/
int32_t calc_energy(int16_t* in,int32_t* in_energy,uint16_t ch_num,uint16_t sample_num);

/**************************************************************************/                     
//  Module		: mux_matrix_init
//  Description :  fill mux matrix with a defined value
/// @return none
/// @param	none
/**************************************************************************/
void mux_matrix_init(uint8_t value);

/**************************************************************************/                     
//  Module		: test_audio_mux
//  Description :  test audio mux module functionality 
/// @return none
/// @param	none
/**************************************************************************/
void test_audio_mux(uint16_t mode);

#endif
