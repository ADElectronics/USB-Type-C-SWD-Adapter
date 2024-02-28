#include "stm32f0xx_init.h"

void SysInit_Clock(void)
{
   // HSE = 16 MHz
   // PLL = (16 MHz / 4) * 12 = 48 MHz
   // Sys = PLL = 48 MHz
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

   LL_RCC_HSE_Enable(); 
   while (LL_RCC_HSE_IsReady() != 1) ;

   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_4, LL_RCC_PLL_MUL_12);
   LL_RCC_PLL_Enable();
   while (LL_RCC_PLL_IsReady() != 1) ;

   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) ;

   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
   SystemCoreClockUpdate();
}

void SysInit_GPIO(void)
{
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

   // Led R (PA6), G (PA7)
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);

   // SWD En N
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

   // SWD Select
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
}

void SysInit_ADC(void)
{
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ANALOG);

   LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

   LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
   LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_71CYCLES_5);
   LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM1_TRGO);
   LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);
   //LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
   LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
   LL_ADC_REG_SetSequencerChannels(ADC1, (LL_ADC_CHANNEL_0 | LL_ADC_CHANNEL_1 | LL_ADC_CHANNEL_2 | LL_ADC_CHANNEL_3));

   LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
   LL_ADC_StartCalibration(ADC1);
   while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) ; // TODO: add timeout

   SysTick_Delay_ms(1);
   LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
   LL_ADC_Enable(ADC1);
   while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) ; // TODO: add timeout
}

void SysInit_TIM(void)
{
   LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

   LL_TIM_SetPrescaler(TIM1, (4800 - 1));
   LL_TIM_SetAutoReload(TIM1, (10000 - 1));
   LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP); 
   LL_TIM_SetRepetitionCounter(TIM1, 0);
   LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_UPDATE);
   //LL_TIM_EnableCounter(TIM1);
}

extern uint16_t adc_data_raw[ADC_CHANNEL_SAMPLES_COUNT * ADC_CHANNELS_COUNT];

void SysInit_DMA(void)
{
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
   NVIC_SetPriority(DMA1_Channel1_IRQn, 1); // DMA IRQ lower priority than ADC IRQ
   NVIC_EnableIRQ(DMA1_Channel1_IRQn);

   LL_DMA_ConfigTransfer(DMA1,
                        LL_DMA_CHANNEL_1,
                        LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                        LL_DMA_MODE_CIRCULAR              |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_HALFWORD        |
                        LL_DMA_MDATAALIGN_HALFWORD        |
                        LL_DMA_PRIORITY_HIGH               );
   LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1,
                          LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                          (uint32_t)&adc_data_raw[0],
                          LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
   LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (ADC_CHANNEL_SAMPLES_COUNT * ADC_CHANNELS_COUNT));
   LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
   LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}
