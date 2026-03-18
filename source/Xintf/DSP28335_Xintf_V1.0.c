#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "main\Variable.h"            
#include "Custom_Xintf_ADC&IO.H"
#include "main\Custom_GlobalMacro.H"
#include "GPIO\Custom_IOMacro.H"
#include "IQmathLib.h"

int ADValue[16]={0};

/*******************************************************************/
#define KGVAD    0.0318717756//0.03305970206 //0.06900261293
#define KDCAD    0.06636892541
#define KGIAD    0.09304423621
#define KFIAD    0.00762962736//(100A/4V) 0.00381481368(50A/4V)
#define KREAL    0.00030518509

extern void FunFaultReact(void);
/*******************************************************************************/
void InitXintf(void)
{
	// All Zones---------------------------------
	// Timing for all zones based on XTIMCLK = 1/2 SYSCLKOUT
	EALLOW;
	XintfRegs.XINTCNF2.bit.XTIMCLK = 1;// XTIMCLK = 1/2 SYSCLKOUT
	// No write buffering
	XintfRegs.XINTCNF2.bit.WRBUFF = 3;

	// XCLKOUT is enabled
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;
	// XCLKOUT = XTIMCLK/2
	XintfRegs.XINTCNF2.bit.CLKMODE = 1;


	// Zone 0------------------------------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone write timing
	XintfRegs.XTIMING0.bit.XWRLEAD = 3;
	XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
	XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
	// Zone read timing
	XintfRegs.XTIMING0.bit.XRDLEAD = 3;
	XintfRegs.XTIMING0.bit.XRDACTIVE = 7;
	XintfRegs.XTIMING0.bit.XRDTRAIL = 3;
	// double all Zone read/write lead/active/trail timing
	XintfRegs.XTIMING0.bit.X2TIMING = 1;
	// Zone will not sample XREADY signal
	XintfRegs.XTIMING0.bit.USEREADY = 0;//baijj9-15
	XintfRegs.XTIMING0.bit.READYMODE = 1;  // sample asynchronous--ignord
	// Size must be either:
	// 0,1 = x32 or
	// 1,1 = x16 other values are reserved
	XintfRegs.XTIMING0.bit.XSIZE = 3;

	// Zone 6------------------------------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone write timing
	XintfRegs.XTIMING6.bit.XWRLEAD = 3;
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;
	// Zone read timing
	XintfRegs.XTIMING6.bit.XRDLEAD = 3;
	XintfRegs.XTIMING6.bit.XRDACTIVE = 7;
	XintfRegs.XTIMING6.bit.XRDTRAIL = 3;
	// double all Zone read/write lead/active/trail timing
	XintfRegs.XTIMING6.bit.X2TIMING = 1;
	// Zone will sample XREADY signal
	XintfRegs.XTIMING6.bit.USEREADY = 0;//baijj9-15
	XintfRegs.XTIMING6.bit.READYMODE = 1;  // sample asynchronous
	// Size must be either:
	// 0,1 = x32 or
	// 1,1 = x16 other values are reserved
	XintfRegs.XTIMING6.bit.XSIZE = 3;
	// Zone 7------------------------------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone write timing
	XintfRegs.XTIMING7.bit.XWRLEAD = 1;
	XintfRegs.XTIMING7.bit.XWRACTIVE = 3;
	XintfRegs.XTIMING7.bit.XWRTRAIL = 1;
	// Zone read timing
	XintfRegs.XTIMING7.bit.XRDLEAD = 1;
	XintfRegs.XTIMING7.bit.XRDACTIVE = 3;
	XintfRegs.XTIMING7.bit.XRDTRAIL = 1;
	// not double all Zone read/write lead/active/trail timing
	XintfRegs.XTIMING7.bit.X2TIMING = 0;
	// Zone will sample XREADY signal
	XintfRegs.XTIMING7.bit.USEREADY = 0;//baijj9-15
	XintfRegs.XTIMING7.bit.READYMODE = 1;  // sample asynchronous
	// Size must be either:
	// 0,1 = x32 or
	// 1,1 = x16 other values are reserved
	XintfRegs.XTIMING7.bit.XSIZE = 3;
	// Bank switching
	// Assume Zone 7 is slow, so add additional BCYC cycles
	// when ever switching from Zone 7 to another Zone.
	// This will help avoid bus contention.
	XintfRegs.XBANK.bit.BANK = 7;
	XintfRegs.XBANK.bit.BCYC = 7;
	EDIS;
	//Force a pipeline flush to ensure that the write to
	//the last register configured occurs before returning.

	//InitXintf16Gpio();
	// InitXintf32Gpio();

	asm(" RPT #7 || NOP");

}
/*****************************************************/
signed int ad_process(signed int a)
{
	signed int b;
	b = a&0x8000;
	if(b == 0x8000)
		a = a-0x10000;
	return(a);
}
/**********************************************************/
void Palarell_Extern_ADCScan(void)
{
	SINT i,temp;
	SINT IaPar_ad, IbPar_ad, IcPar_ad;
	SINT UabSl_ad, UbcSl_ad;
	SINT UabPar_ad, UbcPar_ad;
	SINT UaOut_ad, UcOut_ad, UbOut_ad;
	SINT IaOvh_ad, IbOvh_ad, IcOvh_ad;
	SINT uflowad;

	for(i=0; i<5; i++);

/*J3 (3,4)*/IaPar_ad = AD1;
/*J4 (3,4)*/IbPar_ad = AD1;
/*J5 (3,4)*/IcPar_ad = AD1;
/*J9 (3,4)*/uflowad = AD1;
/*J10(3,4)*/UabSl_ad = AD1;
/*J11(3,4)*/UbcSl_ad = AD1;
/*J12(3,4)*/UaOut_ad = AD1;
/*J6 (3,4)*/UcOut_ad = AD1;

/*J7 (3,4)*/IaOvh_ad = AD2;
/*J8 (3,4)*/IbOvh_ad = AD2;
/*J23(1,2)*/IcOvh_ad = AD2;
/*J23(3,4)*/temp 	 = AD2;
/*J1 (1,2)*/temp 	 = AD2;
/*J1 (3,4)*/UbOut_ad = AD2;
/*J24(1,2)*/UabPar_ad = AD2;
/*J24(3,4)*/UbcPar_ad = AD2;

	(void)temp;

	for(i=0; i<5; i++);

	IaPar = IaPar_ad * KGIAD;
	IbPar = IbPar_ad * KGIAD;
	IcPar = IcPar_ad * KGIAD;

	IaOvh = IaOvh_ad * KGIAD;
	IbOvh = IbOvh_ad * KGIAD;
	IcOvh = IcOvh_ad * KGIAD;

	UabSl = (UabSl_ad * KGVAD);
	UbcSl = (UbcSl_ad * KGVAD);
	UcaSl = -UabSl -UbcSl;

	UabPar = (UabPar_ad * KGVAD);
	UbcPar = (UbcPar_ad * KGVAD);
	UcaPar = -UabPar -UbcPar;

	UaOut = UaOut_ad * KGVAD;
	UbOut = UbOut_ad * KGVAD;
	UcOut = UcOut_ad * KGVAD;
//J9 water
	uFlowRate = -(uflowad * KREAL);
}
/**********************************************************************/
void IOInMapLogic(void)
{
	IOInput1.DataBit.Bit0 = (((!IOInputEn.DataBit.Bit0) & IOInputDisValue.DataBit.Bit0)
							   |(IOInputEn.DataBit.Bit0 & ((IOInputFpga1.DataBit.Bit0^IOInputLogic.DataBit.Bit0)^1)));
	IOInput1.DataBit.Bit1 = (((!IOInputEn.DataBit.Bit1) & IOInputDisValue.DataBit.Bit1)
							   |(IOInputEn.DataBit.Bit1 & ((IOInputFpga1.DataBit.Bit1^IOInputLogic.DataBit.Bit1)^1)));
	IOInput1.DataBit.Bit2 = (((!IOInputEn.DataBit.Bit2) & IOInputDisValue.DataBit.Bit2)
							   |(IOInputEn.DataBit.Bit2 & ((IOInputFpga1.DataBit.Bit2^IOInputLogic.DataBit.Bit2)^1)));
	IOInput1.DataBit.Bit3 = (((!IOInputEn.DataBit.Bit3) & IOInputDisValue.DataBit.Bit3)
							   |(IOInputEn.DataBit.Bit3 & ((IOInputFpga1.DataBit.Bit3^IOInputLogic.DataBit.Bit3)^1)));
	IOInput1.DataBit.Bit4 = (((!IOInputEn.DataBit.Bit4) & IOInputDisValue.DataBit.Bit4)
							   |(IOInputEn.DataBit.Bit4 & ((IOInputFpga1.DataBit.Bit4^IOInputLogic.DataBit.Bit4)^1)));
	IOInput1.DataBit.Bit5 = (((!IOInputEn.DataBit.Bit5) & IOInputDisValue.DataBit.Bit5)
							   |(IOInputEn.DataBit.Bit5 & ((IOInputFpga1.DataBit.Bit5^IOInputLogic.DataBit.Bit5)^1)));
	IOInput1.DataBit.Bit6 = (((!IOInputEn.DataBit.Bit6) & IOInputDisValue.DataBit.Bit6)
							   |(IOInputEn.DataBit.Bit6 & ((IOInputFpga1.DataBit.Bit6^IOInputLogic.DataBit.Bit6)^1)));
	IOInput1.DataBit.Bit7 = (((!IOInputEn.DataBit.Bit7) & IOInputDisValue.DataBit.Bit7)
							   |(IOInputEn.DataBit.Bit7 & ((IOInputFpga1.DataBit.Bit7^IOInputLogic.DataBit.Bit7)^1)));
	IOInput1.DataBit.Bit8 = (((!IOInputEn.DataBit.Bit8) & IOInputDisValue.DataBit.Bit8)
							   |(IOInputEn.DataBit.Bit8 & ((IOInputFpga1.DataBit.Bit8^IOInputLogic.DataBit.Bit8)^1)));
	IOInput1.DataBit.Bit9 = (((!IOInputEn.DataBit.Bit9) & IOInputDisValue.DataBit.Bit9)
							   |(IOInputEn.DataBit.Bit9 & ((IOInputFpga1.DataBit.Bit9^IOInputLogic.DataBit.Bit9)^1)));
	IOInput1.DataBit.Bit10 = (((!IOInputEn.DataBit.Bit10) & IOInputDisValue.DataBit.Bit10)
							   |(IOInputEn.DataBit.Bit10 & ((IOInputFpga1.DataBit.Bit10^IOInputLogic.DataBit.Bit10)^1)));
	IOInput1.DataBit.Bit11 = (((!IOInputEn.DataBit.Bit11) & IOInputDisValue.DataBit.Bit11)
							   |(IOInputEn.DataBit.Bit11 & ((IOInputFpga1.DataBit.Bit11^IOInputLogic.DataBit.Bit11)^1)));

}

