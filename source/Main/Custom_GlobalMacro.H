#ifndef _GLOBLEMACRO_H_
#define _GLOBLEMACRO_H_

extern volatile struct GPIO_CTRL_REGS GpioCtrlRegs;
extern volatile struct GPIO_DATA_REGS GpioDataRegs;
extern volatile struct GPIO_INT_REGS GpioIntRegs;
/***********************************************************/
struct Heartbeat_Check_Type
{
	unsigned int ToggLow_Cnt;
	unsigned int ToggHigh_Cnt;
	unsigned int Comm_result;
};
/***********************************************************/
union SLAVE_TO_MASTER_Type
{
	unsigned int all;
	struct
	{
		unsigned Run:			1;		// bit0
		unsigned Fault:			1;		// bit1
		unsigned WorkMode:		1;		// bit2
		unsigned SingPara:		1;		// bit3
		unsigned ParallReq:		1;		// bit4
		unsigned AddBusOk:		1;		// bit5 0-fail 1-finish
		unsigned Bus1ACB:		1;		// bit6 0-fail 1-finish
		unsigned MasSelect:		1;		// bit7 0-backup 1-active
		unsigned Rsvd:			5;		// bit12~8
		unsigned BakOnline:		1;		// bit13 backup master 0-NoComm 1-SetupComm
		unsigned ActOnline:		1;		// bit14 active master 0-NoComm 1-SetupComm
		unsigned Toggle:		1;		// bit15
	}	bit;
};
/***********************************************************/
union MASTER_TO_SLAVE_Type
{
	unsigned int all;
	struct
	{
		unsigned WorkMode:		1;		// bit0
		unsigned AllowAdd:		1;		// bit1
		unsigned ParallAck:		1;		// bit2
		unsigned BusBuild:		1;		// bit3
		unsigned Identity:		1;		// bit4 0-backup 1-active
		unsigned Fastchar:		2;		// bit6~5 00-no 01-phaseA 10-phaseB 11-phaseC
		unsigned Rsvd:			7;		// bit13~7
		unsigned Online:		1;		// bit14 0-NoComm 1-SetupComm
		unsigned Toggle:		1;		// bit15
	}	bit;
};
/***********************************************************/
union DSP_TO_FPGA_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned WorkMode:		1;              // bit0
		unsigned VoltSel:		1;				// bit1
		unsigned ReadyOk:		1;              // bit2
		unsigned VoltForward:	1;          	// bit3
		unsigned CurrCouple:	1;          	// bit4
		unsigned MasSel:		1;          	// bit5
		unsigned IoutFwdEn:		1;          	// bit6
		unsigned Fastchar:		2;				// bit7~8
		unsigned Rsvd:		    7;
	}	DataBit;
};

