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


static inline bool Cabinet_SwitchIsMatchMode(CabinetContext_t* Context);
static inline void Cabinet_ReadAllSwitchStatus(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_SingleActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ParallelActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackupToSingleActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackupToParallelActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ParallelToBackupActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_SingleToStandbyActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_SingleToBackupActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_StartPowerSupply(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ClosePowerSupply(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_StartThyristorOpIntervalCount(CabinetContext_t* Context, uint32_t Delay);
static ExecuteResult_t Cabinet_ExecuteDegaussProcedure(CabinetContext_t* Context, uint16_t DelayMs);
static ExecuteResult_t Cabinet_ExecuteStandbyProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteSingleProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteParallelProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteBackupProcedure(CabinetContext_t* Context);
static void Cabinet_SetAlarm(CabinetContext_t* Context, AlarmType_t Type, const char* Message);
static void Cabinet_UpdateCabinetState(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToStandby(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToSingle(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToParallel(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToBackup(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ErrBranch(CabinetContext_t* Context);
static void Cabinet_ContinueAction(CabinetContext_t* Context);
static void Cabinet_ProcessDemagnetizeWait(CabinetContext_t* Context);
static bool Cabinet_HasCycle(CabinetContext_t* Context);
static bool Cabinet_IsSatisfyPowerParallel(CabinetContext_t* Context);

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
    Context->emergency_stop_status = false;
    // 读取面板、柜体开关状态
    Cabinet_ReadAllSwitchStatus(Context);

    // 初始化消磁相关状态
    Context->demagnetize_state = DEMAGNETIZE_IDLE;
    Context->demagnetize_counter = 0;

    // 初始化电源状态
    Context->power_status.run_state = POWER_UNKNOWN;
    Context->power_status.comb_state = POWER_COMB_UNKNOWN;
    Context->power_status.ctrl_mode = CONTROL_MODE_UNKNOWN;
    Context->power_status.status_word = 0;
    Context->power_status.is_using = 0;
    Context->power_op_state = POWER_OP_IDLE;
    Context->power_op_counter = 0;
    // 初始化按钮开关间隔状态
    Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
    Context->op_interval_between_thyristor_timeout = 0;
    Context->op_interval_between_thyristor_counter = 0;


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
            }
            break;
        default:break;
    }

}

static inline void
Cabinet_ReadAllSwitchStatus(CabinetContext_t* Context)
{
    // 读取面板状态
    Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);
    Context->switch_list.emergency_stop_switch = (SwitchState_t)Context->switch_feed(DI_ID_EMERGENCY_STOP);
    Context->switch_list.reset_switch = (SwitchState_t)Context->switch_feed(DI_ID_RESET);
    // TODO:待完善按钮反馈
//    Context->switch_list.single_switch = (SwitchState_t)Context->switch_feed();
//    Context->switch_list.bus1_switch = (SwitchState_t)Context->switch_feed();
//    Context->short_switch = (SwitchState_t)Context->switch_feed();
    // 读取内部开关状态
    Context->switch_list.A3QS1_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS1);
    Context->switch_list.A3K17_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3K17);
    Context->switch_list.A3QS2_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS1);
    Context->switch_list.A3QS3_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QS3);
    Context->switch_list.A3QF1_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QF1);
    Context->switch_list.A3QF2_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3QF2);
    Context->switch_list.A3QF3_switch = (SwitchState_t)Context->switch_feed(DI_ID_A3TB3_2);
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
                if ((Context->switch_list.single_switch == SWITCH_OFF) && (Context->switch_list.bus1_switch == SWITCH_OFF))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_SINGLE:
            {
                if ((Context->switch_list.single_switch == SWITCH_ON) && (Context->switch_list.bus1_switch == SWITCH_OFF))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                if ((Context->switch_list.single_switch == SWITCH_OFF) && (Context->switch_list.bus1_switch == SWITCH_ON))
                {
                    return true;
                }
            }
            break;
        case CABINET_STATE_BACKUP:
            {
                if ((Context->switch_list.single_switch == SWITCH_ON) && (Context->switch_list.bus1_switch == SWITCH_ON))
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

    Context->switch_list.manual_switch = (SwitchState_t)Context->switch_feed(DI_ID_LOCALREMOTE);
    // 手动模式
    if (Context->switch_list.manual_switch == SWITCH_ON)
    {
        // 读取旋钮状态
        Context->switch_list.single_switch = (SwitchState_t)Context->switch_feed(SWITCH_ID_SINGLE);
        Context->switch_list.bus1_switch = (SwitchState_t)Context->switch_feed(SWITCH_ID_BUS1);
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
    }
    // 自动模式
    else
    {
        // TODO:接收DCS/监控的单机启动标志
        // 预计是置远程启动标志位
        // 目前先return
        return;
    }

    // 状态没有变化，直接返回
    if (target_state == Context->state)
    {
        Context->target_state = CABINET_STATE_INVALID;
        return;
    }

    // 保存目标状态
    Context->target_state = target_state;

    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                // 柜内开关是否匹配柜体模式
                Cabinet_ReadAllSwitchStatus(Context);
                if (!Cabinet_SwitchIsMatchMode(Context))
                {
                    // 发出警告
                    Cabinet_SetAlarm(Context, ALARM_NONE, "all switch status abnormal");
                    return;
                }
                // 读取电源柜状态
                Context->power_status = CAN_Adapter_GetPowerStatus();
                if (!((Context->power_status.run_state == POWER_SINGLE_RUNNING) ||
                      (Context->power_status.run_state == POWER_PARALLEL)))
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

                // ① 待机->单机
                if (target_state == CABINET_STATE_SINGLE)
                {
                    ExecuteResult_t result;
                    result = Cabinet_ExecuteDegaussProcedure(Context, 5000);
                    if (result == RESULT_SUCCESS)
                    {
                        return;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "power status abnormal");
                    }
                }
                // ② 待机->并机
                else if (target_state == CABINET_STATE_PARALLEL)
                {
                    ExecuteResult_t result;

                    if (Context->power_op_state == POWER_OP_STARTING)
                    {
                        result = Cabinet_StartPowerSupply(Context);
                        if (result == RESULT_WAIT)
                        {
                            return;
                        }
                        else if ((result == RESULT_TIMEOUT) ||
                                 (result == RESULT_COMM_ERROR) ||
                                 (result == RESULT_FAILED))
                        {
                            Cabinet_BackToStandby(Context);
                            return;
                        }
                    }
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_ParallelActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        (void)Cabinet_ExecuteDegaussProcedure(Context, 500);
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
            }
            break;

        case CABINET_STATE_SINGLE:
            {
                // ⑤ 单机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    ExecuteResult_t result;

                    if (Context->power_op_state == POWER_OP_STARTING)
                    {
                        result = Cabinet_StartPowerSupply(Context);
                        if (result == RESULT_WAIT)
                        {
                            return;
                        }
                        else if ((result == RESULT_TIMEOUT) ||
                                 (result == RESULT_COMM_ERROR) ||
                                 (result == RESULT_FAILED))
                        {
                            Cabinet_ErrBranch(Context);
                            return;
                        }
                    }
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_SingleToStandbyActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        result = Cabinet_ExecuteStandbyProcedure(Context);
                        if (result == RESULT_SUCCESS)
                        {
                            (void)Cabinet_ExecuteDegaussProcedure(Context, 500);
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
                    }
                }
                // ⑥ 单机->备机
                else if (Context->target_state == CABINET_STATE_BACKUP)
                {
                    ExecuteResult_t result;
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_SingleToBackupActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        result = Cabinet_ExecuteBackupProcedure(Context);
                        if (result == RESULT_SUCCESS)
                        {
                            Context->switch_list.single_switch = SWITCH_ON;
                            Context->switch_list.bus1_switch = SWITCH_ON;
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
                        }
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
                    }
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                ExecuteResult_t result;
                // ⑦ 并机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    result = Cabinet_ExecuteStandbyProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        // 消磁
                        (void)Cabinet_ExecuteDegaussProcedure(Context, 500);
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
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_ParallelToBackupActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        result = Cabinet_ExecuteBackupProcedure(Context);
                        if (result == RESULT_SUCCESS)
                        {
                            Context->switch_list.single_switch = SWITCH_ON;
                            Context->switch_list.bus1_switch = SWITCH_ON;
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
                        }
                    }
                    else if (result == RESULT_WAIT)
                    {
                        ;
                    }
                    else
                    {
                        // 发出警告
                        Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                        Cabinet_ErrBranch(Context);
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
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_BackupToSingleActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        result = Cabinet_ExecuteSingleProcedure(Context);
                        if (result == RESULT_SUCCESS)
                        {
                            Context->switch_list.single_switch = SWITCH_ON;
                            Context->switch_list.bus1_switch = SWITCH_OFF;
                            Context->state = CABINET_STATE_SINGLE;
                            Context->target_state = CABINET_STATE_INVALID;
                        }
                        else
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                            // 根据状态进行回调
                            Cabinet_ErrBranch(Context);
                        }
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
                // ④ 备机->并机
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    ExecuteResult_t result;
                    // 读取电源状态
                    Context->power_status = CAN_Adapter_GetPowerStatus();

                    result = Cabinet_BackupToParallelActionBranch(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        result = Cabinet_ExecuteParallelProcedure(Context);
                        if (result == RESULT_SUCCESS)
                        {
                            Context->switch_list.single_switch = SWITCH_OFF;
                            Context->switch_list.bus1_switch = SWITCH_ON;
                            Context->state = CABINET_STATE_PARALLEL;
                            Context->target_state = CABINET_STATE_INVALID;
                        }
                        else
                        {
                            // 发出警告
                            Cabinet_SetAlarm(Context, ALARM_NONE, "exectue failed");
                            // 根据状态进行回调
                            Cabinet_ErrBranch(Context);
                        }
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

    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                // ① 待机->单机
                if (Context->target_state == CABINET_STATE_SINGLE)
                {
                    // 判断延时状态
                    if (Context->op_interval_between_thyristor_status != THYRISTOR_OP_IDLE)
                    {
                        demag_result = Cabinet_StartThyristorOpIntervalCount(Context, 500);
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
                            Cabinet_SetAlarm(Context, ALARM_NONE, "delay 500ms failed");
                        }
                    }
                    // 断开消磁
                    Context->switch_control(DO_ID_A3QR3, SWITCH_OFF);
                    demag_result = Context->switch_feed(DI_ID_A3TB3_2);
                    if (demag_result != 0)
                    {
                        Context->demagnetize_state = DEMAGNETIZE_FAILED;
                        return;
                    }
                    else
                    {
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                    }
                    // 延时500ms
                    result = Cabinet_StartThyristorOpIntervalCount(Context, 500);
                    if (result == RESULT_SUCCESS)
                    {
                        ;
                    }
                    else if (result == RESULT_WAIT)
                    {
                        return;
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "delay 500ms failed");
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
                            Cabinet_SetAlarm(Context, ALARM_NONE, "delay 500ms failed");
                        }
                    }

                    result = Cabinet_ExecuteSingleProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_SINGLE;
                        // 重置状态
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Single procedure failed");
                        // 重置
                        Context->target_state = CABINET_STATE_INVALID;
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
                    }
                }
                // ② 待机->并机
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    if (Context->power_op_state == POWER_OP_CLOSING)
                    {
                        result = Cabinet_ClosePowerSupply(Context);
                        if (result == RESULT_WAIT)
                        {
                            return;
                        }
                        else if ((result == RESULT_COMM_ERROR) ||
                            (result == RESULT_TIMEOUT) ||
                            (result == RESULT_FAILED))
                        {
                            Cabinet_BackToStandby(Context);
                        }
                        return;
                    }

                    // 延时500ms
                    result = Cabinet_StartThyristorOpIntervalCount(Context, 500);
                    if (result == RESULT_WAIT)
                    {
                        return;
                    }

                    result = Cabinet_ExecuteParallelProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->switch_list.single_switch = SWITCH_OFF;
                        Context->switch_list.bus1_switch = SWITCH_ON;
                        Context->state = CABINET_STATE_PARALLEL;
                        Context->target_state = CABINET_STATE_INVALID;

                        // 重置状态
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
                    }
                    else
                    {
                        Cabinet_SetAlarm(Context, ALARM_NONE, "Single procedure failed, closing power");
                        (void)Cabinet_ClosePowerSupply(Context);
                        Context->demagnetize_state = DEMAGNETIZE_IDLE;
                        Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
                    }
                }
            }
            break;
        case CABINET_STATE_SINGLE:
            {
                // ⑤ 单机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    Context->switch_list.single_switch = SWITCH_OFF;
                    Context->switch_list.bus1_switch = SWITCH_OFF;
                    Context->state = CABINET_STATE_STANDBY;
                    Context->target_state = CABINET_STATE_INVALID;
                }
            }
            break;
        case CABINET_STATE_PARALLEL:
            {
                // ⑦ 并机->待机
                if (Context->target_state == CABINET_STATE_STANDBY)
                {
                    Context->switch_list.single_switch = SWITCH_OFF;
                    Context->switch_list.bus1_switch = SWITCH_OFF;
                    Context->state = CABINET_STATE_STANDBY;
                    Context->target_state = CABINET_STATE_INVALID;
                }
            }
            break;
        default:
            // 其他状态转换暂未实现
            break;
    }
}

