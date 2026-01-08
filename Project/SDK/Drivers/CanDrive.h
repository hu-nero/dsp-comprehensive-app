#ifndef __CANDRIVE_H_
#define __CANDRIVE_H_

#include "DSP28x_Project.h"
#include "System\Custom_System.H"
#include <stdint.h>

// CAN通道 宏定义
#define CANCHNA (0xaa)					// CANA 应用层通道
#define CANCHNB (0xbb)					// CANB 应用层通道

// CAN邮箱收发的底层函数返回值
#define TRUE	(0x0)					// CAN 发送正确
#define FALSE	(0x1)					// CAN 发送错误

// CAN时钟宏定义(基于PLL和分频器配置) MHz
#define CANSYSCLK	(30 * DSP28_PLLCR / DSP28_DIVSEL / 2)

// 错误状态定义
#define CAN_STATUS_OK         0x00
#define CAN_STATUS_WARNING    0x01
#define CAN_STATUS_ERROR_PASSIVE 0x02
#define CAN_STATUS_BUS_OFF    0x04
#define CAN_STATUS_ACK_ERROR  0x08
#define CAN_STATUS_FORM_ERROR 0x10
#define CAN_STATUS_CRC_ERROR  0x20
#define CAN_STATUS_BIT_ERROR  0x40
#define CAN_STATUS_STUFF_ERROR 0x80


// 波特率配置结构
typedef struct {
    uint16_t brp_reg;
    uint16_t tseg1_reg;
    uint16_t tseg2_reg;
    uint16_t sjw_reg;
    uint16_t sam;
} CAN_BitTiming_t;

// Can 底层 结构体
struct  CAN_ID
{
   Uint16     idl:16;    // idl
   Uint16     idh:13;    // idh
   Uint16 	  aamRes:1;  // bit29
   Uint16     ameRes:1;  // bit30
   Uint16     ideRes:1;  // bit31 PS:已经在发送函数中置位
};
union UCAN_ID
{
   Uint32     all;
   struct CAN_ID  bit;
};
typedef struct _S_CANMSG
{
	union UCAN_ID rid; // 消息ID:0-28位有效
	Uint16 *data;    // 数据:BYTE0-BYTE7
	Uint16 dlc;        // 数据长度:最多8个字节
	Uint16 frmChn;     // 帧通道:cana/canb
}S_CANMSG;

// Can 应用层 结构体
typedef struct _Can_TypeDef
{
	Uint16 CanaRecvData[8];	// CANA 接收数据
	Uint16 CanbRecvData[8];	// CANB 接收数据
	Uint16 CanaSendData[8];	// CANA 发送数据
	Uint16 CanbSendData[8];	// CANB 发送数据
	Uint16 MsgTypeA;	// CAN A通道消息类型
	Uint16 MsgTypeB;	// CAN B通道消息类型
	Uint16 MsgChn;		// 当前消息通道:0xbbbb-canb通路,0xaaaa-cana通路
}Can_TypeDef;

// 初始化
uint16_t InitCanDrive(Can_TypeDef *Can_Struct, uint16_t Bps);		// Can模块初始化
uint16_t DeinitCanDrive(Can_TypeDef *Can_Struct);
extern void InitECanaGpio(void);
extern void InitECanbGpio(void);

// 接收发送
uint16_t CanRecvDrive(Can_TypeDef *Can_Struct, uint16_t Mb, S_CANMSG *s_cRx);		// CANA 接收
uint16_t CanSendDrive(Can_TypeDef *Can_Struct, uint16_t Mb, S_CANMSG *s_cTx);				// CAN发送

extern interrupt void CANA_TxRx_ISR(void);
extern interrupt void CANA_Error_ISR(void);

#endif
