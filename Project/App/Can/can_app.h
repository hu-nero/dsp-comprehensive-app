/*
 * can_app.h
 *
 *  Created on: 2025Äê12ÔÂ25ÈÕ
 *      Author: huxl
 */

#ifndef APP_CAN_CAN_APP_H_
#define APP_CAN_CAN_APP_H_

#include "SDK/Agent/Can/can_agent.h"

extern void CAN_App_Init(void);
extern void CAN_App_MainLoop(void);
extern bool CAN_App_SendParam(uint16_t DstAddr, ParamDevice_t Device, uint16_t *Data, uint16_t TotalLength);

typedef void (*CAN_App_func_status_handler_t)(uint16_t *Data, uint16_t Len);
extern void CAN_App_Set_func_status_handler(CAN_App_func_status_handler_t Handler);
extern void CAN_App_Test(void);

#endif /* APP_CAN_CAN_APP_H_ */