// 检查电源状态用于单机模式
static ExecuteResult_t
Cabinet_SingleActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源已在运行状态
    if (ps->run_state == POWER_SINGLE_RUNNING)
    {
        // 电源已运行，直接进入消磁流程
        return RESULT_SUCCESS;
    }

    // 分支2：电源待机、远控、单机状态
    if ((ps->run_state == POWER_STANDBY) &&
        (ps->ctrl_mode == CONTROL_REMOTE) &&
        (ps->comb_state == POWER_SINGLE))
    {
        return Cabinet_StartPowerSupply(Context);
    }

    // 分支3：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for single mode");

    return RESULT_FAILED;
}

// 检查电源状态用于并机模式
static ExecuteResult_t
Cabinet_ParallelActionBranch(CabinetContext_t* Context)
{
	ExecuteResult_t result;
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源待机、远控、单机或者待机、就地
    if (((ps->run_state == POWER_STANDBY) &&
        (ps->ctrl_mode == CONTROL_REMOTE)&&
        (ps->comb_state == POWER_SINGLE)) ||
        ((ps->run_state == POWER_STANDBY)&&
        (ps->ctrl_mode == CONTROL_LOCAL)))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：电源待机、远控、并机状态
    if ((ps->run_state == POWER_STANDBY) &&
        (ps->ctrl_mode == CONTROL_REMOTE) &&
        (ps->comb_state == POWER_PARALLEL))
    {
        return Cabinet_StartPowerSupply(Context);
    }

    // 分支3：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for parallel mode");
    // 判断母线OK且电源数量大于允许值
    // TODO:添加电压读取函数、context中加入函数指针
    // TODO:添加电源数量解析，放入can_adapter.c
    if (0)
    {
        result = Cabinet_ExecuteParallelProcedure(Context);
        if (result == RESULT_SUCCESS)
        {
            Context->switch_list.single_switch = SWITCH_OFF;
            Context->switch_list.bus1_switch = SWITCH_ON;
            Context->state = CABINET_STATE_PARALLEL;
            Context->target_state = CABINET_STATE_INVALID;
            return RESULT_WAIT;
        }
    }
    else
    {
        return RESULT_FAILED;
    }

    return RESULT_FAILED;
}

