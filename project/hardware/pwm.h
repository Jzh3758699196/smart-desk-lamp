#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void PWM_Init(void);
void PWM_SetDuty(uint16_t duty);   // duty: 0 ~ 100

#endif