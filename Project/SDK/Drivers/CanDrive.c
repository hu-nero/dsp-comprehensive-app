#include "CanDrive.h"
#include <stdbool.h>
#include "string.h"
#include "DSP2833x_PieCtrl.h"

static void InitCanaDrive(uint16_t Bps);
static void InitCanbDrive(uint16_t Bps);
static void InitCanaPinConfig(int canSysClk, int brp);
static void InitCanbPinConfig(int canSysClk, int brp);
static void DeinitCanaDrive(void);
static void DeinitCanbDrive(void);
static void CanaRecvDrive(uint16_t Mb, S_CANMSG *s_cRx);
static void CanbRecvDrive(uint16_t Mb, S_CANMSG *s_cRx);
static void CanaSendDrive(uint16_t Mb, S_CANMSG *s_cTx);
static void CanbSendDrive(uint16_t Mb, S_CANMSG *s_cTx);

static void InitCanaBoxConfig(void);
static void InitCanbBoxConfig(void);
static void InitCanaInterrupt(void);
static void InitCanbInterrupt(void);


uint16_t InitCanDrive(Can_TypeDef *Can_Struct, uint16_t Bps)
{
    switch(Can_Struct->MsgChn)
    {
        case CANCHNA:
            {
                InitCanaDrive(Bps);
            }
            break;
        case CANCHNB:
            {
                InitCanbDrive(Bps);
            }
            break;
        default:return 1;
    }
    return 0;
}

uint16_t DeinitCanDrive(Can_TypeDef *Can_Struct)
{
    switch(Can_Struct->MsgChn)
    {
        case CANCHNA:
            {
                DeinitCanaDrive();
            }
            break;
        case CANCHNB:
            {
                DeinitCanbDrive();
            }
            break;
        default:return 1;
    }
    return 0;
}

uint16_t CanRecvDrive(Can_TypeDef *Can_Struct, uint16_t Mb, S_CANMSG *s_cRx)
{
    switch(Can_Struct->MsgChn)
    {
        case CANCHNA:
            {
                CanaRecvDrive(Mb, s_cRx);
            }
            break;
        case CANCHNB:
            {
                CanbRecvDrive(Mb, s_cRx);
            }
            break;
        default:return 1;
    }
    return 0;
}

uint16_t CanSendDrive(Can_TypeDef *Can_Struct, uint16_t Mb, S_CANMSG *s_cTx)
{
    switch(Can_Struct->MsgChn)
    {
        case CANCHNA:
            {
            	CanaSendDrive(Mb, s_cTx);
            }
            break;
        case CANCHNB:
            {
            	CanbSendDrive(Mb, s_cTx);
            }
            break;
        default:return 1;
    }
    return 0;
}
// 初始化函数
static void InitCanaDrive(uint16_t Bps)
{
    uint32_t canSysClk = CANSYSCLK;
    
    // 使能CAN模块时钟
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 1;  // 使能CAN-A时钟
    EDIS;
    
    // 配置GPIO
    InitECanaGpio();
    
    // 配置CAN时序
    InitCanaPinConfig(canSysClk, Bps);
    
    // 配置邮箱
    InitCanaBoxConfig();
    
    // 配置中断
    InitCanaInterrupt();
}

// 初始化函数
static void InitCanbDrive(uint16_t Bps)
{
    uint32_t canSysClk = CANSYSCLK;
    
    // 使能CAN模块时钟
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 1;  // 使能CAN-B时钟
    EDIS;
    
    // 配置GPIO
    InitECanbGpio();
    
    // 配置CAN时序
    InitCanbPinConfig(canSysClk, Bps);
    
    // 配置邮箱
    InitCanbBoxConfig();
    
    // 配置中断
    InitCanbInterrupt();
}

