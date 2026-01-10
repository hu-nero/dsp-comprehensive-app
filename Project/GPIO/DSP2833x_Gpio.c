#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "main\Custom_GlobalMacro.H"
#include "Main\Variable.H"
#include "Custom_IOMacro.H"
#include "Xintf\Custom_Xintf_ADC&IO.H"
/////////////////////////////////////////////////
void GpioRegs_MUX_Config(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0xA1000000;/*1010,0001,0000,0000;0000,0000,0000,0000
	GPIO15, 10, SCIRXDB(I)
	GPIO14, 10, SCITXDB(O)
	GPIO13, 00, GPIO(O), FPGA
	GPIO12, 01, TZ1(I),

	GPIO11, 00, GPIO(O), 485EN1
	GPIO10, 00, GPIO(O), ADSel3
	GPIO9, 00, GPIO(O), ADSel2
	GPIO8, 00, GPIO(O), ADSel1

	GPIO7, 00, GPIO(I), DI7
	GPIO6, 00, GPIO(I), RSVD-EPWMSYNCI/O
	GPIO5, 00, GPIO(I), DI6
	GPIO4, 00, GPIO(I), DI5

	GPIO3, 00, GPIO(I), DI4
	GPIO2, 00, GPIO(I), DI3
	GPIO1, 00, GPIO(I), DI2
	GPIO0, 00, GPIO(I), DI1
	*/
	GpioCtrlRegs.GPAMUX2.all = 0xA9002AFA;/*1010,1001,0000,0000;0010,1010,1111,1010
	GPIO31, 10, XA17 (O)
	GPIO30, 10, XA18 (O),
	GPIO29, 10, XA19 (O),
	GPIO28, 01, SCIRXDA (I)

	GPIO27, 00, FPGA
	GPIO26, 00, GPIO (I), DI11
	GPIO25, 00, GPIO (I), FPGAISR
	GPIO24, 00, GPIO (O), FPGARST

	GPIO23, 10, MFSXA(I/O)
	GPIO22, 10, MCLKXA(I/O)
	GPIO21, 10, MDRA(I)
	GPIO20, 10, MDXA(O)

	GPIO19, 11, CANRXA(I)
	GPIO18, 11, CANTXA(O)
	GPIO17, 10, CANRXB(I)
	GPIO16, 10, CANTXB(O)
	*/
	GpioCtrlRegs.GPBMUX1.all = 0xAAAAA245;/*1010,1010,1010,1010; 1010,0010,0100,0101
	GPIO47, 10, XA7
	GPIO46, 10, XA6
	GPIO45, 10, XA5
	GPIO44, 10, XA4

	GPIO43, 10, XA3
	GPIO42, 10, XA2
	GPIO41, 10, XA1
	GPIO40, 10, XA0

	GPIO39, 10, XA16(O)
	GPIO38, 10, XWE0(O)
	GPIO37, 00, GPIO(O),RSVD
	GPIO36, 10, XZCS0

	GPIO35, 01, SCITXDA
	GPIO34, 00, NULL
	GPIO33, 01, SCLA
	GPIO32, 01, SDAA
	*/
	GpioCtrlRegs.GPBMUX2.all = 0x50015450;/*0101,0000,0000,0001;0101,0100,0101,0000

	GPIO63, 01, SCITXDC(O)
	GPIO62, 01, SCIRXDC(I)
	GPIO61, 00, GPIO(I), DI10
	GPIO60, 00, GPIO(I), DI9

	GPIO59, 00, GPIO(I), DI12
	GPIO58, 00, GPIO(O), fpga_PWMEN
	GPIO57, 00, GPIO, EEPROM_CS
	GPIO56, 01, SPICLKA

	GPIO55, 01, SPISOMIA
	GPIO54, 01, SPISIMOA
	GPIO53, 01, EQEP1I(I)
	GPIO52, 00, GPIO(I), DI8

	GPIO51, 01, EQEP1B(I)
	GPIO50, 01, EPEQ1A(I)
	GPIO49, 00, GPIO(O), LEDR
	GPIO48, 00, GPIO(O), LEDG
	*/
	GpioCtrlRegs.GPCMUX1.all=0xAAAAAAAA; /*1010,1010,1010,1010;1010,1010,1010,1010
	GPIO79, 10, XD0
	GPIO78, 10, XD1
	GPIO77, 10, XD2
	GPIO76, 10, XD3
	GPIO75, 10, XD4
	GPIO74, 10, XD5
	GPIO73, 10, XD6
	GPIO72, 10, XD7
	GPIO71, 10, XD8
	GPIO70, 10, XD9
	GPIO69, 10, XD10
	GPIO68, 10, XD11
	GPIO67, 10, XD12
	GPIO66, 10, XD13
	GPIO65, 10, XD14
	GPIO64, 10, XD15
	*/
	GpioCtrlRegs.GPCMUX2.all=0x0000AAA8;/*0000,0000,0000,0000;1010,1010,1010,1000
	GPIO87, 10, XA15(O),
	GPIO86, 10, XA14(O),
	GPIO85, 10, XA13(O),
	GPIO84, 10, XA12(O),
	GPIO83, 10, XA11(O),
	GPIO82, 10, XA10(O),
	GPIO81, 10, XA9 (O),
	GPIO80, 00, GPIO
	*/
	EDIS;
}

