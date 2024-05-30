/******************************************************************************
                         	TechMer Ltd.
                  5, Hatzoref st., Holon 58856 ISRAEL
  Copyright TechMer all rights reserved, confidential proprietary

  Project		: DigitalCore
  Author		: Ilan Rosilio		
  Description : Packet Loss Concealment	implementation - code from ITU-T for G711
  				PLC was changed from flouting point to fixed point  
		 
	@file:	plc_fp.c	
  
 Revision History:
																
 Author name  DD-Mon-YYYY   No./Ver.	Description of Changes
------------	-----------  ---------  ---------------------------------------
Ilan Rosilio	21/Apr/2009	  Created	

  ========                                                              
  HISTORY:                                                              
				$Log$
				ilan_r - ?????? ?????? 18 ???????????? 2013 13:20:44
				ilan_r - Tuesday, April 21, 2009 12:20:46 PM
				add Packet Loss Concealment for G711 and activate it for g729 (frame erase)
				ilan_r - Tuesday, April 21, 2009 7:37:19 AM

******************************************************************************/

/*                                                          24.May.2005 v.1.0
=============================================================================

                          U    U   GGG    SSSS  TTTTT
                          U    U  G       S       T
                          U    U  G  GG   SSSS    T
                          U    U  G   G       S   T
                           UUU     GG     SSS     T

                   ========================================
                    ITU-T - USER'S GROUP ON SOFTWARE TOOLS
                   ========================================


       =============================================================
       COPYRIGHT NOTE: This source code, and all of its derivations,
       is subject to the "ITU-T General Public License". Please have
       it  read  in    the  distribution  disk,   or  in  the  ITU-T
       Recommendation G.191 on "SOFTWARE TOOLS FOR SPEECH AND  AUDIO 
       CODING STANDARDS".
	         ** This code has  (C) Copyright by AT&T Corp. **
       =============================================================


MODULE:         G.711 Appendix I PLC module.

ORIGINAL BY:    AT&T Corp.

FUNCTIONS:

g711plc_construct: ...... LowcFE Constructor.

g711plc_dofe: ........... Generate the synthetic signal.
						  At the beginning of an erasure determine the pitch, and extract
						  one pitch period from the tail of the signal. Do an OLA for 1/4
						  of the pitch to smooth the signal. Then repeat the extracted signal
						  for the length of the erasure. If the erasure continues for more than
						  10 msec, increase the number of periods in the pitchbuffer. At the end
						  of an erasure, do an OLA with the start of the first good frame.
						  The gain decays as the erasure gets longer.

g711plc_addtohistory: ... A good frame was received and decoded.
						  If right after an erasure, do an overlap add with the synthetic signal.
						  Add the frame to history buffer.

HISTORY:

  24.May.05  v1.0  Release of 1st G711 PLC module <AT&T>.
				   Integration of this module in STL2005 <Cyril Guillaumé & Stéphane Ragot - stephane.ragot@francetelecom.com>.
=============================================================================
*/
#include <stdint.h>
#include <math.h>
#include <std.h>
#include "plc_fp.h"
//#include <csl_gpio.h>
//#include "..\BoardUtility\BoardUtility.h" //ilan


static void g711plc_scalespeech(LowcFE_c*, short *out);
static void g711plc_getfespeech(LowcFE_c*, short *out, int sz);
static void g711plc_savespeech(LowcFE_c*, short *s);
static int g711plc_findpitch(LowcFE_c*);
static void g711plc_overlapadd(int16_t *l, int16_t *r, int16_t *o, int cnt);
static void g711plc_overlapaddatend(LowcFE_c*, short *s, short *f, int cnt);
static void g711plc_convertsf(short *f, Float *t, int cnt);
static void g711plc_convertfs(Float *f, short *t, int cnt);
static void g711plc_copyf(Float *f, Float *t, int cnt);
static void g711plc_copys(short *f, short *t, int cnt);
static void g711plc_zeros(short *s, int cnt);
//extern GPIO_Handle hGpio;
int temp_pitch;
void g711plc_construct(LowcFE_c *lc)
{
	lc->erasecnt = 0;
	lc->pitchbufend = &lc->pitchbuf[HISTORYLEN];
	g711plc_zeros(lc->history, HISTORYLEN);
}

/*
 * Get samples from the circular pitch buffer. Update poffset so
 * when subsequent frames are erased the signal continues.
 */
static void g711plc_getfespeech(LowcFE_c *lc, short *out, int sz)
{
	while (sz) {
		int cnt = lc->pitchblen - lc->poffset;
		if (cnt > sz)
			cnt = sz;
		//g711plc_convertfs(&lc->pitchbufstart[lc->poffset], out, cnt);
		memcpy(out,&lc->pitchbufstart[lc->poffset],  cnt*2);
		lc->poffset += cnt;
		if (lc->poffset == lc->pitchblen)
			lc->poffset = 0;
		out += cnt;
		sz -= cnt;
	}
}

