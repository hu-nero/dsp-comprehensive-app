/****************************************************************/
//28335 CANA,can 2.0A interface,Convertor inner communication
//modified by lvds,20120808
//Ver1.1
/****************************************************************/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Pub_ECAN_Macro.H"
/********************************************************************************/
unsigned int CANA_Send_Busy=0;
unsigned int CANASendReset_1ms=0;
unsigned int CANASendGP_1ms=0;
extern u16 RemodCmd_Updata;
/*******************************************************************************/
void CANAMB0HardwareSend(unsigned int Buffer[][6],unsigned int teal,unsigned int *front,int NumMax,unsigned int Time_ms)
{
	struct ECAN_REGS ECanaShadow;
	union CANSendCtrl Ctrl;
	
	//send time of timeout 
	if(CANASendGP_1ms != Time_ms)
	{
		CANASendGP_1ms = Time_ms;
		CANASendReset_1ms++;
	}
	
	//busy timeout,Clear busy signal
	if(CANA_Send_Busy==0)
		CANASendReset_1ms=0;
	if(CANASendReset_1ms>=20)
	{
		CANASendReset_1ms=0;
		CANA_Send_Busy=0;
	}
	
	//Clear busy signal after complete send data
	ECanaShadow.CANTA.all=ECanaRegs.CANTA.all;
	if(ECanaShadow.CANTA.bit.TA0==1)
	{
		CANA_Send_Busy=0;
		ECanaShadow.CANTA.bit.TA0=1;
		ECanaRegs.CANTA.all=ECanaShadow.CANTA.all;
	}
  
  //not busy,check data send
	if(CANA_Send_Busy==0)
	{
    if((*front)>=NumMax)
      (*front)=0;
    if(teal!=(*front))//send buffer is not full
    {
      //disable mailbox and ready for change
      ECanaShadow.CANME.all=ECanaRegs.CANME.all;
      ECanaShadow.CANME.bit.ME0=0;//disable mail
      ECanaRegs.CANME.all=ECanaShadow.CANME.all;
      ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
      ECanaShadow.CANMC.bit.CDR=1;
      ECanaShadow.CANMC.bit.MBNR=0;//Set CDR=1 of mailbox0
      ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;

      //Mailbox ID/RTR/Datalength
      Ctrl.CtrlWord=Buffer[*front][0];
      ECanaMboxes.MBOX0.MSGID.all=0;
      ECanaMboxes.MBOX0.MSGID.bit.STDMSGID=Ctrl.CtrlBit.MailID;
      ECanaMboxes.MBOX0.MSGCTRL.bit.RTR=Ctrl.CtrlBit.RTR;
      ECanaMboxes.MBOX0.MSGCTRL.bit.DLC=Buffer[*front][1];

      //valid data
      ECanaMboxes.MBOX0.MDL.word.LOW_WORD=Buffer[*front][2];
      ECanaMboxes.MBOX0.MDL.word.HI_WORD=Buffer[*front][3];
      ECanaMboxes.MBOX0.MDH.word.LOW_WORD=Buffer[*front][4];
      ECanaMboxes.MBOX0.MDH.word.HI_WORD=Buffer[*front][5];

      (*front)++;
      if((*front)>=NumMax)
        (*front)=0;
        
        
      CANA_Send_Busy=1;
      ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
      ECanaShadow.CANMC.bit.CDR=0;
      ECanaShadow.CANMC.bit.MBNR=0;
      ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;//Set CDR=0 of mailbox0

      ECanaShadow.CANME.all=ECanaRegs.CANME.all;
      ECanaShadow.CANME.bit.ME0=1;//enable mail0
      ECanaRegs.CANME.all=ECanaShadow.CANME.all;
      ECanaShadow.CANTRS.all=ECanaRegs.CANTRS.all;
      ECanaShadow.CANTRS.bit.TRS0=1;//enable send request
      ECanaRegs.CANTRS.all=ECanaShadow.CANTRS.all;  
    }
  }
}
/********************************************************************************/
void CANAMB4HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
	u16 RemID, RemMes;
	struct ECAN_REGS ECanaShadow;
	union CANSendCtrl CanCtrl;

	ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
	ECanaShadow.CANRMP.bit.RMP4=1;
	ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
	ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
	ECanaShadow.CANRFP.bit.RFP4=1;
	ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

	CanCtrl.CtrlWord=0;
	CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX4.MSGID.bit.STDMSGID;
	CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX4.MSGCTRL.bit.RTR;
	Buffer[*Teal][0]=CanCtrl.CtrlWord;
	if(CanCtrl.CtrlBit.RTR==0)
	{
		Buffer[*Teal][1]=ECanaMboxes.MBOX4.MSGCTRL.bit.DLC;
		Buffer[*Teal][2]=ECanaMboxes.MBOX4.MDL.word.LOW_WORD;
		Buffer[*Teal][3]=ECanaMboxes.MBOX4.MDL.word.HI_WORD;
		Buffer[*Teal][4]=ECanaMboxes.MBOX4.MDH.word.LOW_WORD;
		Buffer[*Teal][5]=ECanaMboxes.MBOX4.MDH.word.HI_WORD;
	}
	else
	{
		Buffer[*Teal][1]=0;
		Buffer[*Teal][2]=0;
		Buffer[*Teal][3]=0;
		Buffer[*Teal][4]=0;
		Buffer[*Teal][5]=0;
	}

	// add by RemCANsend Ctrl cmd
	RemID = ECanaMboxes.MBOX4.MSGID.bit.STDMSGID;
	RemMes = ECanaMboxes.MBOX4.MDL.word.LOW_WORD;

	if( (RemID == 0x301) && (RemMes == 0x1002) )
	{
//		RemodCmd_Updata = 1;
	}

	(*Teal)++;
	if((*Teal)>=NumMax)
		*Teal=0;
}
/********************************************************************************/
void CANAMB5HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP5=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP5=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX5.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX5.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX5.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX5.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX5.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX5.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX5.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/
void CANAMB6HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP6=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP6=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX6.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX6.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX6.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX6.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX6.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX6.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX6.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/
void CANAMB7HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP7=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP7=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX7.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX7.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX7.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX7.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX7.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX7.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX7.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/
void CANAMB8HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP8=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP8=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX8.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX8.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX8.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX8.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX8.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX8.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX8.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/
void CANAMB9HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP9=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP9=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX9.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX9.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX9.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX9.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX9.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX9.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX9.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/
void CANAMB10HardwareRecv(int Buffer[][6],int *Teal,int NumMax)
{
  struct ECAN_REGS ECanaShadow;
  union CANSendCtrl CanCtrl;

 
  ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
  ECanaShadow.CANRMP.bit.RMP10=1;
  ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;
  ECanaShadow.CANRFP.all=ECanaRegs.CANRFP.all;
  ECanaShadow.CANRFP.bit.RFP10=1;
  ECanaRegs.CANRFP.all=ECanaShadow.CANRFP.all;

  CanCtrl.CtrlWord=0;
  CanCtrl.CtrlBit.MailID =ECanaMboxes.MBOX10.MSGID.bit.STDMSGID;
  CanCtrl.CtrlBit.RTR=ECanaMboxes.MBOX10.MSGCTRL.bit.RTR;
  Buffer[*Teal][0]=CanCtrl.CtrlWord;
  if(CanCtrl.CtrlBit.RTR==0)
  {
    Buffer[*Teal][1]=ECanaMboxes.MBOX10.MSGCTRL.bit.DLC;
    Buffer[*Teal][2]=ECanaMboxes.MBOX10.MDL.word.LOW_WORD;
    Buffer[*Teal][3]=ECanaMboxes.MBOX10.MDL.word.HI_WORD; 
    Buffer[*Teal][4]=ECanaMboxes.MBOX10.MDH.word.LOW_WORD;
    Buffer[*Teal][5]=ECanaMboxes.MBOX10.MDH.word.HI_WORD;
  }
  else
  {
    Buffer[*Teal][1]=0;
    Buffer[*Teal][2]=0;
    Buffer[*Teal][3]=0; 
    Buffer[*Teal][4]=0;
    Buffer[*Teal][5]=0;
  }
  (*Teal)++;
  if((*Teal)>=NumMax)
    *Teal=0;
}
/********************************************************************************/




