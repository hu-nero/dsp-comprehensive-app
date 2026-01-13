/*
 * hal_timer.c
 *
 *  Created on: 2025年12月27日
 *      Author: huxl
 */
#include "hal_timer.h"
#include "stdint_fix.h"
#include "string.h"

halTimerCallbackFunc ghalTimer0Callback = NULL;
static volatile uint32_t gu32TimerCounter = 0;
static volatile uint32_t gu32TimerRunTime = 0;
static volatile bool gboolTimer50msflag = false;

/**
 * @brief :timer init
 *
 * @param Id:timer dev id
 * @param Freq:
 * @param Period:
 * time = Freq * Period / 150 000 000(s)
 *            ex: 500ms: Freq=150, Period=500 000
 *            ex: 1ms: Freq=150, Period=1000
 */
uint16_t
hal_timer_init(uint16_t PortId, float Freq, float Period)
{
    //init timer device
    switch(PortId)
    {
        case HAL_DEV_TIM0:
            {
                TIM0_Init(Freq, Period);
            }
            break;
        case HAL_DEV_TIM1:
            {
                return 1;
            }
        case HAL_DEV_TIM2:
            {
                return 2;
            }
        default:return 3;
    }
    return 0;
}


/**
 * @brief :deinit timer
 *
 * @return :0 is success,1 is error
 */
uint16_t
hal_timer_deinit(uint16_t PortId)
{
    //init timer device
    switch(PortId)
    {
        case HAL_DEV_TIM0:
            {
                TIM0_DeInit();
            }
            break;
        case HAL_DEV_TIM1:
            {
                return 1;
            }
        case HAL_DEV_TIM2:
            {
                return 2;
            }
        default:return 3;
    }
    return 0;
}

/**
 * @brief :get run time
 */
uint32_t
hal_timer_get_timercounter(void)
{
    uint32_t temp;

    ENTER_CRITICAL();
    temp = gu32TimerCounter;
    EXIT_CRITICAL();
    return temp;
}

/**
 * @brief :get run time
 */
bool
hal_timer_get_50ms_flag(void)
{
    bool temp;

    ENTER_CRITICAL();
    temp = gboolTimer50msflag;
    EXIT_CRITICAL();
    return temp;
}

/**
 * @brief :get run time
 */
uint32_t
hal_timer_get_time(void)
{
    uint32_t temp;

    ENTER_CRITICAL();
    temp = gu32TimerRunTime;
    EXIT_CRITICAL();
    return temp;
}

/**
 * @brief :get run time
 */
void
hal_timer_set_50ms_flag(bool Flag)
{
    ENTER_CRITICAL();
    gboolTimer50msflag = Flag;
    EXIT_CRITICAL();
    return;
}

/**
 * @brief :set timer0 callback
 */
void
hal_timer0_callback_set(halTimerCallbackFunc Func)
{
    ghalTimer0Callback = Func;
}
/**
 * @brief :timer0 callback
 */
void
hal_timer0_callback(void)
{
    static uint16_t u16TimerTmp = 0;
    static uint16_t second_counter = 0;

    gu32TimerCounter ++;
    u16TimerTmp++;

    // 50ms flag
    if (u16TimerTmp >= 50)
    {
    	gboolTimer50msflag = true;
        u16TimerTmp = 0;

        second_counter++;
        if (second_counter >= 20)
        {
            second_counter = 0;
            // run time(s)
            gu32TimerRunTime ++;
        }
    }

    if (ghalTimer0Callback != NULL)
    {
        ghalTimer0Callback();
    }

}












