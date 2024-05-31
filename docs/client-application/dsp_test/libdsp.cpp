/*
 * =====================================================================================
 *
 *       Filename:  libdsp.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/2008 04:52:41 PM IDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oleg Galbert (OG), oleg_g@mer.co.il
 *        Company:  MER Group
 *
 * =====================================================================================
 */

#include <assert.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "libdsp.h"
//#include <utils/misc.h>
//#include <utils/logprintf.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>

#define DSP_MEM
ACE_Thread_Mutex libdsp_mutex_;
// HD_extension cache
static HD_extension _ext_cache[2];
static bool is_ext_cached[2] = {false,false};

// used to block dsp if dsp configuration is needed
static bool libdsp_block[2] = {false,false};


/* Workaround */
#define logprintf printf
#define elements_of(x)  4


// Make this volatile, so that compiler won't optimize it away
/// A string representing the version number of this binary.
/// Should be present verbatim in the binary itself
volatile char __libdsp_versionStr[] = "libDSP " STRINGIFY(LIBDSP_MAJOR) "." \
        STRINGIFY(LIBDSP_MINOR) "." STRINGIFY(LIBDSP_BUILD);

DSP_API const char* LIBDSP_VER = STRINGIFY(LIBDSP_MAJOR) "." STRINGIFY(LIBDSP_MINOR) "." STRINGIFY(LIBDSP_BUILD);

DSP_API void SET_MUX_MUTE( HD_mux_configuration& st ) 
{
//	static __attribute__((aligned (8))) int8_t __mux_mute_table[HD_MAX_CHANNELS * HD_MAX_CHANNELS]={0};
//	memcpy(st.channel_level, __mux_mute_table, HD_MAX_CHANNELS * HD_MAX_CHANNELS);
	for(unsigned int i = 0; i < HD_MAX_CHANNELS; i++) {
		for(unsigned int j = 0; j < HD_MAX_CHANNELS; j++) {
			st.channel_level[i][j] = HD_MUX_VOL_MUTE;
		}
	}
	st.reserved = 0;
	st.HD_request = 0;
}
////////////////  Low level hpi_read and hpi_write  //////////////////

static int _hpi_handle1 = -1;
static int _hpi_handle2 = -1;
static char * dspSharedMem;
static HD_tone_mux_configuration sToneMuxCfg;

static bool print_verbose = true;

#define HPI_HANDLE_NUM 	int _hpi_handle=_hpi_handle1;\
		if (dsp_number==DSP_NUM_2)\
		_hpi_handle=_hpi_handle2

DSP_API void dsp_set_verbose_print(bool verbose_prints)
{
	print_verbose = verbose_prints;
}

DSP_API bool is_hpi_open(int dsp_number)
{
	HPI_HANDLE_NUM;
    return (_hpi_handle != -1);
}

DSP_API int _hpi_get_handle(int dsp_number)
{
	HPI_HANDLE_NUM;
	return _hpi_handle;
}

DSP_API int _hpi_read(int hpi_handle, void* dest, uint32_t offset, uint16_t len)
{
	hpi_ioctl_read_data rd = {0,0,0};
	
	rd.target_buf = (uint32_t) dest;
	rd.len = len;
	rd.dsp_ctrl_offset = offset;
	ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, libdsp_mutex_, -1);
#ifndef DSP_MEM
	int ret = ioctl(hpi_handle, HPICTLREAD, &rd);
#else
       // printf("_hpi_read(): dest: 0x%08x offset: %d len: %d dspSharedMem: 0x%08x\n",dest,offset,len,dspSharedMem);

	int ret= 0;
	memcpy(dest,dspSharedMem+offset,len );
	//munmap(dspSharedMem,size);
#endif

    if(ret < 0) {
        logprintf("_hpi_read(): Error : %s\n", strerror(errno));
        ret = DSPLIB_ERR_FAIL;
    } else {
        ret = DSPLIB_ERR_OK;
    }

    return (ret);    
}

DSP_API int _hpi_write(int hpi_handle, const void* source, uint32_t offset, uint16_t len)
{
    //logprintf("_hpi_write(): source: 0x%08x offset:%d len:%d \n", source, offset, len);
	hpi_ioctl_write_data wd = {0,0,0};
	
    wd.source_buf = (uint32_t) source; 
    wd.len = len;
	wd.dsp_ctrl_offset = offset;
	ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, libdsp_mutex_, -1);
#ifndef DSP_MEM
    int ret = ioctl(hpi_handle, HPICTLWRITE, &wd);
#else
    int ret = 0 ;
    memcpy(dspSharedMem+offset,source,len);
#endif

    if(ret < 0) {
        logprintf("_hpi_write(): Error : %s\n", strerror(errno));
        ret = DSPLIB_ERR_FAIL;
    } else {
        ret = DSPLIB_ERR_OK;
    }
    return (ret);    
}

DSP_API int _cache_dsp_ext(int dsp_number)
{
	int rv = DSPLIB_ERR_OK;
	if(is_ext_cached[dsp_number]) {
		return DSPLIB_ERR_OK;
	} else {
		SET_ZERO_STRUCT(_ext_cache[dsp_number]);
		rv = hpi_read_HD_extension( &_ext_cache[dsp_number] ,dsp_number);
		if( rv != DSPLIB_ERR_OK ) return rv;
		is_ext_cached[dsp_number] = true;
	}
	return rv;
}

static int hpi_wait_HD_request(uint32_t offset,int dsp_number)
{
    int rv;
    HD_REQ HD_request;
    unsigned int count = 50;
    HPI_HANDLE_NUM;
    while (count--) {
        HD_request = 0;
        rv = _hpi_read(_hpi_handle, &HD_request, offset, sizeof(HD_REQ));
        if (rv != DSPLIB_ERR_OK) {
            logprintf("hpi_wait_HD_request(): Error exit: %d\n", rv);
            return rv;
        }
        if (DSP_FALSE == HD_request) // success
            return rv;
        usleep(10 * 1000);
    }
    logprintf("hpi_wait_HD_request(): Error Timeout\n");
    return DSPLIB_ERR_FAIL;
}

