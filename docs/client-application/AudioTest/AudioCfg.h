/*
 * AudioCfg.h
 *
 *  Created on: 5 Feb 2023
 *      Author: gili_k
 */

#ifndef AUDIOCFG_H_
#define AUDIOCFG_H_

#include <stdbool.h>
#include <stdint.h>
#include "DigitalCore_hpi.h"

//max number of channels received from Host (must be even)
#define HD_MAX_HOST_DSP_CHANNELS    48
//max number of channels received from codecs (must be even)
#define HD_MAX_CODEC_CHANNELS   20
//max number of internal generated tone channel
#define HD_MAX_TONE_CHANNELS        16
//starting channel number for host channel
#define HD_HOST_CHANNELS_BASE   20

#define HD_MAX_CHANNELS (HD_MAX_HOST_DSP_CHANNELS + HD_MAX_CODEC_CHANNELS)

#define HD_ALG_MAX 10

#define HD_NSAMPLES    80

//#define HD_BOOL int

typedef enum _CodecChannel
{
    codecLeft_rx,
    codecRight_rx,
    codecLeft_tx,
    codecRight_tx,
}ECodecChannel;

typedef enum _filterType
{
    filterDcBlock = (1<<0),//HD_FILTER_NAVY_HEADSET
    filterNavyHD = (1<<1)

}EFilterType;

typedef enum _algType
{
    algFilterDcBlock,
    algFilterNavyHD,
    algAec,
    algLast
}EAlgType;

//typedef enum {HD_DAC=0, HD_ADC=1, HD_DIR_MAX=2}HD_CH_DIR;

typedef struct _algPtr
{
   void  (* _initAlg) (uint32_t , uint32_t );
   void  (* _processAlg) (uint32_t , uint32_t , int16_t* , int16_t* );
   void  (* _configAlg) (uint32_t , uint32_t , ...);
   HD_BOOL _enabledChannels[HD_MAX_CHANNELS][HD_DIR_MAX];
}AlgPtr;

typedef struct _AudioCfgDB
{
    HD_BOOL cd_enable[HD_MAX_CHANNELS][HD_DIR_MAX];
    HD_BOOL agc_enable[HD_MAX_CHANNELS][HD_DIR_MAX];
    HD_BOOL filter_enable[HD_MAX_CHANNELS][HD_DIR_MAX];
    HD_BOOL nr_enable[HD_MAX_CHANNELS][HD_DIR_MAX];
    HD_BOOL aec_enable[HD_MAX_CHANNELS];

    unsigned int filter_type[HD_MAX_CHANNELS][HD_DIR_MAX];


}AudioCfgDB;
/*
typedef struct AP_audio_local_ctrl
{
    HD_BOOL loopback_enable[HD_MAX_CODEC_CHANNELS];

    HD_BOOL vocoder_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL vocoder_vad_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL vocoder_fe_enable_mask[HD_MAX_CHANNELS][2];

    HD_BOOL cd_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL agc_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL filter_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL nr_enable_mask[HD_MAX_CHANNELS][2];
    HD_BOOL ec_enable_mask[HD_MAX_CHANNELS];
    HD_BOOL lec_enable_mask[HD_MAX_CHANNELS];
    HD_BOOL caller_id_enable[HD_MAX_CHANNELS];//contain the caller id channel number for every audio channel
    HD_BOOL dtmf_det_enable[HD_MAX_CHANNELS];//contain the dtmf channel number for every audio channel
    HD_BOOL dly_buffer_enable_mask[HD_MAX_CODEC_CHANNELS][2];//enabled in config struct
    HD_BOOL encrypt_enable_mask[HD_MAX_HOST_DSP_CHANNELS][2];
} AP_audio_local_ctrl;

typedef struct AP_vocoder_config
{
    int16_t vad_enable[HD_MAX_CHANNELS][2];
    int16_t frame_erase_enable[HD_MAX_CHANNELS][2];
    int16_t sub_type[HD_MAX_CHANNELS][2];
    int16_t reserved[HD_MAX_CHANNELS][2];
} AP_vocoder_config;
*/
void AudioCfg_Init();

/*  AudioCfg_InitDebug
 *  Basic configuration for audio module
 *  when DSP working stand alone (with no communication to ARM)
 * */
void AudioCfg_InitDebug();

void ProcessChannels(uint32_t dir, int16_t  speech[][HD_NSAMPLES] , uint64_t activeChannels);

void AlgInit(EAlgType algType, uint32_t channel, uint32_t dir);

void AlgInitAll();


/*extern AP_audio_local_ctrl audio_local_ctrl;
extern AP_vocoder_config vocoder_config;*/
#endif /* AUDIOCFG_H_ */
