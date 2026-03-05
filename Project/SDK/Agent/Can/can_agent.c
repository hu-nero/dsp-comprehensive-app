/*
 * can_agent.c
 *
 *  Created on: 2025年12月25日
 *      Author: huxl
 */
#include "can_agent.h"
#include "string.h"
#include "SDK/Hal/Timer/hal_timer.h"

static uint16_t g_device_addr[eCanPort_Count] = {DEV_ADDRESS_SELF, DEV_ADDRESS_SELF};
static CAN_RxHandler_t g_rx_handler[eCanPort_Count] = {NULL, NULL};
static uint32_t g_heartbeat_tick[eCanPort_Count] = {0, 0};
static uint32_t g_status_tick[eCanPort_Count] = {0, 0};
static DeviceStatus_t g_device_status[eCanPort_Count] = {{0}, {0}};
static PendingCmd_t  g_pending_cmds[eCanPort_Count][MAX_PENDING_CMDS] = {{{0}}, {{0}}};

static ParamSession_t g_param_send_session[eCanPort_Count];
static ParamSession_t g_param_recv_session[eCanPort_Count];
static uint16_t g_param_send_data[eCanPort_Count][PARAM_MAX_PIECES][PARAM_MAX_SIZE];
static uint16_t g_param_recv_data[eCanPort_Count][PARAM_MAX_PIECES][PARAM_MAX_SIZE];

static bool CAN_SendFrame(TeCanPort port, FuncCode_t Func, uint16_t DstAddr, uint16_t *Data, uint16_t Dlc);
static void CAN_Agent_ReceivedFrame(TeCanPort port);
static void CAN_Agent_TimeoutCheck(TeCanPort port);

static uint16_t CAN_Agent_FindEmptySlot(TeCanPort port);
static void CAN_Agent_StartCmdTimeout(TeCanPort port, uint16_t DstAddr, FuncCode_t FunCode, uint16_t *Data);
static void CAN_Agent_StopCmdTimeout(TeCanPort port, uint16_t DstAddr, FuncCode_t FunCode);
static void CAN_Agent_CheckCmdTimeouts(TeCanPort port);

static bool CAN_Agent_HandleParamStartResp(TeCanPort port, uint16_t SrcAddr, ResponseStatus_t Response);
static bool CAN_Agent_HandleParamEndResp(TeCanPort port, uint16_t SrcAddr, ResponseStatus_t Response);

ParamState_t
CAN_Agent_get_send_param_state(TeCanPort port)
{
    ParamSession_t *component = &g_param_send_session[port];
    return component->state;
}

void
CAN_Agent_set_send_param_state(TeCanPort port, ParamState_t State)
{
    ParamSession_t *component = &g_param_send_session[port];
    component->state = State;
}

void
CAN_Agent_clr_send_param_state(TeCanPort port)
{
    ParamSession_t *component = &g_param_send_session[port];
    component->state = PARAM_STATE_IDLE;
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
CAN_Agent_Init(TeCanPort port)
{
    // 初始化 CAN 接口
    hal_can_init(port, eCanPortSpeed_125kbps);

    // timer0 只初始化一次
    if (port == eCanPort_0)
    {
        hal_timer_init(0, 150, 1000); //1ms
    }

    // param init
    g_param_send_session[port].state = PARAM_STATE_IDLE;
    g_param_send_session[port].dst_addr = 0;
    g_param_send_session[port].device_type = PARAM_DEV_NONE;
    g_param_send_session[port].total_pieces = 0;
    g_param_send_session[port].current_piece = 0;
    g_param_send_session[port].frame_length = 0;
    g_param_send_session[port].retry_count = 0;
    g_param_send_session[port].data = &g_param_send_data[port][0][0];
    g_param_recv_session[port].data = &g_param_recv_data[port][0][0];

    memset(g_param_send_data[port], 0, sizeof(uint16_t)*PARAM_MAX_TOTAL_SIZE);
    memset(g_param_recv_data[port], 0, sizeof(uint16_t)*PARAM_MAX_TOTAL_SIZE);

    // 初始化本设备状态
    // TODO: 设备状态是否由代理处理？还是外部系统处理？我们是否只需要关注发送状态？然后再分发处理
    g_device_status[port].run_status = 0;
    g_device_status[port].fault_flag = 0;
    g_device_status[port].local_remote = 0;  // 默认本地
    g_device_status[port].work_mode = 0;
    g_device_status[port].power_status = 1;  // 上电

    // 初始化定时发送任务
    g_heartbeat_tick[port] = 0;
    g_status_tick[port] = 0;
}

// 主循环处理
void
CAN_Agent_Process(TeCanPort port)
{
    // 接收并处理接收到的消息
    CAN_Agent_ReceivedFrame(port);

    // 检查是否需要发送定时消息
    CAN_Agent_TimeoutCheck(port);
}

// 发送故障指令
bool
CAN_Agent_SendFault(TeCanPort port, uint16_t DstAddr, FaultLevel_t Level, uint16_t FaultCode)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0]=故障等级, data[1-2]=故障代码
    data[0] = (uint16_t)Level;
    data[1] = (uint16_t)(FaultCode & 0xFF);
    data[2] = (uint16_t)(FaultCode >> 8);

    return CAN_SendFrame(port, FUNC_FAULT, DstAddr, data, 8);
}

