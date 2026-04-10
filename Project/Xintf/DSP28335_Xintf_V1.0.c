#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "main\Variable.h"            
#include "Custom_Xintf_ADC&IO.H"
#include "main\Custom_GlobalMacro.H"
#include "GPIO\Custom_IOMacro.H"
#include "IQmathLib.h"
#include "stdbool.h"

int ADValue[16]={0};

/*******************************************************************/
#define KGVAD    0.0318717756//0.03305970206 //0.06900261293
#define KDCAD    0.06636892541
#define KGIAD    0.09304423621
#define KFIAD    0.00762962736//(100A/4V) 0.00381481368(50A/4V)
#define KREAL    0.00030518509//0.00030518509

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
    u16 utmp=0;
    u16 utmpH, utmpL;
    u32 ultmp;
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
/*J10(3,4)*/UaOut_ad = AD1;
/*J11(3,4)*/UbOut_ad= AD1;
/*J12(3,4)*/UcOut_ad = AD1;

/*J6 (3,4)*/IcOvh_ad = AD1;
/*J7 (3,4)*/IaOvh_ad = AD2;
/*J8 (3,4)*/IbOvh_ad = AD2;

/*J23(1,2)*/temp	 = AD2;
/*J23(3,4)*/temp 	 = AD2;
/*J1 (1,2)*/UabSl_ad = AD2;
/*J1 (3,4)*/UbcSl_ad = AD2;
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

	UabSl = (UabSl_ad * KGVAD); //single parallel
	UbcSl = (UbcSl_ad * KGVAD);
	UcaSl = -UabSl -UbcSl;

	UabPar = (UabPar_ad * KGVAD);
	UbcPar = (UbcPar_ad * KGVAD);
	UcaPar = -UabPar -UbcPar;

	UaOut = UaOut_ad * KGVAD;
	UbOut = UbOut_ad * KGVAD;
	UcOut = UcOut_ad * KGVAD;

	UabOut = UaOut - UbOut;
	UbcOut = UbOut - UcOut;
	UcaOut = UcOut - UaOut;
