/*
 * can_adapter.c
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#include "can_adapter.h"
#include "App/Can/can_app.h"
#include <string.h>

static PowerStatus_t gPowerStatus;
static GridConnectionStatus_t gGridConnectionStatus;
static CommSyncToSection_t gCommSyncToSectionStatus;
static CommSectionToSync_t gCommSectionToSyncStatus;
static CommSectionToPower_t gCommSectionToPowerStatus;
static CommPowerToSection_t gCommPowerToSectionParam;


void CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr);

static void CAN_Adapter_ParsePowerParam(uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_ParseSyncCabinetStatus(uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc);

// u8转u16
static inline
uint16_t CombineBytes(uint8_t Lo, uint8_t Hi)
{
    return (uint16_t)(Lo | (Hi << 8));
}

PowerStatus_t
CAN_Adapter_GetPowerStatus(void)
{
	union WordPowerSectionInteractStatus_t status;
	status.all = gCommPowerToSectionParam.power_section_interact_word.u16param;
    switch (status.bit.WorkSta)
    {
		case 0:
		{
			gPowerStatus.run_state = POWER_STANDBY;
		}
		break;
		case 1:
		{
			gPowerStatus.run_state = POWER_SINGLE_RUNNING;
		}
		break;
		case 2:
		{
			gPowerStatus.run_state = POWER_PARALLEL_RUNNING;
		}
		break;
		case 3:
		{
			gPowerStatus.run_state = POWER_FAULT;
		}
		break;
    }
    return gPowerStatus;
}

void
CAN_Adapter_ClearPowerStatus(void)
{
    gPowerStatus.single_to_parallel_flag = false;
}

GridConnectionStatus_t
CAN_Adapter_GetGridConnectionStatus(void)
{
    return gGridConnectionStatus;
}

void
CAN_Adapter_ClearGridConnectionStatus(void)
{
    gGridConnectionStatus.state = GRID_DISCONNECTED;
    gGridConnectionStatus.status_word = 0;
}

CommSectionToSync_t
CAN_Adapter_GetCommSectionToSync(void)
{
    return gCommSectionToSyncStatus;
}

void
CAN_Adapter_SetCommSectionToSync(CommSectionToSync_t Status)
{
    gCommSectionToSyncStatus = Status;
}

CommSyncToSection_t
CAN_Adapter_GetSyncToSectionStatus(void)
{
    return gCommSyncToSectionStatus;
}

// 适配器初始化
void
CAN_Adapter_Init(void)
{
    // 初始化电源状态
    gPowerStatus.run_state = POWER_UNKNOWN;
    gPowerStatus.single_to_parallel_flag = false;
    gPowerStatus.status_word = 0;

    // 初始化并网/解列状态
    gGridConnectionStatus.state = GRID_DISCONNECTED;
    gGridConnectionStatus.status_word = 0;

    // 初始化柜体间通信状态
    gCommSyncToSectionStatus.sync_status.all = 0;
    gCommSectionToSyncStatus.section_status.all = 0;
    gCommSectionToPowerStatus.section_status.all = 0;
    memset(&gCommPowerToSectionParam, 0, sizeof(CommPowerToSection_t));
    // 回调
    CAN_App_Set_func_status_handler(CAN_Adapter_RxHandler);
    CAN_Agent_Set_SendStatus_Callback(eCanPort_0, CAN_Adapter_SendStatus_Callback);
    CAN_Agent_Set_SendStatus_Callback(eCanPort_1, CAN_Adapter_SendStatus_Callback);

}

// 发送并网请求指令
bool
CAN_Adapter_SendParallelRequest(void)
{
    uint16_t data[8] = {};
    return CAN_Agent_SendStatus(eCanPort_1, SYNC_CABINET_DEVICE_ADDR, data);
}

// 发送启动电源命令
bool
CAN_Adapter_SendPowerStart(void)
{
    return CAN_Agent_SendControl(eCanPort_0, POWER_DEVICE_ADDR, CMD_START, 0x01);
}

// 发送关闭电源命令
bool
CAN_Adapter_SendPowerClose(void)
{
    return CAN_Agent_SendControl(eCanPort_0, POWER_DEVICE_ADDR, CMD_STOP, 0x01);
}
// 发送状态查询命令
bool
CAN_Adapter_SendStatusQuery(void)
{
    // 通过发送控制命令查询状态
    return CAN_Agent_SendControl(eCanPort_0, POWER_DEVICE_ADDR, CMD_ACK, 0x00);
}

// 发送控制响应
bool
CAN_Adapter_SendControlResponse(uint16_t DstAddr, ResponseStatus_t Status)
{
    return CAN_Agent_SendCtrlResp(eCanPort_0, DstAddr, Status);
}


// 解析电源参数
static void
CAN_Adapter_ParsePowerParam(uint16_t *Data, uint16_t Dlc)
{
    uint16_t block;
    uint16_t state;

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    block = (Data[0] & 0xFF) | (Data[1] << 8);

    switch (block)
    {
        case 0:
            {
                gCommPowerToSectionParam.input_AB_line_voltage = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_BC_line_voltage = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.input_CA_line_voltage = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 1:
            {
                gCommPowerToSectionParam.input_A_phase_voltage = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_B_phase_voltage = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.input_C_phase_voltage = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 2:
            {
                gCommPowerToSectionParam.input_A_phase_current = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_B_phase_current = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.input_C_phase_current = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 3:
            {
                gCommPowerToSectionParam.input_apparent_power = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_active_power = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.input_reactive_power = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 4:
            {
                gCommPowerToSectionParam.input_power_factor = CombineBytes(Data[2], Data[3]) * 100;
                gCommPowerToSectionParam.input_energy_high_byte = CombineBytes(Data[2], Data[3]) * 1;
                gCommPowerToSectionParam.input_energy_low_byte = CombineBytes(Data[4], Data[5]) * 1;
            }
            break;
        case 5:
            {
                gCommPowerToSectionParam.input_transformer_temp = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_reactor_temp = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_reactor_temp = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 6:
            {
                gCommPowerToSectionParam.output_AB_line_voltage = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.output_BC_line_voltage = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_CA_line_voltage = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 7:
            {
                gCommPowerToSectionParam.output_A_phase_voltage = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.output_B_phase_voltage = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_C_phase_voltage = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 8:
            {
                gCommPowerToSectionParam.output_A_phase_current = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.output_B_phase_current = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_C_phase_current = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 9:
            {
                gCommPowerToSectionParam.output_apparent_power = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.output_active_power = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_reactive_power = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 10:
            {
                gCommPowerToSectionParam.output_power_factor = CombineBytes(Data[2], Data[3]) * 100;
                gCommPowerToSectionParam.dc_bus_voltage = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.input_frequency = CombineBytes(Data[4], Data[5]) * 10;
            }
            break;
        case 11:
            {
                gCommPowerToSectionParam.input_igbt_temp = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.output_igbt_temp = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.output_frequency = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        case 12:
            {
                gCommPowerToSectionParam.coolant_flow_rate = CombineBytes(Data[2], Data[3]) * 10;
                gCommPowerToSectionParam.coolant_inlet_temp = CombineBytes(Data[4], Data[5]) * 10;
                gCommPowerToSectionParam.coolant_outlet_temp = CombineBytes(Data[6], Data[7]) * 10;
            }
            break;
        // 状态字
        case 13:
            {
                gCommPowerToSectionParam.power_fault_word_1.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_fault_word_1.u8param[1] = Data[3];
                gCommPowerToSectionParam.power_fault_word_2.u8param[0] = Data[4];
                gCommPowerToSectionParam.power_fault_word_2.u8param[1] = Data[5];
                gCommPowerToSectionParam.power_fault_word_3.u8param[0] = Data[6];
                gCommPowerToSectionParam.power_fault_word_3.u8param[1] = Data[7];
            }
            break;
        case 14:
            {
                gCommPowerToSectionParam.power_fault_word_4.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_fault_word_4.u8param[1] = Data[3];
                gCommPowerToSectionParam.power_fault_word_5.u8param[0] = Data[4];
                gCommPowerToSectionParam.power_fault_word_5.u8param[1] = Data[5];
                gCommPowerToSectionParam.power_fault_word_6.u8param[0] = Data[6];
                gCommPowerToSectionParam.power_fault_word_6.u8param[1] = Data[7];
            }
            break;
        case 15:
            {
                gCommPowerToSectionParam.power_alarm_word_1.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_alarm_word_1.u8param[1] = Data[3];
                gCommPowerToSectionParam.power_alarm_word_2.u8param[0] = Data[4];
                gCommPowerToSectionParam.power_alarm_word_2.u8param[1] = Data[5];
                gCommPowerToSectionParam.power_alarm_word_3.u8param[0] = Data[6];
                gCommPowerToSectionParam.power_alarm_word_3.u8param[1] = Data[7];
            }
            break;
        case 16:
            {
                gCommPowerToSectionParam.power_fault_word_4.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_fault_word_4.u8param[1] = Data[3];
                gCommPowerToSectionParam.power_fault_word_5.u8param[0] = Data[4];
                gCommPowerToSectionParam.power_fault_word_5.u8param[1] = Data[5];
                gCommPowerToSectionParam.power_fault_word_6.u8param[0] = Data[6];
                gCommPowerToSectionParam.power_fault_word_6.u8param[1] = Data[7];
            }
            break;
        case 17:
            {
                gCommPowerToSectionParam.power_status_word_1.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_status_word_1.u8param[1] = Data[3];
                gCommPowerToSectionParam.power_status_word_2.u8param[0] = Data[4];
                gCommPowerToSectionParam.power_status_word_2.u8param[1] = Data[5];
                gCommPowerToSectionParam.power_status_word_3.u8param[0] = Data[6];
                gCommPowerToSectionParam.power_status_word_3.u8param[1] = Data[7];
            }
            break;
        case 18:
            {
                gCommPowerToSectionParam.power_section_interact_word.u8param[0] = Data[2];
                gCommPowerToSectionParam.power_section_interact_word.u8param[1] = Data[3];
            }
            break;
        default:
            break;
    }
}

// 解析同步柜状态字
static void
CAN_Adapter_ParseSyncCabinetStatus(uint16_t *Data, uint16_t Dlc)
{
    uint16_t state;

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    state = (Data[0]&0xFF)|(Data[1]<<8);
    gCommSyncToSectionStatus.sync_status.all = state;
}

// 接收回调
static void
CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc)
{
    if (Port == eCanPort_0)
    {
        CAN_Adapter_ParsePowerParam(Data, Dlc);
    }
    else if (Port == eCanPort_1)
    {
        CAN_Adapter_ParseSyncCabinetStatus(Data, Dlc);
    }
}

// 状态发送回调
void
CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr)
{
    uint16_t data[8]={0};
    if (Port == eCanPort_0)
    {
        uint16_t value = gCommSectionToPowerStatus.section_status.all;
        data[2] = value & 0xFF;
        data[3] = (value >> 8) & 0xFF;
        CAN_Agent_SendStatus(Port, DEV_ADDRESS_BROADCAST, data);
    }
    else if (Port == eCanPort_1)
    {
        uint16_t value = gCommSectionToSyncStatus.section_status.all;
        data[0] = value & 0xFF;
        data[1] = (value >> 8) & 0xFF;
        CAN_Agent_SendStatus(Port, DEV_ADDRESS_BROADCAST, data);
    }
}

