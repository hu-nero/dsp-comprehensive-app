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

extern uint32_t hal_timer_get_timercounter(void);
extern bool hal_timer_get_50ms_flag(void);
extern uint32_t hal_timer_get_time(void);

extern void hal_timer_set_50ms_flag(bool Flag);
/**
 * @brief :set timer0 callback
 */
extern void hal_timer0_callback_set(halTimerCallbackFunc Func);
/**
 * @brief :timer0 callback
 */
extern void hal_timer0_callback(void);






#endif /* SDK_HAL_TIMER_HAL_TIMER_H_ */
