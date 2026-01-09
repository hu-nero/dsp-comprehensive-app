/*
 * section_cabinet.h
 *
 *  Created on: 2026年1月6日
 *      Author: huxl
 */

#ifndef SERVICES_LOGIC_SECTION_CABINET_H_
#define SERVICES_LOGIC_SECTION_CABINET_H_

#include "section_common.h"

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

/* 开关控制回调函数 */
typedef ExecuteResult_t (*SwitchControlFunc_t)(uint16_t switch_id, SwitchState_t state);
typedef uint16_t (*SwitchFeedFunc_t)(uint16_t switch_id);

/* 报警回调函数 */
typedef void (*AlarmCallback_t)(AlarmType_t alarm_type, const char* message);

/* 消磁状态枚举 */
typedef enum
{
    DEMAGNETIZE_IDLE = 0,      /* 空闲状态 */
    DEMAGNETIZE_WAITING,       /* 等待消磁完成 */
    DEMAGNETIZE_DONE,          /* 消磁完成 */
    DEMAGNETIZE_FAILED         /* 消磁失败 */
} DemagnetizeState_t;

/* 区段柜上下文结构体 */
typedef struct {
    CabinetState_t state;              /* 当前状态 */
    CabinetState_t target_state;       /* 保存的目标状态 */
    CabinetMode_t mode;                /* 当前模式 */
    SwitchState_t single_switch;       /* 单机开关状态 */
    SwitchState_t bus1_switch;         /* 母线1开关状态 */
    SwitchState_t short_switch;        /* 短接开关状态（消磁） */
    bool emergency_stop;               /* 急停状态（true=急停按下） */
    PowerStatus_t power_status;        /* 电源状态 */
    uint32_t power_start_timeout;      /* 电源启动超时计数 */
    DemagnetizeState_t demagnetize_state;  /* 消磁状态 */
    uint16_t demagnetize_counter;          // 消磁计数器（50ms为单位）

    /* 回调函数指针 */
    SwitchControlFunc_t switch_control;
    SwitchFeedFunc_t switch_feed;
    AlarmCallback_t alarm_callback;
} CabinetContext_t;


/* 区段柜控制接口声明 */
/**
 * @brief 初始化区段柜控制
 * @param context 区段柜上下文指针
 * @param switch_ctrl 开关控制回调函数
 * @param alarm_cb 报警回调函数
 * @return 初始化结果，true成功，false失败
 */
bool
Cabinet_Init(CabinetContext_t* context,
             SwitchControlFunc_t switch_ctrl,
             SwitchFeedFunc_t  switch_feed,
             AlarmCallback_t alarm_cb);

/**
 * @brief 周期处理函数
 * @param context 区段柜上下文指针
 */
void Cabinet_Process(CabinetContext_t* context);

/**
 * @brief 设置面板按钮状态
 * @param context 区段柜上下文指针
 * @param single_state 单机开关状态
 * @param bus1_state 母线1开关状态
 * @param short_state 短接开关状态
 * @param emergency 急停状态
 * @return 执行结果
 */
ExecuteResult_t Cabinet_SetSwitches(CabinetContext_t* context,
                                    SwitchState_t single_state,
                                    SwitchState_t bus1_state,
                                    SwitchState_t short_state,
                                    bool emergency);

/**
 * @brief 设置工作模式
 * @param context 区段柜上下文指针
 * @param mode 工作模式
 * @return 执行结果
 */
ExecuteResult_t Cabinet_SetMode(CabinetContext_t* context, CabinetMode_t mode);

/**
 * @brief 更新电源状态
 * @param context 区段柜上下文指针
 * @param power_status 电源状态
 */
void Cabinet_UpdatePowerStatus(CabinetContext_t* context, const PowerStatus_t* power_status);

/**
 * @brief 获取当前状态
 * @param context 区段柜上下文指针
 * @return 当前状态
 */
CabinetState_t Cabinet_GetState(const CabinetContext_t* context);

/**
 * @brief 获取当前模式
 * @param context 区段柜上下文指针
 * @return 当前模式
 */
CabinetMode_t Cabinet_GetMode(const CabinetContext_t* context);

#endif /* SERVICES_LOGIC_SECTION_CABINET_H_ */
