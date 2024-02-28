#include "main.h"
#include "stm32f0xx_init.h"

const uint16_t led_pins[LED_COUNT] = { LL_GPIO_PIN_7, LL_GPIO_PIN_6, };
static uint8_t adc_ready;
static swd_conf_t swd;
uint16_t adc_data_raw[ADC_CHANNEL_SAMPLES_COUNT * ADC_CHANNELS_COUNT];

void ADC_Callback()
{
   uint16_t sum[ADC_CHANNELS_COUNT] = { 0 };
   for (uint8_t i = 0; i < ADC_CHANNEL_SAMPLES_COUNT; i++)
   {
      sum[0] += adc_data_raw[0 + (ADC_CHANNELS_COUNT * i)];
      sum[1] += adc_data_raw[1 + (ADC_CHANNELS_COUNT * i)];
      sum[2] += adc_data_raw[2 + (ADC_CHANNELS_COUNT * i)];
      sum[3] += adc_data_raw[3 + (ADC_CHANNELS_COUNT * i)];
   }

   swd.sbu_mv[0] = ((sum[0] / ADC_CHANNEL_SAMPLES_COUNT) * ADC_MAX_VALUE_MV) / ADC_RESOLUTION_MAX_VALUE;
   swd.sbu_mv[1] = ((sum[2] / ADC_CHANNEL_SAMPLES_COUNT) * ADC_MAX_VALUE_MV) / ADC_RESOLUTION_MAX_VALUE;
   swd.cc_mv[0] = ((sum[1] / ADC_CHANNEL_SAMPLES_COUNT) * ADC_MAX_VALUE_MV) / ADC_RESOLUTION_MAX_VALUE;
   swd.cc_mv[1] = ((sum[3] / ADC_CHANNEL_SAMPLES_COUNT) * ADC_MAX_VALUE_MV) / ADC_RESOLUTION_MAX_VALUE;
   adc_ready = 1;
}

void status_Set(led_set_t led, led_state_t state)
{
   led = led & LED_SET_RED;
   swd.led_new[led] = state;
   LL_GPIO_ResetOutputPin(GPIOA, led_pins[led]);
}

void status_Routine()
{
   for (uint8_t i = 0; i < LED_COUNT; i++)
   {
      if (swd.led[i] != swd.led_new[i])
      {
         switch (swd.led_new[i])
         {
            case LED_STATE_OFF:
               LL_GPIO_ResetOutputPin(GPIOA, led_pins[i]);
               swd.led[i] = swd.led_new[i];
               break;

            case LED_STATE_ON:
               LL_GPIO_SetOutputPin(GPIOA, led_pins[i]);
               swd.led[i] = swd.led_new[i];
               break;

            default:
               // TODO: переделать на systick расчёт, но лень чёт
               if (swd.led_counter[i] == 0)
               {
                  LL_GPIO_TogglePin(GPIOA, led_pins[i]);
                  if (swd.led_new[i] == LED_STATE_BLINK_SLOW)
                  {
                     swd.led_counter[i] = LED_BLINK_SLOW_PERIOD_MS;
                  }
                  else
                  {
                     swd.led_counter[i] = LED_BLINK_FAST_PERIOD_MS;
                  }
               }
               else
               {
                  swd.led_counter[i]--;
               }
               break;
         }
      }
   }
}

void swd_UpdateState(swd_state_t state)
{
   if (swd.state != state)
   {
      swd.state = state;
#if 0
      SysView_LogInfo("\r\n[sys] CC: %04u %04u SBU: %04u %04u\r\n", swd.cc_mv[0], swd.cc_mv[0], swd.sbu_mv[0], swd.sbu_mv[1]);
#endif
      switch (swd.state)
      {
         case SWD_STATE_ERROR:
            // TODO: на данный момент не используется ...
            //SysView_LogError("[sys] Hardware issue...\r\n");
            status_Set(LED_SET_GREEN, LED_STATE_OFF);
            status_Set(LED_SET_RED, LED_STATE_ON);
            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
            //NVIC_SystemReset();
            break;
         
         case SWD_STATE_USB_CABLE_CONNECTED:
            //SysView_LogInfo("[sys] USB connected\r\n");
            status_Set(LED_SET_GREEN, LED_STATE_BLINK_FAST);
            status_Set(LED_SET_RED, LED_STATE_OFF);
            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
            break;

         case SWD_STATE_SWD_POS1:
            //SysView_LogInfo("[sys] Forward connection\r\n");
            status_Set(LED_SET_GREEN, LED_STATE_BLINK_SLOW);
            status_Set(LED_SET_RED, LED_STATE_OFF);
            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
            break;

         case SWD_STATE_SWD_POS2:
            //SysView_LogInfo("[sys] Opposite connection\r\n");
            status_Set(LED_SET_GREEN, LED_STATE_BLINK_SLOW);
            status_Set(LED_SET_RED, LED_STATE_OFF);
            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
            break;

         default: // SWD_STATE_IDLE
            //SysView_LogInfo("[sys] USB disconnected\r\n");
            status_Set(LED_SET_GREEN, LED_STATE_OFF);
            status_Set(LED_SET_RED, LED_STATE_BLINK_SLOW);
            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
            break;
      }
   }
}

void swd_Routine()
{
   if (adc_ready)
   {
      adc_ready = 0;

      if (((swd.cc_mv[0] > STATE_CC_CNC_TH_MV) && (swd.cc_mv[1] > STATE_CC_CNC_TH_MV)))
      {
         swd_UpdateState(SWD_STATE_IDLE);
      }
      else
      {
         for (uint8_t i = 0; i < 2; i++)
         {
            if ((swd.cc_mv[0] < STATE_CC_CC_TH_MV) || (swd.cc_mv[1] < STATE_CC_CC_TH_MV))
            {
               if ((swd.state != SWD_STATE_SWD_POS1) & (swd.state != SWD_STATE_SWD_POS2))
               {
                  if (swd.state == SWD_STATE_USB_CABLE_CONNECTED)
                  {
                     if (swd.sbu_mv[i] < STATE_SBU_CC_TH_MV)
                     {
                        if (i)
                        {
                           swd_UpdateState(SWD_STATE_SWD_POS1);
                           break;
                        }
                        else
                        {
                           swd_UpdateState(SWD_STATE_SWD_POS2);
                           break;
                        }
                     }
                  }
                  else
                  {
                     swd_UpdateState(SWD_STATE_USB_CABLE_CONNECTED);
                  }
               }
            }
         }
      }
   }
}

int main(void)
{
   SysInit_Clock();
   SysTick_Config(SystemCoreClock / 1000U); // 1ms
   SysInit_GPIO();
   SysInit_DMA();
   SysInit_ADC();
   SysInit_TIM();

   //SysView_Init();
   //SysView_PrintInfo();

   swd.state = SWD_STATE_START;
   swd_UpdateState(SWD_STATE_IDLE);

   LL_TIM_EnableCounter(TIM1);
   LL_ADC_REG_StartConversion(ADC1);
   SysTick_Delay_ms(100);

   for (;;)
   {
      SysTick_Delay_ms(1);
      status_Routine();
      swd_Routine();
   }
}
