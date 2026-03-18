/*
 * section_cabinet.c
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#include "section_cabinet.h"
#include "Services/Adapter/can_adapter.h"
#include "SDK/Hal/Timer/hal_timer.h"
#include <string.h>

// 回路执行流程
static ExecuteResult_t Cabinet_ConnectSingleCircuit(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ConnectParallelCircuit(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_DisconnectSingleCircuit(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_DisconnectParallelCircuit(CabinetContext_t* Context);

// 执行流程
static ExecuteResult_t Cabinet_ExecuteEmergencyProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecutePanelDegaussProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteAutoDegaussProcedure(CabinetContext_t* Context, uint16_t DelayMs);
static ExecuteResult_t Cabinet_ExecuteStandbyProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteSingleProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteParallelProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteBackupProcedure(CabinetContext_t* Context);
static void Cabinet_SetAlarm(CabinetContext_t* Context, AlarmType_t Type, const char* Message);
static void Cabinet_UpdateCabinetState(CabinetContext_t* Context);

// 状态切换(待机、单机、并机、备机)
static ExecuteResult_t Cabinet_BackToStandby(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToSingle(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToParallel(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToBackup(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ErrBranch(CabinetContext_t* Context);

static void Cabinet_ContinueAction(CabinetContext_t* Context);
static void Cabinet_ProcessDemagnetizeWait(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_WaitForSwitchFeedback(CabinetContext_t* Context, uint16_t SwitchId, SwitchState_t TargetState, uint32_t TimeoutMs, uint32_t StableTime, uint32_t DelayMs);

// 开关、回路检测
static inline void Cabinet_ReadAllSwitchStatus(CabinetContext_t* Context);
static inline bool Cabinet_SwitchIsMatchMode(CabinetContext_t* Context);
static bool Cabinet_HasCycle(CabinetContext_t* Context);
static bool Cabinet_IsSatisfyPowerParallel(CabinetContext_t* Context);

// 故障恢复函数
static void Cabinet_ProcessFaultReset(CabinetContext_t* Context);
static void Cabinet_ProcessAutoParallel(CabinetContext_t* Context);
static uint32_t GetFaultTimerDuration(uint8_t fault_code);
static bool Cabinet_IsDecoupleSignalReceived(CabinetContext_t* Context);
static bool Cabinet_IsPowerStarted(CabinetContext_t* Context);
static bool Cabinet_CheckSwitchMatch(CabinetContext_t* Context);
static bool Cabinet_CheckParallelSwitchState(CabinetContext_t* Context);
static void Cabinet_SendParallelRequest(CabinetContext_t* Context);

// 区段柜控制初始化
bool
Cabinet_Init(CabinetContext_t* Context,
             SwitchControlFunc_t switch_ctrl,
             SwitchFeedFunc_t  switch_feed,
             VoltageReadFunc_t voltage_read,
             CurrentReadFunc_t current_read,
             AlarmCallback_t alarm_cb)
{
    if ((!Context) || (!switch_ctrl) ||
        (!switch_feed) || (!alarm_cb)||
        (!voltage_read) || (!current_read))
    {
        return false;
    }

    CAN_Adapter_Init();
    // 初始化上下文
    memset(Context, 0, sizeof(CabinetContext_t));

    // 保存回调函数
    Context->switch_control = switch_ctrl;
    Context->switch_feed = switch_feed;
    Context->voltage_read = voltage_read;
    Context->current_read = current_read;
    Context->alarm_callback = alarm_cb;

    // 设置默认状态
    Context->state = CABINET_STATE_STANDBY;
    Context->target_state = CABINET_STATE_INVALID;
//    Context->mode = CABINET_MODE_MANUAL;
    Context->emergency_stop_status = false;            // 急停状态
    Context->panel_remote_degauss_flag = false;        // 面板或者远端消磁标志
    // 读取面板、柜体开关状态
    Cabinet_ReadAllSwitchStatus(Context);

    // 初始化消磁相关状态
    Context->demagnetize_state = DEMAGNETIZE_IDLE;
    Context->demagnetize_counter = 0;
    Context->demagnetize_start_time = 0;

    // 初始化开关反馈等待状态
    Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
    Context->switch_feedback_start_time = 0;
    Context->switch_feedback_id = 0;
    Context->switch_feedback_target_state = SWITCH_OFF;
    Context->switch_feedback_current_value = 0;

    // 故障自恢复
    Context->prev_power_run_state.run_state = POWER_UNKNOWN;
    Context->prev_power_run_state.status_word = 0;
    Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
    Context->fault_timer_start = 0;
    Context->fault_timer_duration = 0;
    Context->fault_code = 0;
    Context->parallel_request_sent = false;
    Context->parallel_request_start = 0;
    Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
    Context->recovery_degauss_start_time = 0;
    Context->recovery_degauss_delay_ms = 0;

    return true;
}

// 周期处理函数
void
Cabinet_Process(CabinetContext_t* Context)
{
    if (!Context)
    {
        return;
    }

    // 急停流程
    // 急停到底1是急停还是0是
    Cabinet_ExecuteEmergencyProcedure(Context);
    // 急停下只允许面板消磁
    if (Context->emergency_stop_status == true)
    {
        Cabinet_ExecutePanelDegaussProcedure(Context);
        return;
    }
    // 读取手动/自动旋钮状态
    Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);

    // 面板和远控的急停未触发（0128互斥添加）
    if (Context->panel_remote_degauss_flag == false)
    {
        // 自复位
        if (Context->switch_list.manual_switch == SWITCH_OFF)
        {
            Cabinet_ProcessFaultReset(Context);
            Cabinet_ProcessAutoParallel(Context);
        }
        // (手动/远控模式) 状态转换流程
        if (Context->panel_remote_degauss_flag == false)
        {
            switch (Context->demagnetize_state)
            {
                case DEMAGNETIZE_IDLE:
                    {
                        // 根据当前状态处理状态转换
                        Cabinet_UpdateCabinetState(Context);
                    }
                    break;
                case DEMAGNETIZE_WAITING:
                    {
                        // 消磁等待期间不进行状态转换
                        Cabinet_ProcessDemagnetizeWait(Context);
                    }
                    return;
                case DEMAGNETIZE_DONE:
                    {
                        // TODO:读取按钮读取
                        // 根据记录状态处理状态转换
                        Cabinet_ContinueAction(Context);
                    }
                    break;
                case DEMAGNETIZE_FAILED:
                    {
                        // 发出警告，消磁失败，拨回开关
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                        // 消磁状态回到未触发
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                    }
                    break;
                default:break;
            }
        }
    }
    // 面板消磁流程
    Cabinet_ExecutePanelDegaussProcedure(Context);
}

static inline void
Cabinet_ReadAllSwitchStatus(CabinetContext_t* Context)
{
    // 读取面板状态
    Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);
    Context->switch_list.emergency_stop_switch = (SwitchState_t)Context->switch_feed(DI_ID_EMERGENCY_STOP);
    Context->switch_list.reset_switch = (SwitchState_t)Context->switch_feed(DI_ID_RESET);
    Context->switch_list.single_switch = (SwitchState_t)Context->switch_feed(DI_ID_SINGLE);
    Context->switch_list.bus1_switch = (SwitchState_t)Context->switch_feed(DI_ID_BUS1);
    Context->switch_list.short_switch = (SwitchState_t)Context->switch_feed(DI_ID_Short);
    // 读取内部开关状态
    Context->switch_list.A3QS1_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS1);
    Context->switch_list.A3K17_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3K17);
    Context->switch_list.A3QS2_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS2);
    Context->switch_list.A3QS3_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS3);
    Context->switch_list.A3QF1_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QF1);
    Context->switch_list.A3QF2_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QF2);
    Context->switch_list.A3QF3_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QF3);
}

static inline bool
Cabinet_SwitchIsMatchMode(CabinetContext_t* Context)
{
    // 柜内开关
    if (!((Context->switch_list.A3QS1_switch == SWITCH_ON) &&
        (Context->switch_list.A3K17_switch == SWITCH_OFF) &&
        (Context->switch_list.A3QS2_switch == SWITCH_ON) &&
        (Context->switch_list.A3QS3_switch == SWITCH_ON)))
    {
        return false;
    }

    // 面板按钮与柜子状态
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                if ((Context->switch_list.A3QF1_switch == SWITCH_OFF) && (Context->switch_list.A3QF2_switch == SWITCH_OFF))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_SINGLE:
            {
                if ((Context->switch_list.A3QF1_switch == SWITCH_ON) && (Context->switch_list.A3QF2_switch == SWITCH_OFF))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                if ((Context->switch_list.A3QF1_switch == SWITCH_OFF) && (Context->switch_list.A3QF2_switch == SWITCH_ON))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_BACKUP:
            {
                if ((Context->switch_list.A3QF1_switch == SWITCH_ON) && (Context->switch_list.A3QF2_switch == SWITCH_ON))
                {
                    return true;
                }
            }
            break;
        default:break;
    }
    return false;
}

// 获取当前状态
CabinetState_t
Cabinet_GetState(const CabinetContext_t* Context)
{
    return Context ? Context->state : CABINET_STATE_STANDBY;
}

// 更新区段柜状态
static void
Cabinet_UpdateCabinetState(CabinetContext_t* Context)
{
    CabinetState_t target_state;

    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
    {
        Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);
        // 手动模式
        if (Context->switch_list.manual_switch == SWITCH_ON)
        {
            // 读取旋钮状态
            Context->switch_list.single_switch = (SwitchState_t)Context->switch_feed(DI_ID_SINGLE);
            Context->switch_list.bus1_switch = (SwitchState_t)Context->switch_feed(DI_ID_BUS1);
            // 根据开关状态确定目标状态
            if ((Context->switch_list.single_switch == SWITCH_OFF) && (Context->switch_list.bus1_switch == SWITCH_OFF))
            {
                target_state = CABINET_STATE_STANDBY;
            }
            else if ((Context->switch_list.single_switch == SWITCH_ON) && (Context->switch_list.bus1_switch == SWITCH_OFF))
            {
                target_state = CABINET_STATE_SINGLE;
            }
            else if ((Context->switch_list.single_switch == SWITCH_OFF) && (Context->switch_list.bus1_switch == SWITCH_ON))
            {
                target_state = CABINET_STATE_PARALLEL;
            }
            else
            {
                target_state = CABINET_STATE_BACKUP;
            }

            if ((Context->state == CABINET_STATE_BACKUP) ||
                    (Context->state == CABINET_STATE_SINGLE))
            {
                // 读取电源状态,电源状态影响目标状态
                Context->power_status = CAN_Adapter_GetPowerStatus();
                if (!((Context->power_status.run_state == POWER_SINGLE_RUNNING) ||
                            (Context->power_status.run_state == POWER_PARALLEL_RUNNING)))
                {
                    if (Context->switch_list.bus1_switch == SWITCH_OFF)
                    {
                        target_state = CABINET_STATE_STANDBY;
                    }
                    else if (Context->switch_list.bus1_switch == SWITCH_ON)
                    {
                        target_state = CABINET_STATE_PARALLEL;
                    }
                }
            }

            // 状态没有变化
            if (target_state == Context->state)
            {
                Context->target_state = CABINET_STATE_INVALID;
                return;
            }

            // 保存目标状态
            Context->target_state = target_state;
        }
        // 自动模式
        else
        {
            // TODO:接收DCS/监控的单机启动标志
            // 预计是置远程启动标志位
            // 目前先return
            return;
        }
    }

    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                // ① 待机->单机
                if (Context->target_state == CABINET_STATE_SINGLE)
                {
                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    // 读取电源柜状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();
                    if (!((Context->power_status.run_state == POWER_SINGLE_RUNNING) ||
                                (Context->power_status.run_state == POWER_PARALLEL_RUNNING)))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "power status abnormal");
                        return;
                    }
                    if (Cabinet_HasCycle(Context))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                        return;
                    }

                    (void)Cabinet_ExecuteAutoDegaussProcedure(Context, 5000);
                }
                // ② 待机->并机
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    // 判断是否满足并机系数
                    if (!Cabinet_IsSatisfyPowerParallel(Context))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Not meeting the parallel operation coefficient");
                        return;
                    }
                    // 判断柜内是否有回路
                    if (Cabinet_HasCycle(Context))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                        return;
                    }
                    // 消磁
                    (void)Cabinet_ExecuteAutoDegaussProcedure(Context, 5000);
                }
            }
            break;

        case CABINET_STATE_SINGLE:
            {
                // ⑤ 单机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    result = Cabinet_ExecuteStandbyProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        // 是否有回路
                        if (Cabinet_HasCycle(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                            return;
                        }
                        (void)Cabinet_ExecuteAutoDegaussProcedure(Context, 10000);
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                    }
                }
                // ⑥ 单机->备机
                else if (Context->target_state == CABINET_STATE_BACKUP)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    // 读取电源柜状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();
                    if (Context->power_status.run_state != POWER_PARALLEL_RUNNING)
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "power status abnormal");
                        return;
                    }
                    // 判断是否满足并机系数
                    if (!Cabinet_IsSatisfyPowerParallel(Context))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Not meeting the parallel operation coefficient");
                        return;
                    }
                    result = Cabinet_ExecuteBackupProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_BACKUP;
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "start power failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                        // 清空目标状态
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                // ⑦ 并机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    result = Cabinet_ExecuteStandbyProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        // 消磁
                        (void)Cabinet_ExecuteAutoDegaussProcedure(Context, 10000);
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                    }
                }
                // ⑧ 并机->备机
                else if (Context->target_state == CABINET_STATE_BACKUP)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    // 读取电源柜状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();
                    if (!(Context->power_status.run_state == POWER_PARALLEL_RUNNING))
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "power status abnormal");
                        return;
                    }
                    result = Cabinet_ExecuteBackupProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_BACKUP;
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                        // 清空目标状态
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                }
            }
            break;
        case CABINET_STATE_BACKUP:
            {
                // ③ 备机->单机
                if (Context->target_state == CABINET_STATE_SINGLE)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    result = Cabinet_ExecuteSingleProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_SINGLE;
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                        // 清空目标状态
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                }
                // ④ 备机->并机
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    ExecuteResult_t result;

                    // 开关反馈等待期间，不进行柜体开关状态匹配
                    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
                    {
                        // 柜内开关是否匹配柜体模式
                        Cabinet_ReadAllSwitchStatus(Context);
                        if (!Cabinet_SwitchIsMatchMode(Context))
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                            return;
                        }
                    }
                    result = Cabinet_ExecuteParallelProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_PARALLEL;
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        // 根据状态进行回调
                        Cabinet_ErrBranch(Context);
                        // 清空目标状态
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                }
            }
            break;
        default:break;
    }
}

// 延续区段柜状态
static void
Cabinet_ContinueAction(CabinetContext_t* Context)
{
    ExecuteResult_t result;
    uint16_t demag_result;

    // 急停则不执行任何转换
    if (Context->emergency_stop_status == true)
    {
        return;
    }

    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                // ① 待机->单机
                if (Context->target_state == CABINET_STATE_SINGLE)
                {
                    // 断开消磁
                    Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                    // 等待开关反馈稳定3秒
                    demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                    if (demag_result == RESULT_SUCCESS)
                    {
                        ;
                    }
                    else if (demag_result == RESULT_WAIT)
                    {
                        return;
                    }
                    else
                    {
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        return;
                    }

                    // 判断电源单机还是并机
                    Context->power_status = CAN_Adapter_GetPowerStatus();
                    if (Context->power_status.run_state == POWER_SINGLE_RUNNING)
                    {
                        ;
                    }
                    else if (Context->power_status.run_state == POWER_PARALLEL_RUNNING)
                    {
                        // 判断并机系数是否满足
                        if (Cabinet_IsSatisfyPowerParallel(Context))
                        {
                            ;
                        }
                        else
                        {
                            Cabinet_SetAlarm(Context, ALARM_NONE, "Not satisfied parallel factor");
                            Context->target_state = CABINET_STATE_INVALID;
                            Context->demagnetize_state = DEMAGNETIZE_FAILED;
                            return;
                        }
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "The power is not running");
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        return;
                    }

                    result = Cabinet_ExecuteSingleProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_SINGLE;
                        // 重置状态
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                    }
                    else if (result == RESULT_WAIT)
                    {
                    	return;
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Single procedure failed");
                        // 重置
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                    }
                }
                // ② 待机->并机
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    // 断开消磁
                    Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                    // 等待开关反馈稳定3秒
                    demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                    if (demag_result == RESULT_SUCCESS)
                    {
                        ;
                    }
                    else if (demag_result == RESULT_WAIT)
                    {
                        return;
                    }
                    else
                    {
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        return;
                    }

                    // 闭合并机回路
                    result = Cabinet_ExecuteParallelProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_PARALLEL;
                        Context->target_state = CABINET_STATE_INVALID;

                        // 重置状态
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                    }
                    else if (result == RESULT_WAIT)
                    {
                    	return;
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close the parallel circuit");
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                    }
                }
            }
            break;
        case CABINET_STATE_SINGLE:
            {
                // ⑤ 单机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    // 断开消磁
                    Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                    // 等待开关反馈稳定3秒
                    demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                    if (demag_result == RESULT_SUCCESS)
                    {
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->state = CABINET_STATE_STANDBY;
                        Context->target_state = CABINET_STATE_INVALID;
                    }
                    else if (demag_result == RESULT_WAIT)
                    {
                        return;
                    }
                    else
                    {
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        Context->target_state = CABINET_STATE_INVALID;
                        return;
                    }
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                // ⑦ 并机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    // 断开消磁
                    Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                    // 等待开关反馈稳定3秒
                    demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                    if (demag_result == RESULT_SUCCESS)
                    {
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->state = CABINET_STATE_STANDBY;
                        Context->target_state = CABINET_STATE_INVALID;
                        return;
                    }
                    else if (demag_result == RESULT_WAIT)
                    {
                        return;
                    }
                    else
                    {
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        Context->target_state = CABINET_STATE_INVALID;
                        return;
                    }
                }
            }
            break;
        default:
            // 其他状态转换暂未实现
            break;
    }
}

// 急停结束
static ExecuteResult_t
Cabinet_ResetFromEmergency(CabinetContext_t* Context)
{
    // 断开所有开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
        // 调用IO_Scan()
        (void)Context->switch_feed(0);
    }

    // 系统状态设置为待机
    Context->state = CABINET_STATE_STANDBY;
    Context->target_state = CABINET_STATE_INVALID;
    Context->emergency_stop_status = false;

    return RESULT_SUCCESS;
}

// 公共接口：从急停状态恢复
ExecuteResult_t 
Cabinet_RecoverFromEmergency(CabinetContext_t* Context)
{
    if (!Context)
    {
        return RESULT_FAILED;
    }
    
    // 检查急停信号是否已经解除
    Context->switch_list.emergency_stop_switch = (SwitchState_t)Context->switch_feed(DI_ID_EMERGENCY_STOP);
    Context->emergency_stop_status = (Context->switch_list.emergency_stop_switch == SWITCH_OFF);
    if (Context->switch_list.emergency_stop_switch == SWITCH_OFF)
    {
        // 急停信号仍然存在，不能恢复
        return RESULT_FAILED;
    }
    
    // 恢复系统到安全状态
    return Cabinet_ResetFromEmergency(Context);
}

// 返回到待机状态
static ExecuteResult_t
Cabinet_BackToStandby(CabinetContext_t* Context)
{
    // 断开所有开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
        // 调用IO_Scan()
        (void)Context->switch_feed(0);
    }

    Context->state = CABINET_STATE_STANDBY;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

// 返回到单机状态
static ExecuteResult_t
Cabinet_BackToSingle(CabinetContext_t* Context)
{
    // 开启并机相关开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(DO_ID_A3QF1, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QR1, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
        // 调用IO_Scan()
        (void)Context->switch_feed(0);
    }

    Context->state = CABINET_STATE_SINGLE;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

// 返回到并机状态
static ExecuteResult_t
Cabinet_BackToParallel(CabinetContext_t* Context)
{
    // 开启并机相关开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
        (void)Context->switch_control(DO_ID_A3QF2, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QR2, SWITCH_ON);
        // 调用IO_Scan()
        (void)Context->switch_feed(0);
    }

    Context->state = CABINET_STATE_PARALLEL;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

// 返回到备用状态
static ExecuteResult_t
Cabinet_BackToBackup(CabinetContext_t* Context)
{
    // 开启备用相关开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(DO_ID_A3QF1, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QR1, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QF2, SWITCH_ON);
        (void)Context->switch_control(DO_ID_A3QR2, SWITCH_ON);
        // 调用IO_Scan()
        (void)Context->switch_feed(0);
    }

    Context->state = CABINET_STATE_BACKUP;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

static ExecuteResult_t
Cabinet_ErrBranch(CabinetContext_t* Context)
{
    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
        {
            return Cabinet_BackToStandby(Context);
        }
        case CABINET_STATE_SINGLE:
        {
        	return Cabinet_BackToSingle(Context);
        }
        case CABINET_STATE_PARALLEL:
        {
        	return Cabinet_BackToParallel(Context);
        }
        case CABINET_STATE_BACKUP:
        {
        	return Cabinet_BackToBackup(Context);
        }
        default:
            break;
    }
    return RESULT_FAILED;
}

// 处理消磁等待
static void
Cabinet_ProcessDemagnetizeWait(CabinetContext_t* Context)
{
    // 使用时间戳相减方式进行计时
    uint32_t current_time = hal_timer_get_timestamp();
    
    if ((Context->demagnetize_state == DEMAGNETIZE_WAITING) &&
        (Context->demagnetize_counter > 0))
    {
        // 检查是否到达消磁延时时间
        if ((current_time - Context->demagnetize_start_time) >= Context->demagnetize_counter)
        {
            // 消磁完成（超时）
            Context->demagnetize_state = DEMAGNETIZE_DONE;
        }
    }
}

// 面板急停功能
static ExecuteResult_t
Cabinet_ExecuteEmergencyProcedure(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    // 读取急停反馈
    Context->switch_list.emergency_stop_switch = (SwitchState_t)Context->switch_feed(DI_ID_EMERGENCY_STOP);
    // 默认读取的FPGA的IO值为1时，认为急停触发（IO_scan()可以加取反）
    if (Context->switch_list.emergency_stop_switch == SWITCH_ON)
    {
        // 设置急停状态
        Context->emergency_stop_status = true;
        Context->target_state = CABINET_STATE_INVALID;

        // 判断单机回路
        if (Context->switch_feed(DI_ID_A3QF1))
        {
            Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
            Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_OFF, 5000, 3000, 50);
            // 防止急停后，再面板消磁 switch_feedback_wait_state状态干扰
            Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to disconnect A3QF1");
                return RESULT_FAILED;
            }
        }
        // 判断并机回路状态
        if (Context->switch_feed(DI_ID_A3QF2))
        {
            Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
            Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_OFF, 5000, 3000, 50);
            // 防止急停后，再面板消磁 switch_feedback_wait_state状态干扰
            Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to disconnect A3QF2");
                return RESULT_FAILED;
            }
        }
        Context->state = CABINET_STATE_STANDBY;
        Context->target_state = CABINET_STATE_INVALID;
        return RESULT_EMERGENCY_STOP;
    }
    else
    {
        // 恢复
        if (Context->emergency_stop_status == true)
        {
            Cabinet_ResetFromEmergency(Context);
            Context->emergency_stop_status = false;
        }
    }
    return RESULT_SUCCESS;
}

static ExecuteResult_t
Cabinet_ExecutePanelDegaussProcedure(CabinetContext_t* Context)
{
    SwitchState_t switchStateS1_1,switchStateS1_2;
    ExecuteResult_t demag_result;

    // 手动/自动
    Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);
    // 手动模式
    if (Context->switch_list.manual_switch == SWITCH_ON)
    {
        // 消磁面板闭合
        if (Context->panel_remote_degauss_flag == false)
        {
            Context->switch_list.short_switch = (SwitchState_t)Context->switch_feed(DI_ID_Short);
            if (Context->switch_list.short_switch == SWITCH_OFF)
            {
                return RESULT_INVALID_STATE;
            }
        }
        else if (Context->panel_remote_degauss_flag == true)
        {
            Context->switch_list.short_switch = (SwitchState_t)Context->switch_feed(DI_ID_Short);
            if (Context->switch_list.short_switch == SWITCH_ON)
            {
                return RESULT_INVALID_STATE;
            }
        }
    }
    // 自动模式
    else if (Context->switch_list.manual_switch == SWITCH_OFF)
    {
        // TODO:读取远端消磁，或者远端消磁指令触发消磁函数
        return RESULT_INVALID_STATE;
    }

    // 判断是否发生状态转换
    if (((Context->state != Context->target_state)  &&
		 (Context->target_state != CABINET_STATE_INVALID)) ||
		 (Context->state != CABINET_STATE_STANDBY))
    {
        Cabinet_SetAlarm(Context, ALARM_NONE, "Execution of panel demagnetization failed");
        return RESULT_FAILED;
    }

    // 第一次按下面板消磁按钮
    if (Context->panel_remote_degauss_flag == false)
    {
        // 读取刀闸QS1
        switchStateS1_1 = (SwitchState_t)Context->switch_feed(DI_ID_A3QS1);
        switchStateS1_2 = (SwitchState_t)Context->switch_feed(DI_ID_A3K17);
        if ((switchStateS1_1 == SWITCH_OFF) &&
                (switchStateS1_2 == SWITCH_OFF))
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Abnormal position of knife switch QS1");
            return RESULT_FAILED;
        }
        else if ((switchStateS1_1 == SWITCH_ON) &&
                (switchStateS1_2 == SWITCH_OFF))
        {
            // 判断柜内是否有回路
            if (Cabinet_HasCycle(Context))
            {
                // 发出警告
                Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                return RESULT_FAILED;
            }
        }
        else if ((switchStateS1_1 == SWITCH_OFF) &&
                (switchStateS1_2 == SWITCH_ON))
        {
            // TODO:读取同步柜消息，判断所有电源A2QF2是否闭合
            if (0)
            {
                // 发出警告
                Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                return RESULT_FAILED;
            }
        }

        // 闭合消磁
        Context->switch_control(DO_ID_A3QR3, SWITCH_ON);
        // 等待开关反馈稳定3秒
        demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_ON, 2000, 500, 50);
        if (demag_result == RESULT_SUCCESS)
        {
            Context->panel_remote_degauss_flag = true;
            // TODO:消磁灯常亮
        }
        else if (demag_result == RESULT_WAIT)
        {
            return RESULT_WAIT;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "first Demagnetization failed");
            return RESULT_FAILED;
        }
    }
    // 第二次闭合消磁按钮
    else if (Context->panel_remote_degauss_flag == true)
    {
        // 断开消磁
        Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
        // 等待开关反馈稳定3秒
        demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 2000, 500, 50);
        if (demag_result == RESULT_SUCCESS)
        {
            Context->panel_remote_degauss_flag = false;
            // TODO:消磁灯熄灭
        }
        else if (demag_result == RESULT_WAIT)
        {
            return demag_result;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "second Demagnetization failed");
            return RESULT_FAILED;
        }
    }
    return RESULT_SUCCESS;
}

// 自复位消磁
static ExecuteResult_t
Cabinet_ExecuteRecoveryDegaussProcedure(CabinetContext_t* Context, uint16_t DelayMs)
{
	ExecuteResult_t demag_result;
    // 启动消磁流程
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QR3, SWITCH_ON);
        // 等待开关反馈稳定 5 秒
        demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_ON, 5000, 3000, 50);
        if (demag_result == RESULT_SUCCESS)
        {
            Context->recovery_degauss_delay_ms = DelayMs;
            Context->recovery_degauss_start_time = hal_timer_get_timestamp();
            return RESULT_SUCCESS;
        }
        else if (demag_result == RESULT_WAIT)
        {
            return demag_result;
        }
        else
        {
            // 消磁命令执行失败
            Cabinet_SetAlarm(Context, ALARM_NONE, "Recovery degauss command failed");
            return RESULT_FAILED;
        }
    }
    else
    {
        // 消磁命令执行失败
        Cabinet_SetAlarm(Context, ALARM_NONE, "Recovery degauss command failed");
        return RESULT_FAILED;
    }
}

// 执行消磁流程
static ExecuteResult_t
Cabinet_ExecuteAutoDegaussProcedure(CabinetContext_t* Context, uint16_t DelayMs)
{
	ExecuteResult_t demag_result;
    // 启动消磁流程
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QR3, SWITCH_ON);
        // 等待开关反馈稳定3秒
        demag_result = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_ON, 5000, 3000, 50);
        if (demag_result == RESULT_SUCCESS)
        {
            // 消磁命令执行成功，开始等待
            Context->demagnetize_state = DEMAGNETIZE_WAITING;
            Context->demagnetize_counter = DelayMs;  // 存储延迟毫秒数
            Context->demagnetize_start_time = hal_timer_get_timestamp();  // 记录开始时间
            return RESULT_SUCCESS;
        }
        else if (demag_result == RESULT_WAIT)
        {
            return demag_result;
        }
        else
        {
            // 消磁命令执行失败
            Context->demagnetize_state = DEMAGNETIZE_FAILED;
            Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
            // 根据状态进行回调
            Cabinet_ErrBranch(Context);
            return RESULT_FAILED;
        }
    }
    else
    {
        // 消磁命令执行失败
        Context->demagnetize_state = DEMAGNETIZE_FAILED;
        Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
        // 根据状态进行回调
        Cabinet_ErrBranch(Context);
        return RESULT_FAILED;
    }
}

// 执行待机流程
static ExecuteResult_t
Cabinet_ExecuteStandbyProcedure(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t current;

    // 单机时
    if (Context->state == CABINET_STATE_SINGLE)
    {
        // 断开 A3QR1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
        }
        // I<50A
        /*current = Context->current_read(CURRENT_I5_PHASE_A);*/
        /*if (current >= 50.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 断开 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_OFF, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }
    }
    // 并机时
    else if (Context->state == CABINET_STATE_PARALLEL)
    {
        // 断开 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
        }
        // I<50A
        /*current = Context->current_read(CURRENT_I5_PHASE_A);*/
        /*if (current >= 50.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 断开 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_OFF, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }
    }
    return RESULT_SUCCESS;
}

// 执行单机流程
static ExecuteResult_t
Cabinet_ExecuteSingleProcedure(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t voltage = 0;

    // 待机时
    if (Context->state == CABINET_STATE_STANDBY)
    {
        // 闭合 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF1, SWITCH_ON);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_ON, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to connect A3QF1");
                return RESULT_FAILED;
            }
        }

        // U<36V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage >= 36.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 闭合 A3QR1
        if (Context->switch_control)
        {
            uint16_t voltage;
            Context->switch_control(DO_ID_A3QR1, SWITCH_ON);
            // U>50V
            /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
            /*if (voltage < 50.0)*/
            /*{*/
                /*return RESULT_FAILED;*/
            /*}*/
        }
    }
    // 备用时
    else if (Context->state == CABINET_STATE_BACKUP)
    {
        // 断开 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
        }

        // 断开 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_OFF, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }
    }

    return RESULT_SUCCESS;
}

