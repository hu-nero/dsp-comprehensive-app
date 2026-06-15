#include "Custom_ModbusRTU.h"
#include "Main\Variable.H"
#include "Main\Comm_Variable.H"
#include "SCI_ModbusS\Sci_Modbus_config.H"
#include "GPIO\Custom_IOMacro.H"
/***************************************************************************************************/
extern int GroupCtrl[35];

void Modbus_Send_To_HMI(void)
{
	InputRegBuf[0]  = UabSl_rmsf *10;
	InputRegBuf[1]  = UbcSl_rmsf *10;
	InputRegBuf[2]  = UcaSl_rmsf *10;
	InputRegBuf[3]  = UabPar_rmsf *10;
	InputRegBuf[4]  = UbcPar_rmsf *10;
	InputRegBuf[5]  = UcaPar_rmsf *10;
	InputRegBuf[6]  = UaOut_rmsf *10;
	InputRegBuf[7]  = UbOut_rmsf *10;
	InputRegBuf[8]  = UcOut_rmsf *10;
	InputRegBuf[9]  = UabOut_rmsf *10;
	InputRegBuf[10] = UbcOut_rmsf *10;
	InputRegBuf[11] = UcaOut_rmsf *10;
	InputRegBuf[12] = IaOut_rmsf *10;
	InputRegBuf[13] = IbOut_rmsf *10;
	InputRegBuf[14] = IcOut_rmsf *10;
	InputRegBuf[15] = P_Sys *10;
	InputRegBuf[16] = Q_Sys *10;
	InputRegBuf[17] = S_Sys *10;
	InputRegBuf[18] = PF_Sys *100;
	memcpy(&InputRegBuf[19], &MainStusWord.DataInt, 2);
	memcpy(&InputRegBuf[20], &MainSta2Word.DataInt, 2);
	memcpy(&InputRegBuf[21], &AlarmWord1.DataInt, 2);
	memcpy(&InputRegBuf[22], &AlarmWord2.DataInt, 2);
	memcpy(&InputRegBuf[23], &FaultWord1.DataInt, 2);
	memcpy(&InputRegBuf[24], &FaultWord2.DataInt, 2);
	memcpy(&InputRegBuf[25], &SwitchWord1.DataInt, 2);
	memcpy(&InputRegBuf[26], &SwitchWord2.DataInt, 2);
	memcpy(&InputRegBuf[27], &SwitchWord3.DataInt, 2);
	InputRegBuf[28]   = FlowRate*10;
	InputRegBuf[29]   = FpgaMotorFreq*10;
	InputRegBuf[30]   = SysTime_Year;
	InputRegBuf[31]   = SysTime_Month;
	InputRegBuf[32]   = SysTime_Day;
	InputRegBuf[33]   = SysTime_Hour;
	InputRegBuf[34]   = SysTime_Minute;
	InputRegBuf[35]   = SysTime_Second;
	InputRegBuf[36]   = SysTimeser_cmd;
	memcpy(&InputRegBuf[37], &FltSelfRecvWord.DataInt, 2);

	if(ParaDownloadCtrlWord == 0)
	{
		HoldRegBuf[0] = AlmCntThr_UnderVolt_Phase;
		HoldRegBuf[1] = AlmThr_UnderVolt_Phase;
		HoldRegBuf[2] = AlmCntThr_OverVolt_Work1;
		HoldRegBuf[3] = AlmThr_OverVolt_Work1;
		HoldRegBuf[4] = FltThr_OverVolt_Work1;
		HoldRegBuf[5] = AlmCntThr_OverVolt_Work2;
		HoldRegBuf[6] = AlmThr_OverVolt_Work2;
		HoldRegBuf[7] = FltThr_OverVolt_Work2;
		HoldRegBuf[8] = AlmCntThr_UnderVolt_Work1;
		HoldRegBuf[9] = AlmThr_UnderVolt_Work1;
		HoldRegBuf[10] = FltThr_UnderVolt_Work1;
		HoldRegBuf[11] = AlmCntThr_UnderVolt_Work2;
		HoldRegBuf[12] = AlmThr_UnderVolt_Work2;
		HoldRegBuf[13] = FltThr_UnderVolt_Work2;
		HoldRegBuf[14] = AlmCntThr_OverImba_Volt;
		HoldRegBuf[15] = AlmThr_OverImba_Volt * 100;
		HoldRegBuf[16] = FltCntThr_OverImba_Volt;
		HoldRegBuf[17] = FltThr_OverImba_Volt * 100;
		HoldRegBuf[18] = AlmCntThr_OverImba_Curr;
		HoldRegBuf[19] = AlmThr_OverImba_Curr * 100;
		HoldRegBuf[20] = FltCntThr_OverImba_Curr;
		HoldRegBuf[21] = FltThr_OverImba_Curr * 100;
		HoldRegBuf[22] = AlmCntThr_OverCurr_Work1;
		HoldRegBuf[23] = AlmThr_OverCurr_Work1;
		HoldRegBuf[24] = FltThr_OverCurr_Work1;
		HoldRegBuf[25] = AlmCntThr_OverCurr_Work2;
		HoldRegBuf[26] = AlmThr_OverCurr_Work2;
		HoldRegBuf[27] = FltThr_OverCurr_Work2;
		HoldRegBuf[28] = FltCntThr_OverLow_Flow1;
		HoldRegBuf[29] = FltThr_OverLow_Flow1;
		HoldRegBuf[30] = FltCntThr_OverLow_Flow2;
		HoldRegBuf[31] = FltThr_OverLow_Flow2;
		HoldRegBuf[32] = Auto_Restarttime_Flow;
		HoldRegBuf[33] = Cap_Ctrl_Word.DataInt;
		HoldRegBuf[34] = Can_External_Addr;
		HoldRegBuf[35] = FltThr_UnderCurr_Silicon;
		HoldRegBuf[36] = FltThr_OverCurr_Silicon;
		HoldRegBuf[37] = Auto_Restarttime_Self;
		HoldRegBuf[38] = Auto_Restarttime_Para_Request;
		HoldRegBuf[39] = SysTimeser_back;
	}
}