// 备机->单机分支逻辑
static ExecuteResult_t
Cabinet_BackupToSingleActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源运行、并机
    if ((ps->run_state == POWER_PARALLEL_RUNNING)&&
        (ps->comb_state == POWER_PARALLEL))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Power supply status abnormal");

    return RESULT_FAILED;
}

// 检查电源状态用于并机模式
static ExecuteResult_t
Cabinet_BackupToParallelActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源运行、并机
    if ((ps->run_state == POWER_PARALLEL_RUNNING)&&
        (ps->comb_state == POWER_PARALLEL))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for parallel mode");

    return RESULT_FAILED;
}

static ExecuteResult_t
Cabinet_ParallelToBackupActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源运行、并机
    if ((ps->run_state == POWER_PARALLEL_RUNNING)&&
        (ps->comb_state == POWER_PARALLEL))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for parallel mode");

    return RESULT_FAILED;
}
// 检查电源状态用于单机模式
static ExecuteResult_t
Cabinet_SingleToStandbyActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源运行、并网或者电源运行、单机、就地状态
    if (((ps->run_state == POWER_PARALLEL_RUNNING) &&
         (ps->comb_state == POWER_PARALLEL)) ||
        ((ps->run_state == POWER_SINGLE_RUNNING) &&
         (ps->comb_state == POWER_SINGLE) &&
         (ps->ctrl_mode == CONTROL_LOCAL)))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：电源运行、远控、单机状态
    if((ps->run_state == POWER_SINGLE_RUNNING) &&
       (ps->comb_state == POWER_SINGLE) &&
       (ps->ctrl_mode == CONTROL_REMOTE))
    {
        return Cabinet_ClosePowerSupply(Context);
    }

    // 分支3：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for single mode");

    return RESULT_FAILED;
}

