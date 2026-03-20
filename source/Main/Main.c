//////////////
// main.c
#include "DSP28x_Project.h"
#include "System\Custom_System.H"
#include "IQmathlib.H"
#include "SPI_EEPROM\Custom_SPI_EEPROM.h"
#include "Main\Custom_GlobalMacro.H"
#include "Main\Comm_Variable.H"
#include "main\Variable.h"
#include "math.h"
#include "GPIO\Custom_IOMacro.H"
#include "Arithmetic\Arithmetic.H"
#include "ECANA\Pub_Custom_ECan.H"
#include "ECANA\Custom_ECanA.H"
#include "ECANB\Custom_ECanB.H"
#include "EPWM\Custom_PWM.H"
#include "ADCInner\Custom_InnerADC.H"
#include "Xintf\Custom_Xintf_ADC&IO.H"
#include "DMA/DMA.h"
#include "MCBSP_Flash\MCBSPB_Rom.H"
#include "MCBSP_Flash\Flash_Comm.H"
#include "SCI_monitor\EventLog.H"
#include "SCI_monitor\DataLog.H"
#include "SCI_monitor\MessageParse.h"
#include "SCI_monitor\version.h"
#include "SCI_monitor\ParaDefine.h"
#include "SCI_monitor\softscope.h"
#include "SCI_monitor\ParaDeal.h"
#include "SCI_monitor\SciConfig.h"
#include "SCI_Modbus\Sci_Modbus_Config.H"
#include "SCI_Modbus\Custom_ModbusRTU.H"
#include "SCI_ModbusM\Custom_Remote.H"

#include "Hal.h"
#include "App.h"

extern unsigned int RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart;
void INT28335_process(void);
void pwm_init(void);
/******************************************************************/
interrupt void EPWM1_INT_Underflow_isr(void);
/******************************************************************/
void FunFaultReact(void);
void FunResetAct(void);
void FunStatusUpdate(void);

void npr_1ms_protection(void);

void GetVIRms(void);
void CalImbalance(void);
void GetPQ2r(void);
void RealTime20msFun(void);
void GetFlowRate_Calc(void);

