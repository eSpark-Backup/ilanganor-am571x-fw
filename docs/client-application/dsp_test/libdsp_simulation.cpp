/*
 * =====================================================================================
 *
 *       Filename:  libdsp_simulation.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/03/2009 12:41:10 PM IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oleg Galbert (OG), oleg_g@mer.co.il
 *        Company:  MER Group
 *
 * =====================================================================================
 */

#include "libdsp.h"

#include <assert.h>
#ifdef _MSC_VER
#include <stdint_msvc.h> // Bring MSVC a little more up to speed with C99
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define usleep(x) Sleep((x) / 1000)
#else
#include <stdint.h> // Use the C99 official header
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#endif // _MSC_VER

#include <string.h>

#include "libdsp.h"
#include "utils/logprintf.h"


// Make this volatile, so that compiler won't optimize it away
/// A string representing the version number of this binary.
/// Should be present verbatim in the binary itself
volatile char __libdsp_versionStr[] = "libDSP " STRINGIFY(LIBDSP_MAJOR) "." \
        STRINGIFY(LIBDSP_MINOR) "." STRINGIFY(LIBDSP_BUILD);

DSP_API const char* LIBDSP_VER = STRINGIFY(LIBDSP_MAJOR) "." STRINGIFY(LIBDSP_MINOR) "." STRINGIFY(LIBDSP_BUILD);


DSP_API void SET_MUX_MUTE( HD_mux_configuration& st ) 
{
	for(int i = 0; i < HD_MAX_CHANNELS; i++) {
		for(int j = 0; j < HD_MAX_CHANNELS; j++) {
			st.channel_level[i][j] = HD_MUX_VOL_MUTE;
		}
	}
	st.reserved = 0;
	st.HD_request = 0;
}

struct G711_frame {
        HD_channel_frame ch_hdr;
        HD_audio_g711_frame ch_body;
};


struct G729_frame {
        HD_channel_frame ch_hdr;
        HD_audio_g729_frame ch_body;
};

#define	CHANNEL_NUM         HD_MAX_HOST_DSP_CHANNELS
#define MAX_FRAME_SIZE		sizeof(G711_frame)
uint8_t sample_audio[80]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static uint8_t packet_from_DSP  [sizeof(HD_channels_frame_size) + CHANNEL_NUM * MAX_FRAME_SIZE]={0};

static unsigned int n_host_channels = CHANNEL_NUM;

static int dsp_state_ = HD_OUT_OF_RESET;

DSP_API int hpi_write_HD_channel_configuration ( HD_channel_configuration* st, int dsp_number )
{
	n_host_channels = st->number_of_host_dsp_channel;
	dsp_state_ = HD_READY;
	return DSPLIB_ERR_OK;
}

template<typename Frame>
void frame2buf(Frame* frame, uint8_t* p)
{
	frame->ch_hdr.channel_index = frame->ch_hdr.channel_index + HD_HOST_CHANNELS_BASE;
	memcpy(p, frame, sizeof(Frame));
}

static void dump_frame_g711(G711_frame* rp)
{
	if(rp == NULL) {
		logprintf("NULL frame\n");
	} else { 
		logprintf("[sz:%d dt:%d ch:%d fc:%u cd:%d au:%d ", 
				rp->ch_hdr.size, rp->ch_hdr.data_type, rp->ch_hdr.channel_index, 
				rp->ch_body.frame_count, rp->ch_body.carrier_detect, rp->ch_body.a_law_u_law);
		logprintf("[%01x %01x %01x %01x ...]\n", 
			rp->ch_body.audio[0],rp->ch_body.audio[1],
			rp->ch_body.audio[2],rp->ch_body.audio[3]);
	}
}

void dsp_set_verbose_print(bool verbose_prints)
{

}

static void dump_channel(uint8_t* data, size_t data_size)
{
	logprintf( "[ ");
	for (size_t i = 0; i < data_size; i++) { logprintf( "%01x ", (int)data[i]); }
	logprintf( " ] of size %u\n", data_size);
}