// 发送控制指令
bool
CAN_Agent_SendControl(TeCanPort port, uint16_t DstAddr, CtrlCmd_t Cmd, uint16_t Param)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0-1]=控制指令, data[2]=参数
    data[0] = (Cmd & 0xFF);
    data[1] = ((Cmd >> 8) & 0xFF);
    data[2] = Param;
    CAN_Agent_StartCmdTimeout(port, DstAddr, FUNC_CONTROL, data);
    return CAN_SendFrame(port, FUNC_CONTROL, DstAddr, data, 8);
}

// 发送响应应答
bool
CAN_Agent_SendCtrlResp(TeCanPort port, uint16_t DstAddr, ResponseStatus_t Status)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0-1]=响应状态
    data[0] = (uint16_t)(Status & 0xFF);
    data[1] = (uint16_t)(Status >> 8);

    return CAN_SendFrame(port, FUNC_RESPONSE, DstAddr, data, 8);
}

// 发送状态广播
bool
CAN_Agent_SendStatus(TeCanPort port, DeviceStatus_t Status)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0]=状态字
    data[0] = (Status.run_status & 0x01) |
              ((Status.fault_flag & 0x01) << 1) |
              ((Status.local_remote & 0x01) << 2) |
              ((Status.work_mode & 0x01) << 3) |
              ((Status.power_status & 0x01) << 4);

    return CAN_SendFrame(port, FUNC_STATUS, DEV_ADDRESS_BROADCAST, data, 8);
}

// 发送参数交互指令
bool
CAN_Agent_SendParamCmd(TeCanPort port, uint16_t DstAddr, ParamDevice_t Dev, ParamCmd_t Cmd, uint16_t Piece, uint16_t FrameLen)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0]=设备标识, data[1]=指令码，data[2]=分片数，data[3-4]=长度
    data[0] = (Dev & 0xFF);
    data[1] = (Cmd & 0xFF);
    data[2] = (Piece & 0xFF);
    data[3] = (FrameLen & 0xFF);
    data[4] = ((FrameLen >> 8) & 0xFF);
    CAN_Agent_StartCmdTimeout(port, DstAddr, FUNC_PARAM_CMD, data);
    return CAN_SendFrame(port, FUNC_PARAM_CMD, DstAddr, data, 8);
}

// 发送参数应答
bool
CAN_Agent_SendParamResp(TeCanPort port, uint16_t DstAddr, ResponseStatus_t Status)
{
    uint16_t data[8] = {0};

    // 数据格式: data[0-1]=应答状态
    data[0] = (uint16_t)(Status & 0xFF);
    data[1] = (uint16_t)(Status >> 8);

    return CAN_SendFrame(port, FUNC_PARAM_RESPONSE, DstAddr, data, 8);
}

// 发送参数数据
bool
CAN_Agent_SendParamData(TeCanPort port, uint16_t DstAddr, uint16_t PieceIndex, uint16_t *Data, uint16_t FrameLen)
{
    uint16_t data[8] = {0};

    if (FrameLen > 6)
    {
        return false;
    }
    // 数据格式: data[0-1]=应答状态
    data[0] = (PieceIndex & 0xFF);
    data[1] = (FrameLen & 0xFF);
    memcpy(&data[2], Data, FrameLen * sizeof(uint16_t));

    return CAN_SendFrame(port, FUNC_PARAM_DATA, DstAddr, data, 8);
}

