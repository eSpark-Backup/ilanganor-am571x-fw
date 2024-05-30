/*
 * veuAlg.h
 *
 *  Created on: 13 Feb 2023
 *      Author: gili_k
 */

#ifndef VEUALG_H_
#define VEUALG_H_

void veu_init(uint32_t ch, uint32_t dir);

void veu_process(uint32_t ch, uint32_t dir, int16_t* in, int16_t* out);

void veu_config(uint32_t ch, uint32_t dir, ...);

void veu_config_ch(uint32_t ch, uint32_t dir);

#endif /* VEUALG_H_ */
