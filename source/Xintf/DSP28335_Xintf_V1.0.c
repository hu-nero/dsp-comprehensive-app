#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "main\Variable.h"            
#include "Custom_Xintf_ADC&IO.H"
#include "main\Custom_GlobalMacro.H"
#include "GPIO\Custom_IOMacro.H"
#include "IQmathLib.h"

int ADValue[16]={0};

/*******************************************************************/
#define KGVAD    0.06900261293
#define KDCAD    0.06636892541
#define KGIAD    0.07629627369
#define KFIAD    0.01525925474
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

}
/**********************************************************************************/
void IO_Scan(void)
{
	int i;
	///////////////////////////////////////////////////////////////////////
	for(i=0; i<5; i++);
	IOInputFpga1.DataInt = FPGARD_IOINPUT;
	IOInputFpga1.DataBit.Bit0 = GpioDataRegs.GPADAT.bit.GPIO0;
	IOInputFpga1.DataBit.Bit1 = GpioDataRegs.GPADAT.bit.GPIO1;
	IOInputFpga1.DataBit.Bit2 = GpioDataRegs.GPADAT.bit.GPIO2;
	IOInputFpga1.DataBit.Bit3 = GpioDataRegs.GPADAT.bit.GPIO3;
	IOInputFpga1.DataBit.Bit4 = GpioDataRegs.GPADAT.bit.GPIO4;
	IOInputFpga1.DataBit.Bit5 = GpioDataRegs.GPADAT.bit.GPIO5;
	IOInputFpga1.DataBit.Bit6 = GpioDataRegs.GPADAT.bit.GPIO7;
	IOInputFpga1.DataBit.Bit7 = GpioDataRegs.GPBDAT.bit.GPIO52;
	IOInputFpga1.DataBit.Bit8 = GpioDataRegs.GPBDAT.bit.GPIO60;
	IOInputFpga1.DataBit.Bit9 = GpioDataRegs.GPBDAT.bit.GPIO61;
	IOInputFpga1.DataBit.Bit10 = GpioDataRegs.GPADAT.bit.GPIO26;
	IOInputFpga1.DataBit.Bit11 = GpioDataRegs.GPBDAT.bit.GPIO59;
	///////////////////////////////////////////////////////////////////////
	IOInputFpga2.DataInt = FPGARD_HARDFLT;
	FpgaSofaFault.DataInt = FPGARD_SOFAFLT;
///////////////////////////////INPUT////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////////
//	for(i=0; i<1; i++)
//	{
//		if( ( (IOInput1.DataInt>>i) & 0x0001) == 0x0001 )
//		if(FaultMask_Map(130+i) == 0x0001)
//		{
//			if(!MPR_FaultCode)
//			{
//				FunFaultReact();
//				MPR_FaultCode = 130+i;
//			}
//			FaultWord_Map(130+i);
//		}
//	}
	//////////////////////////////////////////////////////////////////////////
	if( ( (IOInput1.DataInt>>11) & 0x0001) == 0x0001 )
	if(FaultMask_Map(129) == 0x0001)
	{
		if(!MPR_FaultCode)
		{
			FunFaultReact();
			MPR_FaultCode = 129;
		}
		FaultWord_Map(129);
	}
	/////////////////////////////////////////////////////////////////////////
	if(IO_RunRef_Feed == 1)
	{
		CtrlCnt_IORunRefFeed ++;
		if(CtrlCnt_IORunRefFeed > 15)
		{
			CtrlCnt_IORunRefFeed = 15;

			Flag_IORunRef = 1;
		}
	}
	else
	{
		if(CtrlCnt_IORunRefFeed < 0)
		{
			CtrlCnt_IORunRefFeed = 0;
		}
	}

	if(IO_StopRef_Feed == 1)
	{
		CtrlCnt_IORstRefFeed ++;
		if(CtrlCnt_IORstRefFeed > 15)
		{
			CtrlCnt_IORstRefFeed = 15;

			Flag_IORstRef = 1;
		}
	}
	else
	{
		if(CtrlCnt_IORstRefFeed < 0)
		{
			CtrlCnt_IORstRefFeed = 0;
		}
	}
/////////////////////////////OUTPUT////////////////////////////////////////
	if(IOOutput1.DataInt != IOOutput1Old.DataInt)
	{
		IOOutput1Old.DataInt = IOOutput1.DataInt;
		//
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

		FPGAWR_IOOUTPUT = IOOutFpga1.DataInt;
	}
}
/***********************************************************/
extern Uint16  FPGA_Ver[10] ;
int CPLDSWVerRead(void)
{
	int temp, i;
	for(i=0; i<5; i++);
	temp = FPGARD_SWVER1;
	DeviceNum = FPGARD_DEVICE;
	for(i=0; i<5; i++);
	FPGA_Ver[1] = ( 'V'<<0                        | '_'<<8);
	FPGA_Ver[2] = ( ('0' + (temp>>8)&0x00FF ) <<0 | '.'<<8);
	FPGA_Ver[3] = ( ('0' + (temp>>0)&0x00FF ) <<0 | '\0'<<8);
	return temp;
}
/********************************************************************************/
void MotorFreq_FpgaScan(void)
{
	u16 utmp=0;
	u16 utmpH, utmpL;
	u32 ultmp;

	utmpH = FPGARD_FREQH;
	utmpL = FPGARD_FREQL;
	utmp = FPGARD_FREQCNT;

	ultmp = (u32)utmpH*65536 + (u32)utmpL;

	if(ultmp)
	{
		FpgaMotorFreq = 100000.0 *utmp /ultmp;
	}
}

