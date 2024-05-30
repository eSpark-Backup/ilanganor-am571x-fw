/*
 * AudioCfg.c
 *
 *  Created on: 12 Feb 2023
 *      Author: gili_k
 */

#include <string.h>

#include "AudioCfg.h"
#include "filter_dc.h"
#include "filter_navy_headset.h"
#include "veuAlg.h"

AudioCfgDB _audioCfgDB;

AlgPtr  _algPtr[HD_ALG_MAX];

void InitAlgData();

void AudioCfg_Init()
{
    InitAlgData();

    memset((void *) &_audioCfgDB, 0, sizeof(AudioCfgDB));

}

void AudioCfg_InitDebug()
{

    AlgInitAll();


}


void InitAlgData()
{
    _algPtr[0]. _initAlg = &filter_dc_init;
    _algPtr[0]._processAlg = &filter_dc;
    _algPtr[0]._configAlg = NULL;


    _algPtr[1]. _initAlg = &filter_navy_headset_init;
    _algPtr[1]._processAlg = &filter_navy_headset;
    _algPtr[1]._configAlg = NULL;


    _algPtr[2]. _initAlg = &veu_init;
    _algPtr[2]._processAlg = &veu_process;
    _algPtr[2]._configAlg = &veu_config;


    /*  FOR DSP STAND ALONE */
    _algPtr[0]._enabledChannels[codecLeft_rx][HD_ADC] = true;
    _algPtr[0]._enabledChannels[codecRight_rx][HD_ADC] = true;
    _algPtr[1]._enabledChannels[codecLeft_rx][HD_ADC] = true;
    _algPtr[1]._enabledChannels[codecRight_rx][HD_ADC] = true;
    _algPtr[2]._enabledChannels[codecLeft_rx][HD_ADC] = true;
    _algPtr[2]._enabledChannels[codecRight_rx][HD_ADC] = true;
  /*  _algPtr[0]._enabledChannels[codecLeft_rx][HD_ADC] = false;
    _algPtr[0]._enabledChannels[codecRight_rx][HD_ADC] = false;
    _algPtr[1]._enabledChannels[codecLeft_rx][HD_ADC] = false;
    _algPtr[1]._enabledChannels[codecRight_rx][HD_ADC] = false;
    _algPtr[2]._enabledChannels[codecLeft_rx][HD_ADC] = false;
    _algPtr[2]._enabledChannels[codecRight_rx][HD_ADC] = false;*/
}

