
#ifndef _MESSAGE_PARSE_H
#define _MESSAGE_PARSE_H


#define SCI_EXT extern
#include "DSP2833x_Device.h"

/*
*********************************************************************************************************
*                              通信协议解析之状态返回值
*********************************************************************************************************
*/
/*宏定义信息*/
#define RETN_PARA_OPARATION                  0x0000  //操作成功 
#define RETN_ADDR_ERROR_MACRO 	             0x0001  //通讯节点错误
#define RETN_CRC_ERROR_MACRO 	               0x0002  //CRC校验错误 
#define RETN_NO_PARA_OPARATION 	             0x000F  //帧头错误 
#define RETN_HARD_ERROR_MACRO	               0x0003  //参shu类型错误 
#define RETN_PARA_TPYE_ERROR_MACRO	         0x0004  //时间计shu器溢出 
#define RETN_MSG_ERR_MACRO		               0x0005  //发送队列满 
#define RETN_TX_QUE_FULL_ERR_MACRO		       0x0006  //接收队列满 
#define RETN_RXD_QUE_FULL_MACRO	             0x0007  //下位机参shu属性为不可写 
#define RETN_PARA_NOT_WRITE_IN_LOCAL_MACRO	 0x0008  //shu据类型错误 
#define RETN_DATA_TYPE_ERROR_MACRO	         0x0009  //ID非法错误 
#define RETN_ILLEGAL_ID_MACRO	               0x000A  //参shu组号无效错误 
#define RETN_ILLEGAL_GROUP_NUM_MACRO         0x000B  //报文不完整
#define RETN_MSG_NOT_ENOUGH_MACRO            0x000C  //参shu写RAM失败
#define RETN_PARA_WRITE_TO_RAM_EER_MACRO     0x000D  //参shu读RAM失败
#define RETN_PARA_READ_FROM_RAM_EER_MACRO    0x000E  //无用信息
#define RETN_NO_USE_MACRO                    0xFFFF
#define RETN_SUCCESS_MACRO 	                 0x0010  //操作成功
#define RETN_EVENT_QUEUE_FULL_MACRO 	       0x0011  //事件队列满
#define RETN_EVENT_QUEUE_EMPTY_MACRO 	       0x0012  //事件队列空
#define RETN_ILLEGAL_ADDR_MACRO              0x0013  //参数地址错误 
#define RETN_EEPROM_OPRERATION_DONE          0x0014  //eeprom读写已完成 
#define RETN_EEPROM_OPRERATION_DOING         0x0015  //eeprom正在进行读写
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NO_DATA_MSG_LENGHTH  12 //加上校验位
#define SCI_RXD_BUFF_SIZE    52 //接收报文缓冲区，传递到指令具体执行
#define SCI_SEND_BUFF_SIZE   133 //发送报文缓冲区,将指令参数传递到发送缓冲区
/*
*********************************************************************************************************
*                                  函数声明
*********************************************************************************************************
*/  

SCI_EXT Uint16 Monitor_Rx_Protocol(Uint16 *pRxBuffer,Uint16 *pFront,Uint16 nRear,Uint16 *pTxBuffer,Uint16 *pTxTail);
SCI_EXT Uint16 Monitor_Tx_Protocol(Uint16 *pTxBuffer,Uint16 *pRear,Uint32 lFsn,int16 nlen,Uint16 *pBuf,Uint16 uiIDTmp,Uint16 uiSciCtrlWordTmp);
extern const int MessageParse_Ver[3];


#endif

//===========================================================================
// No more.
//===========================================================================
