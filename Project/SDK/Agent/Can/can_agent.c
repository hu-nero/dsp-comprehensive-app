/*
 * can_agent.c
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */
#include "can_agent.h"
#include "string.h"
#include "SDK/Hal/Timer/hal_timer.h"

static uint16_t g_device_addr[eCanPort_Count] = {DEV_ADDRESS_CANA_SELF, DEV_ADDRESS_CANB_SELF};
static CAN_RxHandler_t g_rx_handler[eCanPort_Count] = {NULL, NULL};
static CAN_TxHandler_t g_send_status_callback[eCanPort_Count] = {NULL, NULL};
static CAN_MainLoopHandler_t g_mainloop_callback[eCanPort_Count] = {NULL, NULL};
static uint32_t g_heartbeat_tick[eCanPort_Count] = {0, 0};
static uint32_t g_status_tick[eCanPort_Count] = {0, 0};
static PendingCmd_t  g_pending_cmds[eCanPort_Count][MAX_PENDING_CMDS] = {{{0}}, {{0}}};

static bool CAN_SendFrame(TeCanPort Port, FuncCode_t Func, uint16_t DstAddr, uint16_t *Data, uint16_t Dlc);
static void CAN_Agent_ReceivedFrame(TeCanPort Port);
static void CAN_Agent_TimeoutCheck(TeCanPort Port);

static uint16_t CAN_Agent_FindEmptySlot(TeCanPort Port);
static void CAN_Agent_StartCmdTimeout(TeCanPort Port, uint16_t DstAddr, FuncCode_t FunCode, uint16_t *Data);
static void CAN_Agent_StopCmdTimeout(TeCanPort Port, uint16_t DstAddr, FuncCode_t FunCode);
static void CAN_Agent_CheckCmdTimeouts(TeCanPort Port);

uint16_t
CAN_Agent_SetExternalAddr(uint16_t Addr)
{
	if ((Addr > 0) && (Addr < 0xF))
	{
		g_device_addr[eCanPort_0] = Addr;
		return 0;
	}
	return 1;
}

// CAN 消息ID打包
inline uint16_t
CAN_PackID(FuncCode_t FuncCode, uint16_t SrcAddr, uint16_t DstAddr)
{
    // ID 格式: [10:8]=功能码, [7:4]=源地址, [3:0]=目标地址
    return ((FuncCode & 0x07) << 8) | ((SrcAddr & 0x0F) << 4) | (DstAddr & 0x0F);
}

// CAN 消息ID解包
inline void
CAN_UnpackID(uint16_t CanId, FuncCode_t *FuncCode, uint16_t *SrcAddr, uint16_t *DstAddr)
{
    if (FuncCode)
    {
        *FuncCode = (FuncCode_t)((CanId >> 8) & 0x07);
    }
    if (SrcAddr)
    {
        *SrcAddr = (uint16_t)((CanId >> 4) & 0x0F);
    }
    if (DstAddr)
    {
        *DstAddr = (uint16_t)(CanId & 0x0F);
    }
}

// 代理初始化
void
CAN_Agent_Init(TeCanPort Port)
{
    // 初始化 CAN 接口
    hal_can_init(Port, eCanPortSpeed_125kbps);

    // timer0 只初始化一次
    if (Port == eCanPort_0)
    {
        hal_timer_init(0, 150, 1000); //1ms
    }

    // 初始化定时发送任务
    g_heartbeat_tick[Port] = 0;
    g_status_tick[Port] = 0;
}

// 主循环处理
void
CAN_Agent_Process(TeCanPort Port)
{
    // 接收并处理接收到的消息
    CAN_Agent_ReceivedFrame(Port);

    // 检查是否需要发送定时消息
    CAN_Agent_TimeoutCheck(Port);
}

// 发送故障指令
bool
CAN_Agent_SendFault(TeCanPort Port, uint16_t DstAddr, FaultLevel_t Level, uint16_t FaultCode)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0]=故障等级, data[1-2]=故障代码
    data[0] = (uint16_t)Level;
    data[1] = (uint16_t)(FaultCode & 0xFF);
    data[2] = (uint16_t)(FaultCode >> 8);

    return CAN_SendFrame(Port, FUNC_FAULT, DstAddr, data, 8);
}

// 发送控制指令
bool
CAN_Agent_SendControl(TeCanPort Port, uint16_t DstAddr, CtrlCmd_t Cmd, uint16_t Param)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0-1]=控制指令, data[2]=参数
    data[0] = (Cmd & 0xFF);
    data[1] = ((Cmd >> 8) & 0xFF);
    data[2] = Param;
    CAN_Agent_StartCmdTimeout(Port, DstAddr, FUNC_CONTROL, data);
    return CAN_SendFrame(Port, FUNC_CONTROL, DstAddr, data, 8);
}