// dinit函数
static void DeinitCanaDrive(void)
{
    struct ECAN_REGS ECanaShadow;
    
    // 禁用所有中断标志
    EALLOW;
    ECanaRegs.CANME.all = 0x00000000;  // 先禁用邮箱
    ECanaRegs.CANTA.all = 0xFFFFFFFF;
    ECanaRegs.CANRMP.all = 0xFFFFFFFF;
    ECanaRegs.CANAA.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
    EDIS;
    
    // 进入暂停模式
    EALLOW;
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.SUSP = 1;  // 软停止模式
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    EDIS;
    
    // 禁用中断
    IER &= ~M_INT9;
    PieCtrlRegs.PIEIER9.all = 0x0000;
    
    // 禁用CAN模块时钟
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 0;
    EDIS;
}
// dinit函数
static void DeinitCanbDrive(void)
{
    struct ECAN_REGS ECanbShadow;
    
    // 禁用所有中断标志
    EALLOW;
    ECanbRegs.CANME.all = 0x00000000;  // 先禁用邮箱
    ECanbRegs.CANTA.all = 0xFFFFFFFF;
    ECanbRegs.CANRMP.all = 0xFFFFFFFF;
    ECanbRegs.CANAA.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF1.all = 0xFFFFFFFF;
    EDIS;
    
    // 进入暂停模式
    EALLOW;
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.SUSP = 1;  // 软停止模式
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    EDIS;
    
    // 禁用中断
    IER &= ~M_INT9;
    PieCtrlRegs.PIEIER9.all = 0x0000;
    
    // 禁用CAN模块时钟
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 0;
    EDIS;
}

static void InitCanaPinConfig(int canSysClk, int baudrate)
{
    struct ECAN_REGS ECanaShadow;
    
    // 配置引脚功能
    EALLOW;
    ECanaRegs.CANTIOC.all = 0x00000008;  // bit3: TXFUNC=1, 使能TX功能
    ECanaRegs.CANRIOC.all = 0x00000008;  // bit3: RXFUNC=1, 使能RX功能
    EDIS;
    
    // 配置CANMC寄存器
    EALLOW;
    // 开发板配置: 0x0001F6A0 = 0000,0000,0000,0001,1111,0110,1010,0000
    ECanaRegs.CANMC.all = 0x0001F6A0;
    /*
    bit16:SUSPEND=1 (free mode - 自由模式)
    bit15:MBCC=1 (时间戳计数器清零)
    bit14:TCC=1 (时间戳MSB清零)
    bit13:SCB=1 (eCAN模式)
    bit12:CCR=1 (请求配置模式)
    bit11:PDR=0 (无断电请求)
    bit10:DBO=1 (LSB优先 - 先发送最低有效字节)
    bit9:WUBA=1 (总线活动唤醒)
    bit8:CDR=0 (正常模式)
    bit7:ABO=1 (自动总线打开)
    bit6:STM=0 (正常模式，非自测模式)
    bit5:SRES=1 (软件复位)
    bit4-0:MBNR=0 (邮箱编号为0)
    */
    EDIS;
    
    // 等待进入配置模式(CCE=1)
    do {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 1);
    
    // 配置位时序寄存器(CANBTC)
    EALLOW;
    ECanaShadow.CANBTC.all = 0;
    
#if(CPU_FRQ_150MHZ)   // 150MHz系统时钟，CAN时钟75MHz
    // BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
    if (125 == baudrate) // 85% sampling point
    {
        ECanaShadow.CANBTC.bit.BRPREG = 29;
        ECanaShadow.CANBTC.bit.TSEG2REG = 2;
        ECanaShadow.CANBTC.bit.TSEG1REG = 15;
    }
    else if (500 == baudrate)
    {
        ECanaShadow.CANBTC.bit.BRPREG = 9;
        ECanaShadow.CANBTC.bit.TSEG2REG = 5;
        ECanaShadow.CANBTC.bit.TSEG1REG = 7;
    }
    else if (1000 == baudrate) // 80% sampling point
    {
        ECanaShadow.CANBTC.bit.BRPREG = 4;
        ECanaShadow.CANBTC.bit.TSEG2REG = 2;
        ECanaShadow.CANBTC.bit.TSEG1REG = 10;
    }
#endif
    
#if(CPU_FRQ_100MHZ)   // 100MHz系统时钟，CAN时钟50MHz
    // BAUT=50M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
    if(baudrate == 1000000) {
        ECanaShadow.CANBTC.bit.BRPREG = 9;
        ECanaShadow.CANBTC.bit.TSEG2REG = 1;
        ECanaShadow.CANBTC.bit.TSEG1REG = 6;
    }
#endif
    
    // 通用配置
    ECanaShadow.CANBTC.bit.SAM = 1;  // 三次采样
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;
    
    //  清除SCC模式相关寄存器
    ECanaRegs.CANGAM.all = 0;
    ECanaLAMRegs.LAM0.all = 0;
    ECanaLAMRegs.LAM3.all = 0;
    
    //  退出初始化模式
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 0;  // 退出配置模式
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    EDIS;
    
    //  等待退出配置模式(CCE=0)
    do {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE == 1);
}