void main(void)
{
	int i,j;

	DINT;
	InitSysCtrl();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	memcpy( &RamfuncsRunStart,
			&RamfuncsLoadStart,
			&RamfuncsLoadEnd - &RamfuncsLoadStart);
	InitFlash();
	GPIO_init();

	for(i=0;i<1000;i++)
		for(j=0;j<10000;j++);

	InitXintf();
	Sci_Monitor_init(&ScibRegs);
	Software_oscilloscopeInit();
	Sci_Modbus_init(&ScicRegs, 375e5, 9600);
	Sci_Remote_init(&SciaRegs, 375e5, 9600);
	InitSpi();
	DMA_Init();
	MCBSPB_Init();
	Zone7_Init();
	InitAdc();

	pwm_init();
	INT28335_process();
	CAN_App_Init();

	variable_init();

	EALLOW;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm1Regs.TZCLR.bit.INT = 1;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EDIS;

	EINT;

	Dis_PWM();
	ParaDownloadCtrlWord=1;
	CAN_App_Test();
	for(; ;)
	{
        CAN_App_MainLoop();
		EINT;
///////////////////////////////////////////////////////////////////////////////
		Sci_Monitor_RxData(&ScibRegs, SCIBRecvBuffer, &SCIBRecvTail);
		Monitor_Rx_Protocol(SCIBRecvBuffer, &SCIBRecvFront, SCIBRecvTail, SCIBSendBuffer, &SCIBSendTail);
		Software_oscilloscope(SCIBSend_1ms, SCIBSendBuffer, &SCIBSendTail);
		Sci_Monitor_TxData(&ScibRegs, SCIBSendBuffer, &SCIBSendFront, SCIBSendTail, SCIBSend_1ms);
///////////////////////////////////////////////////////////////////////////////
		if(Modbus_SendFlag == 1)
		{
			Modbus_Send_To_HMI();
			Sci_Modbus_RxData(&ScicRegs, Modbus_RecvBuffer, &Modbus_RecvTail, Modbus_Recv_BUFF_SIZE);
			ModbusRTU_Protocol(Modbus_RecvBuffer, &Modbus_RecvFront, Modbus_RecvTail, Modbus_Recv_BUFF_SIZE, Modbus_SendBuffer);
		    Modbus_Recv_For_HMI();
		    Modbus_SendFlag = 0;
		}
///////////////////////////////////////////////////////////////////////////////
		Sci_Remote_RxData(&ScicRegs, MbsRem_RecvBuffer, &MbsRem_RecvTail, Modbus_Recv_BUFF_SIZE);
		ModbusRem_Protocol(MbsRem_RecvBuffer, &MbsRem_RecvFront, MbsRem_RecvTail, Modbus_Recv_BUFF_SIZE, MbsRem_SendBuffer);
///////////////////////////////////////////////////////////////////////////////
		Flag_EEPROMCheck = EEPROMAccess(&ParaDownloadCtrlWord, &EEPROMAceNum);
		if(Flag_EEPROMCheck == 30)
		{
			if(FaultMask_Map(40) == 1)
			{
				if(NPR_FaultCode == 0)
				{
					FunFaultReact();
					ParaDownloadFaultNum = EEPROMAceNum - 1;
					NPR_FaultCode = 40;
				}
			}
			FaultWord_Map(40);
		}

		if(CtrlCnt_LedTogg > 500)
		{
			CtrlCnt_LedTogg = 0;
			IO_DSPLED_TOG;
			GetFlowRate_Calc();
		}

		if(RealTime1ms >= 20)
		{
			RealTime20msFun();
			RealTime1ms = 0;
		}
//////////////////////////////////////////////////////////////////////////////
		if(SysCnt_1s != SysCnt_1sOld)
		{
			SysCnt_1sOld = SysCnt_1s;
			npr_1ms_protection();
		}
		//////////////////////////////////////////////////////////////////////////////
		if(SysCnt_1ms != SysCnt_1msOld)
		{
			SysCnt_1msOld = SysCnt_1ms;

			IO_Scan();
			Source_Select();
			Inner_ADCScan();
			CalImbalance();
			FunStatusUpdate();
			FunResetAct();
///////////////////////////////////////////////////////////////////////////////////////////
			//Ctrl Logic
			if(Status_NPRRun == 0)
			{
				if(SysCnt_1ms >= 500)
				{
					if(NPR_FaultCode == 0)
					{
						ParaDownloadCtrlWord = 1;
						EEPROMAceNum = 1;
						SysCnt_1ms = 0;
						Status_NPRRun = 1;
					}
				}
			}
	//////////////////////////////////////////////////////////////////////////////////////////
		}
	}
}
/******************************************************************/
void INT28335_process(void)
{
	InitPieCtrl();
	IER = 0x0000;    // Disable CPU interrupts and clear all CPU interrupt flags:
	IFR = 0x0000;
	InitPieVectTable();

	EALLOW;
	PieVectTable.EPWM1_INT = &EPWM1_INT_Underflow_isr;
	EDIS;

	IER |= M_INT2;   // Enable EPWM INTn in the PIE: Group 2 interrupt 1-2
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;

	IER |= M_INT3;   // PWM1(PWM) PWM2(ADSample)
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

	EnableInterrupts();
	Dis_PWM();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}
