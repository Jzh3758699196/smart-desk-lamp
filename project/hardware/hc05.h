#ifndef __HC05_H
#define __HC05_H

#include "stm32f10x.h"

extern int target_lux;

void HC05_Init(void);
void HC05_Process(void);
void HC05_SendString(char *str);

#endif
