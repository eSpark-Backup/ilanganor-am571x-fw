/*
 * =====================================================================================
 * 
 *       Filename:  libdsp.h
 * 
 *    Description:  Main header for DSP library
 * 
 *        Version:  1.0
 *        Created:  07/30/2008 02:15:21 PM IDT
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  Oleg Galbert (OG), oleg_g@tech-mer.com
 *        Company:  TechMer Ltd
 * 
 * =====================================================================================
 */

#ifndef  __LIBDSP_INC__
#define  __LIBDSP_INC__

#ifdef _MSC_VER
#include <stdint_msvc.h> // Bring MSVC a little more up to speed with C99
#else
#include <stdint.h> // Use the C99 official header
#include <stdlib.h>
#endif // _MSC_VER

#if (defined(_WIN32) && !defined(UNDER_CE)) || defined(__CYGWIN__)
	#ifdef __BUILD_DLL // DO NOT define this in application
	     #define DSP_API __declspec(dllexport)
	#else
	     #define DSP_API extern __declspec(dllimport)
	#endif // __BUILD_DLL
#else
	#if (__GNUC__ > 3) && defined (__BUILD_DLL)
		#define DSP_API __attribute__ ((visibility("default")))
	#else
		#define DSP_API
	#endif // (__GNUC__ > 3) && defined (__BUILD_DLL)
#endif // (defined(_WIN32) && !defined(UNDER_CE)) || defined(__CYGWIN__)

#pragma pack( push, 1 )
#include "DigitalCore_hpi.h"
#pragma pack( pop )

/// Convert an immediate value to string
#define STRINGIFY(s) TOSTRING(s)

#define SAFE_ALIGNMENT __attribute__((aligned (8)))

/// Helper macro used by STRINGIFY()
#define TOSTRING(s) #s

#define LIBDSP_MAJOR 4
#define LIBDSP_MINOR 4
#define LIBDSP_BUILD HD_INTERFACE_VERSION

#define HPI_CTRL_OFFSET(field)   ((uint32_t)&((HD_host_dsp_ctrl*)HD_HPI_STRUCT_ADDR)->field)
#define HPI_STRUCT_OFFSET( STRUCT_ADDR, STRUCT_TYPE, field)  ((uint32_t)&(( STRUCT_TYPE* )STRUCT_ADDR)->field)

DSP_API extern const char* LIBDSP_VER;

DSP_API extern int start_DIAL_dtmf_idx;
DSP_API extern int start_DIGITs_dtmf_idx;

#define DSPLIB_ERR_NOT_INITIALIZED  -2
#define DSPLIB_ERR_FAIL             -1
#define DSPLIB_ERR_OK                0

#define DSP_TRUE    1
#define DSP_FALSE   0

#define DSP_TRUE_MASK(_ch) (1<<(_ch%32 ))
#define DSP_FALSE_MASK(_ch) (~(1<<(_ch%32 )))

#define DSP_MASK_INDEX(_ch) (_ch/32)

// declaration dependent on DigitalCore_hpi.h
#define DSP_MAX_INFO_CHAR	HD_MAX_INFO_CHAR
#define DSP_MAX_HOST_DSP_CHANNELS HD_MAX_HOST_DSP_CHANNELS
#define DSP_MAX_CODEC_CHANNELS HD_MAX_CODEC_CHANNELS

#define SET_ZERO_STRUCT( var )  memset( &var, 0, sizeof( var ) ) 
#define SET_BIT_TO_VAL( n , v , d ) ( (v)?((d&(1<<n))?(d):(d^(1<<n))):((d&(1<<n))?(d^(1<<n)):(d)) )

//dsp number indicate which dsp we wants to access, by default we access to dsp 1
#define DSP_NUM_1 0
#define DSP_NUM_2 1

DSP_API void SET_MUX_MUTE( HD_mux_configuration& m );

//typedef HD_dsp_status DSP_status;
//typedef HD_version_info	DSP_Version_Info;


#include "dsphpi.h"

///////////////////////////////////   functions ////////////////////////////
DSP_API int get_arch(enum Arch* arch);
DSP_API int dsp_init(int dsp_number=0,int short_tx_queue=0);
DSP_API int dsp_fini(int dsp_number=0);
DSP_API int dsp_reset(int dsp_number=0);
DSP_API bool is_hpi_open(int dsp_number=0);

DSP_API int dsp_version(HD_version_info* version_info,int dsp_number=0);

