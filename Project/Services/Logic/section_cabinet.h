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
typedef float (*CurrentReadFunc_t)(CurrentReadType_t CurrentPhaseType);
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

    SwitchState_t remote_ctrl_single_feed_switch;            // 远端单机反馈的控制
    SwitchState_t remote_ctrl_parallel_feed_switch;            // 面板短接开关状态（消磁）

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

    SwitchState_t RUNLED_switch;       // 补偿电容
    SwitchState_t FLTLED_switch;       // 补偿电容
} SwitchStateList_t;

// 故障恢复状态枚举
typedef enum
{
    AUTO_RECOVERY_IDLE = 0,
    AUTO_RECOVERY_FAULTCODE_PROCESS,
    AUTO_RECOVERY_DISCONNECT_SINGLE_CIRCUIT,
    AUTO_RECOVERY_DISCONNECT_PARALLEL_CIRCUIT,
    AUTO_RECOVERY_DISCONNECT_CIRCUIT_DEGAUSS,
    AUTO_RECOVERY_PWR_RESTART_JUDGMENT_PROCESS,
    AUTO_RECOVERY_REQUEST_PARALLEL,
    AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT_DEGAUSS,
    AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT_DEGAUSS,
    AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT,
    AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT,
    AUTO_RECOVERY_FAILED
} AutoRecoveryState_t;

// 故障恢复子状态(消磁状态)枚举
typedef enum
{
    AUTO_RECOVERY_SUB_DEGAUSS_START,
    AUTO_RECOVERY_SUB_DEGAUSS_WAIT,
    AUTO_RECOVERY_SUB_DEGAUSS_DONE,
    AUTO_RECOVERY_SUB_DEGAUSS_FAILED,
    AUTO_RECOVERY_SUB_INVALID
} AutoRecoverySubState_t;

// 区段柜上下文结构体
typedef struct
{
    CabinetState_t state;                  // 当前状态
    CabinetState_t target_state;           // 保存的目标状态
    bool emergency_stop_status;            // 急停状态
    bool fault_status;                     // 故障状态
    bool panel_remote_degauss_flag;        // 面板或者远端消磁标志
    AutoRecoverySubState_t degauss_10s_sub_state;  // 自复位消磁子状态
    AutoRecoverySubState_t flt_degauss_sub_state;  // 自复位消磁子状态
    SwitchStateList_t switch_last;         // 所有开关指令
    SwitchStateList_t switch_list;         // 所有开关状态
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

    // 故障恢复
    AutoRecoveryState_t fault_recovery_state;           // 故障恢复状态
    AutoRecoverySubState_t recovery_degauss_sub_state;  // 自复位消磁子状态
    uint32_t fault_timer_start;                         // 当前步骤计时开始时间
    uint32_t fault_timer_duration;                      // 当前步骤计时时长（ms）
    uint16_t fault_code;                                // 电源故障码
    bool parallel_request_sent;                         // 并网请求已发送标志
    uint32_t parallel_request_start;                    // 并网请求发送时间戳
    uint32_t recovery_degauss_start_time;               // 自复位消磁开始时间
    uint16_t recovery_degauss_delay_ms;                 // 自复位消磁延时

    // 故障急停自动恢复相关
    CabinetState_t state_before_fault;                  // 故障前保存的状态
    SwitchStateList_t switch_list_before_fault;         // 故障前保存的开关状态
    uint32_t emergency_degauss_complete_time;           // 急停消磁完成时间戳
    bool fault_auto_recovery_enabled;                   // 故障自动恢复使能标志
    uint16_t u16npr_old_code;
    uint16_t last_fault_code;                           // 上一次发生的故障码
    uint32_t last_fault_timestamp;                      // 上一次故障发生的时间戳（ms）
    bool fault_cleared_processed;                   // 故障自动恢复使能标志

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
extern bool Cabinet_GetPowerOnlineStatus(void);
extern uint16_t Cabinet_GetComPowerToSectionParamStatus1(void);
extern uint16_t Cabinet_GetComSyncToSectionStatus(void);
extern uint16_t Cabinet_GetComPowerToSectionStatus(void);
extern uint16_t Cabinet_SetExternalAddr(uint16_t Addr);

#endif /* SERVICES_LOGIC_SECTION_CABINET_H_ */