// 发送心跳帧
bool
CAN_Agent_SendHeartbeat(TeCanPort port)
{
    uint16_t data[8] = {0};
    static uint8_t heartbeat_counter[eCanPort_Count] = {0, 0};

    // 数据格式: data[0]=心跳计数器
    data[0] = heartbeat_counter[port]++;

    return CAN_SendFrame(port, FUNC_HEARTBEAT, DEV_ADDRESS_BROADCAST, data, 8);
}

// 发送CAN帧
static bool
CAN_SendFrame(TeCanPort port, FuncCode_t Func, uint16_t DstAddr, uint16_t *Data, uint16_t Dlc)
{
    TsCanFrame frame;
    uint16_t can_id;

    if (!Data)
    {
        return false;
    }

    // 构造CAN帧
    can_id = CAN_PackID(Func, g_device_addr[port], DstAddr);

    frame.id = can_id;
    frame.is_ext_id = 0;  // 标准帧
    frame.is_can_fd = 0;
    frame.dlc = Dlc > 8 ? 8 : Dlc;
    memcpy(frame.data, Data, sizeof(uint16_t)*frame.dlc);

    // 通过 FIFO 发送
    if (CAN_FIFO_Write(port, &frame) != TX_OK)
    {
        return false;
    }

    return true;
}

// 接收并处理接收到的帧
static void
CAN_Agent_ReceivedFrame(TeCanPort port)
{
    TsCanFrame *frame;
    FuncCode_t func;
    uint16_t src_addr, dst_addr;

    // 从接收队列中获取帧
    frame = hal_can_rx_queue_de(port);
    if (!frame)
    {
        return;
    }

    // 解析ID
    CAN_UnpackID(frame->id, &func, &src_addr, &dst_addr);

    // 检查是否为本设备消息或广播消息
    if ((dst_addr != g_device_addr[port]) && (dst_addr != DEV_ADDRESS_BROADCAST))
    {
        return;
    }

    // 接收到应答帧，停止对应指令的超时计时
    switch (func)
    {
        case FUNC_RESPONSE:
            {
                CAN_Agent_StopCmdTimeout(port, src_addr, FUNC_CONTROL);
            }
            break;

        case FUNC_PARAM_RESPONSE:
            {
                bool res;
                uint16_t response;
                CAN_Agent_StopCmdTimeout(port, src_addr, FUNC_PARAM_CMD);
                switch (frame->data[0])
                {
                    case PARAM_CMD_START:
                        {
                            // TODO:处理回复信息，如果正常，执行数据发送
                            response = (frame->data[1] & 0xFF)|((frame->data[2]&0xFF)<<8);
                            res = CAN_Agent_HandleParamStartResp(port, src_addr,(ResponseStatus_t)response);
                            if (res == false)
                            {
                                CAN_Agent_clr_send_param_state(port);
                            }
                        }
                        break;
                    case PARAM_CMD_END:
                        {
                            // TODO:结束分片，给怎么办
                            response = (frame->data[1] & 0xFF)|((frame->data[2]&0xFF)<<8);
                            res = CAN_Agent_HandleParamEndResp(port, src_addr,(ResponseStatus_t)response);
                            if (res == false)
                            {
                                CAN_Agent_clr_send_param_state(port);
                            }
                        }
                        break;
                    default:break;
                }

            }
            break;
        default:
            break;
    }

    // TODO: 解析指令帧
    // TODO: 解析数据帧

    // 如果有注册回调函数则调用
    if (g_rx_handler[port])
    {
        g_rx_handler[port](port, func, src_addr, (uint16_t *)frame->data, (uint16_t)frame->dlc);
    }

}

// 定时检查，处理定时发送的消息
static void
CAN_Agent_TimeoutCheck(TeCanPort port)
{
    static uint32_t last_tick[eCanPort_Count] = {0, 0};
    uint32_t current_tick;

    current_tick = hal_timer_get_timestamp();

    // 计算时间差
    uint32_t delta = current_tick - last_tick[port];
    // 10ms 检查一次
    if (delta < 10)
    {
        return;
    }
    last_tick[port] = current_tick;

    // 心跳帧发送周期，默认 1Hz
    if ((current_tick - g_heartbeat_tick[port]) >= 1000)
    {
        //CAN_Agent_SendHeartbeat(port);
        g_heartbeat_tick[port] = current_tick;
    }

    // 状态广播发送周期，默认 1Hz
    if ((current_tick - g_status_tick[port]) >= 1000)
    {
        //CAN_Agent_SendStatus(port, g_device_status[port]);
        g_status_tick[port] = current_tick;
    }
    // 检查cmd是否超时
    CAN_Agent_CheckCmdTimeouts(port);
}

