#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"
#include "Arithmetic.h"
#include "IQmathLib.h"
#include "main\Variable.h"
#include "main\Comm_Variable.h"
#include "math.h"
//////////////////////////////////////////////////////////////
//+------------+------------+------------+------------+------------+-------------+
//|  sqrt(3/2) |      0     | -sqrt(1/2) |  sqrt(2)   | -sqrt(3/2) |  sqrt(3/2)  |
//+------------+------------+------------+------------+------------+-------------+
//|  sqrt(1/2) |  sqrt(1/2) |      0     | -sqrt(3/2) |  sqrt(2)   |  sqrt(1/2)  |
//+------------+------------+------------+------------+------------+-------------+
//|      0     |  sqrt(3/2) |  sqrt(2)   |  sqrt(1/2) |  sqrt(3/2) |  sqrt(3/2)  |
//+------------+------------+------------+------------+------------+-------------+
//| -sqrt(1/2) | -sqrt(1/2) |  sqrt(3/2) |      0     |  sqrt(1/2) | -sqrt(2)    |
//+------------+------------+------------+------------+------------+-------------+

const float decomp[6][2][2]=
		{ 1.224744871,            0, -0.707106781,  1.414213562, -1.224744871,  1.224744871,
		  0.707106781,  0.707106781,            0, -1.224744871,  1.414213562, -0.707106781,
		  0,           -1.224744871, -1.414213562,  0.707106781, -1.224744871,  1.224744871,
		 -0.707106781, -0.707106781,  1.224744871,            0,  0.707106781, -1.414213562 };
/****************************************************************/
//ab/bc line voltage to alpha/beta voltage transform
void FunU3s2sConv(float Uab, float Ubc, float *Ual, float *Ube)
{
	float ftemp;
	ftemp = Uab + (Ubc*0.5);
	*Ual = ftemp * 0.8164965809;	//Ualpha=sqrt(2/3)*(Uab+0.5*Ubc)
	*Ube = Ubc * 0.7071067812;  	//Ubeta=sqrt(1/2)*Ubc
}
/*************************************************************/
//a/b/c voltage(or current) to alpha/beta voltage(current) transform
void FunI3s2sConv(float Ia, float Ib, float Ic, float *Ial, float *Ibe)
{
	*Ial = Ia*0.8164965809 - Ib*0.4082482905 -Ic*0.4082482905; //ialpha=sqrt(2/3)*ia-sqrt(1/6)*ib-sqrt(1/6)*ic
	*Ibe = Ib*0.7071067812 - Ic*0.7071067812; //ibeta=sqrt(1/2)*ib -sqrt(1/2)*ic
}
/************************************************************/
void Fun2s3sConv(float Ux, float Uy, float *Ua, float *Ub, float *Uc)
{
	*Ua = Ux*0.8164965809;    //Ua=sqrt(2/3)*Ualpha
	*Ub = Uy*0.7071067812 - Ux*0.4082482905; //ub=sqrt(1/2)*Ubeta-sqrt(1/6)*Ualpha
	*Uc = -*Ua -*Ub;
}

