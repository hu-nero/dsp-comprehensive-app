#include "main\Variable.h"
#include "main\Comm_Variable.h"
#include "Main\Custom_GlobalMacro.H"
#include "IQmathlib.H"
#include "LVRT.H"
#include "GPIO\Custom_IOMacro.H"
#include "math.h"

void SecCab_LocalCtrl(void);
void SecCab_RemoteCtrl(void);
///////////////////////////////////////////////////////////////////////////////////
void Source_Select(void)
{
//Ctrl Gevin//////////////////////////////////////
	if(CtrlModeWord.DataBit.Control_GivenMode == 0)
	{
		if(MainCtrlWord_CAN.DataBit.Valid ==1)
		{
			MainCtrlWord.DataInt = MainCtrlWord_CAN.DataInt;
			MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
		}
	}
	else if(CtrlModeWord.DataBit.Control_GivenMode == 1)
	{
		MainCtrlWord.DataInt = MainCtrlWord_PC.DataInt;
	}
	else if(CtrlModeWord.DataBit.Control_GivenMode == 2)
	{
		if(MainStusWord.DataBit.LocRem == 0) // Local
		{
			SecCab_LocalCtrl();
		}
		else if(MainStusWord.DataBit.LocRem == 1) // Remote
		{
			SecCab_RemoteCtrl();
		}

		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
}
///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////
void SecCab_LocalCtrl(void)
{
//	if(MainSta2Word.DataBit.SingQF1 == 1)
//	{
//		Flag_IORstRef = 0;
//		Flag_IODgssCtrl = 0;
//
//		if(Flag_IOSingCtrl == 1)
//		{
//			Flag_IOSingCtrl = 0;
//			// todo close single loop
//		}
//	}
//	else
//	{
//
//	}



}
////////////////////////////////////////////////////
void SecCab_RemoteCtrl(void)
{
//	if(MainStusWord.DataBit.MprRun == 1) // PowerCab running
//	{
//		// Runing is stop
//		if(FlagRemote_Stop == 1)
//		{
//			FlagRemote_Stop = 0;
//
//		}
//
//		if( (FlagRemote_Start == 1) || (FlagRemote_Reset == 1) )
//		{
//			FlagRemote_Start = 0;
//			FlagRemote_Reset = 0;
//		}
//	}
//	else if(MainStusWord.DataBit.MprRun == 0) // PowerCab stop
//	{
//		if(FlagRemote_Stop == 1)
//		{
//			FlagRemote_Stop = 0;
//		}
//
//		if(FlagRemote_Start == 1)
//		{
//			FlagRemote_Start = 0;
//
//			if(MainStusWord.DataBit.NPRRun == 0)
//			{
//
//			}
//		}
//		// Stoping is Rst
//		if(FlagRemote_Reset == 1)
//		{
//			FlagRemote_Reset = 0;
//
//			if(MainStusWord.DataBit.NPRRun == 0)
//			{
//			}
//		}
//	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