// 设置消息接收回调函数
void
CAN_SetRxHandler(TeCanPort port, CAN_RxHandler_t Handler)
{
    g_rx_handler[port] = Handler;
}

// 查找空闲位置
static uint16_t
CAN_Agent_FindEmptySlot(TeCanPort port)
{
	int i;
    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        if (!g_pending_cmds[port][i].is_waiting)
        {
            return i;
        }
    }
    return 0xFFFF;  // 没有空闲位置
}

// 开始等待超时的指令计时
static void
CAN_Agent_StartCmdTimeout(TeCanPort port, uint16_t DstAddr, FuncCode_t FunCode, uint16_t *Data)
{
    uint16_t slot = CAN_Agent_FindEmptySlot(port);
    if (slot > MAX_PENDING_CMDS)
    {
        // 总是覆盖第一个
        slot = 0;
    }

    g_pending_cmds[port][slot].is_waiting = true;
    g_pending_cmds[port][slot].tick = hal_timer_get_timestamp();
    g_pending_cmds[port][slot].dst_addr = DstAddr;
    g_pending_cmds[port][slot].func_code = FunCode;
    g_pending_cmds[port][slot].resp_times = 0;
    memcpy(g_pending_cmds[port][slot].data, Data, sizeof(g_pending_cmds[port][slot].data));
}

// 停止等待超时的指令计时
static void
CAN_Agent_StopCmdTimeout(TeCanPort port, uint16_t DstAddr, FuncCode_t FunCode)
{
	int i;
    int most_recent_index = -1;
    uint32_t most_recent_time = 0;

    // 在等待列表中查找最新匹配的目标地址和功能码
    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        // 检查是否匹配目标地址和功能码
        if (g_pending_cmds[port][i].is_waiting &&
            g_pending_cmds[port][i].dst_addr == DstAddr &&
            g_pending_cmds[port][i].func_code == FunCode)
        {
            // 检查这个是否比之前找到的更旧
            if ((most_recent_index == -1) ||
                (g_pending_cmds[port][i].tick < most_recent_time))
            {
                most_recent_index = i;
                most_recent_time = g_pending_cmds[port][i].tick;
            }
        }
    }

    // 如果找到匹配的条目，则清除它
    if (most_recent_index >= 0)
    {
        g_pending_cmds[port][most_recent_index].is_waiting = false;
        g_pending_cmds[port][most_recent_index].tick = 0;
        g_pending_cmds[port][most_recent_index].dst_addr = 0;
        g_pending_cmds[port][most_recent_index].func_code = FUNC_FAULT;
        g_pending_cmds[port][most_recent_index].resp_times = 0;
        memset(g_pending_cmds[port][most_recent_index].data, 0, sizeof(g_pending_cmds[port][most_recent_index].data));
    }
}

// 检查等待超时的指令
static void
CAN_Agent_CheckCmdTimeouts(TeCanPort port)
{
	int i;
    uint32_t current_tick = hal_timer_get_timestamp();

    for (i = 0; i < MAX_PENDING_CMDS; i++)
    {
        if (g_pending_cmds[port][i].is_waiting)
        {
            if ((current_tick - g_pending_cmds[port][i].tick) >= CMD_TIMEOUT_MS)
            {
                // 超时处理
                g_pending_cmds[port][i].is_waiting = false;
                g_pending_cmds[port][i].tick = 0;

                // 根据功能码处理超时响应失败
                switch (g_pending_cmds[port][i].func_code)
                {
                    case FUNC_CONTROL:
                        {
                            if (g_pending_cmds[port][i].resp_times < MAX_TIMEOUT_TIMES)
                            {
                                g_pending_cmds[port][i].resp_times ++;
                                g_pending_cmds[port][i].is_waiting = true;
                                g_pending_cmds[port][i].tick = hal_timer_get_timestamp();
                                CAN_SendFrame(port, FUNC_CONTROL, g_pending_cmds[port][i].dst_addr, g_pending_cmds[port][i].data, 8);
                            }
                        }
                        break;
                    case FUNC_PARAM_CMD:
                        {
                            if (g_pending_cmds[port][i].resp_times < MAX_TIMEOUT_TIMES)
                            {
                                g_pending_cmds[port][i].resp_times ++;
                                g_pending_cmds[port][i].is_waiting = true;
                                g_pending_cmds[port][i].tick = hal_timer_get_timestamp();
                                CAN_SendFrame(port, FUNC_PARAM_CMD, g_pending_cmds[port][i].dst_addr, g_pending_cmds[port][i].data, 8);
                            }
                            else
                            {
                                ParamSession_t *component = &g_param_send_session[port];
                                component->state = PARAM_STATE_IDLE;
                            }
                        }
                        break;
                    case FUNC_PARAM_DATA:
                        {
                            // TODO: 参数数据帧也需要超时重发？不过目前没有用到这个，如果要增加这个功能的话也需要同步修改g_pending_cmds
                        }
                        break;
                    default:break;
                }
            }
        }
    }
}