// 执行并机流程
static ExecuteResult_t
Cabinet_ExecuteParallelProcedure(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t voltage;

    // 待机时
    if (Context->state == CABINET_STATE_STANDBY)
    {
        // 闭合 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF2, SWITCH_ON);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_ON, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }

        // U<36V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage >= 36.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 闭合 A3QR2
        if (Context->switch_control)
        {
            uint16_t voltage;
            Context->switch_control(DO_ID_A3QR2, SWITCH_ON);
            // U>50V
            /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
            /*if (voltage < 50.0)*/
            /*{*/
                /*return RESULT_FAILED;*/
            /*}*/
        }
    }

    // 备机时
    if (Context->state == CABINET_STATE_BACKUP)
    {
        // 断开 A3QR1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
        }

        // 断开 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_OFF, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }
    }
    return RESULT_SUCCESS;
}

// 执行备机流程
static ExecuteResult_t
Cabinet_ExecuteBackupProcedure(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t voltage;

    // 单机时
    if (Context->state == CABINET_STATE_SINGLE)
    {
        // 闭合 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF2, SWITCH_ON);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_ON, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }

        // U<36V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage >= 36.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 闭合 A3QR2
        if (Context->switch_control)
        {
            uint16_t voltage;
            Context->switch_control(DO_ID_A3QR2, SWITCH_ON);
            // U>50V
            /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
            /*if (voltage < 50.0)*/
            /*{*/
                /*return RESULT_FAILED;*/
            /*}*/
        }
    }

    // 并机时
    if (Context->state == CABINET_STATE_PARALLEL)
    {
        // 闭合 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(DO_ID_A3QF1, SWITCH_ON);
            // 等待开关反馈稳定3秒
            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_ON, 5000, 3000, 50);
            if (res == RESULT_SUCCESS)
            {
                ;
            }
            else if (res == RESULT_WAIT)
            {
                return res;
            }
            else
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }

        // U<36V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage >= 36.0)*/
        /*{*/
            /*return RESULT_FAILED;*/
        /*}*/

        // 闭合 A3QR1
        if (Context->switch_control)
        {
            uint16_t voltage;
            Context->switch_control(DO_ID_A3QR1, SWITCH_ON);
            // U>50V
            /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
            /*if (voltage < 50.0)*/
            /*{*/
                /*return RESULT_FAILED;*/
            /*}*/
        }
    }
    return RESULT_SUCCESS;
}

