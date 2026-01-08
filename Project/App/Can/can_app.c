/*
 * can_app.c
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */

/* can_application.c - 应用层示例 */
#include "can_app.h"
#include <stdio.h>
#include <string.h>
#include "Services/Adapter/can_adapter.h"
#include "Services/Logic/section_cabinet.h"

static CAN_App_func_status_handler_t g_func_status_handler = NULL;
/* 全局区段柜上下文（静态分配） */
static CabinetContext_t g_cabinet_context;
/* 硬件开关控制函数 */
static ExecuteResult_t Hardware_SwitchControl(uint16_t switch_id, SwitchState_t state);
static ExecuteResult_t Hardware_Demagnetize(void);
static void Hardware_AlarmCallback(AlarmType_t alarm_type, const char* message);

/* 应用层接收处理函数 */
static void
CAN_App_CanRxHandler(FuncCode_t Func, uint16_t SrcAddr, uint16_t *Data, uint16_t Dlc)
{

    // 处理特定功能码
    switch (Func)
    {
        case FUNC_FAULT://0x00
            {
                uint16_t u16Data[8] = {};
                memcpy(u16Data, Data, Dlc);
                //CAN_Agent_SendFault(SrcAddr, FAULT_LEVEL_WARNING, 0x01);

                // test
                CAN_Agent_SendControl(SrcAddr, CMD_START, 0x01);
            }
            break;

        case FUNC_CONTROL://0x01
            {
                uint16_t u16Data[8] = {};
                memcpy(u16Data, Data, Dlc);
                //CAN_Agent_SendControl(SrcAddr, CMD_START, 0x01);
                // TODO:进行操作
                CAN_Agent_SendCtrlResp(SrcAddr, RESP_SUCCESS);

            }
            break;

        case FUNC_RESPONSE://0x02
            {
                uint16_t u16Data[8] = {};
                memcpy(u16Data, Data, Dlc);
                // TODO:协议层清除了超时计数，应用层该识别应答内容，失败怎么样；成功怎么样
                //CAN_Agent_SendCtrlResp(SrcAddr, RESP_SUCCESS);

            }
            break;

        case FUNC_STATUS://0x03
            {
                uint16_t u16Data[8] = {};
                DeviceStatus_t status;
                memcpy(u16Data, Data, Dlc);

                status.run_status = 1;
                status.fault_flag = 0;
                status.local_remote = 1;
                status.work_mode = 1;
                status.power_status = 1;
                CAN_Agent_SendStatus(status);


                // 如果有注册回调函数则调用
                if (g_func_status_handler)
                {
                    g_func_status_handler(Data,Dlc);
                }
            }
            break;
        case FUNC_PARAM_CMD://0x04
            {

                CAN_Agent_SendParamResp(SrcAddr, RESP_SUCCESS);
            }
            break;
        case FUNC_PARAM_RESPONSE://0x05
            {
                uint16_t u16Data[8] = {};
                memcpy(u16Data, Data, Dlc);
                // TODO:协议层清除了超时计数，应用层该发参，或者结束发送了
            }
            break;
        case FUNC_PARAM_DATA://0x06
            {
                // TODO:处理存储接收数据
            }
            break;
        case FUNC_HEARTBEAT://0x07
            {
                uint16_t u16Data[8] = {};
                uint16_t u16DataTmp[31] = {0x11,0x11,0x11,0x11,0x11,0x11,
                		0x22,0x22,0x22,0x22,0x22,0x22,
                		0x33,0x33,0x33,0x33,0x33,0x33,
						0x44,0x44,0x44,0x44,0x44,0x44,
						0x55,0x55,0x55,0x55,0x55,0x55,
						0x66};
                memcpy(u16Data, Data, Dlc);
                //CAN_Agent_SendHeartbeat();

                // test1
                //CAN_Agent_SendParamCmd(SrcAddr, PARAM_DEV_RECTIFIER, PARAM_CMD_START, 2, 8);

                // test2
                CAN_App_SendParam(SrcAddr, PARAM_DEV_RECTIFIER, u16DataTmp, 31);
            }
            break;

        default:
            break;
    }
}

// 设置消息接收回调函数
void
CAN_App_Set_func_status_handler(CAN_App_func_status_handler_t Handler)
{
	g_func_status_handler = Handler;
}

/* 应用层初始化 */
void
CAN_App_Init(void)
{
    /* 初始化CAN协议栈 */
    CAN_Agent_Init();
    /* 设置接收回调到CAN代理 */
    CAN_SetRxHandler(CAN_App_CanRxHandler);

    /* 初始化区段柜控制 */
    if (!Cabinet_Init(&g_cabinet_context,
                      Hardware_SwitchControl,
                      Hardware_Demagnetize,
                      Hardware_AlarmCallback))
    {
        //printf("Failed to initialize cabinet control!\n");
        return;
    }


}


/* 示例：发送控制命令 */
void
CAN_App_SendControlToDevice(uint16_t DstAddr)
{
    if (CAN_Agent_SendControl(DstAddr, CMD_START, 0))
    {
        //printf("控制命令发送成功\n");
    }
    else
    {
        //printf("控制命令发送失败\n");
    }
}

/* 示例：发送故障报告 */
void
CAN_App_SendFaultReport(void)
{
    if (CAN_Agent_SendFault(DEV_ADDRESS_BROADCAST, FAULT_LEVEL_WARNING, 0x0001))
    {
        //printf("故障报告发送成功\n");
    }
    else
    {
        //printf("故障报告发送失败\n");
    }
}

bool
CAN_App_SendParam(uint16_t DstAddr, ParamDevice_t Device, uint16_t *Data, uint16_t TotalLength)
{
	return CAN_Agent_StartSendParamTranmit(DstAddr, Device, Data, TotalLength);
}


/* 硬件开关控制函数 */
static ExecuteResult_t
Hardware_SwitchControl(uint16_t switch_id, SwitchState_t state)
{
    /* 实际硬件控制代码 */
    //printf("Switch %d set to %s\n", switch_id, state == SWITCH_ON ? "ON" : "OFF");
    return RESULT_SUCCESS;
}

/* 硬件消磁函数 */
static ExecuteResult_t
Hardware_Demagnetize(void)
{
    /* 实际消磁操作 */
    //printf("Performing demagnetization...\n");
    return RESULT_SUCCESS;
}

/* 硬件报警回调函数 */
static void
Hardware_AlarmCallback(AlarmType_t alarm_type, const char* message)
{
    /* 实际报警处理 */
//    printf("Alarm: %s - %s\n",
//           alarm_type == ALARM_STATE_ROLLBACK ? "State Rollback" :
//           alarm_type == ALARM_POWER_START_FAILED ? "Power Start Failed" :
//           alarm_type == ALARM_CAN_COMM_ERROR ? "CAN Comm Error" :
//           alarm_type == ALARM_NOT_MANUAL_MODE ? "Not Manual Mode" : "Unknown",
//           message);
}



