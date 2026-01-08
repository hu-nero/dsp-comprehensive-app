/*
 * can_adapter.h
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#ifndef SERVICES_ADAPTER_CAN_ADAPTER_H_
#define SERVICES_ADAPTER_CAN_ADAPTER_H_

#include <stdint.h>
#include <stdbool.h>
#include "SDK/Agent/Can/can_agent.h"
#include "Services/Logic/section_common.h"

/* 设备地址定义 */
#define CABINET_DEVICE_ADDR    0x01    /* 区段柜设备地址 */
#define POWER_DEVICE_ADDR      0x02    /* 电源设备地址 */
union WordPowerStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned local_remote:		1;		// bit2
		unsigned single_parallel:	1;		// bit3
		unsigned reserve:	    	12;		// bit4-15
	}	bit;
};

/* CAN消息接收回调函数 */
typedef void (*CAN_Adapter_RxHandler_t)(FuncCode_t func, uint16_t src_addr, uint16_t *data, uint16_t dlc);

/* 适配器接口 */
void CAN_Adapter_Init(void);
bool CAN_Adapter_SendPowerStart(void);
bool CAN_Adapter_SendStatusQuery(void);
bool CAN_Adapter_SendControlResponse(uint16_t dst_addr, ResponseStatus_t status);
void CAN_Adapter_ProcessReceivedMessage(FuncCode_t func, uint16_t src_addr, uint16_t *data, uint16_t dlc);

extern PowerStatus_t CAN_Adapter_GetPowerStatus(void);

#endif /* SERVICES_ADAPTER_CAN_ADAPTER_H_ */
