/***************************************************
 * @brief   modbus
 * @date    2024/04/09
 * @author  Cheng peng
****************************************************/
#include "Custom_ModbusRTU.h"

void ModbusRTU_Protocol(u16 *pRxBuffer, u16 *pFront, u16 nRear, u16 RevSize, u16 *pTxBuffer)
{
	u16 ucTempCnt, ucTempCnt1;
	u16 i, ucTempBuf[128];
	u8 FuncCode;
	int uiLength;
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
		if(pRxBuffer[(*pFront)] == SlaveDeviceAddr)
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

				CRC_Check(ucTempBuf, 6, &ucCrcH, &ucCrcL);

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
						case Code06_WHoldReg :
						{
							Func_HoldReg(FuncCode, ucTempBuf, pTxBuffer);
						}break;

						case Code04_RInputReg:
						{
							Func_InputReg(FuncCode, ucTempBuf, pTxBuffer);
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



