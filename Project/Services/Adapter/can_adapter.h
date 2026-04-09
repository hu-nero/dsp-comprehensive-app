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
#define POWER_DEVICE_ADDR             0x02    // 电源设备地址
#define SYNC_CABINET_DEVICE_ADDR      0x01    // 电源设备地址

union WordPowerStatus2_t
{
	unsigned int all;
	struct
	{
		unsigned MprCanNpr:			1;		// bit0-1
		unsigned PwrUartMA:			1;		// bit0-1
		unsigned PwrUartMB:			1;		// bit0-1
		unsigned LocRom:			1;		// bit0-1
		unsigned NprComHMI:			1;		// bit0-1
		unsigned MprComHMI:			1;		// bit0-1
		unsigned A2QF2:	    		1;		// bit0-1
		unsigned powerstate:		2;		// bit0-1
		unsigned reserve:	    	7;		// bit2-15
	}	bit;
};

union WordPowerSectionInteractStatus_t
{
	unsigned int all;
	struct
	{
		unsigned CombStep:			4;		// bit0-3
		unsigned SplitStep:			4;		// bit4-7
		unsigned WorkSta:			2;		// bit8-9
		unsigned reserve:	    	5;		// bit10-14
		unsigned Toggle:	    	1;		// bit15
	}	bit;
};

// 区段柜->电源柜
union WordSectionPowerInteractStatus_t
{
	unsigned int all;
	struct
	{
		unsigned CombStep:			4;		// bit0-3
		unsigned SplitStep:			4;		// bit4-7
		unsigned WorkSta:			2;		// bit8-9
		unsigned FaultSta:			1;		// bit10
		unsigned reserve:	    	5;		// bit11-14
		unsigned Toggle:	    	1;		// bit15
	}	bit;
};

// 同步柜->区段柜
union WordSyncStatus_t
{
	unsigned int all;
	struct
	{
		unsigned heartbeat:			        1;// bit0
		unsigned section_grid_feedback:		4;// bit1-4
		unsigned sync_grid_state:    		1;// bit5
		unsigned off_grid_all:			    1;// bit6
		unsigned is_parallel_factor_satisfy:1;// bit7
		unsigned sync_is_master:			1;// bit8
		unsigned reserve:  	    	        7;// bit9-15
	}	bit;
};

// 区段柜->同步柜
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

// 电源柜->区段柜
typedef struct
{
    uint16_t input_AB_line_voltage;      // 输入AB线电压
    uint16_t input_BC_line_voltage;      // 输入BC线电压
    uint16_t input_CA_line_voltage;      // 输入CA线电压
    uint16_t input_A_phase_voltage;      // 输入A相电压
    uint16_t input_B_phase_voltage;      // 输入B相电压
    uint16_t input_C_phase_voltage;      // 输入C相电压
    uint16_t input_A_phase_current;      // 输入A相电流
    uint16_t input_B_phase_current;      // 输入B相电流
    uint16_t input_C_phase_current;      // 输入C相电流
    uint16_t input_apparent_power;       // 输入视在功率
    uint16_t input_active_power;         // 输入有功功率
    uint16_t input_reactive_power;       // 输入无功功率
    uint16_t input_power_factor;         // 输入功率因数
    uint16_t input_energy_high_byte;     // 输入电量高字节
    uint16_t input_energy_low_byte;      // 输入电量低字节
    uint16_t input_transformer_temp;     // 输入变压器温度
    uint16_t input_reactor_temp;         // 输入电抗器温度
    uint16_t output_reactor_temp;        // 输出电抗器温度
    uint16_t output_AB_line_voltage;     // 输出AB线电压
    uint16_t output_BC_line_voltage;     // 输出BC线电压
    uint16_t output_CA_line_voltage;     // 输出CA线电压
    uint16_t output_A_phase_voltage;     // 输出A相电压
    uint16_t output_B_phase_voltage;     // 输出B相电压
    uint16_t output_C_phase_voltage;     // 输出C相电压
    uint16_t output_A_phase_current;     // 输出A相负载电流
    uint16_t output_B_phase_current;     // 输出B相负载电流
    uint16_t output_C_phase_current;     // 输出C相负载电流
    uint16_t output_apparent_power;      // 输出视在功率
    uint16_t output_active_power;        // 输出有功功率
    uint16_t output_reactive_power;      // 输出无功功率
    uint16_t output_power_factor;        // 输出功率因数
    uint16_t dc_bus_voltage;             // 直流母线电压
    uint16_t input_frequency;            // 输入频率
    uint16_t input_igbt_temp;            // 输入IGBT温度
    uint16_t output_igbt_temp;           // 输出IGBT温度
    uint16_t output_frequency;           // 输出频率
    uint16_t coolant_flow_rate;          // 冷却水流量
    uint16_t coolant_inlet_temp;         // 冷却水进水温度
    uint16_t coolant_outlet_temp;        // 冷却水出水温度

    uint16_t power_fault_word_1;         // 电源故障字1
    uint16_t power_fault_word_2;         // 电源故障字2
    uint16_t power_fault_word_3;         // 电源故障字3
    uint16_t power_fault_word_4;         // 电源故障字4
    uint16_t power_fault_word_5;         // 电源故障字5
    uint16_t power_fault_word_6;         // 电源故障字6
    uint16_t power_alarm_word_1;         // 电源告警字1
    uint16_t power_alarm_word_2;         // 电源告警字2
    uint16_t power_alarm_word_3;         // 电源告警字3
    uint16_t power_alarm_word_4;         // 电源告警字4
    uint16_t power_alarm_word_5;         // 电源告警字5
    uint16_t power_alarm_word_6;         // 电源告警字6
    uint16_t power_status_word_1;        // 电源状态字1
    uint16_t power_status_word_2;        // 电源状态字2
    uint16_t power_status_word_3;        // 电源状态字3
    uint16_t power_section_interact_word;// 电源区段交互字
} CommPowerToSection_t;

// 区段柜->电源柜
typedef struct
{
    union WordSectionPowerInteractStatus_t section_status;
} CommSectionToPower_t;

// 同步柜->区段柜
typedef struct
{
    union WordSyncStatus_t sync_status;
} CommSyncToSection_t;

// 区段柜->同步柜
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

typedef struct
{
    bool sync_online_status;        // 同步柜在线状态
    uint32_t sync_online_start;     // 同步柜在线计时
    bool power_online_status;       // 电源柜在线状态
    uint32_t power_online_start;    // 电源柜在线计时
} AdapterStatus_t;

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

extern AdapterStatus_t CAN_Adapter_GetAdapterStatus(void);
// 柜体交互
extern CommSyncToSection_t CAN_Adapter_GetSyncToSectionStatus(void);
extern CommPowerToSection_t CAN_Adapter_GetCommPowerToSectionParam(void);
extern uint16_t CAN_Adapter_GetCommPowerToSectionStatus1(void);

extern CommSectionToPower_t CAN_Adapter_GetCommSectionToPower(void);
extern void CAN_Adapter_SetCommSectionToPower(CommSectionToPower_t Status);
extern CommSectionToSync_t CAN_Adapter_GetCommSectionToSync(void);
extern void CAN_Adapter_SetCommSectionToSync(CommSectionToSync_t Status);

// CANB
extern bool CAN_Adapter_SendParallelRequest(void);

#endif /* SERVICES_ADAPTER_CAN_ADAPTER_H_ */
