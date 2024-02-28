#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"

#include "systick_delay.h"
#include <stdio.h>

// -------------------------------------------- Status Led Data -------------------------------------------------------- //

typedef enum
{
   LED_STATE_OFF = 0,
   LED_STATE_ON,
   LED_STATE_BLINK_SLOW,
   LED_STATE_BLINK_FAST,
} led_state_t;

typedef enum
{
   LED_SET_GREEN = 0,
   LED_SET_RED,
   LED_COUNT,
} led_set_t;

#define LED_BLINK_SLOW_PERIOD_MS             800
#define LED_BLINK_FAST_PERIOD_MS             250

// -------------------------------------------- SWD Detect Data -------------------------------------------------------- //

typedef enum
{
   SWD_STATE_IDLE = 0,
   SWD_STATE_USB_CABLE_CONNECTED,
   SWD_STATE_SWD_POS1,
   SWD_STATE_SWD_POS2,
   SWD_STATE_ERROR,
   SWD_STATE_START,
} swd_state_t;

typedef struct __attribute__((__packed__))
{
   swd_state_t state;
   led_state_t led[LED_COUNT];
   led_state_t led_new[LED_COUNT];
   uint32_t led_counter[LED_COUNT];
   uint32_t pos;
   uint32_t cc_mv[2];
   uint32_t sbu_mv[2];
} swd_conf_t;

typedef struct __attribute__((__packed__))
{
   uint32_t cc[2];
   uint32_t sbu[2];
} usb_state_t;

#define PIN_CC_TOLERANCE_MV                  50
#define PIN_SBU_TOLERANCE_MV                 80

// -------------------------------------------- App Settings -------------------------------------------------------- //

#define ADC_CHANNEL_SAMPLES_COUNT            2
#define ADC_CHANNELS_COUNT                   4
#define ADC_MAX_VALUE_MV                     3300
#define ADC_RESOLUTION_MAX_VALUE             4095

#define STATE_CC_CNC_TH_MV                   2500
#define STATE_CC_CC_TH_MV                    1500
#define STATE_SBU_CC_TH_MV                   1200

// -------------------------------------------- Debug Settings ------------------------------------------------------ //

#ifndef assert_param
#define assert_param(expr)                   ((void)NULL)
#endif

#endif // _MAIN_H_