static void InitCanbPinConfig(int canSysClk, int baudrate)
{
    struct ECAN_REGS ECanbShadow;
    
    // 配置引脚功能
    EALLOW;
    ECanbRegs.CANTIOC.all = 0x00000008;  // bit3: TXFUNC=1, 使能TX功能
    ECanbRegs.CANRIOC.all = 0x00000008;  // bit3: RXFUNC=1, 使能RX功能
    EDIS;
    
    // 配置CANMC寄存器
    EALLOW;
    // 开发板配置: 0x0001F6A0 = 0000,0000,0000,0001,1111,0110,1010,0000
    ECanbRegs.CANMC.all = 0x0001F6A0;
    /*
    bit16:SUSPEND=1 (free mode - 自由模式)
    bit15:MBCC=1 (时间戳计数器清零)
    bit14:TCC=1 (时间戳MSB清零)
    bit13:SCB=1 (eCAN模式)
    bit12:CCR=1 (请求配置模式)
    bit11:PDR=0 (无断电请求)
    bit10:DBO=1 (LSB优先 - 先发送最低有效字节)
    bit9:WUBA=1 (总线活动唤醒)
    bit8:CDR=0 (正常模式)
    bit7:ABO=1 (自动总线打开)
    bit6:STM=0 (正常模式，非自测模式)
    bit5:SRES=1 (软件复位)
    bit4-0:MBNR=0 (邮箱编号为0)
    */
    EDIS;
    
    // 等待进入配置模式(CCE=1)
    do {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 1);
    
    // 配置位时序寄存器(CANBTC)
    EALLOW;
    ECanbShadow.CANBTC.all = 0;
    
#if(CPU_FRQ_150MHZ)   // 150MHz系统时钟，CAN时钟75MHz
    // BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
    if (125 == baudrate) // 85% sampling point
    {
        ECanbShadow.CANBTC.bit.BRPREG = 29;
        ECanbShadow.CANBTC.bit.TSEG2REG = 2;
        ECanbShadow.CANBTC.bit.TSEG1REG = 15;
    }
    else if (500 == baudrate)
    {
        ECanbShadow.CANBTC.bit.BRPREG = 9;
        ECanbShadow.CANBTC.bit.TSEG2REG = 5;
        ECanbShadow.CANBTC.bit.TSEG1REG = 7;
    }
    else if (1000 == baudrate) // 80% sampling point
    {
        ECanbShadow.CANBTC.bit.BRPREG = 4;
        ECanbShadow.CANBTC.bit.TSEG2REG = 2;
        ECanbShadow.CANBTC.bit.TSEG1REG = 10;
    }
#endif
    
#if(CPU_FRQ_100MHZ)   // 100MHz系统时钟，CAN时钟50MHz
    // BAUT=50M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
    if(baudrate == 1000000) {
        ECanbShadow.CANBTC.bit.BRPREG = 9;
        ECanbShadow.CANBTC.bit.TSEG2REG = 1;
        ECanbShadow.CANBTC.bit.TSEG1REG = 6;
    }
#endif
    
    // 通用配置
    ECanbShadow.CANBTC.bit.SAM = 1;  // 三次采样
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;
    
    //  清除SCC模式相关寄存器
    ECanbRegs.CANGAM.all = 0;
    ECanbLAMRegs.LAM0.all = 0;
    ECanbLAMRegs.LAM3.all = 0;
    
    //  退出初始化模式
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CCR = 0;  // 退出配置模式
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    EDIS;
    
    //  等待退出配置模式(CCE=0)
    do {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE == 1);
}

