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
    POWER_STANDBY = 0,  // 待机
    POWER_RUNNING,      // 运行
    POWER_FAULT,        // 故障
    POWER_UNKNOWN       // 未知
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
typedef enum
{
    CABINET_MODE_MANUAL = 0,   // 手动模式
    CABINET_MODE_AUTO          // 自动模式
} CabinetMode_t;

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

#endif /* SERVICES_LOGIC_SECTION_COMMON_H_ */
