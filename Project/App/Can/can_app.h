/*
 * can_app.h
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */

#ifndef APP_CAN_CAN_APP_H_
#define APP_CAN_CAN_APP_H_

#include "SDK/Agent/Can/can_agent.h"
#include "Services/Logic/section_cabinet.h"

typedef struct
{
    uint16_t A3QF1TurnOnStatus;
    uint16_t A3QF1TurnOffStatus;
    uint16_t A3QF2TurnOnStatus;
    uint16_t A3QF2TurnOffStatus;
    uint32_t A3QF1TurnOntimerstamp;
    uint32_t A3QF1TurnOfftimerstamp;
    uint32_t A3QF2TurnOntimerstamp;
    uint32_t A3QF2TurnOfftimerstamp;
} CanAppA3QFxStatus_t;

extern void CAN_App_Init(void);
extern void CAN_App_MainLoop(void);
extern bool CAN_App_SendParam(TeCanPort port, uint16_t DstAddr, ParamDevice_t Device, uint16_t *Data, uint16_t TotalLength);

typedef void (*CAN_App_func_status_handler_t)(TeCanPort Port, uint16_t *Data, uint16_t Len);
extern void CAN_App_Set_func_status_handler(CAN_App_func_status_handler_t Handler);
extern void CAN_App_RecordPanelSwitchStatus(void);
extern void CAN_App_Switch_Init(void);

#endif /* APP_CAN_CAN_APP_H_ */
