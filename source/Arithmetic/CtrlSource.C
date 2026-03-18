#include "main\Variable.h"
#include "main\Comm_Variable.h"
#include "Main\Custom_GlobalMacro.H"
#include "IQmathlib.H"
#include "LVRT.H"
#include "GPIO\Custom_IOMacro.H"
#include "math.h"
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
		MainCtrlWord.DataBit.ACPrechar = Flag_IORunRef;
		MainCtrlWord.DataBit.ACTON = Flag_IORunRef;
		MainCtrlWord.DataBit.PWMON = Flag_IORunRef;

		if(Flag_IORstRef != Flag_IORstRefOld)
		{
			Flag_IORstRefOld = Flag_IORstRef;
			if(Flag_IORstRef == 1)
			{
				if(((IOInput1.DataInt >> 10)&0x0001) != 0x0001)
				{
					MainCtrlWord.DataBit.Reset = 1;
				}
				else
				{
					MainCtrlWord.DataBit.Reset = 0;
				}
			}
		}

		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
////KVar Gevin/////////////////////////////////////////////////
	if(CtrlModeWord.DataBit.Reactive_GivenMode == 0)
	{
		KVARRef_Sys = KVARRef_CAN;
	}
	else if(CtrlModeWord.DataBit.Reactive_GivenMode == 1)
	{
		KVARRef_Sys = KVARRef_PC;
	}
	else
	{
		KVARRef_Sys = 0;
	}

	KVARRef_PC = KVARRef_Sys;

	KVARRef_Sys -= KVar_filter_Comp;
	if(KVARRef_Sys > KVar_Lim)
		KVARRef_Sys = KVar_Lim;
	if(KVARRef_Sys < -KVar_Lim)
		KVARRef_Sys = -KVar_Lim;

////KW Gevin/////////////////////////////////////////////////
	if(CtrlModeWord.DataBit.Active_GivenMode == 0)
	{
		KWRefRef_Sys = KWRefRef_CAN;
	}
	else if(CtrlModeWord.DataBit.Active_GivenMode == 1)
	{
		KWRefRef_Sys = KWRefRef_PC;
	}
	else
	{
		KWRefRef_Sys = 0;
	}

	KWRefRef_PC = KWRefRef_Sys;

	if(KWRefRef_Sys > KW_Lim)
		KWRefRef_Sys = KW_Lim;
	if(KWRefRef_Sys < -KW_Lim)
		KWRefRef_Sys = -KW_Lim;
}
///////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