// 邮箱初始化
static void InitCanaBoxConfig(void)
{
	int i;
    struct ECAN_REGS ECanaShadow;
    
    // 禁用所有邮箱
    ECanaRegs.CANME.all = 0;
    
    // 初始化所有MSGCTRL寄存器为0
    EALLOW;
    for(i = 0; i < 32; i++) {
        ((volatile struct MBOX*)&ECanaMboxes)[i].MSGCTRL.all = 0x00000000;
    }
    EDIS;
    
    // 配置邮箱方向 - 邮箱0发送，邮箱4接收
    ECanaRegs.CANMD.all = 0x00000010;  // bit4=1(接收), bit0=0(发送)
    
    // 清除所有标志寄存器
    ECanaRegs.CANTRS.all = 0;
    ECanaRegs.CANTRR.all = 0;
    ECanaRegs.CANTA.all = 0xFFFFFFFF;
    ECanaRegs.CANAA.all = 0xFFFFFFFF;
    ECanaRegs.CANRMP.all = 0xFFFFFFFF;
    ECanaRegs.CANRML.all = 0xFFFFFFFF;
    ECanaRegs.CANRFP.all = 0xFFFFFFFF;
    ECanaRegs.CANES.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
    ECanaRegs.CANMIL.all = 0x00000000;  //邮箱4用线0，邮箱0用线0
    ECanaRegs.CANOPC.all = 0xFFFFFFFF;  // 所有接收邮箱不能覆盖
    ECanaRegs.CANTOC.all = 0;  // 禁用超时功能
    ECanaRegs.CANTOS.all = 0xFFFFFFFF;  // 清除超时状态
    
    // 配置邮箱0（发送邮箱）
    EALLOW;
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CDR = 1;  // 设置CDR以访问邮箱数据
    ECanaShadow.CANMC.bit.MBNR = 0; // 邮箱0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    // 初始化邮箱0
    ECanaMboxes.MBOX0.MSGID.all = 0;
    // 设置标准ID
    ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = 1;
    // 标准帧(IDE=0)，发送邮箱不使用屏蔽(AME=0)，非自动应答(AAM=0)
    ECanaMboxes.MBOX0.MSGCTRL.all = 0;
    ECanaMboxes.MBOX0.MSGCTRL.bit.TPL = 31;  // 最高发送优先级
    ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;   // 数据长度8字节
    ECanaMboxes.MBOX0.MDH.all = 0;
    ECanaMboxes.MBOX0.MDL.all = 0;
    
    // 配置邮箱4（接收邮箱）
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CDR = 1;
    ECanaShadow.CANMC.bit.MBNR = 4;  // 邮箱4
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    // 初始化邮箱4
    ECanaMboxes.MBOX4.MSGID.all = 0;
    ECanaMboxes.MBOX4.MSGID.bit.AME = 1;  // 使能接收屏蔽
    ECanaMboxes.MBOX4.MSGID.bit.STDMSGID = 0;
    ECanaMboxes.MBOX4.MSGCTRL.all = 0;
    ECanaMboxes.MBOX4.MSGCTRL.bit.TPL = 27;  // 接收优先级
    ECanaMboxes.MBOX4.MSGCTRL.bit.DLC = 8;   // 数据长度8字节
    ECanaMboxes.MBOX4.MDH.all = 0;
    ECanaMboxes.MBOX4.MDL.all = 0;
    
    // 配置邮箱4的接收屏蔽(LAM4)
    ECanaLAMRegs.LAM4.bit.LAMI = 0;       // 标准帧过滤
    ECanaLAMRegs.LAM4.bit.LAM_H = 0xFFFF;
    ECanaLAMRegs.LAM4.bit.LAM_L = 0xFFFF;
    
    // 清除CDR并使能邮箱
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CDR = 0;    // 清除CDR
    ECanaShadow.CANMC.bit.MBNR = 0;   // 邮箱编号清零
    ECanaShadow.CANMC.bit.SRES = 0;   // 清除软件复位标志
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    // 使能邮箱0,4
    ECanaRegs.CANME.all = 0x00000011;  // 使能邮箱0,4 (bit0,4=1)
    
    EDIS;
}
// 邮箱初始化
static void InitCanbBoxConfig(void)
{
	int i;
    struct ECAN_REGS ECanbShadow;
    
    // 禁用所有邮箱
    ECanbRegs.CANME.all = 0;
    
    // 初始化所有MSGCTRL寄存器为0
    EALLOW;
    for(i = 0; i < 32; i++) {
        ((volatile struct MBOX*)&ECanbMboxes)[i].MSGCTRL.all = 0x00000000;
    }
    EDIS;
    
    // 配置邮箱方向 - 邮箱0发送，邮箱4接收
    ECanbRegs.CANMD.all = 0x00000010;  // bit4=1(接收), bit0=0(发送)
    
    // 清除所有标志寄存器
    ECanbRegs.CANTRS.all = 0;
    ECanbRegs.CANTRR.all = 0;
    ECanbRegs.CANTA.all = 0xFFFFFFFF;
    ECanbRegs.CANAA.all = 0xFFFFFFFF;
    ECanbRegs.CANRMP.all = 0xFFFFFFFF;
    ECanbRegs.CANRML.all = 0xFFFFFFFF;
    ECanbRegs.CANRFP.all = 0xFFFFFFFF;
    ECanbRegs.CANES.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF1.all = 0xFFFFFFFF;
    ECanbRegs.CANMIL.all = 0x00000000;  //邮箱4用线0，邮箱0用线0
    ECanbRegs.CANOPC.all = 0xFFFFFFFF;  // 所有接收邮箱不能覆盖
    ECanbRegs.CANTOC.all = 0;  // 禁用超时功能
    ECanbRegs.CANTOS.all = 0xFFFFFFFF;  // 清除超时状态
    
    // 配置邮箱0（发送邮箱）
    EALLOW;
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CDR = 1;  // 设置CDR以访问邮箱数据
    ECanbShadow.CANMC.bit.MBNR = 0; // 邮箱0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    // 初始化邮箱0
    ECanbMboxes.MBOX0.MSGID.all = 0;
    // 设置标准ID
    ECanbMboxes.MBOX0.MSGID.bit.STDMSGID = 1;
    // 标准帧(IDE=0)，发送邮箱不使用屏蔽(AME=0)，非自动应答(AAM=0)
    ECanbMboxes.MBOX0.MSGCTRL.all = 0;
    ECanbMboxes.MBOX0.MSGCTRL.bit.TPL = 31;  // 最高发送优先级
    ECanbMboxes.MBOX0.MSGCTRL.bit.DLC = 8;   // 数据长度8字节
    ECanbMboxes.MBOX0.MDH.all = 0;
    ECanbMboxes.MBOX0.MDL.all = 0;
    
    // 配置邮箱4（接收邮箱）
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CDR = 1;
    ECanbShadow.CANMC.bit.MBNR = 4;  // 邮箱4
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    // 初始化邮箱4
    ECanbMboxes.MBOX4.MSGID.all = 0;
    ECanbMboxes.MBOX4.MSGID.bit.AME = 1;  // 使能接收屏蔽
    ECanbMboxes.MBOX4.MSGID.bit.STDMSGID = 0;
    ECanbMboxes.MBOX4.MSGCTRL.all = 0;
    ECanbMboxes.MBOX4.MSGCTRL.bit.TPL = 27;  // 接收优先级
    ECanbMboxes.MBOX4.MSGCTRL.bit.DLC = 8;   // 数据长度8字节
    ECanbMboxes.MBOX4.MDH.all = 0;
    ECanbMboxes.MBOX4.MDL.all = 0;
    
    // 配置邮箱4的接收屏蔽(LAM4)
    ECanbLAMRegs.LAM4.bit.LAMI = 0;       // 标准帧过滤
    ECanbLAMRegs.LAM4.bit.LAM_H = 0xFFFF;
    ECanbLAMRegs.LAM4.bit.LAM_L = 0xFFFF;
    
    // 清除CDR并使能邮箱
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CDR = 0;    // 清除CDR
    ECanbShadow.CANMC.bit.MBNR = 0;   // 邮箱编号清零
    ECanbShadow.CANMC.bit.SRES = 0;   // 清除软件复位标志
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    // 使能邮箱0,4
    ECanbRegs.CANME.all = 0x00000011;  // 使能邮箱0,4 (bit0,4=1)
    
    EDIS;
}