#define HPI_WRITE_STRUCT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_write_##STRUCT_TYPE ( STRUCT_TYPE* st ,int dsp_number) \
{ \
	HPI_HANDLE_NUM;\
    st->HD_request = DSP_TRUE; \
    int rv = _hpi_write(_hpi_handle, st, HPI_CTRL_OFFSET(s##STRUCT_TYPE), sizeof( STRUCT_TYPE )); \
    if (rv) return rv; \
    /*logprintf("hpi_write_" #STRUCT_TYPE "(): offset:%d len:%d\n", HPI_CTRL_OFFSET(s##STRUCT_TYPE), sizeof( STRUCT_TYPE ));*/ \
    rv = hpi_wait_HD_request(HPI_CTRL_OFFSET(s##STRUCT_TYPE) + sizeof( STRUCT_TYPE ) - sizeof(HD_REQ),dsp_number); \
    return rv; \
} 

#define HPI_WRITE_STRUCT_NOWAIT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_write_nowait_##STRUCT_TYPE ( STRUCT_TYPE* st,int dsp_number) \
{ \
	HPI_HANDLE_NUM;\
    int rv = hpi_wait_HD_request(HPI_CTRL_OFFSET(s##STRUCT_TYPE) + sizeof( STRUCT_TYPE ) - sizeof(HD_REQ),dsp_number); \
    if (rv) return rv; \
    st->HD_request = DSP_TRUE; \
    return _hpi_write(_hpi_handle, st, HPI_CTRL_OFFSET(s##STRUCT_TYPE), sizeof( STRUCT_TYPE )); \
} 

#define HPI_READ_STRUCT_FUNC_IMP( STRUCT_TYPE )\
DSP_API int hpi_read_##STRUCT_TYPE ( STRUCT_TYPE* st ,int dsp_number ) \
{ \
	HPI_HANDLE_NUM;\
    int rv = _hpi_read(_hpi_handle, st, HPI_CTRL_OFFSET(s##STRUCT_TYPE), sizeof( STRUCT_TYPE )); \
    /*logprintf("hpi_read_" #STRUCT_TYPE "(): offset:%d len:%d\n", HPI_CTRL_OFFSET(s##STRUCT_TYPE), sizeof( STRUCT_TYPE ));*/\
    return rv; \
} 

DSP_API int hpi_read_HD_tel_ctrl_ext( HD_tel_ctrl_ext* st ,int dsp_number)
{ 
	HPI_HANDLE_NUM;
	_cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_TEL_EXT_POS];
//	logprintf("hpi_read_ext_HD_tel_ctrl_ext(): addr: 0x%08x type:%d "
//			"HPI ctrl OFFSET: 0x%08x sz_HD_tel_ctrl_ext:%d\n", 
//			addr, _ext_cache.type[HD_TEL_EXT_POS], 
//			HPI_STRUCT_OFFSET(addr, HD_tel_ext, ctrl), 
//			sizeof( HD_tel_ctrl_ext ));
	if( !(addr && _ext_cache[dsp_number].type[HD_TEL_EXT_POS] == HD_TEL_EXT) ) return DSPLIB_ERR_FAIL;
    int rv = _hpi_read(_hpi_handle, st, HPI_STRUCT_OFFSET(addr, HD_tel_ext, ctrl), sizeof(HD_tel_ctrl_ext));
    return rv; 
} 

DSP_API int hpi_read_HD_request_HD_tel_ctrl_ext(HD_REQ& REQ_VALUE,int dsp_number)
{
	int rv = DSPLIB_ERR_OK;
	HPI_HANDLE_NUM;
	 _cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_TEL_EXT_POS];
	if( !(addr && _ext_cache[dsp_number].type[HD_TEL_EXT_POS] == HD_TEL_EXT) ) return DSPLIB_ERR_FAIL;
	uint32_t offset = HPI_STRUCT_OFFSET(addr,HD_tel_ext, ctrl) + sizeof( HD_tel_ctrl_ext) - sizeof(HD_REQ);
    rv = _hpi_read(_hpi_handle, &( REQ_VALUE ), offset, sizeof(HD_REQ));
    return rv; 
}

DSP_API int hpi_read_HD_generic_ext(unsigned int index, uint32_t struct_offset, char* buffer, size_t buffersz,int dsp_number)
{
	HPI_HANDLE_NUM;
	int rv = DSPLIB_ERR_OK; 
	if(index >= HD_MAX_EXT_POS) return DSPLIB_ERR_FAIL;
	_cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[index];
	if( !addr ) return DSPLIB_ERR_FAIL;
	rv = _hpi_read(_hpi_handle, buffer, addr + struct_offset, buffersz);
	return rv; 
}

DSP_API int hpi_write_HD_generic_ext(unsigned int index, uint32_t struct_offset, const char* buffer, size_t buffersz,int dsp_number)
{
	HPI_HANDLE_NUM;
	if(index >= HD_MAX_EXT_POS) return DSPLIB_ERR_FAIL;

	HD_REQ* req = (HD_REQ*)(buffer + buffersz);
	--req; // make it point to the request field in the end of the structure
	*req = DSP_TRUE;

	_cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[index];

	if( !addr ) return DSPLIB_ERR_FAIL;
	addr += struct_offset;
	int rv = _hpi_write(_hpi_handle, buffer, addr, buffersz);

        rv = hpi_wait_HD_request(addr + buffersz - sizeof(HD_REQ),dsp_number);
	return rv;
}

DSP_API int hpi_wait_HD_request_HD_tel_ctrl_ext(int dsp_number)
{ 
    int rv = DSPLIB_ERR_OK;

    HD_REQ HD_request;
    unsigned int count = 50;
    while (count--) {
        HD_request = 0;
        rv =  hpi_read_HD_request_HD_tel_ctrl_ext(HD_request,dsp_number);
        if (rv != DSPLIB_ERR_OK) {
            logprintf("hpi_wait_HD_request_HD_tel_ctrl_ext(): Error exit: %d\n", rv); 
            return rv;
        }
        if (HD_request == DSP_FALSE)
            return rv;
        usleep(10 * 1000);
    }
    logprintf("hpi_wait_HD_request_HD_tel_ctrl_ext(): Error Timeout\n"); 
    return DSPLIB_ERR_FAIL;
}


DSP_API int hpi_write_ext_HD_tel_ctrl_ext ( HD_tel_ctrl_ext* st,int dsp_number )
{ 
	HPI_HANDLE_NUM;
    st->HD_request = DSP_TRUE; 
	_cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_TEL_EXT_POS];
//	logprintf("hpi_write_ext_HD_tel_ctrl_ext(): addr: 0x%08x type:%d "
//			"HPI ctrl OFFSET: 0x%08x sz_HD_tel_ctrl_ext:%d\n", 
//			addr, _ext_cache.type[HD_TEL_EXT_POS], 
//			HPI_STRUCT_OFFSET(addr, HD_tel_ext, ctrl), 
//			sizeof( HD_tel_ctrl_ext ));
	if( !(addr && _ext_cache[dsp_number].type[HD_TEL_EXT_POS] == HD_TEL_EXT) ) return DSPLIB_ERR_FAIL;
	int rv = _hpi_write(_hpi_handle, st, HPI_STRUCT_OFFSET(addr, HD_tel_ext, ctrl), sizeof( HD_tel_ctrl_ext ));
//	logprintf("hpi_write_ext_HD_tel_ctrl_ext(): Waiting for HD_request\n");
    rv = hpi_wait_HD_request_HD_tel_ctrl_ext(dsp_number);
	return rv; 
} 


DSP_API int hpi_read_HD_tel_status_ext_by_channel( uint32_t chid, HD_tel_status_ext* st ,int dsp_number)
{ 
	HPI_HANDLE_NUM;
	if(chid >= HD_MAX_CODEC_CHANNELS) return DSPLIB_ERR_FAIL; 
	 _cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_TEL_EXT_POS];
//	logprintf("hpi_read_HD_tel_status_ext_by_channel(): chid: %d addr: 0x%08x type:%d "
//			"HPI status OFFSET: 0x%08x sz_HD_tel_ctrl_ext:%d\n",chid, 
//			addr, _ext_cache.type[HD_TEL_EXT_POS], 
//			(HPI_STRUCT_OFFSET(addr, HD_tel_ext, status) + chid), 
//			sizeof( HD_tel_status_ext ));
	if( !(addr && _ext_cache[dsp_number].type[HD_TEL_EXT_POS] == HD_TEL_EXT) ) return DSPLIB_ERR_FAIL;
    int rv = _hpi_read(_hpi_handle, st, 
			(HPI_STRUCT_OFFSET(addr, HD_tel_ext, status) + chid*sizeof(HD_tel_status_ext)), 
			sizeof(HD_tel_status_ext));
    return rv; 
} 

DSP_API int hpi_write_HD_tel_status_ext_by_channel( uint32_t chid, HD_tel_status_ext* st ,int dsp_number)
{ 
	HPI_HANDLE_NUM;
	if(chid >= HD_MAX_CODEC_CHANNELS) return DSPLIB_ERR_FAIL; 
	_cache_dsp_ext(dsp_number);
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_TEL_EXT_POS];
	if( !(addr && _ext_cache[dsp_number].type[HD_TEL_EXT_POS] == HD_TEL_EXT) ) return DSPLIB_ERR_FAIL;
	int rv = _hpi_write(_hpi_handle, st, 
			HPI_STRUCT_OFFSET(addr, HD_tel_ext, status) + chid*sizeof(HD_tel_status_ext), 
			sizeof( HD_tel_status_ext ));
	return rv; 
} 

DSP_API int hpi_read_HD_dly_status_by_channel( uint32_t chid, uint32_t ch_dir, uint16_t *st,int dsp_number)
{ 
	HPI_HANDLE_NUM;
	if(chid >= HD_MAX_CODEC_CHANNELS) return DSPLIB_ERR_FAIL; 
	 _cache_dsp_ext(dsp_number); 
	uint32_t addr = _ext_cache[dsp_number].buffer_addr[HD_DLY_EXT_POS];

	if( !(addr && _ext_cache[dsp_number].type[HD_DLY_EXT_POS] == HD_DLY_EXT) ) return DSPLIB_ERR_FAIL;
    	int rv = _hpi_read(_hpi_handle, st, 
			(HPI_STRUCT_OFFSET(addr, HD_dly_ext, status) + (2*chid+ch_dir)*sizeof(uint16_t)), 
			sizeof(uint16_t)); 
	//ndlogprintf(" %d, %d\n",chid,*st);
    	return rv; 
} 
// -------------------------------------------------------

HPI_WRITE_STRUCT_FUNC_IMP( HD_channel_configuration )
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
HPI_WRITE_STRUCT_FUNC_IMP( HD_aec_initialization )
HPI_WRITE_STRUCT_FUNC_IMP( HD_aec_configuration )
HPI_WRITE_STRUCT_FUNC_IMP( HD_aec_enabling)
HPI_WRITE_STRUCT_FUNC_IMP( HD_aec_agc_configuration)

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

int start_DIAL_dtmf_idx;
int start_DIGITs_dtmf_idx;

static HD_tone_configuration sToneCfg;

struct ToneConf
{
	unsigned int idx;
	unsigned int freq;
};

DSP_API int dsp_init(int dsp_number,int short_tx_queue)
{
    static const char* dev_name = "/dev/mem";
    static const char* dev_name2= "/dev/dsphpi2";
    uint32_t tmp;
    printf("%s \n",dev_name);
//    SLOG_DEBUG("dsplib: %s",dev_name);

    libdsp_block[dsp_number] = false;
    if (dsp_number==DSP_NUM_2)
    {
    	_hpi_handle2 = ::open(dev_name2, O_RDWR);
        if ( _hpi_handle2 == -1 ) {
            logprintf("dsp_init(): Error open \"%s\": %s\n", dev_name2, strerror(errno));
            _hpi_handle2 = 0;
            return  DSPLIB_ERR_FAIL;
        }
        if (short_tx_queue)
        {
        	logprintf("dsp_init(): configure short tx queue (ARM to DSP) for  DSP2\n");
        	::ioctl(_hpi_handle2, HPICTLSHORTTXQUE, &tmp);
        }        
    }
    else
    {
        _hpi_handle1 = ::open(dev_name, O_RDWR);
        if ( _hpi_handle1 == -1 ) {
            logprintf("dsp_init(): Error open \"%s\": %s\n", dev_name, strerror(errno));
            _hpi_handle1 = 0;
            return  DSPLIB_ERR_FAIL;
        }
#ifndef DSP_MEM

        if (short_tx_queue)
        {
        	logprintf("dsp_init(): configure short tx queue (ARM to DSP) for  DSP1\n");
        	::ioctl(_hpi_handle1, HPICTLSHORTTXQUE, &tmp);
        }
#else
        unsigned int base = 0x8D000000;//0x40500000;
        int size= sizeof(HD_host_dsp_ctrl);
        if ((dspSharedMem = (char*) mmap(0,size,PROT_READ|PROT_WRITE, MAP_SHARED, _hpi_handle1,
                                base)) == (caddr_t) -1) {
            printf("Error while maping memory\n");
            return -2;
        }
        printf("sizeof(HD_host_dsp_ctrl) %d  dspSharedMem: 0x%08x\n",size, &dspSharedMem[0]);
#endif
    }
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_reset(int dsp_number)
{
    uint32_t tmp;
    HPI_HANDLE_NUM;
    return ::ioctl(_hpi_handle, HPICTLRESETDSP, &tmp);
}

DSP_API int get_arch(enum Arch* arch,int dsp_number)
{
    uint32_t tmp;
    int ret;
    HPI_HANDLE_NUM;
    ret = ::ioctl(_hpi_handle, HPICTLGETARCH, &tmp);
    if (0 == ret)
        *arch = (enum Arch)tmp;
    return ret;
}

DSP_API int dsp_fini(int dsp_number)
{
    HPI_HANDLE_NUM;	
    ::close(_hpi_handle);
    return DSPLIB_ERR_OK;
}

DSP_API int dsp_version(HD_version_info* version_info,int dsp_number)
{
    HPI_HANDLE_NUM;
    HD_version_info hd_vi;


    int rv = _hpi_read(_hpi_handle, &hd_vi, HPI_CTRL_OFFSET(sHD_version_info), sizeof(HD_version_info));
    //logprintf("dsp_version():%d:%d:[%s]\n", sizeof(HD_version_info), HPI_CTRL_OFFSET(sHD_version_info), (char*)&hd_vi);
    if (rv == DSPLIB_ERR_OK) {
        memcpy((void*)version_info, (void*)&hd_vi, sizeof(HD_version_info));
    }
    return rv;
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
//			ndlogprintf("\tDSP: %s\n", dsp_info.sVendorInfo);
//		ndlogprintf("\tDSP: %s\n\tDSP: %s\n\tDSP: interface %d\n",
//			dsp_info.sDescription,
//			dsp_info.sVersionTag,
//			dsp_info.hpi_interface_version);
		if (dsp_info.hpi_interface_version != HD_INTERFACE_VERSION) {
			logprintf("!!!!DSP interface mismatch: %d instead of %d\n", dsp_info.hpi_interface_version, HD_INTERFACE_VERSION);
			return DSPLIB_ERR_FAIL;
		}
    }
    return rv;
}

#if 0
int read_dsp_ext(HD_tel_ext* ext)
{
	int rv = DSPLIB_ERR_OK;
	if(ext == NULL) return DSPLIB_ERR_FAIL;
	HD_extension st;
	SET_ZERO_STRUCT(st);
	rv = hpi_read_HD_extension( &st );
	if( rv != DSPLIB_ERR_OK ) return rv;
	uint32_t ext_addr = st.buffer_addr[HD_TEL_EXT_POS];
	if( !ext_addr ) return DSPLIB_ERR_FAIL;
	uint32_t ext_type = st.type[HD_TEL_EXT_POS];
	if( (HD_EXTENSION_TYPE)ext_type != HD_TEL_EXT) return DSPLIB_ERR_FAIL;
	ext = (HD_tel_ext*) ext_addr;
	return rv;
}

int write_dsp_ext(HD_tel_ext* ext)
{
	int rv = DSPLIB_ERR_OK;
	if(ext == NULL) return DSPLIB_ERR_FAIL;
	HD_extension st;
	SET_ZERO_STRUCT(st);
	rv = hpi_read_HD_extension( &st );
	if( rv != DSPLIB_ERR_OK ) return rv;
	if( (st.buffer_addr[HD_TEL_EXT_POS]) && 
			((HD_EXTENSION_TYPE) st.type[HD_TEL_EXT_POS] == HD_TEL_EXT)) {
		memcpy((HD_tel_ext*)st.buffer_addr[HD_TEL_EXT_POS], ext, sizeof(HD_tel_ext));
		rv = hpi_write_HD_extension(&st);
	} else {
		return DSPLIB_ERR_FAIL;
	}
	return rv;
}
#endif

DSP_API int dsp_read_gpio(uint32_t& gpio_data,int dsp_number)
{
    HPI_HANDLE_NUM;
    HD_gpio_status hd_st;
    int rv = _hpi_read(_hpi_handle, &hd_st, HPI_CTRL_OFFSET(sHD_gpio_status), sizeof(HD_gpio_status));
    //logprintf("dsp_read_gpio():%d:%d:0x%08x\n", sizeof(HD_gpio_status), HPI_CTRL_OFFSET(sHD_gpio_status), hd_st.data_in);
    if (rv == DSPLIB_ERR_OK) {
        gpio_data = hd_st.data_in;
    } 
    return rv;
}

DSP_API int dsp_read_gpio_data_out(uint32_t& gpio_data,int dsp_number)
{
    HPI_HANDLE_NUM;
    HD_gpio_status hd_st;
    int rv = _hpi_read(_hpi_handle, &hd_st, HPI_CTRL_OFFSET(sHD_gpio_status), sizeof(HD_gpio_status));
    //logprintf("dsp_read_gpio():%d:%d:0x%08x\n", sizeof(HD_gpio_status), HPI_CTRL_OFFSET(sHD_gpio_status), hd_st.data_in);
    if (rv == DSPLIB_ERR_OK) {
        gpio_data = hd_st.data_out;
    } 
    return rv;
}

DSP_API int dsp_write_gpio(uint32_t gpio_data,int dsp_number)
{
    HPI_HANDLE_NUM;
    HD_gpio_status hd_st;
    //logprintf("dsp_write_gpio():%d:%d:0x%08x\n", sizeof(HD_gpio_status), HPI_CTRL_OFFSET(sHD_gpio_status), hd_st.data_out);
    hd_st.data_in = 0;
    hd_st.data_out = gpio_data;
    hd_st.HD_request = DSP_TRUE;
    int rv = _hpi_write(_hpi_handle, &hd_st, HPI_CTRL_OFFSET(sHD_gpio_status), sizeof(HD_gpio_status));
    return rv;
}

#if 0
string dsp_status_as_string(DSP_status* st) 
{
    char buf[320]= {0};
    sprintf(buf, "codec_sync_lost:%d "
    "codec_detected_mcbsp0:%d "
    "codec_detected_mcbsp1:%d "
    "codec_detected_i2c0:%d "
    "codec_detected_i2c1:%d "
    "dsp_state:%d "
    "dsp_error0:%d "
    "dsp_error1:%d",
    st->codec_sync_lost, st->codec_detected_mcbsp0, st->codec_detected_mcbsp1, 
    st->codec_detected_i2c0, st->codec_detected_i2c1, (int) st->dsp_state, st->dsp_error[0],  st->dsp_error[1]);
    printf("*** %s\n", buf);
    return string(buf);
}
#endif

DSP_API int dsp_read_status(HD_dsp_status* dsp_status,int dsp_number)
{
    HPI_HANDLE_NUM;
    int rv = _hpi_read(_hpi_handle, dsp_status, HPI_CTRL_OFFSET(sHD_dsp_status), sizeof(HD_dsp_status));
    //logprintf("dsp_read_status(): offset:%d len:%d\n", HPI_CTRL_OFFSET(sHD_dsp_status), sizeof(HD_dsp_status));

    return rv;
}

// returns dsp_status.dsp_state(>0) or error (<0)
DSP_API int dsp_read_state(int dsp_number)
{
    HD_dsp_status dsp_status;
    SET_ZERO_STRUCT( dsp_status );
    int rv = dsp_read_status(&dsp_status,dsp_number);
    if (print_verbose)
        logprintf("dsp_read_state(): %s:%d\n",dsp_status2str(dsp_status.dsp_state),dsp_status.dsp_state);
    //dsp_status_as_string(&dsp_status); 
    if (rv == DSPLIB_ERR_OK) {
        rv = dsp_status.dsp_state;
    }
    return rv;
}

DSP_API const char* dsp_status2str(int dsp_state)
{
    static const char* st[] = {"HD_OUT_OF_RESET","HD_POWER_UP_BIT","HD_SYSTEM_CONFIG","HD_READY","HD_INITIATE_BIT"};
    if ( dsp_state >=0 && dsp_state < (int) (sizeof(st)/sizeof(st[0])) ) {
        return st[dsp_state];
    }
    return "UNKNOWN_DSP_STATE";
}

DSP_API int dsp_config_tone(int tone_idx, int freq,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    if (print_verbose)
        logprintf("dsp_config_tone(): tone:%d freq:%d\n", tone_idx, freq);
    if ( tone_idx < 1 || freq < 0) return DSPLIB_ERR_FAIL;
    // here we define the tones
    // at the time of config the index have to be -1 from the tone number
    sToneCfg.freq_hz[ tone_idx - 1 ] = freq; 
    sToneCfg.HD_request=DSP_TRUE;
 
    rv = hpi_write_HD_tone_configuration(&sToneCfg,dsp_number);
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_config_tone(): Fail to set tone:%d freq:%d: %d\n", tone_idx, freq, rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_do_tone(int tone_idx, int ch, int volume, bool do_mux_audio, bool wait,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    if ( tone_idx < 0 ) return DSPLIB_ERR_FAIL;
    if (print_verbose)
        logprintf("dsp_do_tone(): tone:%d [%d Hz] ch:%d volume:%d do_mux_audio:%s\n", 
            tone_idx, sToneCfg.freq_hz[ tone_idx - 1], ch, volume, do_mux_audio ? "TRUE":"FALSE");
    int tone_mode = do_mux_audio ? HD_TONE_AUDIO_MUX: HD_TONE_ONLY_MUX;

    sToneMuxCfg.sum_to_speech[ch] = tone_mode;
    sToneMuxCfg.volume[ch] = volume; // 1 the max volume, the more the less
    sToneMuxCfg.tones_number_a[ ch ] = tone_idx;
    sToneMuxCfg.tones_number_b[ ch ] = 0;
    sToneMuxCfg.HD_request= DSP_TRUE;
#if 0
    struct timeval tv_s;
    gettimeofday(&tv_s, NULL);
#endif
    if (wait)
        rv = hpi_write_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
    else
        rv = hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
#if 0
    struct timeval tv_e;
    gettimeofday(&tv_e, NULL);
    logprintf("dsp_do_tone(): tone:%d ch:%d take: %d.%d sec\n", tone_idx, ch, 
            tv_e.tv_sec - tv_s.tv_sec, tv_e.tv_usec - tv_s.tv_usec);
#endif
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_do_tone(): Fail to generate tone:%d: %d\n", tone_idx, rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_do_tone_silence( int ch ,int dsp_number)
{
    sToneMuxCfg.sum_to_speech[ch] = HD_TONE_ONLY_MUX;
    sToneMuxCfg.volume[ch] = 0;
    sToneMuxCfg.tones_number_a[ ch ] = 0;
    sToneMuxCfg.tones_number_b[ ch ] = 0;
    sToneMuxCfg.HD_request= DSP_TRUE;
    return hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
}

DSP_API int dsp_do_tones(int tone_idx, unsigned int* channels, size_t chanelsz, int volume, bool do_mux_audio,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    if ( tone_idx < 0 ) return DSPLIB_ERR_FAIL;
    if (print_verbose)
        logprintf("dsp_do_tones(): tone:%d [%d Hz] volume:%d do_mux_audio:%s\n", 
            tone_idx, sToneCfg.freq_hz[ tone_idx - 1], volume, do_mux_audio ? "TRUE":"FALSE");
    int tone_mode = do_mux_audio ? HD_TONE_AUDIO_MUX: HD_TONE_ONLY_MUX;

    unsigned int i;
    for (i = 0; i < chanelsz; ++i)
    {
        sToneMuxCfg.sum_to_speech[channels[i]] = tone_mode;
        sToneMuxCfg.volume[channels[i]] = volume; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[channels[i]] = tone_idx;
        sToneMuxCfg.tones_number_b[channels[i]] = 0;
    }

    sToneMuxCfg.HD_request = DSP_TRUE;

    rv = hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);

    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_do_tones(): Fail to generate tone:%d: %d\n", tone_idx, rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_do_tones_silence( unsigned int* channels, size_t chanelsz ,int dsp_number)
{
    unsigned int i;
    for (i = 0; i < chanelsz; ++i)
    {
        sToneMuxCfg.sum_to_speech[channels[i]] = HD_TONE_ONLY_MUX;
        sToneMuxCfg.volume[channels[i]] = 0;
        sToneMuxCfg.tones_number_a[channels[i]] = 0;
        sToneMuxCfg.tones_number_b[channels[i]] = 0;
    }
    sToneMuxCfg.HD_request= DSP_TRUE;
    return hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
}

DSP_API int dsp_do_stereo_tone(int tone_idx, int ch1, int ch2, int volume, bool do_mux_audio, bool wait,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;
    if ( tone_idx < 0 ) return DSPLIB_ERR_FAIL;
    if (print_verbose)
        logprintf("dsp_do_stereo_tone(): tone:%d [%d Hz] ch1:%d ch2:%d volume:%d do_mux_audio:%s\n",
            tone_idx, sToneCfg.freq_hz[ tone_idx - 1], ch1, ch2, volume, do_mux_audio ? "TRUE":"FALSE");
    int tone_mode = do_mux_audio ? HD_TONE_AUDIO_MUX: HD_TONE_ONLY_MUX;

    sToneMuxCfg.sum_to_speech[ch1] = tone_mode;
    sToneMuxCfg.sum_to_speech[ch2] = tone_mode;
    sToneMuxCfg.volume[ch1] = volume; // 1 the max volume, the more the less
    sToneMuxCfg.volume[ch2] = volume; // 1 the max volume, the more the less
    sToneMuxCfg.tones_number_a[ ch1 ] = tone_idx;
    sToneMuxCfg.tones_number_a[ ch2 ] = tone_idx;
    sToneMuxCfg.HD_request= DSP_TRUE;
#if 0
    struct timeval tv_s;
    gettimeofday(&tv_s, NULL);
#endif
    if (wait)
        rv = hpi_write_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
    else
        rv = hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);
#if 0
    struct timeval tv_e;
    gettimeofday(&tv_e, NULL);
    logprintf("dsp_do_tone(): tone:%d ch:%d take: %d.%d sec\n", tone_idx, ch,
            tv_e.tv_sec - tv_s.tv_sec, tv_e.tv_usec - tv_s.tv_usec);
#endif
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_do_stereo_tone(): Fail to generate tone:%d: %d\n", tone_idx, rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

////////////////// DSP Configuration //////////////////////////
DSP_API int dsp_ac_configuration(HD_CH_IDX channel_number, bool state, 
						bool analog_loopback, bool digital_loopback,
						int powerdown, int adc_gain, int dac_gain, uint16_t codec_reg,int dsp_number)
{
    int ret_val;  
	HD_ac_configuration st;
	SET_ZERO_STRUCT( st );
	//ac configuration radio codec aic24
	st.channels_mask[channel_number]=state;
	st.analog_loopback = analog_loopback;
	st.digital_loopback = digital_loopback;
	st.powerdown = powerdown;
	st.adc_gain = adc_gain;
	st.dac_gain = dac_gain;
	st.codec_reg = codec_reg;
	st.HD_request = DSP_TRUE;

	ret_val = hpi_write_HD_ac_configuration(&st,dsp_number);
    if(ret_val != DSPLIB_ERR_OK) {
        logprintf("dsp_ac_configuration(): Fail to configure DigitalCore"
                  " ac for ch:%d: %d\n",channel_number,ret_val);
        return ret_val;
    }
    logprintf("dsp_ac_configuration(): DigitalCore ch:%d configured"
              " adc:%d dac:%d pwd:%d st:%d\n", 
              channel_number, adc_gain, dac_gain, powerdown, state);
    return (ret_val);
}

// vocoder
DSP_API int dsp_vocoder_configuration(int chid, HD_VOCODER_TYPE vocoder_type, 
		HD_VOCODER_SUBTYPE vocoder_sub_type, bool vad_enable, bool frame_erase,int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
	HD_vocoder_initialization st0;
	SET_ZERO_STRUCT( st0 );
	st0.type = vocoder_type;

	HD_vocoder_configuration st1;
	SET_ZERO_STRUCT( st1 );
	st1.sub_type = vocoder_sub_type;
	st1.vad_enable = vad_enable ? DSP_TRUE: DSP_FALSE;
	st1.frame_erase_enable = frame_erase ? DSP_TRUE: DSP_FALSE;

	//st0.type=HD_VOCODER_G729;
	st0.channels_mask[chid][HD_ADC] = DSP_TRUE;
	st0.channels_mask[chid][HD_DAC] = DSP_TRUE;

	st1.channels_mask[chid][HD_ADC] = DSP_TRUE;
	st1.channels_mask[chid][HD_DAC] = DSP_TRUE;
		
        if (print_verbose)
	    logprintf("dsp_vocoder_configuration(): chid:%d vocoder:%d subtype:%d vad:%d\n",
			chid, vocoder_type, vocoder_sub_type, vad_enable);
	st0.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_initialization ( &st0 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration(): fail to init DSP vocoders\n");
		return ret;
	}
	st1.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_configuration ( &st1 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration(): fail to configure DSP vocoders\n");
	}
	return ret;
}

DSP_API int dsp_vocoder_configuration_channel_range(int chid_start, uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase, int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
        if (print_verbose)
	    logprintf("dsp_vocoder_configuration_channel_range(): %d channels "
			"from %d\n",
			chid_num, chid_start);
	HD_vocoder_initialization st0;
	SET_ZERO_STRUCT( st0 );
	st0.type = vocoder_type;

	HD_vocoder_configuration st1;
	SET_ZERO_STRUCT( st1 );
	st1.sub_type = vocoder_sub_type;
	st1.vad_enable = vad_enable ? DSP_TRUE: DSP_FALSE;
	st1.frame_erase_enable = frame_erase ? DSP_TRUE: DSP_FALSE;

	//st0.type=HD_VOCODER_G729;
	for(uint32_t i = chid_start; i < chid_start+chid_num; ++i) {
		st0.channels_mask[i][HD_ADC] = DSP_TRUE;
		st0.channels_mask[i][HD_DAC] = DSP_TRUE;
		
		st1.channels_mask[i][HD_ADC] = DSP_TRUE;
		st1.channels_mask[i][HD_DAC] = DSP_TRUE;
		
	}
        if (print_verbose)
	    logprintf("dsp_vocoder_configuration_channel_range(): "
			"vocoder:%d subtype:%d vad:%d\n",
			vocoder_type, vocoder_sub_type, vad_enable);
	st0.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_initialization ( &st0 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration_channel_range(): fail to init DSP vocoders\n");
		return ret;
	}
	st1.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_configuration ( &st1 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration_channel_range(): fail to configure DSP vocoders\n");
	}
	return ret;
}

DSP_API int dsp_vocoder_config_host_channels_auto(uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase,int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
        if (print_verbose) {
	    logprintf("dsp_vocoder_config_host_channels_auto(): %d channels\n",
			chid_num);
	    logprintf("dsp_vocoder_config_host_channels_auto(): "
			"vocoder:%d subtype:%d vad:%d\n",
			vocoder_type, vocoder_sub_type, vad_enable);
	}
	HD_vocoder_initialization st0;
	SET_ZERO_STRUCT( st0 );
	st0.type = vocoder_type;

	HD_vocoder_configuration st1;
	SET_ZERO_STRUCT( st1 );
	st1.sub_type = vocoder_sub_type;
	st1.vad_enable = vad_enable ? DSP_TRUE: DSP_FALSE;
	st1.frame_erase_enable = frame_erase ? DSP_TRUE: DSP_FALSE;

	HD_vocoder_enabling st2;
	SET_ZERO_STRUCT( st2 );

	unsigned int i;
	for(i = HD_HOST_CHANNELS_BASE; i < HD_HOST_CHANNELS_BASE + chid_num; ++i) {
		st0.channels_mask[i][HD_HOST_TO_DSP] = DSP_TRUE;
		st0.channels_mask[i][HD_DSP_TO_HOST] = DSP_TRUE;
		
		st1.channels_mask[i][HD_HOST_TO_DSP] = DSP_TRUE;
		st1.channels_mask[i][HD_DSP_TO_HOST] = DSP_TRUE;

		st2.channels_mask[i][HD_DSP_TO_HOST] = DSP_TRUE;
		st2.channels_mask[i][HD_HOST_TO_DSP] = DSP_TRUE;
	}

	st0.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_initialization ( &st0, dsp_number );
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_config_host_channels_auto(): fail to init DSP vocoders\n");
		return ret;
	}

	// now initialize vocoder for host2dsp channels only
	for(i = HD_HOST_CHANNELS_BASE; i < HD_HOST_CHANNELS_BASE + chid_num; ++i) {
		st0.channels_mask[i][HD_DSP_TO_HOST] = DSP_FALSE;
	}

	if (HD_VOCODER_G729 != vocoder_type) {
		st0.type = HD_VOCODER_G729;
		ret=hpi_write_HD_vocoder_initialization ( &st0 ,dsp_number);
		if(ret != DSPLIB_ERR_OK) {
			logprintf("dsp_vocoder_config_host_channels_auto(): fail to init DSP vocoders\n");
			return ret;
		}
	}

	if (HD_VOCODER_G711 != vocoder_type) {
		st0.type = HD_VOCODER_G711;
		ret=hpi_write_HD_vocoder_initialization ( &st0 ,dsp_number);
		if(ret != DSPLIB_ERR_OK) {
			logprintf("dsp_vocoder_config_host_channels_auto(): fail to init DSP vocoders\n");
			return ret;
		}
	}

	st1.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_configuration ( &st1 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_config_host_channels_auto(): fail to configure DSP vocoders\n");
	}

	st2.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_enabling ( &st2 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_config_host_channels_auto(): fail to enable DSP vocoders\n");
	}
	return ret;
}

DSP_API int dsp_vocoder_configuration(int chids[], uint32_t chid_num, 
	HD_VOCODER_TYPE vocoder_type, HD_VOCODER_SUBTYPE vocoder_sub_type,
	bool vad_enable, bool frame_erase,int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
        if (print_verbose)
	    logprintf("dsp_vocoder_configuration(): %d channels set to vocoder:%d subtype:%d\n",
			chid_num, vocoder_type, vocoder_sub_type);
	HD_vocoder_initialization st0;
	SET_ZERO_STRUCT( st0 );
	// vocoder_type = HD_VOCODER_G729;
	st0.type = vocoder_type;

	HD_vocoder_configuration st1;
	SET_ZERO_STRUCT( st1 );
	st1.sub_type = vocoder_sub_type;
	st1.vad_enable = vad_enable ? DSP_TRUE: DSP_FALSE;
	st1.frame_erase_enable = frame_erase ? DSP_TRUE: DSP_FALSE;

	for(uint32_t i = 0; i < chid_num; ++i) {
		st0.channels_mask[i][HD_ADC] = DSP_TRUE;
		st0.channels_mask[i][HD_DAC] = DSP_TRUE;
		
		st1.channels_mask[i][HD_ADC] = DSP_TRUE;
		st1.channels_mask[i][HD_DAC] = DSP_TRUE;
		
                if (print_verbose)
		    logprintf("dsp_vocoder_configuration(): chid:%d set to vocoder:%d subtype:%d vad_enable:%d\n",
			i, vocoder_type, vocoder_sub_type, vad_enable);
	}
	st0.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_initialization ( &st0 ,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration(): fail to init DSP vocoders\n");
		return ret;
	}
	st1.HD_request=DSP_TRUE;
	ret=hpi_write_HD_vocoder_configuration ( &st1 , dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_vocoder_configuration(): fail to configure DSP vocoders\n");
	}
	return ret;
}

DSP_API int dsp_vocoder_enable(int chid, bool enable, int dsp_number)
{
	HD_vocoder_enabling st3;
	SET_ZERO_STRUCT( st3 );
	st3.channels_mask[chid][HD_ADC] = enable ? DSP_TRUE: DSP_FALSE;
	st3.channels_mask[chid][HD_DAC] = enable ? DSP_TRUE: DSP_FALSE;
        if (print_verbose)
	    logprintf("dsp_vocoder_enable(): chid:%d vocoder: %s\n",
			chid, enable?"ENABLED":"DISABLED");
	st3.HD_request=DSP_TRUE;
	return hpi_write_HD_vocoder_enabling ( &st3 ,dsp_number);
}

DSP_API int dsp_vocoder_enable_channel_range(int chid_start, uint32_t chid_num, bool enable,int dsp_number)
{
	HD_vocoder_enabling st3;
	SET_ZERO_STRUCT( st3 );
	HD_BOOL enabled = enable ? DSP_TRUE: DSP_FALSE;
	for(uint32_t i = chid_start; i < chid_start+chid_num; ++i) {
		st3.channels_mask[i][HD_ADC] = enabled;
		st3.channels_mask[i][HD_DAC] = enabled;
                if (print_verbose)
		    logprintf("dsp_vocoder_enable_channel_range(): chid:%d vocoder: %s\n",
			i, enable?"ENABLED":"DISABLED");
	}
	st3.HD_request=DSP_TRUE;
	return hpi_write_HD_vocoder_enabling ( &st3,dsp_number );
}

DSP_API int dsp_vocoder_enable(int chids[], uint32_t chid_num, bool enable,int dsp_number)
{
	HD_vocoder_enabling st3;
	SET_ZERO_STRUCT( st3 );
	for(uint32_t i = 0; i < chid_num; ++i) {
		st3.channels_mask[i][HD_ADC] = enable ? DSP_TRUE: DSP_FALSE;
		st3.channels_mask[i][HD_DAC] = enable ? DSP_TRUE: DSP_FALSE;
                if (print_verbose)
		    logprintf("dsp_vocoder_enable(): chid:%d vocoder: %s\n",
			i, enable?"ENABLED":"DISABLED");
	}
	st3.HD_request=DSP_TRUE;
	return hpi_write_HD_vocoder_enabling ( &st3, dsp_number );
}
	
// Carrier Detection
DSP_API int dsp_cd_configuration_channel_range(int chid_start, uint32_t chid_num, 
		int16_t threshold_level, int16_t start_duration, int16_t stop_duration, bool mute,int dsp_number)
{
	int ret = DSPLIB_ERR_OK;
	HD_cd_initialization st6;
	SET_ZERO_STRUCT(st6);
	HD_cd_configuration st7;
	SET_ZERO_STRUCT(st7);

	for(uint32_t i = chid_start; i < chid_start+chid_num; ++i) {
		st6.channels_mask[i][HD_DAC] = DSP_TRUE;
		st7.channels_mask[i][HD_DAC] = DSP_TRUE;
                if (print_verbose)
		    logprintf("dsp_cd_configuration_channel_range(): chid:%d CD set:"
				" threshold_level:%d start_duration:%d stop_duration:%d\n",
			i, threshold_level, start_duration, stop_duration);
	}
	st7.threshold_level = threshold_level;
	st7.start_duration = start_duration;
	st7.stop_duration = stop_duration;
	st7.mute = mute?DSP_TRUE:DSP_FALSE;
	st6.HD_request=DSP_TRUE;
	ret = hpi_write_HD_cd_initialization(&st6,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_cd_configuration_channel_range(): Fail to initialize DigitalCore CD\n");
		return ret;
	}
	st7.HD_request=DSP_TRUE;
	ret = hpi_write_HD_cd_configuration(&st7,dsp_number);
	if(ret != DSPLIB_ERR_OK) {
		logprintf("dsp_cd_configuration_channel_range(): Fail to configure DigitalCore CD\n");
		return ret;
	}
	return ret;
}

DSP_API int dsp_cd_enable_channel_range(int chid_start, uint32_t chid_num, bool enable,int dsp_number)
{
	HD_cd_enabling st8;
	SET_ZERO_STRUCT(st8);
	for(uint32_t i = chid_start; i < chid_start+chid_num; ++i) {
		st8.channels_mask[i][HD_DAC] = enable ? DSP_TRUE: DSP_FALSE;
                if (print_verbose)
		    logprintf("dsp_cd_enable_channel_range(): chid:%d CD is %s\n",
			i, enable?"ENABLED":"DISABLED");
	}
	st8.HD_request=DSP_TRUE;
	return hpi_write_HD_cd_enabling( &st8 ,dsp_number);
}

DSP_API int hpi_read_HD_cd_status_channel(uint16_t *status,uint32_t chid_num,uint32_t dir,int dsp_number)
{

    uint16_t hd_st[2];
    int rv;
	if (dsp_number==DSP_NUM_2)
    	  rv= _hpi_read(_hpi_handle2, &hd_st, HPI_CTRL_OFFSET(sHD_cd_status)+chid_num, 1);
	else
	  rv = _hpi_read(_hpi_handle1, &hd_st, HPI_CTRL_OFFSET(sHD_cd_status)+chid_num, 1);
    if (rv == DSPLIB_ERR_OK) {
        *status = hd_st[dir];
    } 
    return rv;
}

////////////////////////////////  Audio fetching functions

DSP_API int dsp_audio_read(uint8_t* data, int size,int dsp_number)
{
    int bytes_read = 0;
    if (dsp_number==DSP_NUM_2)
    	bytes_read =  ::read( _hpi_handle2, data, size);
    else
    	bytes_read =  ::read( _hpi_handle1, data, size);
    return bytes_read;
}

DSP_API int dsp_audio_write(uint8_t* data, int size,int dsp_number)
{
    int bytes_write;
    if (dsp_number==DSP_NUM_2)
	bytes_write = ::write(_hpi_handle2, data, size);
    else
	bytes_write = ::write(_hpi_handle1, data, size);
    return bytes_write;
}

DSP_API int dsp_init_DIAL_dtmf(int start_idx, int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

    const ToneConf tones[] = {
        {INDEX_350Hz,    350},
        {INDEX_440Hz,    440},
        {INDEX_480Hz,    480},
        {INDEX_620Hz,    620}
    };

    unsigned int i;

    start_DIAL_dtmf_idx = start_idx;

    --start_idx;
    for (i = 0; i < elements_of(tones); ++i) {
        sToneCfg.freq_hz[start_idx + tones[i].idx] = tones[i].freq;
        if (print_verbose)
            logprintf("dsp_init_DIAL_dtmf(): tone:%d freq:%d\n", start_idx + tones[i].idx + 1, tones[i].freq);
    }
    sToneCfg.HD_request=DSP_TRUE;
 
    rv = hpi_write_HD_tone_configuration(&sToneCfg, dsp_number);
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_init_DIAL_dtmf(): Fail to set tones %d\n", rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_init_DIGITs_dtmf(int start_idx,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

    const ToneConf tones[] = {
        {INDEX_697Hz,    697},
        {INDEX_770Hz,    770},
        {INDEX_852Hz,    852},
        {INDEX_941Hz,    941},
        {INDEX_1209Hz,   1209},
        {INDEX_1336Hz,   1336},
        {INDEX_1477Hz,   1477},
        {INDEX_1633Hz,   1633}
    };

    unsigned int i;

    start_DIGITs_dtmf_idx = start_idx;

    --start_idx;
    for (i = 0; i < elements_of(tones); ++i) {
        sToneCfg.freq_hz[start_idx + tones[i].idx] = tones[i].freq;
        if (print_verbose)
            logprintf("dsp_init_DIGITs_dtmf(): tone:%d freq:%d\n", start_idx + tones[i].idx + 1, tones[i].freq);
    }
    sToneCfg.HD_request=DSP_TRUE;
 
    rv = hpi_write_HD_tone_configuration(&sToneCfg,dsp_number);
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_init_DIGITs_dtmf(): Fail to set tones %d\n", rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_init_dtmf(int start_idx,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

    const ToneConf tones[] = {
        {INDEX_697Hz,    697},
        {INDEX_770Hz,    770},
        {INDEX_852Hz,    852},
        {INDEX_941Hz,    941},
        {INDEX_1209Hz,   1209},
        {INDEX_1336Hz,   1336},
        {INDEX_1477Hz,   1477},
        {INDEX_1633Hz,   1633},
        {INDEX_DIGITs_LAST + INDEX_350Hz,    350},
        {INDEX_DIGITs_LAST + INDEX_440Hz,    440},
        {INDEX_DIGITs_LAST + INDEX_480Hz,    480},
        {INDEX_DIGITs_LAST + INDEX_620Hz,    620}
    };

    unsigned int i;

    start_DIGITs_dtmf_idx = start_idx;
    start_DIAL_dtmf_idx = start_idx + INDEX_DIGITs_LAST;

    --start_idx;
    for (i = 0; i < elements_of(tones); ++i) {
        sToneCfg.freq_hz[start_idx + tones[i].idx] = tones[i].freq;
        if (print_verbose)
            logprintf("dsp_init_dtmf(): tone:%d freq:%d\n", start_idx + tones[i].idx + 1, tones[i].freq);
    }
    sToneCfg.HD_request=DSP_TRUE;
 
    rv = hpi_write_HD_tone_configuration(&sToneCfg,dsp_number);
    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_init_dtmf(): Fail to set tones %d\n", rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int  dsp_do_dualtone( int tone_1, int tone_2,
		int volume, bool do_mux_audio, int ch1, int ch2, int ch3, int ch4,int dsp_number )
{
    int rv = DSPLIB_ERR_OK;
    if ( tone_1 < 0 ) return DSPLIB_ERR_FAIL;
    if ( tone_2 < 0 ) return DSPLIB_ERR_FAIL;
    if (print_verbose)
        logprintf("dsp_do_dualtone(): tone1:%d [%d Hz] tone2:%d [%d Hz] ch1:%d ch2:%d ch3:%d ch4:%d volume:%d do_mux_audio:%s\n",
            tone_1, sToneCfg.freq_hz[ tone_1 - 1], tone_2, sToneCfg.freq_hz[ tone_2 - 1], ch1, ch2, ch3, ch4, volume, do_mux_audio ? "TRUE":"FALSE");

//    fprintf(stderr, "dsp_stereo_dtmf(): tone1:%d tone2:%d ch1:%d ch2:%d volume:%d do_mux_audio:%s\n",
//              tone_1, tone_2, ch1, ch2, ch3, volume, do_mux_audio ? "TRUE":"FALSE");

    int tone_mode = do_mux_audio ? HD_TONE_AUDIO_MUX: HD_TONE_ONLY_MUX;

    if (-1 < ch1)
    {
        sToneMuxCfg.sum_to_speech[ch1] = tone_mode;
        sToneMuxCfg.volume[ch1] = volume; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch1] = tone_1;
        sToneMuxCfg.tones_number_b[ch1] = tone_2;
    }

    if (-1 < ch2)
    {
        sToneMuxCfg.sum_to_speech[ch2] = tone_mode;
        sToneMuxCfg.volume[ch2] = volume; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch2] = tone_1;
        sToneMuxCfg.tones_number_b[ch2] = tone_2;
    }

    if (-1 < ch3)
    {
        sToneMuxCfg.sum_to_speech[ch3] = tone_mode;
        sToneMuxCfg.volume[ch3] = volume; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch3] = tone_1;
        sToneMuxCfg.tones_number_b[ch3] = tone_2;
    }

    if (-1 < ch4)
    {
        sToneMuxCfg.sum_to_speech[ch4] = tone_mode;
        sToneMuxCfg.volume[ch4] = volume; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch4] = tone_1;
        sToneMuxCfg.tones_number_b[ch4] = tone_2;
    }

    sToneMuxCfg.HD_request= DSP_TRUE;

    rv = hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);

    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_do_dualtone(): Fail to generate dtmf:[%d\%d] - %d\n", tone_1, tone_2, rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_stop_dtmf( int ch1, int ch2, int ch3, int ch4 ,int dsp_number)
{
    int rv = DSPLIB_ERR_OK;

//    logprintf("dsp_stereo_dtmf(): tone1:%d tone2:%d ch1:%d ch2:%d volume:%d do_mux_audio:%s\n",
//            tone_1, tone_2, ch1, ch2, volume, do_mux_audio ? "TRUE":"FALSE");
    if (-1 < ch1)
    {
        sToneMuxCfg.sum_to_speech [ch1] = HD_TONE_ONLY_MUX;
        sToneMuxCfg.volume        [ch1] = 0; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch1] = 0;
        sToneMuxCfg.tones_number_b[ch1] = 0;
    }

    if (-1 < ch2)
    {
        sToneMuxCfg.sum_to_speech [ch2] = HD_TONE_ONLY_MUX;
        sToneMuxCfg.volume        [ch2] = 0; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch2] = 0;
        sToneMuxCfg.tones_number_b[ch2] = 0;
    }

    if (-1 < ch3)
    {
        sToneMuxCfg.sum_to_speech [ch3] = HD_TONE_ONLY_MUX;
        sToneMuxCfg.volume        [ch3] = 0; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch3] = 0;
        sToneMuxCfg.tones_number_b[ch3] = 0;
    }

    if (-1 < ch4)
    {
        sToneMuxCfg.sum_to_speech [ch4] = HD_TONE_ONLY_MUX;
        sToneMuxCfg.volume        [ch4] = 0; // 1 the max volume, the more the less
        sToneMuxCfg.tones_number_a[ch4] = 0;
        sToneMuxCfg.tones_number_b[ch4] = 0;
    }

    sToneMuxCfg.HD_request= DSP_TRUE;

    rv = hpi_write_nowait_HD_tone_mux_configuration(&sToneMuxCfg,dsp_number);

    if(rv != DSPLIB_ERR_OK) {
        logprintf("dsp_stop_dtmf(): Fail to stop dtmf - %d\n", rv);
        return DSPLIB_ERR_FAIL;
    }
    return rv;
}

