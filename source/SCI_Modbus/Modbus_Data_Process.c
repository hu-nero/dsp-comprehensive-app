#include "Custom_ModbusRTU.h"
#include "Main\Variable.H"
#include "Main\Comm_Variable.H"
#include "SCI_Modbus\Sci_Modbus_config.H"
#include "GPIO\Custom_IOMacro.H"
/***************************************************************************************************/
extern int GroupCtrl[35];

void Modbus_Send_To_HMI(void)
{
	InputRegBuf[0]  = Uab_Rmsf*10;
	InputRegBuf[1]  = Ubc_Rmsf*10;
	InputRegBuf[2]  = Uca_Rmsf*10;
	InputRegBuf[3]  = Ia_Rmsf*10;
	InputRegBuf[4]  = Ib_Rmsf*10;
	InputRegBuf[5]  = Ic_Rmsf*10;
	InputRegBuf[6]  = MPR_PsACK*10;
	InputRegBuf[7]  = -MPR_QsACK*10;
	InputRegBuf[8]  = Power_Factor*100;
	InputRegBuf[9]  = mpr_temphigh*10;
	InputRegBuf[10]  = PowerCtrlWord.DataInt;
	//
	if(MainStusWord.DataBit.MprRun == 1)
	{
		InputRegBuf[11] = FpgaMotorFreq*10;
		InputRegBuf[12] = mpr_udf*10/1.414;
	}
	else
	{
		InputRegBuf[11] = 0;
		InputRegBuf[12] = 0;
	}

//	InputRegBuf[13] = 0; //MainStusWord.DataBit.Cap40uF;

//	InputRegBuf[14] = 0; //MainStusWord.DataBit.Cap20uF;

	InputRegBuf[16] =  MainStusWord.DataInt;//MainStusWord.DataBit.WorkMode;// 1  or  2

//	InputRegBuf[15] = MainStusWord.DataBit.LocRem;

//	InputRegBuf[17] = MainStusWord.DataBit.Toggle;
	InputRegBuf[18] = MPR_SsACK*10;
	InputRegBuf[19] = 0;//PowerStatusWord.DataInt; //empty

	if(!MPR_FaultCode)
	{
		if(MainStusWord.DataBit.MprRun == 1)
		{
			InputRegBuf[20] = 1;
		}
		else
		{
			InputRegBuf[20] = 0;
		}
	}
	else
	{
		InputRegBuf[20] = 2;
	}

	InputRegBuf[21] = MPR_FaultCode;
//	InputRegBuf[22] = MainStusWord.DataBit.MprRun;
//	InputRegBuf[23] = Can_Alarm;  //can
	InputRegBuf[24] = AlarmWord1.DataInt;
	InputRegBuf[25] = AlarmWord2.DataInt;
//	InputRegBuf[26] = Reactor_temperature*10;
//	InputRegBuf[27] = MainStusWord.DataInt;

	InputRegBuf[28] = MotorVolt_Imbalance *100;
	InputRegBuf[29] = MotorCurr_Imbalance *100;
	InputRegBuf[30] = FlowRate *10;

	if(ParaDownloadCtrlWord == 0)
	{
		HoldRegBuf[0] = Idq_Loop_Kp *10000;
		HoldRegBuf[1] = Idq_Loop_Ki *10000;
		HoldRegBuf[2] = AlmThr_OverCurr_Irms;
		HoldRegBuf[3] = FltThr_OverCurr_Ipeak;
		HoldRegBuf[4] = FastChar_PhaseSel;
		HoldRegBuf[5] = Udq_Loop_Kp *10000; //Ud_Loop_Kp*1000;
		HoldRegBuf[6] = Udq_Loop_Ki *10000; //Ud_Loop_Ki*10000;
		HoldRegBuf[7] = FastChar_RiseTime;
		HoldRegBuf[8] = FastChar_DCOffset;
		HoldRegBuf[9] = FastChar_HoleTime;
		HoldRegBuf[10] = FastChar_DownTime;
		HoldRegBuf[11] = FastChar_AfterVolt;
		HoldRegBuf[12] = UsdRefRef_HMI * 10;
		HoldRegBuf[13] = MotorFreqRef_HMI *10;
//		HoldRegBuf[14] = ParaSaveFlag;
		HoldRegBuf[15] = AlmThr_OverVolt_Work1;
		HoldRegBuf[16] = FltThr_OverVolt_Work1;
		HoldRegBuf[17] = AlmThr_UnderVolt_Work1;
		HoldRegBuf[18] = FltThr_UnderVolt_Work1;
		HoldRegBuf[19] = AlmThr_OverVolt_Work2;
		HoldRegBuf[20] = FltThr_OverVolt_Work2;
		HoldRegBuf[21] = AlmThr_UnderVolt_Work2;
		HoldRegBuf[22] = FltThr_UnderVolt_Work2;
		HoldRegBuf[23] = AlmThr_OverTemp_IGBT;
		HoldRegBuf[24] = FltThr_OverTemp_IGBT;
		HoldRegBuf[25] = AlmThr_OverDvalue_Freq*10;
		HoldRegBuf[26] = FltThr_OverDvalue_Freq*10;
		HoldRegBuf[27] = Rst_Flag_HMI;
		HoldRegBuf[28] = FastChar_CtrlWord;
		HoldRegBuf[29] = Start_WaitTimeSet;
		HoldRegBuf[30] = FastChar_StopWord;
		HoldRegBuf[31] = FltThr_OverImba_Volt *1000;
		HoldRegBuf[32] = AlmThr_OverImba_Volt *1000;
		HoldRegBuf[33] = FltThr_OverImba_Curr *1000;
		HoldRegBuf[34] = AlmThr_OverImba_Curr *1000;
		HoldRegBuf[35] = Ud_cal_step *100;
		HoldRegBuf[36] = FastChar_ExitCurrSet;
		HoldRegBuf[37] = BreakCtrlWord.DataInt;
//		HoldRegBuf[38] = FilterCapCtrlWord.DataInt;
	}
}

