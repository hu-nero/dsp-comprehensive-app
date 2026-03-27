/*
 * can_agent.h
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */
#ifndef SERVICES_CAN_AGENT_CAN_AGENT_H_
#define SERVICES_CAN_AGENT_CAN_AGENT_H_

#include "stdint.h"
#include "stdbool.h"
#include "SDK/Hal/Can/hal_can.h"

// 本设备地址定义 (4bit)
#define DEV_ADDRESS_SELF      0x02    // 本设备地址
#define DEV_ADDRESS_BROADCAST 0x0F    // 广播地址
// 超时时间（单位 ms）
#define CMD_TIMEOUT_MS    	  500  // 等待响应超时时间
#define MAX_PENDING_CMDS      5
#define MAX_TIMEOUT_TIMES     2    // 超时重发次数
// 功能码定义（3bit）
typedef enum
{
    FUNC_FAULT           = 0x0,  // 发送故障指令
    FUNC_CONTROL         = 0x1,  // 发送控制指令
    FUNC_RESPONSE        = 0x2,  // 发送控制应答
    FUNC_STATUS          = 0x3,  // 状态广播
    FUNC_PARAM_CMD       = 0x4,  // 发送参数指令
    FUNC_PARAM_RESPONSE  = 0x5,  // 发送参数应答
    FUNC_PARAM_DATA      = 0x6,  // 发送参数数据
    FUNC_HEARTBEAT       = 0x7   // 发送心跳帧
} FuncCode_t;

// 通信模式
typedef enum
{
    COM_MODE_UNICAST,   // 单播
    COM_MODE_BROADCAST  // 广播
} ComMode_t;

// 故障等级
typedef enum
{
    FAULT_LEVEL_WARNING = 0x01,   // 告警
    FAULT_LEVEL_NORMAL  = 0x02,   // 一般故障
    FAULT_LEVEL_SERIOUS = 0x03    // 严重故障
} FaultLevel_t;

// 控制指令
typedef enum
{
    CMD_START  = 0x0001,  // 启动
    CMD_STOP   = 0x0002,  // 停止
    CMD_RESET  = 0x0003,  // 复位
    CMD_ACK    = 0x0004   // 确认
} CtrlCmd_t;

// 响应状态
typedef enum
{
    RESP_SUCCESS = 0x0001,  // 成功
    RESP_FAIL    = 0x0002   // 失败
} ResponseStatus_t;

// 交互设备
typedef enum
{
	PARAM_DEV_NONE       = 0x0000,
    PARAM_DEV_RECTIFIER  = 0x0001,   // 整流器
    PARAM_DEV_INVERTER   = 0x0002,   // 逆变器
    PARAM_DEV_SECTION    = 0x0003    // 区段柜
} ParamDevice_t;

// 交互指令码
typedef enum
{
    PARAM_CMD_START  = 0x0001,   // 开始
    PARAM_CMD_END    = 0x0002,   // 结束
} ParamCmd_t;

// 本设备当前状态
typedef struct
{
    uint16_t run_status;     // 运行状态
    uint16_t fault_flag;     // 故障标志位
    uint16_t local_remote;   // 本地远程
    uint16_t work_mode;      // 工作模式
    uint16_t power_status;   // 上电状态
} DeviceStatus_t;

// 等待状态结构
typedef struct
{
    bool is_waiting;
    uint32_t tick;
    FuncCode_t func_code;
    uint16_t dst_addr;
    uint16_t resp_times;
    uint16_t data[CAN_DRIVER_MAX_DLC];
} PendingCmd_t ;

// 代理初始化
extern void CAN_Agent_Init(TeCanPort Port);

// 主循环处理
extern void CAN_Agent_Process(TeCanPort Port);

extern bool CAN_Agent_SendFault(TeCanPort Port, uint16_t DstAddr, FaultLevel_t Level, uint16_t FaultCode);
extern bool CAN_Agent_SendControl(TeCanPort Port, uint16_t DstAddr, CtrlCmd_t Cmd, uint16_t Param);
extern bool CAN_Agent_SendCtrlResp(TeCanPort Port, uint16_t DstAddr, ResponseStatus_t Status);
extern bool CAN_Agent_SendStatus(TeCanPort Port, uint16_t DstAddr, uint16_t *Data);

// TODO: 其他通信方式待实现

// 发送心跳帧
extern bool CAN_Agent_SendHeartbeat(TeCanPort Port);

// 回调函数定义
typedef void (*CAN_RxHandler_t)(TeCanPort port, FuncCode_t func, uint16_t src_addr, uint16_t *data, uint16_t dlc);
typedef void (*CAN_TxHandler_t)(TeCanPort port, FuncCode_t func, uint16_t src_addr);
extern void CAN_SetRxHandler(TeCanPort Port, CAN_RxHandler_t Handler);
extern void CAN_Agent_Set_SendStatus_Callback(TeCanPort Port, CAN_TxHandler_t Handler);


#endif /* SERVICES_CAN_AGENT_CAN_AGENT_H_ */