static void g711plc_scalespeech(LowcFE_c *lc, short *out)
{
	int	i;
	int32_t res;
	//Float	g = (Float)1. - (lc->erasecnt - 1) * ATTENFAC;
	int16_t	g = 32767 - (lc->erasecnt - 1) * ATTENFAC_FP;
	for (i = 0; i < FRAMESZ; i++) {
		//out[i] = (short)(out[i] * g);
		res = out[i] * g;
		out[i]=res>>15;
		g -= ATTENINCR_FP;
	}
}

/*
 * Generate the synthetic signal.
 * At the beginning of an erasure determine the pitch, and extract
 * one pitch period from the tail of the signal. Do an OLA for 1/4
 * of the pitch to smooth the signal. Then repeat the extracted signal
 * for the length of the erasure. If the erasure continues for more than
 * 10 msec, increase the number of periods in the pitchbuffer. At the end
 * of an erasure, do an OLA with the start of the first good frame.
 * The gain decays as the erasure gets longer.
 */
void g711plc_dofe(LowcFE_c *lc,short *out)
{
	if (lc->erasecnt == 0) {
		 /* get history */
		//g711plc_convertsf(lc->history, lc->pitchbuf, HISTORYLEN);
		memcpy(lc->pitchbuf,lc->history , HISTORYLEN*2);
	//	GPIO4_PIN_SET;	
		lc->pitch = g711plc_findpitch(lc);	/* find pitch */
	//	GPIO4_PIN_CLR;	
		lc->poverlap = lc->pitch >> 2;		/* OLA 1/4 wavelength */
		/* save original last poverlap samples */
		//g711plc_copyf(lc->pitchbufend - lc->poverlap, lc->lastq,
		//	lc->poverlap);
		memcpy(lc->lastq,lc->pitchbufend - lc->poverlap, (lc->poverlap)*2);
		lc->poffset = 0;	/* create pitch buffer with 1 period */
		lc->pitchblen = lc->pitch;
		lc->pitchbufstart = lc->pitchbufend - lc->pitchblen;
		g711plc_overlapadd(lc->lastq, lc->pitchbufstart - lc->poverlap,
			lc->pitchbufend - lc->poverlap, lc->poverlap);
		/* update last 1/4 wavelength in history buffer */
		//g711plc_convertfs(lc->pitchbufend - lc->poverlap,
		//	&lc->history[HISTORYLEN-lc->poverlap], lc->poverlap);
		memcpy(&lc->history[HISTORYLEN-lc->poverlap],lc->pitchbufend - lc->poverlap, (lc->poverlap)*2);
		/* get synthesized speech */
		g711plc_getfespeech(lc, out, FRAMESZ);
	} else if (lc->erasecnt == 1 || lc->erasecnt == 2) {
		/* tail of previous pitch estimate */
		short tmp[POVERLAPMAX];
		int saveoffset = lc->poffset;	/* save offset for OLA */
		/* continue with old pitchbuf */
		g711plc_getfespeech(lc, tmp, lc->poverlap);
		/* add periods to the pitch buffer */
		lc->poffset = saveoffset;
		while (lc->poffset > lc->pitch)
			lc->poffset -= lc->pitch;
		lc->pitchblen += lc->pitch;		/* add a period */
		lc->pitchbufstart = lc->pitchbufend - lc->pitchblen;
		g711plc_overlapadd(lc->lastq, lc->pitchbufstart - lc->poverlap,
			lc->pitchbufend - lc->poverlap, lc->poverlap);
		/* overlap add old pitchbuffer with new */
		g711plc_getfespeech(lc, out, FRAMESZ);
		g711plc_overlapadd(tmp, out, out, lc->poverlap);
		g711plc_scalespeech(lc, out);
	} else if (lc->erasecnt > 5) {
		g711plc_zeros(out, FRAMESZ);
	} else {
		g711plc_getfespeech(lc, out, FRAMESZ);
		g711plc_scalespeech(lc, out);
	}
	lc->erasecnt++;
	g711plc_savespeech(lc, out);
}

/*
 * Save a frames worth of new speech in the history buffer.
 * Return the output speech delayed by POVERLAPMAX.
 */
static void g711plc_savespeech(LowcFE_c *lc, short *s)
{
	/* make room for new signal */
	g711plc_copys(&lc->history[FRAMESZ], lc->history, HISTORYLEN - FRAMESZ);
	/* copy in the new frame */
	g711plc_copys(s, &lc->history[HISTORYLEN - FRAMESZ], FRAMESZ);
	/* copy out the delayed frame */
	g711plc_copys(&lc->history[HISTORYLEN - FRAMESZ - POVERLAPMAX], s,
		FRAMESZ);
}

