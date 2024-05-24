
#ifndef __DSP_HPI_H
#define __DSP_HPI_H

enum Arch {
	ArchRAC = 1,
	ArchMACS,
	ArchPiggy,
	ArchACB11
};

#define HPICTLREAD 1000
#define HPICTLWRITE 1001
#define HPICTLRESETDSP 1002
#define HPICTLRELEASEDSP 1003
#define HPICTLGETARCH 1004

#define HPICTLREADVECTOR 1005
#define HPICTLWRITEVECTOR 1006
#define HPICTLSHORTTXQUE 1009

typedef struct hpi_ioctl_read_data_tag {
	uint32_t target_buf;
	uint32_t len;
	uint32_t dsp_ctrl_offset;
} hpi_ioctl_read_data;

typedef struct hpi_ioctl_read_data_tag_vector {
	uint32_t n_elem;
	hpi_ioctl_read_data* buffers;
} hpi_ioctl_read_data_vector;

typedef struct hpi_ioctl_write_data_tag {
	uint32_t source_buf;
	uint32_t len;
	uint32_t dsp_ctrl_offset;
} hpi_ioctl_write_data;

typedef struct hpi_ioctl_write_data_tag_vector {
	uint32_t n_elem;
	hpi_ioctl_write_data* buffers;
} hpi_ioctl_write_data_vector;

#endif // __DSP_HPI_H
