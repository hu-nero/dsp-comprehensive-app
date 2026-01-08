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

/* 内部常量定义 */
#define SWITCH_ID_A3QF1         0x01    /* A3QF1开关 */
#define SWITCH_ID_A3QR1         0x02    /* A3QR1开关 */
#define SWITCH_ID_A3QF2         0x03    /* A3QF1开关 */
#define SWITCH_ID_A3QR2         0x04    /* A3QR1开关 */
#define SWITCH_ID_SINGLE        0x05    /* 单机开关 */
#define SWITCH_ID_BUS1          0x06    /* 母线1开关 */
#define SWITCH_ID_SHORT         0x07    /* 短接开关 */

#define POWER_START_TIMEOUT_MS  1000    /* 电源启动超时（1秒） */
#define TIMER_50MS_INTERVAL     50      /* 50ms计时单位 */
#define DEMAGNETIZE_TIMEOUT_MS  10000   /* 消磁超时（10秒），50ms*200 */

/* 内部函数声明 */
static ExecuteResult_t Cabinet_SingleActionBranch(CabinetContext_t* context);
static ExecuteResult_t Cabinet_ParallelActionBranch(CabinetContext_t* context);
static ExecuteResult_t Cabinet_StartPowerSupply(CabinetContext_t* context);
static ExecuteResult_t Cabinet_ExecuteSingleProcedure(CabinetContext_t* context);
static ExecuteResult_t Cabinet_ExecuteParallelProcedure(CabinetContext_t* context);
static void Cabinet_SetAlarm(CabinetContext_t* context, AlarmType_t type, const char* message);
static void Cabinet_UpdateCabinetState(CabinetContext_t* context);
static ExecuteResult_t Cabinet_BackToStandby(CabinetContext_t* context);
static void Cabinet_ContinueAction(CabinetContext_t* context);
static void Cabinet_ProcessDemagnetizeWait(CabinetContext_t* context);

/* 区段柜控制初始化 */
bool
Cabinet_Init(CabinetContext_t* context,
             SwitchControlFunc_t switch_ctrl,
             DemagnetizeFunc_t demagnetize,
             AlarmCallback_t alarm_cb)
{
    if ((!context) || (!switch_ctrl) || (!demagnetize) || (!alarm_cb))
    {
        return false;
    }

    CAN_Adapter_Init();
    /* 初始化上下文 */
    memset(context, 0, sizeof(CabinetContext_t));

    /* 保存回调函数 */
    context->switch_control = switch_ctrl;
    context->demagnetize = demagnetize;
    context->alarm_callback = alarm_cb;

    /* 设置默认状态 */
    context->state = CABINET_STATE_STANDBY;
    context->target_state = CABINET_STATE_INVALID;
    context->mode = CABINET_MODE_MANUAL;
    context->single_switch = SWITCH_OFF;
    context->bus1_switch = SWITCH_OFF;
    context->short_switch = SWITCH_OFF;
    context->emergency_stop = false;

    /* 初始化消磁相关状态 */
    context->demagnetize_state = DEMAGNETIZE_IDLE;
    context->demagnetize_counter = 0;

    /* 初始化电源状态 */
    context->power_status.run_state = POWER_UNKNOWN;
    context->power_status.comb_state = POWER_COMB_UNKNOWN;
    context->power_status.ctrl_mode = CONTROL_MODE_UNKNOWN;
    context->power_status.status_word = 0;
    context->power_status.is_using = 0;

    return true;
}

// 周期处理函数
void
Cabinet_Process(CabinetContext_t* context)
{
    if (!context)
    {
        return;
    }

    switch (context->demagnetize_state)
    {
        case DEMAGNETIZE_IDLE:
            {
                // 根据当前状态处理状态转换
                Cabinet_UpdateCabinetState(context);
            }
            break;
        case DEMAGNETIZE_WAITING:
            {
                Cabinet_ProcessDemagnetizeWait(context);
            }
            return; /* 消磁等待期间不进行状态转换 */
        case DEMAGNETIZE_DONE:
            {
                // 根据记录状态处理状态转换
                Cabinet_ContinueAction(context);
            }
            break;
        case DEMAGNETIZE_FAILED:
            {
                /* 发出警告，消磁失败，拨回开关 */
                Cabinet_SetAlarm(context, ALARM_NONE, "Demagnetize failed");

                Cabinet_BackToStandby(context);
            }
            break;
        default:break;
    }

}

