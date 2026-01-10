#include "ECANB_Macro.H"
#include "Custom_ECanB.H"
////////////////////////////////////
extern unsigned int MCS2MPR_NodeID;
extern int MCS2MPR_PDO1TimeInterval;
extern int MCS2MPR_PDO2TimeInterval;
extern int MCS2MPR_PDO3TimeInterval;
extern int MCS2MPR_PDO4TimeInterval;
extern int MCS2MPR_SDOTimeInterval;
extern int NODE_STATE;
extern int PDO_SEND_FLAG;
extern const struct CANPDO MCS2MPR_RPDOObj[]; 
extern const struct CANPDO MCS2MPR_TPDOObj[];
extern const struct SDOSummaryType MCS2MPR_CANSDOSummary[];
////////////////////////////////////
int MCS2MPRSend_1msOld=0;
int MCS2MPRSend_1msOld_forNode=0;
unsigned int MCS2MPR_SDOCycleSendIndexCount=1;
unsigned int MCS2MPR_SDOCycleSendSubindexCount=1;
/******************************************************************************/
void MCS2MPR_ProtocalSend_Master(unsigned int PDOBuffer[][6],unsigned int *PDOTeal,unsigned int SDOBuffer[][6],unsigned int *SDOTeal,unsigned int Send_1ms)
{
	int i;
	union CANSendCtrl  Ctrl;
	int flag=0;
	//PDO Send
	if(MCS2MPRSend_1msOld!=Send_1ms)
	{
		MCS2MPRSend_1msOld=Send_1ms;
		for(i=0;i<4;i++)
		{
			if(MCS2MPR_RPDOObj[i].Ctrl.CtrlBit.SendFlag==1)
			if(Send_1ms%(*MCS2MPR_RPDOObj[i].TimeInterval)==0)
			{
				Ctrl.CtrlWord=MCS2MPR_RPDOObj[i].Ctrl.CtrlWord;
				Ctrl.CtrlBit.SendFlag=0;
				Ctrl.CtrlBit.FunCode|=MCS2MPR_NodeID;
				PDOBuffer[*PDOTeal][0]=Ctrl.CtrlWord;
				PDOBuffer[*PDOTeal][1]=MCS2MPR_RPDOObj[i].DataLength;
				PDOBuffer[*PDOTeal][2]=*(unsigned int *)MCS2MPR_RPDOObj[i].PDataBuff[0];
				PDOBuffer[*PDOTeal][3]=*(unsigned int *)MCS2MPR_RPDOObj[i].PDataBuff[1];
				PDOBuffer[*PDOTeal][4]=*(unsigned int *)MCS2MPR_RPDOObj[i].PDataBuff[2];
				PDOBuffer[*PDOTeal][5]=*(unsigned int *)MCS2MPR_RPDOObj[i].PDataBuff[3];
				(*PDOTeal)++;
				if((*PDOTeal)>=8)
					(*PDOTeal)=0;
			}
		}
		if((Send_1ms%MCS2MPR_SDOTimeInterval)==0)
		{
			//SDO Cycle send
			if(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaBit.CycleAccessFlag!=0)
			{
				SDOBuffer[*SDOTeal][0]=(RSDO+MCS2MPR_NodeID)|(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaWord&0x8000);
				SDOBuffer[*SDOTeal][1]=MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].DataLength;
				SDOBuffer[*SDOTeal][2]=(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Index<<8)&0xFF00;
				SDOBuffer[*SDOTeal][3]=(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Subindex<<8)&0xFF00;
				SDOBuffer[*SDOTeal][3]|=(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Index>>8)&0x00FF;
			}
			if(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaBit.CycleAccessFlag==1)//read
			{
				SDOBuffer[*SDOTeal][2]|=0x0040;
				SDOBuffer[*SDOTeal][4]=0;
				SDOBuffer[*SDOTeal][5]=0;
				flag=1;
			}
      
			if(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaBit.CycleAccessFlag==2)//write
			{
				switch(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaBit.ByteBum)
				{
					case 0x00:
					{
						SDOBuffer[*SDOTeal][2]|=0x0023;
						SDOBuffer[*SDOTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object;
						SDOBuffer[*SDOTeal][5]=*((unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object+1);
						flag=2;
						break;
					}
					case 0x01:
					{
						SDOBuffer[*SDOTeal][2]|=0x0027;
						SDOBuffer[*SDOTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object;
						SDOBuffer[*SDOTeal][5]=*((unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object+1)&0x00FF;
						flag=2;
						break;
					}
					case 0x02:
					{
						SDOBuffer[*SDOTeal][2]|=0x002B;
						SDOBuffer[*SDOTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object;
						SDOBuffer[*SDOTeal][5]=0;
						flag=2;
						break;
					}
					case 0x03:
					{
						SDOBuffer[*SDOTeal][2]|=0x002F;
						SDOBuffer[*SDOTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Object&0x00FF;
						SDOBuffer[*SDOTeal][5]=0;
						flag=2;
						break;
					}
					default:break;
				}
			}
			if(flag!=0)
			{
				(*SDOTeal)++;
				if((*SDOTeal)>=8)
					(*SDOTeal)=0;
				MCS2MPR_SDOCycleSendSubindexCount++;
				if(MCS2MPR_SDOCycleSendSubindexCount>MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[0].Subindex)
				{
					MCS2MPR_SDOCycleSendSubindexCount=1;
					MCS2MPR_SDOCycleSendIndexCount++;
					if(MCS2MPR_SDOCycleSendIndexCount>MCS2MPR_CANSDOSummary[0].Index)
						MCS2MPR_SDOCycleSendIndexCount=1;
				}
			}
		}
	}
	if(MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[MCS2MPR_SDOCycleSendSubindexCount].Para.ParaBit.CycleAccessFlag==0)
	{
		MCS2MPR_SDOCycleSendSubindexCount++;
		if(MCS2MPR_SDOCycleSendSubindexCount>MCS2MPR_CANSDOSummary[MCS2MPR_SDOCycleSendIndexCount].Obj[0].Subindex)
		{
			MCS2MPR_SDOCycleSendSubindexCount=1;
			MCS2MPR_SDOCycleSendIndexCount++;
			if(MCS2MPR_SDOCycleSendIndexCount>MCS2MPR_CANSDOSummary[0].Index)
				MCS2MPR_SDOCycleSendIndexCount=1;
		}
	}
}
/******************************************************************************/
void MCS2MPR_ProtocalSend_Slaver(unsigned int Buffer[][6],unsigned int *Teal,unsigned int Send_1ms,unsigned int Permit)
{
	int i;
	union CANSendCtrl Ctrl;
	//PDO Send
	if(MCS2MPRSend_1msOld != Send_1ms)
	{
		MCS2MPRSend_1msOld = Send_1ms;
		for(i=0; i<4; i++)
		{
			if(MCS2MPR_TPDOObj[i].Ctrl.CtrlBit.SendFlag == 1)
			if((Send_1ms % (*MCS2MPR_TPDOObj[i].TimeInterval) == 0) ||
				( (Permit==1) && (PDO_SEND_FLAG==1) ) )
			{
				Ctrl.CtrlWord = MCS2MPR_TPDOObj[i].Ctrl.CtrlWord;
				Ctrl.CtrlBit.SendFlag = 0;
				Ctrl.CtrlBit.FunCode |= MCS2MPR_NodeID;
				Buffer[*Teal][0] = Ctrl.CtrlWord;
				Buffer[*Teal][1] = MCS2MPR_TPDOObj[i].DataLength;
				Buffer[*Teal][2] = *MCS2MPR_TPDOObj[i].PDataBuff[0];
				Buffer[*Teal][3] = *MCS2MPR_TPDOObj[i].PDataBuff[1];
				Buffer[*Teal][4] = *MCS2MPR_TPDOObj[i].PDataBuff[2];
				Buffer[*Teal][5] = *MCS2MPR_TPDOObj[i].PDataBuff[3];
				(*Teal)++;
				if((*Teal)>=8)
					(*Teal)=0;
			}
		}
		PDO_SEND_FLAG=0;
	}
}
/********************************************************************************/
void MCS2MPR_ProtocalSend_Status(unsigned int Buffer[][6],unsigned int *Teal)
{
	union CANSendCtrl Ctrl;
	Ctrl.CtrlWord=0;
	Ctrl.CtrlBit.FunCode = 0x700;
	Ctrl.CtrlBit.FunCode |= MCS2MPR_NodeID; // 1

	Buffer[*Teal][0] = Ctrl.CtrlWord;
	Buffer[*Teal][1] = 1;
	Buffer[*Teal][2] = NODE_STATE&0x7F;
	Buffer[*Teal][3] = 0;
	Buffer[*Teal][4] = 0;
	Buffer[*Teal][5] = 0;
	(*Teal)++;
	if((*Teal)>=8)
	(*Teal)=0;
}
/*********************************************************************************/
void MCS2MPR_ProtocalRecv_Master(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal)
{
	int i,j;
	int Command,Index,Subindex,FunCode;
	union CANSendCtrl  Ctrl;
	if((*Front)!=Teal)//接收缓冲区
	{
		Ctrl.CtrlWord=Buffer[*Front][0];
		FunCode=Ctrl.CtrlBit.FunCode&0x0780;
		switch(FunCode)
		{
			case TPDO1:
			case TPDO2:
			case TPDO3:
			case TPDO4:
			{
				for(i=0;i<4;i++)
				{
					if(MCS2MPR_TPDOObj[i].Ctrl.CtrlBit.FunCode==FunCode)
					if(MCS2MPR_TPDOObj[i].DataLength==Buffer[*Front][1])
					{
						*(unsigned int *)MCS2MPR_TPDOObj[i].PDataBuff[0]=Buffer[*Front][2];
						*(unsigned int *)MCS2MPR_TPDOObj[i].PDataBuff[1]=Buffer[*Front][3];
						*(unsigned int *)MCS2MPR_TPDOObj[i].PDataBuff[2]=Buffer[*Front][4];
						*(unsigned int *)MCS2MPR_TPDOObj[i].PDataBuff[3]=Buffer[*Front][5];
					}
				}
				break;
			}
			case TSDO:
			{
				Command = Buffer[*Front][2]&0x00FF;
				Index = ((Buffer[*Front][3]<<8)&0xFF00)|((Buffer[*Front][2]>>8)&0x00FF);
				Subindex = ((Buffer[*Front][3]>>8)&0x00FF);

				for(i=1; i<=MCS2MPR_CANSDOSummary[0].Index; i++)//查找Index
				{
					if(Index == MCS2MPR_CANSDOSummary[i].Index)
					{
						for(j=1; j<=MCS2MPR_CANSDOSummary[i].Obj[0].Subindex; j++)
						{
							if(Subindex == MCS2MPR_CANSDOSummary[i].Obj[j].Subindex)
							{
								if(((Command>>2)&0x0003) == MCS2MPR_CANSDOSummary[i].Obj[j].Para.ParaBit.ByteBum)
								{
									switch(Command)
									{
									case 0x4F://主站收到从站1个字节反馈
									{
										*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object=Buffer[*Front][4]&0x00FF;
										break;
									}
									case 0x4B://主站收到从站2个字节反馈
									{
										*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object=Buffer[*Front][4];
										break;
									}
									case 0x47://主站收到从站3个字节反馈
									{
										*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object=Buffer[*Front][4];
										*((unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object+1)=Buffer[*Front][5]&0x00FF;
										break;
									}
									case 0x43://主站收到从站4个字节反馈
									{
										*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object=Buffer[*Front][4];
										*((unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object+1)=Buffer[*Front][5];
										break;
									}
									default:break;
									}
								}
								switch(Command)
								{
									case 0x60://主站收到从站写成功反馈，暂不处理
									case 0x80://主站收到从站读写失败反馈，暂不处理
									default:break;
								}
								break;
							}
						}
						break;
					}
				}
			}
			default:break;
		}
		(*Front)++;
		if((*Front)>=8)
			(*Front)=0;
	}
}
/**************************************************************************************/
void MCS2MPR_ProtocalRecv_Slaver(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal,unsigned int SendBuffer[][6],unsigned int *SendTeal)
{
	int i,j;
	int Command, Index, Subindex, FunCode;
	int Remote_flag;
	union CANSendCtrl Ctrl;
	if((*Front) != Teal)//接收缓冲区有数
	{
		Ctrl.CtrlWord = Buffer[*Front][0];
		FunCode = Ctrl.CtrlBit.FunCode & 0x780;
		Remote_flag = Ctrl.CtrlBit.RTR;
		switch(FunCode)
		{
			case RPDO1:
			case RPDO2:
			case RPDO3:
			case RPDO4:
			{
				for(i=0;i<4;i++)
				{
					if(MCS2MPR_RPDOObj[i].Ctrl.CtrlBit.FunCode == FunCode)
					if(MCS2MPR_RPDOObj[i].DataLength == Buffer[*Front][1])
					{
						*MCS2MPR_RPDOObj[i].PDataBuff[0] = Buffer[*Front][2];
						*MCS2MPR_RPDOObj[i].PDataBuff[1] = Buffer[*Front][3];
						*MCS2MPR_RPDOObj[i].PDataBuff[2] = Buffer[*Front][4];
						*MCS2MPR_RPDOObj[i].PDataBuff[3] = Buffer[*Front][5];
					}
				}
				break;
			}
			case NODE_GUARD:
			{
				if(Remote_flag == 1)
				{
					SendBuffer[*SendTeal][0] = (NODE_GUARD + MCS2MPR_NodeID);
					SendBuffer[*SendTeal][1] = 1;
					SendBuffer[*SendTeal][2] = NODE_STATE;
					SendBuffer[*SendTeal][3] = 0;
					SendBuffer[*SendTeal][4] = 0;
					SendBuffer[*SendTeal][5] = 0;
					(*SendTeal)++;
					if((*SendTeal)>=8)
						(*SendTeal)=0;
					NODE_STATE ^= 0x80;
				} 	  
				break;
			}
			case RSDO:
			{
				Command = Buffer[*Front][2]&0x00FF;
				Index = ((Buffer[*Front][3]<<8)&0xFF00) | ((Buffer[*Front][2]>>8)&0x00FF);
				Subindex = (Buffer[*Front][3]>>8)&0x00FF;

				//默认出错，有返回数据，如果对了再处理
				SendBuffer[*SendTeal][0] = (TSDO + MCS2MPR_NodeID);
				SendBuffer[*SendTeal][1] = 8;
				SendBuffer[*SendTeal][2] = 0x0060;
				SendBuffer[*SendTeal][2] |= (Index<<8)&0xFF00;
				SendBuffer[*SendTeal][3] = (Index>>8)&0x00FF;
				SendBuffer[*SendTeal][3] |= (Subindex<<8)&0xFF00;
				SendBuffer[*SendTeal][4] = 0;
				SendBuffer[*SendTeal][5] = 0;

				for(i=1; i<=MCS2MPR_CANSDOSummary[0].Index; i++)	//查找Index
				{
					if(Index == MCS2MPR_CANSDOSummary[i].Index)
					{
						for(j=1; j<=MCS2MPR_CANSDOSummary[i].Obj[0].Subindex; j++)
						{
							if(Subindex == MCS2MPR_CANSDOSummary[i].Obj[j].Subindex)
							{
								if(((Command>>2)&0x0003) == MCS2MPR_CANSDOSummary[i].Obj[j].Para.ParaBit.ByteBum)
								if((Command&0x00F3) == 0x0023)//主站写
								{
									switch(Command)
									{
										case 0x2F://从站收到主站1个字节
										{
											*MCS2MPR_CANSDOSummary[i].Obj[j].Object = Buffer[*Front][4]&0x00FF;
											break;
										}
										case 0x2B://从站收到主站2个字节
										{
											*MCS2MPR_CANSDOSummary[i].Obj[j].Object = Buffer[*Front][4];
											break;
										}
										case 0x27://从站收到主站3个字节
										{
											*MCS2MPR_CANSDOSummary[i].Obj[j].Object = Buffer[*Front][4];
											*(MCS2MPR_CANSDOSummary[i].Obj[j].Object+1) = Buffer[*Front][5]&0x00FF;
											break;
										}
										case 0x23://从站收到主站4个字节写数据
										{
											*MCS2MPR_CANSDOSummary[i].Obj[j].Object=Buffer[*Front][4];
											*(MCS2MPR_CANSDOSummary[i].Obj[j].Object+1)=Buffer[*Front][5];
											break;
										}
										default:break;
									}
									if(MCS2MPR_CANSDOSummary[i].Obj[Subindex].Para.ParaBit.MemoryFlag==1)
										*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[0].Object=0x0100;
									SendBuffer[*SendTeal][2] &= 0xFF00;//返回成功标志
									SendBuffer[*SendTeal][2] |= 0x0060;
								}

								if(Command == 0x0040)	//主站读数据指令
								{
									SendBuffer[*SendTeal][2] &= 0xFF00;//根据数据类型返回相应命令字
									SendBuffer[*SendTeal][2] |= (MCS2MPR_CANSDOSummary[i].Obj[Subindex].Para.ParaWord&0x005F);
									// SendBuffer[*SendTeal][2]|=0x0040;
									switch(MCS2MPR_CANSDOSummary[i].Obj[j].Para.ParaBit.ByteBum)
									{
										case 3://1 byte
										{
											SendBuffer[*SendTeal][4]=(*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object)&0x00FF;
											SendBuffer[*SendTeal][5]=0;
											SendBuffer[*SendTeal][2]|=0x004F;
											break;
										}
										case 2://2 byte
										{
											SendBuffer[*SendTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object;
											SendBuffer[*SendTeal][5]=0;
											SendBuffer[*SendTeal][2]|=0x004B;
											break;
										}
										case 1://3 byte
										{
											SendBuffer[*SendTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object;
											SendBuffer[*SendTeal][5]=(*((unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object+1))&0x00FF;
											SendBuffer[*SendTeal][2]|=0x0047;
											break;
										}
										case 0://4 byte
										{
											SendBuffer[*SendTeal][4]=*(unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object;
											SendBuffer[*SendTeal][5]=*((unsigned int *)MCS2MPR_CANSDOSummary[i].Obj[j].Object+1);
											SendBuffer[*SendTeal][2]|=0x0043;
										}
										default: break;
									}
								}
							}
						}
					}
				}
				(*SendTeal)++;
				if((*SendTeal)>=8)
					(*SendTeal)=0;
				break;
			}
			default:break;
		}
		(*Front)++;
		if((*Front)>=8)
			(*Front)=0;
	}
}
/******************************************************************************************************/

