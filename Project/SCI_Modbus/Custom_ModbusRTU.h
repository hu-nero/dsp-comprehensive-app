#ifndef _CUSTOM_MODBUSRTU_H_
#define _CUSTOM_MODBUSRTU_H_

//#include "Main\Variable_CPU01.H"
#include "RTU_ParaDefine.h"

enum DeviceData_t
{
	Little_endian = (u8)0x00,
	Big_endian    = (u8)0x01,
};

#define SlaveDeviceAddr   0x01
#define SlaveDeviceData   Little_endian

enum ModbusFuncCode_t
{
	Code01_RCoilReg  = (u8)0x01,
	Code02_RDiscReg  = (u8)0x02,
	Code03_RHoldReg  = (u8)0x03,
	Code04_RInputReg = (u8)0x04,
	Code05_WCoilReg  = (u8)0x05,
	Code06_WHoldReg  = (u8)0x06,
	Code0F_WCoilRegs = (u8)0x0F,
	Code10_WHoldRegs = (u8)0x10,
};

extern int InputRegBuf[32];
extern int HoldRegBuf[50];
//////////////////////////////////////////////////////////////////////////
extern void CRC_Check(u16 *pCrcBuf, u16 LEN, u16 *CRCByteH, u16 *CRCByteL);
extern void ModbusRTU_Protocol(u16 *pRxBuffer, u16 *pFront, u16 nRear, u16 RevSize, u16 *pTxBuffer);

extern void Func_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);
extern void Func_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);

extern void Modbus_Send_To_HMI(void);
extern void Modbus_Recv_For_HMI(void);
//////////////////////////////////////////////////////////////////////////
#endif