/* 设置面板按钮状态 */
ExecuteResult_t
Cabinet_SetSwitches(CabinetContext_t* context,
                    SwitchState_t single_state,
                    SwitchState_t bus1_state,
                    SwitchState_t short_state,
                    bool emergency)
{
    if (!context)
    {
        return RESULT_INVALID_STATE;
    }

    context->single_switch = single_state;
    context->bus1_switch = bus1_state;
    context->short_switch = short_state;
    context->emergency_stop = emergency;

    return RESULT_SUCCESS;
}

/* 设置工作模式 */
ExecuteResult_t
Cabinet_SetMode(CabinetContext_t* context, CabinetMode_t mode)
{
    if (!context)
    {
        return RESULT_INVALID_STATE;
    }

    if (mode > CABINET_MODE_AUTO)
    {
        return RESULT_INVALID_STATE;
    }

    context->mode = mode;

    // TODO:自动模式
    if (mode == CABINET_MODE_AUTO)
    {
        Cabinet_SetAlarm(context, ALARM_NONE, "Auto mode not yet implemented");
    }

    return RESULT_SUCCESS;
}

/* 更新电源状态 */
void
Cabinet_UpdatePowerStatus(CabinetContext_t* context, const PowerStatus_t* power_status)
{
    if ((!context) || (!power_status))
    {
        return;
    }

    memcpy(&context->power_status, power_status, sizeof(PowerStatus_t));
}

/* 获取当前状态 */
CabinetState_t
Cabinet_GetState(const CabinetContext_t* context)
{
    return context ? context->state : CABINET_STATE_STANDBY;
}

/* 获取当前模式 */
CabinetMode_t
Cabinet_GetMode(const CabinetContext_t* context)
{
    return context ? context->mode : CABINET_MODE_MANUAL;
}

/* ==================== 内部函数实现 ==================== */

/* 更新区段柜状态 */
static void
Cabinet_UpdateCabinetState(CabinetContext_t* context)
{
    CabinetState_t target_state;

    /* 根据开关状态确定目标状态 */
    if ((context->single_switch == SWITCH_OFF) && (context->bus1_switch == SWITCH_OFF))
    {
        target_state = CABINET_STATE_STANDBY;
    }
    else if ((context->single_switch == SWITCH_ON) && (context->bus1_switch == SWITCH_OFF))
    {
        target_state = CABINET_STATE_SINGLE;
    }
    else if ((context->single_switch == SWITCH_OFF) && (context->bus1_switch == SWITCH_ON))
    {
        target_state = CABINET_STATE_PARALLEL;
    }
    else /* both ON */
    {
        target_state = CABINET_STATE_BACKUP;
    }

    /* 状态没有变化，直接返回 */
    if (target_state == context->state)
    {
        context->target_state = CABINET_STATE_INVALID;
        return;
    }

    /* 保存目标状态 */
    context->target_state = target_state;

    /* 状态转换处理 */
    switch (context->state)
    {
        case CABINET_STATE_STANDBY:
            if (target_state == CABINET_STATE_SINGLE)
            {
                ExecuteResult_t result;
                // 读取电源状态
                context->power_status = CAN_Adapter_GetPowerStatus();

                result = Cabinet_SingleActionBranch(context);
                if (result == RESULT_SUCCESS)
                {
                    result = Cabinet_ExecuteSingleProcedure(context);
                    if (result == RESULT_SUCCESS)
                    {
                        context->state = CABINET_STATE_SINGLE;
                    }
                }
            }
            else if (target_state == CABINET_STATE_PARALLEL)
            {
                ExecuteResult_t result;
                // 读取电源状态
                context->power_status = CAN_Adapter_GetPowerStatus();
                result = Cabinet_ParallelActionBranch(context);
                if (result == RESULT_SUCCESS)
                {
                    result = Cabinet_ExecuteParallelProcedure(context);
                    if (result == RESULT_SUCCESS)
                    {
                        context->state = CABINET_STATE_PARALLEL;
                    }
                }
            }
            /* 其他状态转换暂未实现 */
            break;

        case CABINET_STATE_SINGLE:
            /* 返回待机状态 */
            /*if (target_state == CABINET_STATE_STANDBY)*/
            /*{*/
                /*[> 断开单机相关开关 <]*/
                /*if (context->switch_control)*/
                /*{*/
                    /*(void)context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);*/
                    /*(void)context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);*/
                /*}*/
                /*context->state = CABINET_STATE_STANDBY;*/
            /*}*/
            break;

        default:
            /* 其他状态转换暂未实现 */
            break;
    }
}

