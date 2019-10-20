#ifndef _BEEP_H
#define _BEEP_H

#include "stdint.h"

#define BEEP_INTERVAL 200 //IO toggle interval time.
#define BEEP_DRIVER 	2*BEEP_INTERVAL-1,72 //IO toggle cycle.

void BEEP_Init(void);
void BEEP_Play(uint32_t mtime,uint32_t rate_utime);//mtime must be greater than 200ms;

#endif

