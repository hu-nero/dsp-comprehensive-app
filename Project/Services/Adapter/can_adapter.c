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
static void CAN_Adapter_ParsePowerStatus(uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_ParseGridConnectionStatus(uint16_t *Data, uint16_t Dlc);
static void CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc);

PowerStatus_t
CAN_Adapter_GetPowerStatus(void)
{
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

    // 回调
    CAN_App_Set_func_status_handler(CAN_Adapter_RxHandler);
}

// 发送并网请求指令
bool
CAN_Adapter_SendParallelRequest(void)
{
    // [0x01,0x01]
    return CAN_Agent_SendControl(eCanPort_1, SYNC_CABINET_DEVICE_ADDR, CMD_START, 0x01);
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

// 解析电源状态（CANA）
static void
CAN_Adapter_ParsePowerStatus(uint16_t *Data, uint16_t Dlc)
{
	union WordPowerStatus_t status_word;
    static bool bool_single_flag = false;

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    status_word.all = (Data[0]&0xFF)|(Data[1]<<8);

    // 解析状态字
    switch (status_word.bit.run_state)
    {
        case 0:
            {
                bool_single_flag = false;
                gPowerStatus.run_state = POWER_STANDBY;
            }
            break;
        case 1:
            {
                bool_single_flag = true;
                gPowerStatus.run_state = POWER_SINGLE_RUNNING;
            }
            break;
        case 2:
            {
                if (bool_single_flag)
                {
                    gPowerStatus.single_to_parallel_flag = true;
                }
                bool_single_flag = false;
                gPowerStatus.run_state = POWER_PARALLEL_RUNNING;
            }
            break;
        case 3:
            {
                bool_single_flag = false;
                gPowerStatus.run_state = POWER_FAULT;
            }
            break;
        default:break;

    }
}

// 解析并网/解列状态（CANB）
static void
CAN_Adapter_ParseGridConnectionStatus(uint16_t *Data, uint16_t Dlc)
{
    union WordPowerStatus_t status_word;

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    status_word.all = (Data[0]&0xFF)|(Data[1]<<8);

    if (status_word.bit.run_state & 0x01)
    {
        gGridConnectionStatus.state = GRID_CONNECTED;
    }
    else
    {
        gGridConnectionStatus.state = GRID_DISCONNECTED;
    }

    gGridConnectionStatus.status_word = status_word.all;
}

// 回调
static void
CAN_Adapter_RxHandler(TeCanPort Port, uint16_t *Data, uint16_t Dlc)
{
    if (Port == eCanPort_0)
    {
        CAN_Adapter_ParsePowerStatus(Data, Dlc);
    }
    else if (Port == eCanPort_1)
    {
        CAN_Adapter_ParseGridConnectionStatus(Data, Dlc);
    }
}

