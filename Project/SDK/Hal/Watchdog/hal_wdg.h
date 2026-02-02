/*
 * hal_wdg.h
 *
 *  Created on: 2026年2月2日
 *      Author: huxl
 */
#ifndef _HAL_WDG_H_
#define _HAL_WDG_H_
#include <stdint.h>

typedef void (*halWdgCallbackFunc)(void);

void HAL_WDG_Init(void);
void HAL_WDG_Enable(void);
void HAL_WDG_Disable(void);
void HAL_WDG_Feed(void);
void hal_wdg_callback(void);

extern interrupt void WDG_ISR(void);

#endif /* _HAL_WDG_H_ */