static bool
Cabinet_HasCycle(CabinetContext_t* Context)
{
    uint16_t res = 0;
    res = Context->switch_feed(DI_ID_A3QF1);
    res |= Context->switch_feed(DI_ID_A3QF2);
    // 无回路
    if (res == 0)
    {
        return false;
    }
    return true;
}

// 设置报警
static void
Cabinet_SetAlarm(CabinetContext_t* Context, AlarmType_t Type, const char* Message)
{
    if (Context && Context->alarm_callback)
    {
        Context->alarm_callback(Type, Message);
    }
}
static bool
Cabinet_IsSatisfyPowerParallel(CabinetContext_t* Context)
{
    // TODO:判断并机系数是否满足
    return true;
}

// 等待开关反馈稳定
static ExecuteResult_t
Cabinet_WaitForSwitchFeedback(CabinetContext_t* Context, uint16_t SwitchId, SwitchState_t TargetState, uint32_t TimeoutMs, uint32_t StableTime, uint32_t DelayMs)
{
    static uint16_t check_count = 0;
    uint32_t current_time = hal_timer_get_timestamp();
    uint16_t current_feedback = 0;

    // 是否等待不同的开关
    if ((Context->switch_feedback_wait_state != SWITCH_FEEDBACK_IDLE) && 
        (Context->switch_feedback_id != SwitchId))
    {
        // 立即返回等待状态
        Context->switch_feedback_current_value = Context->switch_feed(SwitchId);
        return RESULT_SUCCESS;
    }

    // 空闲
    if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_IDLE)
    {
        check_count = 0;
        Context->switch_feedback_id = SwitchId;
        Context->switch_feedback_target_state = TargetState;
        Context->switch_feedback_start_time = current_time;
        Context->switch_feedback_wait_state = SWITCH_FEEDBACK_WAITING;
        Context->switch_feedback_current_value = Context->switch_feed(SwitchId);
        return RESULT_WAIT;
    }
    // 等待
    else if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_WAITING)
    {
        // 传播延迟
        if ((current_time - Context->switch_feedback_start_time) < DelayMs)
        {
            return RESULT_WAIT;
        }
        Context->switch_feedback_wait_state = SWITCH_FEEDBACK_CHECKING;
        Context->switch_feedback_start_time = current_time;
        return RESULT_WAIT;
    }
    // 检查
    else if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_CHECKING)
    {
        // 是否超时
        if ((current_time - Context->switch_feedback_start_time) >= TimeoutMs)
        {
            check_count = 0;
            Context->switch_feedback_wait_state = SWITCH_FEEDBACK_TIMEOUT;
            return RESULT_FAILED;
        }
        // 检查反馈
        current_feedback = Context->switch_feed(SwitchId);
        if ((SwitchState_t)current_feedback == TargetState)
        {
            // 是否稳定
            if ((current_time - Context->switch_feedback_start_time) >= StableTime)
            {
                check_count = 0;
                Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
                Context->switch_feedback_start_time = 0;
                Context->switch_feedback_id = 0;
                Context->switch_feedback_target_state = SWITCH_OFF;
                Context->switch_feedback_current_value = 0;
                return RESULT_SUCCESS;
            }
            else
            {
                return RESULT_WAIT;
            }
        }
        else
        {
            if (check_count < 3)
            {
                check_count ++;
                return RESULT_WAIT;
            }
            check_count = 0;
            Context->switch_feedback_wait_state = SWITCH_FEEDBACK_TIMEOUT;
            return RESULT_FAILED;
        }
    }
    else if (Context->switch_feedback_wait_state == SWITCH_FEEDBACK_TIMEOUT)
    {
        // 重置等待状态
        check_count = 0;
        Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
        Context->switch_feedback_start_time = 0;
        Context->switch_feedback_id = 0;
        Context->switch_feedback_target_state = SWITCH_OFF;
        Context->switch_feedback_current_value = 0;
        return RESULT_FAILED;
    }

    // 失败
    check_count = 0;
    Context->switch_feedback_wait_state = SWITCH_FEEDBACK_IDLE;
    return RESULT_FAILED;

}