static ExecuteResult_t
Cabinet_SingleToBackupActionBranch(CabinetContext_t* Context)
{
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源运行、并网
    if ((ps->run_state == POWER_PARALLEL_RUNNING) &&
        (ps->comb_state == POWER_PARALLEL))
    {
        return RESULT_SUCCESS;
    }

    // 分支2：其他状态
    Cabinet_SetAlarm(Context, ALARM_STATE_ROLLBACK, "Invalid power status for single mode");

    return RESULT_FAILED;
}
// 返回到待机状态
static ExecuteResult_t
Cabinet_BackToStandby(CabinetContext_t* Context)
{
    // 断开所有开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QR2, SWITCH_OFF);
    }

    Context->state = CABINET_STATE_STANDBY;
    Context->switch_list.single_switch = SWITCH_OFF;
    Context->switch_list.bus1_switch = SWITCH_OFF;
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
        (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QR2, SWITCH_OFF);
    }

    Context->state = CABINET_STATE_SINGLE;
    Context->switch_list.single_switch = SWITCH_ON;
    Context->switch_list.bus1_switch = SWITCH_OFF;
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
        (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QR2, SWITCH_ON);
    }

    Context->state = CABINET_STATE_PARALLEL;
    Context->switch_list.single_switch = SWITCH_OFF;
    Context->switch_list.bus1_switch = SWITCH_ON;
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
        (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
        (void)Context->switch_control(SWITCH_ID_A3QR2, SWITCH_ON);
    }

    Context->state = CABINET_STATE_BACKUP;
    Context->switch_list.single_switch = SWITCH_ON;
    Context->switch_list.bus1_switch = SWITCH_ON;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

