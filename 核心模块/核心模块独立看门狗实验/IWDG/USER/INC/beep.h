#ifndef _BEEP_H
#define _BEEP_H

#include "stdint.h"

void BEEP_Init(void);
void BEEP_Play(uint32_t mtime);//mtime must be greater than 200ms;

#endif

