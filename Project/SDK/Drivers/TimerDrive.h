/*
 * TimerDrive.h
 *
 *  Created on: 2025Äê12ÔÂ27ÈÕ
 *      Author: huxl
 */

#ifndef SDK_DRIVERS_TIMERDRIVE_H_
#define SDK_DRIVERS_TIMERDRIVE_H_


#include "DSP2833x_Device.h"     // DSP2833x
#include "DSP2833x_Examples.h"   // DSP2833x



void TIM0_Init(float Freq, float Period);
void TIM0_DeInit(void);
extern interrupt void TIM0_ISR(void);


#endif /* SDK_DRIVERS_TIMERDRIVE_H_ */