DSP_API int dsp_dtmf_dial_tone(int volume, bool do_mux_audio, int channel_1, int channel_2, int channel_3, int channel_4,int dsp_number)
{
	// Dial Tone
    int rv = dsp_do_dualtone(start_DIAL_dtmf_idx + INDEX_350Hz, start_DIAL_dtmf_idx + INDEX_440Hz,
                         volume, do_mux_audio, channel_1, channel_2, channel_3, channel_4,dsp_number);
    return rv;
}

DSP_API int dsp_dtmf_busy_tone(int volume, bool do_mux_audio, int channel_1, int channel_2, int channel_3, int channel_4,int dsp_number)
{
	// Busy Tone
    int rv = dsp_do_dualtone(start_DIAL_dtmf_idx + INDEX_480Hz, start_DIAL_dtmf_idx + INDEX_620Hz,
                         volume, do_mux_audio, channel_1, channel_2, channel_3, channel_4,dsp_number);
    return rv;
}

DSP_API int dsp_dtmf_tone(char key, int volume, int length, bool do_mux_audio, int channel_1, int channel_2, int channel_3, int channel_4,int dsp_number)
{
    int rv;
    int tone1, tone2;

    switch(key)
    {
	    case '0': // The '0' digit
		    tone1 = INDEX_941Hz;
		    tone2 = INDEX_1336Hz;
		    break;

	    case '1': // The '1' digit
		    tone1 = INDEX_697Hz;
		    tone2 = INDEX_1209Hz;
		    break;

	    case '2': // The '2' digit
		    tone1 = INDEX_697Hz;
		    tone2 = INDEX_1336Hz;
		    break;

	    case '3': // The '3' digit
		    tone1 = INDEX_697Hz;
		    tone2 = INDEX_1477Hz;
		    break;

	    case '4': // The '4' digit
		    tone1 = INDEX_770Hz;
		    tone2 = INDEX_1209Hz;
		    break;

	    case '5': // The '5' digit
		    tone1 = INDEX_770Hz;
		    tone2 = INDEX_1336Hz;
		    break;

	    case '6': // The '6' digit
		    tone1 = INDEX_770Hz;
		    tone2 = INDEX_1477Hz;
		    break;

	    case '7': // The '7' digit
		    tone1 = INDEX_852Hz;
		    tone2 = INDEX_1209Hz;
		    break;

	    case '8': // The '8' digit
		    tone1 = INDEX_852Hz;
		    tone2 = INDEX_1336Hz;
		    break;

	    case '9': // The '9' digit
		    tone1 = INDEX_852Hz;
		    tone2 = INDEX_1477Hz;
		    break;

	    case '*': // *
		    tone1 = INDEX_941Hz;
		    tone2 = INDEX_1209Hz;
		    break;

	    case '#': // #
		    tone1 = INDEX_941Hz;
		    tone2 = INDEX_1477Hz;
		    break;

	    case 'A': // A
		    tone1 = INDEX_697Hz;
		    tone2 = INDEX_1633Hz;
		    break;

	    case 'B': // B
		    tone1 = INDEX_770Hz;
		    tone2 = INDEX_1633Hz;
		    break;

	    case 'C': // C
		    tone1 = INDEX_852Hz;
		    tone2 = INDEX_1633Hz;
		    break;

	    case 'D': // D
		    tone1 = INDEX_941Hz;
		    tone2 = INDEX_1633Hz;
		    break;

	    default:
		logprintf("dsp_dtmf_tone:: Invalid key specified: '%c'\n", key);
		return DSPLIB_ERR_FAIL;
    }

    rv = dsp_do_dualtone(start_DIGITs_dtmf_idx + tone1, start_DIGITs_dtmf_idx + tone2,
		    volume, do_mux_audio,
		    channel_1, channel_2, channel_3, channel_4,dsp_number);
    if(rv != DSPLIB_ERR_OK) {
        return rv;
    }
    usleep(length * 1000);
    
    rv = dsp_stop_dtmf(channel_1, channel_2, channel_3, channel_4,dsp_number);

    return rv;
}

////////////////////////////////////////////////////////////////////////
