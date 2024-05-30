/*
 * AudioInProcess.c
 *
 *  Created on: 5 Feb 2023
 *      Author: gili_k
 */

#include <stdint.h>
#include <string.h>

#include "AudioCfg.h"


extern AudioCfgDB _audioCfgDB;

extern AlgPtr  _algPtr[HD_ALG_MAX];
/**************************************************************************/
//  Module      : audio_in_process
//  Description : receive an audio buffer from input channel and preform this
//                  process: VOCODER, DTMF Detect, filter, Noise Reduction
//                  Carrier Detect, AGC.
/// @return none
/// @param  none
/**************************************************************************/


void ProcessChannels(uint32_t dir, int16_t  speech[][HD_NSAMPLES] , uint64_t activeChannels)
{
    int     _channel;
    int         _alg;
    int  _minChannel;
    int  _maxChannel;

    _minChannel = (dir == HD_DAC ? HD_MAX_CODEC_CHANNELS : 0 );

    _maxChannel = (dir == HD_DAC ? HD_MAX_CHANNELS : HD_MAX_CODEC_CHANNELS );

    for(_channel = _minChannel; _channel < _maxChannel; _channel++)
    {
        if(activeChannels & (1<<_channel) == 0)
            continue;

        for(_alg = 0; _alg < HD_ALG_MAX; _alg++)
        {
            if(_algPtr[_alg]._enabledChannels[_channel][dir])
            {
                if(_algPtr[_alg]._processAlg != NULL)
                    _algPtr[_alg]._processAlg(_channel, dir, speech[_channel-_minChannel], speech[_channel-_minChannel]);
            }
        }
    }
}

void AlgInit(EAlgType algType, uint32_t channel, uint32_t dir)
{

    if(_algPtr[algType]._initAlg == NULL)
        return;

    _algPtr[algType]._initAlg(channel, dir);
}

void AlgInitAll()
{

    int     _channel;
    int         _alg;

    for(_channel = 0; _channel < HD_MAX_CHANNELS; _channel++)
    {
        for(_alg = 0; _alg < HD_ALG_MAX; _alg++)
        {
            if(_algPtr[_alg]._enabledChannels[_channel][HD_DAC])
            {
                if(_algPtr[_alg]._initAlg != NULL)
                    _algPtr[_alg]._initAlg(_channel, HD_DAC );
            }

            if(_algPtr[_alg]._enabledChannels[_channel][HD_ADC])
            {
                if(_algPtr[_alg]._initAlg != NULL)
                    _algPtr[_alg]._initAlg(_channel, HD_ADC);
            }
        }
    }
}

void CnfgAlg(uint32_t ch, uint32_t dir )
{

}
