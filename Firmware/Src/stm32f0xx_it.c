#include "main.h"
#include "stm32f0xx_it.h"

// -------------------------------------------- DMA Handlers -------------------------------------------------------- //

extern void ADC_Callback(void);

void DMA1_Channel1_IRQHandler(void)
{
   if (LL_DMA_IsActiveFlag_TC1(DMA1))
   {
      LL_DMA_ClearFlag_TC1(DMA1);
      ADC_Callback();
   }
}

// -------------------------------------------- System Int ---------------------------------------------------------- //

void Error_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

void NMI_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

void HardFault_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

void MemManage_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

void BusFault_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

void UsageFault_Handler(void)
{
   __disable_irq();
   for (;;) ;
}

// если не юзается в FreeRTOS или ThreadX
void __attribute__ ((weak)) SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}

// если не юзается в FreeRTOS или ThreadX
void __attribute__ ((weak)) PendSV_Handler(void)
{

}

#ifdef AZURE_RTOS_THREADX
extern void _tx_timer_interrupt(void); // ThreadX
#endif

void SysTick_Handler(void)
{
   SysTick_Inc(); // No RTOS
#ifdef USE_HAL_DRIVER // HAL
   HAL_IncTick();
#endif
#ifdef xPortSysTickHandler // FreeRTOS
   SysTick_RTOS();
#endif
#ifdef AZURE_RTOS_THREADX // ThreadX
   _tx_timer_interrupt();
#endif
}
