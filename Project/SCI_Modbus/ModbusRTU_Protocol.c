#include "Custom_ModbusRTU.h"
#include "main\Variable.h"
#include "SCI_Modbus\Sci_Modbus_config.H"


void Func_DiscReg(u8 FuncCode, u8 *pMesBuf, u8 *pTxBuffer);
void Func_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);
void Func_CoilReg(u8 FuncCode, u8 *pMesBuf, u8 *pTxBuffer);
void Func_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer);
/**********************************************************************************/
void CRC_Check(u16 *pCrcBuf, u16 LEN, u16 *CRCByteH, u16 *CRCByteL)
{
	u16 CRCRetn;
	u16  i, j;

	CRCRetn = 0xFFFF;
	for(i=0; i<LEN; i++)
	{
		CRCRetn ^= *(pCrcBuf + i);
		for(j=0; j<8; j++)
		{
			if(CRCRetn & 0x0001)
				CRCRetn = (CRCRetn >> 1) ^ 0xA001;
			else
				CRCRetn = (CRCRetn >> 1);
		}
	}
	CRCRetn = ((CRCRetn >> 8) + (CRCRetn << 8));

	if(SlaveDeviceData == Little_endian)
	{
		*CRCByteH = (CRCRetn>>8) & 0xFF;
		*CRCByteL = CRCRetn & 0xFF;
	}
	else
	{
		*CRCByteL = (CRCRetn>>8) & 0xFF;
		*CRCByteH = CRCRetn & 0xFF;
	}
}
/**********************************************************************************/

void Func_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer)
{
	u8 i, j, ucTempCnt;
	u16 usStartAddr, usReadLen;

	usStartAddr = ((u16)pMesBuf[2]<<8 ) | pMesBuf[3];
	usReadLen = ((u16)pMesBuf[4]<<8 ) | pMesBuf[5];

	// [cmd] SavleAddr(1byte) + Funcode(1byte) + RegsAddr(2byte) + RegsNum(2byte) + CrcCheck(2byte)
	// [ack] SavleAddr(1byte) + Funcode(1byte) + ByteNum(1byte) + RegsData(n*2byte) + CrcCheck(2byte)

	if(FuncCode == Code04_RInputReg)
	{
		ucTempCnt = 3;

		for(i=0; i<usReadLen; i++)
		{
			for(j=1; j<(sInputReg_Group[0].RegAddrNum + 1); j++)
			{
				if( usStartAddr == (sInputReg_Group[j].RegAddrNum) )
				{
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(sInputReg_Group[j].Obj)) >>8) & 0x00FF; //dataH
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(sInputReg_Group[j].Obj))    ) & 0x00FF; //dataL
					break;
				}

				if(j == sInputReg_Group[0].RegAddrNum)
				{
					if( usStartAddr != (sInputReg_Group[j].RegAddrNum) )
					{
						pTxBuffer[ucTempCnt++] = 0x0000; //dataH
						pTxBuffer[ucTempCnt++] = 0x0000; //dataL
					}
				}
			}

			usStartAddr ++;
		}

		ucTempCnt = usReadLen*2;
		pTxBuffer[0] = SlaveDeviceAddr;
		pTxBuffer[1] = Code04_RInputReg;
		pTxBuffer[2] = ucTempCnt; //*2
		CRC_Check(pTxBuffer, (ucTempCnt+3), pTxBuffer+ucTempCnt+3, pTxBuffer+ucTempCnt+4);
		Sci_Modbus_SendArray(&SciaRegs, pTxBuffer, ucTempCnt+5);
	}
}
/**********************************************************************************/
void Func_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer)
{
	u8 i, j, ucTempCnt;
	u16 usStartAddr, usReadLen;
	u16 usTempData;

	usStartAddr = ((u16)pMesBuf[2]<<8 ) | pMesBuf[3];
	usReadLen = ((u16)pMesBuf[4]<<8 ) | pMesBuf[5];

	// [cmd] SavleAddr(1byte) + Funcode(1byte) + RegsAddr(2byte) + RegsNum(2byte) + CrcCheck(2byte)
	// [ack] SavleAddr(1byte) + Funcode(1byte) + ByteNum(1byte) + RegsData(n*2byte) + CrcCheck(2byte)
	if(FuncCode == Code03_RHoldReg)
	{
		ucTempCnt = 3;

		for(i=0; i<usReadLen; i++)
		{
			for(j=1; j<(sHoldReg_Group[0].RegAddrNum + 1); j++)
			{
				if( usStartAddr == (sHoldReg_Group[j].RegAddrNum) )
				{
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(sHoldReg_Group[j].RegObj)) >>8) & 0x00FF; //dataH
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(sHoldReg_Group[j].RegObj))    ) & 0x00FF; //dataL
					break;
				}

				if(j == sHoldReg_Group[0].RegAddrNum)
				{
					if( usStartAddr != (sHoldReg_Group[j].RegAddrNum) )
					{
						pTxBuffer[ucTempCnt++] = 0x0000; //dataH
						pTxBuffer[ucTempCnt++] = 0x0000; //dataL
					}
				}
			}

			usStartAddr ++;
		}

		ucTempCnt = usReadLen*2;
		pTxBuffer[0] = SlaveDeviceAddr;        // SavleAddr(1byte)
		pTxBuffer[1] = Code03_RHoldReg;  // FunsCode(1byte)
		pTxBuffer[2] = ucTempCnt;        // ByteNum(1byte)
		CRC_Check(pTxBuffer, (ucTempCnt+3), pTxBuffer+ucTempCnt+3, pTxBuffer+ucTempCnt+4);
		Sci_Modbus_SendArray(&SciaRegs, pTxBuffer, ucTempCnt+5);
	}

	else if(FuncCode == Code06_WHoldReg)
	{
		usTempData = ((pMesBuf[4] << 8) & 0xFF00)  | (pMesBuf[5] & 0x00FF);

		for(j=1; j<(sHoldReg_Group[0].RegAddrNum + 1); j++)
		{
			if( usStartAddr == (sHoldReg_Group[j].RegAddrNum) )
			{
				*(u16 *)sHoldReg_Group[j].RegObj = usTempData;
				break;
			}
		}
		// Slave begin ack
		pTxBuffer[0] = SlaveDeviceAddr;
		pTxBuffer[1] = Code06_WHoldReg;
		pTxBuffer[2] = pMesBuf[2];
		pTxBuffer[3] = pMesBuf[3];
		pTxBuffer[4] = pMesBuf[4];
		pTxBuffer[5] = pMesBuf[5];
		CRC_Check(pTxBuffer, 6, pTxBuffer+6, pTxBuffer+7);

		Sci_Modbus_SendArray(&SciaRegs, pTxBuffer, 8);
	}
	else if(FuncCode == Code10_WHoldRegs)
	{

	}
}




