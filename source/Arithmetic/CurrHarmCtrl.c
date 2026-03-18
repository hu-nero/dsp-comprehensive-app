/// NprCtrl.c
#include "DSP28x_Project.h"
#include "IQmathLib.h"
#include "main\Variable.h"
#include "Arithmetic.h"

int Curr_Har1_EnDelayCount = 0;
int Curr_Har2_EnDelayCount = 0;

/*******************************************************************************************************/
void Curr_Har1_Ctrl(int PWMFlag, int CtrlEn)
{
//	int i;
//	int Angle;
//
//	if(PWMFlag == 1)
//	{
//		Curr_Har1_dErr = IgdRef - npr_id;
//		Curr_Har1_qErr = IgqRef - npr_iq;
//	}
//	else
//	{
//		Curr_Har1_dErr = 0;
//		Curr_Har1_qErr = 0;
//		for(i=0; i<3; i++)
//		{
//			Curr_Har1_d_LPF_X[i] = 0;
//			Curr_Har1_d_LPF_Y[i] = 0;
//			Curr_Har1_q_LPF_X[i] = 0;
//			Curr_Har1_q_LPF_Y[i] = 0;
//		}
//	}
//
//	Angle = GridUconvert_Angle*Curr_Har1_Order;
//
//	Fun2s2rConv(Curr_Har1_dErr, Curr_Har1_qErr, Angle, &Curr_Har1_d, &Curr_Har1_q);
//
//	Curr_Har1_df = Freq50LPFilter(Curr_Har1_d_LPF_X, Curr_Har1_d_LPF_Y, Curr_Har1_d, 3000);
//	Curr_Har1_qf = Freq50LPFilter(Curr_Har1_q_LPF_X, Curr_Har1_q_LPF_Y, Curr_Har1_q, 3000);
//
//	Curr_Har1_dLoop_Out = FunPIComCtrl(Curr_Har1_Loop_Kp, Curr_Har1_Loop_Ki, Curr_Har1_df, &Curr_Har1_dErrtotal, Curr_Har1_Loop_Lim, Curr_Har1_Loop_Lim);
//	Curr_Har1_qLoop_Out = FunPIComCtrl(Curr_Har1_Loop_Kp, Curr_Har1_Loop_Ki, Curr_Har1_qf, &Curr_Har1_qErrtotal, Curr_Har1_Loop_Lim, Curr_Har1_Loop_Lim);
//
//	if( (CtrlEn == 0) || (PWMFlag == 0) )
//		Curr_Har1_EnDelayCount = 0;
//	else
//	{
//		if(Curr_Har1_EnDelayCount < 1000)
//			Curr_Har1_EnDelayCount ++ ;
//	}
//	if(Curr_Har1_EnDelayCount < 800)
//	{
//		Curr_Har1_dErrtotal = 0;
//		Curr_Har1_qErrtotal = 0;
//		Curr_Har1_dLoop_Out = 0;
//		Curr_Har1_qLoop_Out = 0;
//	}
//	Fun2r2sConv(Curr_Har1_dLoop_Out, Curr_Har1_qLoop_Out, Angle, &Curr_Har1_alOut, &Curr_Har1_beOut);
}
/********************************************************************************************/

/***************************************************************************************************/













