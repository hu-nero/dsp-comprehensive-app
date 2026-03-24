#ifndef _CUSTOM_DCS_H_
#define _CUSTOM_DCS_H_

#include "Main\Variable.h"
#include "DCS_ParaDefine.h"
#include "SCI_DCS\Sci_DCS_Config.h"

extern int DcsInputRegBuf[64];
extern int DcsHoldRegBuf[64];

#define DcsDeviceAddr   0x01

#include "SCI_Modbus\Custom_ModbusRTU.H"
//////////////////////////////////////////////////////////////////////////
extern void CRC_Check_DCS(u16 *pCrcBuf, u16 LEN, u16 *CRCByteH, u16 *CRCByteL);
extern void ModbusDcs_Protocol(u16 *pRxBuffer, u16 *pFront, u16 nRear, u16 RevSize, u16 *pTxBuffer);

extern void Modbus_Send_To_Dcs(void);
extern void Modbus_Recv_For_Dcs(void);

extern void DCS_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);
extern void DCS_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);

//////////////////////////////////////////////////////////////////////////
#endif