void GpioRegs_DIR_Config(void)
{
	EALLOW;
	GpioCtrlRegs.GPADIR.all=0xE9156F00;/*1110,1001, 0001,0101; 0110,1111, 0000,0000
	0,input;1,output
	1110, 1001
	GPIO31, 1, XA17 (O)
	GPIO30, 1, XA18 (O)
	GPIO29, 1, XA19 (O)
	GPIO28, 0, SCIRXDA (I)

	GPIO27, 1, FPGA
	GPIO26, 0, GPIO (I), DI11
	GPIO25, 0, GPIO (I), FPGAISR
	GPIO24, 1, GPIO (O), FPGARST

	0001,0101
	GPIO23, 0, MFSXA(I/O)
	GPIO22, 0, MCLKXA(I/O)
	GPIO21, 0, MDRA(I)
	GPIO20, 1, MDXA(O)

	GPIO19, 0, CANRXA(I)
	GPIO18, 1, CANTXA(O)
	GPIO17, 0, CANRXB(I)
	GPIO16, 1, CANTXB(O)

	0110,1111
	GPIO15, 0, SCIRXDB(I)
	GPIO14, 1, SCITXDB(O)
	GPIO13, 1, GPIO(O), FPGA
	GPIO12, 0, TZ1(I),
	GPIO11, 1, GPIO(O), 485EN1
	GPIO10, 1, GPIO(O), ADSel3
	GPIO9, 1, GPIO(O), ADSel2
	GPIO8, 1, GPIO(O), ADSel1
	00000000
	GPIO7, 0, GPIO(I), DI7
	GPIO6, 0, GPIO(I), RSVD-EPWMSYNCI/O
	GPIO5, 0, GPIO(I), DI6
	GPIO4, 0, GPIO(I), DI5
	GPIO3, 0, GPIO(I), DI4
	GPIO2, 0, GPIO(I), DI3
	GPIO1, 0, GPIO(I), DI2
	GPIO0, 0, GPIO(I), DI1
	*/
	GpioCtrlRegs.GPBDIR.all=0x8743FF7A;/*1000,0111,0100,0011,1111,1111,0111,1010
	0,input; 1,output

	1000,0111
	GPIO63, 1, SCITXDC(O)
	GPIO62, 0, SCIRXDC(I)
	GPIO61, 0, GPIO(I), DI10
	GPIO60, 0, GPIO(I), DI9
	GPIO59, 0, GPIO(I), DI12
	GPIO58, 1, GPIO(O), FPGA-PWMEN
	GPIO57, 1, GPIOOut, EEPROM_CS
	GPIO56, 1, SPICLKA

	0100,0011
	GPIO55, 0, SPISOMIA
	GPIO54, 1, SPISIMOA
	GPIO53, 0, EQEP1I(I)
	GPIO52, 0, GPIO(I), DI8

	GPIO51, 0, EQEP1B(I)
	GPIO50, 0, EPEQ1A(I)
	GPIO49, 1, GPIO(O), LEDR
	GPIO48, 1, GPIO(O), LEDG

	1111,1111
	GPIO47,1,XA7
	GPIO46,1,XA6
	GPIO45,1,XA5
	GPIO44,1,XA4
	GPIO43,1,XA3
	GPIO42,1,XA2
	GPIO41,1,XA1
	GPIO40,1,XA0

	0111,1010
	GPIO39,0, GPIO(I), DI11
	GPIO38,1,XWE0(O)
	GPIO37,1,GPIO(O),RSVD
	GPIO36,1,XZCS0(O)
	GPIO35,1,SCITXDA
	GPIO34,0,NULL
	GPIO33,1,SCLA
	GPIO32,0,SDAA
	*/
	GpioCtrlRegs.GPCDIR.all=0x00FF0000;/*XXXX,XXXX,1111,1111,0000,0000,0000,0000
	0,input;1,output

	1111,1111
	GPIO87, 1, XA15(O)
	GPIO86, 1, XA14(O)
	GPIO85, 1, XA13(O)
	GPIO84, 1, XA12(O)
	GPIO83, 1, XA11(O)
	GPIO82, 1, XA10(O)
	GPIO81, 1, XA9 (O)
	GPIO80, 1, GPIO

	0000,0000
	GPIO79,0,XD0
	GPIO78,0,XD1
	GPIO77,0,XD2
	GPIO76,0,XD3
	GPIO75,0,XD4
	GPIO74,0,XD5
	GPIO73,0,XD6
	GPIO72,0,XD7

	0000,0000
	GPIO71,0,XD8
	GPIO70,0,XD9
	GPIO69,0,XD10
	GPIO68,0,XD11
	GPIO67,0,XD12
	GPIO66,0,XD13
	GPIO65,0,XD14
	GPIO64,0,XD15
	*/
	EDIS;
}

