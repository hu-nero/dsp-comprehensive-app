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


static ExecuteResult_t Cabinet_SingleActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ParallelActionBranch(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_StartPowerSupply(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteSingleProcedure(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_ExecuteParallelProcedure(CabinetContext_t* Context);
static void Cabinet_SetAlarm(CabinetContext_t* Context, AlarmType_t Type, const char* Message);
static void Cabinet_UpdateCabinetState(CabinetContext_t* Context);
static ExecuteResult_t Cabinet_BackToStandby(CabinetContext_t* Context);
static void Cabinet_ContinueAction(CabinetContext_t* Context);
static void Cabinet_ProcessDemagnetizeWait(CabinetContext_t* Context);

// 区段柜控制初始化
bool
Cabinet_Init(CabinetContext_t* Context,
             SwitchControlFunc_t switch_ctrl,
             SwitchFeedFunc_t  switch_feed,
             AlarmCallback_t alarm_cb)
{
    if ((!Context) || (!switch_ctrl) || (!switch_feed) || (!alarm_cb))
    {
        return false;
    }

    CAN_Adapter_Init();
    // 初始化上下文
    memset(Context, 0, sizeof(CabinetContext_t));

    // 保存回调函数
    Context->switch_control = switch_ctrl;
    Context->switch_feed = switch_feed;
    Context->alarm_callback = alarm_cb;

    // 设置默认状态
    Context->state = CABINET_STATE_STANDBY;
    Context->target_state = CABINET_STATE_INVALID;
    Context->mode = CABINET_MODE_MANUAL;
    Context->single_switch = SWITCH_OFF;
    Context->bus1_switch = SWITCH_OFF;
    Context->short_switch = SWITCH_OFF;
    Context->emergency_stop = false;

    // 初始化消磁相关状态
    Context->demagnetize_state = DEMAGNETIZE_IDLE;
    Context->demagnetize_counter = 0;

    // 初始化电源状态
    Context->power_status.run_state = POWER_UNKNOWN;
    Context->power_status.comb_state = POWER_COMB_UNKNOWN;
    Context->power_status.ctrl_mode = CONTROL_MODE_UNKNOWN;
    Context->power_status.status_word = 0;
    Context->power_status.is_using = 0;

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

                Cabinet_BackToStandby(Context);
            }
            break;
        default:break;
    }

}

// 设置面板按钮状态
ExecuteResult_t
Cabinet_SetSwitches(CabinetContext_t* Context,
                    SwitchState_t single_state,
                    SwitchState_t bus1_state,
                    SwitchState_t short_state,
                    bool Emergency)
{
    if (!Context)
    {
        return RESULT_INVALID_STATE;
    }

    Context->single_switch = single_state;
    Context->bus1_switch = bus1_state;
    Context->short_switch = short_state;
    Context->emergency_stop = Emergency;

    return RESULT_SUCCESS;
}

// 设置工作模式
ExecuteResult_t
Cabinet_SetMode(CabinetContext_t* Context, CabinetMode_t Mode)
{
    if (!Context)
    {
        return RESULT_INVALID_STATE;
    }

    if (Mode > CABINET_MODE_AUTO)
    {
        return RESULT_INVALID_STATE;
    }

    Context->mode = Mode;

    // TODO:自动模式
    if (Mode == CABINET_MODE_AUTO)
    {
        Cabinet_SetAlarm(Context, ALARM_NONE, "Auto mode not yet implemented");
    }

    return RESULT_SUCCESS;
}

// 获取当前状态
CabinetState_t
Cabinet_GetState(const CabinetContext_t* Context)
{
    return Context ? Context->state : CABINET_STATE_STANDBY;
}

// 获取当前模式
CabinetMode_t
Cabinet_GetMode(const CabinetContext_t* Context)
{
    return Context ? Context->mode : CABINET_MODE_MANUAL;
}

// 更新区段柜状态
static void
Cabinet_UpdateCabinetState(CabinetContext_t* Context)
{
    CabinetState_t target_state;

    // 读取旋钮状态
    Context->single_switch = (SwitchState_t)Context->switch_feed(SWITCH_ID_SINGLE);
    Context->bus1_switch = (SwitchState_t)Context->switch_feed(SWITCH_ID_BUS1);
    // 根据开关状态确定目标状态
    if ((Context->single_switch == SWITCH_OFF) && (Context->bus1_switch == SWITCH_OFF))
    {
        target_state = CABINET_STATE_STANDBY;
    }
    else if ((Context->single_switch == SWITCH_ON) && (Context->bus1_switch == SWITCH_OFF))
    {
        target_state = CABINET_STATE_SINGLE;
    }
    else if ((Context->single_switch == SWITCH_OFF) && (Context->bus1_switch == SWITCH_ON))
    {
        target_state = CABINET_STATE_PARALLEL;
    }
    else
    {
        target_state = CABINET_STATE_BACKUP;
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
            if (target_state == CABINET_STATE_SINGLE)
            {
                ExecuteResult_t result;
                // 读取电源状态
                Context->power_status = CAN_Adapter_GetPowerStatus();

                result = Cabinet_SingleActionBranch(Context);
                if (result == RESULT_SUCCESS)
                {
                    result = Cabinet_ExecuteSingleProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_SINGLE;
                    }
                }
            }
            else if (target_state == CABINET_STATE_PARALLEL)
            {
                ExecuteResult_t result;
                // 读取电源状态
                Context->power_status = CAN_Adapter_GetPowerStatus();
                result = Cabinet_ParallelActionBranch(Context);
                if (result == RESULT_SUCCESS)
                {
                    result = Cabinet_ExecuteParallelProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_PARALLEL;
                    }
                }
            }
            break;

        case CABINET_STATE_SINGLE:
            /* 返回待机状态 */
            /*if (target_state == CABINET_STATE_STANDBY)*/
            /*{*/
                /*[> 断开单机相关开关 <]*/
                /*if (Context->switch_control)*/
                /*{*/
                    /*(void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);*/
                    /*(void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);*/
                /*}*/
                /*Context->state = CABINET_STATE_STANDBY;*/
            /*}*/
            break;

        default:
            /* 其他状态转换暂未实现 */
            break;
    }
}

