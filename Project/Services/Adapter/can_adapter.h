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

// 设备地址定义
#define CABINET_DEVICE_ADDR    0x01    // 区段柜设备地址
#define POWER_DEVICE_ADDR      0x02    // 电源设备地址

union WordPowerStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};

// 适配器接口
extern void CAN_Adapter_Init(void);
extern bool CAN_Adapter_SendPowerStart(void);
extern bool CAN_Adapter_SendPowerClose(void);
extern bool CAN_Adapter_SendStatusQuery(void);
extern bool CAN_Adapter_SendControlResponse(uint16_t DstAddr, ResponseStatus_t Status);
extern PowerStatus_t CAN_Adapter_GetPowerStatus(void);

#endif /* SERVICES_ADAPTER_CAN_ADAPTER_H_ */