void Modbus_Recv_For_HMI(void)
{
	if(ParaDownloadCtrlWord == 0)
	{
		Idq_Loop_Kp = HoldRegBuf[0]*0.0001;
		Idq_Loop_Ki = HoldRegBuf[1]*0.0001;
		AlmThr_OverCurr_Irms = HoldRegBuf[2];
		FltThr_OverCurr_Ipeak = HoldRegBuf[3];
		FastChar_PhaseSel = HoldRegBuf[4];
		Udq_Loop_Kp = HoldRegBuf[5]*0.0001;
		Udq_Loop_Ki = HoldRegBuf[6]*0.0001;
		FastChar_RiseTime = HoldRegBuf[7];
		FastChar_DCOffset = HoldRegBuf[8];
		FastChar_HoleTime = HoldRegBuf[9];
		FastChar_DownTime = HoldRegBuf[10];
		FastChar_AfterVolt = HoldRegBuf[11];
		UsdRefRef_HMI = HoldRegBuf[12]*0.1;
		MotorFreqRef_HMI = HoldRegBuf[13]*0.1;
		ParaSaveFlag = HoldRegBuf[14];
		AlmThr_OverVolt_Work1 = HoldRegBuf[15];
		FltThr_OverVolt_Work1 = HoldRegBuf[16];
		AlmThr_UnderVolt_Work1 = HoldRegBuf[17];
		FltThr_UnderVolt_Work1 = HoldRegBuf[18];
		AlmThr_OverVolt_Work2 = HoldRegBuf[19];
		FltThr_OverVolt_Work2 = HoldRegBuf[20];
		AlmThr_UnderVolt_Work2 = HoldRegBuf[21];
		FltThr_UnderVolt_Work2 = HoldRegBuf[22];
		AlmThr_OverTemp_IGBT = HoldRegBuf[23];
		FltThr_OverTemp_IGBT = HoldRegBuf[24];
		AlmThr_OverDvalue_Freq = HoldRegBuf[25]*0.1;
		FltThr_OverDvalue_Freq = HoldRegBuf[26]*0.1;
		Rst_Flag_HMI = HoldRegBuf[27];

		if(MainStusWord.DataBit.FastChar == 1)
		{
			HoldRegBuf[28] = 0;
			FastChar_CtrlWord = 0;
		}
		else
		{
			FastChar_CtrlWord = HoldRegBuf[28];
		}


		Start_WaitTimeSet = HoldRegBuf[29];

		if(MainStusWord.DataBit.FastChar == 0)
		{
			HoldRegBuf[30] = 0;
			FastChar_StopWord = 0;
		}
		else
		{
			FastChar_StopWord = HoldRegBuf[30];
		}

		FltThr_OverImba_Volt = HoldRegBuf[31]*0.001;
		AlmThr_OverImba_Volt = HoldRegBuf[32]*0.001;
		FltThr_OverImba_Curr = HoldRegBuf[33]*0.001;
		AlmThr_OverImba_Curr = HoldRegBuf[34]*0.001;
		Ud_cal_step = HoldRegBuf[35] *0.01;
		FastChar_ExitCurrSet = HoldRegBuf[36];
		BreakCtrlWord.DataInt = HoldRegBuf[37];
//		FilterCapCtrlWord.DataInt = HoldRegBuf[38];

		if(ParaSaveFlag != ParaSaveFlagOld)
		{
			ParaSaveFlagOld = ParaSaveFlag;
			ParaDownloadCtrlWord = 2;
			GroupCtrl[6] |= 0x0100;
			GroupCtrl[7] |= 0x0100;
			GroupCtrl[8] |= 0x0100;
			GroupCtrl[9] |= 0x0100;
			GroupCtrl[10] |= 0x0100;
			GroupCtrl[11] |= 0x0100;
			GroupCtrl[15] |= 0x0100;
		}
	}
}