/************************************************************/
void Fun2s2rConv(float Ux, float Uy, float Theta, float *Ud, float *Uq)
{
	*Ud = Ux*cos(Theta) + Uy*sin(Theta);    //um = ualfa*cos_sitar + ubata*sin_sitar
	*Uq = Uy*cos(Theta) - Ux*sin(Theta);    //ut = -ualfa*sin_sitar + ubata*cos_sitar
}
//////////////////////////////////////////
//d/q voltege(or current) to alpha/beta voltage(or current) transform
void Fun2r2sConv(float Ud, float Uq, float Theta, float *Ux, float *Uy)
{
	*Ux = Ud*cos(Theta) - Uq*sin(Theta);
	*Uy = Ud*sin(Theta) + Uq*cos(Theta);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//SVPWM implement  arithmetic ;Ual,Ube,Udc using Q3 ;Time_Per,period of timer that PWM compare using
//*Cmp1,*Cmp2,return value that give to compare_register1 and compare_register2
void SVPWM(float Ual, float Ube, float Udc, int Time_Per, int *PCmp1, int *PCmp2, int *PCmp3)
{
	int phase;
	int Cmp0, Cmp1, Cmp2;
	float fCmp1, fCmp2;
	float t;
	float UalPer, UbePer;
	float npr_vector_d, npr_vector_q;

	UalPer = Ual / Udc;
	UbePer = Ube / Udc;

	npr_vector_d = UalPer * Sqrt_3;
	npr_vector_q = UbePer;

	if((npr_vector_q < npr_vector_d) && (npr_vector_q >= 0))
		phase = 0;
	else if((npr_vector_q >= npr_vector_d) && (npr_vector_q > -npr_vector_d))
		phase = 1;
	else if((npr_vector_q > 0) && (npr_vector_q <= -npr_vector_d))
		phase = 2;
	else if((npr_vector_q <= 0) && (npr_vector_q > npr_vector_d))
		phase = 3;
	else if((npr_vector_q < -npr_vector_d) && (npr_vector_q <= npr_vector_d))
		phase = 4;
	else
		phase = 5;
	  
	fCmp1 = UalPer*decomp[phase][0][0] + UbePer*decomp[phase][1][0];
	Cmp1 = (int)(fCmp1*Time_Per);

	fCmp2 = UalPer*decomp[phase][0][1] + UbePer*decomp[phase][1][1];
	Cmp2 = (int)(fCmp2*Time_Per);

	//2�����ƴ���
	if(Cmp1 > Time_Per)
	{
		Cmp1 = Time_Per;
		Cmp2 = 0;
	}
	if(Cmp2 > Time_Per)
	{
		Cmp2 = Time_Per;
		Cmp1 = 0;
	}

	if((Cmp1+Cmp2) <= Time_Per)
	{
		Cmp0 = (Time_Per - Cmp1 - Cmp2);
	}
	else	//1�����ƴ���
	{
		t = ( (float)Time_Per ) / ( (float)(Cmp1+Cmp2) );
		Cmp1 = (int)(Cmp1 * t);
		Cmp2 = (int)(Cmp2 * t);
		Cmp0 = 0;
	}

	if(Flag_PwmModuMode == 0)
	{
		Cmp1 += Cmp0;
		Cmp2 = Time_Per + 1;
	}
	else if(Flag_PwmModuMode == 1)
	{
		Cmp0 = Cmp0>>1;
		Cmp1 += Cmp0;
		Cmp2 += Cmp1;
	}

	switch(phase)
	{
		case 0:
			{*PCmp1=Cmp0; *PCmp2=Cmp1; *PCmp3=Cmp2; break;}
		case 1:
			{*PCmp1=Cmp1; *PCmp2=Cmp0; *PCmp3=Cmp2; break;}
		case 2:
			{*PCmp1=Cmp2; *PCmp2=Cmp0; *PCmp3=Cmp1; break;}
		case 3:
			{*PCmp1=Cmp2; *PCmp2=Cmp1; *PCmp3=Cmp0; break;}
		case 4:
			{*PCmp1=Cmp1; *PCmp2=Cmp2; *PCmp3=Cmp0; break;}
		case 5:
			{*PCmp1=Cmp0; *PCmp2=Cmp2; *PCmp3=Cmp1; break;}
		default:break;
	}
	*PCmp1 = Time_Per - (*PCmp1);
	*PCmp2 = Time_Per - (*PCmp2);
	*PCmp3 = Time_Per - (*PCmp3);
}
/***********************************************************************/
float FunPIComCtrl(float Kp, float Ki, float err, float *Perrtotal, float IMaxLim)
{
	float ftemp;
	float lfErrorTotal;

	lfErrorTotal = (*Perrtotal);
	ftemp = Ki*err;
	lfErrorTotal += ftemp;

	if(lfErrorTotal > IMaxLim)
		lfErrorTotal = IMaxLim;
	if(lfErrorTotal < (-IMaxLim))
		lfErrorTotal = -IMaxLim;
	*Perrtotal = lfErrorTotal;

	ftemp = Kp*err;
	ftemp += lfErrorTotal;

	if(ftemp > IMaxLim)
		ftemp = IMaxLim;
	if(ftemp < (-IMaxLim))
		ftemp = -IMaxLim;


	return (ftemp);
}
/**********************************************************************/
float FunPIComCtrl_DLim(float Kp, float Ki, float err, float *Perrtotal, float MinLim, float MaxLim)
{
	float ftemp;
	float32 lfErrorTotal;

	if(Ki==0)
		*Perrtotal = 0;

	lfErrorTotal = (*Perrtotal) + (Ki * err);

	if(lfErrorTotal > MaxLim)
		lfErrorTotal = MaxLim;
	if(lfErrorTotal < -MinLim)
		lfErrorTotal = -MinLim;

	*Perrtotal = lfErrorTotal;

	ftemp = lfErrorTotal + (Kp * err);

	if(ftemp > MaxLim)
		ftemp = MaxLim;
	if(ftemp < -MinLim)
		ftemp = -MinLim;
	return ftemp;
}
/**********************************************************************/
float Freq100LPFilter(float *x, float *y, float in, int Freq)
{
	float X0, X1, X2, Y1, Y2;

	if(Freq == 3000)
	{
		X0 = 0.00952576237619545;
		X1 = 0.0190515247523909;
		X2 = 0.00952576237619545;
		Y1 = -1.70555214554408;
		Y2 = 0.743655195048865;
	}
	else if(Freq == 4000)
	{
		X0 = 0.0055427172102806817;
		X1 = 0.011085434420561363;
		X2 = 0.0055427172102806817;
		Y1 = -1.7786317778245848;
		Y2 = 0.80080264666570755;
	}
  
	x[2] = x[1];
	x[1] = x[0];
	x[0] = in;
	y[2] = y[1];
	y[1] = y[0];
	y[0] = X0*x[0];
	y[0] += X1*x[1];
	y[0] += X2*x[2];
	y[0] -= Y1*y[1];
	y[0] -= Y2*y[2];
	return (y[0]);
}
/*********************************************************/
float Freq100_700BPFilter(float *x, float *y, float in, int Freq_Samp)
{
	float X0, X1, X2, Y1, Y2;

	if(Freq_Samp == 3000)
	{
		X0 = 0.420807779837731;
		X1 = 0;
		X2 = -0.420807779837731;
		Y1 = -0.958089247000156;
		Y2 = 0.158384440324536;
	}
	else if(Freq_Samp == 4000)
	{
		X0 = 0.33754015188354675;
		X1 = 0;
		X2 = -0.33754015188354675;
		Y1 = -1.2030019100150913;
		Y2 = 0.3249196962329064;
	}

	x[2] = x[1];
	x[1] = x[0];
	x[0] = in;
	y[2] = y[1];
	y[1] = y[0];
	y[0] = X0*x[0];
	y[0] += X1*x[1];
	y[0] += X2*x[2];
	y[0] -= Y1*y[1];
	y[0] -= Y2*y[2];
	return (y[0]);
}
/*********************************************************/
float Freq40_62d5BPFilter(float *x, float *y, float in, int Freq_Samp)
{
	float X0, X1, X2, Y1, Y2;

	if(Freq_Samp == 3000)
	{
		X0 = 0.023023722046119;
		X1 = 0;
		X2 = -0.023023722046119;
		Y1 = -1.94324661846179;
		Y2 = 0.953952555907762;
	}
	else if(Freq_Samp == 4000)
	{
		X0 = 0.017366377223324458;
		X1 = 0;
		X2 = -0.017366377223324458;
		Y1 = -1.9592083518704624;
		Y2 = 0.96526724555335108;
	}
	else if(Freq_Samp == 5000)
	{
		X0 = 0.013941009325041435;
		X1 = 0;
		X2 = -0.013941009325041435;
		Y1 = -1.9682261977448705;
		Y2 = 0.97211798134991723;
	}

	x[2] = x[1];
	x[1] = x[0];
	x[0] = in;
	y[2] = y[1];
	y[1] = y[0];
	y[0]  = X0*x[0];
	y[0] += X1*x[1];
	y[0] += X2*x[2];
	y[0] -= Y1*y[1];
	y[0] -= Y2*y[2];
	return (y[0]);
}
/*********************************************************/
float Phase_90Delay_Filter(float *x, float *x_90, float in, int Freq_Samp)
{
	float X0, X1, X2, Y1, Y2;

	if(Freq_Samp == 3000)
	{
		X0 = 0.9006958008;
		X1 = -1.9006958008;
		X2 = 1;
		Y1 = -1.9006958008;
		Y2 = 0.9006958008;
	}
	else if(Freq_Samp == 4000)
	{
		X0 = 0.924511;
		X1 = -1.924418447;
		X2 = 1;
		Y1 = -1.924418447;
		Y2 = 0.924511;
	}
	else if(Freq_Samp == 5000)
	{
		X0 = 0.9390819441;
		X1 = -1.939081944;
		X2 = 1;
		Y1 = -1.939081944;
		Y2 = 0.9390819441;
	}
	
	x[2] = x[1];
	x[1] = x[0];
	x[0] = in;

	x_90[2] = x_90[1];           
	x_90[1] = x_90[0];
	
	x_90[0]  = x[0]*X0;
	x_90[0] += x[1]*X1;
	x_90[0] += x[2]*X2;
	x_90[0] -= x_90[1]*Y1;
	x_90[0] -= x_90[2]*Y2;
	return(-x_90[0]);
}
/*********************************************************************/

////end of program