// 中断初始化
static void InitCanaInterrupt(void)
{
    // 配置全局中断屏蔽
    EALLOW;
    
    // 禁用所有中断线
    ECanaRegs.CANGIM.all = 0x00000000;

    // 配置邮箱中断屏蔽 - 使能邮箱0,4中断
    ECanaRegs.CANMIM.all = 0x00000011;
    
    // 配置中断线分配：
    ECanaRegs.CANMIL.all = 0x00000000;

    // 配置全局中断屏蔽寄存器
    ECanaRegs.CANGIM.all = 0x00003807;
    /*
    bit0: I0EN=1 - 使能中断线0 (接收)
    bit1: I1EN=1 - 使能中断线1 (发送)
    bit2: GIL=0  - 全局中断
    bit3: 保留

    bit9: EPIM=1 - 使能错误被动中断
    bit10: BOIM=1 - 使能总线关闭中断
    */
    // 时间戳计数器清零
    ECanaRegs.CANTSC = 0;
    EDIS;
    
    // 配置PIE中断向量表
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    
    EALLOW;
    PieVectTable.ECAN0INTA = &CANA_TxRx_ISR;   // 中断线0
    PieVectTable.ECAN1INTA = &CANA_Error_ISR;   // 中断线1
    EDIS;
    
    // 使能PIE中断
    IER |= M_INT9;  // CAN中断在INT9组
    
    // 使能PIE组9的特定中断
    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;  // ECAN0INTA (中断线0 - 接收)
    PieCtrlRegs.PIEIER9.bit.INTx6 = 1;  // ECAN1INTA (中断线1 - 发送)
    
    // 全局使能中断
    EINT;  // 使能全局中断
    ERTM;  // 使能实时调试中断
}

