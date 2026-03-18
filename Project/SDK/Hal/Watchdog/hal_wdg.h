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

void hal_wdg_init(void);
void hal_wdg_enable(void);
void hal_wdg_disable(void);
void hal_wdg_feed(void);
void hal_wdg_callback(void);

extern interrupt void WDG_ISR(void);

#endif /* _HAL_WDG_H_ */
