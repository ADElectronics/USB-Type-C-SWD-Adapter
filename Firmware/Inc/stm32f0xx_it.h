#ifndef _STM32F0XX_IT_H_
#define _STM32F0XX_IT_H_

void Error_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif // _STM32F0XX_IT_H_