bool
CAN_Agent_StartSendParamTranmit(TeCanPort port, uint16_t DstAddr, ParamDevice_t Device, uint16_t *Data, uint16_t TotalLen)
{
    ParamSession_t *component = &g_param_send_session[port];
    if (component->state != PARAM_STATE_IDLE)
    {
        return false;
    }
    if ((!Data) || (TotalLen == 0) || (TotalLen > PARAM_MAX_TOTAL_SIZE))
    {
        return false;
    }
    //1.填充send param的结构体，主要填充data

    // 计算分片数
    uint16_t total_pieces = (TotalLen + PARAM_MAX_SIZE-1) / PARAM_MAX_SIZE;
    if (total_pieces > PARAM_MAX_PIECES)
    {
        return false;
    }

    // 初始化发送会话
    memset(component->data, 0, sizeof(uint16_t)*PARAM_MAX_TOTAL_SIZE);
    component->state = PARAM_STATE_WAIT_START_RESP;
    component->dst_addr = DstAddr;
    component->device_type = Device;
    component->total_pieces = total_pieces;
    component->current_piece = 0;
    component->frame_length = TotalLen;
    component->retry_count = 0;

    // 填充buf
    memcpy(component->data, Data, sizeof(uint16_t)*TotalLen);

    //2.执行发送
    return CAN_Agent_SendParamCmd(port, DstAddr, Device, PARAM_CMD_START, total_pieces, TotalLen);
}

/* 处理参数响应 */
static bool
CAN_Agent_HandleParamStartResp(TeCanPort port, uint16_t SrcAddr, ResponseStatus_t Response)
{
    bool res;
    uint16_t index,len;

    ParamSession_t *component = &g_param_send_session[port];
    if (component->state != PARAM_STATE_WAIT_START_RESP)
    {
        return false;
    }
    if (SrcAddr != component->dst_addr)
        return false;
    if (Response != RESP_SUCCESS)
        return false;

    // 分片
    for (index=0;index<component->total_pieces;index++)
    {
        if (index == component->total_pieces -1)
        {
            if (component->frame_length % PARAM_MAX_SIZE == 0)
            {
                len = PARAM_MAX_SIZE;
            }
            else
            {
                len = component->frame_length % PARAM_MAX_SIZE;
            }
        }
        else
        {
            len = PARAM_MAX_SIZE;
        }
        res = CAN_Agent_SendParamData(port, component->dst_addr, index, (component->data+index*PARAM_MAX_SIZE), len);
        if (res == false)
            break;
    }
    if (index != component->total_pieces)
    {
        res = false;
    }
    else
    {
        //发送结束命令
        res = CAN_Agent_SendParamCmd(port, component->dst_addr, component->device_type, PARAM_CMD_END, 0, 0);
        component->state = PARAM_STATE_WAIT_END_RESP;
    }
    return res;
}

/* 处理参数响应 */
static bool
CAN_Agent_HandleParamEndResp(TeCanPort port, uint16_t SrcAddr, ResponseStatus_t Response)
{
    ParamSession_t *component = &g_param_send_session[port];
    if (component->state != PARAM_STATE_WAIT_END_RESP)
    {
        return false;
    }
    if (SrcAddr != component->dst_addr)
        return false;
    if (Response != RESP_SUCCESS)
        return false;

    // send state
    component->state = PARAM_STATE_IDLE;
    return true;
}