// 中断初始化
static void InitCanbInterrupt(void)
{
    // 配置全局中断屏蔽
    EALLOW;
    
    // 禁用所有中断线
    ECanbRegs.CANGIM.all = 0x00000000;

    // 配置邮箱中断屏蔽 - 使能邮箱0,4中断
    ECanbRegs.CANMIM.all = 0x00000011;
    
    // 配置中断线分配：
    ECanbRegs.CANMIL.all = 0x00000000;

    // 配置全局中断屏蔽寄存器
    ECanbRegs.CANGIM.all = 0x00003807;
    /*
    bit0: I0EN=1 - 使能中断线0 (接收)
    bit1: I1EN=1 - 使能中断线1 (发送)
    bit2: GIL=0  - 全局中断
    bit3: 保留

    bit9: EPIM=1 - 使能错误被动中断
    bit10: BOIM=1 - 使能总线关闭中断
    */
    // 时间戳计数器清零
    ECanbRegs.CANTSC = 0;
    EDIS;
    
    // 配置PIE中断向量表
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    
    EALLOW;
    PieVectTable.ECAN0INTB = &CANB_TxRx_ISR;   // 中断线0
    PieVectTable.ECAN1INTB = &CANB_Error_ISR;   // 中断线1
    EDIS;
    
    // 使能PIE中断
    IER |= M_INT9;  // CAN中断在INT9组
    
    // 使能PIE组9的特定中断
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  // ECAN0INTB (中断线0 - 接收)
    PieCtrlRegs.PIEIER9.bit.INTx8 = 1;  // ECAN1INTB (中断线1 - 发送)
    
    // 全局使能中断
    EINT;  // 使能全局中断
    ERTM;  // 使能实时调试中断
}

// 发送函数
static void CanaSendDrive(uint16_t Mb, S_CANMSG *s_cTx)
{
    struct ECAN_REGS ECanaShadow;
    
    // 邮箱号检查
    if (Mb > 31)
    {
        return;
    }
    
    // 检查是否是发送邮箱
    if (ECanaRegs.CANMD.all & (1 << Mb))
    {
        return;  // 该邮箱是接收邮箱，不能发送
    }
    
    // 检查是否正在发送
    ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
    if (ECanaShadow.CANTA.bit.TA0 == 1)
    {
        // 清除发送完成标志
        ECanaShadow.CANTA.bit.TA0 = 1;
        ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
    }
    
    // 禁用邮箱准备更新数据
    EALLOW;
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.all &= ~(1 << Mb);  // 禁用邮箱0
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;
    
    // 设置CDR以访问邮箱
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CDR = 1;
    ECanaShadow.CANMC.bit.MBNR = Mb;  // 设置邮箱编号
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    // 配置消息ID和控制字

    volatile struct MBOX *mbox = &((volatile struct MBOX*)&ECanaMboxes)[Mb];
    mbox->MSGID.all = 0;
    mbox->MSGID.bit.STDMSGID = s_cTx->rid.bit.idl & 0x7FF;  // 设置标准ID
    mbox->MSGCTRL.bit.RTR = 0;         // 设置RTR位
    mbox->MSGCTRL.bit.DLC = s_cTx->dlc;              // 设置数据长度
    
    // 写入数据
    mbox->MDL.word.LOW_WORD = ((uint16_t)s_cTx->data[1] << 8) | s_cTx->data[0];
    mbox->MDL.word.HI_WORD = ((uint16_t)s_cTx->data[3] << 8) | s_cTx->data[2];
    mbox->MDH.word.LOW_WORD = ((uint16_t)s_cTx->data[5] << 8) | s_cTx->data[4];
    mbox->MDH.word.HI_WORD = ((uint16_t)s_cTx->data[7] << 8) | s_cTx->data[6];
    
    // 清除CDR
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CDR = 0;
    ECanaShadow.CANMC.bit.MBNR = 0;
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    // 使能邮箱
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.all |= (1 << Mb);  // 使能邮箱0
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;
    
    // 启动发送
    ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
    ECanaShadow.CANTRS.all |= (1 << Mb);  // 设置发送请求
    ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
    
    EDIS;
}

