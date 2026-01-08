#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

extern int ECANA_MPR2NPR_NodeID;
extern int ECANA_CH2_NodeID;
/*****************************************************************************/
void InitECanaGpio(void)
{
	EALLOW;
	/*
	GpioCtrlRegs.GPACTRL.bit.QUALPRD3=0; //GPIO24~31,Sampling Period = TSYSCLKOUT   
	GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;// Asynch qual for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1; // Configure GPIO30 for CANRXA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1; // Configure GPIO31 for CANTXA operation
	GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;	 // Enable pull-up for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;	 // Enable pull-up for GPIO31 (CANTXA)
	EDIS;
	*/

	GpioCtrlRegs.GPACTRL.bit.QUALPRD3=0; //GPIO24~31,Sampling Period = TSYSCLKOUT   
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;// Asynch qual for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3; // Configure GPIO30 for CANRXA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 =3; // Configure GPIO31 for CANTXA operation
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	 // Enable pull-up for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	 // Enable pull-up for GPIO31 (CANTXA)

	EDIS;
}
/*********************************************************************************/
void InitECana(void)		// Initialize eCAN-A module
{
	struct ECAN_REGS ECanaShadow;
	//Create a shadow register structure for the CAN control registers  is must when bit operation
	//InitECanaGpio();
	EALLOW;

	ECanaRegs.CANTIOC.all=0x00000008;/*
	bit31~4,reserved
	bit3: TX-FUNC,1,enable
	bit2~0:reserved  */
	ECanaRegs.CANRIOC.all=0x00000008;/*
	bit31~4,reserved
	bit3: RX-FUNC,1,enable
	bit2~0:reserved  */

	//set CCR(change configuration request)
	ECanaRegs.CANMC.all=0x0001F6A0;/*0000,0000,0000,0001,1111,0110,1010,0000
	bit31~17:reserved,0000,0000,0000,000
	bit16:SUSPEND,1,free mode
	bit15:MBCC,Mailbox timestamp counter clear bit,1,reset to 0 
	bit14:TCC,Time stamp counter MSB clear bit,1,clear 0
	bit13:SCB,SCC compatibility bit,1,ecan mode
	bit12:CCR,1
	bit11:PDR,Power down mode request,0:no power down request
	bit10:DBO,Data byte order.1:least significant byte first
	bit9:WUBA Wake up on bus activity.1,leaves the power-down mode after detecting any bus activity.
	bit8:CDR,change data request.0,normal
	bit7:ABO,Auto bus on.1
	bit6:STM,Self test mode.0,narmal
	bit5:SRES,software reset.1,reset
	bit4~0:MBNR 4:0 Mailbox number,0
	*/
	do
	{
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	}	while(ECanaShadow.CANES.bit.CCE != 1); //read CCE(change configuration enable) status,and wait this bit enable

	ECanaShadow.CANBTC.all = 0;
#if(CPU_FRQ_150MHZ)   // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
	//The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
	ECanaShadow.CANBTC.bit.BRPREG = 19;
	ECanaShadow.CANBTC.bit.TSEG2REG = 2;
	ECanaShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
#endif
#if(CPU_FRQ_100MHZ)  // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	//The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	ECanaShadow.CANBTC.bit.BRPREG = 9;
	ECanaShadow.CANBTC.bit.TSEG2REG = 1;
	ECanaShadow.CANBTC.bit.TSEG1REG = 6;
#endif
	ECanaShadow.CANBTC.bit.SAM=1;  //three time sample
	ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;
  
	//scc mode only
	ECanaRegs.CANGAM.all=0;
	ECanaLAMRegs.LAM0.all =0;
	ECanaLAMRegs.LAM3.all =0;
  
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR=0;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	do
	{
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	}	while(ECanaShadow.CANES.bit.CCE==1); // Wait for CCE bit to be clear
	ECanaRegs.CANGIM.all=0x00000001;//enable Interrupt 0 
	ECanaRegs.CANMIM.all=0x00000010;//enable mailbox4 interrupt
	ECanaRegs.CANTSC=0;//Time-Stamp Counter Register
	EDIS;  

	ECanaRegs.CANME.all=0;//disable all mailbox 
	ECanaRegs.CANMD.all=0x000000F0;//0,send;1,receive
	ECanaRegs.CANTRS.all=0;
	ECanaRegs.CANTRR.all=0;//can not be 1
	ECanaRegs.CANTA.all=0xFFFFFFFF;	//Clear all TAn bits in Transmission-Acknowledge Register
	ECanaRegs.CANAA.all=0xFFFFFFFF;	//Clear all AAn bits in Abort-Acknowledge Register
	ECanaRegs.CANRMP.all=0xFFFFFFFF;	//Clear all pending bits in Received-Message-Pending Register
	ECanaRegs.CANRML.all=0xFFFFFFFF;	//Clear all receive lost bits 
	ECanaRegs.CANRFP.all=0xFFFFFFFF;	//Clear all Romate frame pending bits 
	ECanaRegs.CANES.all=0xFFFFFFFF;	//Clear all Romate frame pending bits in Error and Status Register
	ECanaRegs.CANTEC.all=0;	
	ECanaRegs.CANTEC.all=0;
	ECanaRegs.CANGIF0.all=0xFFFFFFFF;
	ECanaRegs.CANGIF1.all=0xFFFFFFFF;
	ECanaRegs.CANMIL.all=0;//all mailbox interrupt is generated on interrupt line 0
	ECanaRegs.CANOPC.all=0xFFFFFFFF;//all receivemailbox can't be overwriten
	ECanaRegs.CANTOC.all=0;//The time-out function is disabled
	ECanaRegs.CANTOS.all=0xFFFFFFFF;//clear all status
    
	//Mailbox 0
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR=1;
	ECanaShadow.CANMC.bit.MBNR=0;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	ECanaMboxes.MBOX0.MSGID.all=0;
	ECanaMboxes.MBOX0.MSGID.bit.STDMSGID=ECANA_MPR2NPR_NodeID;
	ECanaMboxes.MBOX0.MSGCTRL.all=0;
	ECanaMboxes.MBOX0.MSGCTRL.bit.TPL=31;
	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC=8;
	ECanaMboxes.MBOX0.MDH.all=0;
	ECanaMboxes.MBOX0.MDL.all=0;
	
	//Mailbox 4  
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR=1;
	ECanaShadow.CANMC.bit.MBNR=4;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	ECanaMboxes.MBOX4.MSGID.all=0;
	ECanaMboxes.MBOX4.MSGID.bit.AME=1;
	ECanaMboxes.MBOX4.MSGID.bit.STDMSGID=ECANA_MPR2NPR_NodeID;
	ECanaMboxes.MBOX4.MSGCTRL.all=0;
	ECanaMboxes.MBOX4.MSGCTRL.bit.TPL=27;
	ECanaMboxes.MBOX4.MSGCTRL.bit.DLC=8;
	ECanaMboxes.MBOX4.MDH.all=0;
	ECanaMboxes.MBOX4.MDL.all=0;
	ECanaLAMRegs.LAM4.bit.LAMI=0;
	ECanaLAMRegs.LAM4.bit.LAM_H=0x1E00;//1111,0000000,00
	ECanaLAMRegs.LAM4.bit.LAM_L=0;
	
	//Mailbox 5  
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR=1;
	ECanaShadow.CANMC.bit.MBNR=5;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	ECanaMboxes.MBOX5.MSGID.all=0;
	ECanaMboxes.MBOX5.MSGID.bit.AME=1;
	ECanaMboxes.MBOX5.MSGID.bit.STDMSGID=ECANA_CH2_NodeID;
	ECanaMboxes.MBOX5.MSGCTRL.all=0;
	ECanaMboxes.MBOX5.MSGCTRL.bit.TPL=26;
	ECanaMboxes.MBOX5.MSGCTRL.bit.DLC=8;
	ECanaMboxes.MBOX5.MDH.all=0;
	ECanaMboxes.MBOX5.MDL.all=0;
	ECanaLAMRegs.LAM5.bit.LAMI=0;
	ECanaLAMRegs.LAM5.bit.LAM_H=0x1E00;//1111,0000000,00
	ECanaLAMRegs.LAM5.bit.LAM_L=0;

	//CLEAR CDR
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR=0;
	ECanaShadow.CANMC.bit.MBNR=0;
	ECanaShadow.CANMC.bit.SRES=0;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	ECanaRegs.CANME.all=0x00000031;//enable Mailbox 0 and 4,5
}	