/* 返回到待机状态 */
static ExecuteResult_t
Cabinet_BackToStandby(CabinetContext_t* context)
{
    /* 断开单机相关开关 */
    if (context->switch_control)
    {
        (void)context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
        (void)context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);
    }

    context->state = CABINET_STATE_STANDBY;
    context->single_switch = SWITCH_OFF;
    context->bus1_switch = SWITCH_OFF;
    context->target_state = CABINET_STATE_INVALID;

    return RESULT_SUCCESS;
}

/* 延续区段柜状态 */
static void
Cabinet_ContinueAction(CabinetContext_t* context)
{
    ExecuteResult_t result;
    /* 状态转换处理 */
    switch (context->state)
    {
        case CABINET_STATE_STANDBY:
            {
                if (context->target_state == CABINET_STATE_SINGLE)
                {
                    result = Cabinet_ExecuteSingleProcedure(context);
                    if (result == RESULT_SUCCESS)
                    {
                        context->state = CABINET_STATE_SINGLE;
                        context->single_switch = SWITCH_ON;
                        context->bus1_switch = SWITCH_OFF;
                    }
                    else
                    {
                        context->single_switch = SWITCH_OFF;
                        context->bus1_switch = SWITCH_OFF;

                    }
                    context->target_state = CABINET_STATE_INVALID;
                }
                else if (context->target_state == CABINET_STATE_PARALLEL)
                {
                    result = Cabinet_ExecuteParallelProcedure(context);
                    if (result == RESULT_SUCCESS)
                    {
                        context->state = CABINET_STATE_PARALLEL;
                        context->single_switch = SWITCH_OFF;
                        context->bus1_switch = SWITCH_ON;
                    }
                    else
                    {
                        context->single_switch = SWITCH_OFF;
                        context->bus1_switch = SWITCH_OFF;

                    }
                    context->target_state = CABINET_STATE_INVALID;
                }
            }
            break;

        case CABINET_STATE_SINGLE:
            /* 返回待机状态 */
            /*if (target_state == CABINET_STATE_STANDBY)*/
            /*{*/
                /*[> 断开单机相关开关 <]*/
                /*if (context->switch_control)*/
                /*{*/
                    /*(void)context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);*/
                    /*(void)context->switch_control(SWITCH_ID_A3QR1, SWITCH_OFF);*/
                /*}*/
                /*context->state = CABINET_STATE_STANDBY;*/
            /*}*/
            break;

        default:
            /* 其他状态转换暂未实现 */
            break;
    }
}

/* 检查电源状态用于单机模式 */
static ExecuteResult_t
Cabinet_SingleActionBranch(CabinetContext_t* context)
{
    PowerStatus_t* ps = &context->power_status;  // MODIFIED: 使用指针简化访问

    /* 分支1：电源已在运行状态 */
    if (ps->run_state == POWER_RUNNING)  // MODIFIED: 修正指针访问语法
    {
        /* 启动消磁流程 */
        if (context->demagnetize)
        {
            ExecuteResult_t demag_result = context->demagnetize();

            if (demag_result == RESULT_SUCCESS)
            {
                /* 消磁命令执行成功，开始等待 */
                context->demagnetize_state = DEMAGNETIZE_WAITING;
                context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL; /* 转换为50ms计数 */  // MODIFIED: 使用明确单位
                return RESULT_WAIT; // 等待消磁
            }
            else
            {
                /* 消磁命令执行失败 */  // MODIFIED: 完善失败处理
                Cabinet_SetAlarm(context, ALARM_NONE, "Demagnetize command failed");
                context->single_switch = SWITCH_OFF;
                context->bus1_switch = SWITCH_OFF;
                context->target_state = CABINET_STATE_INVALID;
                return demag_result;
            }
        }
        else
        {
            /* 没有消磁函数，直接执行单机流程 */  // MODIFIED: 处理无消磁函数情况
            ExecuteResult_t result = Cabinet_ExecuteSingleProcedure(context);
            if (result == RESULT_SUCCESS)
            {
                context->state = CABINET_STATE_SINGLE;
            }
            return result;
        }
    }

    /* 分支2：电源待机、远控、单机状态 */
    if ((ps->run_state == POWER_STANDBY) &&  // MODIFIED: 修正指针访问语法
        (ps->ctrl_mode == CONTROL_REMOTE) &&
        (ps->comb_state == POWER_SINGLE))
    {
        return Cabinet_StartPowerSupply(context);
    }

    /* 分支3：其他状态 */
    Cabinet_SetAlarm(context, ALARM_STATE_ROLLBACK, "Invalid power status for single mode");

    /* 回退到待机状态 */
    context->single_switch = SWITCH_OFF;
    context->bus1_switch = SWITCH_OFF;
    context->target_state = CABINET_STATE_INVALID;  // ADDED: 清理目标状态

    return RESULT_FAILED;
}