// 发送响应应答
bool
CAN_Agent_SendCtrlResp(TeCanPort Port, uint16_t DstAddr, ResponseStatus_t Status)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0-1]=响应状态
    data[0] = (uint16_t)(Status & 0xFF);
    data[1] = (uint16_t)(Status >> 8);

    return CAN_SendFrame(Port, FUNC_RESPONSE, DstAddr, data, 8);
}

// 发送状态广播
bool
CAN_Agent_SendStatus(TeCanPort Port, uint16_t DstAddr, uint16_t *Data)
{
    return CAN_SendFrame(Port, FUNC_STATUS, DstAddr, Data, 8);
}

// 发送心跳帧
bool
CAN_Agent_SendHeartbeat(TeCanPort Port)
{
    uint16_t data[8] = {0};
    static uint8_t heartbeat_counter[eCanPort_Count] = {0, 0};

    // 数据格式: data[0]=心跳计数器
    data[0] = heartbeat_counter[Port]++;

    return CAN_SendFrame(Port, FUNC_HEARTBEAT, DEV_ADDRESS_BROADCAST, data, 8);
}

// 发送CAN帧
static bool
CAN_SendFrame(TeCanPort Port, FuncCode_t Func, uint16_t DstAddr, uint16_t *Data, uint16_t Dlc)
{
    TsCanFrame frame;
    uint16_t can_id;

    if (!Data)
    {
        return false;
    }

    // 构造CAN帧
    can_id = CAN_PackID(Func, g_device_addr[Port], DstAddr);

    frame.id = can_id;
    frame.is_ext_id = 0;  // 标准帧
    frame.is_can_fd = 0;
    frame.dlc = Dlc > 8 ? 8 : Dlc;
    memcpy(frame.data, Data, sizeof(uint16_t)*frame.dlc);

    // 通过 FIFO 发送
    if (CAN_FIFO_Write(Port, &frame) != TX_OK)
    {
        return false;
    }

    return true;
}

// 接收并处理接收到的帧
static void
CAN_Agent_ReceivedFrame(TeCanPort Port)
{
    TsCanFrame *frame;
    FuncCode_t func;
    uint16_t src_addr, dst_addr;

    // 从接收队列中获取帧
    frame = hal_can_rx_queue_de(Port);
    if (!frame)
    {
        return;
    }

    // 解析ID
    CAN_UnpackID(frame->id, &func, &src_addr, &dst_addr);

    // 检查是否为本设备消息或广播消息
    if ((dst_addr != g_device_addr[Port]) && (dst_addr != DEV_ADDRESS_BROADCAST))
    {
        return;
    }

    // 接收到应答帧，停止对应指令的超时计时
    if (func == FUNC_RESPONSE)
    {
        CAN_Agent_StopCmdTimeout(Port, src_addr, FUNC_CONTROL);
    }

    // 如果有注册回调函数则调用
    if (g_rx_handler[Port])
    {
        g_rx_handler[Port](Port, func, src_addr, (uint16_t *)frame->data, (uint16_t)frame->dlc);
    }

}

// 定时检查，处理定时发送的消息
static void
CAN_Agent_TimeoutCheck(TeCanPort Port)
{
    static uint32_t last_tick[eCanPort_Count] = {0, 0};
    uint32_t current_tick;

    current_tick = hal_timer_get_timestamp();

    // 计算时间差
    uint32_t delta = current_tick - last_tick[Port];
    // 10ms 检查一次
    if (delta < 10)
    {
        return;
    }
    last_tick[Port] = current_tick;

    // 心跳帧发送周期，默认 1Hz
    if ((current_tick - g_heartbeat_tick[Port]) >= 1000)
    {
        //CAN_Agent_SendHeartbeat(Port);
        g_heartbeat_tick[Port] = current_tick;
    }

    // 状态广播发送周期，默认 3Hz
    if ((current_tick - g_status_tick[Port]) >= 300)
    {
        // 回调
        if (g_send_status_callback[Port])
        {
            g_send_status_callback[Port](Port, FUNC_STATUS, DEV_ADDRESS_BROADCAST);
        }
        g_status_tick[Port] = current_tick;
    }

    // 循环处理回调
    if (g_mainloop_callback[Port])
    {
        g_mainloop_callback[Port](Port);
    }
    // 检查cmd是否超时
    CAN_Agent_CheckCmdTimeouts(Port);
}