// 发送函数
static void CanbSendDrive(uint16_t Mb, S_CANMSG *s_cTx)
{
    struct ECAN_REGS ECanbShadow;
    
    // 邮箱号检查
    if (Mb > 31)
    {
        return;
    }
    
    // 检查是否是发送邮箱
    if (ECanbRegs.CANMD.all & (1 << Mb))
    {
        return;  // 该邮箱是接收邮箱，不能发送
    }
    
    // 检查是否正在发送
    ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    if (ECanbShadow.CANTA.bit.TA0 == 1)
    {
        // 清除发送完成标志
        ECanbShadow.CANTA.bit.TA0 = 1;
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
    }
    
    // 禁用邮箱准备更新数据
    EALLOW;
    ECanbShadow.CANME.all = ECanbRegs.CANME.all;
    ECanbShadow.CANME.all &= ~(1 << Mb);  // 禁用邮箱0
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
    
    // 设置CDR以访问邮箱
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CDR = 1;
    ECanbShadow.CANMC.bit.MBNR = Mb;  // 设置邮箱编号
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    // 配置消息ID和控制字

    volatile struct MBOX *mbox = &((volatile struct MBOX*)&ECanbMboxes)[Mb];
    mbox->MSGID.all = 0;
    mbox->MSGID.bit.STDMSGID = s_cTx->rid.bit.idl & 0x7FF;  // 设置标准ID
    mbox->MSGCTRL.bit.RTR = 0;         // 设置RTR位
    mbox->MSGCTRL.bit.DLC = s_cTx->dlc;              // 设置数据长度
    
    // 写入数据
    mbox->MDL.word.LOW_WORD = ((uint16_t)s_cTx->data[1] << 8) | s_cTx->data[0];
    mbox->MDL.word.HI_WORD = ((uint16_t)s_cTx->data[3] << 8) | s_cTx->data[2];
    mbox->MDH.word.LOW_WORD = ((uint16_t)s_cTx->data[5] << 8) | s_cTx->data[4];
    mbox->MDH.word.HI_WORD = ((uint16_t)s_cTx->data[7] << 8) | s_cTx->data[6];
    
    // 清除CDR
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CDR = 0;
    ECanbShadow.CANMC.bit.MBNR = 0;
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    // 使能邮箱
    ECanbShadow.CANME.all = ECanbRegs.CANME.all;
    ECanbShadow.CANME.all |= (1 << Mb);  // 使能邮箱0
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
    
    // 启动发送
    ECanbShadow.CANTRS.all = ECanbRegs.CANTRS.all;
    ECanbShadow.CANTRS.all |= (1 << Mb);  // 设置发送请求
    ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    
    EDIS;
}

// 接收函数
static void CanaRecvDrive(uint16_t Mb, S_CANMSG *s_cRx)
{
    struct ECAN_REGS ECanaShadow;
    
    // 邮箱号检查
    if (Mb > 31) {
        return;
    }
    
    // 检查是否有接收消息
    ECanaShadow.CANRMP.all = ECanaRegs.CANRMP.all;
    if (!(ECanaShadow.CANRMP.all & (1 << Mb))) {
        return;  // 该邮箱没有接收消息
    }
    
    // 清除接收标志
    ECanaShadow.CANRMP.bit.RMP4 = 1;  // 清除邮箱4的接收标志
    ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;
    
    // 清除远程帧标志
    ECanaShadow.CANRFP.all = ECanaRegs.CANRFP.all;
    ECanaShadow.CANRFP.bit.RFP4 = 1;  // 清除邮箱4的远程帧标志
    ECanaRegs.CANRFP.all = ECanaShadow.CANRFP.all;
    
    // 读取消息信息
    volatile struct MBOX *mbox = &((volatile struct MBOX*)&ECanaMboxes)[Mb];
    
    // 如果是数据帧(RTR=0)，读取数据
    if (mbox->MSGCTRL.bit.RTR == 0)
    {
        uint16_t low_word = mbox->MDL.word.LOW_WORD;
        uint16_t hi_word = mbox->MDL.word.HI_WORD;
        uint16_t low_word_h = mbox->MDH.word.LOW_WORD;
        uint16_t hi_word_h = mbox->MDH.word.HI_WORD;
        
        // 转换为字节数组
        s_cRx->rid.bit.idl = mbox->MSGID.bit.STDMSGID;
        s_cRx->dlc = mbox->MSGCTRL.bit.DLC;
        s_cRx->data[0] = low_word & 0xFF;
        s_cRx->data[1] = (low_word >> 8) & 0xFF;
        s_cRx->data[2] = hi_word & 0xFF;
        s_cRx->data[3] = (hi_word >> 8) & 0xFF;
        s_cRx->data[4] = low_word_h & 0xFF;
        s_cRx->data[5] = (low_word_h >> 8) & 0xFF;
        s_cRx->data[6] = hi_word_h & 0xFF;
        s_cRx->data[7] = (hi_word_h >> 8) & 0xFF;
    } else {
        // 远程帧，数据清零
        memset(s_cRx->data, 0, 8);
    }
    
    s_cRx->frmChn = 0xAAAA;
}