static void dump_frame_g729(G729_frame* rp)
{
    if(rp == NULL) {
        logprintf("NULL frame\n");
    } else {
        logprintf("[sz:%03d dt:%01d ch:%02d fc:%06d VAD:%01d CC:%01d ",
                rp->ch_hdr.size,rp->ch_hdr.data_type,rp->ch_hdr.channel_index,
				rp->ch_body.frame_count, rp->ch_body.vad_flag, rp->ch_body.carrier_detect);
//		dump_channel(rp->ch_body.audio, sizeof(rp->ch_body.audio));
#if 1
        logprintf("[%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]\n",
            (uint8_t)rp->ch_body.audio[0],(uint8_t)rp->ch_body.audio[1],
            (uint8_t)rp->ch_body.audio[2],(uint8_t)rp->ch_body.audio[3],
            (uint8_t)rp->ch_body.audio[4],(uint8_t)rp->ch_body.audio[5],
            (uint8_t)rp->ch_body.audio[6],(uint8_t)rp->ch_body.audio[7],
            (uint8_t)rp->ch_body.audio[8],(uint8_t)rp->ch_body.audio[9]);
    }
#endif
}

void sim_set_g729_frame(G729_frame& frame, short ch, int VADFlag, int CC, uint8_t* audio, short fc)
{
	frame.ch_hdr.size = sizeof(frame);
	frame.ch_hdr.data_type = HD_G729_FRAME; 
	frame.ch_hdr.channel_index = ch;
	frame.ch_body.frame_count = fc;
    if(audio == NULL) {
		static unsigned char silence_speech_data[] = {0xa8,0xcd,0xc9,0xa0,0x00,0xfa,0xda,0x38,0x01,0xf8};
		frame.ch_body.vad_flag = VADFlag;
		frame.ch_body.carrier_detect = CC;
        memcpy(frame.ch_body.audio, silence_speech_data, sizeof(frame.ch_body.audio));
    } else {
		frame.ch_body.vad_flag = VADFlag;
		frame.ch_body.carrier_detect = CC;
        memcpy(frame.ch_body.audio, audio, sizeof(frame.ch_body.audio));
    }
}

void sim_set_g711_frame(G711_frame& frame, int ch, int fc, int cd, int au, uint8_t* audio)
{
	frame.ch_hdr.size = sizeof(frame);
	frame.ch_hdr.data_type = HD_G711_FRAME;
	frame.ch_hdr.channel_index = ch;
	frame.ch_body.frame_count = fc;
	frame.ch_body.carrier_detect = cd;
	frame.ch_body.a_law_u_law = au;
	if(audio == NULL) {
		int null_value = frame.ch_body.a_law_u_law == HD_A_LAW ? 0xd5: 0xff;
		memset(frame.ch_body.audio, null_value, sizeof(frame.ch_body.audio));
	} else {
		memcpy(frame.ch_body.audio, audio, sizeof(frame.ch_body.audio));
	}
}

int simulate_g729_packets(int no_frames)
{
	G729_frame frame_from_LAN;
	assert(no_frames <= CHANNEL_NUM);
	uint32_t packet_size = sizeof(HD_channels_frame_size) +  no_frames * sizeof(G729_frame);
	((HD_channels_frame_size*)packet_from_DSP)->size = packet_size; 
	((HD_channels_frame_size*)packet_from_DSP)->num_of_frames = no_frames;
	uint8_t* p = packet_from_DSP + sizeof(HD_channels_frame_size);
	uint8_t* right_border = packet_from_DSP + sizeof(packet_from_DSP);

	for (int fn = 0; fn < no_frames; fn++) {
		if( p >= right_border) {
			logprintf("RacDeMux::process_g729_packets(): p:0x%08x >= right_border:0x%08x", p, right_border);
			return packet_size;
		}
		sim_set_g729_frame(frame_from_LAN, fn, 0, 0, NULL, 1111);
		frame2buf(&frame_from_LAN, p);
		p = p + (int)frame_from_LAN.ch_hdr.size;
	} // end of for  			
	return packet_size;
}		


