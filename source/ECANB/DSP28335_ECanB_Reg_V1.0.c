/****************************************************************/
//28335 CANB,can 2.0A interface,Convertor outer communication
//modified by lvds,20120808
//Ver1.2
/****************************************************************/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

/*****************************************************************/
// This function initializes the eCAN module to a known state.
/********************************************************************************/
extern unsigned int MCS2MPR_NodeID;
extern unsigned int  BaundNum;
extern void CANBVariableInit(void);
/*****************************************************************************/
void InitECanbGpio(void)
{
	EALLOW;
	GpioCtrlRegs.GPACTRL.bit.QUALPRD2=0; //GPIO24~31,Sampling Period = TSYSCLKOUT
	GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;// Asynch qual for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2; // Configure GPIO30 for CANRXB operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO16 =2; // Configure GPIO31 for CANTXB operation
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;	 // Enable pull-up for GPIO30 (CANRXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;	 // Enable pull-up for GPIO31 (CANTXA)
	EDIS;
}
/*********************************************************************************/
void InitECanb(void)		// Initialize eCAN-B module
{
  struct ECAN_REGS ECanbShadow;
  //Create a shadow register structure for the CAN control registers  is must when bit operation
	//InitECanbGpio();
	CANBVariableInit();
	EALLOW;		
  
  ECanbRegs.CANTIOC.all=0x00000008;/*
    bit31~4,reserved
    bit3: TX-FUNC,1,enable
    bit2~0:reserved  */
  ECanbRegs.CANRIOC.all=0x00000008;/*
    bit31~4,reserved
    bit3: RX-FUNC,1,enable
    bit2~0:reserved  */
  
  //set CCR(change configuration request)
  ECanbRegs.CANMC.all=0x0001F6A0;/*0000,0000,0000,0001,1111,0110,1010,0000
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
	  ECanbShadow.CANES.all = ECanbRegs.CANES.all;
  } while(ECanbShadow.CANES.bit.CCE != 1); //read CCE(change configuration enable) status,and wait this bit enable
  
  ECanbShadow.CANBTC.all = 0;
/*#if(CPU_FRQ_150MHZ)   // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
	//The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
	ECanbShadow.CANBTC.bit.BRPREG = 9;
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
#endif*/
#if(CPU_FRQ_150MHZ)   // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
	//The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
	 switch(BaundNum)
	  {
	    case 1://1M
		ECanbShadow.CANBTC.bit.BRPREG = 14;
		ECanbShadow.CANBTC.bit.TSEG2REG = 1;
		ECanbShadow.CANBTC.bit.TSEG1REG = 1;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
		case 2://500K
		ECanbShadow.CANBTC.bit.BRPREG = 9;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
		case 3://250K
		ECanbShadow.CANBTC.bit.BRPREG = 19;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
		case 4://125K
		ECanbShadow.CANBTC.bit.BRPREG = 39;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
		case 5://50K
		ECanbShadow.CANBTC.bit.BRPREG = 99;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
		default:
		ECanbShadow.CANBTC.bit.BRPREG = 9;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;//BAUT=75M/[(BRPREG+1)(TSEG2REG+1+TSEG1REG+1+1)]
		break;
	  }
#endif
#if(CPU_FRQ_100MHZ)  // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	//The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	ECanbShadow.CANBTC.bit.BRPREG = 9;
	ECanbShadow.CANBTC.bit.TSEG2REG = 1;
	ECanbShadow.CANBTC.bit.TSEG1REG = 6;
#endif
  ECanbShadow.CANBTC.bit.SAM=1;  //three time sample
  ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;
  
  //scc mode only
  ECanbRegs.CANGAM.all=0;
  ECanbLAMRegs.LAM0.all =0;
	ECanbLAMRegs.LAM3.all =0;
  
  
  ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
  ECanbShadow.CANMC.bit.CCR=0;
  ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;
  do
	{
	  ECanbShadow.CANES.all = ECanbRegs.CANES.all;
  } while(ECanbShadow.CANES.bit.CCE==1); // Wait for CCE bit to be clear
  ECanbRegs.CANGIM.all=0x00000001;//enable Interrupt 0 
  ECanbRegs.CANMIM.all=0x00000030;//enable mailbox5/4 interrupt
  ECanbRegs.CANTSC=0;//Time-Stamp Counter Register
  EDIS;  
  
	ECanbRegs.CANME.all=0;//disable all mailbox
	ECanbRegs.CANMD.all=0x000000F0;//0,send;1,receive
	ECanbRegs.CANTRS.all=0;
	ECanbRegs.CANTRR.all=0;//can not be 1
	ECanbRegs.CANTA.all=0xFFFFFFFF;	//Clear all TAn bits in Transmission-Acknowledge Register
	ECanbRegs.CANAA.all=0xFFFFFFFF;	//Clear all AAn bits in Abort-Acknowledge Register
	ECanbRegs.CANRMP.all=0xFFFFFFFF;	//Clear all pending bits in Received-Message-Pending Register
	ECanbRegs.CANRML.all=0xFFFFFFFF;	//Clear all receive lost bits
	ECanbRegs.CANRFP.all=0xFFFFFFFF;	//Clear all Romate frame pending bits
	ECanbRegs.CANES.all=0xFFFFFFFF;	//Clear all Romate frame pending bits in Error and Status Register
	ECanbRegs.CANTEC.all=0;
	ECanbRegs.CANTEC.all=0;
	ECanbRegs.CANGIF0.all=0xFFFFFFFF;
	ECanbRegs.CANGIF1.all=0xFFFFFFFF;
	ECanbRegs.CANMIL.all=0;//all mailbox interrupt is generated on interrupt line 0
	ECanbRegs.CANOPC.all=0xFFFFFFFF;//all receivemailbox can't be overwriten
	ECanbRegs.CANTOC.all=0;//The time-out function is disabled
	ECanbRegs.CANTOS.all=0xFFFFFFFF;//clear all status
    
	//Mailbox 0
	ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CDR=1;
	ECanbShadow.CANMC.bit.MBNR=0;
	ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;
	ECanbMboxes.MBOX0.MSGID.all=0;
	ECanbMboxes.MBOX0.MSGID.bit.STDMSGID=MCS2MPR_NodeID;
	ECanbMboxes.MBOX0.MSGCTRL.all=0;
	ECanbMboxes.MBOX0.MSGCTRL.bit.TPL=31;
	ECanbMboxes.MBOX0.MSGCTRL.bit.DLC=8;
	ECanbMboxes.MBOX0.MDH.all=0;
	ECanbMboxes.MBOX0.MDL.all=0;
	
	//Mailbox 4,NMT receive
	ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CDR=1;
	ECanbShadow.CANMC.bit.MBNR=4;
	ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;
	ECanbMboxes.MBOX4.MSGID.all=0;
	ECanbMboxes.MBOX4.MSGID.bit.AME=1;
	ECanbMboxes.MBOX4.MSGID.bit.STDMSGID=0;//ԭ4ΪMPR2EPS_NodeID
	ECanbMboxes.MBOX4.MSGCTRL.all=0;
	ECanbMboxes.MBOX4.MSGCTRL.bit.TPL=27;
	ECanbMboxes.MBOX4.MSGCTRL.bit.DLC=8;
	ECanbMboxes.MBOX4.MDH.all=0;
	ECanbMboxes.MBOX4.MDL.all=0;
	ECanbLAMRegs.LAM4.bit.LAMI=0;
	ECanbLAMRegs.LAM4.bit.LAM_H=0x1E00;//1111,0000000,00
	ECanbLAMRegs.LAM4.bit.LAM_L=0;
	
	//Mailbox 5  
	ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CDR=1;
	ECanbShadow.CANMC.bit.MBNR=5;
	ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;
	ECanbMboxes.MBOX5.MSGID.all=0;
	ECanbMboxes.MBOX5.MSGID.bit.AME=1;
	ECanbMboxes.MBOX5.MSGID.bit.STDMSGID=MCS2MPR_NodeID;
	ECanbMboxes.MBOX5.MSGCTRL.all=0;
	ECanbMboxes.MBOX5.MSGCTRL.bit.TPL=26;
	ECanbMboxes.MBOX5.MSGCTRL.bit.DLC=8;
	ECanbMboxes.MBOX5.MDH.all=0;
	ECanbMboxes.MBOX5.MDL.all=0;
	ECanbLAMRegs.LAM5.bit.LAMI=0;
	ECanbLAMRegs.LAM5.bit.LAM_H=0x1E00;//1111,0000000,00
	ECanbLAMRegs.LAM5.bit.LAM_L=0;
	
	//CLEAR CDR
	ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CDR=0;
	ECanbShadow.CANMC.bit.MBNR=0;
	ECanbShadow.CANMC.bit.SRES=0;
	ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;
	ECanbRegs.CANME.all=0x00000031;//enable Mailbox 0 and 4/5
}	
/*********************************************************************************/



