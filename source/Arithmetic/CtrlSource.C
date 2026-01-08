#include "main\Variable.h"
#include "main\Comm_Variable.h"
#include "Main\Custom_GlobalMacro.H"
#include "IQmathlib.H"
#include "LVRT.H"
#include "GPIO\Custom_IOMacro.H"
#include "math.h"

extern void FunFaultReact(void);
///////////////////////////////////////////////////////////////////////////////////
void Source_Select(void)
{
	////////////////////////////////////////////////
	// ctrl         ////////////////////////////////
	////////////////////////////////////////////////
	if(CtrlModeWord.DataBit.Control_GivenMode == 0)      //CAN
	{
		MainCtrlWord_CAN.DataInt = 0; //temp add
		MainCtrlWord.DataInt = MainCtrlWord_CAN.DataInt;
		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
	else if(CtrlModeWord.DataBit.Control_GivenMode == 1) //HMI
	{
		MainCtrlWord_HMI.DataInt = 0; //temp add
		MainCtrlWord.DataInt = MainCtrlWord_HMI.DataInt;
		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
	else if(CtrlModeWord.DataBit.Control_GivenMode == 2)  //LOCAL
	{
		MainCtrlWord.DataInt = MainCtrlWord_PC.DataInt;
		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
	else if(CtrlModeWord.DataBit.Control_GivenMode == 3)  //Hard IO
	{
		// from hmi reset cmd
		if( (MainStusWord.DataBit.LocRem == 0) && (MainStusWord.DataBit.MprRun == 0) )
		{
			if(Rst_Flag_HMI != Rst_Flag_HMIOld)
			{
				Rst_Flag_HMIOld = Rst_Flag_HMI;
				if(Rst_Flag_HMI == 1)
				{
					MainCtrlWord.DataBit.Reset = 1;
					Rst_Flag_HMI = 0;
				}
			}
		}
		////////////////////////////////////////////////////
		if(MainStusWord.DataBit.SingPara == 0)  // Single
		{
			////////////////////////////////////////////////////
			if(MainStusWord.DataBit.LocRem == 0)  // Local 0
			{
				if(MainStusWord.DataBit.MprRun == 1)
				{
					// Runing is stop
					if(Flag_IORstRef == 1)
					{
						Flag_IORstRef = 0;

						MainCtrlWord.DataBit.NPR_Run = 0;
						MainCtrlWord.DataBit.PWMON = 0;
					}

					if(Flag_IORunRef == 1)
					{
						Flag_IORunRef = 0;
					}
				}
				else
				{
					if(Flag_IORunRef == 1)
					{
						Flag_IORunRef = 0;

						if(MainStusWord.DataBit.NPRRun == 0)
						{
							MainCtrlWord.DataBit.NPR_Run = 1;
							MainCtrlWord.DataBit.PWMON = 1;
						}
					}
					// Stoping is Rst
					if(Flag_IORstRef == 1)
					{
						Flag_IORstRef = 0;

						if(MainStusWord.DataBit.NPRRun == 0)
						{
							MainCtrlWord.DataBit.Reset = 1;
						}
					}
				}
			}
			else  // IO_Rem_Feed
			{
				if(PowerCtrlWord.DataBit.CmdVaild == 1)
				{
					if(PowerCtrlWord.DataBit.Run == 1)
					{
						MainCtrlWord.DataBit.NPR_Run =1 ;
						MainCtrlWord.DataBit.PWMON = 1;
					}
					else
					{
						MainCtrlWord.DataBit.NPR_Run = 0;
						MainCtrlWord.DataBit.PWMON = 0;
					}

					CAN_SectorRst_Flag = PowerCtrlWord.DataBit.Reset;
					if(CAN_SectorRst_Flag != CAN_SectorRst_FlagOld)
					{
						CAN_SectorRst_FlagOld = CAN_SectorRst_Flag;
						if(CAN_SectorRst_Flag == 1)
						{
							MainCtrlWord.DataBit.Reset = 1;
						}
					}
				}
				else
				{
					if( (Flag_IORunRef == 1) || (Flag_IORstRef == 1) )
					{
						Flag_IORunRef = 0;
						Flag_IORstRef = 0;
						Operation_ineffective = 1;
					}
					else
					{
						Operation_ineffective = 0;
					}
				}
			}
		}
		else   // Paraller
		{
			if(MainStusWord.DataBit.LocRem == 0)  // Local
			{
				if(1) //if(MasToSlaWord.bit.AllowAdd == 1)
				{
					if(MainStusWord.DataBit.MprRun == 1)
					{
						// Runing is stop
						if(Flag_IORstRef == 1)
						{
							Flag_IORstRef = 0;

							MainCtrlWord.DataBit.NPR_Run = 0;
							MainCtrlWord.DataBit.PWMON = 0;
						}
						// Runing clear Start cmd
						if(Flag_IORunRef == 1)
						{
							Flag_IORunRef = 0;
						}
					}
					else
					{
						// Stoping is Start
						if(Flag_IORunRef == 1)
						{
							Flag_IORunRef = 0;

							if(MainStusWord.DataBit.NPRRun == 0)
							{
								MainCtrlWord.DataBit.NPR_Run = 1;
								MainCtrlWord.DataBit.PWMON = 1;
							}
						}
						// Stoping is Rst
						if(Flag_IORstRef == 1)
						{
							Flag_IORstRef = 0;

							if(MainStusWord.DataBit.NPRRun == 0)
							{
								MainCtrlWord.DataBit.Reset = 1;
							}
						}
					}
				}
				else
				{
					// Alarm: Master is not ready, not allow add
					Flag_IORstRef = 0;
					Flag_IORunRef = 0;
				}
			}
			else //Remod MainStusWord.DataBit.LocRem=0
			{
				// Resived
			}
		} //end singpara select

		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
	else
	{
		MainCtrlWord.DataInt = 0;
		MainCtrlWord_PC.DataInt = MainCtrlWord.DataInt;
	}
/////////////////////////////////////////////////////////////////////////
//// Volt Given /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
	if(CtrlModeWord.DataBit.Us_GivenMode == 0)   //can
	{
		UsdRefRef = UsdRefRef_CAN;
		UsdRefRef_PC = UsdRefRef;
	}
	else if(CtrlModeWord.DataBit.Us_GivenMode == 1) //HMI
	{
		if(MainStusWord.DataBit.SingPara == 0)  // single
		{
			if(MainStusWord.DataBit.MprRun == 0)
			{
				Flag_VoltRef = MainStusWord.DataBit.WorkMode;
				if(Flag_VoltRef != FlagOld_VoltRef)
				{
					FlagOld_VoltRef = Flag_VoltRef;

					if(Flag_VoltRef == 0)
					{
						UsdRefRef_HMI = 190;
					}
					else if(Flag_VoltRef == 1)
					{
						UsdRefRef_HMI = 380;
					}
				}
			}

			if(UsdRefRef_HMI != UsdRefRefOld_HMI)
			{
				UsdRefRefOld_HMI = UsdRefRef_HMI;

				UsdRefRef_Buf = UsdRefRef_HMI *Sqrt_2;
				UsdRefRef = UsdRefRef_Buf;
			}

			UsdRefRef_PC = UsdRefRef_HMI;
			UdRefRef_Outer = UsdRefRef_HMI;
		}
		else // parallel
		{
			if(UdRefRef_Outer != UdRefRefOld_Outer)
			{
				UdRefRefOld_Outer = UdRefRef_Outer;

				UsdRefRef_Buf = UdRefRef_Outer *Sqrt_2;
				UsdRefRef = UsdRefRef_Buf;
			}

			UsdRefRef_PC = UdRefRef_Outer;
			UsdRefRef_HMI = UdRefRef_Outer;
		}
	}
	else if(CtrlModeWord.DataBit.Us_GivenMode == 2) // Local
	{
		if(MainStusWord.DataBit.SingPara == 0) //single
		{
			if(MainStusWord.DataBit.MprRun == 0)
			{
				Flag_VoltRef = MainStusWord.DataBit.WorkMode;
				if(Flag_VoltRef != FlagOld_VoltRef)
				{
					FlagOld_VoltRef = Flag_VoltRef;

					if(Flag_VoltRef == 0)
					{
						UsdRefRef_PC = 190;
					}
					else if(Flag_VoltRef == 1)
					{
						UsdRefRef_PC = 380;
					}
				}
			}

			if(UsdRefRef_PC != UsdRefRefOld_PC) //
			{
				UsdRefRefOld_PC = UsdRefRef_PC;

				UsdRefRef_Buf = UsdRefRef_PC *Sqrt_2;
				UsdRefRef = UsdRefRef_Buf;
			}

			UsdRefRef_HMI = UsdRefRef_PC;
			UdRefRef_Outer = UsdRefRef_PC;
		}
		else  // parallel
		{
			if(UdRefRef_Outer != UdRefRefOld_Outer)
			{
				UdRefRefOld_Outer = UdRefRef_Outer;

				UsdRefRef_Buf = UdRefRef_Outer *Sqrt_2;
				UsdRefRef = UsdRefRef_Buf;
			}

			UsdRefRef_PC = UdRefRef_Outer;
			UsdRefRef_HMI = UdRefRef_Outer;
		}
	}
	else
	{
		UsdRefRef = 0;
		UsdRefRef_PC = UsdRefRef;
	}
/////////////////////////////////////////////////////////////////////////
//// Freq Given /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
	if(CtrlModeWord.DataBit.Freq_GivenMode == 0)
	{
		MotorFreqRef = MotorFreqRef_CAN;
		MotorFreqRef_PC = MotorFreqRef;
	}
	else if(CtrlModeWord.DataBit.Freq_GivenMode == 1) //HMI
	{
		if(MainStusWord.DataBit.MprRun == 0)
		{
			Flag_FreqRef = MainStusWord.DataBit.WorkMode;
			if(Flag_FreqRef != FlagOld_FreqRef)
			{
				FlagOld_FreqRef = Flag_FreqRef;

				if(Flag_FreqRef == 0)
				{
					MotorFreqRef_HMI = 517;
				}
				else if(Flag_FreqRef == 1)
				{
					MotorFreqRef_HMI = 1550;
				}
			}
		}

		MotorFreqRef = MotorFreqRef_HMI;
		MotorFreqRef_PC = MotorFreqRef;
	}
	else if(CtrlModeWord.DataBit.Freq_GivenMode == 2)  //PC
	{
		if(MainStusWord.DataBit.MprRun == 0)
		{
			Flag_FreqRef = MainStusWord.DataBit.WorkMode;
			if(Flag_FreqRef != FlagOld_FreqRef)
			{
				FlagOld_FreqRef = Flag_FreqRef;

				if(Flag_FreqRef == 0)
				{
					MotorFreqRef_PC = 517;
				}
				else if(Flag_FreqRef == 1)
				{
					MotorFreqRef_PC = 1550;
				}
			}
		}

		MotorFreqRef = MotorFreqRef_PC;
		MotorFreqRef_HMI = MotorFreqRef_PC;
		MotorFreqRef_PC = MotorFreqRef;
	}
	else
	{
		MotorFreqRef = 0;
		MotorFreqRef_PC = MotorFreqRef;
	}
	////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////
void Udq_Ramp_Limit(int PWM_Flag)
{
	float ftemp;
///////////////////////////////////////////////////////////////
	if(UsdRefRef > Usd_Lim)
		UsdRefRef = Usd_Lim;

	if( (PWM_Flag == 1) && (MainCtrlWord.DataBit.PWMON==1) )
	{
		UdqStep_Count ++;
		if(UdqStep_Count >= UdqStep_CountMax)
		{
			UdqStep_Count = 0;

			ftemp = 0.3*(UsdRefRef - UsdRef);
			if(ftemp > Ud_cal_step)
				ftemp = Ud_cal_step;
			if(ftemp < -Ud_cal_step)
				ftemp = -Ud_cal_step;
			UsdRef += ftemp;
		}
	}

	if( (PWM_Flag==0) || (MainCtrlWord.DataBit.PWMON==0) )
	{
		ftemp = 0.3*UsdRef;
		if(ftemp > Ud_cal_step)
			ftemp = Ud_cal_step;
		if(ftemp < -Ud_cal_step)
			ftemp = -Ud_cal_step;
		UsdRef -= ftemp;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Udq_FastChar(void)
{
	FastChar_CtrlCount ++;
	if(FastChar_RunStatus == 0)     // ready
	{
		FastChar_CtrlCount = 0;

		FastChar_RiseGiven = 0;
		FastChar_DownGiven = 0;
		FastChar_UdCompon = 0;
		FastChar_UqCompon = 0;

		MainStusWord.DataBit.FastChar = 0;

		FastChar_UsdOld = UsdRef;
		FastChar_UsqOld = UsqRef;

		if(FastChar_CtrlWord == 1)
		{
			FastChar_RunStatus = 1;

			FastChar_HoleCount = FastChar_HoleTime *9;
			FastChar_DownCount = FastChar_DownTime *9;
			FastChar_RiseGiven = FastChar_DCOffset;

			//calc Hole time
			if(FastChar_HoleCount <= 0)
			{
				FastChar_HoleCount = 1;
			}

			//calc Down time
			if(FastChar_DownCount <= 0)
			{
				FastChar_DownCount = 1;
			}
			FastChar_DownStep = (FastChar_UsdOld - FastChar_AfterVolt) / FastChar_DownCount;
		}
	}
	else if(FastChar_RunStatus == 1)  //begin RiseRef
	{
		if(FastChar_CtrlCount < FastChar_HoleCount)
		{
			if(FastChar_PhaseSel == 1)  // Phase A
			{
				FastChar_UdCompon =  FastChar_RiseGiven *cos(MotorVolt_Angle);// *0.81649658; // (2/sqrt(6))*cos()
				FastChar_UqCompon = -FastChar_RiseGiven *cos(MotorVolt_Angle);// *0.81649658;
			}
			else if(FastChar_PhaseSel == 2)  // Phase B
			{
				FastChar_UdCompon =  FastChar_RiseGiven *cos(MotorVolt_Angle - _2PIdiv3);//*0.81649658 ; // (2/sqrt(6))*cos()
				FastChar_UqCompon = -FastChar_RiseGiven *cos(MotorVolt_Angle - _2PIdiv3);//*0.81649658;
			}
			else if(FastChar_PhaseSel == 3)  // Phase C
			{
				FastChar_UdCompon =  FastChar_RiseGiven *cos(MotorVolt_Angle + _2PIdiv3);//0.81649658 ; // (2/sqrt(6))*cos()
				FastChar_UqCompon = -FastChar_RiseGiven *cos(MotorVolt_Angle + _2PIdiv3);//*0.81649658 ;
			}
			else if(FastChar_PhaseSel == 7) // Phase ABC
			{
				FastChar_UdCompon =  FastChar_RiseGiven ;
			}
			else
			{
				FastChar_UdCompon = 0;
				FastChar_UqCompon = 0;
			}
		}
		else
		{
			FastChar_CtrlCount = 0;
			FastChar_RunStatus = 2;
		}

		MainStusWord.DataBit.FastChar = 1;
	}
	else if(FastChar_RunStatus == 2)
	{
		FastChar_UdCompon = 0;
		FastChar_UqCompon = 0;
		FastChar_DownGiven = 0;
		FastChar_CtrlCount = 0;
		FastChar_RunStatus = 3;
	}
	else if(FastChar_RunStatus == 3)  //begin DownRef
	{
		if(FastChar_CtrlCount < FastChar_DownCount)
		{
			FastChar_DownGiven += FastChar_DownStep;
			FastChar_UdCompon = -FastChar_DownGiven;
			FastChar_UqCompon = 0;
		}
		else
		{
			FastChar_CtrlCount = 0;
			FastChar_RunStatus = 4;
		}
	}
	else if(FastChar_RunStatus == 4)
	{
		FastChar_CtrlWord = 0;


		if(FastChar_StopWord == 1)
		{
			FastChar_RunStatus = 0;
		}

		if(Iphase_max > FastChar_ExitCurrSet)
		{
			FastChar_RunStatus = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////



