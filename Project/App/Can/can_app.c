/*
 * can_app.c
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */

/* can_application.c - 应用演示层 */
#include "can_app.h"
#include <stdio.h>
#include <string.h>
#include "Services/Adapter/can_adapter.h"
#include "Services/Logic/section_cabinet.h"
#include "Main/Variable.h"

#define IO_A3QF1_En					    IOOutput1.DataBit.Bit0 = 1
#define IO_A3QF1_Dis	  				IOOutput1.DataBit.Bit0 = 0
#define IO_A3QF1_Feed   				IOInput1.DataBit.Bit0

#define IO_A3QR1_En					    IOOutput1.DataBit.Bit1 = 1
#define IO_A3QR1_Dis	  				IOOutput1.DataBit.Bit1 = 0
#define IO_A3QR1_Feed					IOInput1.DataBit.Bit1

#define IO_A3QF2_En					    IOOutput1.DataBit.Bit2 = 1
#define IO_A3QF2_Dis	  				IOOutput1.DataBit.Bit2 = 0
#define IO_A3QF2_Feed   				IOInput1.DataBit.Bit2

#define IO_A3QR2_En					    IOOutput1.DataBit.Bit3 = 1
#define IO_A3QR2_Dis	  				IOOutput1.DataBit.Bit3 = 0
#define IO_A3QR2_Feed					IOInput1.DataBit.Bit3

#define IO_SINGLE_En					IOOutput1.DataBit.Bit4 = 1
#define IO_SINGLE_Dis	  				IOOutput1.DataBit.Bit4 = 0
#define IO_SINGLE_Feed					IOInput1.DataBit.Bit15

#define IO_BUS1_En					    IOOutput1.DataBit.Bit5 = 1
#define IO_BUS1_Dis	  				    IOOutput1.DataBit.Bit5 = 0
#define IO_BUS1_Feed					IOInput1.DataBit.Bit14

#define IO_SHORT_En					    IOOutput1.DataBit.Bit6 = 1
#define IO_SHORT_Dis	  				IOOutput1.DataBit.Bit6 = 0
#define IO_SHORT_Feed					IOInput1.DataBit.Bit15

#define ADC0_CH0_Value					*(unsigned  int *)(0x4010)
#define ADC0_CH1_Value					*(unsigned  int *)(0x4020)
#define ADC0_CH2_Value					*(unsigned  int *)(0x4030)

#define ADC0_CH3_Value					*(unsigned  int *)(0x4040)
#define ADC0_CH4_Value					*(unsigned  int *)(0x4050)
#define ADC0_CH5_Value					*(unsigned  int *)(0x4060)

static CAN_App_func_status_handler_t g_func_status_handler = NULL;
// 全局柜体控制上下文
static CabinetContext_t g_cabinet_context;
// 硬件开关控制接口
static ExecuteResult_t Hardware_SwitchControl(uint16_t SwitchId, SwitchState_t State);
static uint16_t Hardware_SwitchFeed(uint16_t SwitchId);
static uint16_t Hardware_VoltageRead(VoltageReadType_t VoltagePhaseType);
static uint16_t Hardware_CurrentRead(CurrentReadType_t CurrentPhaseType);
static void Hardware_AlarmCallback(AlarmType_t AlarmType, const char* Message);

