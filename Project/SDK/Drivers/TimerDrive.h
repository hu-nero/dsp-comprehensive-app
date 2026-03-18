/*
 * TimerDrive.h
 *
 *  Created on: 2025年12月27日
 *      Author: huxl
 */

#ifndef SDK_DRIVERS_TIMERDRIVE_H_
#define SDK_DRIVERS_TIMERDRIVE_H_


#include "DSP2833x_Device.h"     // DSP2833x
#include "DSP2833x_Examples.h"   // DSP2833x



void TIM0_Init(float Freq, float Period);
void TIM1_Init(float Freq, float Period);
void TIM0_DeInit(void);
void TIM1_DeInit(void);
extern interrupt void TIM0_ISR(void);
extern interrupt void TIM1_ISR(void);


#endif /* SDK_DRIVERS_TIMERDRIVE_H_ */
