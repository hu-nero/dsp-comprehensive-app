/***************************************************
 * @brief   modbus
 * @date    2024/04/09
 * @author  Cheng peng
****************************************************/

#include "Custom_Remote.h"

u16  ucTempBuf[128];
void ModbusRem_Protocol(u16 *pRxBuffer, u16 *pFront, u16 nRear, u16 RevSize, u16 *pTxBuffer)
{
	u16 ucTempCnt, ucTempCnt1;
	u16 i;
	u8 FuncCode;
	int uiLength,usTempDataLen;
	u16 ucCrcH, ucCrcL;

	//Step1: Count recv data length form USART
	if(nRear != *pFront)
	{
		uiLength = nRear - *pFront;
		if(uiLength < 0)
			uiLength += RevSize;
	}
	else
		uiLength = 0;

	//Step2: begin analyse data
	if(uiLength >= 8)  //generate a message
	{
		//Step2-1: Byte1 is Check slave address
		if(pRxBuffer[(*pFront)] == PS_Monitor_ID)
		{
			//Step2-2: Byte2 is Modbus different Function code
			ucTempCnt = (*pFront) + 1;
			if(ucTempCnt >= RevSize)
				ucTempCnt -= RevSize;
			if(pRxBuffer[ucTempCnt] == Code10_WHoldRegs)
			{
				ucTempCnt = (*pFront) + 4;
				if(ucTempCnt >= RevSize)
				ucTempCnt -= RevSize;
				usTempDataLen = ((pRxBuffer[ucTempCnt] << 8) & 0xFF00);

				ucTempCnt = (*pFront) + 5;
				if(ucTempCnt >= RevSize)
				ucTempCnt -= RevSize;
				usTempDataLen |= (pRxBuffer[ucTempCnt] & 0x00FF);
				if(usTempDataLen == 6)
				{
					ucTempCnt = (*pFront) + 6;
					if(ucTempCnt >= RevSize)
					ucTempCnt -= RevSize;
					if(pRxBuffer[ucTempCnt] == 12)
					{
						FuncCode = Code10_WHoldRegs;
					}
				}
			}
			else
			{
				FuncCode = pRxBuffer[ucTempCnt];
			}
		}
	}
	if( ((uiLength >= 8)&&(FuncCode != Code10_WHoldRegs)) || ((uiLength >= 21)&&(FuncCode == Code10_WHoldRegs)) )
	{
		if(pRxBuffer[(*pFront)] == PS_Monitor_ID)
		{
			//Step2-2: Byte2 is Modbus different Function code
			ucTempCnt = (*pFront) + 1;
			if(ucTempCnt >= RevSize)
				ucTempCnt -= RevSize;
			FuncCode = pRxBuffer[ucTempCnt];
			if((FuncCode == Code0F_WCoilRegs) || (FuncCode == Code10_WHoldRegs))
			{
				//Step2-2-1: Look up The Master read or write register number
				// delay developed
////////////////////////////add by lhy 2026.5.6///////////////////////////////////
				//Step2-2-1-1: Begin CRC check
				for(i=0; i<19; i++)
				{
					ucTempCnt = (*pFront) + i;
					if(ucTempCnt >= RevSize)
						ucTempCnt -= RevSize;
					ucTempBuf[i] = pRxBuffer[ucTempCnt];
				}

				CRC_Check_Romote(ucTempBuf, 19, &ucCrcH, &ucCrcL);

				//Step2-2-1-2: The receive CRC compared with the calculate
				ucTempCnt = (*pFront) + 19;
				if(ucTempCnt >= RevSize)
					ucTempCnt -= RevSize;

				ucTempCnt1 = (*pFront) + 20;
				if(ucTempCnt1 >= RevSize)
					ucTempCnt1 -= RevSize;

				if( ((ucCrcH == pRxBuffer[ucTempCnt]) && (ucCrcL == pRxBuffer[ucTempCnt1])) ||
					((ucCrcL == pRxBuffer[ucTempCnt]) && (ucCrcH == pRxBuffer[ucTempCnt1])) )
				{
					//Step2-2-1-3: Data processinxg through different function codes
					switch(FuncCode)
					{
						case Code10_WHoldRegs:
						{
							Remote_HoldReg(FuncCode, ucTempBuf, pTxBuffer);
							Modbus_Recv_For_Rem();
						}   break;
					}
					(*pFront) += 21;
					if((*pFront) >= RevSize)
						(*pFront) -= RevSize;
				}
				else //Error: CRC check error
				{
					(*pFront) += 1;
					if((*pFront) >= RevSize)
						(*pFront) -= RevSize;
				}
/////////////////////////////////////////////////////////////////////////////////////
			}
			else if( (FuncCode >= Code01_RCoilReg) && (FuncCode <= Code06_WHoldReg) )
			{
				//Step2-2-2-1: Begin CRC check
				for(i=0; i<6; i++)
				{
					ucTempCnt = (*pFront) + i;
					if(ucTempCnt >= RevSize)
						ucTempCnt -= RevSize;
					ucTempBuf[i] = pRxBuffer[ucTempCnt];
				}

				CRC_Check_Romote(ucTempBuf, 6, &ucCrcH, &ucCrcL);

				//Step2-2-2-2: The receive CRC compared with the calculate
				ucTempCnt = (*pFront) + 6;
				if(ucTempCnt >= RevSize)
					ucTempCnt -= RevSize;

				ucTempCnt1 = (*pFront) + 7;
				if(ucTempCnt1 >= RevSize)
					ucTempCnt1 -= RevSize;

				if( ((ucCrcH == pRxBuffer[ucTempCnt]) && (ucCrcL == pRxBuffer[ucTempCnt1])) ||
					((ucCrcL == pRxBuffer[ucTempCnt]) && (ucCrcH == pRxBuffer[ucTempCnt1])) )
				{
					//Step2-2-2-3: Data processing through different function codes
					switch(FuncCode)
					{
						case Code01_RCoilReg :
						case Code05_WCoilReg :
						{
							// receive
						}break;

						case Code02_RDiscReg :
						{
							// receive
						}break;

						case Code03_RHoldReg :
						{
							Modbus_Send_To_Rem();
							Remote_HoldReg(FuncCode, ucTempBuf, pTxBuffer);
						}break;
						case Code06_WHoldReg :
						{
							Remote_HoldReg(FuncCode, ucTempBuf, pTxBuffer);
							Modbus_Recv_For_Rem();
						}   break;

						case Code04_RInputReg:
						{
							Modbus_Send_To_Rem();
							Remote_InputReg(FuncCode, ucTempBuf, pTxBuffer);
						}break;
					}
					(*pFront) += 8;
					if((*pFront) >= RevSize)
						(*pFront) -= RevSize;
				}
				else //Error: CRC check error
				{
					(*pFront) += 1;
					if((*pFront) >= RevSize)
						(*pFront) -= RevSize;
				}
			}
			else    //Error: Byte2 not normal funition code
			{
				(*pFront) += 1;
				if((*pFront) >= RevSize)
					(*pFront) -= RevSize;
			}
		}
		else	//Error: Byte1 not slave address,continue lookup
		{
			(*pFront) += 1;
			if((*pFront) >= RevSize)
				(*pFront) -= RevSize;
		}
	}
}