// 根据故障码获取计时时长（ms）
static uint32_t
GetFaultTimerDuration(uint8_t fault_code)
{
    switch (fault_code)
    {
        case 0x01: return 60000;   // 重启失败 -> 60s
        case 0x02: return 120000;  // 停电 -> 120s
        case 0x03: return 60000;   // 非停水停电 -> 60s
        case 0x04: return 1800000; // 停水 -> 30min
        default:   return 60000;
    }
}

// 检测是否收到同步柜解列信号
static bool
Cabinet_IsDecoupleSignalReceived(CabinetContext_t* Context)
{
    // TODO: 从CAN接收缓冲区读取同步柜解列标志
    // 修改can_adapter.c
    (void)Context;
    return false;
}

// 检测功率柜是否已启动（单机运行）
static bool
Cabinet_IsPowerStarted(CabinetContext_t* Context)
{
    PowerStatus_t power = CAN_Adapter_GetPowerStatus();
    return (power.run_state == POWER_SINGLE_RUNNING);
}

// 检查区段柜内开关状态是否匹配单机模式
static bool
Cabinet_CheckSwitchMatch(CabinetContext_t* Context)
{
    // TODO:待修改
    return (Context->switch_list.A3QS1_switch == SWITCH_ON &&
            Context->switch_list.A3K17_switch == SWITCH_OFF);
}