/*
 * A good frame was received and decoded.
 * If right after an erasure, do an overlap add with the synthetic signal.
 * Add the frame to history buffer.
 */
void g711plc_addtohistory(LowcFE_c *lc, short *s)
{
	if (lc->erasecnt) {
		short overlapbuf[FRAMESZ];
		/*
		 * longer erasures require longer overlaps
		 * to smooth the transition between the synthetic
		 * and real signal.
		 */
		int olen = lc->poverlap + (lc->erasecnt - 1) * EOVERLAPINCR;
		if (olen > FRAMESZ)
			olen = FRAMESZ;
		g711plc_getfespeech(lc, overlapbuf, olen);
		g711plc_overlapaddatend(lc, s, overlapbuf, olen);
		lc->erasecnt = 0;
	}
	g711plc_savespeech(lc, s);
}

/*
 * Overlapp add the end of the erasure with the start of the first good frame
 * Scale the synthetic speech by the gain factor before the OLA.
 */
static void g711plc_overlapaddatend(LowcFE_c *lc, short *s, short *f, int cnt)
{
	int	i;
	int16_t	incrg;
	int16_t	lw, rw;
	int32_t	t;
	
	int16_t	incr = 32767 / cnt;
	int16_t	gain = 32767 - (lc->erasecnt - 1) * ATTENFAC_FP;
	if (gain < 0)
		gain = 0;
	//incrg = incr * gain;
	t=(incr * gain)>>15;
	incrg=t;
	lw = (32767 - incr) * gain;
	t=((32767 - incr) * gain)>>15;
	lw=t;
	rw = incr;
	for (i = 0; i < cnt; i++) {
		t = lw * f[i] + rw * s[i];
		t=t>>15;
		if (t > 32767)
			t = 32767;
		else if (t < -32768)
			t = -32768;
		s[i] = (short)t;
		lw -= incrg;
		rw += incr;
	}
}

/*
 * Overlapp add left and right sides
 */
static void g711plc_overlapadd(int16_t *l, int16_t *r, int16_t *o, int cnt)
{
	int	i;
	int16_t	incr, lw, rw;
	int32_t	 t;

	if (cnt == 0)
		return;
	incr = 32767 / cnt;
	lw = 32767 - incr;
	rw = incr;
	for (i = 0; i < cnt; i++) {
		t = lw * l[i] + rw * r[i];
		t=t>>15;
		if (t > 32767)
			t = 32767;
		else if (t < -32768)
			t = -32768;
		o[i] = t;
		lw -= incr;
		rw += incr;
	}
}


/*
 * Estimate the pitch.
 * l - pointer to first sample in last 20 msec of speech.
 * r - points to the sample PITCH_MAX before l
 */

static int g711plc_findpitch(LowcFE_c *lc)
{
	int	i, j, k;
	int	bestmatch;
	int64_t	bestcorr;
	int32_t	corr;		/* correlation */
	int32_t	energy;		/* running energy */
	int64_t	corr64;		/* correlation */
	int64_t	energy64;		/* running energy */	
	int64_t	scale;		/* scale correlation by average power */
	int16_t	*rp;		/* segment to match */
	int16_t	*l = lc->pitchbufend - CORRLEN;
	int16_t	*r = lc->pitchbufend - CORRBUFLEN;

	/* coarse search */
	rp = r;
	energy = 0;
	corr = 0;
	energy64 = 0;
	corr64 = 0;	

	
	for (i = 0; i < CORRLEN; i += NDEC) {
//		energy64 += rp[i] * rp[i];
		corr64 += rp[i] * l[i];
	}
//	scale = energy64;
//	if (scale < CORRMINPOWER)
//		scale = CORRMINPOWER;
//	corr64 = corr64 / sqrt(scale);
	bestcorr = corr64;
	bestmatch = 0;
	for (j = NDEC; j <= PITCHDIFF; j += NDEC) {
//		energy64 -= rp[0] * rp[0];
//		energy64 += rp[CORRLEN] * rp[CORRLEN];
		rp += NDEC;
		corr64 = 0;
		for (i = 0; i < CORRLEN; i += NDEC)
			corr64 += rp[i] * l[i];
//		scale = energy64;
//		if (scale < CORRMINPOWER)
//			scale = CORRMINPOWER;
//		corr64 /= sqrt(scale);
		if (corr64 >= bestcorr) {
			bestcorr = corr64;
			bestmatch = j;
		}
	}
	/* fine search */
/*	
	j = bestmatch - (NDEC - 1);
	if (j < 0)
		j = 0;
	k = bestmatch + (NDEC - 1);
	if (k > PITCHDIFF)
		k = PITCHDIFF;
	rp = &r[j];
	energy = 0.f;
	corr = 0.f;
	for (i = 0; i < CORRLEN; i++) {
		energy += rp[i] * rp[i];
		corr += rp[i] * l[i];
	}
	scale = energy;
	if (scale < CORRMINPOWER)
		scale = CORRMINPOWER;
	corr = corr / (Float)sqrt(scale);
	bestcorr = corr;
	bestmatch = j;
	for (j++; j <= k; j++) {
		energy -= rp[0] * rp[0];
		energy += rp[CORRLEN] * rp[CORRLEN];
		rp++;
		corr = 0.f;
		for (i = 0; i < CORRLEN; i++)
			corr += rp[i] * l[i];
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		corr = corr / (Float)sqrt(scale);
		if (corr > bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
*/	
	temp_pitch=bestmatch;
	return PITCH_MAX - bestmatch;
}
 
