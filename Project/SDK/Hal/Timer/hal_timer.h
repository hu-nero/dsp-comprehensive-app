/*
 * hal_timer.h
 *
 *  Created on: 2025年12月27日
 *      Author: huxl
 */

#ifndef SDK_HAL_TIMER_HAL_TIMER_H_
#define SDK_HAL_TIMER_HAL_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "TimerDrive.h"

typedef enum
{
    HAL_DEV_TIM0 = 0,
    HAL_DEV_TIM1,
    HAL_DEV_TIM2,
    HAL_DEV_TIM_MAX_NUM
} HAL_DEV_TIM_Id;

typedef void (*halTimerCallbackFunc)(void);

extern uint16_t hal_timer_init(uint16_t PortId, float Freq, float Period);

/**
 * @brief :deinit timer
 *
 * @return :0 is success,1 is error
 */
extern uint16_t hal_timer_deinit(uint16_t PortId);

extern uint32_t hal_timer_get_timestamp(void);
extern uint32_t hal_timer_get_time(void);
extern bool hal_timer_get_50ms_flag(void);

extern void hal_timer_set_50ms_flag(bool Flag);
/**
 * @brief :set timer0 callback
 */
extern void hal_timer0_callback_set(halTimerCallbackFunc Func);
/**
 * @brief :timer0 callback
 */
extern void hal_timer0_callback(void);

// Timer1相关函数
/**
 * @brief :set timer1 callback
 */
extern void hal_timer1_callback_set(halTimerCallbackFunc Func);
/**
 * @brief :timer1 callback
 */
extern void hal_timer1_callback(void);

/**
 * @brief :启动Timer1定时（毫秒级）
 * @param ms 定时时间(毫秒)
 */
extern void hal_timer1_start(uint32_t ms);

/**
 * @brief :停止Timer1定时
 */
extern void hal_timer1_stop(void);

/**
 * @brief :获取Timer1定时结束信号
 * @return true:定时结束, false:定时中或未启动
 */
extern bool hal_timer1_get_timeout_flag(void);

/**
 * @brief :清除Timer1定时结束信号
 */
extern void hal_timer1_clear_timeout_flag(void);

/**
 * @brief :获取Timer1当前计数值(ms)
 */
extern uint32_t hal_timer1_get_current_count(void);

/**
 * @brief :获取Timer1剩余时间(ms)
 */
extern uint32_t hal_timer1_get_remaining_time(void);

#endif /* SDK_HAL_TIMER_HAL_TIMER_H_ */
