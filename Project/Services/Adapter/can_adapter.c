/*
 * can_adapter.c
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#include "can_adapter.h"
#include "App/Can/can_app.h"
#include "SDK/Hal/Timer/hal_timer.h"
#include <string.h>

static AdapterStatus_t gAdapterStatus;
static PowerStatus_t gPowerStatus;
static u16 gSyncToSection_PwrToPwrless;
static GridConnectionStatus_t gGridConnectionStatus;
static CommSyncToSection_t gCommSyncToSectionStatus;
static CommSectionToSync_t gCommSectionToSyncStatus;
static CommSectionToPower_t gCommSectionToPowerStatus;
static CommPowerToSection_t gCommPowerToSectionParam;
static uint32_t gSingleToParaCount,gPwrToPwrlessCount;
uint8_t byteData[8] = {0};

// 回调
void CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr);
static void CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_MainLoop_Callback(TeCanPort Port);

static void CAN_Adapter_ParsePowerParam(uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_ParseSyncCabinetStatus(uint16_t *Data, uint16_t Dlc);
// 转u16
static inline
uint16_t CombineBytes(uint16_t Lo, uint16_t Hi)
{
    return (uint16_t)((Lo&0xFF) | ((Hi&0xFF) << 8));
}



PowerStatus_t
CAN_Adapter_GetPowerStatus(void)
{
    static uint16_t tmp_single_to_parallel_flag = 0;
	union WordPowerSectionInteractStatus_t status;
	status.all = gCommPowerToSectionParam.power_section_interact_word;
    switch (status.bit.WorkSta)
    {
		case 0:
		{
            tmp_single_to_parallel_flag = 0;
			gPowerStatus.run_state = POWER_STANDBY;
		}
		break;
		case 1:
		{
			gPowerStatus.run_state = POWER_SINGLE_RUNNING;
            tmp_single_to_parallel_flag = 1;
		}
		break;
		case 2:
		{
			gPowerStatus.run_state = POWER_PARALLEL_RUNNING;
            if (tmp_single_to_parallel_flag == 1)
            {
                gPowerStatus.single_to_parallel_flag = true;
                gSingleToParaCount = hal_timer_get_timestamp();
            }
            tmp_single_to_parallel_flag = 2;
		}
		break;
		case 3:
		{
            tmp_single_to_parallel_flag = 3;
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

CommPowerToSection_t
CAN_Adapter_GetCommPowerToSectionParam(void)
{
    return gCommPowerToSectionParam;
}

uint16_t
CAN_Adapter_GetCommPowerToSectionStatus1(void)
{
    return gCommPowerToSectionParam.power_status_word_1;
}

CommSectionToPower_t
CAN_Adapter_GetCommSectionToPower(void)
{
    return gCommSectionToPowerStatus;
}

void
CAN_Adapter_SetCommSectionToPower(CommSectionToPower_t Status)
{
    gCommSectionToPowerStatus = Status;
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

uint16_t
CAN_Adapter_GetAvoidOverdrive(void)
{
    return gCommSectionToSyncStatus.section_status.bit.avoid_overdrive;
}

void
CAN_Adapter_SetAvoidOverdrive(uint16_t Status)
{
    gCommSectionToSyncStatus.section_status.bit.avoid_overdrive = Status&1;
}

CommSyncToSection_t
CAN_Adapter_GetCommSyncToSectionStatus(void)
{
    return gCommSyncToSectionStatus;
}

uint16_t
CAN_Adapter_GetCommPowerToSectionStatus(void)
{
    return gCommPowerToSectionParam.power_section_interact_word;
}

AdapterStatus_t
CAN_Adapter_GetAdapterStatus(void)
{
    return gAdapterStatus;
}

void
CAN_Adapter_SetMaintenStatus(u16 Status)
{
    gSyncToSection_PwrToPwrless = Status;
}

u16
CAN_Adapter_GetMaintenStatus(void)
{
    return gSyncToSection_PwrToPwrless;
}
// 适配器初始化
void
CAN_Adapter_Init(void)
{
    // 初始化适配器结构体
    memset(&gAdapterStatus, 0, sizeof(AdapterStatus_t));
    gAdapterStatus.power_online_status = true;
    gAdapterStatus.sync_online_status = true;

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
    // 初始默认电源处于工况2
    gCommPowerToSectionParam.power_status_word_1 = 0x10;
    // 回调
    CAN_App_Set_func_status_handler(CAN_Adapter_RxHandler);
    CAN_Agent_Set_SendStatus_Callback(eCanPort_0, CAN_Adapter_SendStatus_Callback);
    CAN_Agent_Set_SendStatus_Callback(eCanPort_1, CAN_Adapter_SendStatus_Callback);
    CAN_Agent_Set_MainLoop_Callback(eCanPort_0, CAN_Adapter_MainLoop_Callback);
    CAN_Agent_Set_MainLoop_Callback(eCanPort_1, CAN_Adapter_MainLoop_Callback);
    // 单转并计时
    gSingleToParaCount = 0;

    // 同步柜 检修有电转无电
    gSyncToSection_PwrToPwrless = 0;
    gPwrToPwrlessCount = 0;

}

// 发送并网请求指令
bool
CAN_Adapter_SendParallelRequest(void)
{
    gCommSectionToSyncStatus.section_status.bit.grid_connection_request = 1;
    return true;
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

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    block = (Data[0] & 0xFF) | ((Data[1] & 0xFF) << 8);

    switch (block)
    {
        case 0:
            {
                gCommPowerToSectionParam.input_AB_line_voltage = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_BC_line_voltage = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.input_CA_line_voltage = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 1:
            {
                gCommPowerToSectionParam.input_A_phase_voltage = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_B_phase_voltage = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.input_C_phase_voltage = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 2:
            {
                gCommPowerToSectionParam.input_A_phase_current = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_B_phase_current = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.input_C_phase_current = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 3:
            {
                gCommPowerToSectionParam.input_apparent_power = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_active_power = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.input_reactive_power = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 4:
            {
                gCommPowerToSectionParam.input_power_factor = CombineBytes(Data[2], Data[3]) / 100;
                gCommPowerToSectionParam.input_energy_high_byte = CombineBytes(Data[2], Data[3]) / 1;
                gCommPowerToSectionParam.input_energy_low_byte = CombineBytes(Data[4], Data[5]) / 1;
            }
            break;
        case 5:
            {
                gCommPowerToSectionParam.input_transformer_temp = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_reactor_temp = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_reactor_temp = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 6:
            {
                gCommPowerToSectionParam.output_AB_line_voltage = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.output_BC_line_voltage = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_CA_line_voltage = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 7:
            {
                gCommPowerToSectionParam.output_A_phase_voltage = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.output_B_phase_voltage = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_C_phase_voltage = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 8:
            {
                gCommPowerToSectionParam.output_A_phase_current = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.output_B_phase_current = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_C_phase_current = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 9:
            {
                gCommPowerToSectionParam.output_apparent_power = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.output_active_power = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_reactive_power = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 10:
            {
                gCommPowerToSectionParam.output_power_factor = CombineBytes(Data[2], Data[3]) / 100;
                gCommPowerToSectionParam.dc_bus_voltage = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.input_frequency = CombineBytes(Data[4], Data[5]) / 10;
            }
            break;
        case 11:
            {
                gCommPowerToSectionParam.input_igbt_temp = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.output_igbt_temp = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.output_frequency = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        case 12:
            {
                gCommPowerToSectionParam.coolant_flow_rate = CombineBytes(Data[2], Data[3]) / 10;
                gCommPowerToSectionParam.coolant_inlet_temp = CombineBytes(Data[4], Data[5]) / 10;
                gCommPowerToSectionParam.coolant_outlet_temp = CombineBytes(Data[6], Data[7]) / 10;
            }
            break;
        // 状态字
        case 13:
            {
                gCommPowerToSectionParam.power_fault_word_1 = CombineBytes(Data[2],Data[3]);
                gCommPowerToSectionParam.power_fault_word_2 = CombineBytes(Data[4],Data[5]);
                gCommPowerToSectionParam.power_fault_word_3 = CombineBytes(Data[6],Data[7]);
            }
            break;
        case 14:
            {
                gCommPowerToSectionParam.power_fault_word_4 = CombineBytes(Data[2],Data[3]);
                gCommPowerToSectionParam.power_fault_word_5 = CombineBytes(Data[4],Data[5]);
                gCommPowerToSectionParam.power_fault_word_6 = CombineBytes(Data[6],Data[7]);
            }
            break;
        case 15:
            {
                gCommPowerToSectionParam.power_alarm_word_1 = CombineBytes(Data[2],Data[3]);
                gCommPowerToSectionParam.power_alarm_word_2 = CombineBytes(Data[4],Data[5]);
                gCommPowerToSectionParam.power_alarm_word_3 = CombineBytes(Data[6],Data[7]);
            }
            break;
        case 16:
            {
                gCommPowerToSectionParam.power_fault_word_4 = CombineBytes(Data[2],Data[3]);
                gCommPowerToSectionParam.power_fault_word_5 = CombineBytes(Data[4],Data[5]);
                gCommPowerToSectionParam.power_fault_word_6 = CombineBytes(Data[6],Data[7]);
            }
            break;
        case 17:
            {
                gCommPowerToSectionParam.power_status_word_1 = CombineBytes(Data[2],Data[3]);
                gCommPowerToSectionParam.power_status_word_2 = CombineBytes(Data[4],Data[5]);
                gCommPowerToSectionParam.power_status_word_3 = CombineBytes(Data[6],Data[7]);
            }
            break;
        case 18:
            {
                /*gCommPowerToSectionParam.power_section_interact_word = (Data[2]&0xFF)|((Data[3]&0xFF)<<8);*/
                gCommPowerToSectionParam.power_section_interact_word = CombineBytes(Data[2],Data[3]);
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
    static uint16_t tmp_pwr_to_pwrless_flag = 0;
    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    gCommSyncToSectionStatus.sync_status.all = CombineBytes(Data[0], Data[1]);
    switch(gCommSyncToSectionStatus.sync_status.bit.mainten_has_power)
    {
        case 0:
            {
                // 检修无电了
                if (tmp_pwr_to_pwrless_flag == 1)
                {
                    tmp_pwr_to_pwrless_flag = 0;
                    gSyncToSection_PwrToPwrless = 1;
                    gPwrToPwrlessCount = hal_timer_get_timestamp();
                }
            }
            break;
        case 1:
            {
                // 检修有电
                tmp_pwr_to_pwrless_flag = 1;
            }
            break;
        default:break;
    }
}