//J9 water
	uFlowRate = -(uflowad * KREAL);

    // frequency

    utmpL = FPGARDCLKCNTL;
    utmpH = FPGARDCLKCNTH;
    utmp = FPGARDFREQ;

    ultmp = (u32)utmpH*65536 + (u32)utmpL;

    if(ultmp)
    {
        FpgaMotorFreq = 100000.0 *utmp /ultmp;
    }
}
/**********************************************************************/
void IOInMapLogic(void)
{
	IOInput1.DataBit.Bit0 = (((!IOInput1En.DataBit.Bit0) & IOInput1DisValue.DataBit.Bit0)
	                           |(IOInput1En.DataBit.Bit0 & ((IOInput1Fpga.DataBit.Bit0^IOInput1Logic.DataBit.Bit0)^1)));
	IOInput1.DataBit.Bit1 = (((!IOInput1En.DataBit.Bit1) & IOInput1DisValue.DataBit.Bit1)
	                           |(IOInput1En.DataBit.Bit1 & ((IOInput1Fpga.DataBit.Bit1^IOInput1Logic.DataBit.Bit1)^1)));
	IOInput1.DataBit.Bit2 = (((!IOInput1En.DataBit.Bit2) & IOInput1DisValue.DataBit.Bit2)
	                           |(IOInput1En.DataBit.Bit2 & ((IOInput1Fpga.DataBit.Bit2^IOInput1Logic.DataBit.Bit2)^1)));
	IOInput1.DataBit.Bit3 = (((!IOInput1En.DataBit.Bit3) & IOInput1DisValue.DataBit.Bit3)
	                           |(IOInput1En.DataBit.Bit3 & ((IOInput1Fpga.DataBit.Bit3^IOInput1Logic.DataBit.Bit3)^1)));
	IOInput1.DataBit.Bit4 = (((!IOInput1En.DataBit.Bit4) & IOInput1DisValue.DataBit.Bit4)
	                           |(IOInput1En.DataBit.Bit4 & ((IOInput1Fpga.DataBit.Bit4^IOInput1Logic.DataBit.Bit4)^1)));
	IOInput1.DataBit.Bit5 = (((!IOInput1En.DataBit.Bit5) & IOInput1DisValue.DataBit.Bit5)
	                           |(IOInput1En.DataBit.Bit5 & ((IOInput1Fpga.DataBit.Bit5^IOInput1Logic.DataBit.Bit5)^1)));
	IOInput1.DataBit.Bit6 = (((!IOInput1En.DataBit.Bit6) & IOInput1DisValue.DataBit.Bit6)
	                           |(IOInput1En.DataBit.Bit6 & ((IOInput1Fpga.DataBit.Bit6^IOInput1Logic.DataBit.Bit6)^1)));
	IOInput1.DataBit.Bit7 = (((!IOInput1En.DataBit.Bit7) & IOInput1DisValue.DataBit.Bit7)
	                           |(IOInput1En.DataBit.Bit7 & ((IOInput1Fpga.DataBit.Bit7^IOInput1Logic.DataBit.Bit7)^1)));
	IOInput1.DataBit.Bit8 = (((!IOInput1En.DataBit.Bit8) & IOInput1DisValue.DataBit.Bit8)
	                           |(IOInput1En.DataBit.Bit8 & ((IOInput1Fpga.DataBit.Bit8^IOInput1Logic.DataBit.Bit8)^1)));
	IOInput1.DataBit.Bit9 = (((!IOInput1En.DataBit.Bit9) & IOInput1DisValue.DataBit.Bit9)
	                           |(IOInput1En.DataBit.Bit9 & ((IOInput1Fpga.DataBit.Bit9^IOInput1Logic.DataBit.Bit9)^1)));
	IOInput1.DataBit.Bit10 = (((!IOInput1En.DataBit.Bit10) & IOInput1DisValue.DataBit.Bit10)
	                           |(IOInput1En.DataBit.Bit10 & ((IOInput1Fpga.DataBit.Bit10^IOInput1Logic.DataBit.Bit10)^1)));
	IOInput1.DataBit.Bit11 = (((!IOInput1En.DataBit.Bit11) & IOInput1DisValue.DataBit.Bit11)
	                           |(IOInput1En.DataBit.Bit11 & ((IOInput1Fpga.DataBit.Bit11^IOInput1Logic.DataBit.Bit11)^1)));
	IOInput1.DataBit.Bit12 = (((!IOInput1En.DataBit.Bit12) & IOInput1DisValue.DataBit.Bit12)
	                           |(IOInput1En.DataBit.Bit12 & ((IOInput1Fpga.DataBit.Bit12^IOInput1Logic.DataBit.Bit12)^1)));
	IOInput1.DataBit.Bit13 = (((!IOInput1En.DataBit.Bit13) & IOInput1DisValue.DataBit.Bit13)
	                           |(IOInput1En.DataBit.Bit13 & ((IOInput1Fpga.DataBit.Bit13^IOInput1Logic.DataBit.Bit13)^1)));
	IOInput1.DataBit.Bit14 = (((!IOInput1En.DataBit.Bit14) & IOInput1DisValue.DataBit.Bit14)
	                           |(IOInput1En.DataBit.Bit14 & ((IOInput1Fpga.DataBit.Bit14^IOInput1Logic.DataBit.Bit14)^1)));
	IOInput1.DataBit.Bit15 = (((!IOInput1En.DataBit.Bit15) & IOInput1DisValue.DataBit.Bit15)
	                           |(IOInput1En.DataBit.Bit15 & ((IOInput1Fpga.DataBit.Bit15^IOInput1Logic.DataBit.Bit15)^1)));
	////////////////////////////////////////////////////////////////////////////////
	IOInput2.DataBit.Bit0 = (((!IOInput2En.DataBit.Bit0) & IOInput2DisValue.DataBit.Bit0)
	                           |(IOInput2En.DataBit.Bit0 & ((IOInput2Fpga.DataBit.Bit0^IOInput2Logic.DataBit.Bit0)^1)));
	IOInput2.DataBit.Bit1 = (((!IOInput2En.DataBit.Bit1) & IOInput2DisValue.DataBit.Bit1)
	                           |(IOInput2En.DataBit.Bit1 & ((IOInput2Fpga.DataBit.Bit1^IOInput2Logic.DataBit.Bit1)^1)));
	IOInput2.DataBit.Bit2 = (((!IOInput2En.DataBit.Bit2) & IOInput2DisValue.DataBit.Bit2)
	                           |(IOInput2En.DataBit.Bit2 & ((IOInput2Fpga.DataBit.Bit2^IOInput2Logic.DataBit.Bit2)^1)));
	IOInput2.DataBit.Bit3 = (((!IOInput2En.DataBit.Bit3) & IOInput2DisValue.DataBit.Bit3)
	                           |(IOInput2En.DataBit.Bit3 & ((IOInput2Fpga.DataBit.Bit3^IOInput2Logic.DataBit.Bit3)^1)));
	IOInput2.DataBit.Bit4 = (((!IOInput2En.DataBit.Bit4) & IOInput2DisValue.DataBit.Bit4)
	                           |(IOInput2En.DataBit.Bit4 & ((IOInput2Fpga.DataBit.Bit4^IOInput2Logic.DataBit.Bit4)^1)));
	IOInput2.DataBit.Bit5 = (((!IOInput2En.DataBit.Bit5) & IOInput2DisValue.DataBit.Bit5)
	                           |(IOInput2En.DataBit.Bit5 & ((IOInput2Fpga.DataBit.Bit5^IOInput2Logic.DataBit.Bit5)^1)));
	IOInput2.DataBit.Bit6 = (((!IOInput2En.DataBit.Bit6) & IOInput2DisValue.DataBit.Bit6)
	                           |(IOInput2En.DataBit.Bit6 & ((IOInput2Fpga.DataBit.Bit6^IOInput2Logic.DataBit.Bit6)^1)));
	IOInput2.DataBit.Bit7 = (((!IOInput2En.DataBit.Bit7) & IOInput2DisValue.DataBit.Bit7)
	                           |(IOInput2En.DataBit.Bit7 & ((IOInput2Fpga.DataBit.Bit7^IOInput2Logic.DataBit.Bit7)^1)));
	IOInput2.DataBit.Bit8 = (((!IOInput2En.DataBit.Bit8) & IOInput2DisValue.DataBit.Bit8)
	                           |(IOInput2En.DataBit.Bit8 & ((IOInput2Fpga.DataBit.Bit8^IOInput2Logic.DataBit.Bit8)^1)));
	IOInput2.DataBit.Bit9 = (((!IOInput2En.DataBit.Bit9) & IOInput2DisValue.DataBit.Bit9)
	                           |(IOInput2En.DataBit.Bit9 & ((IOInput2Fpga.DataBit.Bit9^IOInput2Logic.DataBit.Bit9)^1)));
	IOInput2.DataBit.Bit10 = (((!IOInput2En.DataBit.Bit10) & IOInput2DisValue.DataBit.Bit10)
	                           |(IOInput2En.DataBit.Bit10 & ((IOInput2Fpga.DataBit.Bit10^IOInput2Logic.DataBit.Bit10)^1)));
}
void IOInputCtrlFlag(void)
{
	static bool bool_io_rst = false;
	static bool bool_io_single = false;
	static bool bool_io_parallel = false;
	static bool bool_io_degauss = false;

    if (!IO_LocRem_Feed)
    {
        if((bool_io_rst == false)&&(IO_RstRef_Feed == 1))
        {
            if(CtrlCnt_IORstRefFeed < 20)
            {
                CtrlCnt_IORstRefFeed ++;
            }
            else
            {
                bool_io_rst = true;
                CtrlCnt_IORstRefFeed = 0;
            }
        }
        else if((bool_io_rst == true)&&(IO_RstRef_Feed == 0))
        {
            if(CtrlCnt_IORstRefFeed < 20)
            {
                CtrlCnt_IORstRefFeed ++;
            }
            else
            {
                bool_io_rst = false;
                CtrlCnt_IORstRefFeed = 0;
                Flag_IORstRef = 1;
            }
        }

        if((bool_io_single == false)&&(IO_SingCtrl_Feed == 1))
        {
            if(CtrlCnt_IOSingCtrl < 20)
            {
                CtrlCnt_IOSingCtrl ++;
            }
            else
            {
                bool_io_single = true;
                CtrlCnt_IOSingCtrl = 0;
            }
        }
        else if((bool_io_single == true)&&(IO_SingCtrl_Feed == 0))
        {
            if(CtrlCnt_IOSingCtrl < 20)
            {
                CtrlCnt_IOSingCtrl ++;
            }
            else
            {
                bool_io_single = false;
                CtrlCnt_IOSingCtrl = 0;
                Flag_IOSingCtrl = 1;
            }
        }

        if((bool_io_parallel == false)&&(IO_ParaCtrl_Feed == 1))
        {
            if(CtrlCnt_IOParaCtrl < 20)
            {
                CtrlCnt_IOParaCtrl ++;
            }
            else
            {
                bool_io_parallel = true;
                CtrlCnt_IOParaCtrl = 0;
            }
        }
        else if((bool_io_parallel == true)&&(IO_ParaCtrl_Feed == 0))
        {
            if(CtrlCnt_IOParaCtrl < 20)
            {
                CtrlCnt_IOParaCtrl ++;
            }
            else
            {
                bool_io_parallel = false;
                CtrlCnt_IOParaCtrl = 0;
                Flag_IOParaCtrl = 1;
            }
        }

        if((bool_io_degauss == false)&&(IO_DgssCtrl_Feed == 1))
        {
            if(CtrlCnt_IODgssCtrl < 20)
            {
                CtrlCnt_IODgssCtrl ++;
            }
            else
            {
                bool_io_degauss = true;
                CtrlCnt_IODgssCtrl = 0;
            }
        }
        else if((bool_io_degauss == true)&&(IO_DgssCtrl_Feed == 0))
        {
            if(CtrlCnt_IODgssCtrl < 20)
            {
                CtrlCnt_IODgssCtrl ++;
            }
            else
            {
                bool_io_degauss = false;
                CtrlCnt_IODgssCtrl = 0;
                Flag_IODgssCtrl = 1;
            }
        }
    }
    else
    {
    	bool_io_rst = false;
    	bool_io_single = false;
    	bool_io_parallel = false;
    	bool_io_degauss = false;
    }
}
/**********************************************************************************/
void IOOutMapLogic(void)
{
	IOOutFpga1.DataBit.Bit0 = (IOOutput1.DataBit.Bit0 & IOOut1CtrlWord.DataBit.Bit0)^0;
	IOOutFpga1.DataBit.Bit1 = (IOOutput1.DataBit.Bit1 & IOOut1CtrlWord.DataBit.Bit1)^0;
	IOOutFpga1.DataBit.Bit2 = (IOOutput1.DataBit.Bit2 & IOOut1CtrlWord.DataBit.Bit2)^0;
	IOOutFpga1.DataBit.Bit3 = (IOOutput1.DataBit.Bit3 & IOOut1CtrlWord.DataBit.Bit3)^0;
	IOOutFpga1.DataBit.Bit4 = (IOOutput1.DataBit.Bit4 & IOOut1CtrlWord.DataBit.Bit4)^0;
	IOOutFpga1.DataBit.Bit5 = (IOOutput1.DataBit.Bit5 & IOOut1CtrlWord.DataBit.Bit5)^0;
	IOOutFpga1.DataBit.Bit6 = (IOOutput1.DataBit.Bit6 & IOOut1CtrlWord.DataBit.Bit6)^0;
	IOOutFpga1.DataBit.Bit7 = (IOOutput1.DataBit.Bit7 & IOOut1CtrlWord.DataBit.Bit7)^0;
	IOOutFpga1.DataBit.Bit8 = (IOOutput1.DataBit.Bit8 & IOOut1CtrlWord.DataBit.Bit8)^0;
	IOOutFpga1.DataBit.Bit9 = (IOOutput1.DataBit.Bit9 & IOOut1CtrlWord.DataBit.Bit9)^0;
	IOOutFpga1.DataBit.Bit10 = (IOOutput1.DataBit.Bit10 & IOOut1CtrlWord.DataBit.Bit10)^0;
	IOOutFpga1.DataBit.Bit11 = (IOOutput1.DataBit.Bit11 & IOOut1CtrlWord.DataBit.Bit11)^0;
	IOOutFpga1.DataBit.Bit12 = (IOOutput1.DataBit.Bit12 & IOOut1CtrlWord.DataBit.Bit12)^0;
	IOOutFpga1.DataBit.Bit13 = (IOOutput1.DataBit.Bit13 & IOOut1CtrlWord.DataBit.Bit13)^0;
	IOOutFpga1.DataBit.Bit14 = (IOOutput1.DataBit.Bit14 & IOOut1CtrlWord.DataBit.Bit14)^0;
	IOOutFpga1.DataBit.Bit15 = (IOOutput1.DataBit.Bit15 & IOOut1CtrlWord.DataBit.Bit15)^0;

	IOOutFpga2.DataBit.Bit0 = (IOOutput2.DataBit.Bit0 & IOOut2CtrlWord.DataBit.Bit0)^0;
	IOOutFpga2.DataBit.Bit1 = (IOOutput2.DataBit.Bit1 & IOOut2CtrlWord.DataBit.Bit1)^0;
	IOOutFpga2.DataBit.Bit2 = (IOOutput2.DataBit.Bit2 & IOOut2CtrlWord.DataBit.Bit2)^0;
	IOOutFpga2.DataBit.Bit3 = (IOOutput2.DataBit.Bit3 & IOOut2CtrlWord.DataBit.Bit3)^0;
	IOOutFpga2.DataBit.Bit4 = (IOOutput2.DataBit.Bit4 & IOOut2CtrlWord.DataBit.Bit4)^0;
	IOOutFpga2.DataBit.Bit5 = (IOOutput2.DataBit.Bit5 & IOOut2CtrlWord.DataBit.Bit5)^0;
	IOOutFpga2.DataBit.Bit6 = (IOOutput2.DataBit.Bit6 & IOOut2CtrlWord.DataBit.Bit6)^0;
	IOOutFpga2.DataBit.Bit7 = (IOOutput2.DataBit.Bit7 & IOOut2CtrlWord.DataBit.Bit7)^0;
	IOOutFpga2.DataBit.Bit8 = (IOOutput2.DataBit.Bit8 & IOOut2CtrlWord.DataBit.Bit8)^0;
	IOOutFpga2.DataBit.Bit9 = (IOOutput2.DataBit.Bit9 & IOOut2CtrlWord.DataBit.Bit9)^0;
	IOOutFpga2.DataBit.Bit10 = (IOOutput2.DataBit.Bit10 & IOOut2CtrlWord.DataBit.Bit10)^0;
	IOOutFpga2.DataBit.Bit11 = (IOOutput2.DataBit.Bit11 & IOOut2CtrlWord.DataBit.Bit11)^0;
	IOOutFpga2.DataBit.Bit12 = (IOOutput2.DataBit.Bit12 & IOOut2CtrlWord.DataBit.Bit12)^0;
	IOOutFpga2.DataBit.Bit13 = (IOOutput2.DataBit.Bit13 & IOOut2CtrlWord.DataBit.Bit13)^0;
	IOOutFpga2.DataBit.Bit14 = (IOOutput2.DataBit.Bit14 & IOOut2CtrlWord.DataBit.Bit14)^0;
	IOOutFpga2.DataBit.Bit15 = (IOOutput2.DataBit.Bit15 & IOOut2CtrlWord.DataBit.Bit15)^0;
}
/***********************************************************************/
void IO_Scan(void)
{
	int i;
	//read IO status input
	for(i=0; i<5; i++);
	IOInput1Fpga.DataInt = FPGARDIOSTS1;
	for(i=0; i<5; i++);
//	IOInputFpga2.DataInt = FPGARDIOSTS2;
	IOInput2Fpga.DataInt = FPGARDIOSTS3;
	IOInMapLogic();
    IOInputCtrlFlag();
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

