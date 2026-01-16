/*
 * section_cabinet.h
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#ifndef SERVICES_LOGIC_SECTION_CABINET_H_
#define SERVICES_LOGIC_SECTION_CABINET_H_

#include "section_common.h"

#define SWITCH_ID_A3QF1         0x01    // A3QF1开关
#define SWITCH_ID_A3QR1         0x02    // A3QR1开关
#define SWITCH_ID_A3QF2         0x03    // A3QF1开关
#define SWITCH_ID_A3QR2         0x04    // A3QR1开关
#define SWITCH_ID_SINGLE        0x05    // 单机开关
#define SWITCH_ID_BUS1          0x06    // 母线1开关
#define SWITCH_ID_SHORT         0x07    // 短接开关

#define POWER_START_TIMEOUT_MS  1000    // 电源启动超时（1秒）
#define TIMER_50MS_INTERVAL     50      // 50ms计时单位
#define DEMAGNETIZE_TIMEOUT_MS  10000   // 消磁超时（10秒），50ms*200

// 开关控制回调函数
typedef ExecuteResult_t (*SwitchControlFunc_t)(uint16_t SwitchId, SwitchState_t State);
typedef uint16_t (*SwitchFeedFunc_t)(uint16_t SwitchId);
// 电压读取回调函数
typedef uint16_t (*VoltageReadFunc_t)(VoltageReadType_t VoltagePhaseType);
// 报警回调函数
typedef void (*AlarmCallback_t)(AlarmType_t AlarmType, const char* Message);

// 消磁状态枚举
typedef enum
{
    DEMAGNETIZE_IDLE = 0,      // 空闲状态
    DEMAGNETIZE_WAITING,       // 等待消磁完成
    DEMAGNETIZE_DONE,          // 消磁完成
    DEMAGNETIZE_FAILED         // 消磁失败
} DemagnetizeState_t;

// 电源操作状态枚举
typedef enum
{
    POWER_OP_IDLE = 0,         // 空闲状态
    POWER_OP_STARTING,         // 正在启动电源
    POWER_OP_CLOSING           // 正在关闭电源
} PowerOpState_t;

// 区段柜上下文结构体
typedef struct
{
    CabinetState_t state;              // 当前状态
    CabinetState_t target_state;       // 保存的目标状态
    CabinetMode_t mode;                // 当前模式
    SwitchState_t single_switch;       // 单机开关状态
    SwitchState_t bus1_switch;         // 母线1开关状态
    SwitchState_t short_switch;        // 短接开关状态（消磁）
    bool emergency_stop;               // 急停状态
    PowerStatus_t power_status;        // 电源状态
    uint32_t power_op_timeout;      // 电源启动/关闭超时计数
    uint32_t power_op_counter;         // 电源操作计数器
    PowerOpState_t power_op_state;     // 电源操作状态
    DemagnetizeState_t demagnetize_state;  // 消磁状态
    uint16_t demagnetize_counter;          // 消磁计数器（50ms为单位）

    // 回调函数指针
    SwitchControlFunc_t switch_control;
    SwitchFeedFunc_t switch_feed;
    VoltageReadFunc_t voltage_read;
    AlarmCallback_t alarm_callback;
} CabinetContext_t;

extern bool Cabinet_Init(CabinetContext_t* Context,
             SwitchControlFunc_t switch_ctrl,
             SwitchFeedFunc_t  switch_feed,
             VoltageReadFunc_t voltage_read,
             AlarmCallback_t alarm_cb);
extern void Cabinet_Process(CabinetContext_t* Context);
extern ExecuteResult_t Cabinet_SetSwitches(CabinetContext_t* Context,
                                    SwitchState_t single_state,
                                    SwitchState_t bus1_state,
                                    SwitchState_t short_state,
                                    bool emergency);

extern ExecuteResult_t Cabinet_SetMode(CabinetContext_t* Context, CabinetMode_t Mode);
extern CabinetState_t Cabinet_GetState(const CabinetContext_t* Context);
extern CabinetMode_t Cabinet_GetMode(const CabinetContext_t* Context);

#endif /* SERVICES_LOGIC_SECTION_CABINET_H_ */