static ExecuteResult_t Cabinet_ErrBranch(CabinetContext_t* Context)
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
    // 使用50ms标志进行计时
    if (hal_timer_get_50ms_flag())
    {
        hal_timer_set_50ms_flag(false);

        if (Context->demagnetize_counter > 0)
        {
            Context->demagnetize_counter--;

            // TODO:可以发出警告，正在消磁中
            if (Context->demagnetize_counter == 0)
            {
                // 消磁完成（超时）
                Context->demagnetize_state = DEMAGNETIZE_DONE;
            }
        }
    }
}

// 启动电源
static ExecuteResult_t
Cabinet_StartPowerSupply(CabinetContext_t* Context)
{
    // 启动电源启动超时计数
    if (Context->power_op_state == POWER_OP_IDLE)
    {
        // 发送启动电源命令
        if (!CAN_Adapter_SendPowerStart())
        {
            Cabinet_SetAlarm(Context, ALARM_CAN_COMM_ERROR, "Failed to send power start command");
            Cabinet_BackToStandby(Context);
            return RESULT_COMM_ERROR;
        }

        Context->power_op_timeout = POWER_START_TIMEOUT_MS;
        Context->power_op_counter = 0;
        Context->power_op_state = POWER_OP_STARTING;
        return RESULT_WAIT;
    }

    // 检查超时
    if (Context->power_op_counter < Context->power_op_timeout)
    {
        Context->power_op_counter++;

        // 更新电源状态
        Context->power_status = CAN_Adapter_GetPowerStatus();

        // 检查电源是否已启动
        if (Context->power_status.run_state == POWER_PARALLEL_RUNNING)
        {
            Context->power_op_timeout = 0;
            Context->power_op_counter = 0;
            Context->power_op_state = POWER_OP_IDLE;
            return RESULT_SUCCESS;
        }
        return RESULT_WAIT;
    }
    else if (Context->power_op_counter >= Context->power_op_timeout)
    {
        Cabinet_SetAlarm(Context, ALARM_POWER_START_FAILED, "Power start timeout");
        Context->power_op_timeout = 0;
        Context->power_op_counter = 0;
        Context->power_op_state = POWER_OP_IDLE;
        return RESULT_TIMEOUT;
    }

    return RESULT_FAILED;
}

