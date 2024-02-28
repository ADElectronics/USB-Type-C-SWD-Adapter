#ifndef _SYSTICK_DELAY_H_
#define _SYSTICK_DELAY_H_

#include "main.h"

void SysTick_Delay_ms(uint32_t ms);
void SysTick_Inc(void);
uint32_t SysTick_GetCurrentTick(void);

#endif // _SYSTICK_DELAY_H_