// 返回到待机状态
static ExecuteResult_t
Cabinet_BackToStandby(CabinetContext_t* Context)
{
    // 断开单机相关开关
    if (Context->switch_control)
    {
        (void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
        (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
    }

    Context->state = CABINET_STATE_STANDBY;
    Context->single_switch = SWITCH_OFF;
    Context->bus1_switch = SWITCH_OFF;
    Context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

// 延续区段柜状态
static void
Cabinet_ContinueAction(CabinetContext_t* Context)
{
    ExecuteResult_t result;
    // 状态转换处理
    switch (Context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                if (Context->target_state == CABINET_STATE_SINGLE)
                {
                    result = Cabinet_ExecuteSingleProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_SINGLE;
                        Context->single_switch = SWITCH_ON;
                        Context->bus1_switch = SWITCH_OFF;
                    }
                    else
                    {
                        Context->single_switch = SWITCH_OFF;
                        Context->bus1_switch = SWITCH_OFF;
                    }
                    Context->target_state = CABINET_STATE_INVALID;
                }
                else if (Context->target_state == CABINET_STATE_PARALLEL)
                {
                    result = Cabinet_ExecuteParallelProcedure(Context);
                    if (result == RESULT_SUCCESS)
                    {
                        Context->state = CABINET_STATE_PARALLEL;
                        Context->single_switch = SWITCH_OFF;
                        Context->bus1_switch = SWITCH_ON;
                    }
                    else
                    {
                        Context->single_switch = SWITCH_OFF;
                        Context->bus1_switch = SWITCH_OFF;

                    }
                    Context->target_state = CABINET_STATE_INVALID;
                }
            }
            break;

        case CABINET_STATE_SINGLE:
            /* 返回待机状态 */
            /*if (target_state == CABINET_STATE_STANDBY)*/
            /*{*/
                /*[> 断开单机相关开关 <]*/
                /*if (Context->switch_control)*/
                /*{*/
                    /*(void)Context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);*/
                    /*(void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);*/
                /*}*/
                /*Context->state = CABINET_STATE_STANDBY;*/
            /*}*/
            break;

        default:
            /* 其他状态转换暂未实现 */
            break;
    }
}

// 检查电源状态用于单机模式
static ExecuteResult_t
Cabinet_SingleActionBranch(CabinetContext_t* Context)
{
	ExecuteResult_t result;
    PowerStatus_t* ps = &Context->power_status;

    // 分支1：电源已在运行状态
    if (ps->run_state == POWER_RUNNING)
    {
        // 启动消磁流程
        if (Context->switch_control)
        {
            uint16_t demag_result;
            Context->switch_control(SWITCH_ID_SHORT, SWITCH_ON);
            demag_result = Context->switch_feed(SWITCH_ID_SHORT);

            if (demag_result == 1)
            {
                // 消磁命令执行成功，开始等待
                Context->demagnetize_state = DEMAGNETIZE_WAITING;
                Context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL;
                return RESULT_WAIT;
            }
            else
            {
                // 消磁命令执行失败
                Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
                Context->single_switch = SWITCH_OFF;
                Context->bus1_switch = SWITCH_OFF;
                Context->target_state = CABINET_STATE_INVALID;
                return RESULT_FAILED;
            }
        }
        else
        {
            // 没有消磁函数，直接执行单机流程
            result = Cabinet_ExecuteSingleProcedure(Context);
            if (result == RESULT_SUCCESS)
            {
                Context->state = CABINET_STATE_SINGLE;
            }
            return result;
        }
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

    // 回退到待机状态
    Context->single_switch = SWITCH_OFF;
    Context->bus1_switch = SWITCH_OFF;
    Context->target_state = CABINET_STATE_INVALID;

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
        // 启动消磁流程
        if (Context->switch_control)
        {
            uint16_t demag_result;
            Context->switch_control(SWITCH_ID_SHORT, SWITCH_ON);
            demag_result = Context->switch_feed(SWITCH_ID_SHORT);
            if (demag_result == 1)
            {
                // 消磁命令执行成功，开始等待
                Context->demagnetize_state = DEMAGNETIZE_WAITING;
                Context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL;
                return RESULT_WAIT;
            }
            else
            {
                // 消磁命令执行失败
                Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
                Context->single_switch = SWITCH_OFF;
                Context->bus1_switch = SWITCH_OFF;
                Context->target_state = CABINET_STATE_INVALID;
                return RESULT_FAILED;
            }
        }
        else
        {
            // 没有消磁函数，直接执行并机流程
            result = Cabinet_ExecuteParallelProcedure(Context);
            if (result == RESULT_SUCCESS)
            {
                Context->state = CABINET_STATE_PARALLEL;
            }
            return result;
        }
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

    // 回退到待机状态
    Context->single_switch = SWITCH_OFF;
    Context->bus1_switch = SWITCH_OFF;
    Context->target_state = CABINET_STATE_INVALID;

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
    static uint32_t power_start_counter = 0;

    // 启动电源启动超时计数
    if (Context->power_start_timeout == 0)
    {
        // 发送启动电源命令
        if (!CAN_Adapter_SendPowerStart())
        {
            Cabinet_SetAlarm(Context, ALARM_CAN_COMM_ERROR, "Failed to send power start command");
            Context->single_switch = SWITCH_OFF;
            Context->bus1_switch = SWITCH_OFF;
            Context->target_state = CABINET_STATE_INVALID;
            return RESULT_COMM_ERROR;
        }

        Context->power_start_timeout = POWER_START_TIMEOUT_MS;
        power_start_counter = 0;
        return RESULT_WAIT;
    }

    // 检查超时
    if (power_start_counter < Context->power_start_timeout)
    {
        power_start_counter++;

        // 更新电源状态
        Context->power_status = CAN_Adapter_GetPowerStatus();

        // 检查电源是否已启动
        if (Context->power_status.run_state == POWER_RUNNING)
        {
            Context->power_start_timeout = 0;
            power_start_counter = 0;

            // 启动消磁流程
            if (Context->switch_control)
            {
            	uint16_t demag_result;
                Context->switch_control(SWITCH_ID_SHORT, SWITCH_ON);
                demag_result = Context->switch_feed(SWITCH_ID_SHORT);
                if (demag_result == RESULT_SUCCESS)
                {
                    // 消磁命令执行成功，开始等待
                    Context->demagnetize_state = DEMAGNETIZE_WAITING;
                    Context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL;
                    return RESULT_WAIT;
                }
                else
                {
                    // 消磁命令执行失败
                    Cabinet_SetAlarm(Context, ALARM_NONE, "Demagnetize command failed");
                    Context->single_switch = SWITCH_OFF;
                    Context->bus1_switch = SWITCH_OFF;
                    Context->target_state = CABINET_STATE_INVALID;
                    return RESULT_FAILED;
                }
            }
            else
            {
                // 没有消磁函数，直接执行单机流程
                ExecuteResult_t result = Cabinet_ExecuteSingleProcedure(Context);
                if (result == RESULT_SUCCESS)
                {
                    Context->state = CABINET_STATE_SINGLE;
                }
                return result;
            }
        }
        else if (power_start_counter >= Context->power_start_timeout)
        {
            Cabinet_SetAlarm(Context, ALARM_POWER_START_FAILED, "Power start timeout");
            Context->single_switch = SWITCH_OFF;
            Context->bus1_switch = SWITCH_OFF;
            Context->target_state = CABINET_STATE_INVALID;
            Context->power_start_timeout = 0;
            power_start_counter = 0;
            return RESULT_TIMEOUT;
        }
    }

    return RESULT_WAIT;
}

// 执行单机流程
static ExecuteResult_t
Cabinet_ExecuteSingleProcedure(CabinetContext_t* Context)
{
    uint16_t res = 0;

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
            Cabinet_SetAlarm(Context, ALARM_NONE, "Failed to close A3QR1");
            // 回滚：断开A3QR1
            (void)Context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
            return RESULT_FAILED;
        }
    }

    return RESULT_SUCCESS;
}

// 执行并机流程
static ExecuteResult_t
Cabinet_ExecuteParallelProcedure(CabinetContext_t* Context)
{
    uint16_t res = 0;

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
            // 回滚：断开A3QR2
            (void)Context->switch_control(SWITCH_ID_A3QR2, SWITCH_OFF);
            return RESULT_FAILED;
        }
    }

    return RESULT_SUCCESS;
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