// 关闭电源
static ExecuteResult_t
Cabinet_ClosePowerSupply(CabinetContext_t* Context)
{
    // 启动电源关闭超时计数
    if (Context->power_op_state == POWER_OP_IDLE)
    {
        // 发送关闭电源命令
        if (!CAN_Adapter_SendPowerClose())
        {
            Cabinet_SetAlarm(Context, ALARM_CAN_COMM_ERROR, "Failed to send power close command");
            return RESULT_COMM_ERROR;
        }

        Context->power_op_timeout = POWER_START_TIMEOUT_MS;
        Context->power_op_counter = 0;
        Context->power_op_state = POWER_OP_CLOSING;
        return RESULT_WAIT;
    }

    // 检查超时
    if (Context->power_op_counter < Context->power_op_timeout)
    {
        Context->power_op_counter++;
        // 更新电源状态
        Context->power_status = CAN_Adapter_GetPowerStatus();
        // 检查电源是否已关闭
        if (Context->power_status.run_state == POWER_STANDBY)
        {
            Context->power_op_timeout = 0;
            Context->power_op_counter = 0;
            Context->power_op_state = POWER_OP_IDLE;
            return RESULT_SUCCESS;
        }
        return RESULT_WAIT;
    }
    else if (Context->power_op_counter >= Context->power_op_timeout)
    {
        Cabinet_SetAlarm(Context, ALARM_POWER_START_FAILED, "Power close timeout");
        Context->power_op_timeout = 0;
        Context->power_op_counter = 0;
        Context->power_op_state = POWER_OP_IDLE;
        return RESULT_TIMEOUT;
    }

    return RESULT_FAILED;
}

// 启动晶闸管控制间隔计数
static ExecuteResult_t
Cabinet_StartThyristorOpIntervalCount(CabinetContext_t* Context, uint32_t interval_ms)
{
    // 启动超时计数
    if (Context->op_interval_between_thyristor_status == THYRISTOR_OP_IDLE)
    {
        Context->op_interval_between_thyristor_timeout = interval_ms / TIMER_50MS_INTERVAL;
        Context->op_interval_between_thyristor_counter = 0;
        Context->op_interval_between_thyristor_status = THYRISTOR_OP_WAITTING;
        return RESULT_WAIT;
    }
    else if (Context->op_interval_between_thyristor_status == THYRISTOR_OP_WAITTING)
    {
        // 检查超时
        if (Context->op_interval_between_thyristor_counter < Context->op_interval_between_thyristor_timeout)
        {
            Context->op_interval_between_thyristor_counter++;
            return RESULT_WAIT;
        }
        else
        {
            Context->op_interval_between_thyristor_status = THYRISTOR_OP_DONE;
            return RESULT_SUCCESS;
        }
    }
    else if (Context->op_interval_between_thyristor_status == THYRISTOR_OP_DONE)
    {
        Context->op_interval_between_thyristor_status = THYRISTOR_OP_IDLE;
        return RESULT_SUCCESS;
    }

    return RESULT_FAILED;
}

