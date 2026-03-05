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
#include "string.h"
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
#include "SCI_ModbusM\Custom_ModbusM.H"

#include "string.h"
#include "App.h"

#include "Hal.h"
/******************************************************************/
extern unsigned int RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart;
void INT28335_process(void);
void pwm_init(void);
/******************************************************************/
void ConvInner_CANAComm(void);
void ConvInner_CANBComm(void);
void CANB_Sector_Send_Power(void);
void CANB_Sector_Recv_Power(void);
/******************************************************************/
interrupt void EPWM1_TZINT_isr(void);   // PDP
interrupt void XINT1_FPGA_isr(void);
interrupt void CANARecv_isr(void);      // CANA isr
interrupt void CANBRecv_isr(void);      // CANA isr
interrupt void Timer2INT_ISR(void);
/******************************************************************/
void FunFaultReact(void);
void FunResetAct(void);
void FunStatusUpdate(void);

void mpr_PWM_protection(void);
void mpr_1ms_protection(void);
void mpr_1ms_AlarmCheck(void);

void CoolFanCtrl(void);
void UnloadFun(void);

void variable_algorithm_init(void);
void RealTime20msFun(void);
void GetPQVIRms(void);
void GetFlowRate_Calc(void);
/******************************************************************/
f32 fCycBuf[64]={0}, 	fIsrBuf[64]={0};
s32	lCycBuf[64]={0}, 	lIsrBuf[64]={0};
s16 iCycBuf[128]={0}, 	iIsrBuf[128]={0};
/******************************************************************/
void Init_XINTF1(void)
{
	EALLOW;
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 25;
	EDIS;

	XIntruptRegs.XINT1CR.bit.POLARITY = 0;
	XIntruptRegs.XINT1CR.bit.ENABLE = 1;
}

