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
#include "SCI_ModbusS\Sci_Modbus_Config.H"
#include "SCI_ModbusS\Custom_ModbusRTU.H"
#include "SCI_ModbusM\Custom_Remote.H"
#include "SCI_DCS\Custom_DCS.H"

#include "Hal.h"
#include "App.h"
#include "stdint_fix.h"

//#define DEBUG
#define Release
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
extern unsigned int RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart;

void INT28335_process(void);
void pwm_init(void);
/******************************************************************/
interrupt void EPWM1_INT_Underflow_isr(void);
/******************************************************************/
void FunFaultReact(void);
void FunResetAct(void);
void FunStatusUpdate(void);

void RemCommVarRemap(void);
void DcsCommVarRemap(void);
void GetVoltCurr(void);
void npr_1s_protection(void);

void RmsFillter(void);
void CalImbalance(void);
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

	for(i=0;i<5000;i++)
		for(j=0;j<10000;j++);

	InitXintf();

#ifdef DEBUG
	Sci_Monitor_init(&ScibRegs);
	Software_oscilloscopeInit();
#else
	Sci_Modbus_init(&ScibRegs, 375e5, 9600);
#endif

	Sci_DCS_init(&SciaRegs, 375e5, 9600);
	Sci_Remote_init(&ScicRegs, 375e5, 9600);

	InitSpi();
	DMA_Init();
	MCBSPB_Init();
	Zone7_Init();
	InitAdc();

	pwm_init();
	INT28335_process();

	variable_init();
    variable_algorithm_init();

	EALLOW;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm1Regs.TZCLR.bit.INT = 1;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EDIS;

	EINT;

	Dis_PWM();
	ParaDownloadCtrlWord=1;

	CAN_App_Init();
	CAN_App_Switch_Init();
	for(; ;)
	{

        CAN_App_RxLoop();
		EINT;
///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
		Sci_Monitor_RxData(&ScibRegs, SCIBRecvBuffer, &SCIBRecvTail);
		Monitor_Rx_Protocol(SCIBRecvBuffer, &SCIBRecvFront, SCIBRecvTail, SCIBSendBuffer, &SCIBSendTail);
		Software_oscilloscope(SCIBSend_1ms, SCIBSendBuffer, &SCIBSendTail);
		Sci_Monitor_TxData(&ScibRegs, SCIBSendBuffer, &SCIBSendFront, SCIBSendTail, SCIBSend_1ms);
#else
		Modbus_Send_To_HMI();
		Sci_Modbus_RxData(&ScibRegs, Modbus_RecvBuffer, &Modbus_RecvTail, Modbus_Recv_BUFF_SIZE);
		ModbusRTU_Protocol(Modbus_RecvBuffer, &Modbus_RecvFront, Modbus_RecvTail, Modbus_Recv_BUFF_SIZE, Modbus_SendBuffer);
		Modbus_Recv_For_HMI();
#endif
		///////////////////////////////////////////////////////////////////////////////
		Sci_DCS_RxData(&SciaRegs, MbsDcs_RecvBuffer, &MbsDcs_RecvTail, Modbus_Recv_BUFF_SIZE);
		ModbusDcs_Protocol(MbsDcs_RecvBuffer, &MbsDcs_RecvFront, MbsDcs_RecvTail, Modbus_Recv_BUFF_SIZE, MbsDcs_SendBuffer);
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

		if (flag_EPWM1_INT_Underflow == 1)
		{
			flag_EPWM1_INT_Underflow = 0;
			GetVoltCurr();
		}
//////////////////////////////////////////////////////////////////////////////
        if(SysCnt_1s != SysCnt_1sOld)
        {
            SysCnt_1sOld = SysCnt_1s;
			FunStatusUpdate();
            npr_1s_protection();
            MainStusWord.DataBit.Toggle ^= 1;
        }
		//////////////////////////////////////////////////////////////////////////////
		if(SysCnt_1ms != SysCnt_1msOld)
		{
			SysCnt_1msOld = SysCnt_1ms;
            CAN_App_MainLoop();
			IO_Scan();
			Inner_ADCScan();
			CalImbalance();
			FunResetAct();
			RemCommVarRemap();
			DcsCommVarRemap();
//			RmsFillter();
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
void RemCommVarRemap(void)
{
	RemInputSum.StaWord1.DataInt = MainStusWord.DataInt;
	RemInputSum.StaWord2.DataInt = MainSta2Word.DataInt;
	RemInputSum.FltWord1.DataInt = FaultWord1.DataInt;
	RemInputSum.FltWord2.DataInt = FaultWord2.DataInt;
	RemInputSum.AlmWord1.DataInt = AlarmWord1.DataInt;
	RemInputSum.AlmWord2.DataInt = AlarmWord2.DataInt;
	RemInputSum.UabSl = UabSl_rmsf *10;
	RemInputSum.UbcSl = UbcSl_rmsf *10;
	RemInputSum.UcaSl  = UcaSl_rmsf *10;
	RemInputSum.UabPar = UabPar_rmsf *10;
	RemInputSum.UbcPar = UbcPar_rmsf *10;
	RemInputSum.UcaPar = UcaPar_rmsf *10;
	RemInputSum.UaOut = UaOut_rmsf *10;
	RemInputSum.UbOut = UbOut_rmsf *10;
	RemInputSum.UcOut = UcOut_rmsf *10;
	RemInputSum.UabOut = UabOut_rmsf *10;
	RemInputSum.UbcOut = UbcOut_rmsf *10;
	RemInputSum.UcaOut = UcaOut_rmsf *10;
	RemInputSum.IaOut = IaOut_rmsf *10;
	RemInputSum.IbOut = IbOut_rmsf *10;
	RemInputSum.IcOut = IcOut_rmsf *10;
	RemInputSum.P_Sys = (s16)(P_Sys *10);
	RemInputSum.Q_Sys = (s16)(Q_Sys *10);
	RemInputSum.S_Sys = (s16)(S_Sys *10);
	RemInputSum.PF_Sys = (s16)(PF_Sys *100);
	RemInputSum.Ublk = Uout_Imbalance *100;
	RemInputSum.Iblk = Iout_Imbalance *100;
	RemInputSum.FLwater = FlowRate*10;
}

void DcsCommVarRemap(void)
{
	DcsInputSum.StaWord1.DataInt = MainStusWord.DataInt;
	DcsInputSum.StaWord2.DataInt = MainSta2Word.DataInt;
	DcsInputSum.FltWord1.DataInt = FaultWord1.DataInt;
	DcsInputSum.FltWord2.DataInt = FaultWord2.DataInt;
	DcsInputSum.AlmWord1.DataInt = AlarmWord1.DataInt;
	DcsInputSum.AlmWord2.DataInt = AlarmWord2.DataInt;
	DcsInputSum.UabSl = UabSl_rmsf *10;
	DcsInputSum.UbcSl = UbcSl_rmsf *10;
	DcsInputSum.UcaSl  = UcaSl_rmsf *10;
	DcsInputSum.UabPar = UabPar_rmsf *10;
	DcsInputSum.UbcPar = UbcPar_rmsf *10;
	DcsInputSum.UcaPar = UcaPar_rmsf *10;
	DcsInputSum.UaOut = UaOut_rmsf *10;
	DcsInputSum.UbOut = UbOut_rmsf *10;
	DcsInputSum.UcOut = UcOut_rmsf *10;
	DcsInputSum.UabOut = UabOut_rmsf *10;
	DcsInputSum.UbcOut = UbcOut_rmsf *10;
	DcsInputSum.UcaOut = UcaOut_rmsf *10;
	DcsInputSum.IaOut = IaOut_rmsf *10;
	DcsInputSum.IbOut = IbOut_rmsf *10;
	DcsInputSum.IcOut = IcOut_rmsf *10;
	DcsInputSum.P_Sys = P_Sys *10;
	DcsInputSum.Q_Sys = Q_Sys *10;
	DcsInputSum.S_Sys = S_Sys *10;
	DcsInputSum.PF_Sys = PF_Sys *100;
	DcsInputSum.Ublk = Uout_Imbalance *100;
	DcsInputSum.Iblk = Iout_Imbalance *100;
	DcsInputSum.FLwater = FlowRate*10;
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

	if((AlarmWord1.DataInt) || (AlarmWord2.DataInt))
	{
		MainStusWord.DataBit.Alarm = 1;
	}
    else
    {
		MainStusWord.DataBit.Alarm = 0;
    }

	if(NPR_FaultCode)
	{
		MainStusWord.DataBit.Fault = 1;
		MainStusWord.DataBit.Running = 0;
	}
	else
	{
		MainStusWord.DataBit.Fault = 0;

		if((MainSta2Word.DataBit.SingQF1 && MainSta2Word.DataBit.SingQR1) ||
           (MainSta2Word.DataBit.ParaQF2 && MainSta2Word.DataBit.ParaQR2))
		{
			MainStusWord.DataBit.Running = 1;
		}
		else
		{
			MainStusWord.DataBit.Running = 0;
		}
	}

	if(MainStusWord.DataBit.Running)
	{
		IO_RunLed_ON;
	}
	else
	{
		IO_RunLed_OFF;
	}

	if(MainStusWord.DataBit.Fault)
	{
		IO_FltLed_ON;
	}
    else if (MainStusWord.DataBit.Alarm)
    {
        IO_FltLed_TOG;
    }
	else
	{
        /*if (MainStusWord.DataBit.Alarm == 0)*/
        IO_FltLed_OFF;
	}
}
/******************************************************************/
void npr_1s_protection(void)
{
	uint16_t status1,status2,status3;

	status1 = Cabinet_GetComPowerToSectionParamStatus1();
    status2 = Cabinet_GetComSyncToSectionStatus();
    status3 = Cabinet_GetComPowerToSectionStatus();
	/////////////////////////////////////////////

    if (((MainSta2Word.DataBit.SingQF1 == 1) && (MainSta2Word.DataBit.SingQR1 == 1) && ((status3>>10)&1)) ||
        ((MainSta2Word.DataBit.ParaQF2 == 1) && (MainSta2Word.DataBit.ParaQR2 == 1)))
    {
        // 对地
        if(vphase_min < AlmThr_UnderVolt_Phase)
        {
            if (AlmCnt_UnderVolt_Phase > AlmCntThr_UnderVolt_Phase)
            {
                AlmCnt_UnderVolt_Phase = AlmCntThr_UnderVolt_Phase;
                AlarmWord_Map(1);
            }
            else
            {
                AlmCnt_UnderVolt_Phase ++;
            }
        }
        else
        {
            if (AlmCnt_UnderVolt_Phase > 0)
            {
                AlmCnt_UnderVolt_Phase --;
                AlarmWord_Clear(1);
            }
        }

        // 工况1
        if (((status1>>4)&1) == 0)
        { 
            // 过压故障
            if(vLine_max > FltThr_OverVolt_Work1)
            {
                if(FltCnt_OverVolt_Work1 > 1)
                {
                    FltCnt_OverVolt_Work1 = 0;
                    NPR_FaultCode = 17;
                    FaultWord_Map(17);
                }
                else
                {
                    FltCnt_OverVolt_Work1 ++;
                }
            }
            else
            {
                FltCnt_OverVolt_Work1 = 0;
            }
            // 过压告警
            if(vLine_max > AlmThr_OverVolt_Work1)
            {
                AlmCnt_OverVolt_Work1 ++;
                // 仅告警
                if(AlmCnt_OverVolt_Work1 > 2)
                {
                    AlarmWord_Map(2);
                }
                // 告警转故障
                if(AlmCnt_OverVolt_Work1 > (AlmCntThr_OverVolt_Work1))
                {
                    AlmCnt_OverVolt_Work1 = AlmCntThr_OverVolt_Work1;
                    NPR_FaultCode = 17;
                    FaultWord_Map(17);
                }
            }
            else
            {
                if(AlmCnt_OverVolt_Work1 > 0)
                {
                    AlmCnt_OverVolt_Work1 --;
                    AlarmWord_Clear(2);
                }
            }

            /////////////////////////////////////////////
            // 单机直接判断欠压；并机需要读取同步柜给的并网有电信号
            if ((((MainSta2Word.DataBit.SingQF1 == 1) && (MainSta2Word.DataBit.SingQR1 == 1)) && ((status3>>10)&1)) ||
                (((MainSta2Word.DataBit.ParaQF2 == 1) && (MainSta2Word.DataBit.ParaQR2 == 1)) && (((status2>>9)&1) == 1)) )
            {
                // 欠压故障
                if(vLine_min < FltThr_UnderVolt_Work1)
                {
                    if(FltCnt_UnderVolt_Work1 > AlmCntThr_UnderVolt_Work1)
                    {
                        FltCnt_UnderVolt_Work1 = 0;
                        NPR_FaultCode = 19;
                        FaultWord_Map(19);
                    }
                    else
                    {
                        FltCnt_UnderVolt_Work1 ++;
                    }
                }
                else
                {
                    FltCnt_UnderVolt_Work1 = 0;
                }
                // 欠压告警
                if(vLine_min < AlmThr_UnderVolt_Work1)
                {
                    if(AlmCnt_UnderVolt_Work1 > 2)
                    {
                        AlmCnt_UnderVolt_Work1 = 2;
                        AlarmWord_Map(4);
                    }
                    else
                    {
                        AlmCnt_UnderVolt_Work1 ++;
                    }
                }
                else
                {
                    AlmCnt_UnderVolt_Work1 = 0;
                    AlarmWord_Clear(4);
                }
                //////////////////////////////////////////////////
            }
            else
            {
                FltCnt_UnderVolt_Work1 = 0;
                AlmCnt_UnderVolt_Work1 = 0;
            }

            // 电流
            // 过流故障
            if(Iphase_max > FltThr_OverCurr_Work1)
            {
                if(FltCnt_OverCurr_Work1 > 1)
                {
                    FltCnt_OverCurr_Work1 = 0;
                    NPR_FaultCode = 23;
                    FaultWord_Map(23);
                }
                else
                {
                    FltCnt_OverCurr_Work1 ++;
                }
            }
            else
            {
                FltCnt_OverCurr_Work1 = 0;
            }
            // 过流告警
            if(Iphase_max > AlmThr_OverCurr_Work1)
            {
                if(AlmCnt_OverCurr_Work1 > AlmCntThr_OverCurr_Work1)
                {
                    AlmCnt_OverCurr_Work1 = 0;
                    NPR_FaultCode = 23;
                    FaultWord_Map(23);
                }
                else
                {
                    AlmCnt_OverCurr_Work1 ++;
                    if(AlmCnt_OverCurr_Work1 > 1)
                    {
                        AlarmWord_Map(8);
                    }
                }
            }
            else
            {
                if(AlmCnt_OverCurr_Work1 > 0)
                {
                    AlmCnt_OverCurr_Work1 --;
                    AlarmWord_Clear(8);
                }
            }
        }
        // 工况2
        else if (((status1>>4)&1) == 1)
        {
            /////////////////////////////////////////////
            // 过压故障
            if(vLine_max > FltThr_OverVolt_Work2)
            {
                if(FltCnt_OverVolt_Work2 > 1)
                {
                    FltCnt_OverVolt_Work2 = 0;
                    NPR_FaultCode = 18;
                    FaultWord_Map(18);
                }
                else
                {
                    FltCnt_OverVolt_Work2 ++;
                }
            }
            else
            {
                FltCnt_OverVolt_Work2 = 0;
            }
            // 过压告警
            if(vLine_max > AlmThr_OverVolt_Work2)
            {
                AlmCnt_OverVolt_Work2 ++;
                if(AlmCnt_OverVolt_Work2 > 2)
                {
                    AlarmWord_Map(3);
                }
                if(AlmCnt_OverVolt_Work2 > (AlmCntThr_OverVolt_Work2))
                {
                    AlmCnt_OverVolt_Work2 = AlmCntThr_OverVolt_Work2;
                    NPR_FaultCode = 18;
                    FaultWord_Map(18);
                }
            }
            else
            {
                if(AlmCnt_OverVolt_Work2 > 0)
                {
                    AlmCnt_OverVolt_Work2 --;
                    AlarmWord_Clear(3);
                }
            }
            /////////////////////////////////////////////
            // 欠压
            // 单机直接判断欠压；并机需要读取同步柜给的并网有电信号
            if ((((MainSta2Word.DataBit.SingQF1 == 1) && (MainSta2Word.DataBit.SingQR1 == 1)) && ((status3>>10)&1)) ||
                (((MainSta2Word.DataBit.ParaQF2 == 1) && (MainSta2Word.DataBit.ParaQR2 == 1)) && (((status2>>9)&1) == 1)) )
            {
                // 欠压故障
                if(vLine_min < FltThr_UnderVolt_Work2)
                {
                    if(FltCnt_UnderVolt_Work2 > AlmCntThr_UnderVolt_Work2)
                    {
                        FltCnt_UnderVolt_Work2 = 0;
                        NPR_FaultCode = 20;
                        FaultWord_Map(20);
                    }
                    else
                    {
                        FltCnt_UnderVolt_Work2 ++;
                    }
                }
                else
                {
                    FltCnt_UnderVolt_Work2 = 0;
                }
                // 欠压告警
                if(vLine_min < AlmThr_UnderVolt_Work2)
                {
                    if(AlmCnt_UnderVolt_Work2 > 2)
                    {
                        AlmCnt_UnderVolt_Work2 = 2;
                        AlarmWord_Map(5);
                    }
                    else
                    {
                        AlmCnt_UnderVolt_Work2 ++;
                    }
                }
                else
                {
					AlmCnt_UnderVolt_Work2 = 0;
					AlarmWord_Clear(5);
                }
            }
            else
            {
            	FltCnt_UnderVolt_Work2 = 0;
                AlmCnt_UnderVolt_Work2 = 0;
            }

            // 电流
            // 过流故障
            if(Iphase_max > FltThr_OverCurr_Work2)
            {
                if(FltCnt_OverCurr_Work2 > 1)
                {
                    FltCnt_OverCurr_Work2 = 1;
                    NPR_FaultCode = 24;
                    FaultWord_Map(24);
                }
                else
                {
                    FltCnt_OverCurr_Work2 ++;
                }
            }
            else
            {
                FltCnt_OverCurr_Work2 = 0;
            }
            // 过流告警
            if(Iphase_max > AlmThr_OverCurr_Work2)
            {
                if(AlmCnt_OverCurr_Work2 > AlmCntThr_OverCurr_Work2)
                {
                    AlmCnt_OverCurr_Work2 = 0;
                    NPR_FaultCode = 24;
                    FaultWord_Map(24);
                }
                else
                {
                    AlmCnt_OverCurr_Work2 ++;
                    if(AlmCnt_OverCurr_Work2 > 1)
                    {
                        AlarmWord_Map(9);
                    }
                }
            }
            else
            {
                if(AlmCnt_OverCurr_Work2 > 0)
                {
                    AlmCnt_OverCurr_Work2 --;
                    AlarmWord_Clear(9);
                }
            }
        }
        //////////////////////////////////////////////////
        if (vLine_max > 100)
        {
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
        }
        ////////////////////////////////////////////////////////
        if (Iphase_max > 30)
        {
            if(Iout_Imbalance > AlmThr_OverImba_Curr)
            {
                AlmCnt_OverImba_Curr ++;
                if(AlmCnt_OverImba_Curr > AlmCntThr_OverImba_Curr)
                {
                    AlarmWord_Map(7);
                }
            }
            else
            {
                AlmCnt_OverImba_Curr = 0;
                AlarmWord_Clear(7);
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
        }
    }
    else
    {
        AlarmWord_Clear(1);
        AlarmWord_Clear(2);
        AlarmWord_Clear(4);
        AlarmWord_Clear(8);
        AlarmWord_Clear(3);
        AlarmWord_Clear(5);
        AlarmWord_Clear(9);
        AlarmWord_Clear(6);
        AlarmWord_Clear(7);
        AlmCnt_UnderVolt_Phase = 0;
        FltCnt_OverVolt_Work1 = 0;
        AlmCnt_OverVolt_Work1 = 0;
        FltCnt_UnderVolt_Work1 = 0;
        AlmCnt_UnderVolt_Work1 = 0;
        FltCnt_OverCurr_Work1 = 0;
        AlmCnt_OverCurr_Work1 = 0;
        FltCnt_OverVolt_Work2 = 0;
        AlmCnt_OverVolt_Work2 = 0;
        FltCnt_UnderVolt_Work2 = 0;
        AlmCnt_UnderVolt_Work2 = 0;
        FltCnt_OverCurr_Work2 = 0;
        AlmCnt_OverCurr_Work2 = 0;
        AlmCnt_OverImba_Volt = 0;
        FltCnt_OverImba_Volt = 0;
        AlmCnt_OverImba_Curr = 0;
        FltCnt_OverImba_Curr = 0;
    }
    //////////////////////////////////////////////////////////
    if (Cap_Ctrl_Word.DataBit.Bit5)
    {
        // 下限I水流量保护
        if((FlowRate < FltThr_OverLow_Flow1) && (FlowRate >= FltThr_OverLow_Flow2))
        {
            AlarmWord_Map(10);
            if(FltCnt_OverLow_Flow1 > FltCntThr_OverLow_Flow1)
            {
                FltCnt_OverLow_Flow1 = FltCntThr_OverLow_Flow1;
                NPR_FaultCode = 25;
                FaultWord_Map(25);
            }
            else
            {
                FltCnt_OverLow_Flow1 ++;
            }
        }
        else
        {
            AlarmWord_Clear(10);
            FltCnt_OverLow_Flow1 = 0;
            if ((FltCnt_OverLow_Flow1 == 0) &&
                    (NPR_FaultCode == 25))
            {
                NPR_FaultCode = 0;
                FaultWord_Clear(25);
            }
        }
        // 下限II水流量保护
        if (FlowRate < FltThr_OverLow_Flow2)
        {
            AlarmWord_Map(13);
            if(FltCnt_OverLow_Flow2 > FltCntThr_OverLow_Flow2)
            {
                FltCnt_OverLow_Flow2 = FltCntThr_OverLow_Flow2;
                NPR_FaultCode = 29;
                FaultWord_Map(29);
            }
            else
            {
                FltCnt_OverLow_Flow2 ++;
            }
        }
        else
        {
            AlarmWord_Clear(13);
            FltCnt_OverLow_Flow2 = 0;
            if ((FltCnt_OverLow_Flow2 == 0) &&
                    (NPR_FaultCode == 29))
            {
                NPR_FaultCode = 0;
                FaultWord_Clear(29);
            }
        }
    }
    else
    {
        FltCnt_OverLow_Flow1 = 0;
        FltCnt_OverLow_Flow2 = 0;
        AlarmWord_Clear(10);
        AlarmWord_Clear(13);
        FaultWord_Clear(25);
        FaultWord_Clear(29);
    }

    // 可控硅关断
    if (MainStusWord.DataBit.Running)
    {
        if ((IaPar_rms < FltThr_UnderCurr_Silicon) &&
            (IbPar_rms < FltThr_UnderCurr_Silicon) &&
            (IcPar_rms < FltThr_UnderCurr_Silicon))
        {
        	if(FltCnt_UnderCurr_Silicon > 7)
			{
				FltCnt_UnderCurr_Silicon = 7;
				NPR_FaultCode = 27;
				FaultWord_Map(27);
			}
			else
			{
				FltCnt_UnderCurr_Silicon ++;
			}
        }
        else
        {
			FltCnt_UnderCurr_Silicon = 0;
        }
    }
    else
    {
        if ((IaPar_rms > FltThr_OverCurr_Silicon) &&
            (IbPar_rms > FltThr_OverCurr_Silicon) &&
            (IcPar_rms > FltThr_OverCurr_Silicon))
        {
			if(FltCnt_OverCurr_Silicon > 7)
			{
				FltCnt_OverCurr_Silicon = 7;
				NPR_FaultCode = 28;
				FaultWord_Map(28);
			}
			else
			{
				FltCnt_OverCurr_Silicon ++;
			}
		}
		else
		{
			FltCnt_OverCurr_Silicon = 0;
		}
    }

    // DO开关控制与反馈状态匹配(QR1、QR2、QR3、QF1、QF2)
    if (MainSta2WordCtrl.DataBit.SingQF1 != MainSta2Word.DataBit.SingQF1)
    {
        if (FltCnt_OverTime_QF1 < 3)
        {
        	FltCnt_OverTime_QF1++;
        }
        else
        {
            AlarmWord_Map(18);
        }
    }
    else
    {
        if (FltCnt_OverTime_QF1 > 0)
        {
        	FltCnt_OverTime_QF1--;
            if (FltCnt_OverTime_QF1 == 0)
            {
                AlarmWord_Clear(18);
            }
        }
    }
    if (MainSta2WordCtrl.DataBit.ParaQF2 != MainSta2Word.DataBit.ParaQF2)
    {
        if (FltCnt_OverTime_QF2 < 3)
        {
        	FltCnt_OverTime_QF2++;
        }
        else
        {
            AlarmWord_Map(19);
        }
    }
    else
    {
        if (FltCnt_OverTime_QF2 > 0)
        {
        	FltCnt_OverTime_QF2--;
            if (FltCnt_OverTime_QF2 == 0)
            {
                AlarmWord_Clear(19);
            }
        }
    }
    if (MainSta2WordCtrl.DataBit.SingQR1 != MainSta2Word.DataBit.SingQR1)
    {
        if (FltCnt_OverTime_QR1 < 3)
        {
        	FltCnt_OverTime_QR1++;
        }
        else
        {
            AlarmWord_Map(20);
        }
    }
    else
    {
        if (FltCnt_OverTime_QR1 > 0)
        {
        	FltCnt_OverTime_QR1--;
            if (FltCnt_OverTime_QR1 == 0)
            {
                AlarmWord_Clear(20);
            }
        }
    }
    if (MainSta2WordCtrl.DataBit.ParaQR2 != MainSta2Word.DataBit.ParaQR2)
    {
        if (FltCnt_OverTime_QR2 < 3)
        {
        	FltCnt_OverTime_QR2++;
        }
        else
        {
            AlarmWord_Map(21);
        }
    }
    else
    {
        if (FltCnt_OverTime_QR2 > 0)
        {
        	FltCnt_OverTime_QR2--;
            if (FltCnt_OverTime_QR2 == 0)
            {
                AlarmWord_Clear(21);
            }
        }
    }
    if (MainSta2WordCtrl.DataBit.DgssKM1 != MainSta2Word.DataBit.DgssKM1)
    {
        if (FltCnt_OverTime_QR3 < 3)
        {
        	FltCnt_OverTime_QR3++;
        }
        else
        {
            AlarmWord_Map(22);
        }
    }
    else
    {
        if (FltCnt_OverTime_QR3 > 0)
        {
        	FltCnt_OverTime_QR3--;
            if (FltCnt_OverTime_QR3 == 0)
            {
                AlarmWord_Clear(22);
            }
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

	ftemp = uFlowRate * 19.81 - 6.691;//uFlowRate *54.46 - 20.6

	FlowRate = FlowRate*0.9 + ftemp*0.1;
}
/******************************************************************/
void GetVoltCurr(void)
{

    UabSl_rms -= fabsf(UabSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcSl_rms -= fabsf(UbcSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaSl_rms -= fabsf(UcaSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UabPar_rms -= fabsf(UabPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcPar_rms -= fabsf(UbcPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaPar_rms -= fabsf(UcaPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    IaPar_rms -= sqrtf(IaPar_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    IbPar_rms -= sqrtf(IbPar_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    IcPar_rms -= sqrtf(IcPar_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    IaOvh_rms -= sqrtf(IaOvh_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    IbOvh_rms -= sqrtf(IbOvh_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    IcOvh_rms -= sqrtf(IcOvh_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UaOut_rms -= fabsf(UaOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbOut_rms -= fabsf(UbOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcOut_rms -= fabsf(UcOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UabOut_rms -= fabsf(UabOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcOut_rms -= fabsf(UbcOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaOut_rms -= fabsf(UcaOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    if( MainSta2Word.DataBit.WorkLpQS1 && (!MainSta2Word.DataBit.CheckLpQS1) )
    {
        if(MainSta2Word.DataBit.SingQF1 || MainSta2Word.DataBit.ParaQF2)
        {
            work1_rms -= ((UaOut_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]+UbOut_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]+UcOut_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc]))* 0.05;

            work2_rms -= ((UbcOut_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]+UcaOut_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]+UabOut_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc])/ Sqrt_3)* 0.05;
        }
    }
    else if( (!MainSta2Word.DataBit.WorkLpQS1) && (MainSta2Word.DataBit.CheckLpQS1) )
    {
        work1_rms -= ((UaOut_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]+UbOut_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]+UcOut_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc]))* 0.05;

        work2_rms -= ((UbcOut_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]+UcaOut_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]+UabOut_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc])/ Sqrt_3)* 0.05;
    }

    UabSl_HalfMemory[SysCnt_10msRmsCalc] = UabSl;
    UbcSl_HalfMemory[SysCnt_10msRmsCalc] = UbcSl;
    UcaSl_HalfMemory[SysCnt_10msRmsCalc] = UcaSl;

    UabPar_HalfMemory[SysCnt_10msRmsCalc] = UabPar;
    UbcPar_HalfMemory[SysCnt_10msRmsCalc] = UbcPar;
    UcaPar_HalfMemory[SysCnt_10msRmsCalc] = UcaPar;

    IaPar_HalfMemory[SysCnt_10msRmsCalc] = IaPar;
    IbPar_HalfMemory[SysCnt_10msRmsCalc] = IbPar;
    IcPar_HalfMemory[SysCnt_10msRmsCalc] = IcPar;

    IaOvh_HalfMemory[SysCnt_10msRmsCalc] = IaOvh;
    IbOvh_HalfMemory[SysCnt_10msRmsCalc] = IbOvh;
    IcOvh_HalfMemory[SysCnt_10msRmsCalc] = IcOvh;

    UaOut_HalfMemory[SysCnt_10msRmsCalc] = UaOut;
    UbOut_HalfMemory[SysCnt_10msRmsCalc] = UbOut;
    UcOut_HalfMemory[SysCnt_10msRmsCalc] = UcOut;

    UabOut_HalfMemory[SysCnt_10msRmsCalc] = UabOut;
    UbcOut_HalfMemory[SysCnt_10msRmsCalc] = UbcOut;
    UcaOut_HalfMemory[SysCnt_10msRmsCalc] = UcaOut;

    UabSl_rms += fabsf(UabSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcSl_rms += fabsf(UbcSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaSl_rms += fabsf(UcaSl_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UabPar_rms += fabsf(UabPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcPar_rms += fabsf(UbcPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaPar_rms += fabsf(UcaPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    IaPar_rms += sqrtf(IaPar_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    IbPar_rms += sqrtf(IbPar_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    IcPar_rms += sqrtf(IcPar_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    IaOvh_rms += sqrtf(IaOvh_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    IbOvh_rms += sqrtf(IbOvh_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    IcOvh_rms += sqrtf(IcOvh_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UaOut_rms += fabsf(UaOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbOut_rms += fabsf(UbOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcOut_rms += fabsf(UcOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    UabOut_rms += fabsf(UabOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0571;
    UbcOut_rms += fabsf(UbcOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.057;
    UcaOut_rms += fabsf(UcaOut_HalfMemory[SysCnt_10msRmsCalc]) * 0.0569;

    if( MainSta2Word.DataBit.WorkLpQS1 && (!MainSta2Word.DataBit.CheckLpQS1) )
    {
        if(MainSta2Word.DataBit.SingQF1 || MainSta2Word.DataBit.ParaQF2)
        {
            work1_rms += ((UaOut_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]+UbOut_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]+UcOut_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc]))* 0.05;

            work2_rms += ((UbcOut_HalfMemory[SysCnt_10msRmsCalc]*IaPar_HalfMemory[SysCnt_10msRmsCalc]+UcaOut_HalfMemory[SysCnt_10msRmsCalc]*IbPar_HalfMemory[SysCnt_10msRmsCalc]+UabOut_HalfMemory[SysCnt_10msRmsCalc]*IcPar_HalfMemory[SysCnt_10msRmsCalc])/ Sqrt_3)* 0.05;
        }
    }
    else if( (!MainSta2Word.DataBit.WorkLpQS1) && (MainSta2Word.DataBit.CheckLpQS1) )
    {
        work1_rms += ((UaOut_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]+UbOut_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]+UcOut_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc]))* 0.05;

        work2_rms += ((UbcOut_HalfMemory[SysCnt_10msRmsCalc]*IaOvh_HalfMemory[SysCnt_10msRmsCalc]+UcaOut_HalfMemory[SysCnt_10msRmsCalc]*IbOvh_HalfMemory[SysCnt_10msRmsCalc]+UabOut_HalfMemory[SysCnt_10msRmsCalc]*IcOvh_HalfMemory[SysCnt_10msRmsCalc])/ Sqrt_3)* 0.05;
    }

    UabPar_rmsf = UabPar_rmsf * 0.9999 + fabsf(UabPar_rms * 0.0001);
    UbcPar_rmsf = UbcPar_rmsf * 0.9999 + fabsf(UbcPar_rms * 0.0001);
    UcaPar_rmsf = UcaPar_rmsf * 0.9999 + fabsf(UcaPar_rms * 0.0001);

    UabSl_rmsf = UabSl_rmsf * 0.9999 + fabsf(UabSl_rms * 0.0001);
    UbcSl_rmsf = UbcSl_rmsf * 0.9999 + fabsf(UbcSl_rms * 0.0001);
    UcaSl_rmsf = UcaSl_rmsf * 0.9999 + fabsf(UcaSl_rms * 0.0001);

    IaOvh_rmsf = IaOvh_rmsf * 0.9999 + fabsf(IaOvh_rms * 0.0001);
    IbOvh_rmsf = IbOvh_rmsf * 0.9999 + fabsf(IbOvh_rms * 0.0001);
    IcOvh_rmsf = IcOvh_rmsf * 0.9999 + fabsf(IcOvh_rms * 0.0001);

    IaPar_rmsf = IaPar_rmsf * 0.9999 + fabsf(IaPar_rms * 0.0001);
    IbPar_rmsf = IbPar_rmsf * 0.9999 + fabsf(IbPar_rms * 0.0001);
    IcPar_rmsf = IcPar_rmsf * 0.9999 + fabsf(IcPar_rms * 0.0001);

    UaOut_rmsf = UaOut_rmsf * 0.9999 + fabsf(UaOut_rms * 0.0001);
    UbOut_rmsf = UbOut_rmsf * 0.9999 + fabsf(UbOut_rms * 0.0001);
    UcOut_rmsf = UcOut_rmsf * 0.9999 + fabsf(UcOut_rms * 0.0001);

    UabOut_rmsf = UabOut_rmsf * 0.9999 + fabsf(UabOut_rms * 0.0001);
    UbcOut_rmsf = UbcOut_rmsf * 0.9999 + fabsf(UbcOut_rms * 0.0001);
    UcaOut_rmsf = UcaOut_rmsf * 0.9999 + fabsf(UcaOut_rms * 0.0001);

    P_Work = P_Work*0.9999 + work1_rms*0.0001;

    Q_Work = Q_Work*0.9999 + work2_rms*0.0001;

    SysCnt_10msRmsCalc ++;
    if(SysCnt_10msRmsCalc >= 20)
    {
        SysCnt_10msRmsCalc = 0;
        Flag_RmsBufFull = 1;
    }

    if( MainSta2Word.DataBit.WorkLpQS1 && (!MainSta2Word.DataBit.CheckLpQS1) )
    {
        if(MainSta2Word.DataBit.SingQF1 || MainSta2Word.DataBit.ParaQF2)
        {
            P_Sys = (P_Work)*0.00107;
            Q_Sys = (Q_Work)*0.001062;
            if (Flag_RmsBufFull)
            {
                // 视在功率
                S_Work = P_Sys*P_Sys + Q_Sys*Q_Sys;
                S_Sys = sqrtf(S_Work);
                // 功率因子
                if (S_Sys != 0)
                {
                    PF_Sys = P_Sys / S_Sys;
                }
                Flag_RmsBufFull = 0;
            }
        }
        else
        {
            P_Sys = 0;
            Q_Sys = 0;
            S_Sys = 0;
            PF_Sys = 0;
        }
    }
    else if( (!MainSta2Word.DataBit.WorkLpQS1) && (MainSta2Word.DataBit.CheckLpQS1) )
    {
        P_Sys = (P_Work)*0.00107;
        Q_Sys = (Q_Work)*0.001062;
        if (Flag_RmsBufFull)
        {
            // 视在功率
            S_Work = P_Sys*P_Sys + Q_Sys*Q_Sys;
            S_Sys = sqrtf(S_Work);
            // 功率因子
            if (S_Sys != 0)
            {
                PF_Sys = P_Sys / S_Sys;
            }
            Flag_RmsBufFull = 0;
        }
    }
    else
    {
        P_Sys = 0;
        Q_Sys = 0;
        S_Sys = 0;
        PF_Sys = 0;
    }

}

void RmsFillter(void)
{
	UabPar_rmsf = UabPar_rmsf * 0.995 + UabPar_rms * 0.005;
	UbcPar_rmsf = UbcPar_rmsf * 0.995 + UbcPar_rms * 0.005;
	UcaPar_rmsf = UcaPar_rmsf * 0.995 + UcaPar_rms * 0.005;

	UabSl_rmsf = UabSl_rmsf * 0.995 + UabSl_rms * 0.005;
	UbcSl_rmsf = UbcSl_rmsf * 0.995 + UbcSl_rms * 0.005;
	UcaSl_rmsf = UcaSl_rmsf * 0.995 + UcaSl_rms * 0.005;

	IaOvh_rmsf = IaOvh_rmsf * 0.995 + IaOvh_rms * 0.005;
	IbOvh_rmsf = IbOvh_rmsf * 0.995 + IbOvh_rms * 0.005;
	IcOvh_rmsf = IcOvh_rmsf * 0.995 + IcOvh_rms * 0.005;

	IaPar_rmsf = IaPar_rmsf * 0.995 + IaPar_rms * 0.005;
	IbPar_rmsf = IbPar_rmsf * 0.995 + IbPar_rms * 0.005;
	IcPar_rmsf = IcPar_rmsf * 0.995 + IcPar_rms * 0.005;

	UaOut_rmsf = UaOut_rmsf * 0.995 + UaOut_rms * 0.005;
	UbOut_rmsf = UbOut_rmsf * 0.995 + UbOut_rms * 0.005;
	UcOut_rmsf = UcOut_rmsf * 0.995 + UcOut_rms * 0.005;

	UabOut_rmsf = UabOut_rmsf * 0.995 + UabOut_rms * 0.005;
	UbcOut_rmsf = UbcOut_rmsf * 0.995 + UbcOut_rms * 0.005;
	UcaOut_rmsf = UcaOut_rmsf * 0.995 + UcaOut_rms * 0.005;
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

	if ((MainSta2Word.DataBit.WorkLpQS1)  && (vLine_avg != 0))
	{
        Uout_Imbalance = (vLine_max - vLine_min) / vLine_avg;
	}
	else
	{
		Uout_Imbalance = 0;
	}
	///////////////////////////////////////////////
	if( MainSta2Word.DataBit.WorkLpQS1 && (!MainSta2Word.DataBit.CheckLpQS1) )
	{
		IaOut_rmsf = IaPar_rmsf;
		IbOut_rmsf = IbPar_rmsf;
		IcOut_rmsf = IcPar_rmsf;
	}
	else if( (!MainSta2Word.DataBit.WorkLpQS1) && (MainSta2Word.DataBit.CheckLpQS1) )
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

	if (( MainSta2Word.DataBit.WorkLpQS1 || MainSta2Word.DataBit.CheckLpQS1 ) && (Iphase_avg != 0))
	{
		Iout_Imbalance = (Iphase_max - Iphase_min) / Iphase_avg;
	}
	else
	{
		Iout_Imbalance = 0;
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
   flag_EPWM1_INT_Underflow = 1;
////////////////////////////////////////////////////////////////////////
}
/******************************************************************/