void Modbus_Recv_For_HMI(void)
{
	if(ParaDownloadCtrlWord == 0)
	{
		AlmCntThr_UnderVolt_Phase = HoldRegBuf[0];
		AlmThr_UnderVolt_Phase = HoldRegBuf[1];
		AlmCntThr_OverVolt_Work1 = HoldRegBuf[2];
		AlmThr_OverVolt_Work1 = HoldRegBuf[3];
		FltThr_OverVolt_Work1 = HoldRegBuf[4];
		AlmCntThr_OverVolt_Work2 = HoldRegBuf[5];
		AlmThr_OverVolt_Work2 = HoldRegBuf[6];
		FltThr_OverVolt_Work2 = HoldRegBuf[7];
		AlmCntThr_UnderVolt_Work1 = HoldRegBuf[8];
		AlmThr_UnderVolt_Work1 = HoldRegBuf[9];
		FltThr_UnderVolt_Work1 = HoldRegBuf[10];
		AlmCntThr_UnderVolt_Work2 = HoldRegBuf[11];
		AlmThr_UnderVolt_Work2 = HoldRegBuf[12];
		FltThr_UnderVolt_Work2 = HoldRegBuf[13];
		AlmCntThr_OverImba_Volt = HoldRegBuf[14];
		AlmThr_OverImba_Volt = HoldRegBuf[15] *0.01;
		FltCntThr_OverImba_Volt = HoldRegBuf[16];
		FltThr_OverImba_Volt = HoldRegBuf[17] *0.01;
		AlmCntThr_OverImba_Curr = HoldRegBuf[18];
		AlmThr_OverImba_Curr = HoldRegBuf[19] *0.01;
		FltCntThr_OverImba_Curr = HoldRegBuf[20];
		FltThr_OverImba_Curr = HoldRegBuf[21] *0.01;
		AlmCntThr_OverCurr_Work1 = HoldRegBuf[22];
		AlmThr_OverCurr_Work1 = HoldRegBuf[23];
		FltThr_OverCurr_Work1 = HoldRegBuf[24];
		AlmCntThr_OverCurr_Work2 = HoldRegBuf[25];
		AlmThr_OverCurr_Work2 = HoldRegBuf[26];
		FltThr_OverCurr_Work2 = HoldRegBuf[27];
		FltCntThr_OverLow_Flow1 = HoldRegBuf[28];
		FltThr_OverLow_Flow1 = HoldRegBuf[29];
		FltCntThr_OverLow_Flow2 = HoldRegBuf[30];
		FltThr_OverLow_Flow2 = HoldRegBuf[31];
		Auto_Restarttime_Flow = HoldRegBuf[32];
		Cap_Ctrl_Word.DataInt = HoldRegBuf[33];
		Can_External_Addr = HoldRegBuf[34];
		FltThr_UnderCurr_Silicon = HoldRegBuf[35];
		FltThr_OverCurr_Silicon = HoldRegBuf[36];
		Auto_Restarttime_Self = HoldRegBuf[37];
		Auto_Restarttime_Para_Request = HoldRegBuf[38];
		SysTimeser_back = HoldRegBuf[39];


		ParaSaveFlag = Cap_Ctrl_Word.DataBit.Bit4;
		if(ParaSaveFlag != ParaSaveFlagOld)
		{
			ParaSaveFlagOld = ParaSaveFlag;
			ParaDownloadCtrlWord = 2;
			GroupCtrl[5] |= 0x0100;
			GroupCtrl[6] |= 0x0100;
		}
	}
}