/* 检查电源状态用于并机模式 */
static ExecuteResult_t
Cabinet_ParallelActionBranch(CabinetContext_t* context)
{
    PowerStatus_t* ps = &context->power_status;

    // 分支1：电源待机、远控、单机或者待机、就地
    if (((ps->run_state == POWER_STANDBY) &&
        (ps->ctrl_mode == CONTROL_REMOTE)&&
        (ps->comb_state == POWER_SINGLE)) ||
        ((ps->run_state == POWER_STANDBY)&&
        (ps->ctrl_mode == CONTROL_LOCAL)))
    {
        /* 启动消磁流程 */
        if (context->demagnetize)
        {
            ExecuteResult_t demag_result = context->demagnetize();

            if (demag_result == RESULT_SUCCESS)
            {
                /* 消磁命令执行成功，开始等待 */
                context->demagnetize_state = DEMAGNETIZE_WAITING;
                context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL; /* 转换为50ms计数 */  // MODIFIED: 使用明确单位
                return RESULT_WAIT; // 等待消磁
            }
            else
            {
                /* 消磁命令执行失败 */  // MODIFIED: 完善失败处理
                Cabinet_SetAlarm(context, ALARM_NONE, "Demagnetize command failed");
                context->single_switch = SWITCH_OFF;
                context->bus1_switch = SWITCH_OFF;
                context->target_state = CABINET_STATE_INVALID;
                return demag_result;
            }
        }
        else
        {
            /* 没有消磁函数，直接执行并机流程 */
            ExecuteResult_t result = Cabinet_ExecuteParallelProcedure(context);
            if (result == RESULT_SUCCESS)
            {
                context->state = CABINET_STATE_PARALLEL;
            }
            return result;
        }
    }

    /* 分支2：电源待机、远控、并机状态 */
    if ((ps->run_state == POWER_STANDBY) &&  // MODIFIED: 修正指针访问语法
        (ps->ctrl_mode == CONTROL_REMOTE) &&
        (ps->comb_state == POWER_PARALLEL))
    {
        return Cabinet_StartPowerSupply(context);
    }

    /* 分支3：其他状态 */
    Cabinet_SetAlarm(context, ALARM_STATE_ROLLBACK, "Invalid power status for parallel mode");

    /* 回退到待机状态 */
    context->single_switch = SWITCH_OFF;
    context->bus1_switch = SWITCH_OFF;
    context->target_state = CABINET_STATE_INVALID;  // ADDED: 清理目标状态

    return RESULT_FAILED;
}

/* 处理消磁等待 */
static void
Cabinet_ProcessDemagnetizeWait(CabinetContext_t* context)
{
    /* 使用50ms标志进行计时 */
    if (hal_timer_get_50ms_flag())
    {
        hal_timer_set_50ms_flag(false); /* 清除标志 */

        if (context->demagnetize_counter > 0)
        {
            context->demagnetize_counter--;

            // TODO:可以发出警告，正在消磁中
            if (context->demagnetize_counter == 0)
            {
                /* 消磁完成（超时） */
                context->demagnetize_state = DEMAGNETIZE_DONE;
            }
        }
    }
}