// 主循环回调
static void
CAN_Adapter_MainLoop_Callback(TeCanPort Port)
{
    uint32_t now = hal_timer_get_timestamp();
    if (Port == eCanPort_1)
    {
        if ( now - gAdapterStatus.power_online_start > 5000)
        {
            gAdapterStatus.power_online_status = false;
        } 
    }
    else if (Port == eCanPort_0)
    {
        if ( now - gAdapterStatus.sync_online_start > 5000)
        {
            gAdapterStatus.sync_online_status = false;
        } 
    }
    // 单转并计时清空
    if (gPowerStatus.single_to_parallel_flag == true)
    {
        if (now - gSingleToParaCount > 120000)
        {
            gPowerStatus.single_to_parallel_flag = false;
        }
    }
    // 检修有电转无电 计时清空
    if (gSyncToSection_PwrToPwrless == 1)
    {
        if (now - gPwrToPwrlessCount > 120000)
        {
            gSyncToSection_PwrToPwrless = 0;
        }
    }
}

// 接收回调
static void
CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc)
{
    if (Port == eCanPort_1)
    {
        gAdapterStatus.power_online_status = true;
        gAdapterStatus.power_online_start = hal_timer_get_timestamp();
        CAN_Adapter_ParsePowerParam(Data, Dlc);
    }
    else if (Port == eCanPort_0)
    {
        // tmp variable
        CommSyncToSection_t state;
        state.sync_status.all = CombineBytes(Data[0], Data[1]);
        if (state.sync_status.bit.sync_is_master == 1)
        {
            gAdapterStatus.sync_online_status = true;
            gAdapterStatus.sync_online_start = hal_timer_get_timestamp();
            CAN_Adapter_ParseSyncCabinetStatus(Data, Dlc);
        }
    }
}

// 状态发送回调
void
CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr)
{
    static uint16_t heartbeat0 = 0;
    static uint16_t heartbeat1 = 0;
    uint16_t data[8]={0};
    // 电源
    if (Port == eCanPort_1)
    {
        uint16_t value;
        gCommSectionToPowerStatus.section_status.bit.Toggle = heartbeat0;
		value = gCommSectionToPowerStatus.section_status.all;
		heartbeat0 = !heartbeat0;

        data[2] = value & 0xFF;
        data[3] = (value >> 8) & 0xFF;
        CAN_Agent_SendStatus(Port, DEV_ADDRESS_BROADCAST, data);
    }
    // 同步柜
    else if (Port == eCanPort_0)
    {
        uint16_t value;
        gCommSectionToSyncStatus.section_status.bit.heartbeat = heartbeat1;
        gCommSectionToSyncStatus.section_status.bit.sync_cabinet_online = gAdapterStatus.sync_online_status;
        heartbeat1 = !heartbeat1;
        value = gCommSectionToSyncStatus.section_status.all;

        data[0] = value & 0xFF;
        data[1] = (value >> 8) & 0xFF;
        CAN_Agent_SendStatus(Port, DEV_ADDRESS_BROADCAST, data);
    }
}

