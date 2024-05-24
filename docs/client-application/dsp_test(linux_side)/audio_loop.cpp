
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "libdsp.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>

#define PACKET_SIZE (11704 * 2)
static __attribute__((aligned (8))) uint8_t packet_from_DSP[PACKET_SIZE];
/*
int main(int argc, char* argv[])
{
	int bytes, packet_bytes;

	dsp_init();
#if 0
	struct sched_param param;
	param.sched_priority = 10; // sync this with libRTP (RTP should be lower)
	int ret;
	ret = ::sched_setscheduler(0, SCHED_RR, &param);
	if (ret)
	{
		fprintf(stderr, "RacDeMux::svc() sched_setscheduler error: %s\n", strerror(errno));
	}
#else
	::nice(-20);
#endif

	struct timeval last_tv, curr_tv, delta;
	struct timeval delta_tv = {0, 10700};
	struct timeval delta_tv_early = {0, 9500};
	gettimeofday(&last_tv, NULL);
	for (;;)
	{
		gettimeofday(&curr_tv, NULL);
		timersub(&curr_tv, &last_tv, &delta);
		if (timercmp(&delta, &delta_tv, >))
			fprintf(stderr, "Packet too late - delta: %d.%d\n", delta.tv_sec, delta.tv_usec);
		else if (timercmp(&delta, &delta_tv_early, <))
			fprintf(stderr, "Packet too early - delta: %d.%d\n", delta.tv_sec, delta.tv_usec);

		last_tv = curr_tv;
		packet_bytes = dsp_audio_read(packet_from_DSP, sizeof(packet_from_DSP));
		if (packet_bytes < 0)
		{
			fprintf(stderr, "Fetcher::svc(): Fail read %d: %s\n", packet_bytes, strerror(errno));
			continue;
		}
#if 1
		bytes = dsp_audio_write(packet_from_DSP, packet_bytes);
		if (bytes != packet_bytes)
		{
			fprintf(stderr, "Fetcher::svc(): Fail write bytes %d: %s\n",
					   packet_bytes, strerror(errno));
		}
#endif
	}

	return 0;
}
*/