// 应用层CAN接收处理函数
static void
CAN_App_CanRxHandler(FuncCode_t Func, uint16_t SrcAddr, uint16_t *Data, uint16_t Dlc)
{
    // 根据不同功能码处理
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
                // TODO:控制处理
                CAN_Agent_SendCtrlResp(SrcAddr, RESP_SUCCESS);

            }
            break;

        case FUNC_RESPONSE://0x02
            {
                uint16_t u16Data[8] = {};
                memcpy(u16Data, Data, Dlc);
                // TODO:此处需要根据实际应用识别响应数据，失败则如何成功则如何
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


                // 调用已注册的回调函数
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
                // TODO:此处需要根据实际应用解析数据，校验数据完整性
            }
            break;
        case FUNC_PARAM_DATA://0x06
            {
                // TODO:数据存储处理
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

// 应用层初始化
void
CAN_App_Init(void)
{
    // 初始化CAN协议栈
    CAN_Agent_Init(eCanPort_0);
    //CAN_Agent_Init(eCanPort_1);
    // 注册接收回调到CAN驱动
    CAN_SetRxHandler(CAN_App_CanRxHandler);

    // 初始化柜体控制
    if (!Cabinet_Init(&g_cabinet_context,
                      Hardware_SwitchControl,
                      Hardware_SwitchFeed,
                      Hardware_VoltageRead,
                      Hardware_CurrentRead,
                      Hardware_AlarmCallback))
    {
        // init failed
        return;
    }

}

// 应用层主循环
void
CAN_App_MainLoop(void)
{
    // 处理CAN协议栈
    CAN_Agent_Process();

    // 处理柜体状态机
    Cabinet_Process(&g_cabinet_context);

    // switch test
    CAN_App_Test();
}

void
CAN_App_Test(void)
{
    // switch enable test
    Hardware_SwitchControl(SWITCH_ID_A3QF1,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_A3QR1,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_A3QF2,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_A3QR2,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_SINGLE,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_BUS1,SWITCH_ON);
    Hardware_SwitchControl(SWITCH_ID_SHORT,SWITCH_ON);
    uint16_t res = Hardware_SwitchFeed(SWITCH_ID_A3QF1);
    res = res;
}

// 发送控制命令
void
CAN_App_SendControlToDevice(uint16_t DstAddr)
{
    if (CAN_Agent_SendControl(DstAddr, CMD_START, 0))
    {
        //控制命令发送成功
    }
    else
    {
        //控制命令发送失败
    }
}

// 发送故障报告
void
CAN_App_SendFaultReport(void)
{
    if (CAN_Agent_SendFault(DEV_ADDRESS_BROADCAST, FAULT_LEVEL_WARNING, 0x0001))
    {
        //故障报告发送成功
    }
    else
    {
        //故障报告发送失败
    }
}

bool
CAN_App_SendParam(uint16_t DstAddr, ParamDevice_t Device, uint16_t *Data, uint16_t TotalLength)
{
	return CAN_Agent_StartSendParamTranmit(DstAddr, Device, Data, TotalLength);
}


/**
 * @brief :硬件开关控制接口
 *
 * @param switch_id:开关id
 * @param state:开启/关闭
 *
 * @return :开关状态反馈;0/1
 */
static ExecuteResult_t
Hardware_SwitchControl(uint16_t SwitchId, SwitchState_t State)
{
    switch (SwitchId)
    {
        case DO_ID_LOCALREMOTE:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_DEGAUSS:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QF1:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR1:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QF2:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR2:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR3:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR4:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR5:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR6:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        case DO_ID_A3QR7:
            {
                if (State == SWITCH_ON)
                {
                }
                else if (State == SWITCH_OFF)
                {
                }
            }
            break;
        default:break;
    }
    return RESULT_SUCCESS;
}

static uint16_t
Hardware_SwitchFeed(uint16_t SwitchId)
{
    switch (SwitchId)
    {
        case DI_ID_LOCALREMOTE:
            {
            }
            break;
        case DI_ID_EMERGENCY_STOP:
            {
            }
            break;
        case DI_ID_RESET:
            {
            }
            break;
        case DI_ID_A3QF1:
            {
            }
            break;
        case DI_ID_A3QF2:
            {
            }
            break;
        case DI_ID_A3QS1:
            {
            }
            break;
        case DI_ID_A3K17:
            {
            }
            break;
        case DI_ID_A3QS2:
            {
            }
            break;
        case DI_ID_A3QS3:
            {
            }
            break;
        case DI_ID_A3TB3_1:
            {
            }
            break;
        case DI_ID_A3TB3_2:
            {
            }
            break;
        case DI_ID_A3ON_2:
            {
            }
            break;
        case DI_ID_A3OFF_2:
            {
            }
            break;
        case DI_ID_A3XCON_1:
            {
            }
            break;
        case DI_ID_A3XCOFF_2:
            {
            }
            break;
        default:break;
    }
    return 0;
}

static uint16_t
Hardware_VoltageRead(VoltageReadType_t VoltagePhaseType)
{
    switch (VoltagePhaseType)
    {
        case VOLTAGE_U5_PHASE_A:
            {
                return ADC0_CH0_Value;
            }
        case VOLTAGE_U5_PHASE_B:
            {
                return ADC0_CH1_Value;
            }
        case VOLTAGE_U5_PHASE_C:
            {
                return ADC0_CH2_Value;
            }
        case VOLTAGE_U6_PHASE_A:
            {
            }
            break;
        case VOLTAGE_U6_PHASE_B:
            {
            }
            break;
        case VOLTAGE_U6_PHASE_C:
            {
            }
            break;
        case VOLTAGE_U7_PHASE_A:
            {
            }
            break;
        case VOLTAGE_U7_PHASE_B:
            {
            }
            break;
        case VOLTAGE_U7_PHASE_C:
            {
            }
            break;
        default:break; 
    }
    return 0;
}

static uint16_t
Hardware_CurrentRead(CurrentReadType_t CurrentPhaseType)
{
    switch (CurrentPhaseType)
    {
        case CURRENT_I5_PHASE_A:
            {
                return ADC0_CH3_Value;
            }
        case CURRENT_I5_PHASE_B:
            {
                return ADC0_CH4_Value;
            }
        case CURRENT_I5_PHASE_C:
            {
                return ADC0_CH5_Value;
            }
        case CURRENT_I6_PHASE_A:
            {
            }
            break;
        case CURRENT_I6_PHASE_B:
            {
            }
            break;
        case CURRENT_I6_PHASE_C:
            {
            }
            break;
        default:break; 
    }
    return 0;
}

/* 硬件报警回调函数 */
static void
Hardware_AlarmCallback(AlarmType_t AlarmType, const char* Message)
{
    /* 实际应用中报警 */
//           alarm_type == ALARM_STATE_ROLLBACK ? "State Rollback" :
//           alarm_type == ALARM_POWER_START_FAILED ? "Power Start Failed" :
//           alarm_type == ALARM_CAN_COMM_ERROR ? "CAN Comm Error" :
//           alarm_type == ALARM_NOT_MANUAL_MODE ? "Not Manual Mode" : "Unknown",
//           message);
}