inline void Init_PieVect(void)
{
	InitPieCtrl();
	IER = 0x0000;    // Disable CPU interrupts and clear all CPU interrupt flags:
	IFR = 0x0000;
	InitPieVectTable();
}
/******************************************************************/
void main(void)
{
	int i,j;

//	l32 lEmifH, lEmifL;
//	s16 iEmifH, iEmifL;

	/*struct ECAN_REGS ECanaShadow;*/
	/*struct ECAN_REGS ECanbShadow;*/

	DINT;
	InitSysCtrl();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

    Init_PieVect();

	memcpy( &RamfuncsRunStart,
			&RamfuncsLoadStart,
			&RamfuncsLoadEnd - &RamfuncsLoadStart);
	InitFlash();
	GPIO_init();

	for(i=0;i<1000;i++)
		for(j=0;j<10000;j++);

	InitXintf();
	InitCpuTimers();
//	hal_wdg_init();
//	InitECana();
//	InitECanb();
	Sci_Monitor_init(&ScibRegs);
	Software_oscilloscopeInit();
	Sci_Modbus_init(&ScicRegs, 375e5, 9600);
	Sci_ModbusM_init(&SciaRegs, 375e5, 9600);
	ModbusMaster_Init(&NodeEnergy);
	InitSpi();
	DMA_Init();
	MCBSPB_Init();
	Zone7_Init();
	InitAdc();

	Init_XINTF1();
	pwm_init();
//	INT28335_process();
	variable_algorithm_init();
	variable_init();
//	ECanaShadow.CANRMP.bit.RMP4 = 1;
//	ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
//	ECanbShadow.CANRMP.bit.RMP4 = 1;
//	ECanbRegs.CANRMP.all=ECanbShadow.CANRMP.all;
//	ECanbShadow.CANRMP.bit.RMP5=1;
//	ECanbRegs.CANRMP.all=ECanbShadow.CANRMP.all;

	EALLOW;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm1Regs.TZCLR.bit.INT = 1;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EDIS;

	EINT;

	ParaDownloadCtrlWord = 1;
	memset(&fCycBuf, 0, sizeof(fCycBuf));
	memset(&lCycBuf, 0, sizeof(lCycBuf));
	memset(&iCycBuf, 0, sizeof(iCycBuf));
	memset(&fIsrBuf, 0, sizeof(fIsrBuf));
	memset(&lIsrBuf, 0, sizeof(lIsrBuf));
	memset(&iIsrBuf, 0, sizeof(iIsrBuf));

	CPLDSWVerRead();

	for(i=0;i<1000;i++)
		for(j=0;j<10000;j++);
	CAN_App_Init();
    //test
    CAN_App_Test();
    for(;;)
    {
        CAN_App_MainLoop();
//        hal_wdg_feed();
    }

	/*{*/
		/*EINT;*/
/*///////////////////////////////////////////////////////////////////////////////*/
		/*ConvInner_CANAComm();     //CANA_NPR2MPR*/
		/*ConvInner_CANBComm();     //CANB_NPR2MCS*/
/*///////////////////////////////////////////////////////////////////////////////*/
		/*Sci_Monitor_RxData(&ScibRegs, SCIBRecvBuffer, &SCIBRecvTail);*/
		/*Monitor_Rx_Protocol(SCIBRecvBuffer, &SCIBRecvFront, SCIBRecvTail, SCIBSendBuffer, &SCIBSendTail);*/
		/*Software_oscilloscope(SCIBSend_1ms, SCIBSendBuffer, &SCIBSendTail);*/
		/*Sci_Monitor_TxData(&ScibRegs, SCIBSendBuffer, &SCIBSendFront, SCIBSendTail, SCIBSend_1ms);*/
/*///////////////////////////////////////////////////////////////////////////////*/
		/*Modbus_Send_To_HMI();*/
		/*Sci_Modbus_RxData(&SciaRegs, Modbus_RecvBuffer, &Modbus_RecvTail, Modbus_Recv_BUFF_SIZE);*/
		/*ModbusRTU_Protocol(Modbus_RecvBuffer, &Modbus_RecvFront, Modbus_RecvTail, Modbus_Recv_BUFF_SIZE, Modbus_SendBuffer);*/
		/*Modbus_Recv_For_HMI();*/
/*///////////////////////////////////////////////////////////////////////////////*/
		/*Flag_EEPROMCheck = EEPROMAccess(&ParaDownloadCtrlWord, &EEPROMAceNum);*/
		/*{*/
			/*if(FaultMask_Map(40) == 1)*/
			/*{*/
				/*if(MPR_FaultCode == 0)*/
				/*{*/
					/*FunFaultReact();*/
					/*ParaDownloadFaultNum = EEPROMAceNum - 1;*/
					/*MPR_FaultCode = 40;*/
				/*}*/
			/*}*/
			/*FaultWord_Map(40);*/
		/*}*/
/*//////////////////////////////////////////////////////////////////////////////*/
		/*if(CtrlCnt_LedTogg > 200)*/
		/*{*/
			/*CtrlCnt_LedTogg = 0;*/

			/*for(i=0; i<5; i++);*/
			/*IO_LEDG_TOG;*/
			/*for(i=0; i<5; i++);*/
			/*IO_LEDFPGA_TOG;*/

			/*if(MPR_FaultCode)*/
			/*{*/
				/*IO_LEDR_TOG;*/
			/*}*/
			/*else*/
			/*{*/
				/*IO_LEDR_OFF;*/
			/*}*/

			/*GetFlowRate_Calc();*/
			/*MotorFreq_FpgaScan();*/
		/*}*/
/*//////////////////////////////////////////////////////////////////////////////*/
		/*if(SysCnt_1ms != SysCnt_1msOld)*/
		/*{*/
			/*SysCnt_1msOld = SysCnt_1ms;*/
			/*//////////////////////////////////////////////////////////////////*/
			/*for(i=0; i<45; i++)*/
			/*{*/
				/*lEmifH = *(unsigned  int *)(0x4010 + 2*i);*/
				/*lEmifL = *(unsigned  int *)(0x4011 + 2*i);*/
				/*lCycBuf[i] = ( (lEmifH<<16)&0xFFFF0000) | (lEmifL&0x0000FFFF);*/
				/*fCycBuf[i] = (f32)(lCycBuf[i]) / 16777216.0;*/
			/*}*/

			/*MotorVolt_Angle = fCycBuf[0];*/
			/*mpr_ia 			= fCycBuf[1]*2048;*/
			/*mpr_ib 			= fCycBuf[2]*2048;*/
			/*mpr_ic 			= fCycBuf[3]*2048;*/
			/*mpr_uab 		= fCycBuf[4]*2048;*/
			/*mpr_ubc 		= fCycBuf[5]*2048;*/
			/*mpr_uca 		= fCycBuf[6]*2048;*/
			/*mpr_udc 		= fCycBuf[7]*2048;*/
			/*UsCtrl_d 		= fCycBuf[8]*2048;*/
			/*UsCtrl_q 		= fCycBuf[9]*2048;*/
			/*Ia_Rms 			= fCycBuf[10]*2048;*/
			/*Ib_Rms 			= fCycBuf[11]*2048;*/
			/*Ic_Rms 			= fCycBuf[12]*2048;*/
			/*Ua_Rms 			= fCycBuf[13]*2048;*/
			/*Ub_Rms 			= fCycBuf[14]*2048;*/
			/*Uc_Rms 			= fCycBuf[15]*2048;*/
			/*Uab_Rms 		= fCycBuf[16]*2048;*/
			/*Ubc_Rms 		= fCycBuf[17]*2048;*/
			/*Uca_Rms 		= fCycBuf[18]*2048;*/
			/*MPR_Ps 			= fCycBuf[19]*4480;*/
			/*MPR_Qs 			= fCycBuf[20]*4480;*/
			/*MPR_Ss 			= fCycBuf[21]*5270;*/
			/*Ioa_Rms 		= fCycBuf[22]*2048;*/
			/*Iob_Rms 		= fCycBuf[23]*2048;*/
			/*Ioc_Rms 		= fCycBuf[24]*2048;*/
			/*UabBus1_Rms 	= fCycBuf[25]*2048;*/
			/*UbcBus1_Rms 	= fCycBuf[26]*2048;*/
			/*UcaBus1_Rms 	= fCycBuf[27]*2048;*/
			/*UabBus2_Rms 	= fCycBuf[28]*2048;*/
			/*UbcBus2_Rms 	= fCycBuf[29]*2048;*/
			/*UcaBus2_Rms 	= fCycBuf[30]*2048;*/
			/*mpr_id			= fCycBuf[31]*2048;*/
			/*mpr_iq			= fCycBuf[32]*2048;*/
			/*mpr_idf			= fCycBuf[33]*2048;*/
			/*mpr_iqf			= fCycBuf[34]*2048;*/
			/*Id_Loop_Out		= fCycBuf[35]*2048;*/
			/*Iq_Loop_Out		= fCycBuf[36]*2048;*/
			/*IdRef_Outer 	= fCycBuf[37]*2048;*/
			/*IqRef_Outer 	= fCycBuf[38]*2048;*/
			/*if(MainStusWord.DataBit.SingPara)*/
			/*{*/
				/*UdRefRef_Outer =  fCycBuf[39]*2048;*/
			/*}*/
			/*UdcOffset_Outer = fCycBuf[40]*2048;*/
			/*mpr_iod			= fCycBuf[41]*2048;*/
			/*mpr_ioq			= fCycBuf[42]*2048;*/
			/*mpr_iodf		= fCycBuf[43]*2048;*/
			/*mpr_ioqf		= fCycBuf[44]*2048;*/
			/*//////////////////////////////////////////////////////////////////*/
			/*FPGAWR_CTRLREF = DspToFpgaWord.DataInt;*/
			/*fCycBuf[0] 	= Idq_Loop_Kp;*/
			/*fCycBuf[1] 	= Idq_Loop_Ki;*/
			/*fCycBuf[2] 	= Id_Loop_Lim/2048.0;*/
			/*fCycBuf[3] 	= Id_Loop_Lim/2048.0;  //Iq_Loop_Lim/2048.0;*/
			/*fCycBuf[4] 	= FltThr_OverVolt_Udc/2048.0;*/
			/*fCycBuf[5] 	= FltThr_OverCurr_Ipeak/2048.0;*/
			/*fCycBuf[6] 	= FilterPara_Induct;*/
			/*fCycBuf[7] 	= FastChar_DCOffset/2048.0;*/
			/*fCycBuf[8] 	= Para_IoFwdCoeff;*/
			/*for(i=0; i<9; i++)*/
			/*{*/
				/*lCycBuf[i] 	= (s32)(fCycBuf[i]*16777216.0);*/
				/*iEmifH 		=  (s16)((lCycBuf[i]>>16) & 0x0000FFFF);*/
				/*iEmifL 		=  (s16)((lCycBuf[i]>> 0) & 0x0000FFFF);*/
				/**(unsigned  int *)(0x4110 + 2*i) = iEmifH;*/
				/**(unsigned  int *)(0x4111 + 2*i) = iEmifL;*/
			/*}*/
			/*//////////////////////////////////////////////////////////////////*/
			/*Flag_DspWrFpga ^= 0xFFFF;*/
			/*FPGAWR_TOGGLE = Flag_DspWrFpga;*/

			/*Flag_DspRdFpga = FPGARD_TOGGLE;*/
			/*if(Flag_DspRdFpga != Flag_DspWrFpga)*/
			/*{*/
				/*if(MPR_FaultCode == 0)*/
				/*{*/
					/*FunFaultReact();*/
					/*MPR_FaultCode = 250;*/
					/*FaultWord_Map(250);*/
				/*}*/
			/*}*/

			/*if(ParaDownloadCtrlWord == 0)*/
				/*IO_Scan();*/
			/*mpr_1ms_protection();*/
			/*mpr_1ms_AlarmCheck();*/

			/*Source_Select();*/
			/*Udq_Ramp_Limit(MainStusWord.DataBit.MprRun);*/
			/*Inner_ADCScan();*/
			/*GetPQVIRms();*/
			/*CoolFanCtrl();*/
			/*FunResetAct();*/
			/*FunStatusUpdate();*/
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
			/*if(MainStusWord.DataBit.NprFault | MainStusWord.DataBit.MprFault)*/
			/*{*/
				/*Status_MPRRun = 12;*/
			/*}*/

			/*FpgaToDspWord.DataBit.SyncOk = 1;*/
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
			/*if(Status_MPRRun == 0)*/
			/*{*/
				/*if(ParaDownloadCtrlWord == 0)*/
				/*if(SysCnt_1ms > 500)*/
				/*{*/
					/*SysCnt_1ms = 0;*/

					/*IO_ACB1ON_Dis;*/
					/*IO_ACB1OFF_Dis;*/
					/*MainStusWord.DataBit.ACB1Req = 0;*/

					/*SlaToMasWord.bit.ParallReq = 0;*/

					/*Status_MPRRun = 1;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 1)*/
			/*{*/
				/*if(SysCnt_1ms > 1000)*/
				/*{*/
					/*SysCnt_1ms = 0;*/

					/*if(MainStusWord.DataBit.SingPara == 0)*/
					/*{*/
						/*Status_MPRRun = 2;*/
					/*}*/
					/*else*/
					/*{*/
						/*if(MasToSlaWord.bit.AllowAdd == 1)*/
						/*{*/
							/*Status_MPRRun = 2;*/
						/*}*/
					/*}*/
				/*}*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*else if(Status_MPRRun == 2)*/
			/*{*/
				/*if(MainCtrlWord.DataBit.NPR_Run == 1)*/
				/*if(!(MainStusWord.DataBit.NprFault | MainStusWord.DataBit.MprFault) )*/
				/*if(SysCnt_1ms > 500)*/
				/*{*/
					/*SysCnt_1ms = 0;*/
					/*IO_NprRun_En;*/
					/*Status_MPRRun = 3;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 3)*/
			/*{*/
				/*if(SysCnt_1ms > 100)*/
				/*{*/
					/*IO_ACB1OFF_Dis;*/
				/*}*/

				/*if(MainCtrlWord.DataBit.NPR_Run == 0)*/
				/*{*/
					/*IO_NprRun_Dis;*/

					/*Status_MPRRun = 2;*/
				/*}*/
				/*else if(MainCtrlWord.DataBit.NPR_Run == 1) // ctrl npr start*/
				/*{*/
					/*if(MainStusWord.DataBit.NPRRun)*/
					/*if(SysCnt_1ms > 500)*/
					/*{*/
						/*SysCnt_1ms = 0;*/
						/*Status_MPRRun = 4;*/
					/*}*/
				/*}*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*else if(Status_MPRRun == 4)*/
			/*{*/
				/*if(SysCnt_1ms > 1000)*/
				/*{*/
					/*SysCnt_1ms = 0;*/

					/*if(MainStusWord.DataBit.SingPara == 0)*/
					/*{*/
						/*Status_MPRRun = 5;*/

						/*MainStusWord.DataBit.VoltSel = 0;*/
					/*}*/
					/*else if( MainStusWord.DataBit.SingPara & FpgaToDspWord.DataBit.SyncOk)*/
					/*{*/
						/*if(MainCtrlWord.DataBit.PWMON)*/
						/*{*/
							/*if(MasToSlaWord.bit.BusBuild == 0)*/
							/*{*/
								/*IO_ACB1ON_En;*/
								/*IO_ACB1OFF_Dis;*/
								/*MainStusWord.DataBit.ACB1Req = 1;*/

								/*MainStusWord.DataBit.VoltSel = 1;*/

								/*SysCnt_1ms = 0;*/

								/*Status_MPRRun = 7;*/
							/*}*/
							/*else*/
							/*{*/
								/*IO_ACB1OFF_Dis;*/
								/*MainStusWord.DataBit.ACB1Req = 0;*/

								/*MainStusWord.DataBit.VoltSel = 0;*/

								/*Status_MPRRun = 9;*/
							/*}*/
							/*SlaToMasWord.bit.ParallReq = 1;*/
						/*}*/
					/*}*/
				/*}*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*// Power Single run*/
			/*else if(Status_MPRRun == 5)*/
			/*{*/
				/*if(MainCtrlWord.DataBit.NPR_Run == 0)*/
				/*if(SysCnt_1ms > 500)*/
				/*{*/
					/*SysCnt_1ms = 0;*/
					/*IO_NprRun_Dis;*/
					/*Status_MPRRun = 3;*/
				/*}*/

				/*if( (MainCtrlWord.DataBit.PWMON == 1) && (MainStusWord.DataBit.MprRun == 0) )*/
				/*if(MainStusWord.DataBit.NPRRun == 1)*/
				/*if(MainStusWord.DataBit.ACB1Ack == 0)*/
				/*if(!(MainStusWord.DataBit.NprFault | MainStusWord.DataBit.MprFault) )*/
				/*{*/
					/*En_PWM();*/
					/*MainStusWord.DataBit.MprRun = 1;*/

					/*SlaToMasWord.bit.ParallReq = 0;*/

					/*Status_MPRRun = 6;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 6)*/
			/*{*/
				/*if(MainCtrlWord.DataBit.PWMON == 0)*/
				/*{*/
					/*if(UsdRef < fabs(10) )*/
					/*{*/
						/*Dis_PWM();*/
						/*MainStusWord.DataBit.MprRun = 0;*/

						/*IO_ACB1OFF_Dis;*/
						/*IO_ACB1ON_Dis;*/
						/*MainStusWord.DataBit.ACB1Req = 0;*/

						/*Status_MPRRun = 5;*/

						/*FastChar_RunStatus = 0;*/
					/*}*/
				/*}*/


			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*// Power Paller not voltgle*/
			/*else if(Status_MPRRun == 7)*/
			/*{*/
				/*if(SysCnt_1ms > 100)*/
				/*{*/
					/*IO_ACB1ON_Dis;*/
				/*}*/

				/*if(MainCtrlWord.DataBit.NPR_Run == 0)*/
				/*if(SysCnt_1ms > 500)*/
				/*{*/
					/*SysCnt_1ms = 0;*/
					/*IO_NprRun_Dis;*/

					/*Status_MPRRun = 3;*/
				/*}*/

				/*if( (MainCtrlWord.DataBit.PWMON == 1) && (MainStusWord.DataBit.MprRun == 0) )*/
				/*if(MainStusWord.DataBit.NPRRun == 1)*/
				/*if( (MasToSlaWord.bit.ParallAck == 1) && (MainStusWord.DataBit.ACB1Ack == 1) )*/
				/*if(!(MainStusWord.DataBit.NprFault | MainStusWord.DataBit.MprFault) )*/
				/*{*/
					/*IO_ACB1ON_Dis;*/
					/*IO_ACB1OFF_Dis;*/

					/*En_PWM();*/
					/*MainStusWord.DataBit.MprRun = 1;*/

					/*SlaToMasWord.bit.ParallReq = 0;*/

					/*Status_MPRRun = 8;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 8)*/
			/*{*/

				/*if(MainCtrlWord.DataBit.PWMON == 0)*/
				/*{*/
					/*Dis_PWM();*/
					/*MainStusWord.DataBit.MprRun = 0;*/

					/*IO_ACB1OFF_En;*/
					/*IO_ACB1ON_Dis;*/
					/*MainStusWord.DataBit.ACB1Req = 0;*/

					/*Status_MPRRun = 3;*/

					/*SysCnt_1ms = 0;*/

					/*FastChar_RunStatus = 0;*/
				/*}*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*// Power Paller have voltgle*/
			/*else if(Status_MPRRun == 9)*/
			/*{*/
				/*if(MainCtrlWord.DataBit.NPR_Run == 0)*/
				/*if(SysCnt_1ms > 500)*/
				/*{*/
					/*SysCnt_1ms = 0;*/
					/*IO_NprRun_Dis;*/
					/*Status_MPRRun = 3;*/
				/*}*/

				/*if( (MainCtrlWord.DataBit.PWMON == 1) && (MainStusWord.DataBit.MprRun == 0) )*/
				/*if(MainStusWord.DataBit.NPRRun == 1)*/
				/*if( (MasToSlaWord.bit.ParallAck == 1) && (MainStusWord.DataBit.ACB1Ack == 0) )*/
				/*if(!(MainStusWord.DataBit.NprFault | MainStusWord.DataBit.MprFault) )*/
				/*{*/
					/*En_PWM();*/
					/*MainStusWord.DataBit.MprRun = 1;*/

					/*SlaToMasWord.bit.ParallReq = 0;*/

					/*Status_MPRRun = 10;*/

					/*SysCnt_1ms = 0;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 10)*/
			/*{*/
				/*if(SlaToMasWord.bit.AddBusOk == 1)*/
				/*{*/
					/*IO_ACB1ON_En;*/
					/*IO_ACB1OFF_Dis;*/
					/*MainStusWord.DataBit.ACB1Req = 1;*/

					/*MainStusWord.DataBit.VoltSel = 1;*/

					/*SysCnt_1ms = 0;*/

					/*Status_MPRRun = 11;*/
				/*}*/
			/*}*/
			/*else if(Status_MPRRun == 11)*/
			/*{*/
				/*if(SysCnt_1ms > 100)*/
				/*{*/
					/*IO_ACB1ON_Dis;*/
				/*}*/

				/*if(MainCtrlWord.DataBit.PWMON == 0)*/
				/*{*/
					/*Dis_PWM();*/
					/*MainStusWord.DataBit.MprRun = 0;*/

					/*IO_ACB1OFF_En;*/
					/*IO_ACB1ON_Dis;*/
					/*MainStusWord.DataBit.ACB1Req = 0;*/

					/*Status_MPRRun = 3;*/

					/*SysCnt_1ms = 0;*/

					/*FastChar_RunStatus = 0;*/
				/*}*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
			/*if(Status_MPRRun == 12)*/
			/*{*/
				/*Dis_PWM();*/
				/*MainStusWord.DataBit.MprRun = 0;*/

				/*IO_NprRun_Dis;*/

				/*IO_ACB1OFF_En;*/
				/*IO_ACB1ON_Dis;*/
				/*MainStusWord.DataBit.ACB1Req = 0;*/

				/*FastChar_RunStatus = 0;*/
			/*}*/
/*///////////////////////////////////////////////////////////////////////////////*/
		/*}*/
	/*}*/
}
/******************************************************************/
void INT28335_process(void)
{
	InitPieCtrl();
	IER = 0x0000;    // Disable CPU interrupts and clear all CPU interrupt flags:
	IFR = 0x0000;
	InitPieVectTable();

	EALLOW;
	PieVectTable.EPWM1_TZINT = &EPWM1_TZINT_isr;
	PieVectTable.TINT2       = &Timer2INT_ISR;
	PieVectTable.XINT1		 = &XINT1_FPGA_isr;
	PieVectTable.ECAN0INTA   = &CANARecv_isr;
	PieVectTable.ECAN0INTB   = &CANBRecv_isr;
	EDIS;

	IER |= M_INT2;   // Enable EPWM INTn in the PIE: Group 2 interrupt 1-2
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;

//	IER |= M_INT3;   // PWM1(PWM) PWM2(ADSample)
//	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;  //XINT1

	IER |= M_INT9;   // Enable TINT0 in the PIE: Group 1 interrupt 5
	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;
	IER |= M_INT14;

	EnableInterrupts();
	Dis_PWM();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1;		// CPU Timer1
	EDIS;
}
/******************************************************************/
void pwm_init(void)
{
	Dis_PWM();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	InitEPwm1();
	EDIS;
}
/******************************************************************/
void GetFlowRate_Calc(void)
{
	float ftmp;
	int itmp;

	itmp = *(unsigned  int *)0x4003;
	ftmp = -itmp * 0.00030518509;

	if(ftmp < 0.4)
		ftmp = 0.4;

	if(ftmp > 2)
		ftmp = 2;

	ftmp = ftmp *54.46 - 20.6;

	FlowRate = FlowRate*0.9 + ftmp*0.1;
}
/******************************************************************/
void ConvInner_CANAComm(void)
{
//	CANA_Sector_Send_Power();
//	CANA_Sector_Recv_Power();
////////////////////////////////////CH1//////////////////////////////////
	CAN_Pub_CHx_NMTRecv_Slaver(CANAMB4RecvBuffer,&CANAMB4RecvFront,CANAMB4RecvTeal,4,
						&ECANA_Toggle,CANAMB0SendBuffer,&CANAMB0SendTeal,16);
	CAN_Pub_CHx_SDO_ProtocalRecv_Slaver(CANAMB4RecvBuffer,&CANAMB4RecvFront,CANAMB4RecvTeal,4,
						MPR2NPR_CANSDOSummary,CANAMB0SendBuffer,&CANAMB0SendTeal,16);
	CAN_Pub_CHx_ProtocalRecv(CANAMB4RecvBuffer,&CANAMB4RecvFront,CANAMB4RecvTeal,4,MPR2NPR_RPDOObj);
	CANA_NPR_Recv_MPR();
/////////////////////////////////////////////////////////////////////////////////
	if(CANSend_1ms != CANSend_1msOld)
	{
///////////////////////////CH1//////////////////////////////////////////////////
		CAN_Pub_Low_CHx_ProtocalSend(CANAMB0SendBuffer, &CANAMB0SendTeal, 16, MPR2NPR_TPDOObj, CANSend_1ms);
		if((ECANA_Toggle & 0x0080)==0)
		{
			CANA_CH1_HCount=0; CANA_CH1_LCount++;
		}
		else
		{
			CANA_CH1_LCount=0; CANA_CH1_HCount++;
		}

		if(ECANA_FaultMask.DataBit.Bit0 == 1)
		{
			if((CANA_CH1_HCount>=5000) || (CANA_CH1_LCount>=5000))
			{
				CANA_CH1_HCount = 0;
				CANA_CH1_LCount = 0;
				ECANA_FaultCode.DataBit.Bit0 = 1;
			}
		}
	}
	if(RealTime1ms >= 20)
	{
		RealTime20msFun();
		RealTime1ms = 0;
	}
//////////////////////////////////////////////////////////////////////////
	CANA_NPR_Send_MPR();
	CANAMB0HardwareSend(CANAMB0SendBuffer, CANAMB0SendTeal, &CANAMB0SendFront, 16, CANSend_1ms);
}
/******************************************************************/
void ConvInner_CANBComm(void)
{
	MCS2MPR_ProtocalRecv_Slaver(CANBMB5RecvBuffer, &CANBMB5RecvFront, CANBMB5RecvTeal, CANBMB0SDOSendBuffer,&CANBMB0SDOSendTeal);
//	CANB_NPR_Recv_MCS();
//	CANB_NPR_Send_MCS();
	CANB_Sector_Send_Power();
	CANB_Sector_Recv_Power();
//	if((NODE_STATE&0x7F) == 0x05)	//Send PDO
	{
		MCS2MPR_ProtocalSend_Slaver(CANBMB0PDOSendBuffer, &CANBMB0PDOSendTeal, CANSend_1ms, 1);
	}
	CANBMB0HardwareSend(CANBMB0PDOSendBuffer, CANBMB0PDOSendTeal, &CANBMB0PDOSendFront, CANBMB0SDOSendBuffer, CANBMB0SDOSendTeal, &CANBMB0SDOSendFront, CANSend_1ms);//锟斤拷莘锟斤拷锟�
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
	if(SysTimeYearL > 63)
		SysTimeYearL = 63;

	ulRealTimeSecond  = (((long)SysTimeYearL) <<26) & 0xFC000000;
	ulRealTimeSecond |= ( (long)SysTime_Month <<22) & 0x03C00000;
	ulRealTimeSecond |= ( (long)SysTime_Day   <<17) & 0x003E0000;
	ulRealTimeSecond |= ( (long)SysTime_Hour  <<12) & 0x0001F000;
	ulRealTimeSecond |= (       SysTime_Minute<<6 ) & 0x00000FC0;
	ulRealTimeSecond |= SysTimeSecond               & 0x003F;
}
/******************************************************************/
void FunFaultReact(void)
{
	Dis_PWM();
	MainStusWord.DataBit.MprRun = 0;

	IO_NprRun_Dis;

	IO_ACB1OFF_En;
	IO_ACB1ON_Dis;
	MainStusWord.DataBit.ACB1Req = 0;

	IO_ACB2OFF_En;
	IO_ACB2ON_Dis;
	MainStusWord.DataBit.ACB2Req = 0;

	FastChar_RunStatus = 0;
}
/******************************************************************/
void CoolFanCtrl(void)
{
//	if(mpr_temphigh >= CtrlThr_FanIgbtOn_TempH)
//	{
//		if(CtrlCnt_IgbtFanOn < 2000)
//		{
//			CtrlCnt_IgbtFanOn ++;
//		}
//		else
//		{
//			IO_FAN_IGBT_ON;
//			Flag_FanIgbtOn = 1;
//		}
//	}
//
//	if(mpr_temphigh <= CtrlThr_FanIgbtOff_TempL)
//	{
//		if(CtrlCnt_IgbtFanOn > 100)
//		{
//			CtrlCnt_IgbtFanOn --;
//		}
//		else
//		{
//			IO_FAN_IGBT_OFF;
//			Flag_FanIgbtOn = 0;
//		}
//	}
}
/******************************************************************/
void FunResetAct(void)
{
	MainCtrlWord.DataBit.Reset = MainCtrlWord_PC.DataBit.Reset;
	if(MainStusWord.DataBit.MprRun == 1)
		MainCtrlWord.DataBit.Reset = 0;

	if(MainCtrlWord.DataBit.Reset==1)
	{
		MainCtrlWord.DataInt = 0;
		MainCtrlWord.DataBit.Reset = 1;

		MainCtrlWord_PC.DataInt = 0;
		MainCtrlWord_PC.DataBit.Reset = 1;

		MainCtrlWord_HMI.DataInt = 0;
		MainCtrlWord_HMI.DataBit.Reset = 1;

		MainCtrlWord_CAN.DataInt = 0;
		MainCtrlWord_CAN.DataBit.Reset = 1;

		IO_FPGARST_EN;

		if(CtrlCnt_RstDelay == 0)
			CtrlCnt_RstDelay = 1;
	}

	if(CtrlCnt_RstDelay != 0)
	{
		CtrlCnt_RstDelay ++;
		if(CtrlCnt_RstDelay < 100)
		{
			IO_NprRst_En;
			MainCtrlWord.DataBit.Reset = 1;
		}
		if(CtrlCnt_RstDelay > 2000)
		{
			CtrlCnt_RstDelay=0;
			MainCtrlWord.DataBit.Reset=0;
			MainCtrlWord_PC.DataBit.Reset=0;
			MainCtrlWord_HMI.DataBit.Reset=0;
			MainCtrlWord_CAN.DataBit.Reset=0;

			IO_NprRst_Dis;
		}
	}

	if((MainCtrlWord.DataBit.Reset == 0) && ((Flag_RstCmdOld & 0x0001) == 1))
	{
		if(MainStusWord.DataBit.MprRun == 0)
		{
			IO_PWM_UNLOCK;
			IO_FPGARST_DIS;

			variable_algorithm_init();
			MainCtrlWord.DataInt = 0;
			MainCtrlWord_CAN.DataInt = 0;

			MPR_FaultCode = 0;
			FaultWord1.DataInt = 0;
			FaultWord2.DataInt = 0;
			FaultWord3.DataInt = 0;
			FaultWord4.DataInt = 0;
			FaultWord5.DataInt = 0;
			FaultWord6.DataInt = 0;
			FaultWord7.DataInt = 0;
			FaultWord8.DataInt = 0;

			AlarmWord1.DataInt = 0;
			AlarmWord2.DataInt = 0;
			AlarmWord3.DataInt = 0;
			AlarmWord4.DataInt = 0;
			AlarmWord5.DataInt = 0;
			AlarmWord6.DataInt = 0;
			AlarmWord7.DataInt = 0;
			AlarmWord8.DataInt = 0;

			Status_MPRRun = 0;
			if(Manu_Oper_Force == 10)
			{
				Status_MPRRun = 0;
			}
			//
			FastChar_RunStatus = 0;
		}
	}

	Flag_RstCmdOld = MainCtrlWord.DataBit.Reset;
}
/******************************************************************/
void FunStatusUpdate(void)
{
	if(MainCtrlWord_PC.DataBit.Stop == 1)
		MainCtrlWord.DataBit.Stop = 1;

	if(MainCtrlWord.DataBit.Reset == 0)
	if(MainCtrlWord.DataBit.Stop == 1)
	{
		MainCtrlWord.DataInt = 0;
		MainCtrlWord.DataBit.Stop = 1;

		MainCtrlWord_PC.DataInt = 0;
		MainCtrlWord_PC.DataBit.Stop = 1;

		MainCtrlWord_HMI.DataInt = 0;
		MainCtrlWord_HMI.DataBit.Stop = 1;

		MainCtrlWord_CAN.DataInt = 0;
		MainCtrlWord_CAN.DataBit.Stop = 1;
	}
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	if(MPR_FaultCode)
	{
		MainStusWord.DataBit.MprFault = 1;
	}
	else
	{
		MainStusWord.DataBit.MprFault = 0;
	}
	//////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MprRun == 0)
	{
		MainStusWord.DataBit.WorkMode = IO_WorkRef_Feed;
	}
	//////////////////////////////////////////////////////////
	if(MainCtrlWord.DataBit.Reset == 1)
	{
		MainStusWord.DataBit.NprFault = 0;
	}
	else
	{
		MainStusWord.DataBit.NprFault = IO_NprFault_Feed;
	}
	//////////////////////////////////////////////////////////
	MainStusWord.DataBit.NPRRun = IO_NprRun_Feed;
	MainStusWord.DataBit.SingPara = IO_SingPara_Feed;
	MainStusWord.DataBit.LocRem  = IO_LocRem_Feed;
	MainStusWord.DataBit.ACB1Ack = IO_ACBBus1_Feed;
	MainStusWord.DataBit.ACB2Ack = IO_ACBBus2_Feed;
	//////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MprFault | MainStusWord.DataBit.NprFault)
	{
		IO_Fault_ON;
	}
	else
	{
		IO_Fault_OFF;
	}
	//////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MprRun == 1)
	{
		if(UsdRef > (UsdRefRef*0.95) )
		{
			IO_Run_ON;
		}
		else
		{
			if(SysCnt_1s != SysCnt_1sOld)
			{
				SysCnt_1sOld = SysCnt_1s;
				IO_Run_TOG;
			}
		}
	}
	else
	{
		IO_Run_OFF;
	}
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	SlaToMas1Word.bit.Fault     = MainStusWord.DataBit.MprFault | MainStusWord.DataBit.NprFault;
	SlaToMas1Word.bit.Run       = MainStusWord.DataBit.MprRun;
	SlaToMas1Word.bit.WorkMode  = MainStusWord.DataBit.WorkMode;
	SlaToMas1Word.bit.SingPara  = MainStusWord.DataBit.SingPara;
	SlaToMas1Word.bit.Bus1ACB   = MainStusWord.DataBit.ACB1Ack;
	SlaToMas1Word.bit.ActOnline = HeartbeatCheck1.Comm_result;
	SlaToMas1Word.bit.BakOnline = HeartbeatCheck2.Comm_result;
	SlaToMas1Word.bit.MasSelect = !MainStusWord.DataBit.MasSel;
	SlaToMas1Word.bit.ParallReq = SlaToMasWord.bit.ParallReq;
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	SlaToMas2Word.bit.Fault     = MainStusWord.DataBit.MprFault | MainStusWord.DataBit.NprFault;
	SlaToMas2Word.bit.Run       = MainStusWord.DataBit.MprRun;
	SlaToMas2Word.bit.WorkMode  = MainStusWord.DataBit.WorkMode;
	SlaToMas2Word.bit.SingPara  = MainStusWord.DataBit.SingPara;
	SlaToMas2Word.bit.Bus1ACB   = MainStusWord.DataBit.ACB1Ack;
	SlaToMas2Word.bit.ActOnline = HeartbeatCheck2.Comm_result;
	SlaToMas2Word.bit.BakOnline = HeartbeatCheck1.Comm_result;
	SlaToMas2Word.bit.MasSelect = MainStusWord.DataBit.MasSel;
	SlaToMas2Word.bit.ParallReq = SlaToMasWord.bit.ParallReq;
	/////////////////////////////////////////////////////////////////
	if( (MainStusWord.DataBit.MprRun==1)&& (fabs(UsdRefRef - mpr_udf) < 30))
	{
		CtrlCnt_AddBusOk ++;
		if(CtrlCnt_AddBusOk > 5000)
		{
			CtrlCnt_AddBusOk = 5000;
			SlaToMasWord.bit.AddBusOk = 1;
			SlaToMas1Word.bit.AddBusOk = 1;
			SlaToMas2Word.bit.AddBusOk = 1;
		}
	}
	else
	{
		CtrlCnt_AddBusOk = 0;
		SlaToMasWord.bit.AddBusOk = 0;
		SlaToMas1Word.bit.AddBusOk = 0;
		SlaToMas2Word.bit.AddBusOk = 0;
	}
	/////////////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MasSel == 0)
	{
		MasToSlaWord.all = Mas1ToSlaWord.all;
	}
	else
	{
		MasToSlaWord.all = Mas2ToSlaWord.all;
	}
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	DspToFpgaWord.DataBit.VoltSel  = MainStusWord.DataBit.VoltSel;
	DspToFpgaWord.DataBit.WorkMode = MainStusWord.DataBit.WorkMode;
	DspToFpgaWord.DataBit.MasSel = MainStusWord.DataBit.MasSel;
	DspToFpgaWord.DataBit.VoltForward = 0;
	DspToFpgaWord.DataBit.CurrCouple = 0;
	if(Status_MPRRun > 0)
	{
		DspToFpgaWord.DataBit.ReadyOk = 1;
	}
	DspToFpgaWord.DataBit.IoutFwdEn = 0;
	/////////////////////////////////////////////////////////////////
}
/******************************************************************/
void mpr_1ms_AlarmCheck(void)
{
	if(MainStusWord.DataBit.WorkMode == 0)
	{
		if(vLine_max > AlmThr_OverVolt_Work1)
		{
			AlarmWord_Map(129);
		}
		else
		{
			AlarmWord_Clear(129);
		}
	}
	else
	{
		if(vLine_max > AlmThr_OverVolt_Work2)
		{
			AlarmWord_Map(130);
		}
		else
		{
			AlarmWord_Clear(130);
		}
	}

	if(MainStusWord.DataBit.WorkMode == 0)
	{
		if((MainStusWord.DataBit.MprRun == 1)&&(MainCtrlWord.DataBit.PWMON == 1))
		{
			if(UsdRef > UsdRefRef*0.95)
			{
				if(vLine_min < AlmThr_UnderVolt_Work1)
				{
					MPR_AlarmCode = 131;
					AlarmWord_Map(131);
				}
				else
				{
					AlarmWord_Clear(131);
				}
			}
			else
			{
				AlarmWord_Clear(131);
			}
		}
		else
		{
			AlarmWord_Clear(131);
		}
	}
	else
	{
		if((MainStusWord.DataBit.MprRun == 1)&&(MainCtrlWord.DataBit.PWMON == 1))
		{
			if(UsdRef > UsdRefRef*0.95)
			{
				if(vLine_min < AlmThr_UnderVolt_Work2)
				{
					if(MPR_AlarmCode == 0)
					{
						MPR_AlarmCode = 132;
					}
					AlarmWord_Map(132);
				}
				else
				{
					AlarmWord_Clear(132);
				}
			}
			else
			{
				AlarmWord_Clear(132);
			}
		}
		else
		{
			AlarmWord_Clear(132);
		}
	}
	///////////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MprRun == 1)
	{
		if( UsdRef > (UsdRefRef*0.95) )
		{
			if(fabs(MotorFreqRef - FpgaMotorFreq) > AlmThr_OverDvalue_Freq)
			{
				AlarmWord_Map(133);
			}
			else
			{
				AlarmWord_Clear(133);
			}
		}
		else
		{
			AlarmWord_Clear(133);
		}
	}
	else
	{
		AlarmWord_Clear(133);
	}
	////////////////////////////////////////////////////////
	if(mpr_temphigh > AlmThr_OverTemp_IGBT)
	{
		AlarmWord_Map(134);
	}
	else
	{
		AlarmWord_Clear(134);
	}
	//----------------------------------------
	if((mpr_udc >= AlmThr_OverVolt_Udc) && (AlmThr_OverVolt_Udc > 0) )
	{
		if(MPR_AlarmCode == 0)
		{
			MPR_AlarmCode = 135;
		}
		AlarmWord_Map(135);
	}
	else
	{
		AlarmWord_Clear(135);
	}
	/////////////////////////////////////////////
	if((MainStusWord.DataBit.MprRun == 1) && (mpr_udc < AlmThr_UnderVolt_Udc))
	{
		if(MPR_AlarmCode == 0)
		{
			MPR_AlarmCode = 136;
		}
		AlarmWord_Map(136);
	}
	else
	{
		AlarmWord_Clear(136);
	}

	//////////////////////////////////////////////////////////
	if((Iphase_max >= AlmThr_OverCurr_Irms) && (AlmThr_OverCurr_Irms > 0))
	{
		if(Iphase_max == Ia_Rmsf)
		{
			AlarmWord_Map(137);
		}
		else if(Iphase_max == Ib_Rmsf)
		{
			AlarmWord_Map(138);
		}
		else if(Iphase_max == Ic_Rmsf)
		{
			AlarmWord_Map(139);
		}
	}
	else
	{
		AlarmWord_Clear(137);
		AlarmWord_Clear(138);
		AlarmWord_Clear(139);
	}
	/////////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.LocRem == 1)//Rem
	{
		if( (Flag_IORunRef == 1) || (Flag_IORstRef == 1) || (Rst_Flag_HMI == 1))
		{
			Flag_IORunRef = 0;
			Flag_IORstRef = 0;
			Rst_Flag_HMI = 0;

			AlarmWord_Map(140);
			AlmCnt_OpearatError = 3000;
		}
		else
		{
			if(AlmCnt_OpearatError > 0)
			{
				AlmCnt_OpearatError--;
			}
			else
			{
				AlarmWord_Clear(140);
			}
		}
	}
	else
	{
		if(AlmCnt_OpearatError > 0)
		{
			AlmCnt_OpearatError --;
		}
		else
		{
			AlarmWord_Clear(140);
		}
	}
	//////////////////////////////////////////////////////////
	if(MotorVolt_Imbalance > AlmThr_OverImba_Volt)
	{
		AlarmWord_Map(141);
	}
	else
	{
		AlarmWord_Clear(141);
	}
	////////////////////////////////////////////////////////
	if(MotorCurr_Imbalance > AlmThr_OverImba_Curr)
	{
		AlarmWord_Map(142);
	}
	else
	{
		AlarmWord_Clear(142);
	}

//	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
//	if(CommToggle_MPR2ARM == 1)
//	{
//		MPR2ARM_CANCommLow_Count = 0;
//		MPR2ARM_CANCommHigh_Count ++;
//	}
//	else
//	{
//		MPR2ARM_CANCommHigh_Count = 0;
//		MPR2ARM_CANCommLow_Count ++;
//	}
//
//	if(( MPR2ARM_CANCommHigh_Count >= 60000)||( MPR2ARM_CANCommLow_Count >= 60000))
//	{
//		if(MPR2ARM_CANCommHigh_Count >= 60000)
//		{
//			MPR2ARM_CANCommHigh_Count = 60000;
//			MPR2ARM_CANCommLow_Count = 0;
//		}
//		else
//		{
//			MPR2ARM_CANCommLow_Count = 60000;
//			MPR2ARM_CANCommHigh_Count = 0;
//		}
//		AlarmWord_Map(146);
//		MPR2ARM_CANCommFlag = 0;
//	}
//	else
//	{
//		AlarmWord_Clear(146);
//		MPR2ARM_CANCommFlag = 1;
//	}
}
/******************************************************************/
void mpr_1ms_protection(void)
{
	float ftmp;
	int i;

//	///////////////////////////////////////////////////////////////////
//	// F16~F18  Over Curr 3AC
//	// F19~F21  Over Curr 3AC
//	// F22      Over Curr 1DC
//	// F23      Over Volg 2DC
//	// F24      Drive 3Phase
//	// F25      Connect 3Phase
	if(DspToFpgaWord.DataBit.ReadyOk)
	{
		for(i=0; i<9; i++)
		{
			if( ((IOInputFpga2.DataInt>>i) & 0x0001) != 0x0001 )
			{
				if(FaultMask_Map(145+i) == 1)
				{
					if(MPR_FaultCode == 0)
					{
						FunFaultReact();
						MPR_FaultCode = 145+i;
					}
				}
				FaultWord_Map(145+i);
			}
		}
		for(i=10; i<13; i++)
		{
			if( ((IOInputFpga2.DataInt>>i) & 0x0001) != 0x0001 )
			{
				if(FaultMask_Map(145+i) == 1)
				{
					if(MPR_FaultCode == 0)
					{
						FunFaultReact();
						MPR_FaultCode = 145+i;
					}
				}
				FaultWord_Map(145+i);
			}
		}
	}
////// F060 AC RMS OverCurr  //////////////////////////////////////////////////////////////
	if(Iphase_max > FltThr_OverCurr_Irms)
	{
		if(FltCnt_OverCurr_Irms < 10)
		{
			FltCnt_OverCurr_Irms ++;
		}
		else
		{
			if(Iphase_max == Ia_Rmsf)
			{
				if(FaultMask_Map(194) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 194;
				}
				FaultWord_Map(194);
			}
			else if(Iphase_max == Ib_Rmsf)
			{
				if(FaultMask_Map(195) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 195;
				}
				FaultWord_Map(195);
			}
			else if(Iphase_max == Ic_Rmsf)
			{
				if(FaultMask_Map(196) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 196;
				}
				FaultWord_Map(196);
			}
		}
	}
	else
	{
		if(FltCnt_OverCurr_Irms > 0)
		{
			FltCnt_OverCurr_Irms --;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////
	if(mpr_isum > FltThr_OverCurr_Isum)
	{
		if(FltCnt_OverCurr_Isum < 20)
		{
			FltCnt_OverCurr_Isum ++;
		}
		else
		{
			if(FaultMask_Map(198) == 1)
			if(MPR_FaultCode == 0)
			{
				FunFaultReact();
				MPR_FaultCode = 198;
			}
			FaultWord_Map(198);
		}
	}
	else
	{
		if(FltCnt_OverCurr_Isum > 0)
		{
			FltCnt_OverCurr_Isum --;
		}
	}
	////// F044 NTC Fault ////////////////////////////////////////////////////////////////////
	ftmp = fabs(mpr_temphigh - mpr_templow);
	if(ftmp > 10)
	{
		if(FltCnt_OverDvalue_Tempr < 1000)
		{
			FltCnt_OverDvalue_Tempr ++;
		}
		else
		{
			if(FaultMask_Map(199) == 1)
			if(MPR_FaultCode == 0)
			{
				FunFaultReact();
				MPR_FaultCode = 199;
			}
			FaultWord_Map(199);
		}
	}
	else
	{
		if( FltCnt_OverDvalue_Tempr > 0)
		{
			FltCnt_OverDvalue_Tempr --;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////
	////// F69~71 PowerUnit OverTempr Fault /////////////////////////////////////////////////////
	if(mpr_temphigh > FltThr_OverTemp_IGBT)
	{
		if(FltCnt_OverTemp_IGBT < 1000)
		{
			FltCnt_OverTemp_IGBT ++;
		}
		else
		{
			if( (mpr_temphigh == TemprCalc[1]) || (mpr_temphigh == TemprCalc[2]) )
			{
				if(FaultMask_Map(200) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 200;
				}
				FaultWord_Map(200);
			}

			if( (mpr_temphigh == TemprCalc[3]) || (mpr_temphigh == TemprCalc[4]) )
			{
				if(FaultMask_Map(201) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 201;
				}
				FaultWord_Map(201);
			}

			if( (mpr_temphigh == TemprCalc[5]) || (mpr_temphigh == TemprCalc[6]) )
			{
				if(FaultMask_Map(202) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 202;
				}
				FaultWord_Map(202);
			}
		}
	}
	else
	{
		if(FltCnt_OverTemp_IGBT > 0)
		{
			FltCnt_OverTemp_IGBT --;
		}
	}
	//////////////////////////////////////////////////////////////////
	if((MainStusWord.DataBit.MprRun == 1)&& (UsdRef > 100))
	{
		if(fabs(MotorFreqRef - FpgaMotorFreq) > FltThr_OverDvalue_Freq)
		{
			if(FltCnt_OverDvalue_Freq < 11000)
			{
				FltCnt_OverDvalue_Freq ++;
			}
			else
			{
				if(FaultMask_Map(203) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 203;
				}
				FaultWord_Map(203);
			}
		}
		else
		{
			if(FltCnt_OverDvalue_Freq > 0)
			{
				FltCnt_OverDvalue_Freq --;
			}
		}
	}
	else
	{
		FltCnt_OverDvalue_Freq = 0;
	}

	////////////////////////////////////////////////////////////////////////
//	if( ( (MainCtrlWord.DataBit.Cap40uF==1) && (MainStusWord.DataBit.Cap40uF==0) )
//	 || ( (MainCtrlWord.DataBit.Cap40uF==0) && (MainStusWord.DataBit.Cap40uF==1) )  )
//	{
//		if(FltCnt_OverTime_Cap40uF < 1000)
//		{
//			FltCnt_OverTime_Cap40uF ++;
//		}
//		else
//		{
//			if(FaultMask_Map(204) == 1)
//			if(MPR_FaultCode == 0)
//			{
//				FunFaultReact();
//				MPR_FaultCode = 204;
//			}
//			FaultWord_Map(204);
//		}
//	}
//	else
//	{
//		if(FltCnt_OverTime_Cap40uF > 0)
//		{
//			FltCnt_OverTime_Cap40uF --;
//		}
//	}
	////////////////////////////////////////////////////////////////////////
//	if( ( (MainCtrlWord.DataBit.Cap20uF==1) && (MainStusWord.DataBit.Cap20uF==0) )
//	 || ( (MainCtrlWord.DataBit.Cap20uF==0) && (MainStusWord.DataBit.Cap20uF==1) )  )
//	{
//		if(FltCnt_OverTime_Cap20uF < 1000)
//		{
//			FltCnt_OverTime_Cap20uF ++;
//		}
//		else
//		{
//			if(FaultMask_Map(205) == 1)
//			if(MPR_FaultCode == 0)
//			{
//				FunFaultReact();
//				MPR_FaultCode = 205;
//			}
//			FaultWord_Map(205);
//		}
//	}
//	else
//	{
//		if(FltCnt_OverTime_Cap20uF > 0)
//		{
//			FltCnt_OverTime_Cap20uF --;
//		}
//	}
	//////////////////////////////////////////////////////////
	if(CtrlCnt_StartNpr < 60000)
	{
		CtrlCnt_StartNpr ++;
	}
	else
	{
		if( ( (MainCtrlWord.DataBit.NPR_Run==1) && (MainStusWord.DataBit.NPRRun==0) )
		 || ( (MainCtrlWord.DataBit.NPR_Run==0) && (MainStusWord.DataBit.NPRRun==1) )  )
		{
			if(FltCnt_OverTime_NprRun < 60000)
			{
				if(MainStusWord.DataBit.MprRun == 1)
				{
					FltCnt_OverTime_NprRun ++;
				}
				else
				{
					FltCnt_OverTime_NprRun += 1;
				}
			}
			else
			{
				if(FaultMask_Map(206) == 1)
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 206;
				}
				FaultWord_Map(206);
			}
		}
		else
		{
			if(FltCnt_OverTime_NprRun > 0)
			{
				FltCnt_OverTime_NprRun --;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	if(PowerCtrlWord.DataBit.Toggle == 1)
	{
		FltCnt_ToggLow_CAN = 0;
		FltCnt_ToggHigh_CAN ++;
	}
	else
	{
		FltCnt_ToggHigh_CAN = 0;
		FltCnt_ToggLow_CAN ++;
	}

	if(( FltCnt_ToggHigh_CAN >= 3000)||( FltCnt_ToggLow_CAN >= 3000))
	{
		if(FltCnt_ToggHigh_CAN >= 3000)
		{
			FltCnt_ToggHigh_CAN = 3000;
			FltCnt_ToggLow_CAN = 0;
		}
		else
		{
			FltCnt_ToggLow_CAN = 3000;
			FltCnt_ToggHigh_CAN = 0;
		}
	}
	else
	{

	}
	/////////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.FastChar == 0)
	if(MotorVolt_Imbalance > FltThr_OverImba_Volt)
	{
		if(FltCnt_OverImba_Volt < 3000)
		{
			FltCnt_OverImba_Volt ++;
		}
		else
		{
			if(FaultMask_Map(215) == 1)
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 215;
			}
			FaultWord_Map(215);
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
	if(MainStusWord.DataBit.FastChar == 0)
	if(MotorCurr_Imbalance > FltThr_OverImba_Curr)
	{
		if(FltCnt_OverImba_Curr < 3000)
		{
			FltCnt_OverImba_Curr ++;
		}
		else
		{
			if(FaultMask_Map(216) == 1)
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 216;
			}
			FaultWord_Map(216);
		}
	}
	else
	{
		if(FltCnt_OverImba_Curr > 0)
		{
			FltCnt_OverImba_Curr --;
		}
	}

	//////////////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.MprRun == 1)
	{
		if(MainStusWord.DataBit.WorkMode == 0)
		{
			// work1 Over Volt
			if((vLine_max > FltThr_OverVolt_Work1) && (FltThr_OverVolt_Work1 > 0))
			{
				if(FltCnt_OverVolt_Work1 < 10)
				{
					FltCnt_OverVolt_Work1 ++;
				}
				else
				{
					if(FaultMask_Map(210) == 1)
					{
						if(!MPR_FaultCode)
						{
							FunFaultReact();
							MPR_FaultCode = 210;
						}
						FaultWord_Map(210);
					}
				}
			}
			else
			{
				if(FltCnt_OverVolt_Work1 > 0)
				{
					FltCnt_OverVolt_Work1 --;
				}
			}

			// work1 Under Volt
			if(UsdRef > (UsdRefRef*0.9))
			if((MainStusWord.DataBit.MprRun == 1) && (MainCtrlWord.DataBit.PWMON == 1))
			{
				CtrlCnt_FltDlyUV1 ++;
				if(CtrlCnt_FltDlyUV1 > 5000)
				{
					CtrlCnt_FltDlyUV1 = 5000;

					if( (vLine_min < FltThr_UnderVolt_Work1) && (FltThr_UnderVolt_Work1 > 0) )
					{
						if(FltCnt_UnderVolt_Work1 < 20)
						{
							FltCnt_UnderVolt_Work1 ++;
						}
						else
						{
							if(FaultMask_Map(211) == 1)
							{
								if(!MPR_FaultCode)
								{
									FunFaultReact();
									MPR_FaultCode = 211;
								}
								FaultWord_Map(211);
							}
						}
					}
					else
					{
						if(FltCnt_UnderVolt_Work1 > 0)
						{
							FltCnt_UnderVolt_Work1 --;
						}
					}
				}
			}
			else
			{
				CtrlCnt_FltDlyUV1 = 0;
				FltCnt_UnderVolt_Work1 = 0;
			}
		}
		else if(MainStusWord.DataBit.WorkMode == 1)
		{
			// Work2  Over Volt
			if(MainStusWord.DataBit.FastChar == 0)
			if(MainStusWord.DataBit.MprRun == 1)
			if((vLine_max > FltThr_OverVolt_Work2) && (FltThr_OverVolt_Work2 > 0))
			{
				if(FltCnt_OverVolt_Work2 <= 500)
				{
					FltCnt_OverVolt_Work2 += 1;
				}
				else
				{
					if(FaultMask_Map(212) == 1)
					{
						if(!MPR_FaultCode)
						{
							FunFaultReact();
							MPR_FaultCode = 212;
						}
						FaultWord_Map(212);
					}
				}
			}
			else
			{
				if(FltCnt_OverVolt_Work2 > 0)
				{
					FltCnt_OverVolt_Work2 --;
				}
			}
			// Work2  Under Volt
			if(MainStusWord.DataBit.FastChar == 0)
			if((MainStusWord.DataBit.MprRun == 1) && (MainCtrlWord.DataBit.PWMON == 1))
			if(UsdRef > (UsdRefRef*0.9))
			{
				CtrlCnt_FltDlyUV2 ++;
				if(CtrlCnt_FltDlyUV2 > 5000)
				{
					CtrlCnt_FltDlyUV2 = 5000;
					if((vLine_min < FltThr_UnderVolt_Work2)&&(FltThr_UnderVolt_Work2 > 0))
					{
						if(FltCnt_UnderVolt_Work2 <= 20)
						{
							FltCnt_UnderVolt_Work2 ++;
						}
						else
						{
							if(FaultMask_Map(213) == 1)
							{
								if(!MPR_FaultCode)
								{
									FunFaultReact();
									MPR_FaultCode = 213;
								}
								FaultWord_Map(213);
							}
						}
					}
					else
					{
						if(FltCnt_UnderVolt_Work2 > 0)
						{
							FltCnt_UnderVolt_Work2 --;
						}
					}
				}
			}
			else
			{
				CtrlCnt_FltDlyUV2 = 0;
				FltCnt_UnderVolt_Work2 =0;
			}
		}
	}
	///////////////////////////////////////////////////////////////////////
	// DC Volg Order
	if((MainStusWord.DataBit.MprRun == 1) && (mpr_udc < FltThr_UnderVolt_Udc))
	{
		if(FltCnt_UnderVolt_Udc < 200)
		{
			FltCnt_UnderVolt_Udc ++;
		}
		else
		{
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 181;
			}
			FaultWord_Map(181);
		}
	}
	else
	{
		if(FltCnt_UnderVolt_Udc > 0)
		{
			FltCnt_UnderVolt_Udc --;
		}
	}

	////////////////////////////////////////////
	if( MainStusWord.DataBit.ACB1Req != MainStusWord.DataBit.ACB1Ack )
	{
		if(FltCnt_OverTime_ACB1Out < 3000)
		{
			FltCnt_OverTime_ACB1Out ++;
		}
		else
		{
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 182;
			}
			FaultWord_Map(182);
		}
	}
	else
	{
		if(FltCnt_OverTime_ACB1Out > 0)
		{
			FltCnt_OverTime_ACB1Out --;
		}
	}

	if( MainStusWord.DataBit.ACB2Req != MainStusWord.DataBit.ACB2Ack )
	{
		if(FltCnt_OverTime_ACB2Out < 3000)
		{
			FltCnt_OverTime_ACB2Out ++;
		}
		else
		{
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 183;
			}
			FaultWord_Map(183);
		}
	}
	else
	{
		if(FltCnt_OverTime_ACB2Out > 0)
		{
			FltCnt_OverTime_ACB2Out --;
		}
	}
	////////////////////////////////////////////
	// 177 - ia oi
	// 178 - ib oi
	// 179 - ic oi
	// 180 - udc ov
	if(DspToFpgaWord.DataBit.ReadyOk)
	{
		for(i=0; i<4; i++)
		{
			if( ((FpgaSofaFault.DataInt>>i) & 0x0001) != 0x0001 )
			{
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 177+i;

				}
				FaultWord_Map(177+i);
			}
		}
	}
	//////////////////////////////////////////////////
	if(FlowRate < 25)
	{
		if(FltCnt_OverLow_FlowRate < 5000)
		{
			FltCnt_OverLow_FlowRate ++;
		}
		else
		{
			if(FaultMask_Map(220) == 1)
			if(!MPR_FaultCode)
			{
				FunFaultReact();
				MPR_FaultCode = 220;
			}
			FaultWord_Map(220);
		}
	}
	else
	{
		if(FltCnt_OverLow_FlowRate > 0)
		{
			FltCnt_OverLow_FlowRate --;
		}
	}
	//////////////////////////////////////////////////
}
/******************************************************************/
void mpr_pwm_protection(void)
{
//	// AC Curr Over
//	if(mpr_ipeak > FltThr_OverCurrEst_Ipeak)
//	{
//		FltCnt_OverCurr_Ipeak += 5;
//	}
//	else if(mpr_ipeak > FltThr_OverCurr_Ipeak)
//	{
//		FltCnt_OverCurr_Ipeak ++;
//	}
//	else
//	{
//		if(FltCnt_OverCurr_Ipeak > 0)
//		{
//			FltCnt_OverCurr_Ipeak --;
//		}
//	}
//	if(FltCnt_OverCurr_Ipeak >= 10)
//	{
//		FltCnt_OverCurr_Ipeak = 10;
//		if(MPR_FaultCode == 0)
//		{
//			FunFaultReact();
//
//			if(MPR_FaultCode == 0)
//			{
//				FunFaultReact();
//				if(mpr_ipeak == fabs(mpr_ia))
//				{
//					MPR_FaultCode = 177;
//					FaultWord_Map(177);
//				}
//				if(mpr_ipeak == fabs(mpr_ib))
//				{
//					MPR_FaultCode = 178;
//					FaultWord_Map(178);
//				}
//				if(mpr_ipeak == fabs(mpr_ic))
//				{
//					MPR_FaultCode = 179;
//					FaultWord_Map(179);
//				}
//			}
//		}
//	}
//	// DC Volg Over
//	if(mpr_udc > FltThr_OverVolt_Udc)
//	{
//		if(FltCnt_OverVolt_Udc < 150)
//		{
//			FltCnt_OverVolt_Udc ++;
//			if(mpr_udc > FltThr_OverVoltEst_Udc)
//			{
//				FltCnt_OverVolt_Udc += 80;
//			}
//		}
//		else
//		{
//			if(FaultMask_Map(180) == 1)
//			{
//				if(!MPR_FaultCode)
//				{
//					FunFaultReact();
//					MPR_FaultCode = 180;
//				}
//				FaultWord_Map(180);
//			}
//		}
//	}
//	else
//	{
//		if(FltCnt_OverVolt_Udc > 0)
//		{
//			FltCnt_OverVolt_Udc --;
//		}
//	}
}
/******************************************************************/
void GetPQVIRms(void)
{
	Uab_Rmsf = Uab_Rmsf*0.9 + Uab_Rms*0.1 *1.09487;
	Ubc_Rmsf = Ubc_Rmsf*0.9 + Ubc_Rms*0.1 *1.09487;
	Uca_Rmsf = Uca_Rmsf*0.9 + Uca_Rms*0.1 *1.09487;
	Ua_Rmsf  = Ua_Rmsf*0.9  + Ua_Rms*0.1 *1.09487;
	Ub_Rmsf  = Ub_Rmsf*0.9  + Ub_Rms*0.1 *1.09487;
	Uc_Rmsf  = Uc_Rmsf*0.9  + Uc_Rms*0.1 *1.09487;
	Ia_Rmsf = Ia_Rmsf*0.9 + Ia_Rms*0.1 *1.09487;
	Ib_Rmsf = Ib_Rmsf*0.9 + Ib_Rms*0.1 *1.09487;
	Ic_Rmsf = Ic_Rmsf*0.9 + Ic_Rms*0.1 *1.09487;

	Ioa_Rmsf = Ioa_Rmsf*0.9 + Ioa_Rms*0.1 *1.09487;
	Iob_Rmsf = Iob_Rmsf*0.9 + Iob_Rms*0.1 *1.09487;
	Ioc_Rmsf = Ioc_Rmsf*0.9 + Ioc_Rms*0.1 *1.09487;
	UabBus1_Rmsf = UabBus1_Rmsf*0.9 + UabBus1_Rms*0.1 *1.09487;
	UbcBus1_Rmsf = UbcBus1_Rmsf*0.9 + UbcBus1_Rms*0.1 *1.09487;
	UcaBus1_Rmsf = UcaBus1_Rmsf*0.9 + UcaBus1_Rms*0.1 *1.09487;
	UabBus2_Rmsf = UabBus2_Rmsf*0.9 + UabBus2_Rms*0.1 *1.09487;
	UbcBus2_Rmsf = UbcBus2_Rmsf*0.9 + UbcBus2_Rms*0.1 *1.09487;
	UcaBus2_Rmsf = UcaBus2_Rmsf*0.9 + UcaBus2_Rms*0.1 *1.09487;
	///////////////////////////
	vLine_avg = (Uab_Rmsf + Ubc_Rmsf + Uca_Rmsf)*0.333333;

	vLine_min = Uab_Rmsf;
	if(vLine_min > Ubc_Rmsf)
		vLine_min = Ubc_Rmsf;
	if(vLine_min > Uca_Rmsf)
		vLine_min = Uca_Rmsf;

	vLine_max = Uab_Rmsf;
	if(vLine_max < Ubc_Rmsf)
		vLine_max = Ubc_Rmsf;
	if(vLine_max < Uca_Rmsf)
		vLine_max = Uca_Rmsf;

	if(MainStusWord.DataBit.MprRun)
	{
		MotorVolt_Imbalance = (vLine_max - vLine_min) / vLine_avg;
	}
	else
	{
		MotorVolt_Imbalance = 0;
	}
	///////////////////////////////////////////////
	Iphase_avg = (Ia_Rmsf + Ib_Rmsf + Ic_Rmsf)*0.333333;

	Iphase_min = Ia_Rmsf;
	if(Iphase_min > Ib_Rmsf)
		Iphase_min = Ib_Rmsf;
	if(Iphase_min > Ic_Rmsf)
		Iphase_min = Ic_Rmsf;

	Iphase_max = Ia_Rmsf;
	if(Iphase_max < Ib_Rmsf)
		Iphase_max = Ib_Rmsf;
	if(Iphase_max < Ic_Rmsf)
		Iphase_max = Ic_Rmsf;

	if(MainStusWord.DataBit.MprRun)
	{
		MotorCurr_Imbalance = (Iphase_max - Iphase_min) / Iphase_avg;
	}
	else
	{
		MotorCurr_Imbalance = 0;
	}
	////////////////////////////////////////////////////////////
//	MPR_Ps = (mpr_udf*mpr_idf + mpr_uqf*mpr_iqf) * 0.001 *0.85;
//	MPR_Qs = (mpr_uqf*mpr_idf - mpr_udf*mpr_iqf) * 0.001 *0.85;
	if(MainStusWord.DataBit.MprRun == 1)
	{
		MPR_PsACK = MPR_PsACK*0.999 + MPR_Ps*0.001*0.96;
		MPR_QsACK = MPR_QsACK*0.999 + MPR_Qs*0.001*0.78;
		MPR_SsACK = MPR_SsACK*0.999 + MPR_Ss*0.001*0.97;

		if(MPR_SsACK != 0)
		{
			Power_Factor = fabs(MPR_PsACK/MPR_SsACK);
		}
		else
		{
			Power_Factor = 0;
		}
	}
	else
	{
		MPR_PsACK = 0;
		MPR_QsACK = 0;
		MPR_SsACK = 0;
		Power_Factor = 0;
	}
}
/******************************************************************/
interrupt void EPWM1_TZINT_isr(void)
{
	u16 i;
	IO_PWM_LOCK;
	EINT;
	FunFaultReact();
	IO_Scan();

	if(!MPR_FaultCode)
	{
		for(i=0; i<9; i++)
		{
			if( ((IOInputFpga2.DataInt>>i) & 0x0001) != 0x0001 )
			{
				if(FaultMask_Map(145 + i) == 1)
				{
					if(MPR_FaultCode == 0)
					{
						FunFaultReact();
						MPR_FaultCode = 145+i;
					}
				}
				FaultWord_Map(145+i);
			}
		}
		for(i=10; i<13; i++)
		{
			if( ((IOInputFpga2.DataInt>>i) & 0x0001) != 0x0001 )
			{
				if(FaultMask_Map(145 + i) == 1)
				{
					if(MPR_FaultCode == 0)
					{
						FunFaultReact();
						MPR_FaultCode = 145+i;
					}
				}
				FaultWord_Map(145+i);
			}
		}
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

//void Comm_heartbeatToggle(struct Heartbeat_Check_Type *p, int togglebit)
//{
//	if(togglebit == 1)
//	{
//		p->ToggLow_Cnt = 0;
//		p->ToggHigh_Cnt ++;
//	}
//	else
//	{
//		p->ToggHigh_Cnt = 0;
//		p->ToggLow_Cnt ++;
//	}
//
//	if( ( p->ToggHigh_Cnt >= 10) || ( p->ToggLow_Cnt >= 10) )
//	{
//		if(p->ToggHigh_Cnt >= 10)
//		{
//			p->ToggHigh_Cnt = 10;
//			p->ToggLow_Cnt = 0;
//		}
//		else
//		{
//			p->ToggLow_Cnt = 10;
//			p->ToggHigh_Cnt = 0;
//		}
//
//		p->Comm_result = 0;
//	}
//	else
//	{
//		p->Comm_result = 1;
//	}
//}

void Comm_heartbeatToggle(struct Heartbeat_Check_Type *p, int togglebit)
{
    if (togglebit == 1)
    {
        p->ToggLow_Cnt = 0;
        if (p->ToggHigh_Cnt < 10)
        	p->ToggHigh_Cnt ++;
    }
    else
    {
        p->ToggHigh_Cnt = 0;
        if (p->ToggLow_Cnt < 10)
        	p->ToggLow_Cnt ++;
    }

    p->Comm_result = (p->ToggHigh_Cnt < 10) && (p->ToggLow_Cnt < 10);
}
/******************************************************************/
interrupt void XINT1_FPGA_isr(void)
{
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;

	u16 i;
	s32 lEmifH, lEmifL;
	s16 iEmifH, iEmifL;
	f32 ftmp;

	/////////////////////////////////////////////////////////
	Mas1ToSlaWord.all = FPGARD_MASTOSLA1;
	Mas2ToSlaWord.all = FPGARD_MASTOSLA2;
	/////////////////////////////////////////////////////////
	Comm_heartbeatToggle(&HeartbeatCheck1, Mas1ToSlaWord.bit.Toggle);
	Comm_heartbeatToggle(&HeartbeatCheck2, Mas2ToSlaWord.bit.Toggle);
	/////////////////////////////////////////////////////////
	if(MainStusWord.DataBit.SingPara == 1)
	{
		if( (HeartbeatCheck1.Comm_result == 0) && (HeartbeatCheck2.Comm_result == 0) )
		{
			if(MPR_FaultCode == 0)
			{
				FunFaultReact();
				MPR_FaultCode = 240;
			}
			FaultWord_Map(240);
		}
		/////////////////////////////////////////////////////////
		if(HeartbeatCheck1.Comm_result == 0)
		{
			Mas1ToSlaWord.bit.Identity = 0;
		}

		if(HeartbeatCheck2.Comm_result == 0)
		{
			Mas2ToSlaWord.bit.Identity = 0;
		}

		if( (Mas1ToSlaWord.bit.Identity == 1) && (Mas2ToSlaWord.bit.Identity == 0) )
		{
			MainStusWord.DataBit.MasSel = 0;
			FltCnt_OverTime_MasShift = 0;
		}
		else if( (Mas1ToSlaWord.bit.Identity == 0) && (Mas2ToSlaWord.bit.Identity == 1) )
		{
			MainStusWord.DataBit.MasSel = 1;
			FltCnt_OverTime_MasShift = 0;
		}
		else
		{
			if(FltCnt_OverTime_MasShift < 100)
			{
				FltCnt_OverTime_MasShift ++;
			}
			else
			{
				if(MPR_FaultCode == 0)
				{
					FunFaultReact();
					MPR_FaultCode = 241;
				}
				FaultWord_Map(241);
			}
		}
	}
	/////////////////////////////////////////////////////////
	for(i=0; i<4; i++)
	{
		lEmifH = *(unsigned  int *)(0x4090 + 2*i);
		lEmifL = *(unsigned  int *)(0x4091 + 2*i);
		lIsrBuf[i] = ( (lEmifH<<16)&0xFFFF0000) | (lEmifL&0x0000FFFF);
		fIsrBuf[i] = (f32)(lIsrBuf[i]) / 16777216.0;
	}

	mpr_ud		=  fIsrBuf[0]*2048;
	mpr_uq		=  fIsrBuf[1]*2048;
	mpr_udf		=  fIsrBuf[2]*2048;
	mpr_uqf		=  fIsrBuf[3]*2048;
	/////////////////////////////////////////////////////////
	//DC filter
	mpr_udcf = mpr_udcf*0.8 + mpr_udc*0.2;
	/////////////////////////////////////////////////////////
	// Curr Protection
//	mpr_isum = mpr_ia + mpr_ib + mpr_ic;
//	mpr_ipeak = fabs(mpr_ia);
//	if(mpr_ipeak < fabs(mpr_ib))  mpr_ipeak = fabs(mpr_ib);
//	if(mpr_ipeak < fabs(mpr_ic))  mpr_ipeak = fabs(mpr_ic);
//	mpr_pwm_protection();
	////////////////////////////////////////////////////
	Udq_FastChar();
	UdRef_All = UsdRef + FastChar_UdCompon;
	UqRef_All = UsqRef + FastChar_UqCompon;

	if(MainStusWord.DataBit.MprRun == 1)
	{
		// Volg Loop Ctrl
		ftmp = UdRef_All - mpr_udf;
		Ud_Loop_Out = FunPIComCtrl(Udq_Loop_Kp, Udq_Loop_Ki, ftmp, &Errtotal_Ud, Udq_Loop_Lim);

		ftmp = UqRef_All - mpr_uqf;
		Uq_Loop_Out = FunPIComCtrl(Udq_Loop_Kp, Udq_Loop_Ki, ftmp, &Errtotal_Uq, Udq_Loop_Lim);
		////////////////////////////////////////////////////////////
	}
	else
	{
		Errtotal_Ud = Errtotal_Ud*0.9;
		if(Errtotal_Ud>0)	Errtotal_Ud -= 0.01;
		if(Errtotal_Ud<0)	Errtotal_Ud += 0.01;

		Ud_Loop_Out = Ud_Loop_Out*0.9;
		if(Ud_Loop_Out>0)	Ud_Loop_Out -= 0.01;
		if(Ud_Loop_Out<0)	Ud_Loop_Out += 0.01;

		Errtotal_Uq = Errtotal_Uq*0.9;
		if(Errtotal_Uq>0)	Errtotal_Uq -= 0.01;
		if(Errtotal_Uq<0)	Errtotal_Uq += 0.01;

		Uq_Loop_Out = Uq_Loop_Out*0.9;
		if(Uq_Loop_Out>0)	Uq_Loop_Out -= 0.01;
		if(Uq_Loop_Out<0)	Uq_Loop_Out += 0.01;
	}

	IgdRef = Ud_Loop_Out;
	IgqRef = Uq_Loop_Out;
	// WL= 2PIf*L = 0.195
	Ud_FeedFwd = 0; //mpr_udf - 0.195*mpr_iqf;
	Uq_FeedFwd = 0; //mpr_uqf + 0.195*mpr_idf;
	/////////////////////////////////////////////////////////////////////////////
	fIsrBuf[0] = IgdRef/2048.0;
	fIsrBuf[1] = IgqRef/2048.0;
	fIsrBuf[2] = Ud_FeedFwd/2048.0;
	fIsrBuf[3] = Uq_FeedFwd/2048.0;

	for(i=0; i<4; i++)
	{
		lIsrBuf[i] = (s32)(fIsrBuf[i]*16777216.0);
		iEmifH =  (s16)((lIsrBuf[i]>>16) & 0x0000FFFF);
		iEmifL =  (s16)((lIsrBuf[i]>> 0) & 0x0000FFFF);
		*(unsigned  int *)(0x4190 + 2*i) = iEmifH;
		*(unsigned  int *)(0x4191 + 2*i) = iEmifL;
	}

	SlaToMas1Word.bit.Toggle ^= 1;
	SlaToMas2Word.bit.Toggle ^= 1;
	FPGAWR_SLATOMAS1 = SlaToMas1Word.all;
	FPGAWR_SLATOMAS2 = SlaToMas2Word.all;
	/////////////////////////////////////////////////////////////////////////////
}
/****************************************************************************/
interrupt void CANARecv_isr(void)
{
	EINT;
	if(ECanaRegs.CANGIF0.bit.GMIF0 == 1)
	{
		ECAN_CommFlag |= 0x0001;
		switch(ECanaRegs.CANGIF0.bit.MIV0)
		{
			case 4:{CANAMB4HardwareRecv(CANAMB4RecvBuffer,&CANAMB4RecvTeal,4);break;}
			case 5:{CANAMB5HardwareRecv(CANAMB5RecvBuffer,&CANAMB5RecvTeal,4);break;}
			case 6:{CANAMB6HardwareRecv(CANAMB6RecvBuffer,&CANAMB6RecvTeal,4);break;}
			case 7:{CANAMB7HardwareRecv(CANAMB7RecvBuffer,&CANAMB7RecvTeal,4);break;}
			case 8:{CANAMB8HardwareRecv(CANAMB8RecvBuffer,&CANAMB8RecvTeal,4);break;}
		}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
/****************************************************************************/
interrupt void CANBRecv_isr(void)  //canopen
{
	if(ECanbRegs.CANGIF0.bit.GMIF0 == 1)
	{
		switch(ECanbRegs.CANGIF0.bit.MIV0)
		{
			case 4:
			{
				CANBMB4HardwareRecv(CANBMB4RecvBuffer,&CANBMB4RecvTeal);
				break;
			}
			case 5:
			{
				CANBMB5HardwareRecv(CANBMB5RecvBuffer,&CANBMB5RecvTeal);
				break;
			}
			default:break;
		}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	EINT;
}
/****************************************************************************/
interrupt void Timer2INT_ISR(void)
{
//	SysCnt_1msBase ++;
//	if(SysCnt_1msBase >= 0)
	{
//		SysCnt_1msBase = 0;
		SCIBSend_1ms ++;
		if(SCIBSend_1ms >= 10000)
			SCIBSend_1ms = 0;

		CANSend_1ms ++;
		if(CANSend_1ms >= 10000)
			CANSend_1ms = 0;

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

		Test_Time1ms++;
		if(Test_Time1ms > 60000)
			Test_Time1ms = 0;

		SysCnt_1sBase ++;
		if(SysCnt_1sBase > 2000)
		{
			SysCnt_1sBase = 0;

			SysCnt_1s ++;
			if(SysCnt_1s > 30000)
			{
				SysCnt_1s = 0;
			}
		}
	}
}
/****************************************************************************/
/****************************************************************************/