DSP_API int read_dsp_version(bool print_vendor=true);

DSP_API int read_dsp_ext(HD_tel_ext* ext);
DSP_API int write_dsp_ext(HD_tel_ext* ext);

DSP_API int dsp_read_gpio(uint32_t& gpio_data,int dsp_number=0);
DSP_API int dsp_write_gpio(uint32_t gpio_data,int dsp_number=0);
DSP_API int dsp_read_gpio_data_out(uint32_t& gpio_data,int dsp_number=0);

DSP_API void dsp_set_verbose_print(bool verbose_prints);

// returns dsp_status.dsp_state(>0) or error (<0)
DSP_API int dsp_read_state(int dsp_number=0);
DSP_API int dsp_read_status(HD_dsp_status* dsp_status,int dsp_number=0);
// returns dsp_status.dsp_state as string
DSP_API const char* dsp_status2str(int dsp_state);

// tone_idx starts from 1
DSP_API int dsp_config_tone(int tone_idx, int freq, int dsp_number=0);

// tone_idx starts from 0
DSP_API int dsp_do_tone(int tone_idx, int ch, int volume, bool do_mux_audio, bool wait=true, int dsp_number=0);
DSP_API int dsp_do_tone_silence( int ch, int dsp_number=0);

DSP_API int dsp_do_tones(int tone_idx, unsigned int* channels, size_t chanelsz, int volume, bool do_mux_audio,int dsp_number=0);
DSP_API int dsp_do_tones_silence( unsigned int* channels, size_t chanelsz, int dsp_number=0);

// tone_idx starts from 0
DSP_API int dsp_do_stereo_tone(int tone_idx, int ch1, int ch2, int volume, bool do_mux_audio, bool wait=true, int dsp_number=0);


// DO NOT change order of these (hardcoded in dsp_init_dtmf)

enum DTMF_DIAL_tone_idx {
    INDEX_350Hz,
    INDEX_440Hz,
    INDEX_480Hz,
    INDEX_620Hz,
    INDEX_DIAL_LAST
};

enum DTMF_DIGITs_tone_idx {
	INDEX_697Hz,
	INDEX_770Hz,
	INDEX_852Hz,
	INDEX_941Hz,
	INDEX_1209Hz,
	INDEX_1336Hz,
	INDEX_1477Hz,
	INDEX_1633Hz,
	INDEX_DIGITs_LAST
};

// Initialize dial tone frequencies
// start_idx must start from 1
DSP_API int dsp_init_DIAL_dtmf(int start_idx, int dsp_number=0);

// Initialize dialing related frequencies - for 0-9 and A-D keys
// start_idx must start from 1
DSP_API int dsp_init_DIGITs_dtmf(int start_idx, int dsp_number=0);

// Initialize all dtmf frequencies - equivalent to calling dsp_init_DIGITs_dtmf and dsp_init_DIAL_dtmf
// start_idx must start from 1
DSP_API int dsp_init_dtmf(int start_idx, int dsp_number=0);

DSP_API int dsp_dtmf_dial_tone(int volume, bool do_mux_audio, int channel_1, int channel_2 = -1, int channel_3 = -1, int channel_4 = -1, int dsp_number=0);
DSP_API int dsp_dtmf_busy_tone(int volume, bool do_mux_audio, int channel_1, int channel_2 = -1, int channel_3 = -1, int channel_4 = -1, int dsp_number=0);

DSP_API int dsp_dtmf_tone(char key, int volume, int length, bool do_mux_audio, int channel_1, int channel_2 = -1, int channel_3 = -1, int channel_4 = -1, int dsp_number=0);
DSP_API int  dsp_do_dualtone( int tone_1, int tone_2,
		int volume, bool do_mux_audio, int ch1 , int ch2 = -1, int ch3 = -1, int ch4 = -1, int dsp_number=0);

DSP_API int  dsp_stop_dtmf( int ch1 , int ch2 = -1, int ch3 = -1, int ch4 = -1, int dsp_number=0);


DSP_API int dsp_ac_configuration(HD_CH_IDX channel_number, bool state, 
						bool analog_loopback, bool digital_loopback,
						int powerdown, int adc_gain, int dac_gain, uint16_t codec_reg = 0
						, int dsp_number=0);
// Vocoder
DSP_API int dsp_vocoder_configuration(int chid, HD_VOCODER_TYPE vocoder_type, 
		HD_VOCODER_SUBTYPE vocoder_sub_type, bool vad_enable, bool frame_erase=false, int dsp_number=0);