void IOInputFaultCtrl(void)
{
	int i;
	CtrlCnt_IOFltDelay ++;
	if(CtrlCnt_IOFltDelay > 1000)
	{
		CtrlCnt_IOFltDelay = 1000;

		for(i=0; i<1; i++)
		{
			if( ( (IOInput1.DataInt>>i) & 0x0001) == 0x0001 )
			if(FaultMask_Map(i+2) == 0x0001)
			{
				if(!NPR_FaultCode)
				{
					FunFaultReact();
					NPR_FaultCode = i+2;
				}
				FaultWord_Map(i+2);
			}
		}


		for(i=5; i<8; i++)
		{
			if(((IOInput1.DataInt >> i)&0x0001) == 0x0001)
			if(FaultMask_Map(i+2) == 0x0001)
			{
				if(!NPR_FaultCode)
				{
					FunFaultReact();
					NPR_FaultCode = i+2;
				}
				FaultWord_Map(i+2);
			}
		}

		for(i=10; i<11; i++)
		{
			if(((IOInput1.DataInt >> i)&0x0001) == 0x0001)
			if(FaultMask_Map(i+2) == 0x0001)
			{
				if(!NPR_FaultCode)
				{
					FunFaultReact();
					NPR_FaultCode = i+2;
				}
				FaultWord_Map(i+2);
			}
		}
	}

	if(IO_RunRef_Feed == 1)
	{
		CtrlCnt_IORunRefFeed ++;
		if(CtrlCnt_IORunRefFeed > 20)
		{
			CtrlCnt_IORunRefFeed = 20;
			Flag_IORunRef = 1;
		}
	}
	else
	{
		CtrlCnt_IORunRefFeed --;
		if(CtrlCnt_IORunRefFeed < -20)
		{
			CtrlCnt_IORunRefFeed = -20;
			Flag_IORunRef = 0;
		}
	}

	if(IO_RstRef_Feed == 1)
	{
		CtrlCnt_IORstRefFeed ++;
		if(CtrlCnt_IORstRefFeed > 50)
		{
			CtrlCnt_IORstRefFeed = 50;
			Flag_IORstRef = 1;
		}
	}
	else
	{
		CtrlCnt_IORstRefFeed --;
		if(CtrlCnt_IORstRefFeed < 1)
		{
			CtrlCnt_IORstRefFeed = 1;
			Flag_IORstRef = 0;
		}
	}
}
/**********************************************************************************/
void IOOutMapLogic(void)
{
	IOOutFpga1.DataBit.Bit0 = (IOOutput1.DataBit.Bit0 & IOOutCtrl1Word.DataBit.Bit0)^0;
	IOOutFpga1.DataBit.Bit1 = (IOOutput1.DataBit.Bit1 & IOOutCtrl1Word.DataBit.Bit1)^0;
	IOOutFpga1.DataBit.Bit2 = (IOOutput1.DataBit.Bit2 & IOOutCtrl1Word.DataBit.Bit2)^0;
	IOOutFpga1.DataBit.Bit3 = (IOOutput1.DataBit.Bit3 & IOOutCtrl1Word.DataBit.Bit3)^0;
	IOOutFpga1.DataBit.Bit4 = (IOOutput1.DataBit.Bit4 & IOOutCtrl1Word.DataBit.Bit4)^0;
	IOOutFpga1.DataBit.Bit5 = (IOOutput1.DataBit.Bit5 & IOOutCtrl1Word.DataBit.Bit5)^0;
	IOOutFpga1.DataBit.Bit6 = (IOOutput1.DataBit.Bit6 & IOOutCtrl1Word.DataBit.Bit6)^0;
	IOOutFpga1.DataBit.Bit7 = (IOOutput1.DataBit.Bit7 & IOOutCtrl1Word.DataBit.Bit7)^0;
	IOOutFpga1.DataBit.Bit8 = (IOOutput1.DataBit.Bit8 & IOOutCtrl1Word.DataBit.Bit8)^0;
	IOOutFpga1.DataBit.Bit9 = (IOOutput1.DataBit.Bit9 & IOOutCtrl1Word.DataBit.Bit9)^0;
}
/***********************************************************************/
void IO_Scan(void)
{
	int i;
	//read IO status input
	for(i=0; i<5; i++);
	IOInputFpga1.DataInt = FPGARDIOSTS1;
	for(i=0; i<5; i++);
	IOInputFpga2.DataInt = FPGARDIOSTS2;
	IOInputFpga3.DataInt = FPGARDIOSTS3;
	IOInMapLogic();
	IOInputFaultCtrl();
	//write IO output
	IOOutMapLogic();

	if (IOOutFpga1Old.DataInt != IOOutFpga1.DataInt)
	{
		for(i=0; i<5; i++);
		IOOutFpga1Old.DataInt = IOOutFpga1.DataInt;
		FPGAWTIOSTS1 = IOOutFpga1.DataInt;
		for(i=0; i<5; i++);
	}

	if (IOOutFpga2Old.DataInt != IOOutFpga2.DataInt)
	{
		for(i=0; i<5; i++);
		IOOutFpga2Old.DataInt = IOOutFpga2.DataInt;
		FPGAWTIOSTS2 = IOOutFpga2Old.DataInt;
		for(i=0; i<5; i++);
	}
}
/***********************************************************/
int CPLDSWVerRead(void)
{
	int temp, i;
	for(i=0; i<5; i++);
	temp = FPGARDSWVER1;
	for(i=0; i<5; i++);
	return temp;
}
/********************************************************************************/