/******************************************************************/
void pwm_init(void)
{
	Dis_PWM();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	InitEPwm1();
	InitEPwm2();
	InitEPwm3();
	EDIS;
}
/******************************************************************/
void  RealTime20msFun(void)
{
	UINT temp = 0;
	UINT SysTimeSecond = 0;
	UINT SysTime_Minute = 0;
	UINT SysTime_Hour = 0;
	UINT SysTime_Day = 0;
	UINT SysTime_Month = 0;
	UINT SysTimeYearL = 0;
	/******************************************/
	MainStusWord.DataBit.Toggle ^= 1;

	temp = (Time_L & 0x000000FF);
	SysTimeSecond = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTimeSecond>59)
		SysTimeSecond=59;

	temp = ((Time_L & 0x0000FF00) >> 8);
	SysTime_Minute = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTime_Minute>59)
		SysTime_Minute=59;
	
	temp = ((Time_L & 0x00FF0000) >> 16);
	SysTime_Hour = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTime_Hour>23)
		SysTime_Hour=23;
	
	temp = ((Time_L & 0xFF000000) >> 24);
	SysTime_Day = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTime_Day>31)
		SysTime_Day=31;
	
	temp = Time_H & 0x000000FF;
	SysTime_Month = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTime_Month>12)
		SysTime_Month=12;
	
	temp = ((Time_H & 0x0000FF00) >> 8);
	SysTimeYearL = (((temp & 0x00F0) >> 4) * 10 + (temp & 0x000F));
	if(SysTimeYearL>63)
		SysTimeYearL=63;
	
	ulRealTimeSecond  = (((long)SysTimeYearL) <<26)&0xFC000000;
	ulRealTimeSecond |= ( (long)SysTime_Month <<22)&0x03C00000;
	ulRealTimeSecond |= ( (long)SysTime_Day   <<17)&0x003E0000;
	ulRealTimeSecond |= ( (long)SysTime_Hour  <<12)&0x0001F000;
	ulRealTimeSecond |= (       SysTime_Minute<<6 )&0x00000FC0;
	ulRealTimeSecond |= SysTimeSecond              &0x003F;
}
/******************************************************************/
void FunFaultReact(void)
{

}
/******************************************************************/
void FunResetAct(void)
{
	MainCtrlWord.DataBit.Reset = MainCtrlWord_PC.DataBit.Reset;
	if(MainStusWord.DataBit.Running == 1)
		MainCtrlWord.DataBit.Reset = 0;

	if(MainCtrlWord.DataBit.Reset == 1)
	{
		MainCtrlWord.DataInt = 0;
		MainCtrlWord.DataBit.Reset = 1;

		MainCtrlWord_PC.DataInt = 0;
		MainCtrlWord_PC.DataBit.Reset = 1;

		MainCtrlWord_CAN.DataInt = 0;
		MainCtrlWord_CAN.DataBit.Reset = 1;

		IO_FPGARST_EN;

		if(CtrlCnt_RstDelay == 0)
		{
			CtrlCnt_RstDelay = 1;
		}
	}

	if(CtrlCnt_RstDelay != 0)
	{
		CtrlCnt_RstDelay ++;
		if(CtrlCnt_RstDelay < 100)
		{
			MainCtrlWord.DataBit.Reset = 1;
		}
		if(CtrlCnt_RstDelay > 1500)
		{
			CtrlCnt_RstDelay = 0;

			MainCtrlWord.DataBit.Reset=0;
			MainCtrlWord_PC.DataBit.Reset=0;
			MainCtrlWord_CAN.DataBit.Reset=0;
		}
	}

	if((MainCtrlWord.DataBit.Reset == 0) && ((Flag_RstCmdOld & 0x0001) == 1))
	{
		if(MainStusWord.DataBit.Running == 0)
		{
			IO_PWM_UNLOCK;

			IO_FPGARST_DIS;
		}
	}
	Flag_RstCmdOld = MainCtrlWord.DataBit.Reset;
}
/******************************************************************/
void FunStatusUpdate(void)
{
	MainStusWord.DataBit.LocRem = IO_LocRem_Feed;
	MainStusWord.DataBit.Estop = IO_Estop_Feed;
	MainSta2Word.DataBit.SingQF1 = IO_SingQF1_Feed;
	MainSta2Word.DataBit.ParaQF2 = IO_ParaQF2_Feed;
	MainSta2Word.DataBit.WorkLpQS1 = IO_WorkLpQS1_Feed;
	MainSta2Word.DataBit.CheckLpQS1 = IO_CheckLpQS1_Feed;
	MainSta2Word.DataBit.CompCapQS2 = IO_CompCapQS2_Feed;
	MainStusWord.DataBit.PwrUps = IO_PwrUps_Feed;
	MainStusWord.DataBit.PwrGrid = IO_PwrGrid_Feed;
	MainSta2Word.DataBit.DgssKM1 = IO_DgssKM1_Feed;
	MainStusWord.DataBit.ShockCarIn = IO_ShockCar_Feed;
	MainStusWord.DataBit.CompCarIn = IO_CompCar_Feed;
	MainSta2Word.DataBit.DgssQS3 = IO_DgssQS3_Feed;
	MainSta2Word.DataBit.SingQR1 = IO_SingQR1_Feed;
	MainSta2Word.DataBit.ParaQR2 = IO_ParaQR2_Feed;

	if(NPR_FaultCode)
	{
		MainStusWord.DataBit.Fault = 1;
		MainStusWord.DataBit.Running = 0;
	}
	else
	{
		MainStusWord.DataBit.Fault = 0;

		if(MainSta2Word.DataBit.WorkLpQS1 & MainSta2Word.DataBit.CheckLpQS1)
		{
			MainStusWord.DataBit.Running = 1;
		}
		else
		{
			MainStusWord.DataBit.Running = 0;
		}
	}

}
/******************************************************************/
void npr_1ms_protection(void)
{
	//
	if(vphase_min < AlmThr_UnderVolt_Phase)
	{
		if(AlmCnt_UnderVolt_Phase < AlmCntThr_UnderVolt_Phase)
		{
			AlmCnt_UnderVolt_Phase ++;
			AlarmWord_Map(1);
		}
	}
	else
	{
		AlmCnt_UnderVolt_Phase = 0;
		AlarmWord_Clear(1);
	}
	/////////////////////////////////////////////
	if(vLine_max > AlmThr_OverVolt_Work1)
	{
		AlarmWord_Map(2);

		AlmCnt_OverVolt_Work1 ++;
		if(AlmCnt_OverVolt_Work1 > AlmCntThr_OverVolt_Work1)
		{
			NPR_FaultCode = 17;
			FaultWord_Map(17);
		}

		if(vLine_max > FltThr_OverVolt_Work1)
		{
			NPR_FaultCode = 17;
			FaultWord_Map(17);
		}
	}
	else
	{
		AlarmWord_Clear(2);
		AlmCnt_OverVolt_Work1 = 0;
	}
	/////////////////////////////////////////////
	if(vLine_max > AlmThr_OverVolt_Work2)
	{
		AlarmWord_Map(3);

		AlmCnt_OverVolt_Work2 ++;
		if(AlmCnt_OverVolt_Work2 > AlmCntThr_OverVolt_Work2)
		{
			NPR_FaultCode = 18;
			FaultWord_Map(18);
		}

		if(vLine_max > FltThr_OverVolt_Work2)
		{
			NPR_FaultCode = 18;
			FaultWord_Map(18);
		}
	}
	else
	{
		AlarmWord_Clear(3);
		AlmCnt_OverVolt_Work2 = 0;
	}
	/////////////////////////////////////////////
	if(vLine_min < AlmThr_UnderVolt_Work1)
	{
		AlmCnt_UnderVolt_Work1 ++;
		if(AlmCnt_UnderVolt_Work1 > AlmCntThr_UnderVolt_Work1)
		{
			AlarmWord_Map(4);
		}

		if(vLine_min < FltThr_UnderVolt_Work1)
		{
			NPR_FaultCode = 19;
			FaultWord_Map(19);
		}
	}
	else
	{
		AlarmWord_Clear(4);
		AlmCnt_UnderVolt_Work1 = 0;
	}
	/////////////////////////////////////////////
	if(vLine_min < AlmThr_UnderVolt_Work2)
	{
		AlmCnt_UnderVolt_Work2 ++;
		if(AlmCnt_UnderVolt_Work1 > AlmCntThr_UnderVolt_Work2)
		{
			AlarmWord_Map(5);
		}

		if(vLine_min < FltThr_UnderVolt_Work2)
		{
			NPR_FaultCode = 20;
			FaultWord_Map(20);
		}
	}
	else
	{
		AlarmWord_Clear(5);
		AlmCnt_UnderVolt_Work2 = 0;
	}
//////////////////////////////////////////////////
	if(Uout_Imbalance > AlmThr_OverImba_Volt)
	{
		AlmCnt_OverImba_Volt ++;
		if(AlmCnt_OverImba_Volt > AlmCntThr_OverImba_Volt)
		{
			AlarmWord_Map(6);
		}
	}
	else
	{
		AlmCnt_OverImba_Volt = 0;
		AlarmWord_Clear(6);
	}

	if(Uout_Imbalance > FltThr_OverImba_Volt)
	{
		if(FltCnt_OverImba_Volt < FltCntThr_OverImba_Volt)
		{
			FltCnt_OverImba_Volt ++;
		}
		else
		{
			NPR_FaultCode = 21;
			FaultWord_Map(21);
		}
	}
	else
	{
		if(FltCnt_OverImba_Volt > 0)
		{
			FltCnt_OverImba_Volt --;
		}
	}
	////////////////////////////////////////////////////////
	if(Iout_Imbalance > AlmThr_OverImba_Curr)
	{
		AlmCnt_OverImba_Curr ++;
		if(AlmCnt_OverImba_Curr > AlmCntThr_OverImba_Curr)
		{
			AlarmWord_Map(6);
		}
	}
	else
	{
		AlmCnt_OverImba_Curr = 0;
		AlarmWord_Clear(6);
	}

	if(Iout_Imbalance > FltThr_OverImba_Curr)
	{
		if(FltCnt_OverImba_Curr < FltCntThr_OverImba_Curr)
		{
			FltCnt_OverImba_Curr ++;
		}
		else
		{
			NPR_FaultCode = 22;
			FaultWord_Map(22);
		}
	}
	else
	{
		if(FltCnt_OverImba_Curr > 0)
		{
			FltCnt_OverImba_Curr --;
		}
	}
//////////////////////////////////////////////////
	if(Iphase_max > AlmThr_OverCurr_Work1)
	{
		AlmCnt_OverCurr_Work1 ++;
		if(AlmCnt_OverCurr_Work1 > AlmCntThr_OverCurr_Work1)
		{
			AlarmWord_Map(8);
		}

		if(Iphase_max > FltThr_OverCurr_Work1)
		{
			NPR_FaultCode = 23;
			FaultWord_Map(23);
		}
	}
	else
	{
		AlarmWord_Clear(8);
		AlmCnt_OverCurr_Work1 = 0;
	}

	if(Iphase_max > AlmThr_OverCurr_Work2)
	{
		AlmCnt_OverCurr_Work2 ++;
		if(AlmCnt_OverCurr_Work2 > AlmCntThr_OverCurr_Work2)
		{
			AlarmWord_Map(9);
		}

		if(Iphase_max > FltThr_OverCurr_Work2)
		{
			NPR_FaultCode = 24;
			FaultWord_Map(24);
		}
	}
	else
	{
		AlarmWord_Clear(9);
		AlmCnt_OverCurr_Work2 = 0;
	}
	//////////////////////////////////////////////////////////
	if(FlowRate > FltThr_OverLow_Flow1)
	{
		AlarmWord_Map(10);

		if(AlmCnt_OverLow_Flow1 < AlmCntThr_OverLow_Flow1)
		{
			AlmCnt_OverLow_Flow1 ++;
		}
		else
		{
			NPR_FaultCode = 25;
			FaultWord_Map(25);
		}
	}
	else
	{
		AlarmWord_Clear(10);

		if(AlmCnt_OverLow_Flow1 > 0)
		{
			AlmCnt_OverLow_Flow1 --;
		}
	}

	if(FlowRate > FltThr_OverLow_Flow2)
	{
		AlarmWord_Map(10);

		if(AlmCnt_OverLow_Flow2 < AlmCntThr_OverLow_Flow2)
		{
			AlmCnt_OverLow_Flow2 ++;
		}
		else
		{
			NPR_FaultCode = 25;
			FaultWord_Map(25);
		}
	}
	else
	{
		AlarmWord_Clear(10);

		if(AlmCnt_OverLow_Flow2 > 0)
		{
			AlmCnt_OverLow_Flow2 --;
		}
	}
	//////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.PwrGrid)
	{
		AlarmWord_Clear(11);
	}
	else
	{
		AlarmWord_Map(11);
	}

	if(MainStusWord.DataBit.PwrUps)
	{
		AlarmWord_Clear(12);
	}
	else
	{
		AlarmWord_Map(12);
	}
	////////////////////////////////////////////////////
	if(MainStusWord.DataBit.Estop)
	{
		NPR_FaultCode = 26;
		FaultWord_Map(26);
	}
}
/******************************************************************/
void GetFlowRate_Calc(void)
{
	float ftemp;
	if(uFlowRate < 0.4)
		uFlowRate = 0.4;

	if(uFlowRate > 2)
		uFlowRate = 2;

	ftemp = uFlowRate *54.46 - 20.6;

	FlowRate = FlowRate*0.9 + ftemp*0.1;
}
/******************************************************************/
void GetVIRms(void)
{
	// ===================== 1. UabSl/UbcSl/UcaSl �?=====================
	UabSl_rms -= UabSl_HalfMemory[SysCnt_10msRmsCalc];
	UbcSl_rms -= UbcSl_HalfMemory[SysCnt_10msRmsCalc];
	UcaSl_rms -= UcaSl_HalfMemory[SysCnt_10msRmsCalc];

	UabSl_HalfMemory[SysCnt_10msRmsCalc] = fabs(UabSl * 0.0555);
	UbcSl_HalfMemory[SysCnt_10msRmsCalc] = fabs(UbcSl * 0.0555);
	UcaSl_HalfMemory[SysCnt_10msRmsCalc] = fabs(UcaSl * 0.0555);

	UabSl_rms += UabSl_HalfMemory[SysCnt_10msRmsCalc];
	UbcSl_rms += UbcSl_HalfMemory[SysCnt_10msRmsCalc];
	UcaSl_rms += UcaSl_HalfMemory[SysCnt_10msRmsCalc];

	UabSl_rmsf = UabSl_rmsf * 0.875 + UabSl_rms * 0.125;
	UbcSl_rmsf = UbcSl_rmsf * 0.875 + UbcSl_rms * 0.125;
	UcaSl_rmsf = UcaSl_rmsf * 0.875 + UcaSl_rms * 0.125;
	// ===================== 2. UabPar/UbcPar/UcaPar �?=====================
	UabPar_rms -= UabPar_HalfMemory[SysCnt_10msRmsCalc];
	UbcPar_rms -= UbcPar_HalfMemory[SysCnt_10msRmsCalc];
	UcaPar_rms -= UcaPar_HalfMemory[SysCnt_10msRmsCalc];

	UabPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(UabPar * 0.0555);
	UbcPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(UbcPar * 0.0555);
	UcaPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(UcaPar * 0.0555);

	UabPar_rms += UabPar_HalfMemory[SysCnt_10msRmsCalc];
	UbcPar_rms += UbcPar_HalfMemory[SysCnt_10msRmsCalc];
	UcaPar_rms += UcaPar_HalfMemory[SysCnt_10msRmsCalc];

	UabPar_rmsf = UabPar_rmsf * 0.875 + UabPar_rms * 0.125;
	UbcPar_rmsf = UbcPar_rmsf * 0.875 + UbcPar_rms * 0.125;
	UcaPar_rmsf = UcaPar_rmsf * 0.875 + UcaPar_rms * 0.125;
	// ===================== 3. IaPar/IbPar/IcPar �?=====================
	IaPar_rms -= IaPar_HalfMemory[SysCnt_10msRmsCalc];
	IbPar_rms -= IbPar_HalfMemory[SysCnt_10msRmsCalc];
	IcPar_rms -= IcPar_HalfMemory[SysCnt_10msRmsCalc];

	IaPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(IaPar * 0.0555);
	IbPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(IbPar * 0.0555);
	IcPar_HalfMemory[SysCnt_10msRmsCalc] = fabs(IcPar * 0.0555);

	IaPar_rms += IaPar_HalfMemory[SysCnt_10msRmsCalc];
	IbPar_rms += IbPar_HalfMemory[SysCnt_10msRmsCalc];
	IcPar_rms += IcPar_HalfMemory[SysCnt_10msRmsCalc];

	IaPar_rmsf = IaPar_rmsf * 0.875 + IaPar_rms * 0.125;
	IbPar_rmsf = IbPar_rmsf * 0.875 + IbPar_rms * 0.125;
	IcPar_rmsf = IcPar_rmsf * 0.875 + IcPar_rms * 0.125;
	// ===================== 4. IaOvh/IbOvh/IcOvh �?=====================
	IaOvh_rms -= IaOvh_HalfMemory[SysCnt_10msRmsCalc];
	IbOvh_rms -= IbOvh_HalfMemory[SysCnt_10msRmsCalc];
	IcOvh_rms -= IcOvh_HalfMemory[SysCnt_10msRmsCalc];

	IaOvh_HalfMemory[SysCnt_10msRmsCalc] = fabs(IaOvh * 0.0555);
	IbOvh_HalfMemory[SysCnt_10msRmsCalc] = fabs(IbOvh * 0.0555);
	IcOvh_HalfMemory[SysCnt_10msRmsCalc] = fabs(IcOvh * 0.0555);

	IaOvh_rms += IaOvh_HalfMemory[SysCnt_10msRmsCalc];
	IbOvh_rms += IbOvh_HalfMemory[SysCnt_10msRmsCalc];
	IcOvh_rms += IcOvh_HalfMemory[SysCnt_10msRmsCalc];

	IaOvh_rmsf = IaOvh_rmsf * 0.875 + IaOvh_rms * 0.125;
	IbOvh_rmsf = IbOvh_rmsf * 0.875 + IbOvh_rms * 0.125;
	IcOvh_rmsf = IcOvh_rmsf * 0.875 + IcOvh_rms * 0.125;
	// ===================== 5. UaOut/UbOut/UcOut �?=====================
	UaOut_rms -= UaOut_HalfMemory[SysCnt_10msRmsCalc];
	UbOut_rms -= UbOut_HalfMemory[SysCnt_10msRmsCalc];
	UcOut_rms -= UcOut_HalfMemory[SysCnt_10msRmsCalc];

	UaOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UaOut * 0.0555);
	UbOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UbOut * 0.0555);
	UcOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UcOut * 0.0555);

	UaOut_rms += UaOut_HalfMemory[SysCnt_10msRmsCalc];
	UbOut_rms += UbOut_HalfMemory[SysCnt_10msRmsCalc];
	UcOut_rms += UcOut_HalfMemory[SysCnt_10msRmsCalc];

	UaOut_rmsf = UaOut_rmsf * 0.875 + UaOut_rms * 0.125;
	UbOut_rmsf = UbOut_rmsf * 0.875 + UbOut_rms * 0.125;
	UcOut_rmsf = UcOut_rmsf * 0.875 + UcOut_rms * 0.125;
	// ===================== 6. UabOut/UbcOut/UcaOut �?=====================
	UabOut_rms -= UabOut_HalfMemory[SysCnt_10msRmsCalc];
	UbcOut_rms -= UbcOut_HalfMemory[SysCnt_10msRmsCalc];
	UcaOut_rms -= UcaOut_HalfMemory[SysCnt_10msRmsCalc];

	UabOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UabOut * 0.0555);
	UbcOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UbcOut * 0.0555);
	UcaOut_HalfMemory[SysCnt_10msRmsCalc] = fabs(UcaOut * 0.0555);

	UabOut_rms += UabOut_HalfMemory[SysCnt_10msRmsCalc];
	UbcOut_rms += UbcOut_HalfMemory[SysCnt_10msRmsCalc];
	UcaOut_rms += UcaOut_HalfMemory[SysCnt_10msRmsCalc];

	UabOut_rmsf = UabOut_rmsf * 0.875 + UabOut_rms * 0.125;
	UbcOut_rmsf = UbcOut_rmsf * 0.875 + UbcOut_rms * 0.125;
	UcaOut_rmsf = UcaOut_rmsf * 0.875 + UcaOut_rms * 0.125;
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	SysCnt_10msRmsCalc ++;
	if(SysCnt_10msRmsCalc >= 20)
		SysCnt_10msRmsCalc = 0;
}

