#ifndef _CUSTOM_REMOTE_H_
#define _CUSTOM_REMOTE_H_

#include "Main\Variable.h"
#include "Remote_ParaDefine.h"
#include "SCI_ModbusM\Sci_Remote_Config.h"

extern int RemInputRegBuf[64];
extern int RemHoldRegBuf[64];

#define RemDeviceAddr   0x01

#include "SCI_ModbusS\Custom_ModbusRTU.H"
//////////////////////////////////////////////////////////////////////////
extern void CRC_Check_Romote(u16 *pCrcBuf, u16 LEN, u16 *CRCByteH, u16 *CRCByteL);
extern void ModbusRem_Protocol(u16 *pRxBuffer, u16 *pFront, u16 nRear, u16 RevSize, u16 *pTxBuffer);

extern void Modbus_Send_To_Rem(void);
extern void Modbus_Recv_For_Rem(void);

extern void Remote_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);
extern void Remote_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);

//////////////////////////////////////////////////////////////////////////
#endif