/* 启动电源 */
static ExecuteResult_t
Cabinet_StartPowerSupply(CabinetContext_t* context)
{
    static uint32_t power_start_counter = 0;

    /* 启动电源启动超时计数 */
    if (context->power_start_timeout == 0)
    {
        /* 发送启动电源命令 */
        if (!CAN_Adapter_SendPowerStart())
        {
            Cabinet_SetAlarm(context, ALARM_CAN_COMM_ERROR, "Failed to send power start command");
            context->single_switch = SWITCH_OFF;
            context->bus1_switch = SWITCH_OFF;
            context->target_state = CABINET_STATE_INVALID;
            return RESULT_COMM_ERROR;
        }

        context->power_start_timeout = POWER_START_TIMEOUT_MS;
        power_start_counter = 0;
        return RESULT_WAIT;  /* 等待电源响应 */
    }

    /* 检查超时 */
    if (power_start_counter < context->power_start_timeout)
    {
        power_start_counter++;

        /* 更新电源状态 */
        PowerStatus_t power_status = CAN_Adapter_GetPowerStatus();
        Cabinet_UpdatePowerStatus(context, &power_status);

        /* 检查电源是否已启动 */
        if (context->power_status.run_state == POWER_RUNNING)
        {
            context->power_start_timeout = 0; /* 清除电源启动超时计数 */
            power_start_counter = 0;

            /* 启动消磁流程 */
            if (context->demagnetize)
            {
                ExecuteResult_t demag_result = context->demagnetize();

                if (demag_result == RESULT_SUCCESS)
                {
                    /* 消磁命令执行成功，开始等待 */
                    context->demagnetize_state = DEMAGNETIZE_WAITING;
                    context->demagnetize_counter = DEMAGNETIZE_TIMEOUT_MS / TIMER_50MS_INTERVAL; /* 转换为50ms计数 */
                    return RESULT_WAIT; // 等待消磁
                }
                else
                {
                    /* 消磁命令执行失败 */
                    Cabinet_SetAlarm(context, ALARM_NONE, "Demagnetize command failed");
                    context->single_switch = SWITCH_OFF;
                    context->bus1_switch = SWITCH_OFF;
                    context->target_state = CABINET_STATE_INVALID;
                    return demag_result;
                }
            }
            else
            {
                /* 没有消磁函数，直接执行单机流程 */
                ExecuteResult_t result = Cabinet_ExecuteSingleProcedure(context);
                if (result == RESULT_SUCCESS)
                {
                    context->state = CABINET_STATE_SINGLE;
                }
                return result;
            }
        }
        else if (power_start_counter >= context->power_start_timeout)
        {
            Cabinet_SetAlarm(context, ALARM_POWER_START_FAILED, "Power start timeout");
            context->single_switch = SWITCH_OFF;
            context->bus1_switch = SWITCH_OFF;
            context->target_state = CABINET_STATE_INVALID;
            context->power_start_timeout = 0;
            power_start_counter = 0;
            return RESULT_TIMEOUT;
        }
    }

    return RESULT_WAIT;
}

/* 执行单机流程 */
static ExecuteResult_t
Cabinet_ExecuteSingleProcedure(CabinetContext_t* context)
{
    ExecuteResult_t result;

    /* 闭合 A3QF1 */
    if (context->switch_control)
    {
        result = context->switch_control(SWITCH_ID_A3QF1, SWITCH_ON);
        if (result != RESULT_SUCCESS)
        {
            Cabinet_SetAlarm(context, ALARM_NONE, "Failed to close A3QF1");
            return result;
        }
    }

    /* 闭合 A3QR1（并断开相应互锁回路） */
    if (context->switch_control)
    {
        result = context->switch_control(SWITCH_ID_A3QR1, SWITCH_ON);
        if (result != RESULT_SUCCESS)
        {
            Cabinet_SetAlarm(context, ALARM_NONE, "Failed to close A3QR1");
            /* 回滚：断开A3QF1 */
            (void)context->switch_control(SWITCH_ID_A3QF1, SWITCH_OFF);
            return result;
        }
    }

    return RESULT_SUCCESS;
}

/* 执行并机流程 */
static ExecuteResult_t
Cabinet_ExecuteParallelProcedure(CabinetContext_t* context)
{
    ExecuteResult_t result;

    /* 闭合 A3QF2 */
    if (context->switch_control)
    {
        result = context->switch_control(SWITCH_ID_A3QF2, SWITCH_ON);
        if (result != RESULT_SUCCESS)
        {
            Cabinet_SetAlarm(context, ALARM_NONE, "Failed to close A3QF2");
            return result;
        }
    }

    // 闭合 A3QR2
    if (context->switch_control)
    {
        result = context->switch_control(SWITCH_ID_A3QR2, SWITCH_ON);
        if (result != RESULT_SUCCESS)
        {
            Cabinet_SetAlarm(context, ALARM_NONE, "Failed to close A3QR2");
            /* 回滚：断开A3QF2 */
            (void)context->switch_control(SWITCH_ID_A3QF2, SWITCH_OFF);
            return result;
        }
    }

    return RESULT_SUCCESS;
}
/* 设置报警 */
static void
Cabinet_SetAlarm(CabinetContext_t* context, AlarmType_t type, const char* message)
{
    if (context && context->alarm_callback)
    {
        context->alarm_callback(type, message);
    }
}
