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

// 电源并机状态
typedef enum
{
    POWER_SINGLE = 0,   // 单机
    POWER_PARALLEL,     // 并机
    POWER_COMB_UNKNOWN  // 未知
} PowerCombState_t;

// 电源控制模式
typedef enum
{
    CONTROL_LOCAL = 0,  // 就地
    CONTROL_REMOTE,     // 远控
    CONTROL_MODE_UNKNOWN
} ControlMode_t;

// 电源状态结构体
typedef struct
{
    PowerRunState_t run_state;     // 运行状态
    PowerCombState_t comb_state;   // 并机状态
    ControlMode_t ctrl_mode;       // 控制模式
    uint32_t status_word;          // 状态字原始值
    uint16_t is_using;             // 是否正在使用
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
    ALARM_MAX
} AlarmType_t;

// 反馈输入
typedef enum
{
    DI_ID_LOCALREMOTE = 0,     // 本地远端
    DI_ID_EMERGENCY_STOP,      // 急停
    DI_ID_RESET,               // 复位
    DI_ID_A3QF1,               // 单机QF1
    DI_ID_A3QF2,               // 并机QF2
    DI_ID_A3QS1,               // 单机/并机母线刀闸
    DI_ID_A3K17,               // 检修母线刀闸
    DI_ID_A3QS2,               // 补偿电容刀闸
    DI_ID_A3QS3,               // 消磁刀闸
    DI_ID_A3TB3_1,             // UPS供电
    DI_ID_A3TB3_2,             // 消磁接通反馈
    DI_ID_A3ON_2,              // 远控启动(预留)
    DI_ID_A3OFF_2,             // 远控停止(预留)
    DI_ID_A3XCON_1,            // 远控消磁启动(预留)
    DI_ID_A3XCOFF_2            // 远控消磁停止(预留)
} DigitalInputType_t;

// 按钮输出
typedef enum
{
    DO_ID_LOCALREMOTE = 0,   // 本地远端
    DO_ID_DEGAUSS,       // 消磁按键K2
    DO_ID_A3QF1,       // 单机QF1
    DO_ID_A3QR1,       // 单机QR1
    DO_ID_A3QF2,       // 并机QF2
    DO_ID_A3QR2,       // 并机QR2
    DO_ID_A3QR3,       // 消磁回路QR3
    DO_ID_A3QR4,       // 补偿电容
    DO_ID_A3QR5,       // 补偿电容
    DO_ID_A3QR6,       // 补偿电容
    DO_ID_A3QR7,       // 补偿电容
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
    CURRENT_U5_PHASE_A = 0,   // U5_A相电流
    CURRENT_U5_PHASE_B,       // U5_B相电流
    CURRENT_U5_PHASE_C,       // U5_C相电流
    CURRENT_U6_PHASE_A,       // U6_A相电流
    CURRENT_U6_PHASE_B,       // U6_B相电流
    CURRENT_U6_PHASE_C,       // U6_C相电流
    CURRENT_U7_PHASE_A,       // U7_A相电流
    CURRENT_U7_PHASE_B,       // U7_B相电流
    CURRENT_U7_PHASE_C        // U7_C相电流
} CurrentReadType_t;

#endif /* SERVICES_LOGIC_SECTION_COMMON_H_ */