int simulate_g711_packets(int no_frames)
{
	G711_frame frame_from_LAN;
	assert(no_frames <= CHANNEL_NUM);
	uint32_t packet_size = sizeof(HD_channels_frame_size) +  no_frames * sizeof(G711_frame);
	((HD_channels_frame_size*)packet_from_DSP)->size = packet_size; 
	((HD_channels_frame_size*)packet_from_DSP)->num_of_frames = no_frames;

	uint8_t* p = packet_from_DSP + sizeof(HD_channels_frame_size);
	uint8_t* right_border = packet_from_DSP + sizeof(packet_from_DSP);

	for (int fn = 0; fn < no_frames; fn++) {
		if( p >= right_border) {
			logprintf("RacDeMux::process_g711_packets(): p:0x%08x >= right_border:0x%08x", p, right_border);
			return packet_size;
		}
		sim_set_g711_frame(frame_from_LAN, fn, 2222, 0, HD_A_LAW, NULL);
		frame2buf(&frame_from_LAN, p);
		p = p + (int)frame_from_LAN.ch_hdr.size;
	} // end of for  			

	return packet_size;
}

////////////////  Low level hpi_read and hpi_write  //////////////////

DSP_API bool is_hpi_open(int dsp_number)
{
    return true; 
}

DSP_API int _hpi_get_handle(int dsp_number)
{
	return -1;
}

#define HPI_WRITE_STRUCT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_write_##STRUCT_TYPE ( STRUCT_TYPE* st, int dsp_number ) \
{ \
    return DSPLIB_ERR_OK; \
} 

#define HPI_WRITE_STRUCT_NOWAIT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_write_nowait_##STRUCT_TYPE ( STRUCT_TYPE* st, int dsp_number ) \
{ \
    return DSPLIB_ERR_OK; \
} 
 
#define HPI_READ_STRUCT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_read_##STRUCT_TYPE ( STRUCT_TYPE* st, int dsp_number ) \
{ \
	return DSPLIB_ERR_OK; \
} 


#define HPI_WRITE_STRUCT_FUNC_WAIT_IMP( STRUCT_TYPE )\
static int hpi_wait_HD_request_##STRUCT_TYPE() \
{ \
    int rv = DSPLIB_ERR_OK; \
    return rv; \
}
#if 0
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_channel_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_ac_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_ac_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_cd_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_cd_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_cd_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_agc_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_agc_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_agc_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_filter_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_filter_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_vocoder_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_vocoder_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_vocoder_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_nr_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_nr_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_nr_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_mux_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_mux_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_tone_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_tone_mux_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_dtmf_det_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_caller_id_configuration , HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_echo_c_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_echo_c_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_echo_c_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_line_echo_c_initialization, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_line_echo_c_enabling, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_line_echo_c_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_gpio_configuration, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_gpio_status, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_dsp_status, HD_request)
HPI_READ_STRUCT_FUNC_HD_REQUEST_IMP( HD_dsp_bit, HD_request)
#endif
// ------------------------------------------------------------------
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_channel_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_ac_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_ac_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_cd_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_cd_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_cd_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_agc_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_agc_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_agc_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_filter_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_filter_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_vocoder_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_vocoder_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_vocoder_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_nr_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_nr_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_nr_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_mux_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_mux_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_tone_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_tone_mux_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_dtmf_det_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_caller_id_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_echo_c_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_echo_c_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_echo_c_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_line_echo_c_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_line_echo_c_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_line_echo_c_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_gpio_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_gpio_status )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_dsp_status )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_dsp_bit )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_encrypt_initialization )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_encrypt_enabling )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_encrypt_configuration )
HPI_WRITE_STRUCT_FUNC_WAIT_IMP( HD_cd_status )