// 检查并机前开关状态是否匹配
static bool
Cabinet_CheckParallelSwitchState(CabinetContext_t* Context)
{
    // TODO:待修改
    return (Context->switch_list.A3QF1_switch == SWITCH_OFF &&
            Context->switch_list.A3QF2_switch == SWITCH_OFF);
}

// 发送并网请求（需实现CAN发送）
static void
Cabinet_SendParallelRequest(CabinetContext_t* Context)
{
    // TODO: 通过CAN发送并网请求给同步柜
    CAN_Adapter_SendParallelRequest();
    (void)Context;
}

static ExecuteResult_t
Cabinet_ConnectSingleCircuit(CabinetContext_t* Context)
{
	ExecuteResult_t res;
    // 闭合 A3QF1
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QF1, SWITCH_ON);
        // 等待开关反馈稳定3秒
        res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_ON, 5000, 3000, 50);
        if (res == RESULT_SUCCESS)
        {
            ;
        }
        else if (res == RESULT_WAIT)
        {
            return res;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to connect A3QF1");
            return RESULT_FAILED;
        }
    }

    // U<36V
    /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
    /*if (voltage >= 36.0)*/
    /*{*/
    /*return RESULT_FAILED;*/
    /*}*/

    // 闭合 A3QR1
    if (Context->switch_control)
    {
        uint16_t voltage;
        Context->switch_control(DO_ID_A3QR1, SWITCH_ON);
        // U>50V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage < 50.0)*/
        /*{*/
        /*return RESULT_FAILED;*/
        /*}*/
    }
}