DSP_API int dsp_vocoder_configuration_channel_range(int chid_start, uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase=false, int dsp_number=0);
DSP_API int dsp_vocoder_configuration(int chids[], uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase=false, int dsp_number=0);
DSP_API int dsp_vocoder_config_host_channels_auto(uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase, int dsp_number=0);

DSP_API int dsp_vocoder_enable(int chid, bool enable, int dsp_number=0);
DSP_API int dsp_vocoder_enable_channel_range(int chid_start, uint32_t chid_num, bool enable, int dsp_number=0);
DSP_API int dsp_vocoder_enable(int chids[], uint32_t chid_num, bool enable, int dsp_number=0);

// Carrier Detection
DSP_API int dsp_cd_configuration_channel_range(int chid_start, uint32_t chid_num, 
		int16_t threshold_level, int16_t start_duration, int16_t stop_duration, bool mute, int dsp_number=0);
DSP_API int dsp_cd_enable_channel_range(int chid_start, uint32_t chid_num, bool enable, int dsp_number=0); 

DSP_API int hpi_read_HD_cd_status_channel(uint16_t *status,uint32_t chid_num,uint32_t dir, int dsp_number=0); 

// Audio
DSP_API int dsp_audio_read(uint8_t* data, int size,int dsp_number=0);
DSP_API int dsp_audio_write(uint8_t* data, int size,int dsp_number=0);

///////////// Extension access API //////////
//

DSP_API int hpi_read_HD_generic_ext(unsigned int index, uint32_t struct_offset, char* buffer, size_t buffersz,int dsp_number=0);
DSP_API int hpi_write_HD_generic_ext(unsigned int index, uint32_t struct_offset, const char* buffer, size_t buffersz,int dsp_number=0);


// HD_tel_ctrl_ext
DSP_API int hpi_read_HD_tel_ctrl_ext( HD_tel_ctrl_ext* st ,int dsp_number=0);
DSP_API int hpi_write_ext_HD_tel_ctrl_ext ( HD_tel_ctrl_ext* st ,int dsp_number=0 );
// HD_tel_status_ext
DSP_API int hpi_read_HD_tel_status_ext_by_channel( uint32_t chid, HD_tel_status_ext* st ,int dsp_number=0);
DSP_API int hpi_write_HD_tel_status_ext_by_channel( uint32_t chid, HD_tel_status_ext* st ,int dsp_number=0);
DSP_API int hpi_read_HD_dly_status_by_channel( uint32_t chid, uint32_t ch_dir,uint16_t* st ,int dsp_number=0);


////////////////////// Very Low level API
DSP_API int _hpi_get_handle(int dsp_number=0);
DSP_API int _hpi_read(int hpi_handle, void* dest, uint32_t offset, uint16_t len);
DSP_API int _hpi_write(int hpi_handle, const void* source, uint32_t offset, uint16_t len);

////////////////////// Low level API 

