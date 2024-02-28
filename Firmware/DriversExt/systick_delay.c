#include "systick_delay.h"

volatile uint32_t tick = 0;

__inline void SysTick_Delay_ms(uint32_t ms)
{
   uint32_t tickstart = tick;
   while((tick - tickstart) < ms) ;
}

__inline void SysTick_Inc(void)
{
   tick++;
}

uint32_t SysTick_GetCurrentTick(void)
{
   return tick;
}
