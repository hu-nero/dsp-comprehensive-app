/*
 * section_cabinet.h
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#ifndef SERVICES_LOGIC_SECTION_CABINET_H_
#define SERVICES_LOGIC_SECTION_CABINET_H_

#include "section_common.h"

// 开关控制回调函数
typedef ExecuteResult_t (*SwitchControlFunc_t)(uint16_t SwitchId, SwitchState_t State);
typedef uint16_t (*SwitchFeedFunc_t)(uint16_t SwitchId);
// 电压读取回调函数
typedef uint16_t (*VoltageReadFunc_t)(VoltageReadType_t VoltagePhaseType);
// 电流读取回调函数
typedef uint16_t (*CurrentReadFunc_t)(CurrentReadType_t CurrentPhaseType);
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

// 开关反馈等待状态枚举
typedef enum {
    SWITCH_FEEDBACK_IDLE = 0,
    SWITCH_FEEDBACK_WAITING,
    SWITCH_FEEDBACK_CHECKING,
    SWITCH_FEEDBACK_TIMEOUT
} SwitchFeedbackWaitState_t;

// 区段柜所有开关
typedef struct
{
    SwitchState_t manual_switch;             // 手动/自动开关状态
    SwitchState_t emergency_stop_switch;   // 急停开关状态
    SwitchState_t reset_switch;            // 复位开关状态
    SwitchState_t single_switch;           // 单机开关状态
    SwitchState_t bus1_switch;             // 母线1开关状态
    SwitchState_t short_switch;            // 面板短接开关状态（消磁）

    SwitchState_t A3QS1_switch;              // 单机/并机母线刀闸
    SwitchState_t A3K17_switch;              // 检修母线刀闸
    SwitchState_t A3QS2_switch;              // 补偿电容刀闸
    SwitchState_t A3QS3_switch;              // 消磁刀闸
    SwitchState_t A3QF1_switch;       // 单机QF1
    SwitchState_t A3QR1_switch;       // 单机QR1
    SwitchState_t A3QF2_switch;       // 并机QF2
    SwitchState_t A3QF3_switch;       // 消磁按键反馈
    SwitchState_t A3QR2_switch;       // 并机QR2
    SwitchState_t A3QR3_switch;       // 消磁回路QR3
    SwitchState_t A3QR4_switch;       // 补偿电容
    SwitchState_t A3QR5_switch;       // 补偿电容
    SwitchState_t A3QR6_switch;       // 补偿电容
    SwitchState_t A3QR7_switch;       // 补偿电容
} SwitchStateList_t;

// 区段柜上下文结构体
typedef struct
{
    CabinetState_t state;                  // 当前状态
    CabinetState_t target_state;           // 保存的目标状态
//    CabinetMode_t mode;                  // 当前模式
    bool emergency_stop_status;            // 急停状态
    bool panel_remote_degauss_flag;        // 面板或者远端消磁标志
    SwitchStateList_t switch_list;         // 所有开关
    PowerStatus_t power_status;			   // 电源柜状态
    DemagnetizeState_t demagnetize_state;  // 消磁状态
    uint16_t demagnetize_counter;          // 消磁计数器（ms）
    uint32_t demagnetize_start_time;       // 消磁开始时间戳

    // 开关反馈等待相关状态
    SwitchFeedbackWaitState_t switch_feedback_wait_state;    // 反馈等待状态
    uint32_t switch_feedback_start_time;                     // 反馈检测开始时间
    uint16_t switch_feedback_id;                             // 反馈的开关ID
    SwitchState_t switch_feedback_target_state;              // 反馈需要的目标状态
    uint16_t switch_feedback_current_value;                  // 当前反馈值

    // 回调函数指针
    SwitchControlFunc_t switch_control;
    SwitchFeedFunc_t switch_feed;
    VoltageReadFunc_t voltage_read;
    CurrentReadFunc_t current_read;
    AlarmCallback_t alarm_callback;
} CabinetContext_t;

extern bool Cabinet_Init(CabinetContext_t* Context,
             SwitchControlFunc_t switch_ctrl,
             SwitchFeedFunc_t  switch_feed,
             VoltageReadFunc_t voltage_read,
             CurrentReadFunc_t current_read,
             AlarmCallback_t alarm_cb);
extern void Cabinet_Process(CabinetContext_t* Context);
extern ExecuteResult_t Cabinet_SetSwitches(CabinetContext_t* Context,
                                    SwitchState_t single_state,
                                    SwitchState_t bus1_state,
                                    SwitchState_t short_state,
                                    bool emergency);
extern CabinetState_t Cabinet_GetState(const CabinetContext_t* Context);
extern ExecuteResult_t Cabinet_RecoverFromEmergency(CabinetContext_t* Context);

#endif /* SERVICES_LOGIC_SECTION_CABINET_H_ */
