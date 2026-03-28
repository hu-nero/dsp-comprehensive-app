/*
 * section_common.h
 *
 *  Created on: 2026年1月7日
 *      Author: huxl
 */

#ifndef SERVICES_LOGIC_SECTION_COMMON_H_
#define SERVICES_LOGIC_SECTION_COMMON_H_

#include <stdint.h>
#include <stdbool.h>

// 电源运行状态
typedef enum
{
    POWER_STANDBY = 0,           // 待机
	POWER_SINGLE_RUNNING,        // 单机运行
    POWER_PARALLEL_RUNNING,      // 并机运行
    POWER_FAULT,                 // 故障
    POWER_UNKNOWN                // 未知
} PowerRunState_t;

// 电源状态结构体
typedef struct
{
    PowerRunState_t run_state;     // 运行状态
    bool single_to_parallel_flag;
    uint32_t status_word;          // 状态字原始值
} PowerStatus_t;

// 区段柜面板模式
//typedef enum
//{
    //CABINET_MODE_MANUAL = 0,   // 手动模式
    //CABINET_MODE_AUTO          // 自动模式
//} CabinetMode_t;

// 开关状态
typedef enum
{
    SWITCH_OFF = 0,            // 断开
    SWITCH_ON                  // 接通
} SwitchState_t;

// 区段柜工作状态
typedef enum
{
    CABINET_STATE_STANDBY = 0, // 待机（单机断开、母线1断开）
    CABINET_STATE_SINGLE,      // 单机（单机接通、母线1断开）
    CABINET_STATE_PARALLEL,    // 并机（单机断开、母线1接通）
    CABINET_STATE_BACKUP,      // 备用（单机接通、母线1接通）
    CABINET_STATE_INVALID      // 无效状态
} CabinetState_t;

// 执行结果
typedef enum
{
    RESULT_SUCCESS = 0,        // 成功
    RESULT_WAIT,               // 等待状态
    RESULT_FAILED,             // 失败
    RESULT_INVALID_STATE,      // 无效状态
    RESULT_COMM_ERROR,         // 通信错误
    RESULT_TIMEOUT,            // 超时
    RESULT_EMERGENCY_STOP      // 急停激活
} ExecuteResult_t;

// 报警类型
typedef enum
{
    ALARM_NONE = 0,
    ALARM_STATE_ROLLBACK,      // 状态回退
    ALARM_POWER_START_FAILED,  // 电源启动失败
    ALARM_CAN_COMM_ERROR,      // CAN通信错误
    ALARM_NOT_MANUAL_MODE,     // 非手动模式
    ALARM_FAULT,               // 故障
    ALARM_MAX
} AlarmType_t;

// 反馈输入
typedef enum
{
    DI_ID_LOCALREMOTE = 0,     // 本地远端
    DI_ID_EMERGENCY_STOP,      // 急停
    DI_ID_RESET,               // 复位(点动)
    DI_ID_A3QF1,               // 单机QF1
    DI_ID_A3QF2,               // 并机QF2
    DI_ID_A3QS1,               // 单机/并机母线刀闸
    DI_ID_A3K17,               // 检修母线刀闸
    DI_ID_A3QS2,               // 补偿电容刀闸
    DI_ID_A3QS3,               // 消磁刀闸
    DI_ID_A3TB3_1,             // UPS供电
    DI_ID_A3TB3_2,             // 市电反馈
    DI_ID_A3QF3,               // 消磁晶闸管反馈
    DI_ID_SINGLE,              // 单机按钮(点动)
    DI_ID_BUS1,                // 并机按钮(点动)
    DI_ID_Short,               // 短接按钮(点动)
    DI_ID_IMPACT_VEHICLE,      // 冲击车
    DI_ID_COMPENSATION_VEHICLE,// 补偿车
    DI_ID_A3QR1,               // 单机QR1
    DI_ID_A3QR2                // 并机QR2
} DigitalInputType_t;

// 按钮输出
typedef enum
{
    DO_ID_REMOTE_SINGLE_CTRL_FEED = 0,   // 本地远端
    DO_ID_RESERVE1,
    DO_ID_A3QF1,       // 单机QF1
    DO_ID_REMOTE_PARALEL_CTRL_FEED,      // 消磁按键K2
    DO_ID_A3QF2,       // 并机QF2
    DO_ID_RESERVE2,
    DO_ID_A3QR3,       // 消磁回路QR3
    DO_ID_A3QR4,       // 补偿电容
    DO_ID_A3QR5,       // 补偿电容
    DO_ID_A3QR6,       // 补偿电容
    DO_ID_A3QR7,       // 补偿电容
    DO_ID_RESERVE3,
    DO_ID_RUNLED,      // 运行灯
    DO_ID_FLTLED,      // 故障灯
    DO_ID_A3QR1,       // 消磁回路QR3
    DO_ID_A3QR2,       // 消磁回路QR3
    DO_ID_RESET        // 复位后取消点动记录状态
} DigitalOutputType_t;

// 三相电压采集点类型
typedef enum
{
    VOLTAGE_U5_PHASE_A = 0,   // U5_A相电压
    VOLTAGE_U5_PHASE_B,       // U5_B相电压
    VOLTAGE_U5_PHASE_C,       // U5_C相电压
    VOLTAGE_U6_PHASE_A,       // U6_A相电压
    VOLTAGE_U6_PHASE_B,       // U6_B相电压
    VOLTAGE_U6_PHASE_C,       // U6_C相电压
    VOLTAGE_U7_PHASE_A,       // U7_A相电压
    VOLTAGE_U7_PHASE_B,       // U7_B相电压
    VOLTAGE_U7_PHASE_C        // U7_C相电压
} VoltageReadType_t;

// 三相电流采集点类型
typedef enum
{
    CURRENT_I5_PHASE_A = 0,   // I5_A相电流
    CURRENT_I5_PHASE_B,       // I5_B相电流
    CURRENT_I5_PHASE_C,       // I5_C相电流
    CURRENT_I6_PHASE_A,       // I6_A相电流
    CURRENT_I6_PHASE_B,       // I6_B相电流
    CURRENT_I6_PHASE_C,       // I6_C相电流
} CurrentReadType_t;

#endif /* SERVICES_LOGIC_SECTION_COMMON_H_ */
