
#ifndef _SCICONFIG_H
#define _SCICONFIG_H


#include "DSP2833x_Device.h"
#define SCICFG_EXT extern


/*
*********************************************************************************************************
*                                SCI_BAUD define
*********************************************************************************************************
*/

#define BAUD_19200  0x00f3 
#define BAUD_38400  0x0079     
#define BAUD_57600  0x0050        
#define BAUD_115200 0x0028
#define BAUD_230400 0x0014
#define BAUD_460800	0x0009
#define BAUD_921600 0x0004


#define SCI_RX_QUE_BUFF_SIZE 512   //接收缓冲区byte
#define SCI_TX_QUE_BUFF_SIZE 1024  //发送缓冲区word


SCICFG_EXT void Sci_Monitor_init(volatile struct SCI_REGS *p);
SCICFG_EXT void Sci_Monitor_RxData(volatile struct SCI_REGS *p,Uint16 *pRxBuffer,Uint16 *pRear);
SCICFG_EXT void Sci_Monitor_TxData(volatile struct SCI_REGS *p,Uint16 *pTxBuffer,Uint16 *pFront,Uint16 nRear,Uint16 nTimeFlag);



#endif

//===========================================================================
// End of file.
//===========================================================================
