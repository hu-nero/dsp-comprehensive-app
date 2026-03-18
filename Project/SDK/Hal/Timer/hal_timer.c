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

// Timer1相关变量
static halTimerCallbackFunc ghalTimer1Callback = NULL;
static volatile bool gboolTimer1Timeout = false;  // 定时结束信号
static volatile uint32_t gu32Timer1TargetMs = 0;  // 目标时间(ms)
static volatile uint32_t gu32Timer1Counter = 0;   // Timer1计数器

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
                TIM1_Init(Freq, Period);
            }
            break;  // 添加break
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
                TIM1_DeInit();
            }
            break;  // 添加break
        case HAL_DEV_TIM2:
            {
                return 2;
            }
        default:return 3;
    }
    return 0;
}

/**
 * @brief :get time stamp(ms)
 */
uint32_t
hal_timer_get_timestamp(void)
{
    uint32_t temp;

    ENTER_CRITICAL();
    temp = gu32TimerCounter;
    EXIT_CRITICAL();
    return temp;
}

/**
 * @brief :get run time(s)
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
 * @brief :get 50ms flag
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
 * @brief :set 50ms flag
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

/**
 * @brief :设置timer1回调函数
 */
void
hal_timer1_callback_set(halTimerCallbackFunc Func)
{
    ghalTimer1Callback = Func;
}

/**
 * @brief :启动Timer1定时（毫秒级）
 * @param ms 定时时间(毫秒)
 * @note Timer1需要配置为1ms中断一次
 */
void
hal_timer1_start(uint32_t ms)
{
    ENTER_CRITICAL();
    gu32Timer1TargetMs = ms;
    gu32Timer1Counter = 0;
    gboolTimer1Timeout = false;
    EXIT_CRITICAL();

    // 确保Timer1已经初始化并启动
    // 如果Timer1未启动，可以在这里启动
    // CpuTimer1Regs.TCR.bit.TSS = 0;
}

/**
 * @brief :停止Timer1定时
 */
void
hal_timer1_stop(void)
{
    ENTER_CRITICAL();
    gu32Timer1TargetMs = 0;
    gu32Timer1Counter = 0;
    gboolTimer1Timeout = false;
    EXIT_CRITICAL();
}

/**
 * @brief :获取Timer1定时结束信号
 * @return true:定时结束, false:定时中或未启动
 */
bool
hal_timer1_get_timeout_flag(void)
{
    bool temp;

    ENTER_CRITICAL();
    temp = gboolTimer1Timeout;
    EXIT_CRITICAL();

    return temp;
}

/**
 * @brief :清除Timer1定时结束信号
 */
void
hal_timer1_clear_timeout_flag(void)
{
    ENTER_CRITICAL();
    gboolTimer1Timeout = false;
    EXIT_CRITICAL();
}

/**
 * @brief :获取Timer1当前计数值(ms)
 */
uint32_t
hal_timer1_get_current_count(void)
{
    uint32_t temp;

    ENTER_CRITICAL();
    temp = gu32Timer1Counter;
    EXIT_CRITICAL();

    return temp;
}

/**
 * @brief :获取Timer1剩余时间(ms)
 * @return 剩余毫秒数，0表示定时结束或未启动
 */
uint32_t
hal_timer1_get_remaining_time(void)
{
    uint32_t remaining;

    ENTER_CRITICAL();
    if(gu32Timer1TargetMs > gu32Timer1Counter)
    {
        remaining = gu32Timer1TargetMs - gu32Timer1Counter;
    }
    else
    {
        remaining = 0;
    }
    EXIT_CRITICAL();

    return remaining;
}

/**
 * @brief :timer1 callback (1ms中断一次)
 */
void
hal_timer1_callback(void)
{
    // 如果设置了目标时间且未超时
    if(gu32Timer1TargetMs > 0 && !gboolTimer1Timeout)
    {
        gu32Timer1Counter++;

        // 检查是否达到目标时间
        if(gu32Timer1Counter >= gu32Timer1TargetMs)
        {
            gboolTimer1Timeout = true;  // 给出定时结束信号

            // 自动停止计数（可选）
            // gu32Timer1TargetMs = 0;

            // 调用用户回调（如果有）
            if(ghalTimer1Callback != NULL)
            {
                ghalTimer1Callback();
            }
        }
    }

    // 其他Timer1相关处理可以在这里添加
}