// 接收函数
static void CanbRecvDrive(uint16_t Mb, S_CANMSG *s_cRx)
{
    struct ECAN_REGS ECanbShadow;
    
    // 邮箱号检查
    if (Mb > 31) {
        return;
    }
    
    // 检查是否有接收消息
    ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
    if (!(ECanbShadow.CANRMP.all & (1 << Mb))) {
        return;  // 该邮箱没有接收消息
    }
    
    // 清除接收标志
    ECanbShadow.CANRMP.bit.RMP4 = 1;  // 清除邮箱4的接收标志
    ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
    
    // 清除远程帧标志
    ECanbShadow.CANRFP.all = ECanbRegs.CANRFP.all;
    ECanbShadow.CANRFP.bit.RFP4 = 1;  // 清除邮箱4的远程帧标志
    ECanbRegs.CANRFP.all = ECanbShadow.CANRFP.all;
    
    // 读取消息信息
    volatile struct MBOX *mbox = &((volatile struct MBOX*)&ECanbMboxes)[Mb];
    
    // 如果是数据帧(RTR=0)，读取数据
    if (mbox->MSGCTRL.bit.RTR == 0)
    {
        uint16_t low_word = mbox->MDL.word.LOW_WORD;
        uint16_t hi_word = mbox->MDL.word.HI_WORD;
        uint16_t low_word_h = mbox->MDH.word.LOW_WORD;
        uint16_t hi_word_h = mbox->MDH.word.HI_WORD;
        
        // 转换为字节数组
        s_cRx->rid.bit.idl = mbox->MSGID.bit.STDMSGID;
        s_cRx->dlc = mbox->MSGCTRL.bit.DLC;
        s_cRx->data[0] = low_word & 0xFF;
        s_cRx->data[1] = (low_word >> 8) & 0xFF;
        s_cRx->data[2] = hi_word & 0xFF;
        s_cRx->data[3] = (hi_word >> 8) & 0xFF;
        s_cRx->data[4] = low_word_h & 0xFF;
        s_cRx->data[5] = (low_word_h >> 8) & 0xFF;
        s_cRx->data[6] = hi_word_h & 0xFF;
        s_cRx->data[7] = (hi_word_h >> 8) & 0xFF;
    } else {
        // 远程帧，数据清零
        memset(s_cRx->data, 0, 8);
    }
    
    s_cRx->frmChn = 0xAAAA;
}

// 检查邮箱接收状态
bool CanaCheckRxStatus(uint16_t mailbox)
{
    return (ECanaRegs.CANRMP.all & (1 << mailbox)) != 0;
}

// 检查邮箱接收状态
bool CanbCheckRxStatus(uint16_t mailbox)
{
    return (ECanbRegs.CANRMP.all & (1 << mailbox)) != 0;
}

// 清除所有中断标志
void CanaClearAllFlags(void)
{
    EALLOW;
    ECanaRegs.CANTA.all = 0xFFFFFFFF;
    ECanaRegs.CANRMP.all = 0xFFFFFFFF;
    ECanaRegs.CANAA.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
    EDIS;
}

// 清除所有中断标志
void CanbClearAllFlags(void)
{
    EALLOW;
    ECanbRegs.CANTA.all = 0xFFFFFFFF;
    ECanbRegs.CANRMP.all = 0xFFFFFFFF;
    ECanbRegs.CANAA.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF0.all = 0xFFFFFFFF;
    ECanbRegs.CANGIF1.all = 0xFFFFFFFF;
    EDIS;
}
