/****************************************************************/
//28335 CANB,can 2.0A interface,Convertor outer communication
//modified by lvds,20120808
//Ver1.2
/****************************************************************/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Custom_ECanB.H"
void NODE_STATE_CH(void);
extern unsigned int CANBMB0PDOSendBuffer[8][6];
extern unsigned int CANBMB0PDOSendTeal;
unsigned int NMT_COMM=0, NMT_COMMOld=0;
extern int NMTTimeInterval;
/*****************************************************************/
// This function initializes the eCAN module to a known state.
/********************************************************************************/
struct 
{
	unsigned RepeatFlag:1;
	unsigned RepeatCount:8;	
	unsigned Busy:1;
	unsigned Unused:6;
	int RepeatIndex;
	int RepeatSubindex;
	int NodeID;
} CanBSendCtrl;

unsigned int CANBSendReset_1ms;
unsigned int CANBSendRepeat_1ms;
unsigned int CANBSendGP_1ms;
extern unsigned int MCS2MPR_NodeID;
extern int NODE_STATE;
extern int PDO_SEND_FLAG;
unsigned int NODE_ID;
/********************************************************************************/
void CANBVariableInit(void)
{
	CanBSendCtrl.RepeatFlag=0;
	CanBSendCtrl.RepeatCount=0;
	CanBSendCtrl.Busy=0;
	CanBSendCtrl.RepeatIndex=0;
	CanBSendCtrl.RepeatSubindex=0;
	CanBSendCtrl.NodeID=0;
	CANBSendReset_1ms=0;
	CANBSendRepeat_1ms=0;
	CANBSendGP_1ms=0;
}
/*******************************************************************************/
int CANBMB0HardwareSend(unsigned int PDOBuffer[][6],unsigned int PDOteal,unsigned int *PDOfront,unsigned int SDOBuffer[][6],unsigned int SDOteal,unsigned int *SDOfront,unsigned int Time_ms)
{
	struct ECAN_REGS ECanbShadow;
	int StatusFlag=0;	//1,send fault
	int SendFlag=0;		//1,send is necessary
	union
	{
		int CtrlWord;
		struct 
		{
			unsigned MailBoxID:11;
			unsigned RTR:1;
			unsigned unused:2;
			unsigned SendFlag:1;
			unsigned ACKFlag:1;
		}CtrlBit;
	}Ctrl;
	
	//send time of timeout 
	if(CANBSendGP_1ms != Time_ms)
	{
		CANBSendGP_1ms = Time_ms;
		CANBSendReset_1ms++;
		CANBSendRepeat_1ms++;
	}
	
	//busy timeout,Clear busy signal
	if(CanBSendCtrl.Busy == 0)
		CANBSendReset_1ms=0;
	if(CANBSendReset_1ms>=20)
	{
		CANBSendReset_1ms=0;
		CanBSendCtrl.Busy=0;
	}
	
	//Clear busy signal after complete send data
	ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
	if(ECanbShadow.CANTA.bit.TA0==1)
	{
		CanBSendCtrl.Busy=0;
		ECanbShadow.CANTA.bit.TA0=1;
		ECanbRegs.CANTA.all=ECanbShadow.CANTA.all;
	}
  
	//not busy,check data send
	if(CanBSendCtrl.Busy==0)
	{
		//PDO发送
		if((*PDOfront) >= 8)
			(*PDOfront) = 0;
		if(PDOteal != (*PDOfront))//send buffer is not full
		{
			//disable mailbox and ready for change
			ECanbShadow.CANME.all=ECanbRegs.CANME.all;
			ECanbShadow.CANME.bit.ME0=0;//disable mail
			ECanbRegs.CANME.all=ECanbShadow.CANME.all;
			ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
			ECanbShadow.CANMC.bit.CDR=1;
			ECanbShadow.CANMC.bit.MBNR=0;//Set CDR=1 of mailbox0
			ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;

			//Mailbox ID/RTR/Datalength
			Ctrl.CtrlWord=PDOBuffer[*PDOfront][0];
			ECanbMboxes.MBOX0.MSGID.all=0;
			ECanbMboxes.MBOX0.MSGID.bit.STDMSGID = Ctrl.CtrlBit.MailBoxID;
			ECanbMboxes.MBOX0.MSGCTRL.bit.RTR = Ctrl.CtrlBit.RTR;
			ECanbMboxes.MBOX0.MSGCTRL.bit.DLC=PDOBuffer[*PDOfront][1];

			if(Ctrl.CtrlBit.RTR==1)
				asm(" NOP");
			//valid data
			ECanbMboxes.MBOX0.MDL.word.LOW_WORD = PDOBuffer[*PDOfront][2];
			ECanbMboxes.MBOX0.MDL.word.HI_WORD = PDOBuffer[*PDOfront][3];
			ECanbMboxes.MBOX0.MDH.word.LOW_WORD = PDOBuffer[*PDOfront][4];
			ECanbMboxes.MBOX0.MDH.word.HI_WORD = PDOBuffer[*PDOfront][5];
			(*PDOfront)++;
			if((*PDOfront)>=8)
				(*PDOfront)=0;
			SendFlag=1;
		}
    
		//SDO发送
		if(SendFlag==0)
		{
			if((CanBSendCtrl.RepeatFlag==0)&&(CanBSendCtrl.RepeatCount!=0))
			{  //需要重发，对方有响应，重发结束
				CanBSendCtrl.RepeatCount=0;

				(*SDOfront)++;
				if((*SDOfront)>=8)
					(*SDOfront)=0;
			}

			if((CanBSendCtrl.RepeatFlag==0)&&(SDOteal!=(*SDOfront))||((CanBSendCtrl.RepeatFlag==1)&&(CANBSendRepeat_1ms>=3)))
			{  //不需要重发时检测队首队尾，需要重发时检测时间
				if(CanBSendCtrl.RepeatFlag==1)
				{//时间到重发
					CANBSendRepeat_1ms=0;
					CanBSendCtrl.RepeatCount++;
					if(CanBSendCtrl.RepeatCount>=3)
					{
						StatusFlag=1;
						CanBSendCtrl.RepeatFlag=0;
					}
				}
				//disable mailbox and ready for change
				ECanbShadow.CANME.all=ECanbRegs.CANME.all;
				ECanbShadow.CANME.bit.ME0=0;//disable mail
				ECanbRegs.CANME.all=ECanbShadow.CANME.all;
				ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
				ECanbShadow.CANMC.bit.CDR=1;
				ECanbShadow.CANMC.bit.MBNR=0;//Set CDR=1 of mailbox0
				ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;

				//Mailbox ID/RTR/Datalength
				Ctrl.CtrlWord=SDOBuffer[*SDOfront][0];
				ECanbMboxes.MBOX0.MSGID.all=0;
				ECanbMboxes.MBOX0.MSGID.bit.STDMSGID=Ctrl.CtrlBit.MailBoxID;
				ECanbMboxes.MBOX0.MSGCTRL.bit.RTR=Ctrl.CtrlBit.RTR;
				ECanbMboxes.MBOX0.MSGCTRL.bit.DLC=SDOBuffer[*SDOfront][1];

				if(Ctrl.CtrlBit.RTR==1)
					asm(" NOP");

				//valid data
				ECanbMboxes.MBOX0.MDL.word.LOW_WORD=SDOBuffer[*SDOfront][2];
				ECanbMboxes.MBOX0.MDL.word.HI_WORD=SDOBuffer[*SDOfront][3];
				ECanbMboxes.MBOX0.MDH.word.LOW_WORD=SDOBuffer[*SDOfront][4];
				ECanbMboxes.MBOX0.MDH.word.HI_WORD=SDOBuffer[*SDOfront][5];

				//首次发送检测，SDO request ack check
				if(CanBSendCtrl.RepeatCount==0)
				{
					if(Ctrl.CtrlBit.ACKFlag==1)
					{
						CanBSendCtrl.RepeatFlag=1;
						CanBSendCtrl.RepeatCount=1;
						CANBSendRepeat_1ms=0;
						CanBSendCtrl.RepeatIndex=(SDOBuffer[*SDOfront][2]>>8)&0x00FF;
						CanBSendCtrl.RepeatIndex|=(SDOBuffer[*SDOfront][3]<<8)&0xFF00;
						CanBSendCtrl.RepeatSubindex=(SDOBuffer[*SDOfront][3]>>8)&0x00FF;
						CanBSendCtrl.NodeID=Ctrl.CtrlBit.MailBoxID&0x007F;
					}
					else
					{
						(*SDOfront)++;
						if((*SDOfront)>=8)
							(*SDOfront)=0;
					}
				}
				SendFlag=1;
			}
		}
	}

	//send data from mailbox to PIN->TXD
	if(SendFlag==1)
	{
		CanBSendCtrl.Busy=1;
		ECanbShadow.CANMC.all=ECanbRegs.CANMC.all;
		ECanbShadow.CANMC.bit.CDR=0;
		ECanbShadow.CANMC.bit.MBNR=0;
		ECanbRegs.CANMC.all=ECanbShadow.CANMC.all;//Set CDR=0 of mailbox0

		ECanbShadow.CANME.all=ECanbRegs.CANME.all;
		ECanbShadow.CANME.bit.ME0=1;//enable mail0
		ECanbRegs.CANME.all=ECanbShadow.CANME.all;
		ECanbShadow.CANTRS.all=ECanbRegs.CANTRS.all;
		ECanbShadow.CANTRS.bit.TRS0=1;//enable send request
		ECanbRegs.CANTRS.all=ECanbShadow.CANTRS.all;
	}
	return StatusFlag;
}
/********************************************************************************/
void CANBMB4HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal)
{
	struct ECAN_REGS ECanbShadow;
	union
	{
		int CtrlWord;
		struct
		{
			unsigned MailBoxID:11;
			unsigned RTR:1;
			unsigned unused:2;
			unsigned SendFlag:1;
			unsigned ACKFlag:1;
		}CtrlBit;
	}CanCtrl;
  
	ECanbShadow.CANRMP.all=ECanbRegs.CANRMP.all;
	ECanbShadow.CANRMP.bit.RMP4=1;
	ECanbRegs.CANRMP.all=ECanbShadow.CANRMP.all;
	ECanbShadow.CANRFP.all=ECanbRegs.CANRFP.all;
	ECanbShadow.CANRFP.bit.RFP4=1;
	ECanbRegs.CANRFP.all=ECanbShadow.CANRFP.all;

	CanCtrl.CtrlWord=0;
	CanCtrl.CtrlBit.MailBoxID =ECanbMboxes.MBOX4.MSGID.bit.STDMSGID;
	CanCtrl.CtrlBit.RTR=ECanbMboxes.MBOX4.MSGCTRL.bit.RTR;
	Buffer[*Teal][0]=CanCtrl.CtrlWord;
	if(CanCtrl.CtrlBit.RTR==0)
	{
		Buffer[*Teal][1]=ECanbMboxes.MBOX4.MSGCTRL.bit.DLC;
		Buffer[*Teal][2]=ECanbMboxes.MBOX4.MDL.word.LOW_WORD;
		Buffer[*Teal][3]=ECanbMboxes.MBOX4.MDL.word.HI_WORD;
		Buffer[*Teal][4]=ECanbMboxes.MBOX4.MDH.word.LOW_WORD;
		Buffer[*Teal][5]=ECanbMboxes.MBOX4.MDH.word.HI_WORD;

		if((((Buffer[*Teal][3]<<8)&0xFF00)|((Buffer[*Teal][2]>>8)&0x00FF))==CanBSendCtrl.RepeatIndex)  // 3低 2高
		if(((Buffer[*Teal][3]>>8)&0x00FF)==CanBSendCtrl.RepeatSubindex)  // 3高8
		if(CanBSendCtrl.NodeID==(CanCtrl.CtrlBit.MailBoxID&0x007F))
			CanBSendCtrl.RepeatFlag=0;
	}
	else
	{
		Buffer[*Teal][1]=0;
		Buffer[*Teal][2]=0;
		Buffer[*Teal][3]=0;
		Buffer[*Teal][4]=0;
		Buffer[*Teal][5]=0;
	}

	if(CanCtrl.CtrlBit.MailBoxID==0)
	{
		NODE_ID = ((Buffer[*Teal][2]&0xFF00)>>8); // 高8  桥站号
		NMT_COMM = (Buffer[*Teal][2]&0xFF);    // 低8 NMT命令
		NODE_STATE_CH();
	}
	else if (CanCtrl.CtrlBit.MailBoxID==0x80)
	{
		PDO_SEND_FLAG=1;
	}
	else;
		(*Teal)++;
	if((*Teal)>=8)
		*Teal=0;
}
/**************************************************************************/
void CANBMB5HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal)
{
	struct ECAN_REGS ECanbShadow;
	union
	{
		int CtrlWord;
		struct
		{
			unsigned MailBoxID:11;
			unsigned RTR:1;
			unsigned unused:2;
			unsigned SendFlag:1;
			unsigned ACKFlag:1;
		}CtrlBit;
	}CanCtrl;

	ECanbShadow.CANRMP.all=ECanbRegs.CANRMP.all;
	ECanbShadow.CANRMP.bit.RMP5=1;
	ECanbRegs.CANRMP.all=ECanbShadow.CANRMP.all;
	ECanbShadow.CANRFP.all=ECanbRegs.CANRFP.all;
	ECanbShadow.CANRFP.bit.RFP5=1;
	ECanbRegs.CANRFP.all=ECanbShadow.CANRFP.all;

	CanCtrl.CtrlWord=0;
	CanCtrl.CtrlBit.MailBoxID =ECanbMboxes.MBOX5.MSGID.bit.STDMSGID;
	CanCtrl.CtrlBit.RTR=ECanbMboxes.MBOX5.MSGCTRL.bit.RTR;
	Buffer[*Teal][0]=CanCtrl.CtrlWord;
	if(CanCtrl.CtrlBit.RTR==0)
	{
		Buffer[*Teal][1] = ECanbMboxes.MBOX5.MSGCTRL.bit.DLC;
		Buffer[*Teal][2] = ECanbMboxes.MBOX5.MDL.word.LOW_WORD;
		Buffer[*Teal][3] = ECanbMboxes.MBOX5.MDL.word.HI_WORD;
		Buffer[*Teal][4] = ECanbMboxes.MBOX5.MDH.word.LOW_WORD;
		Buffer[*Teal][5] = ECanbMboxes.MBOX5.MDH.word.HI_WORD;

		if((((Buffer[*Teal][3]<<8)&0xFF00)|((Buffer[*Teal][2]>>8)&0x00FF))==CanBSendCtrl.RepeatIndex)
		if(((Buffer[*Teal][3]>>8)&0x00FF)==CanBSendCtrl.RepeatSubindex)
		if(CanBSendCtrl.NodeID == (CanCtrl.CtrlBit.MailBoxID&0x007F))
			CanBSendCtrl.RepeatFlag=0;
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
	if((*Teal)>=8)
		*Teal=0;
}
/////////////////////////////////////////////////////////////////////////////
void NODE_STATE_CH(void)
{
	if((NODE_ID==0)||(MCS2MPR_NodeID==NODE_ID))//0对所有节点
	{
		if (NMT_COMM==1)
		{
			NODE_STATE&=0x80;
			NODE_STATE|=0x05;
		}
		else if (NMT_COMM==2)
		{
			NODE_STATE&=0x80;
			NODE_STATE|=0x04;
		}
		else if ((NMT_COMM==0x81)||(NMT_COMM==0x82))
		{
			if( (NMT_COMMOld!=NMT_COMM) || (NMTTimeInterval>10) )
			{
				NODE_STATE=0;
				MCS2MPR_ProtocalSend_Status(CANBMB0PDOSendBuffer, &CANBMB0PDOSendTeal);
				NMTTimeInterval=0;
			}
		}
		else if (NMT_COMM==0x80)
		{
			NODE_STATE&=0x80;
			NODE_STATE|=0x7F;
		}
		NMT_COMMOld = NMT_COMM;
	}
}

