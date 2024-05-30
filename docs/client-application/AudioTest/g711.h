/******************************************************************************/
//                         TechMer
//         		5 Hatzoref str. Holon 58856 ISRAEL
//  Copyright TechMer all rights reserved, confidential proprietary
//
//  Project		: DigitalCore
//  Author		: Ilan Rosilio		
//  Description : g711 implementaion, sun Microsystem code
//				  
///	@file:	g711.h	
//  
// Revision History:
//																
// Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
//------------	-----------  ---------  ---------------------------------------
//Ilan Rosilio	30/Oct/2008   Created	
//
/******************************************************************************/
#ifndef G711_
#define G711_

static int search(int val,int *table,int size);

/*
 * linear2alaw() - Convert a 16-bit linear PCM value to 8-bit A-law
 *
 * linear2alaw() accepts an 16-bit integer and encodes it as A-law data.
 *
 *		Linear Input Code	Compressed Code
 *	------------------------	---------------
 *	0000000wxyza			000wxyz
 *	0000001wxyza			001wxyz
 *	000001wxyzab			010wxyz
 *	00001wxyzabc			011wxyz
 *	0001wxyzabcd			100wxyz
 *	001wxyzabcde			101wxyz
 *	01wxyzabcdef			110wxyz
 *	1wxyzabcdefg			111wxyz
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */
int linear2alaw(int pcm_val);	/* 2's complement (16-bit range) */

/*
 * alaw2linear() - Convert an A-law value to 16-bit linear PCM
 *
 */
int alaw2linear(int	a_val);

/*
 * linear2ulaw() - Convert a linear PCM value to u-law
 *
 * In order to simplify the encoding process, the original linear magnitude
 * is biased by adding 33 which shifts the encoding range from (0 - 8158) to
 * (33 - 8191). The result can be seen in the following encoding table:
 *
 *	Biased Linear Input Code	Compressed Code
 *	------------------------	---------------
 *	00000001wxyza			000wxyz
 *	0000001wxyzab			001wxyz
 *	000001wxyzabc			010wxyz
 *	00001wxyzabcd			011wxyz
 *	0001wxyzabcde			100wxyz
 *	001wxyzabcdef			101wxyz
 *	01wxyzabcdefg			110wxyz
 *	1wxyzabcdefgh			111wxyz
 *
 * Each biased linear code has a leading 1 which identifies the segment
 * number. The value of the segment number is equal to 7 minus the number
 * of leading 0's. The quantization interval is directly available as the
 * four bits wxyz.  * The trailing bits (a - h) are ignored.
 *
 * Ordinarily the complement of the resulting code word is used for
 * transmission, and so the code word is complemented before it is returned.
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */
int linear2ulaw(int pcm_val);	/* 2's complement (16-bit range) */


/*
 * ulaw2linear() - Convert a u-law value to 16-bit linear PCM
 *
 * First, a biased linear code is derived from the code word. An unbiased
 * output can then be obtained by subtracting 33 from the biased code.
 *
 * Note that this function expects to be passed the complement of the
 * original code word. This is in keeping with ISDN conventions.
 */
int ulaw2linear(int u_val);

/* A-law to u-law conversion */
int alaw2ulaw(int aval);

/* u-law to A-law conversion */
int ulaw2alaw(int uval);

/**************************************************************************/                     
//  Module		: g711_test
//  Description : test g711 module 
/// @return none
/// @param	none
/**************************************************************************/
int g711_test(void);

/**************************************************************************/                     
//  Module		: g711_linear_to_ulaw
//  Description : convert block of PCM audio to g711 u-law format 
/// @return none
/// @param	none
/**************************************************************************/
int g711_linear_to_ulaw(char *out,int16_t *in,int size);


/**************************************************************************/                     
//  Module		: g711_linear_to_alaw
//  Description : convert block of PCM audio to g711 a-law format 
/// @return none
/// @param	none
/**************************************************************************/
int g711_linear_to_alaw(char *out,int16_t *in,int size);


/**************************************************************************/                     
//  Module		: g711_ulaw_to_linear
//  Description : convert block of g711 u-law to PCM formt 
/// @return none
/// @param	none
/**************************************************************************/
int g711_ulaw_to_linear(int16_t *out,char *in,int size);


/**************************************************************************/                     
//  Module		: g711_linear_to_ulaw
//  Description : convert block of g711 a-law to PCM formt 
/// @return none
/// @param	none
/**************************************************************************/
int g711_alaw_to_linear(int16_t *out,char *in,int size);




#endif //end G711_

