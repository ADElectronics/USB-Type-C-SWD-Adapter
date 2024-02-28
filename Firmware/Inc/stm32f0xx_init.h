#ifndef _STM32F0XX_INIT_H_
#define _STM32F0XX_INIT_H_

#include "main.h"

void SysInit_Clock(void);
void SysInit_GPIO(void);
void SysInit_ADC(void);
void SysInit_TIM(void);
void SysInit_DMA(void);

#endif // _STM32F0XX_INIT_H_