// -------------------------------------------------------

//HPI_WRITE_STRUCT_FUNC_IMP( HD_channel_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_ac_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_ac_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_cd_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_cd_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_cd_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_agc_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_agc_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_agc_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_filter_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_filter_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_vocoder_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_vocoder_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_vocoder_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_nr_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_nr_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_nr_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_mux_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_mux_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_tone_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_tone_mux_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_dtmf_det_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_caller_id_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_echo_c_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_echo_c_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_echo_c_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_line_echo_c_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_line_echo_c_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_line_echo_c_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_gpio_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_gpio_status )
HPI_WRITE_STRUCT_FUNC_IMP( HD_dsp_bit )
HPI_WRITE_STRUCT_FUNC_IMP( HD_encrypt_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_encrypt_enabling )
HPI_WRITE_STRUCT_FUNC_IMP( HD_encrypt_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_cd_status )

HPI_READ_STRUCT_FUNC_IMP( HD_channel_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_ac_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_ac_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_cd_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_cd_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_cd_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_agc_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_agc_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_agc_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_filter_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_filter_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_vocoder_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_vocoder_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_vocoder_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_nr_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_nr_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_nr_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_mux_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_mux_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_tone_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_tone_mux_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_dtmf_det_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_caller_id_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_echo_c_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_echo_c_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_echo_c_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_line_echo_c_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_line_echo_c_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_line_echo_c_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_gpio_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_gpio_status )
HPI_READ_STRUCT_FUNC_IMP( HD_dsp_bit )
HPI_READ_STRUCT_FUNC_IMP( HD_extension )
HPI_READ_STRUCT_FUNC_IMP( HD_encrypt_initialization )
HPI_READ_STRUCT_FUNC_IMP( HD_encrypt_enabling )
HPI_READ_STRUCT_FUNC_IMP( HD_encrypt_configuration )
HPI_READ_STRUCT_FUNC_IMP( HD_cd_status )
// -------------------------------------------------------
HPI_WRITE_STRUCT_NOWAIT_FUNC_IMP( HD_mux_configuration )
HPI_WRITE_STRUCT_NOWAIT_FUNC_IMP( HD_tone_mux_configuration )

/////////////////////////// End of low level API 

DSP_API int hpi_write_ext_HD_tel_ctrl_ext ( HD_tel_ctrl_ext* , int dsp_number)
{
	return DSPLIB_ERR_OK;
}

/////////////////////////// End of low level API 

int start_DIAL_dtmf_idx;
int start_DIGITs_dtmf_idx;