// 设置消息接收回调函数
void
CAN_SetRxHandler(TeCanPort Port, CAN_RxHandler_t Handler)
{
    g_rx_handler[Port] = Handler;
}
// 回调
void
CAN_Agent_Set_SendStatus_Callback(TeCanPort Port, CAN_TxHandler_t Handler)
{
    g_send_status_callback[Port] = Handler;
}

void
CAN_Agent_Set_MainLoop_Callback(TeCanPort Port, CAN_MainLoopHandler_t Handler)
{
    g_mainloop_callback[Port] = Handler;
}

// 查找空闲位置
static uint16_t
CAN_Agent_FindEmptySlot(TeCanPort Port)
{
	int i;
    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        if (!g_pending_cmds[Port][i].is_waiting)
        {
            return i;
        }
    }
    return 0xFFFF;  // 没有空闲位置
}

// 开始等待超时的指令计时
static void
CAN_Agent_StartCmdTimeout(TeCanPort Port, uint16_t DstAddr, FuncCode_t FunCode, uint16_t *Data)
{
    uint16_t slot = CAN_Agent_FindEmptySlot(Port);
    if (slot > MAX_PENDING_CMDS)
    {
        // 总是覆盖第一个
        slot = 0;
    }

    g_pending_cmds[Port][slot].is_waiting = true;
    g_pending_cmds[Port][slot].tick = hal_timer_get_timestamp();
    g_pending_cmds[Port][slot].dst_addr = DstAddr;
    g_pending_cmds[Port][slot].func_code = FunCode;
    g_pending_cmds[Port][slot].resp_times = 0;
    memcpy(g_pending_cmds[Port][slot].data, Data, sizeof(g_pending_cmds[Port][slot].data));
}

// 停止等待超时的指令计时
static void
CAN_Agent_StopCmdTimeout(TeCanPort Port, uint16_t DstAddr, FuncCode_t FunCode)
{
	int i;
    int most_recent_index = -1;
    uint32_t most_recent_time = 0;

    // 在等待列表中查找最新匹配的目标地址和功能码
    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        // 检查是否匹配目标地址和功能码
        if (g_pending_cmds[Port][i].is_waiting &&
            g_pending_cmds[Port][i].dst_addr == DstAddr &&
            g_pending_cmds[Port][i].func_code == FunCode)
        {
            // 检查这个是否比之前找到的更旧
            if ((most_recent_index == -1) ||
                (g_pending_cmds[Port][i].tick < most_recent_time))
            {
                most_recent_index = i;
                most_recent_time = g_pending_cmds[Port][i].tick;
            }
        }
    }

    // 如果找到匹配的条目，则清除它
    if (most_recent_index >= 0)
    {
        g_pending_cmds[Port][most_recent_index].is_waiting = false;
        g_pending_cmds[Port][most_recent_index].tick = 0;
        g_pending_cmds[Port][most_recent_index].dst_addr = 0;
        g_pending_cmds[Port][most_recent_index].func_code = FUNC_FAULT;
        g_pending_cmds[Port][most_recent_index].resp_times = 0;
        memset(g_pending_cmds[Port][most_recent_index].data, 0, sizeof(g_pending_cmds[Port][most_recent_index].data));
    }
}

// 检查等待超时的指令
static void
CAN_Agent_CheckCmdTimeouts(TeCanPort Port)
{
	int i;
    uint32_t current_tick = hal_timer_get_timestamp();

    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        if (g_pending_cmds[Port][i].is_waiting)
        {
            if ((current_tick - g_pending_cmds[Port][i].tick) >= CMD_TIMEOUT_MS)
            {
                // 超时处理
                g_pending_cmds[Port][i].is_waiting = false;
                g_pending_cmds[Port][i].tick = 0;

                // 根据功能码处理超时响应失败
                if (g_pending_cmds[Port][i].func_code == FUNC_CONTROL)
                {
                    if (g_pending_cmds[Port][i].resp_times < MAX_TIMEOUT_TIMES)
                    {
                        g_pending_cmds[Port][i].resp_times ++;
                        g_pending_cmds[Port][i].is_waiting = true;
                        g_pending_cmds[Port][i].tick = hal_timer_get_timestamp();
                        CAN_SendFrame(Port, FUNC_CONTROL, g_pending_cmds[Port][i].dst_addr, g_pending_cmds[Port][i].data, 8);
                    }
                }
            }
        }
    }
}