void GpioRegs_CTRL_Config(void)
{
	EALLOW;
	GpioCtrlRegs.GPACTRL.all=0x00000000;/*
	Specifies the sampling period for pins,00,Sampling Period = TSYSCLKOUT
	bit31~24: GPIO24 ~ GPIO31
	bit23~16: GPIO16 ~ GPIO23
	bit15~8:  GPIO8 ~ GPIO15
	bit7~0:   GPIO0 ~ GPIO7
	*/
	GpioCtrlRegs.GPBCTRL.all=0x00000000;/*
	Specifies the sampling period for pins,00,Sampling Period = TSYSCLKOUT
	bit31~24: GPIO56 ~ GPIO63
	bit23~16: GPIO48 ~ GPIO55
	bit15~8:  GPIO40 ~ GPIO47
	bit7~0:   GPIO32 ~ GPIO39

	GPIO53,01,EQEP1I
	GPIO51,01,EQEP1B
	GPIO50,01,EPEQ1A
	*/
	EDIS;
}
/*********************************************************************************/
void GPIO_init(void)
{
	EALLOW;

	GpioRegs_MUX_Config();
	GpioRegs_CTRL_Config();
	GpioRegs_DIR_Config();

	GpioCtrlRegs.GPAQSEL1.all=0xFFFFFFFF;/*
	Qualification Select,11,Asynchronous
	GPIO15~GPIO0,
	*/
	GpioCtrlRegs.GPAQSEL2.all=0xFFFFFFFF;/*
	Qualification Select,11,Asynchronous,
	GPIO31~GPIO16,
	*/
	GpioCtrlRegs.GPBQSEL1.all=0xFFFFFFFF;/*
	Qualification Select,11,Asynchronous
	GPIO47~GPIO32,
	*/
	GpioCtrlRegs.GPBQSEL2.all=0xFFFFFBAF;/*
	Qualification Select,11,Asynchronous
	GPIO63~GPIO48,
	6362,6160,5958,5756;5554,5352,5150,4948
	1111,1111,1111,1111;1111,1011,1010,1111
	GPIO53,01,EQEP1I
	GPIO51,01,EQEP1B
	GPIO50,01,EPEQ1A
	*/

	GpioCtrlRegs.GPAPUD.all = 0;
	GpioCtrlRegs.GPBPUD.all = 0;
	GpioCtrlRegs.GPCPUD.all = 0;

	GpioIntRegs.GPIOXINT1SEL.all = 0;
	GpioIntRegs.GPIOXINT2SEL.all = 0;
	GpioIntRegs.GPIOXINT3SEL.all = 0;
	GpioIntRegs.GPIOXINT4SEL.all = 0;
	GpioIntRegs.GPIOXINT5SEL.all = 0;
	GpioIntRegs.GPIOXINT6SEL.all = 0;
	GpioIntRegs.GPIOXINT7SEL.all = 0;
	GpioIntRegs.GPIOLPMSEL.all = 0;
	EDIS;

	GpioDataRegs.GPADAT.all = 0xFFFFFFFF;
	GpioDataRegs.GPBDAT.all = 0xFFFFFFFF;
	GpioDataRegs.GPCDAT.all = 0xFFFFFFFF;

	IOInputFpga1.DataInt = 0;
	IOInput1.DataInt = 0;
	IOInput2.DataInt = 0;
	IOInputEn.DataInt = 0xFFFF;
	IOInputLogic.DataInt = 0;
	IOInputDisValue.DataInt = 0;

	IOOutFpga1.DataInt = 0;
	IOOutFpga1Old.DataInt = 0;
	IOOutput1.DataInt = 0;
	IOOutput1Old.DataInt = 0;

	IOOutCtrl1Word.DataInt = 0xFFFF;

	IO_ACB1OFF_Dis;
	IO_ACB2OFF_Dis;

	IO_NprRun_Dis;
	IO_NprRst_Dis;

	IO_EN2RS485_RECV;
	IO_EN1RS485_RECV;
	IO_PWM_UNLOCK;
}
/********************************************************************************/
void Flash_CS_En(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO23=1;
}
/////////////////////////////////////////////
void Flash_CS_Dis(void)
{
	GpioDataRegs.GPASET.bit.GPIO23=1;
}
/*******************************************************************************/
void Flash_HardResetEn(void)
{
	//GpioDataRegs.GPBCLEAR.bit.GPIO58=1;
}
//////////////////////////////////////
void Flash_HardResetDis(void)
{
	//GpioDataRegs.GPBSET.bit.GPIO58=1;
}
/***************************************************************************/
void EEPROM_CS_En(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO57=1;
	asm(" NOP");
}
/////////////////////////////////////////////
void EEPROM_CS_Dis(void)
{
	GpioDataRegs.GPBSET.bit.GPIO57=1;
}
/********************************************************************************/
void RS485_Send_En(void)
{
	int i=0;
	for(i=0; i<5; i++);
	GpioDataRegs.GPASET.bit.GPIO11 = 1;

	for(i=0; i<5; i++);
}
/********************************************************************************/
void RS485_Send_Dis(void)
{
	int i=0;
	for(i=0; i<5; i++);
	GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
	for(i=0; i<5; i++);
}
/********************************************************************************/
void RS485_Send_En2(void)
{
	int i=0;
	for(i=0; i<5; i++);
	GpioDataRegs.GPASET.bit.GPIO26 = 1;
	for(i=0; i<5; i++);
}
/********************************************************************************/
void RS485_Send_Dis2(void)
{
	int i=0;
	for(i=0; i<5; i++);
	GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;
	for(i=0; i<5; i++);
}
/********************************************************************************/


