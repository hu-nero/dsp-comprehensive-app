// FILE:   DSP2833x_EPwm.c
#include "DSP2833x_Device.h"     
#include "DSP2833x_Examples.h"   
#include "DSP2833x_EPwm_defines.H"
#include "Main\Variable.h"
#include "Main\Comm_Variable.H"
#include "GPIO\Custom_IOMacro.H"

#pragma CODE_SECTION(Dis_PWM, "ramfuncs");

/********************************************************************************/
void InitEPwm1(void)
{
	//Time-Base Submodule Registers
	EPwm1Regs.TBPRD            = PWM_PeriodReg; //Time-Base Period Register
	EPwm1Regs.TBPHS.half.TBPHS = 0;             //Time-Base Phase Register
	EPwm1Regs.TBCTR            = 0;             //Time-Base Counter Register
	// Time-Base Control Register
	EPwm1Regs.TBCTL.bit.FREE_SOFT = 2;
	EPwm1Regs.TBCTL.bit.PHSDIR    = TB_UP;
	EPwm1Regs.TBCTL.bit.CLKDIV    = TB_DIV1;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;         // Clock ratio to SYSCLKOU
	EPwm1Regs.TBCTL.bit.SYNCOSEL  = TB_CTR_ZERO;
	EPwm1Regs.TBCTL.bit.PRDLD     = TB_SHADOW;               //The period register (TBPRD) is loaded from its shadow register
	EPwm1Regs.TBCTL.bit.PHSEN     = TB_DISABLE;      // Disable phase loading
	EPwm1Regs.TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN; // Count UPDOWN
	//Time-Base Status Register
	EPwm1Regs.TBSTS.bit.CTRMAX = 1;
	EPwm1Regs.TBSTS.bit.SYNCI  = 1;
	////////////////////////////
	//Counter-Compare Submodule Registers
	EPwm1Regs.CMPA.half.CMPA = (PWM_PeriodReg/2);
	EPwm1Regs.CMPB           = 0;
	//Counter-Compare Control Register
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	//////////////////////////////
	//Action-Qualifier Submodule Registers
	//Action-Qualifier Output A Control Register
	EPwm1Regs.AQCTLA.bit.CBD = AQ_NO_ACTION;
	EPwm1Regs.AQCTLA.bit.CBU = AQ_NO_ACTION;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;//AQ_CLEAR;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;//AQ_SET;
	EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
	//Action-Qualifier Output B Control Register
	EPwm1Regs.AQCTLB.bit.CBD = AQ_NO_ACTION;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_NO_ACTION;
	EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;//AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;//AQ_SET;
	EPwm1Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;
	//Action-Qualifier Software Force Register
	EPwm1Regs.AQSFRC.bit.RLDCSF = 3; //Load immediately
	EPwm1Regs.AQSFRC.bit.OTSFB  = 0;
	EPwm1Regs.AQSFRC.bit.ACTSFB = 0; //Does nothing (action disabled)
	EPwm1Regs.AQSFRC.bit.OTSFA  = 0;
	EPwm1Regs.AQSFRC.bit.ACTSFA = 0; //Does nothing (action disabled)
	//Action-Qualifier Continuous Software Force Register (AQCSFRC)
	EPwm1Regs.AQCSFRC.bit.CSFB = 2;  //Forces a continuous high on output B
	EPwm1Regs.AQCSFRC.bit.CSFA = 1;  //Forces a continuous low on output A
	//////////////////////////////
	//Dead-Band Submodule Registers
	EPwm1Regs.DBCTL.bit.IN_MODE  = DBA_RED_DBB_FED; //EPWMxA  is the source for rising-edge delayed signal,EPWMxB is the source for falling-edge delayed
	EPwm1Regs.DBCTL.bit.POLSEL   = DB_ACTV_LOC;     //Active low complementary (ALC) mode. EPWMxA is inverted
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  //Dead-band is fully enabled for both edge
	EPwm1Regs.DBRED              = PWM_DeadSpace;
	EPwm1Regs.DBFED              = PWM_DeadSpace;
	////////////////////////////
	//PWM-Chopper Submodule Control Register
	EPwm1Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
	//////////////////////////////////////
	//Trip-Zone Submodule Control and Status Registers
	//EALLOW;
	//Trip-Zone Select Register
	EPwm1Regs.TZSEL.bit.OSHT6 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.OSHT5 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.OSHT4 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.OSHT3 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.OSHT2 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.OSHT1 = TZ_ENABLE;

	EPwm1Regs.TZSEL.bit.CBC6 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.CBC5 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.CBC4 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.CBC3 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.CBC2 = TZ_DISABLE;
	EPwm1Regs.TZSEL.bit.CBC1 = TZ_DISABLE;
	//Trip-Zone Control Register (TZCTL)
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_HI;
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_HI;
	//Trip-Zone Enable Interrupt Register
	EPwm1Regs.TZEINT.bit.OST = 1;  //enable one shot interrupt��will cause a EPWMx_ TZINT PIE interrupt��������PWM1
	EPwm1Regs.TZEINT.bit.CBC = 0;
	//Trip-Zone Clear Register (TZCLR)
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm1Regs.TZCLR.bit.CBC = 1;
	EPwm1Regs.TZCLR.bit.INT = 1;
	//EDIS;

	////////////////////////////////////////////
	//Event-Trigger Submodule Registers
	//Event-Trigger Selection Register (ETSEL)
	EPwm1Regs.ETSEL.bit.SOCBEN  = 1;   //Enable EPWMxSOCA pulse
	EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTR_PRD;  //Enable event time-base counter equal to period
	EPwm1Regs.ETSEL.bit.SOCAEN  = 1;   //Enable EPWMxSOCA pulse
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD;
	EPwm1Regs.ETSEL.bit.INTEN   = 1;    //Enable EPWMx_INT generation
	EPwm1Regs.ETSEL.bit.INTSEL  = 1;   //Enable event time-base counter equal to zero. (TBCTR = 0x0000)
	//Event-Trigger Prescale Register(ETPS)
	EPwm1Regs.ETPS.bit.SOCBPRD = ET_1ST;   //Generate the EPWMxSOCB pulse on the first event
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;   //Generate the EPWMxSOCA pulse on the first event
	EPwm1Regs.ETPS.bit.INTPRD  = ET_1ST;   //Generate an interrupt on the first event
	//Event-Trigger Clear Register (ETCLR)
	EPwm1Regs.ETCLR.bit.SOCB = 1;
	EPwm1Regs.ETCLR.bit.SOCA = 1;
	EPwm1Regs.ETCLR.bit.INT  = 1;
}
/******************************************************************************************/
void InitEPwm2(void)
{
	//Time-Base Submodule Registers
	EPwm2Regs.TBPRD = PWM_PeriodReg; //Time-Base Period Register
	EPwm2Regs.TBPHS.half.TBPHS=0; //Time-Base Phase Register
	EPwm2Regs.TBCTR = 0;          //Time-Base Counter Register
	// Time-Base Control Register
	EPwm2Regs.TBCTL.bit.FREE_SOFT = 2;
	EPwm2Regs.TBCTL.bit.PHSDIR = 1;
	EPwm2Regs.TBCTL.bit.CLKDIV    = TB_DIV1;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOU
	EPwm2Regs.TBCTL.bit.SYNCOSEL  = 0;
	EPwm2Regs.TBCTL.bit.PRDLD     = 0;              //The period register (TBPRD) is loaded from its shadow register
	EPwm2Regs.TBCTL.bit.PHSEN     = 1;
	EPwm2Regs.TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN; // Count UPDOWN
	//Time-Base Status Register
	EPwm2Regs.TBSTS.bit.CTRMAX = 1;  //clear event: TBCTR arrive max value 0xFFFF
	EPwm2Regs.TBSTS.bit.SYNCI  = 1;  //clear event: sysnc
	////////////////////////////
	//Counter-Compare Submodule Registers
	EPwm2Regs.CMPA.half.CMPA = (PWM_PeriodReg/2);
	EPwm2Regs.CMPB =0;
	//Counter-Compare Control Register
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	//////////////////////////////
	//Action-Qualifier Submodule Registers
	//Action-Qualifier Output A Control Register
	EPwm2Regs.AQCTLA.bit.CBD=0;
	EPwm2Regs.AQCTLA.bit.CBU=0;
	EPwm2Regs.AQCTLA.bit.CAD=AQ_SET;//AQ_CLEAR;
	EPwm2Regs.AQCTLA.bit.CAU=AQ_CLEAR;//AQ_SET;
	EPwm2Regs.AQCTLA.bit.PRD=0;
	EPwm2Regs.AQCTLA.bit.ZRO=0;
	//Action-Qualifier Output B Control Register
	EPwm2Regs.AQCTLB.bit.CBD=0;
	EPwm2Regs.AQCTLB.bit.CBU=0;
	EPwm2Regs.AQCTLB.bit.CAD=AQ_SET;//AQ_CLEAR;
	EPwm2Regs.AQCTLB.bit.CAU=AQ_CLEAR;//AQ_SET;
	EPwm2Regs.AQCTLB.bit.PRD=0;
	EPwm2Regs.AQCTLB.bit.ZRO=0;
	//Action-Qualifier Software Force Register
	EPwm2Regs.AQSFRC.bit.RLDCSF=3;//Load immediately
	EPwm2Regs.AQSFRC.bit.OTSFB=0;
	EPwm2Regs.AQSFRC.bit.ACTSFB=0;//Does nothing (action disabled)
	EPwm2Regs.AQSFRC.bit.OTSFA=0;
	EPwm2Regs.AQSFRC.bit.ACTSFA=0;//Does nothing (action disabled)
	//Action-Qualifier Continuous Software Force Register (AQCSFRC)
	EPwm2Regs.AQCSFRC.bit.CSFB=2;//Forces a continuous high on output B
	EPwm2Regs.AQCSFRC.bit.CSFA=1;//Forces a continuous low on output A
	//////////////////////////////
	//Dead-Band Submodule Registers
	EPwm2Regs.DBCTL.bit.IN_MODE=2;//EPWMxA  is the source for rising-edge delayed signal,EPWMxB is the source for falling-edge delayed
	EPwm2Regs.DBCTL.bit.POLSEL=1;//Active low complementary (ALC) mode. EPWMxA is inverted
	//EPwm2Regs.DBCTL.bit.POLSEL=2;
	EPwm2Regs.DBCTL.bit.OUT_MODE=3;//Dead-band is fully enabled for both edge
	EPwm2Regs.DBRED = PWM_DeadSpace;//rising delay 2us
	EPwm2Regs.DBFED = PWM_DeadSpace;//falling delay 2us
	////////////////////////////
	//PWM-Chopper Submodule Control Register
	EPwm2Regs.PCCTL.bit.CHPEN=0;
	//////////////////////////////////////
	//Trip-Zone Submodule Control and Status Registers
	//EALLOW;
	//Trip-Zone Select Register
	EPwm2Regs.TZSEL.bit.OSHT6 = 0;
	EPwm2Regs.TZSEL.bit.OSHT5 = 0;
	EPwm2Regs.TZSEL.bit.OSHT4 = 0;
	EPwm2Regs.TZSEL.bit.OSHT3 = 0;
	EPwm2Regs.TZSEL.bit.OSHT2 = 0;
	EPwm2Regs.TZSEL.bit.OSHT1 = 1;
	EPwm2Regs.TZSEL.bit.CBC6 = 0;
	EPwm2Regs.TZSEL.bit.CBC5 = 0;
	EPwm2Regs.TZSEL.bit.CBC4 = 0;
	EPwm2Regs.TZSEL.bit.CBC3 = 0;
	EPwm2Regs.TZSEL.bit.CBC2 = 0;
	EPwm2Regs.TZSEL.bit.CBC1 = 0;
	//Trip-Zone Control Register (TZCTL)
	EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_HI;//TZ_FORCE_LO;//TZ_HIZ;//High impedance
	EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_HI;//TZ_FORCE_LO;//TZ_HIZ;
	//Trip-Zone Enable Interrupt Register
	EPwm2Regs.TZEINT.bit.OST=0;  //disable one shot interrupt
	EPwm2Regs.TZEINT.bit.CBC=0;
	//Trip-Zone Clear Register (TZCLR)
	EPwm2Regs.TZCLR.bit.OST=1;
	EPwm2Regs.TZCLR.bit.CBC=1;
	EPwm2Regs.TZCLR.bit.INT=1;
	//EDIS;

	////////////////////////////////////////////
	//Event-Trigger Submodule Registers
	//Event-Trigger Selection Register (ETSEL)
	EPwm2Regs.ETSEL.bit.SOCBEN=0;   //disable EPWMxSOCB pulse
	EPwm2Regs.ETSEL.bit.SOCBSEL=0;  //disable EPWMxSOCB pulse
	EPwm2Regs.ETSEL.bit.SOCAEN=0;   //disable EPWMxSOCA pulse
	EPwm2Regs.ETSEL.bit.SOCASEL=0;  //disable EPWMxSOCA pulse
	EPwm2Regs.ETSEL.bit.INTEN=0;    //disable EPWMx_INT generation
	EPwm2Regs.ETSEL.bit.INTSEL=0;   //disable EPWMx_INT generation
	//Event-Trigger Prescale Register(ETPS)
	EPwm2Regs.ETPS.bit.SOCBPRD=0;   //disable EPWMxSOCB pulse
	EPwm2Regs.ETPS.bit.SOCAPRD=0;   //disable EPWMxSOCA pulse
	EPwm2Regs.ETPS.bit.INTPRD=0;    //disable EPWMx_INT generation
	//Event-Trigger Clear Register (ETCLR)
	EPwm2Regs.ETCLR.bit.SOCB=1;
	EPwm2Regs.ETCLR.bit.SOCA=1;
	EPwm2Regs.ETCLR.bit.INT=1;
}
/*****************************************************************************************/
void InitEPwm3(void)
{
	//Time-Base Submodule Registers
	EPwm3Regs.TBPRD = PWM_PeriodReg; //Time-Base Period Register,2kHz
	EPwm3Regs.TBPHS.half.TBPHS=0;//Time-Base Phase Register
	EPwm3Regs.TBCTR = 0; //Time-Base Counter Register
	// Time-Base Control Register
	EPwm3Regs.TBCTL.bit.FREE_SOFT=2;
	EPwm3Regs.TBCTL.bit.PHSDIR=1;
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOU
	EPwm3Regs.TBCTL.bit.SYNCOSEL=0;
	EPwm3Regs.TBCTL.bit.PRDLD=0;         //The period register (TBPRD) is loaded from its shadow register
	EPwm3Regs.TBCTL.bit.PHSEN = 1;
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count UPDOWN
	//Time-Base Status Register
	EPwm3Regs.TBSTS.bit.CTRMAX=1;
	EPwm3Regs.TBSTS.bit.SYNCI=1;

	////////////////////////////
	//Counter-Compare Submodule Registers
	EPwm3Regs.CMPA.half.CMPA = (PWM_PeriodReg/2);
	EPwm3Regs.CMPB =0;
	//Counter-Compare Control Register
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	//////////////////////////////
	//Action-Qualifier Submodule Registers
	//Action-Qualifier Output A Control Register
	EPwm3Regs.AQCTLA.bit.CBD=0;
	EPwm3Regs.AQCTLA.bit.CBU=0;
	EPwm3Regs.AQCTLA.bit.CAD=AQ_SET;//AQ_CLEAR;
	EPwm3Regs.AQCTLA.bit.CAU=AQ_CLEAR;//AQ_SET;
	EPwm3Regs.AQCTLA.bit.PRD=0;
	EPwm3Regs.AQCTLA.bit.ZRO=0;
	//Action-Qualifier Output B Control Register
	EPwm3Regs.AQCTLB.bit.CBD=0;
	EPwm3Regs.AQCTLB.bit.CBU=0;
	EPwm3Regs.AQCTLB.bit.CAD=AQ_SET;//AQ_CLEAR;
	EPwm3Regs.AQCTLB.bit.CAU=AQ_CLEAR;//AQ_SET;
	EPwm3Regs.AQCTLB.bit.PRD=0;
	EPwm3Regs.AQCTLB.bit.ZRO=0;
	//Action-Qualifier Software Force Register
	EPwm3Regs.AQSFRC.bit.RLDCSF=3;//Load immediately
	EPwm3Regs.AQSFRC.bit.OTSFB=0;
	EPwm3Regs.AQSFRC.bit.ACTSFB=0;//Does nothing (action disabled)
	EPwm3Regs.AQSFRC.bit.OTSFA=0;
	EPwm3Regs.AQSFRC.bit.ACTSFA=0;//Does nothing (action disabled)
	//Action-Qualifier Continuous Software Force Register (AQCSFRC)
	EPwm3Regs.AQCSFRC.bit.CSFB=2;//Forces a continuous high on output B
	EPwm3Regs.AQCSFRC.bit.CSFA=1;//Forces a continuous low on output A
	//////////////////////////////
	//Dead-Band Submodule Registers
	EPwm3Regs.DBCTL.bit.IN_MODE=2;//EPWMxA  is the source for rising-edge delayed signal,EPWMxB is the source for falling-edge delayed
	EPwm3Regs.DBCTL.bit.POLSEL=1;//Active low complementary (ALC) mode. EPWMxA is inverted
	//EPwm3Regs.DBCTL.bit.POLSEL=2;
	EPwm3Regs.DBCTL.bit.OUT_MODE=3;//Dead-band is fully enabled for both edge
	EPwm3Regs.DBRED = PWM_DeadSpace;//rising delay 4us
	EPwm3Regs.DBFED = PWM_DeadSpace;//falling delay 4us

	////////////////////////////
	//PWM-Chopper Submodule Control Register
	EPwm3Regs.PCCTL.bit.CHPEN=0;
	//////////////////////////////////////
	//Trip-Zone Submodule Control and Status Registers
	//EALLOW;
	//Trip-Zone Select Register
	EPwm3Regs.TZSEL.bit.OSHT6 = 0;
	EPwm3Regs.TZSEL.bit.OSHT5 = 0;
	EPwm3Regs.TZSEL.bit.OSHT4 = 0;
	EPwm3Regs.TZSEL.bit.OSHT3 = 0;
	EPwm3Regs.TZSEL.bit.OSHT2 = 0;
	EPwm3Regs.TZSEL.bit.OSHT1 = 1;
	EPwm3Regs.TZSEL.bit.CBC6 = 0;
	EPwm3Regs.TZSEL.bit.CBC5 = 0;
	EPwm3Regs.TZSEL.bit.CBC4 = 0;
	EPwm3Regs.TZSEL.bit.CBC3 = 0;
	EPwm3Regs.TZSEL.bit.CBC2 = 0;
	EPwm3Regs.TZSEL.bit.CBC1 = 0;
	//Trip-Zone Control Register (TZCTL)
	EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_HI;//TZ_FORCE_LO;//TZ_HIZ;//High impedance
	EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_HI;//TZ_FORCE_LO;//TZ_HIZ;
	//Trip-Zone Enable Interrupt Register
	EPwm3Regs.TZEINT.bit.OST=0;  //disable one shot interrupt
	EPwm3Regs.TZEINT.bit.CBC=0;
	//Trip-Zone Clear Register (TZCLR)
	EPwm3Regs.TZCLR.bit.OST=1;
	EPwm3Regs.TZCLR.bit.CBC=1;
	EPwm3Regs.TZCLR.bit.INT=1;
	//EDIS;
	////////////////////////////////////////////
	//Event-Trigger Submodule Registers
	//Event-Trigger Selection Register (ETSEL)
	EPwm3Regs.ETSEL.bit.SOCBEN=0;   //disable EPWMxSOCB pulse
	EPwm3Regs.ETSEL.bit.SOCBSEL=0;  //disable EPWMxSOCB pulse
	EPwm3Regs.ETSEL.bit.SOCAEN=0;   //disable EPWMxSOCA pulse
	EPwm3Regs.ETSEL.bit.SOCASEL=0;  //disable EPWMxSOCA pulse
	EPwm3Regs.ETSEL.bit.INTEN=0;    //disable EPWMx_INT generation
	EPwm3Regs.ETSEL.bit.INTSEL=0;   //disable EPWMx_INT generation
	//Event-Trigger Prescale Register(ETPS)
	EPwm3Regs.ETPS.bit.SOCBPRD=0;   //disable EPWMxSOCB pulse
	EPwm3Regs.ETPS.bit.SOCAPRD=0;   //disable EPWMxSOCA pulse
	EPwm3Regs.ETPS.bit.INTPRD=0;    //disable EPWMx_INT generation
	//Event-Trigger Clear Register (ETCLR)
	EPwm3Regs.ETCLR.bit.SOCB=1;
	EPwm3Regs.ETCLR.bit.SOCA=1;
	EPwm3Regs.ETCLR.bit.INT=1;
}
/********************************************************************************/
// InitEPwm: 
void InitEPwm(void)
{
	InitEPwm1();
//	InitEPwm2();
//	InitEPwm3();
}
/*********************************************************************************/
void Dis_PWM(void)
{
	IO_PWM_DIS;
}
/********************************************************************************/
void En_PWM(void)
{
	IO_PWM_EN;
}
//===========================================================================
// End of file.
//===========================================================================
