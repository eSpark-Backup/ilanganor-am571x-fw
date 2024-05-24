/******************************************************************************/
//                         C. Mer Industries Ltd.
//                  5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright C.Mer Industries all rights reserved, confidential proprietary
//
//  Project		: ACS RAC
//  Author		: Ilan Rosilio		
//  Description : 
/// 	function to read and write data to DSP memory via HPI channel
///	@file:	hpi_control.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	20/aug/2006   Created	
//
/******************************************************************************/

#ifndef __DSP_CONFIG_H
#define __DSP_CONFIG_H

#ifdef _MSC_VER
#include <stdint_msvc.h> // Bring MSVC a little more up to speed with C99
#else
#include <stdint.h> // Use the C99 official header
#endif // _MSC_VER


#include "libdsp.h"
/********************************************************************************
*                                  INCLUDE FILES
*******************************************************************************/

/********************************************************************************
*                  DEFINITIONS 
*******************************************************************************/

#define ACB3_BOARD			0
#define ACB4_BOARD			1
#define MACS_BOARD			2
#define NON_BOARD			3
#define MVT_BOARD			4
#define MACS_2R_1T_BOARD	5
#define RACB2_BOARD			6
#define ACB11_BOARD			7
#define SVT_BOARD			8
#define VT_BOARD			9
#define INFRA_BOARD			10
#define MACS_R_BOARD		11
#define RCGW_BOARD			12
#define ACB14_BOARD			13
#define ACB12_BOARD			14
#define SVT2_BOARD           15




#define MACS_RADIO_BOARD		0
#define MACS_FIELD_TEL_BOARD	1
#define MACS_TEL_BOARD			2

#define VOCODER_G729	0
#define VOCODER_G711	1
#define VOCODER_PCM		2

//define character for automatic test 
//char to put for menu start/stop
#define PRINT_SUB_MENU_START	putchar(31) 
#define PRINT_SUB_MENU_STOP	putchar(28) 
//char to put for error resault start/stop
#define PRINT_ERR_START		putchar(29) 
#define PRINT_ERR_STOP		putchar(30) 
//char to put for pass resault start/stop
#define PRINT_OK_START		putchar(31)
#define PRINT_OK_STOP		putchar(30)
//char to put for end of test
#define PRINT_TEST_STOP		putchar(28)
//char to put for value returned
#define PRINT_VAL_START		putchar(31)

#define DC_GPIO_IN	0
#define DC_GPIO_OUT	1

#define ERR_INVALID_VALUE	-3

/********************************************************************************
*                  struct 
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************************
*                  Prototype 
*******************************************************************************/
/**************************************************************************/                  
//  Module		: read_dsp_version
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int print_dsp_version(int *board_name);

/**************************************************************************/                  
//  Module		: set_channel_config
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_channel_config(int *board_name);

/**************************************************************************/                  
//  Module		: ac_init
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_ac_init(int *board_name);

/**************************************************************************/                  
//  Module		: ac_config
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_ac_config(int *board_name);


/**************************************************************************/                  
//  Module		: set_gpio_config
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_gpio_config(int *board_name);

/**************************************************************************/                  
//  Module		: set_gpio_set_state
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_gpio_set_state(int *board_name);

/**************************************************************************/                  
//  Module		: set_vocoder_init
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_vocoder_init(int *board_name);

/**************************************************************************/                  
//  Module		: set_vocoder_enable
//  Description : 
///		    		
/// @return none
/// @param	none
/**************************************************************************/
int set_vocoder_enable(int *board_name);




#ifdef __cplusplus
}
#endif

#endif /* __DSP_CONFIG_H */





