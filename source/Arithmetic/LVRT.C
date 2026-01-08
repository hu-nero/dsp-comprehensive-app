#include "main\Variable.h"
#include "Main\Custom_GlobalMacro.H"
#include "IQmathlib.H"
#include "math.h"
extern void  Dis_PWM(void);
extern void  En_PWM(void);
/******************电网电压判断各种标志位******************/
void GridVolGetFlagFun(void)
{
///////////// 跌落深度计算 //////////////////////////////////////////
	if(GridSideVoltage > 0)
		lvrt_deepth = vLine_min/GridSideVoltage;// 跌落深度Q12定标
	else
		lvrt_deepth = 0;

	if(lvrt_deepth < LVRT_In_Thr)
	{
		if(LVRT_SwCount < 10)
			LVRT_SwCount += 8;
		else
			LVRTstate = 1;
	}
	else if(lvrt_deepth > LVRT_Out_Thr)
	{
		if(LVRT_SwCount > 0)
			LVRT_SwCount=-8;
		else
			LVRTstate = 0;
	}

/////////////// 电网不平衡 ///////////////////////////////////////
	if(GridVolt_Imbalance > UgImBal_HThr)   // 0.15, Q12
	{
		if(UgImBal_Count < 10)
			UgImBal_Count ++;
		else
			UgImBal_Flag = 1;          // 电网不平衡标志
	}
	else if(GridVolt_Imbalance < UgImBal_LThr)    // 0.1, Q12
	{
		if(UgImBal_Count > 0)
			UgImBal_Count--;
		else
			UgImBal_Flag = 0;          // 电网不平衡标志
	}
///////////////////////////////////////////////////////////////////////
	if((MainStusWord.DataBit.PWM==0) && (PWM_Modu_FlagOld==1))
	if(LVRTstate == 0)
	{
		LVRTstate = 1;
		LVRTstate_count = 1;
	}
	PWM_Modu_FlagOld = MainStusWord.DataBit.PWM;


	if(LVRTstate_count != 0)
		LVRTstate_count++;
	if(LVRTstate_count >= 10000)
		LVRTstate_count = 0;
/////////////////////////////////////////////////////
}
/************低电压穿越封脉冲*************************/
void PWM_Lock(void)
{
	SINT iac_lock_temp;
	if(LVRTstate == 0)
		iac_lock_temp = iac_lockThr;
	else
		iac_lock_temp = iac_lockThr + 3;

	if(ipeak > iac_lock_temp)
	{
		if(LockFlag == 0)
		{
			LockFlag = 1;
			LockFlag_count += 500;
		}
	}
	else
	{
		if(LockFlag == 1)
			LockFlag = 0;
	}
////////// 手动封脉冲////////////	
	if(UnloadCtrl_PC == 3)
	{
		LockFlag = 10;
		LockFlag_count += 500;
		UnloadCtrl_PC = 0;
	}
	else
	{
		if(LockFlag == 10)
			LockFlag = 0;
	}
////////// 执行封脉冲////////////	  
	if(MainStusWord.DataBit.PWM == 1)
	{
		if(LockFlag != 0)
		{
			Dis_PWM();
			MainStusWord.DataBit.PWM = 0;
			PWMEnCount = 0;
		}
	}
//////////////////////////////////////////////////
}
/******************脉冲使能******************************************/
void PWM_Unlock(void)
{
	if(MainCtrlWord.DataBit.PWMON == 1)
	if(MainStusWord.DataBit.PWM == 0) // 只开一次脉冲即可
	{
		PWMEnCount++;
		if(PWMEnCount > lock_time)
		{
			PWMEnCount = lock_time;
			if(ipeak < iac_unlock)
			if(LockFlag == 0)
			{
				En_PWM();      // NPR start run
				MainStusWord.DataBit.PWM = 1;
			}
		}
	}
}
/*********************************************************************/
float LVRT_IqRef(float iq) //Q8
{
	float IqRefRtn;
	float dtemp;

	IqRefRtn = iq;

	if(LVRTstate == 1)
	{
		dtemp = 0.9 - lvrt_deepth;	//Q12  0.9-deep
		dtemp = dtemp* LVRT_k; 	//Q12
		dtemp = dtemp*GridSideCurrent; //Q12+8-12=Q8
		dtemp = dtemp*1.732;	//Q8+12-12=Q8  *1.732
		IqRefRtn = -dtemp;
	}
	else
	{
		IqRefRtn=0;
	}


	return IqRefRtn;
}
/*********************************************************************/
//float LVRT_Iqn_Ref(float IqnRef)
//{
//	float itemp;
//	float deepth_n, deepth_p;
//	float IqnRefRtn;
//	static float tempCnt=0;
//
//	IqnRefRtn = IqnRef;
//
//	itemp =  npr_udn*npr_udn + npr_uqn*npr_uqn;
//	itemp = sqrtf(itemp);
//	itemp = itemp*0.5;
//	deepth_n = itemp/GridSideVoltage;	//Q3-3+12 = Q12
//
//	itemp = npr_udp*npr_udp + npr_uqp*npr_uqp;
//	itemp = sqrtf(itemp);
//	itemp = itemp*0.5;
//	deepth_p = itemp/ GridSideVoltage;	//Q3-3+12 = Q12
//
//	if( (deepth_p >= 0.58) && (deepth_p <= 0.82) )	//Q12 0.58~0.82
//	{
//		tempCnt++;
//		if(tempCnt > 10)
//		{
//			itemp = deepth_n*1.5; 	//Q12+2-6=Q8  1.5*LVRT_deepN
//			itemp = itemp*GridSideCurrent; //Q8+8-8=Q8  1.5*LVRT_deepN*I
//			IqnRefRtn = itemp;
//		}
//	}
//	else
//	{
//		tempCnt-=5;
//	}
//	return IqnRefRtn;
//}