static ExecuteResult_t
Cabinet_ConnectParallelCircuit(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t voltage;
    // 闭合 A3QF2
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QF2, SWITCH_ON);
        // 等待开关反馈稳定3秒
        res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_ON, 5000, 3000, 50);
        if (res == RESULT_SUCCESS)
        {
            ;
        }
        else if (res == RESULT_WAIT)
        {
            return res;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
            return RESULT_FAILED;
        }
    }

    // U<36V
    /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
    /*if (voltage >= 36.0)*/
    /*{*/
    /*return RESULT_FAILED;*/
    /*}*/

    // 闭合 A3QR2
    if (Context->switch_control)
    {
        uint16_t voltage;
        Context->switch_control(DO_ID_A3QR2, SWITCH_ON);
        // U>50V
        /*voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);*/
        /*if (voltage < 50.0)*/
        /*{*/
        /*return RESULT_FAILED;*/
        /*}*/
    }
}

static ExecuteResult_t
Cabinet_DisconnectSingleCircuit(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    uint16_t voltage;
    // 断开 A3QR1
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QR1, SWITCH_OFF);
    }
    // I<50A
    /*current = Context->current_read(CURRENT_I5_PHASE_A);*/
    /*if (current >= 50.0)*/
    /*{*/
    /*return RESULT_FAILED;*/
    /*}*/

    // 断开 A3QF1
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QF1, SWITCH_OFF);
        // 等待开关反馈稳定3秒
        res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF1, SWITCH_OFF, 5000, 3000, 50);
        if (res == RESULT_SUCCESS)
        {
            ;
        }
        else if (res == RESULT_WAIT)
        {
            return res;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
            return RESULT_FAILED;
        }
    }
}

