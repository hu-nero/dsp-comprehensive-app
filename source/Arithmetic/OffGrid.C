/// NprCtrl.c
#include "DSP28x_Project.h"
#include "NprCtrl.h"
#include "IQmathLib.h"
#include "main\Variable.H"
#include "main\Custom_GlobalMacro.H"
#include "Arithmetic.H"

////////////////////////////////////////////////////////////////////////
int Volt_Har1_EnDelayCount = 0;
int Volt_Har2_EnDelayCount = 0;
/***********************************************************************************************/
void OffGrid_Ug_Loop(int PWM_Flag)
{
	float ftemp;

	ftemp = OffGrid_UgdRef - npr_udpf;
	OffGrid_Ud_Loop_Out = FunPIComCtrl(Ud_Loop_Kp, Ud_Loop_Ki, ftemp, &OffGrid_Errtotal_Ud, Udq_Loop_Lim, Udq_Loop_Lim);

	ftemp = OffGrid_UgqRef - npr_udpf;
	OffGrid_Uq_Loop_Out = FunPIComCtrl(Uq_Loop_Kp, Uq_Loop_Ki, ftemp, &OffGrid_Errtotal_Uq, Udq_Loop_Lim, Udq_Loop_Lim);

	ftemp = OffGrid_UgdnRef - npr_udnf;
	OffGrid_Udn_Loop_Out = FunPIComCtrl(Udn_Loop_Kp, Udn_Loop_Ki, ftemp, &OffGrid_Errtotal_Udn, Udqn_Loop_Lim, Udqn_Loop_Lim);

	ftemp = OffGrid_UgqnRef - npr_uqnf;
	OffGrid_Uqn_Loop_Out = FunPIComCtrl(Uqn_Loop_Kp, Uqn_Loop_Ki, ftemp, &OffGrid_Errtotal_Uqn, Udqn_Loop_Lim, Udqn_Loop_Lim);

	if(PWM_Flag == 0)
	{
		OffGrid_Errtotal_Ud = OffGrid_Errtotal_Ud*0.9;
		if(OffGrid_Errtotal_Ud>0)	OffGrid_Errtotal_Ud -= 8;
		if(OffGrid_Errtotal_Ud<0)	OffGrid_Errtotal_Ud += 8;

		OffGrid_Ud_Loop_Out = OffGrid_Ud_Loop_Out*0.9;
		if(OffGrid_Ud_Loop_Out>0)	OffGrid_Ud_Loop_Out -= 8;
		if(OffGrid_Ud_Loop_Out<0)	OffGrid_Ud_Loop_Out += 8;

		OffGrid_Errtotal_Uq = OffGrid_Errtotal_Uq*0.9;
		if(OffGrid_Errtotal_Uq>0)	OffGrid_Errtotal_Uq -= 8;
		if(OffGrid_Errtotal_Uq<0)	OffGrid_Errtotal_Uq += 8;

		OffGrid_Uq_Loop_Out = OffGrid_Uq_Loop_Out*0.9;
		if(OffGrid_Uq_Loop_Out>0)	OffGrid_Uq_Loop_Out -= 8;
		if(OffGrid_Uq_Loop_Out<0)	OffGrid_Uq_Loop_Out += 8;

		OffGrid_Errtotal_Udn = OffGrid_Errtotal_Udn*0.9;
		if(OffGrid_Errtotal_Udn>0)	OffGrid_Errtotal_Udn -= 8;
		if(OffGrid_Errtotal_Udn<0)	OffGrid_Errtotal_Udn += 8;

		OffGrid_Udn_Loop_Out = OffGrid_Udn_Loop_Out*0.9;
		if(OffGrid_Udn_Loop_Out>0)	OffGrid_Udn_Loop_Out -= 8;
		if(OffGrid_Udn_Loop_Out<0)	OffGrid_Udn_Loop_Out += 8;

		OffGrid_Errtotal_Uqn = OffGrid_Errtotal_Uqn*0.9;
		if(OffGrid_Errtotal_Uqn>0)	OffGrid_Errtotal_Uqn -= 8;
		if(OffGrid_Errtotal_Uqn<0)	OffGrid_Errtotal_Uqn += 8;

		OffGrid_Uqn_Loop_Out = OffGrid_Uqn_Loop_Out*0.9;
		if(OffGrid_Uqn_Loop_Out>0)	OffGrid_Uqn_Loop_Out -= 8;
		if(OffGrid_Uqn_Loop_Out<0)	OffGrid_Uqn_Loop_Out += 8;
	}

	UgCtrl_d = OffGrid_Ud_Loop_Out + OffGrid_UgdRef;
	UgCtrl_q = OffGrid_Uq_Loop_Out ;

	UgCtrl_dn = OffGrid_Udn_Loop_Out;
	UgCtrl_qn = OffGrid_Uqn_Loop_Out;
}
/***********************************************************************************/
void Udq_Ramp_Limit(int PWM_Flag)
{
	float ftemp;
///////////////////////////////////////////////////////////////
	if(PWM_Flag == 1)
	{
		ftemp = OffGrid_UgdRefRef - OffGrid_UgdRef;
		if(ftemp > OffGrid_UgdRefRamp)
			ftemp = OffGrid_UgdRefRamp;
		if(ftemp < -OffGrid_UgdRefRamp)
			ftemp = -OffGrid_UgdRefRamp;
		OffGrid_UgdRef += ftemp;
	}
	else
	{
		OffGrid_UgdRef = OffGrid_UgdRef*0.9;
		if(OffGrid_UgdRef>0)	OffGrid_UgdRef -= 8;
		if(OffGrid_UgdRef<0)	OffGrid_UgdRef += 8;
	}
}
/***********************************************************/
void Volt_Har1_Ctrl(int PWMFlag, int CtrlEn)
{
	int i;
	float Angle;

	if(PWMFlag == 1)
	{
		Volt_Har1_dErr = npr_udp - OffGrid_UgdRef;
		Volt_Har1_qErr = npr_uqp - OffGrid_UgqRef;
	}
	else
	{
		Volt_Har1_dErr = 0;
		Volt_Har1_qErr = 0;
		for(i=0; i<3; i++)
		{
			Volt_Har1_d_LPF_X[i]=0;
			Volt_Har1_d_LPF_Y[i]=0;
			Volt_Har1_q_LPF_X[i]=0;
			Volt_Har1_q_LPF_Y[i]=0;
		}
	}

	Angle = GridUconvert_Angle*Volt_Har1_Order;

	while(Angle < 0)
		Angle += _2PI;
	while(Angle >= _2PI)
		Angle -= _2PI;

	Fun2s2rConv(Volt_Har1_dErr, Volt_Har1_qErr, Angle, &Volt_Har1_d, &Volt_Har1_q);

//	Volt_Har1_df = Freq50LPFilter(Volt_Har1_d_LPF_X, Volt_Har1_d_LPF_Y, Volt_Har1_d, 3000);
//	Volt_Har1_qf = Freq50LPFilter(Volt_Har1_q_LPF_X, Volt_Har1_q_LPF_Y, Volt_Har1_q, 3000);

	Volt_Har1_dLoop_Out = FunPIComCtrl(Volt_Har1_Loop_Kp, Volt_Har1_Loop_Ki, -Volt_Har1_df, &Volt_Har1_dErrtotal, Volt_Har_Loop_Lim, Volt_Har_Loop_Lim);
	Volt_Har1_qLoop_Out = FunPIComCtrl(Volt_Har1_Loop_Kp, Volt_Har1_Loop_Ki, -Volt_Har1_qf, &Volt_Har1_qErrtotal, Volt_Har_Loop_Lim, Volt_Har_Loop_Lim);

	if( (CtrlEn == 0) || (PWMFlag == 0) )
		Volt_Har1_EnDelayCount = 0;
	else
	{
		if(Volt_Har1_EnDelayCount < 1000)
			Volt_Har1_EnDelayCount ++ ;
	}
	if(Volt_Har1_EnDelayCount < 800)
	{
		Volt_Har1_dErrtotal = 0;
		Volt_Har1_qErrtotal = 0;
		Volt_Har1_dLoop_Out = 0;
		Volt_Har1_qLoop_Out = 0;
	}
	Fun2r2sConv(Volt_Har1_dLoop_Out, Volt_Har1_qLoop_Out, Angle, &Volt_Har1_alOut, &Volt_Har1_beOut);
}
/********************************************************************************************/
void Volt_Har2_Ctrl(int PWMFlag, int CtrlEn)
{
	int i;

	float Angle;

	if(PWMFlag == 1)
	{
		Volt_Har2_dErr = npr_udp - OffGrid_UgdRef;
		Volt_Har2_qErr = npr_uqp - OffGrid_UgqRef;
	}
	else
	{
		Volt_Har2_dErr = 0;
		Volt_Har2_qErr = 0;
		for(i=0;i<3;i++)
		{
			Volt_Har2_d_LPF_X[i]=0;
			Volt_Har2_d_LPF_Y[i]=0;
			Volt_Har2_q_LPF_X[i]=0;
			Volt_Har2_q_LPF_Y[i]=0;
		}
	}

	Angle = GridUconvert_Angle*Volt_Har2_Order;

	while(Angle < 0)
		Angle += 23040;
	while(Angle >= 23040)
		Angle -= 23040;

	Fun2s2rConv(Volt_Har2_dErr, Volt_Har2_qErr, Angle, &Volt_Har2_d, &Volt_Har2_q);

//	Volt_Har2_df = Freq50LPFilter(Volt_Har2_d_LPF_X, Volt_Har2_d_LPF_Y, Volt_Har2_d, 3000);
//	Volt_Har2_qf = Freq50LPFilter(Volt_Har2_q_LPF_X, Volt_Har2_q_LPF_Y, Volt_Har2_q, 3000);

	Volt_Har2_dLoop_Out = FunPIComCtrl(Volt_Har2_Loop_Kp, Volt_Har2_Loop_Ki, -Volt_Har2_df, &Volt_Har2_dErrtotal, Volt_Har_Loop_Lim, Volt_Har_Loop_Lim);
	Volt_Har2_qLoop_Out = FunPIComCtrl(Volt_Har2_Loop_Kp, Volt_Har2_Loop_Ki, -Volt_Har2_qf, &Volt_Har2_qErrtotal, Volt_Har_Loop_Lim, Volt_Har_Loop_Lim);

	if((CtrlEn == 0)||(PWMFlag == 0))
		Volt_Har2_EnDelayCount = 0;
	else
	{
		if(Volt_Har2_EnDelayCount < 1000)
			Volt_Har2_EnDelayCount ++ ;
	}
	if(Volt_Har2_EnDelayCount < 800)
	{
		Volt_Har2_dErrtotal = 0;
		Volt_Har2_qErrtotal = 0;
		Volt_Har2_dLoop_Out = 0;
		Volt_Har2_qLoop_Out = 0;
	}
	Fun2r2sConv(Volt_Har2_dLoop_Out, Volt_Har2_qLoop_Out, Angle, &Volt_Har2_alOut, &Volt_Har2_beOut);
}
/***************************************************************************************************/
