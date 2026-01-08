/*
 * event.h
 *
 *  Created on: 2025Äê12ÔÂ17ÈÕ
 *      Author: huxl
 */

#ifndef SDK_HAL_EVENT_EVENT_H_
#define SDK_HAL_EVENT_EVENT_H_

#include "Hal.h"

interrupt void CANA_TxRx_ISR(void);
interrupt void CANA_Error_ISR(void);

interrupt void TIM0_ISR(void);


#endif /* SDK_HAL_EVENT_EVENT_H_ */
