#include "Custom_Remote.h"
#include "Main\Variable.H"
#include "SCI_Modbus\Sci_Modbus_config.H"

/***************************************************************************************************/
void Modbus_Send_To_Rem(void)
{
	memcpy(RemInputRegBuf, &RemInputSum, sizeof(RemInputSum));
}

void Modbus_Recv_For_Rem(void)
{
	memcpy(&RemHoldSum, RemHoldRegBuf, sizeof(RemHoldSum));

//	if(RemHoldSum.CtrlWord.bit.SlCtrl)
//	{
//		switch(RemHoldSum.CtrlWord.bit.SlCtrl)
//		{
//			case 1: FlagRem_SingOn = 1; break;
//			case 2: FlagRem_SingOff = 1; break;
//			default: break;
//		}
//	}
//	else if(RemHoldSum.CtrlWord.bit.ParCtrl)
//	{
//		switch(RemHoldSum.CtrlWord.bit.ParCtrl)
//		{
//			case 1: FlagRem_ParaOn = 1; break;
//			case 2: FlagRem_ParaOff = 1; break;
//			default: break;
//		}
//	}
//	else if(RemHoldSum.CtrlWord.bit.SingPara)
//	{
//
//	}
//	else if(RemHoldSum.CtrlWord.bit.ComCut)
//	{
//		switch(RemHoldSum.CtrlWord.bit.ComCut)
//		{
//			case 0: break;
//			case 1: FlagRem_Combine = 1;  break;
//			case 2: FlagRem_Split = 1;   break;
//			case 3: break;
//		}
//	}
//	else if(RemHoldSum.CtrlWord.bit.Reset)
//	{
//		FlagRem_Reset = 1;
//	}
}