void CalImbalance(void)
{
	///////////////////////////
	vphase_min = UaOut_rmsf;
	if(vphase_min > UbOut_rmsf)
		vphase_min = UbOut_rmsf;
	if(vphase_min > UcOut_rmsf)
		vphase_min = UcOut_rmsf;
	///////////////////////////
	vLine_avg = (UabOut_rmsf + UbcOut_rmsf + UcaOut_rmsf)*0.333333;

	vLine_min = UabOut_rmsf;
	if(vLine_min > UbcOut_rmsf)
		vLine_min = UbcOut_rmsf;
	if(vLine_min > UcaOut_rmsf)
		vLine_min = UcaOut_rmsf;

	vLine_max = UabOut_rmsf;
	if(vLine_max < UbcOut_rmsf)
		vLine_max = UbcOut_rmsf;
	if(vLine_max < UcaOut_rmsf)
		vLine_max = UcaOut_rmsf;

	if(MainSta2Word.DataBit.WorkLpQS1)
	{
		Uout_Imbalance = (vLine_max - vLine_min) / vLine_avg;
	}
	else
	{
		Uout_Imbalance = 0;
	}
	///////////////////////////////////////////////
	if( MainSta2Word.DataBit.WorkLpQS1 & (!MainSta2Word.DataBit.CheckLpQS1) )
	{
		IaOut_rmsf = IaPar_rmsf;
		IbOut_rmsf = IbPar_rmsf;
		IcOut_rmsf = IcPar_rmsf;
	}
	else if( (!MainSta2Word.DataBit.WorkLpQS1) & (MainSta2Word.DataBit.CheckLpQS1) )
	{
		IaOut_rmsf = IaOvh_rmsf;
		IbOut_rmsf = IbOvh_rmsf;
		IcOut_rmsf = IcOvh_rmsf;
	}
	else
	{
		IaOut_rmsf = 0;
		IbOut_rmsf = 0;
		IcOut_rmsf = 0;
	}

	Iphase_avg = (IaOut_rmsf + IbOut_rmsf + IcOut_rmsf)*0.333333;

	Iphase_min = IaOut_rmsf;
	if(Iphase_min > IbOut_rmsf)
		Iphase_min = IbOut_rmsf;
	if(Iphase_min > IcOut_rmsf)
		Iphase_min = IcOut_rmsf;

	Iphase_max = IaOut_rmsf;
	if(Iphase_max < IbOut_rmsf)
		Iphase_max = IbOut_rmsf;
	if(Iphase_max < IcOut_rmsf)
		Iphase_max = IcOut_rmsf;

	if( MainSta2Word.DataBit.WorkLpQS1 | MainSta2Word.DataBit.CheckLpQS1 )
	{
		Iout_Imbalance = (Iphase_max - Iphase_min) / Iphase_avg;
	}
	else
	{
		Iout_Imbalance = 0;
	}
}
/******************************************************************/
void GetPQ2r(void)
{
	float ftmp = 0;
	ftmp = UaOut*IaPar + UaOut*IbPar + UaOut*IcPar ;
	P_Work = P_Work*0.99 + ftmp*0.01;

	ftmp = (UbcOut*IaPar + UcaOut*IbPar + UabOut*IcPar) / Sqrt_3;
	Q_Work = Q_Work*0.99 + ftmp*0.01;

	ftmp = P_Work*P_Work + Q_Work*Q_Work;
	S_Work = sqrtf(ftmp);

	PF_Work = atan2(Q_Work, P_Work);
	//////////////////////////////////////////////////////////////
	ftmp = UaOut*IaOvh + UaOut*IbOvh + UaOut*IcOvh ;
	P_Ovh = P_Ovh*0.99 + ftmp*0.01;

	ftmp = (UbcOut*IaOvh + UcaOut*IbOvh + UabOut*IcOvh) / Sqrt_3;
	Q_Ovh = Q_Ovh*0.99 + ftmp*0.01;

	ftmp = P_Ovh*P_Ovh + Q_Ovh*Q_Ovh;
	S_Ovh = sqrtf(ftmp);

	PF_Ovh = atan2(Q_Ovh, P_Ovh);

	if( MainSta2Word.DataBit.WorkLpQS1 & (!MainSta2Word.DataBit.CheckLpQS1) )
	{
		if(MainSta2Word.DataBit.SingQF1 | MainSta2Word.DataBit.ParaQF2)
		{
			P_Sys = P_Work;
			Q_Sys = P_Work;
			S_Sys = S_Work;
			PF_Sys = PF_Work;
		}
		else
		{
			P_Sys = 0;
			Q_Sys = 0;
			S_Sys = 0;
			PF_Sys = 0;
		}
	}
	else if( (!MainSta2Word.DataBit.WorkLpQS1) & (MainSta2Word.DataBit.CheckLpQS1) )
	{
		P_Sys = P_Ovh;
		Q_Sys = P_Ovh;
		S_Sys = S_Ovh;
		PF_Sys = PF_Ovh;
	}
	else
	{
		P_Sys = 0;
		Q_Sys = 0;
		S_Sys = 0;
		PF_Sys = 0;
	}
}
/******************************************************************/
interrupt void EPWM1_INT_Underflow_isr(void)
{
	int i;

	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	EINT;

	IO_ADCONVSTSTART;
	for(i=0; i<5; i++);
	IO_ADCONVSTSTOP;
	for(i=0; i<70; i++);
	// Get System Ctrl 1ms
	SysCnt_1msBase ++;
	if(SysCnt_1msBase >= 16)
	{
		SysCnt_1msBase = 0;
		SCIBSend_1ms ++;
		if(SCIBSend_1ms >= 10000)
			SCIBSend_1ms = 0;

		RealTime1ms ++;
		SysTime_MicroSecond ++;

		if(SysTime_MicroSecond >= 1000)
		{
			SysTime_MicroSecond = 0;
		}
		SysCnt_1ms ++;
		if(SysCnt_1ms > 10000)
			SysCnt_1ms = 0;

		CtrlCnt_LedTogg ++;

		SysCnt_1sBase ++;
		if(SysCnt_1sBase > 1000)
		{
			SysCnt_1sBase = 0;
			SysCnt_1s ++;

			if(SysCnt_1s >10000)
			{
				SysCnt_1s = 0;
			}
		}
	}
//////////////////////////////////////////////////////
	if(IO_ADNotBusy == 1)
		Palarell_Extern_ADCScan();
////////////////////////////////////////////////////////////////////////
	GetVIRms();
	GetPQ2r();
////////////////////////////////////////////////////////////////////////
}
/******************************************************************/
