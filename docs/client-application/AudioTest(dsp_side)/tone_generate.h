/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : configure systems as defined by arm, open drivers as defined
//					by host cpu
///	@file:	tone_generate.c	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	14/Aug/2008	  Created	
//
/******************************************************************************/
#ifndef TONE_GEN_
#define TONE_GEN_

#include "main.h"

#define TG_WAIT_STATE		1
#define TG_HIGH_GUARD_STATE	2
#define TG_FUNCTION_STATE	3
#define TG_LOW_GUARD_STATE	4


#define TG_DISABLE	0
#define TG_ENABLE	1
#define TG_SINGLE	2


//#define TG_GAURD_TONE_FREQ			2175	//guard tone freq in hertz
//#define TG_HIGH_GAURD_TONE_VOLUME	32766	//max volume out
//#define TG_LOW_GAURD_TONE_VOLUME	834	//-20 db of speech (assumed speeech vol is ~16383)
//#define TG_FUNC_TONE_VOLUME			10922	//1/3 max guard tone

#define TG_GAURD_TONE_FREQ			2175	//guard tone freq in hertz
//#define TG_HIGH_GAURD_TONE_VOLUME	32766	//max volume out
//#define TG_LOW_GAURD_TONE_VOLUME	2520//834	//-20 db of speech (assumed speeech vol is ~16383)
//#define TG_FUNC_TONE_VOLUME			10922	//1/3 max guard tone
#define TG_1000HZ_TONE_FREQ			1000
#define TG_300HZ_TONE_FREQ			300
#define TG_3000HZ_TONE_FREQ			3000

#define TG_HIGH_GAURD_TONE_VOLUME	1	//max volume out
#define TG_LOW_GAURD_TONE_VOLUME	13//834	//-20 db of speech (assumed speeech vol is ~16383)
#define TG_FUNC_TONE_VOLUME			3	//1/3 max guard tone


#define TG_HIGH_GAURD_DURATION		12	//HIGH GAURD duration in 10ms steps
#define TG_FUNC_DURATION			4	//FUNCTION duration in 10ms steps

#define TG_PTT_F1_COMAND			1950	//PTT command freq in hertz
#define TG_PTT_F2_COMAND			1850	//PTT command freq in hertz
#define TG_REPETER_DISABLE_COMAND	1550
#define TG_REPETER_ENABLE_COMAND	1450

#define TG_GUARD_TONE_NUM			1
#define TG_LOW_GUARD_TONE_NUM		2
#define TG_PTT_F1_TONE_NUM			3	
#define TG_PTT_F2_TONE_NUM			4	
#define TG_REPETER_DISABLE_TONE_NUM	5
#define TG_REPETER_ENABLE_TONE_NUM	6


#define TG_ADDING_TONE	1 //tone will be added to speech
#define TG_ONLY_TONE	2 //no speech only tone


typedef struct TG_tone_table_type
{
	uint32_t freq_hz; // frequency of generated signal in Hz
//	uint32_t duration; // tone duration in msec, 0 mean forever
//	uint32_t volume; // volume of tune 	
	uint32_t enable;
	int16_t tone_samples[NSAMPLES];
	uint32_t arg;		//tone sin arg neede for tone gen 
	uint32_t arg_inc;
}TG_tone_table_type;

typedef struct TG_tone_mux_type
{
	uint8_t tones_number_a[HD_MAX_CHANNELS]; // number of one tone to sum to the output
	uint8_t tones_number_b[HD_MAX_CHANNELS]; // number of second tone to sum to the output
											
	uint8_t sum_to_speech[HD_MAX_CHANNELS];//set to true to sum tone and speech
											//set to false to set tone only	 
	uint8_t volume[HD_MAX_CHANNELS];//volume of tone

}TG_tone_mux_type;

/**************************************************************************/                     
//  Module		: tone_gen
//  Description :  generate single tone
/// @return none
/// @param	none
/**************************************************************************/
//void tone_gen(int16_t* in,uint16_t size_in,uint16_t hertz,uint16_t volume,uint16_t mode);

/**************************************************************************/                     
//  Module		: tone_gen
//  Description :  generate single tone
/// @return none
/// @param	none
/**************************************************************************/
void tone_gen(int16_t* in,uint16_t size_in,uint16_t ch);

/**************************************************************************/                     
//  Module		: tone_command
//  Description :  generate tone command
/// @return none
/// @param	none
/**************************************************************************/
//void tone_command(int16_t command,int16_t* in,int16_t mode);
void tone_command(int16_t command,int16_t ch,int16_t mode);

/**************************************************************************/                     
//  Module		: tone_generate
//  Description :  update tone configuration and generate tone if needed
/// @return none
/// @param	none
/**************************************************************************/
void tone_generate(void);

/**************************************************************************/                     
//  Module		: tone_mux
//  Description :  
/// @return none
/// @param	none
/**************************************************************************/
void tone_mux(int16_t *out,int16_t ch);

/**************************************************************************/                     
//  Module		: tone_init
//  Description :  init mux module
/// @return none
/// @param	none
/**************************************************************************/
void tone_init(void);

/**************************************************************************/                     
//  Module		: test_tone
//  Description :  mux tone for testing (assume host ctrl is in debug and
//				 tone was configured
/// @return none
/// @param	none
/**************************************************************************/
void test_tone(void);

/**************************************************************************/                     
//  Module		: sin_func
//  Description :  implementation of sin function usinf look up table
//				 argument receive is in pi - 16383(q15 - 0.5) is pi/2 32767 is pi
/// @return none
/// @param	none
/**************************************************************************/
int16_t sin_func(uint16_t phase);

/**************************************************************************/
//  Module		: tone_mux_config
//  Description :  config tone mux according to host control
/// @return none
/// @param	none
/**************************************************************************/
void tone_mux_config(void);

#endif //TONE_GEN_
