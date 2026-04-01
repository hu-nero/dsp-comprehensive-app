#include "Custom_DCS.h"
#include "Main\Variable.H"
#include "SCI_DCS\Sci_DCS_config.H"
#include "SCI_ModbusM\Custom_Remote.h"

/***************************************************************************************************/
void Modbus_Send_To_Dcs(void)
{
	memcpy(DcsInputRegBuf, &DcsInputSum, sizeof(DcsInputSum));
}

void Modbus_Recv_For_Dcs(void)
{
	memcpy(&RemHoldSum, RemHoldRegBuf, sizeof(RemHoldSum));
//
//	if(RemHoldSum.CtrlWord.bit.Cmd)
//	{
//		switch(RemHoldSum.CtrlWord.bit.Cmd)
//		{
//			case 1: FlagRemote_Start = 1; break;
//			case 2: FlagRemote_Stop = 1; break;
//			case 3: FlagRemote_Reset = 1; break;
//			default: break;
//		}
//	}
//	else if(RemHoldSum.CtrlWord.bit.FastChar)
//	{
//
//	}
//	else if(RemHoldSum.CtrlWord.bit.ComCut)
//	{
//		switch(RemHoldSum.CtrlWord.bit.ComCut)
//		{
//			case 0: break;
//			case 1: FlagRemote_Combine = 1;  break;
//			case 2: FlagRemote_Split = 1;   break;
//			case 3: break;
//		}
//	}
//	else if(RemHoldSum.CtrlWord.bit.SingPara)
//	{
//
//	}
}
