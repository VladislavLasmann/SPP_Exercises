/*
 * Copyright (c) 2009 The Trustees of Indiana University and Indiana
 *                    University Research and Technology
 *                    Corporation.  All rights reserved.
 *
 * Author(s): Torsten Hoefler <htor@cs.indiana.edu>
 *
 */

#include <inttypes.h>
#include <stdio.h>
#include "calibrate.h"
#define UINT32_T uint32_t
#define UINT64_T uint64_t

#define HRT_INIT(print, freq) do {\
  if(print) printf("# initializing x86-64 timer (takes some seconds)\n"); \
  HRT_CALIBRATE(freq); \
} while(0) 


typedef struct {
	UINT32_T l;
	UINT32_T h;
} x86_64_timeval_t;

#define HRT_TIMESTAMP_T x86_64_timeval_t

/* TODO: Do we need a while loop here? aka Is rdtsc atomic? - check in the documentation */	
#define HRT_GET_TIMESTAMP(t1)   \
  __asm__ __volatile__ (        \
  "xorl %%eax, %%eax \n\t"      \
  "cpuid             \n\t"      \
  "rdtsc             \n\t"      \
  : "=a" (t1.l), "=d" (t1.h)    \
  :                             \
  : "%ebx", "%ecx"              \
  );

#define HRT_GET_ELAPSED_TICKS(t1, t2, numptr)   \
  *numptr = (((( UINT64_T ) t2.h) << 32) | t2.l) - (((( UINT64_T ) t1.h) << 32) | t1.l);

#define HRT_GET_TIME(t1, time) time = (((( UINT64_T ) t1.h) << 32) | t1.l)

/* Timing API: */

static const char* CPU_FREQ_FILENAME = ".cpu_freq";
static unsigned long long g_timerfreq;

/*
static double HRT_GET_USEC(unsigned long long ticks) {
  return 1e6*(double)ticks/(double)g_timerfreq;
}
*/
static double HRT_GET_MSEC(unsigned long long ticks) {
  return 1e3*(double)ticks/(double)g_timerfreq;
}

/* Must be called before get_clock_time */  
static void init_clock_time () { 
	
	char buff[128];
	FILE* cpufreq_file = fopen (CPU_FREQ_FILENAME, "r");
	if (cpufreq_file) {
		if (fgets (buff, 128, cpufreq_file)) {
			g_timerfreq = (uint64_t)strtod (buff, NULL);
		}
		else {
			exit (-1);
		}
		fclose (cpufreq_file);
	}
	else {
		HRT_INIT(0, g_timerfreq); 
		FILE* cpufreq_file = fopen (CPU_FREQ_FILENAME, "w");
		fprintf(cpufreq_file, "%llu\n", g_timerfreq);
		fclose( cpufreq_file );
	}
	/*printf ("cycles per sec = %llu\n", g_timerfreq);*/
}

/* Returns accurate time in ms */
double get_clock_time () { 
	
	HRT_TIMESTAMP_T t;
	uint64_t ticks;
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	return HRT_GET_MSEC(ticks);
}
