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
#define CABINET_DEVICE_ADDR           0x01    // 区段柜设备地址
#define POWER_DEVICE_ADDR             0x02    // 电源设备地址
#define SYNC_CABINET_DEVICE_ADDR      0x01    // 电源设备地址

union WordPowerStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};

// 并网/解列状态定义
typedef enum
{
    GRID_DISCONNECTED = 0,    // 解列状态
    GRID_CONNECTED            // 并网状态
} GridConnectionState_t;

// 并网/解列状态结构体
typedef struct
{
    GridConnectionState_t state;     // 并网/解列状态
    uint32_t status_word;            // 状态字原始值
} GridConnectionStatus_t;

// 适配器接口
extern void CAN_Adapter_Init(void);
extern bool CAN_Adapter_SendPowerStart(void);
extern bool CAN_Adapter_SendPowerClose(void);
extern bool CAN_Adapter_SendStatusQuery(void);
extern bool CAN_Adapter_SendControlResponse(uint16_t DstAddr, ResponseStatus_t Status);
extern PowerStatus_t CAN_Adapter_GetPowerStatus(void);
extern void CAN_Adapter_ClearPowerStatus(void);
extern GridConnectionStatus_t CAN_Adapter_GetGridConnectionStatus(void);
extern void CAN_Adapter_ClearGridConnectionStatus(void);

// CANB
extern bool CAN_Adapter_SendParallelRequest(void);

#endif /* SERVICES_ADAPTER_CAN_ADAPTER_H_ */