union FPGA_TO_DSP_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned SyncOk:	1;             // bit0
		unsigned Rsvd:	 	15;
	}	DataBit;
};
/***********************************************************/
union Main_Control_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned NPR_Run:	1;				// bit2
		unsigned PWMON:		1;				// bit3
		unsigned Stop:		1;				// bit6
		unsigned Reset:		1;				// bit7
		unsigned Rsvd:		12;				// bit8
	}	DataBit;
};
/***********************************************************/
union Main_Status_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned VoltSel:	1;			// bit0: 0-self 1-master
		unsigned WorkMode:	1;			// bit1: 0-mode1 1-mode2
		unsigned LocRem:	1;			// bit2: 0-local 1-remod
		unsigned SingPara:	1;			// bit3: 0-single 1-parall
		unsigned NPRRun:	1;			// bit4: 0-stop 1-run
		unsigned NprFault:	1;			// bit5: 0-normal 1-flaut
		unsigned MprRun:	1;			// bit6: 0-stop 1-run
		unsigned MprFault:	1;			// bit7: 0-normal 1-flaut
		unsigned FastChar:	1;			// bit8: 0-normal 1-fastchar
		unsigned ACB1Req:	1;			// bit9: 0-disconnect 1-connect
		unsigned ACB1Ack:	1;			// bit10: 0-disconnect 1-connect
		unsigned ACB2Req:	1;			// Bit11: 0-disconnect 1-connect
		unsigned ACB2Ack:	1;			// Bit12: 0-disconnect 1-connect
		unsigned MasSel:	1;			// bit13: 0-1#Master 1-2#Master
		unsigned Rsvd:		1;			// bit14:
		unsigned Toggle:	1;			// bit15: 0-low 1-high
	}	DataBit;
};
/***********************************************************/
union Control_Mode_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned Control_GivenMode:3;	// bit2-bit0:
		unsigned Us_GivenMode:3;		// bit5-bit3:
		unsigned Freq_GivenMode:3;		// bit8-bit6:
		unsigned Unused9:1;				// bit9:
		unsigned Unused10:1;			// bit10:
		unsigned Unused11:1;			// bit11:
		unsigned Unused12:1;			// bit12:
		unsigned Unused13:1;			// bit13:
		unsigned Unused14:1;			// bit14:
		unsigned Unused15:1;			// bit15:
	}DataBit;
};
/***********************************************************/
union Break_Ctrl_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned Breaker1:1;	// bit0:
		unsigned Breaker2:1;		// bit1:
		unsigned Breaker3:1;		// bit2:
		unsigned Unused:13;
	}DataBit;
};
/***********************************************************/
union Break_Status_Word_Type
{
	u16 DataInt;
	struct
	{
		unsigned Breaker1:1;	// bit0:
		unsigned Breaker2:1;		// bit1:
		unsigned Breaker3:1;		// bit2:
		unsigned Unused:13;
	}	DataBit;
};
/***********************************************************/
union Power_Status_Word_BitType
{
	unsigned int DataInt;
	struct
	{
		unsigned Run:1;
		unsigned Fault:1;
		unsigned Loc2Rem:1;
		unsigned WorkMode:1;
		unsigned PowerOn:1;
		unsigned PowerOnOld:1;
		unsigned FastChar:1;
		unsigned Rsvd:7;
		unsigned Toggle:1;
		unsigned Rsvd_Ban:1;
	}	DataBit;
};


union Power_Ctrl_Word_BitType
{
	unsigned int DataInt;
	struct
	{
		unsigned Run: 1;
		unsigned Stop:1;
		unsigned Reset:1;
		unsigned ReStart: 1;
		unsigned CmdVaild:1;
		unsigned CmdVaildOld:1;
		unsigned Rvsd2: 8;
		unsigned Toggle:1;
		unsigned Rvsd3: 1;
	}DataBit;
};

/*****************************************************/
////////////////////////////////////////////////////////////////
union IOinCtrl_Type 
{
	unsigned int DataInt;
	struct
	{
		unsigned FilterCn:4;
		unsigned FilterMax:4;
		unsigned MapBit:4;
		unsigned Logic:1;
		unsigned En:1;
		unsigned Value:1;
		unsigned ReservValue:1;
	}DataBit;
};
////////////////////////////////////////////////
union DataBit_Type
{
	unsigned int DataInt;
	struct
	{
		unsigned Bit0:1;
		unsigned Bit1:1;
		unsigned Bit2:1;
		unsigned Bit3:1;
		unsigned Bit4:1;
		unsigned Bit5:1;
		unsigned Bit6:1;
		unsigned Bit7:1;
		unsigned Bit8:1;
		unsigned Bit9:1;
		unsigned Bit10:1;
		unsigned Bit11:1;
		unsigned Bit12:1;
		unsigned Bit13:1;
		unsigned Bit14:1;
		unsigned Bit15:1;
	}DataBit;
};
/***********************************************/
union UnloadCtrl_t
{
	int DataInt;
	struct
	{
		unsigned int highcount:4;
		unsigned int lowcount:4;
		unsigned int Valid:1;
		unsigned int FaultCount:3;
		unsigned int TestFlag:1;
		unsigned int CountFlag:1;
		unsigned int unused:2;
	}DataBit;
};
/***********************************************/
union Conv_MainCtrlWordType
{
	unsigned int DataInt;
	struct
	{
		unsigned rsvd1:3;
		unsigned NPR_OnRun:1;
		unsigned MPR_OnRun:1;
		unsigned rsvd2:1;
		unsigned ManStop:1;
		unsigned Reset:1;
		unsigned MPRUser:1;
		unsigned NPRUser:1;
		unsigned NPRLVRT:1;
		unsigned RT_PWM:1;
		unsigned Mode1:1;
		unsigned Mode2:1;
		unsigned Mode3:1;
		unsigned ToggleBit:1;
	}DataBit;
};
/***********************************************/
struct Bilinear_Tran_Type
{
	float Para_A[3];
	float Para_B[3];
	s32 Para_Num[3];
	s32 Para_Den[3];
};

#endif