DSP_API int hpi_write_HD_channel_configuration ( HD_channel_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_ac_initialization ( HD_ac_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_ac_configuration ( HD_ac_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_cd_initialization ( HD_cd_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_cd_enabling ( HD_cd_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_cd_configuration ( HD_cd_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_agc_initialization ( HD_agc_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_agc_enabling ( HD_agc_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_agc_configuration ( HD_agc_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_filter_initialization ( HD_filter_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_filter_enabling ( HD_filter_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_vocoder_initialization ( HD_vocoder_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_vocoder_enabling ( HD_vocoder_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_vocoder_configuration ( HD_vocoder_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_nr_initialization ( HD_nr_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_nr_enabling ( HD_nr_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_nr_configuration ( HD_nr_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_mux_initialization ( HD_mux_initialization* st, int dsp_number=0);
DSP_API int hpi_write_HD_mux_configuration ( HD_mux_configuration* st, int dsp_number=0);
DSP_API int hpi_write_HD_tone_configuration ( HD_tone_configuration* st, int dsp_number=0);
DSP_API int hpi_write_HD_tone_mux_configuration ( HD_tone_mux_configuration* st, int dsp_number=0);
DSP_API int hpi_write_HD_dtmf_det_enabling ( HD_dtmf_det_enabling* st, int dsp_number=0);
DSP_API int hpi_write_HD_caller_id_configuration(HD_caller_id_configuration * st, int dsp_number=0);
DSP_API int hpi_write_HD_echo_c_initialization ( HD_echo_c_initialization* st, int dsp_number=0);
DSP_API int hpi_write_HD_echo_c_enabling ( HD_echo_c_enabling* st, int dsp_number=0);
DSP_API int hpi_write_HD_echo_c_configuration ( HD_echo_c_configuration* st, int dsp_number=0);
DSP_API int hpi_write_HD_line_echo_c_initialization ( HD_line_echo_c_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_line_echo_c_enabling ( HD_line_echo_c_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_line_echo_c_configuration ( HD_line_echo_c_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_gpio_configuration ( HD_gpio_configuration* st,int dsp_number=0);
DSP_API int hpi_write_HD_gpio_status ( HD_gpio_status* st ,int dsp_number=0);
DSP_API int hpi_write_HD_dsp_bit ( HD_dsp_bit* st ,int dsp_number=0);
DSP_API int hpi_write_HD_extension ( HD_extension* st ,int dsp_number=0);
DSP_API int hpi_write_HD_encrypt_initialization ( HD_encrypt_initialization* st ,int dsp_number=0);
DSP_API int hpi_write_HD_encrypt_enabling ( HD_encrypt_enabling* st ,int dsp_number=0);
DSP_API int hpi_write_HD_encrypt_configuration ( HD_encrypt_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_HD_cd_status ( HD_cd_status* st ,int dsp_number=0);
DSP_API int hpi_write_HD_aec_initialization(HD_aec_initialization* st, int dsp_number = 0);
DSP_API int hpi_write_HD_aec_configuration(HD_aec_configuration* st, int dsp_number = 0);
DSP_API int hpi_write_HD_aec_agc_configuration(HD_aec_agc_configuration* st, int dsp_number = 0);
DSP_API int hpi_write_HD_aec_enabling(HD_aec_enabling* st, int dsp_number = 0);

DSP_API int hpi_read_HD_channel_configuration ( HD_channel_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_ac_initialization ( HD_ac_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_ac_configuration ( HD_ac_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_cd_initialization ( HD_cd_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_cd_enabling ( HD_cd_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_cd_configuration ( HD_cd_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_agc_initialization ( HD_agc_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_agc_enabling ( HD_agc_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_agc_configuration ( HD_agc_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_filter_initialization ( HD_filter_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_filter_enabling ( HD_filter_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_vocoder_initialization ( HD_vocoder_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_vocoder_enabling ( HD_vocoder_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_vocoder_configuration ( HD_vocoder_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_nr_initialization ( HD_nr_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_nr_enabling ( HD_nr_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_nr_configuration ( HD_nr_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_mux_initialization ( HD_mux_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_mux_configuration ( HD_mux_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_tone_configuration ( HD_tone_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_tone_mux_configuration ( HD_tone_mux_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_dtmf_det_enabling ( HD_dtmf_det_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_caller_id_configuration(HD_caller_id_configuration * st ,int dsp_number=0);
DSP_API int hpi_read_HD_echo_c_initialization ( HD_echo_c_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_echo_c_enabling ( HD_echo_c_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_echo_c_configuration ( HD_echo_c_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_line_echo_c_initialization ( HD_line_echo_c_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_line_echo_c_enabling ( HD_line_echo_c_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_line_echo_c_configuration ( HD_line_echo_c_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_gpio_configuration ( HD_gpio_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_gpio_status ( HD_gpio_status* st ,int dsp_number=0);
DSP_API int hpi_read_HD_dsp_bit ( HD_dsp_bit* st ,int dsp_number=0);
DSP_API int hpi_read_HD_extension ( HD_extension* st ,int dsp_number=0);
DSP_API int hpi_read_HD_encrypt_initialization ( HD_encrypt_initialization* st ,int dsp_number=0);
DSP_API int hpi_read_HD_encrypt_enabling ( HD_encrypt_enabling* st ,int dsp_number=0);
DSP_API int hpi_read_HD_encrypt_configuration ( HD_encrypt_configuration* st ,int dsp_number=0);
DSP_API int hpi_read_HD_cd_status ( HD_cd_status* st ,int dsp_number=0);


////////////////// No wait variants (do not wait for DSP to complete the operation)
DSP_API int hpi_write_nowait_HD_mux_configuration ( HD_mux_configuration* st ,int dsp_number=0);
DSP_API int hpi_write_nowait_HD_tone_mux_configuration ( HD_tone_mux_configuration* st ,int dsp_number=0);

#endif   /* ----- #ifndef __LIBDSP_INC__  ----- */