DSP_API int dsp_init(int dsp_number,int short_tx_queue)
{
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_reset(int dsp_number)
{
	return DSPLIB_ERR_OK;
}

DSP_API int get_arch(enum Arch* arch)
{
    *arch = ArchMACS;
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_fini(int dsp_number)
{
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_version(HD_version_info* version_info, int dsp_number)
{
	version_info->MajorVersionNumber = 1;
	version_info->MinorVersionNumber = 2;
	version_info->BugfixReleaseNumber = 3;
	strcpy((char*)version_info->sDescription, "Simulated libDSP");
	strcpy((char*)version_info->sVendorInfo, "TechMer Ltd., 5 Hatzoref str. Holon 58856 ISRAEL");
	strcpy((char*)version_info->sVersionTag, "Simulation");

    return DSPLIB_ERR_OK;
}

DSP_API int read_dsp_version(bool print_vendor)
{
    int rv = DSPLIB_ERR_OK;
    HD_version_info dsp_info;
    rv = dsp_version(&dsp_info);
    if(rv == DSPLIB_ERR_OK) {
		logprintf("DSP: ver %d.%d.%d\n", dsp_info.MajorVersionNumber,
				dsp_info.MinorVersionNumber, 
				dsp_info.BugfixReleaseNumber);
		if (print_vendor)
			logprintf("DSP: %s\n", dsp_info.sVendorInfo);
		logprintf("DSP: %s\n", dsp_info.sDescription);
		logprintf("DSP: %s\n", dsp_info.sVersionTag);
    }
    return rv;
}

DSP_API int dsp_read_gpio(uint32_t& gpio_data)
{
    gpio_data = 0;
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_read_gpio_data_out(uint32_t& gpio_data)
{
	gpio_data = 0;
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_write_gpio(uint32_t gpio_data)
{
    return DSPLIB_ERR_OK;
}

static int dsp_frame_count = 0;
DSP_API int dsp_read_status(HD_dsp_status* dsp_status, int dsp_number)
{
    // Advance the framecount (to give the watcher an impression that we're alive)
    dsp_status->frame_count = dsp_frame_count++;
    
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_read_state(int dsp_number)
{
    return dsp_state_;
}

DSP_API const char* dsp_status2str(int dsp_state)
{
    static const char* st[] = {"HD_OUT_OF_RESET","HD_POWER_UP_BIT","HD_SYSTEM_CONFIG","HD_READY","HD_INITIATE_BIT"};
    if ( dsp_state >=0 && dsp_state < (int) (sizeof(st)/sizeof(st[0])) ) {
        return st[dsp_state];
    }
    return "UNKNOWN_DSP_STATE";
}

DSP_API int dsp_config_tone(int tone_idx, int freq, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    logprintf("dsp_config_tone(): tone:%d freq:%d\n", tone_idx, freq);
    return rv;
}

DSP_API int dsp_do_tone(int tone_idx, int ch, int volume, bool do_mux_audio, bool wait , int dsp_number) 
{
    int rv = DSPLIB_ERR_OK;
    logprintf("dsp_do_tone(): tone:%d ch:%d volume:%d do_mux_audio:%s\n", 
            tone_idx, ch, volume, do_mux_audio ? "TRUE":"FALSE");
    return rv;
}

DSP_API int dsp_do_tone_silence( int ch , int dsp_number) 
{
    int rv = DSPLIB_ERR_OK;
    return rv;
}

DSP_API int dsp_do_tones(int tone_idx, unsigned int* channels, size_t chanelsz, int volume, bool do_mux_audio, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    return rv;
}

DSP_API int dsp_do_tones_silence( unsigned int* channels, size_t chanelsz , int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    return rv;
}

DSP_API int dsp_do_stereo_tone(int tone_idx, int ch1, int ch2, int volume, bool do_mux_audio, bool wait, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    if ( tone_idx < 0 ) return DSPLIB_ERR_FAIL;
    logprintf("dsp_do_stereo_tone(): tone:%d ch1:%d ch2:%d volume:%d do_mux_audio:%s\n",
            tone_idx, ch1, ch2, volume, do_mux_audio ? "TRUE":"FALSE");
    return rv;
}

DSP_API int dsp_audio_read(uint8_t* data, int size, int dsp_number)
{
    int bytes_read = simulate_g711_packets(n_host_channels);
    usleep(10*1000);
    memcpy(data, packet_from_DSP, bytes_read);
    return bytes_read;
}

DSP_API int dsp_audio_write(uint8_t* data, int size, int dsp_number)
{
    int bytes_write = size;
    return bytes_write;
}

DSP_API int dsp_ac_configuration(HD_CH_IDX channel_number, bool state, 
						bool analog_loopback, bool digital_loopback,
						int powerdown, int adc_gain, int dac_gain, uint16_t codec_reg, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

	logprintf("dsp_ac_configuration(): DigitalCore ch:%d configured"
		" adc:%d dac:%d pwd:%d st:%d\n", 
		channel_number, adc_gain, dac_gain, powerdown, state);

    return rv;
}

DSP_API int dsp_vocoder_configuration(int chid, HD_VOCODER_TYPE vocoder_type, 
		HD_VOCODER_SUBTYPE vocoder_sub_type, bool vad_enable, bool frame_erase, int dsp_number) 
{
    int rv = DSPLIB_ERR_OK;
    logprintf("dsp_vocoder_configuration(): chid:%d vocoder:%d subtype:%d vad:%d\n",
			chid, vocoder_type, vocoder_sub_type, vad_enable);
    return rv;
}

DSP_API int dsp_vocoder_configuration_channel_range(int chid_start, uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	logprintf("dsp_vocoder_configuration_channel_range(): %d channels "
			"from %d\n",
			chid_num, chid_start);
	return ret;
}

DSP_API int dsp_vocoder_config_host_channels_auto(uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase, int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
	logprintf("dsp_vocoder_config_host_channels_auto(): %d channels\n",
			chid_num);
	logprintf("dsp_vocoder_config_host_channels_auto(): "
			"vocoder:%d subtype:%d vad:%d\n",
			vocoder_type, vocoder_sub_type, vad_enable);
	return ret;
}

DSP_API int dsp_vocoder_configuration(int chids[], uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	logprintf("dsp_vocoder_configuration(): %d channels set to vocoder:%d subtype:%d\n",
			chid_num, vocoder_type, vocoder_sub_type);
	return ret;
}

DSP_API int dsp_vocoder_enable(int chid, bool enable, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	logprintf("dsp_vocoder_enable(): chid:%d vocoder: %s\n",
			chid, enable?"ENABLED":"DISABLED");
	return ret;
}

DSP_API int dsp_vocoder_enable_channel_range(int chid_start, uint32_t chid_num, bool enable, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	return ret;
}

DSP_API int dsp_vocoder_enable(int chids[], uint32_t chid_num, bool enable, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	return ret;
}
	
DSP_API int dsp_cd_configuration_channel_range(int chid_start, uint32_t chid_num, 
		int16_t threshold_level, int16_t start_duration, int16_t stop_duration, bool mute, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	return ret;
}

DSP_API int dsp_cd_enable_channel_range(int chid_start, uint32_t chid_num, bool enable, int dsp_number) 
{
	int ret = DSPLIB_ERR_OK;
	return ret;
}

DSP_API int dsp_init_dtmf(int start_idx)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int dsp_init_DIAL_dtmf(int start_idx)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int dsp_init_DIGITs_dtmf(int start_idx)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int dsp_dtmf_busy_tone(int volume, bool do_mux_audio, int channel_1, int channel_2, int channel_3, int channel_4)
{
    int rv = DSPLIB_ERR_OK;
    
    return rv;
}

DSP_API int  dsp_do_dualtone( int tone_1, int tone_2,
		int volume, bool do_mux_audio, int ch1 , int ch2, int ch3, int ch4)
{
    int rv = DSPLIB_ERR_OK;
    logprintf("dsp_do_dualtone(): tone1:%d tone2:%d ch1:%d ch2:%d ch3:%d ch4:%d volume:%d do_mux_audio:%s\n",
            tone_1, tone_2, ch1, ch2, ch3, ch4, volume, do_mux_audio ? "TRUE":"FALSE");
    return rv;
}

DSP_API int  dsp_stop_dtmf( int ch1 , int ch2, int ch3, int ch4)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int dsp_dtmf_tone(char key, int volume, int length, bool do_mux_audio, int channel_1, int channel_2, int channel_3, int channel_4)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int hpi_read_HD_tel_status_ext_by_channel( uint32_t chid, HD_tel_status_ext* st , int dsp_number)
{
	int rv = DSPLIB_ERR_OK;

	st->general_status = 0;

	return rv;
}

DSP_API int hpi_read_HD_generic_ext(unsigned int index, uint32_t struct_offset, char* buffer, size_t buffersz, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}

DSP_API int hpi_write_HD_generic_ext(unsigned int index, uint32_t struct_offset, const char* buffer, size_t buffersz, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

    return rv;
}