static ExecuteResult_t
Cabinet_DisconnectParallelCircuit(CabinetContext_t* Context)
{
	ExecuteResult_t res;
    // 断开 A3QR2
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QR2, SWITCH_OFF);
    }
    // I<50A
    /*current = Context->current_read(CURRENT_I5_PHASE_A);*/
    /*if (current >= 50.0)*/
    /*{*/
    /*return RESULT_FAILED;*/
    /*}*/

    // 断开 A3QF2
    if (Context->switch_control)
    {
        Context->switch_control(DO_ID_A3QF2, SWITCH_OFF);
        // 等待开关反馈稳定3秒
        res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF2, SWITCH_OFF, 5000, 3000, 50);
        if (res == RESULT_SUCCESS)
        {
            ;
        }
        else if (res == RESULT_WAIT)
        {
            return res;
        }
        else
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
            return RESULT_FAILED;
        }
    }
}

// 核心故障恢复状态机
static void
Cabinet_ProcessFaultReset(CabinetContext_t* Context)
{
    ExecuteResult_t res;
    // 急停则退出
    if (Context->emergency_stop_status)
    {
        if (Context->fault_recovery_state != AUTO_RECOVERY_IDLE)
        {
            Cabinet_SetAlarm(Context, ALARM_NONE, "Auto recovery aborted by emergency");
            Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
        }
        return;
    }

    // 故障恢复状态机
    switch (Context->fault_recovery_state)
    {
        case AUTO_RECOVERY_IDLE:
            {
                // 读取电源柜状态
                Context->power_status = CAN_Adapter_GetPowerStatus();
                if (Context->power_status.run_state == POWER_FAULT)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_FAULTCODE_PROCESS;
                }
                else
                {
                    // TODO: 同步柜全部解列信号
                    /*if (true)*/
                    /*{*/
                        /*Context->fault_recovery_state = AUTO_RECOVERY_DISCONNECT_PARALLEL_CIRCUIT;*/
                    /*}*/
                    /*else*/
                    /*{*/
                        /*return;*/
                    /*}*/
                }
            }
            break;
        case AUTO_RECOVERY_FAULTCODE_PROCESS:
            {
                Context->fault_timer_start = hal_timer_get_timestamp();
                // TODO: 根据通信协议进行故障解析
                Context->fault_timer_duration = GetFaultTimerDuration(0xFF);

                if (Context->state == CABINET_STATE_SINGLE)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_DISCONNECT_SINGLE_CIRCUIT;
                }
                else if (Context->state == CABINET_STATE_PARALLEL)

                {
                    // TODO: 同步柜全部解列信号
                    /*if (true)*/
                    /*{*/
                        /*Context->fault_recovery_state = AUTO_RECOVERY_DISCONNECT_PARALLEL_CIRCUIT;*/
                    /*}*/
                    /*else*/
                    /*{*/
                        /*return;*/
                    /*}*/
                }
                else
                {
                    Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet state Abnormal");
                    Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
                }
            }
            break;
        case AUTO_RECOVERY_DISCONNECT_SINGLE_CIRCUIT:
            {
                res = Cabinet_DisconnectSingleCircuit(Context);
                if (res == RESULT_SUCCESS)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_DISCONNECT_CIRCUIT_DEGAUSS;
                }
                else if (res == RESULT_FAILED)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                }
            }
            break;
        case AUTO_RECOVERY_DISCONNECT_PARALLEL_CIRCUIT:
            {
                res = Cabinet_DisconnectParallelCircuit(Context);
                if (res == RESULT_SUCCESS)
                {
                	Context->fault_recovery_state = AUTO_RECOVERY_DISCONNECT_CIRCUIT_DEGAUSS;
                }
                else if (res == RESULT_FAILED)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                }
            }
            break;
        case AUTO_RECOVERY_DISCONNECT_CIRCUIT_DEGAUSS:
            {
                // 子状态消磁
                switch (Context->recovery_degauss_sub_state)
                {
                    case AUTO_RECOVERY_SUB_DEGAUSS_START:
                        {
                            if (Cabinet_HasCycle(Context))
                            {
                                // 发出警告
                                Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_FAILED;
                                return;
                            }

                            // 自复位消磁
                            ExecuteResult_t degauss_res = Cabinet_ExecuteRecoveryDegaussProcedure(Context, 10000);
                            if (degauss_res == RESULT_SUCCESS)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_WAIT;
                            }
                            else if (degauss_res == RESULT_FAILED)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_FAILED;
                            }
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_WAIT:
                        {
                            // 等待消磁
                            uint32_t now = hal_timer_get_timestamp();
                            if (now - Context->recovery_degauss_start_time >= Context->recovery_degauss_delay_ms)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_DONE;
                            }
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_DONE:
                        {
                            // 断开消磁
                            Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                            // 等待开关反馈稳定3秒
                            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                            if (res == RESULT_SUCCESS)
                            {
                                Context->fault_recovery_state = AUTO_RECOVERY_PWR_RESTART_JUDGMENT_PROCESS;
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                            }
                            else if (res == RESULT_WAIT)
                            {
                                return;
                            }
                            else
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                                return;
                            }

                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_FAILED:
                        {
                            // 失败
                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                            Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                        }
                        break;
                    default:
                        {
                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_START;
                        }
                        break;
                }
            }
            break;
        case AUTO_RECOVERY_PWR_RESTART_JUDGMENT_PROCESS:
            {
                uint32_t now = hal_timer_get_timestamp();
                if (now-Context->fault_timer_start < Context->fault_timer_duration)
                {
                    Context->power_status = CAN_Adapter_GetPowerStatus();
					if (Context->power_status.run_state == POWER_SINGLE_RUNNING)
                    {
                    	Context->fault_recovery_state = AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT_DEGAUSS;
                    }
                }
                else
                {
                	Context->fault_recovery_state = AUTO_RECOVERY_REQUEST_PARALLEL;
                }
            }
            break;
        case AUTO_RECOVERY_REQUEST_PARALLEL:
            {
                static uint16_t count = 0;
                GridConnectionStatus_t grid_state;
                Cabinet_SendParallelRequest(Context);
                grid_state = CAN_Adapter_GetGridConnectionStatus();
                if ((grid_state.state != GRID_CONNECTED) && (count < 50))
                {
                    count ++;
                }
                else if (grid_state.state == GRID_CONNECTED)
                {
                    count = 0;
                	Context->fault_recovery_state = AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT_DEGAUSS;
                }
                else
                {
                	Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                }
            }
            break;
        case AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT_DEGAUSS:
        case AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT_DEGAUSS:
            {
                // 子状态消磁
                switch (Context->recovery_degauss_sub_state)
                {
                    case AUTO_RECOVERY_SUB_DEGAUSS_START:
                        {
                            if (Cabinet_HasCycle(Context))
                            {
                                // 发出警告
                                Cabinet_SetAlarm(Context, ALARM_NONE, "cabinet has cycle");
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_FAILED;
                                return;
                            }

                            // 自复位消磁
                            ExecuteResult_t degauss_res = Cabinet_ExecuteRecoveryDegaussProcedure(Context, 5000);
                            if (degauss_res == RESULT_SUCCESS)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_WAIT;
                            }
                            else if (degauss_res == RESULT_FAILED)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_FAILED;
                            }
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_WAIT:
                        {
                            // 等待消磁
                            uint32_t now = hal_timer_get_timestamp();
                            if (now - Context->recovery_degauss_start_time >= Context->recovery_degauss_delay_ms)
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_DONE;
                            }
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_DONE:
                        {
                            // 断开消磁
                            Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                            // 等待开关反馈稳定3秒
                            res = Cabinet_WaitForSwitchFeedback(Context, DI_ID_A3QF3, SWITCH_OFF, 5000, 3000, 50);
                            if (res == RESULT_SUCCESS)
                            {
                                ;
                            }
                            else if (res == RESULT_WAIT)
                            {
                                return;
                            }
                            else
                            {
                                Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                                return;
                            }
                            // 判断单机还是并机
                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                            if (Context->fault_recovery_state == AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT_DEGAUSS)
                            {
                                Context->fault_recovery_state = AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT;
                            }
                            else if (Context->fault_recovery_state == AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT_DEGAUSS)
                            {
                                Context->fault_recovery_state = AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT;
                            }
                        }
                        break;
                    case AUTO_RECOVERY_SUB_DEGAUSS_FAILED:
                        {
                            // 失败
                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_INVALID;
                            Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                        }
                        break;
                    default:
                        {
                            Context->recovery_degauss_sub_state = AUTO_RECOVERY_SUB_DEGAUSS_START;
                        }
                        break;
                }
            }
            break;
        case AUTO_RECOVERY_CONNECT_SINGLE_CIRCUIT:
            {
                res = Cabinet_ConnectSingleCircuit(Context);
                if (res == RESULT_SUCCESS)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
                }
                else if (res == RESULT_FAILED)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                }
            }
            break;
        case AUTO_RECOVERY_CONNECT_PARALLEL_CIRCUIT:
            {
                res = Cabinet_ConnectParallelCircuit(Context);
                if (res == RESULT_SUCCESS)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
                }
                else if (res == RESULT_FAILED)
                {
                    Context->fault_recovery_state = AUTO_RECOVERY_FAILED;
                }
            }
            break;
        case AUTO_RECOVERY_FAILED:
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "auto recovery failed");
                Context->fault_recovery_state = AUTO_RECOVERY_IDLE;
            }
            break;
        default:break;
    }
}

// 自动并机流程
static void
Cabinet_ProcessAutoParallel(CabinetContext_t* Context)
{
    ExecuteResult_t res;

    // 读取电源柜状态
    Context->power_status = CAN_Adapter_GetPowerStatus();
    // 判断电源是否由单机->并机
    if (Context->power_status.single_to_parallel_flag == false)
    {
        return;
    }

    // 闭合并机回路
    res = Cabinet_ConnectParallelCircuit(Context);
    if (res == RESULT_SUCCESS)
    {
        ;
    }
    else if (res == RESULT_WAIT)
    {
        return;
    }
    else
    {
        Cabinet_SetAlarm(Context, ALARM_NONE, "The parallel operation circuit connection failed");
        return;
    }

    // 断开单机回路
    res = Cabinet_DisconnectSingleCircuit(Context);
    if (res == RESULT_SUCCESS)
    {
        CAN_Adapter_ClearPowerStatus();
    }
    else if (res == RESULT_WAIT)
    {
        return;
    }
    else
    {
        Cabinet_SetAlarm(Context, ALARM_NONE, "The parallel operation circuit connection failed");
        return;
    }
}





