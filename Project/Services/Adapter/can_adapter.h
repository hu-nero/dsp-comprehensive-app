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

// 同步给区段柜状态字
union WordSyncToSectionStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};

// 并联电源并机状态
union WordParallelPwrParallelStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};

// 并联电源在线且无故障状态
union WordParallelPwrOnlineNoFaultStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};

// 区段实际并机状态
union WordSectionParallelStatus_t
{
	unsigned int all;
	struct
	{
		unsigned run_state:			2;		// bit0-1
		unsigned reserve:	    	14;		// bit2-15
	}	bit;
};
union WordSectionOnlineNoFaultStatus_t
{
	unsigned int all;
	struct
	{
		unsigned section_1_status:			1;// bit0
		unsigned section_2_status:			1;// bit1
		unsigned section_3_status:			1;// bit2
		unsigned section_4_status:			1;// bit3
		unsigned section_5_status:			1;// bit4
		unsigned section_6_status:			1;// bit5
		unsigned section_7_status:			1;// bit6
		unsigned section_8_status:			1;// bit7
		unsigned online_num:	    	    8;// bit8-15
	}	bit;
};

union WordSectionStatus_t
{
	unsigned int all;
	struct
	{
		unsigned heartbeat:			        1;// bit0
		unsigned sync_cabinet_online:		1;// bit1
		unsigned single_circuit:			1;// bit2
		unsigned parallel_circuit:			1;// bit3
		unsigned A3QS1_2:			        1;// bit4
		unsigned grid_connection_request:	1;// bit5
		unsigned grid_connection_done:      1;// bit6
		unsigned fault_signal:			    1;// bit7
		unsigned overdrive:			        1;// bit8
		unsigned reserve2:  	    	    7;// bit9-15
	}	bit;
};

typedef struct
{
    union WordSyncToSectionStatus_t sync_to_section_status;
    union WordParallelPwrParallelStatus_t parallel_pwr_parallel_status;
    union WordParallelPwrOnlineNoFaultStatus_t parallel_pwr_online_nofault_status;
    union WordSectionParallelStatus_t section_parallel_status;
    union WordSectionOnlineNoFaultStatus_t section_online_nofault_status;
} CommSyncToSection_t;

typedef struct
{
    union WordSectionStatus_t section_status;
} CommSectionToSync_t;

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
extern CommSyncToSection_t CAN_Adapter_GetSyncToSectionStatus(void);

// 发给同步柜
extern CommSectionToSync_t CAN_Adapter_GetCommSectionToSync(void);
extern void CAN_Adapter_SetCommSectionToSync(CommSectionToSync_t Status);

// CANB
extern bool CAN_Adapter_SendParallelRequest(void);

#endif /* SERVICES_ADAPTER_CAN_ADAPTER_H_ */
