#include "Custom_DCS.h"
#include "Main\Variable.H"
#include "SCI_DCS\Sci_DCS_config.H"

/**********************************************************************************/
void CRC_Check_DCS(u16 *pCrcBuf, u16 LEN, u16 *CRCByteH, u16 *CRCByteL)
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

void DCS_InputReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer)
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

		for(i=0; i<(RemInputReg_Group[0].RegAddrNum + 1); i++)
		{
			if( usStartAddr == (RemInputReg_Group[i].RegAddrNum) )
			{
				for(j=0; j<usReadLen; j++)
				{
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(RemInputReg_Group[i+j].Obj)) >>8) & 0x00FF; //dataH
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(RemInputReg_Group[i+j].Obj)) >>0) & 0x00FF; //dataL
				}
				break;
			}
		}

		ucTempCnt = usReadLen*2;
		pTxBuffer[0] = DcsDeviceAddr;
		pTxBuffer[1] = Code04_RInputReg;
		pTxBuffer[2] = ucTempCnt; //*2
		CRC_Check_DCS(pTxBuffer, (ucTempCnt+3), pTxBuffer+ucTempCnt+3, pTxBuffer+ucTempCnt+4);
		Sci_DCS_SendArray(&SciaRegs, pTxBuffer, ucTempCnt+5);
	}
}
/**********************************************************************************/
void DCS_HoldReg(u8 FuncCode, u16 *pMesBuf, u16 *pTxBuffer)
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

		for(i=0; i<(RemHoldReg_Group[0].RegAddrNum + 1); i++)
		{
			if( usStartAddr == (RemHoldReg_Group[i].RegAddrNum) )
			{
				for(j=0; j<usReadLen; j++)
				{
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(RemHoldReg_Group[i+j].RegObj)) >>8) & 0x00FF; //dataH
					pTxBuffer[ucTempCnt++] = ((*(u16 *)(RemHoldReg_Group[i+j].RegObj)) >>0) & 0x00FF; //dataL
				}
				break;
			}
		}

		ucTempCnt = usReadLen*2;
		pTxBuffer[0] = DcsDeviceAddr;		// SavleAddr(1byte)
		pTxBuffer[1] = Code03_RHoldReg;  	// FunsCode(1byte)
		pTxBuffer[2] = ucTempCnt;       	// ByteNum(1byte)
		CRC_Check_DCS(pTxBuffer, (ucTempCnt+3), pTxBuffer+ucTempCnt+3, pTxBuffer+ucTempCnt+4);
		Sci_DCS_SendArray(&SciaRegs, pTxBuffer, ucTempCnt+5);
	}

	else if(FuncCode == Code06_WHoldReg)
	{
		usTempData = ((pMesBuf[4] << 8) & 0xFF00)  | (pMesBuf[5] & 0x00FF);

		for(j=1; j<(RemHoldReg_Group[0].RegAddrNum + 1); j++)
		{
			if( usStartAddr == (RemHoldReg_Group[j].RegAddrNum) )
			{
				*(u16 *)RemHoldReg_Group[j].RegObj = usTempData;
				break;
			}
		}
		// Slave begin ack
		pTxBuffer[0] = DcsDeviceAddr;
		pTxBuffer[1] = Code06_WHoldReg;
		pTxBuffer[2] = pMesBuf[2];
		pTxBuffer[3] = pMesBuf[3];
		pTxBuffer[4] = pMesBuf[4];
		pTxBuffer[5] = pMesBuf[5];
		CRC_Check_DCS(pTxBuffer, 6, pTxBuffer+6, pTxBuffer+7);

		Sci_DCS_SendArray(&SciaRegs, pTxBuffer, 8);
	}
	else if(FuncCode == Code10_WHoldRegs)
	{

	}
}




