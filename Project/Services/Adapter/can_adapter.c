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


void CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr);

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

    // 初始化同步柜向区段柜通信状态
    gCommSyncToSectionStatus.section_online_nofault_status.all = 0;
    gCommSectionToSyncStatus.section_status.all = 0;

    // 回调
    CAN_App_Set_func_status_handler(CAN_Adapter_RxHandler);
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
    uint16_t section,wordTmp[3];

    if ((!Data) || (Dlc < 1))
    {
        return;
    }

    section = (Data[0]&0xFF)|(Data[1]<<8);

    // 解析状态字
    switch (section)
    {
        case 0:
            {
            }
            break;
        case 1:
            {
                wordTmp[0] = (Data[2]&0xFF)|(Data[3]<<8);
                wordTmp[1] = (Data[4]&0xFF)|(Data[5]<<8);
gCommSyncToSectionStatus.section_online_nofault_status.all = wordTmp[1];
            }
            break;
        default:break;
    }

    /*status_word.all = (Data[0]&0xFF)|(Data[1]<<8);*/

    /*if (status_word.bit.run_state & 0x01)*/
    /*{*/
        /*gGridConnectionStatus.state = GRID_CONNECTED;*/
    /*}*/
    /*else*/
    /*{*/
        /*gGridConnectionStatus.state = GRID_DISCONNECTED;*/
    /*}*/

    /*gGridConnectionStatus.status_word = status_word.all;*/
}

// 接收回调
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

// 状态发送回调
void
CAN_Adapter_SendStatus_Callback(TeCanPort Port, FuncCode_t Func, uint16_t SrcAddr)
{
    uint16_t data[8]={0};
    if (Port == eCanPort_0)
    {
    }
    else if (Port == eCanPort_1)
    {
        uint16_t value = gCommSectionToSyncStatus.section_status.all;
        data[0] = value & 0xFF;
        data[1] = (value >> 8) & 0xFF;
        CAN_Agent_SendStatus(Port, DEV_ADDRESS_BROADCAST, data);
    }
}