#if 0
static int g711plc_findpitch(LowcFE_c *lc)
{
	int	i, j, k,ii;
	int	bestmatch;
	int64_t	bestcorr;
	int32_t	corr;		/* correlation */
	int64_t	corr64;		/* correlation */
	int32_t	energy;		/* running energy */
	int32_t	scale;		/* scale correlation by average power */
	int16_t	*rp;		/* segment to match */
	int16_t	*l = lc->pitchbufend - CORRLEN;
	int16_t	*r = lc->pitchbufend - CORRBUFLEN;

	/* coarse search */
	rp = r;
	energy = 0;
	corr = 0;
	corr64 = 0;
	for(ii=0;ii<16;ii++)
	{
		for (i = 0; i < (CORRLEN/16); i += NDEC) {
			//energy += rp[i] * rp[i];
			corr += ((rp[ii*16+i] * l[ii*16+i])>>4);//max len=(15+15-4)+5
		}
		
		corr64+=(corr);//max len=31+16
		corr=0;
	}


/*			
	scale = energy;
	if (scale < CORRMINPOWER)
		scale = CORRMINPOWER;
	corr = corr / sqrt(scale);
*/
	bestcorr = corr64;
	bestmatch = 0;
	for (j = NDEC; j <= PITCHDIFF; j += NDEC) {
//		energy -= rp[0] * rp[0];
//		energy += rp[CORRLEN] * rp[CORRLEN];
		rp += NDEC;
		corr = 0;
		corr64 = 0;
		for(ii=0;ii<16;ii++)
		{
			for (i = 0; i < (CORRLEN/16); i += NDEC) {
				corr += ((rp[ii*16+i] * l[ii*16+i])>>4);//max len=(15+15-4)+5
			}
			
			corr64+=(corr);//max len=31+16
			corr=0;
		}
/*		
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		corr /= sqrt(scale);
*/		
		if (corr64 >= bestcorr) {
			bestcorr = corr64;
			bestmatch = j;
		}
	}
	/* fine search */
	/*
	j = bestmatch - (NDEC - 1);
	if (j < 0)
		j = 0;
	k = bestmatch + (NDEC - 1);
	if (k > PITCHDIFF)
		k = PITCHDIFF;
	rp = &r[j];
	energy = 0.f;
	corr = 0.f;
	for (i = 0; i < CORRLEN; i++) {
		energy += rp[i] * rp[i];
		corr += rp[i] * l[i];
	}
	scale = energy;
	if (scale < CORRMINPOWER)
		scale = CORRMINPOWER;
	corr = corr / (Float)sqrt(scale);
	bestcorr = corr;
	bestmatch = j;
	for (j++; j <= k; j++) {
		energy -= rp[0] * rp[0];
		energy += rp[CORRLEN] * rp[CORRLEN];
		rp++;
		corr = 0.f;
		for (i = 0; i < CORRLEN; i++)
			corr += rp[i] * l[i];
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		corr = corr / (Float)sqrt(scale);
		if (corr > bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
	*/
	temp_pitch=bestmatch;
	return PITCH_MAX - bestmatch;
}
#endif

static void g711plc_convertsf(short *f, Float *t, int cnt)
{
	int	i;
	for (i = 0; i < cnt; i++)
		t[i] = (Float)f[i];
}

static void g711plc_convertfs(Float *f, short *t, int cnt)
{
	int	i;
	for (i = 0; i < cnt; i++)
		t[i] = (short)f[i];
}

static void g711plc_copyf(Float *f, Float *t, int cnt)
{
	int	i;
	for (i = 0; i < cnt; i++)
		t[i] = f[i];
}

static void g711plc_copys(short *f, short *t, int cnt)
{
	int	i;
	for (i = 0; i < cnt; i++)
		t[i] = f[i];
}

static void g711plc_zeros(short *s, int cnt)
{
	int	i;
	for (i = 0; i < cnt; i++)
		s[i] = 0;
}
