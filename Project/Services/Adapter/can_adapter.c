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
static void CAN_Adapter_ParsePowerStatus(uint16_t *Data, uint16_t Dlc);

PowerStatus_t
CAN_Adapter_GetPowerStatus(void)
{
    return gPowerStatus;
}
// 适配器初始化
void
CAN_Adapter_Init(void)
{
    CAN_App_Set_func_status_handler(CAN_Adapter_ParsePowerStatus);
}

// 发送启动电源命令
bool
CAN_Adapter_SendPowerStart(void)
{
    return CAN_Agent_SendControl(POWER_DEVICE_ADDR, CMD_START, 0x01);
}

// 发送关闭电源命令
bool
CAN_Adapter_SendPowerClose(void)
{
    return CAN_Agent_SendControl(POWER_DEVICE_ADDR, CMD_STOP, 0x01);
}
// 发送状态查询命令
bool
CAN_Adapter_SendStatusQuery(void)
{
    // 通过发送控制命令查询状态
    return CAN_Agent_SendControl(POWER_DEVICE_ADDR, CMD_ACK, 0x00);
}

// 发送控制响应
bool
CAN_Adapter_SendControlResponse(uint16_t DstAddr, ResponseStatus_t Status)
{
    return CAN_Agent_SendCtrlResp(DstAddr, Status);
}


// 解析电源状态
static void
CAN_Adapter_ParsePowerStatus(uint16_t *Data, uint16_t Dlc)
{
	union WordPowerStatus_t status_word;

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
                gPowerStatus.run_state = POWER_STANDBY;
            }
            break;
        case 1:
            {
                gPowerStatus.run_state = POWER_RUNNING;
            }
            break;
        case 2:
            {
                gPowerStatus.run_state = POWER_FAULT;
            }
            break;
        default:break;

    }
    switch (status_word.bit.local_remote)
    {
        case 0:
            {
                gPowerStatus.ctrl_mode = CONTROL_LOCAL;
            }
            break;
        case 1:
            {
                gPowerStatus.ctrl_mode = CONTROL_REMOTE;
            }
            break;
        default:break;

    }
    switch (status_word.bit.single_parallel)
    {
        case 0:
            {
                gPowerStatus.comb_state = POWER_SINGLE;
            }
            break;
        case 1:
            {
                gPowerStatus.comb_state = POWER_PARALLEL;
            }
            break;
        default:break;

    }
}