// 执行消磁流程
static ExecuteResult_t Cabinet_ExecuteDegaussProcedure(CabinetContext_t* Context, uint16_t DelayMs)
{
    // 启动消磁流程
    if (Context->switch_control)
    {
        uint16_t demag_result;
        Context->switch_control(DO_ID_A3QR3, SWITCH_ON);
        demag_result = Context->switch_feed(DI_ID_A3TB3_2);

        if (demag_result == 1)
        {
            // 消磁命令执行成功，开始等待
            Context->demagnetize_state = DEMAGNETIZE_WAITING;
            Context->demagnetize_counter = DelayMs / TIMER_50MS_INTERVAL;
            return RESULT_SUCCESS;
        }
        else
        {
            // 消磁命令执行失败
            Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
            // 根据状态进行回调
            Cabinet_ErrBranch(Context);
            return RESULT_FAILED;
        }
    }
    else
    {
        // 消磁命令执行失败
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
    uint16_t res = 0;

    // 单机时
    if ((Context->switch_list.single_switch == SWITCH_ON) &&
        (Context->switch_list.bus1_switch == SWITCH_OFF))
    {
        // 断开 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QF1);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }

        res = 0;
        // 断开 A3QR1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QR1);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                // 回滚：闭合A3QF1
                (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
                return RESULT_FAILED;
            }
        }
    }
    // 并机时
    else if ((Context->switch_list.single_switch == SWITCH_OFF) &&
        (Context->switch_list.bus1_switch == SWITCH_ON))
    {
        // 断开 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QF2);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }

        res = 0;
        // 断开 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR2, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QR2);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                // 回滚：闭合A3QF2
                (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
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
    uint16_t res = 0;
    uint16_t voltage = 0;

    // 待机时
    if ((Context->switch_list.single_switch == SWITCH_OFF) &&
        (Context->switch_list.bus1_switch == SWITCH_OFF))
    {
        // 闭合 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QF1);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }

        // U<36V
        voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);
        if (voltage >= 36.0)
        {
            return RESULT_FAILED;
        }

        res = 0;
        // 闭合 A3QR1
        if (Context->switch_control)
        {
            uint16_t voltage;
            Context->switch_control(SWITCH_ID_A3QR1, SWITCH_ON);
            voltage = Context->voltage_read(VOLTAGE_U7_PHASE_A);
            if (voltage < 50.0)
            {
                return RESULT_FAILED;
            }
        }
    }
    // 备用时
    else if ((Context->switch_list.single_switch == SWITCH_ON) &&
        (Context->switch_list.bus1_switch == SWITCH_ON))
    {
        // 断开 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR2, SWITCH_OFF);
        }
        res = 0;
        // 断开 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QF2);
            if (res != 0)
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
    uint16_t res = 0;

    // 待机时
    if ((Context->switch_list.single_switch == SWITCH_OFF) &&
        (Context->switch_list.bus1_switch == SWITCH_OFF))
    {
        // 闭合 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QF2);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }

        // 闭合 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR2, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QR2);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QR2");
                // 回滚：断开A3QF2
                (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
                return RESULT_FAILED;
            }
        }
    }

    // 备用时
    if ((Context->switch_list.single_switch == SWITCH_ON) &&
        (Context->switch_list.bus1_switch == SWITCH_ON))
    {
        // 断开 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QF1);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }

        res = 0;
        // 断开 A3QR1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
            res = Context->switch_feed(SWITCH_ID_A3QR1);
            if (res != 0)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                // 回滚：闭合A3QF1
                (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
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
    uint16_t res = 0;

    // 单机时
    if ((Context->switch_list.single_switch == SWITCH_ON) &&
        (Context->switch_list.bus1_switch == SWITCH_OFF))
    {
        // 闭合 A3QF2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QF2);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF2");
                return RESULT_FAILED;
            }
        }

        // 闭合 A3QR2
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR2, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QR2);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QR2");
                // 回滚：断开A3QF2
                (void)Context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
                return RESULT_FAILED;
            }
        }
    }

    // 并机时
    if ((Context->switch_list.single_switch == SWITCH_OFF) &&
        (Context->switch_list.bus1_switch == SWITCH_ON))
    {
        // 闭合 A3QF1
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QF1);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                return RESULT_FAILED;
            }
        }

        res = 0;
        // 闭合 A3QR1（并断开相应互锁回路）
        if (Context->switch_control)
        {
            Context->switch_control(SWITCH_ID_A3QR1, SWITCH_ON);
            res = Context->switch_feed(SWITCH_ID_A3QR1);
            if (res != 1)
            {
                Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QF1");
                // 回滚：断开A3QF1
                (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
                return RESULT_FAILED;
            }
        }
    }
    return RESULT_SUCCESS;
}
static bool
Cabinet_HasCycle(CabinetContext_t* Context)
{
    uint16_t res = 0;
    res = Context->switch_feed(SWITCH_ID_A3QF1);
    res |= Context->switch_feed(SWITCH_ID_A3QF2);
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
    return false;
}



