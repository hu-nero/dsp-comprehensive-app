#include "MPPT.h"
#include "IQmathLib.h"
#include "main\Variable.H"
#include "main\Comm_Variable.H"
#include "main\Custom_GlobalMacro.H"
#include "Arithmetic.H"
#include "DSP28x_Project.h"
#include "math.h"

/////////////////////////////////////////////////////////////////////////////////
void Mppt_Calc(void)
{
	if(MPPT_EnFlag == 1)
	{
		Vnow = npr_udcf;
//		Inow = Idc_Rmsf;
		MPPT_1msCount++;
		if(MPPT_1msCount >= MPPT_AccessTime)
		{
			MPPT_1msCount = 0;

//			Pnow = fabs(Vnow*Inow);
			Pnow = fabs(NPR_Power);
	//MPPT扰动
			if(MPPT_State == 0)   //记录P/V后 ,开始向上扰动
			{
				P_act = Pnow;
				V_act = Vnow;
				VmppOut = V_act + Vmpp_Disturb_Step;
				MPPT_State = 1;
			}
			else if(MPPT_State == 1)	//记录向上扰动P/V后,向下扰动
			{
				P_up = Pnow;
				V_up = Vnow;
				VmppOut = V_act - Vmpp_Disturb_Step;
				MPPT_State = 2;
			}
			else if(MPPT_State == 2)    //记录向下扰动P/V后,开始计算
			{
				P_down = Pnow;
				V_down = Vnow;
				MPPT_State = 0;

				if( (P_act > P_up)&&(P_act < P_down) )   //最大功率点 后
				{
					VmppOut = V_down;
					MPPT_RiseCount ++;
					MPPT_StopCount = 0;
					MPPT_FallCount = 0;
				}
				else if( (P_act < P_up)&&(P_act > P_down) )      //最大功率点 前
				{
					VmppOut = V_up;
					MPPT_RiseCount = 0;
					MPPT_StopCount = 0;
					MPPT_FallCount ++;
				}
				else
				{
					VmppOut = V_act;
					MPPT_RiseCount = 0;
					MPPT_StopCount ++;
					MPPT_FallCount = 0;
				}
			}

	//MPPT扰动步长限幅
			if( (MPPT_RiseCount >= 10) || (MPPT_FallCount >= 10) )
			{
				MPPT_RiseCount = 0;
				MPPT_FallCount = 0;
				Vmpp_Disturb_Step = Vmpp_Disturb_Step * 2;
				if(Vmpp_Disturb_Step > Vmpp_Disturb_StepMax)
				{
					Vmpp_Disturb_Step = Vmpp_Disturb_StepMax;
				}
			}
			else if(MPPT_StopCount >= 20)
			{
				MPPT_StopCount = 0;
				Vmpp_Disturb_Step = Vmpp_Disturb_Step * 0.5;
				if(Vmpp_Disturb_Step < Vmpp_Disturb_StepMin)
				{
					Vmpp_Disturb_Step = Vmpp_Disturb_StepMin;
				}
			}
	//MPPT扰动电压限幅
			if(VmppOut > VmppMax)
			{
				VmppOut = VmppMax;
			}
			if(VmppOut < VmppMin)
			{
				VmppOut = VmppMin;
			}

			Udc_set = VmppOut;
		}
	}
}

