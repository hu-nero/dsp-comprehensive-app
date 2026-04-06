#ifndef _GLOBLEMACRO_H_
#define _GLOBLEMACRO_H_

extern volatile struct GPIO_CTRL_REGS GpioCtrlRegs;
extern volatile struct GPIO_DATA_REGS GpioDataRegs;
extern volatile struct GPIO_INT_REGS GpioIntRegs;
/***********************************************************/
union Remote_Ctrl_Word_Type
{
	unsigned int all;
	struct
	{
		unsigned rem_single_start:		1;		// 0:NoAct 1:enable
		unsigned rem_single_stop:		1;		// 0:NoAct 1:enable
		unsigned rem_parallel_start:	1;		// 0:NoAct 1:enable
		unsigned rem_parallel_stop:		1;		// 0:NoAct 1:enable
		unsigned rem_short_start:		1;		// 0:NoAct 1:enable
		unsigned rem_short_stop:		1;		// 0:NoAct 1:enable
		unsigned ComCut:		        2;		// 0:NoAct 1:Comm 2:Cut 3:Invalid
		unsigned Reset:			        1;		// 0:NoAct 1:enable
		unsigned Rsvd:			        7;		//
	}	bit;
};
/***********************************************************/
union Main_Control_Word_Type
{
	unsigned int DataInt;
	struct
	{
		unsigned ACPrechar:1;            // bit0:
		unsigned ACTON:1; 	             // bit1:
		unsigned PWMON:1;                // bit2:
		unsigned Rsvd1:3;				 // bit3~5:
		unsigned Stop:1;                 // bit6:
		unsigned Reset:1;                // bit7:
		unsigned Rsvd2:5;                // bit8~12:
		unsigned Valid:2;                // bit14-bit13:
		unsigned Toggle:1;               // bit15:
	}	DataBit;
};
/***********************************************************/
union NPR_Status_Word_Type
{
	unsigned int DataInt;
	struct
	{
		unsigned LocRem:	1;			// bit0: 0-self 1-master
		unsigned Running:	1;
		unsigned Estop:		1;			// bit1: 0-mode1 1-mode2
		unsigned PwrUps:	1;			// bit8: 0-normal 1-fastchar
		unsigned PwrGrid:	1;			// bit9: 0-disconnect 1-connect
		unsigned ShockCarIn:1;			// Bit11: 0-disconnect 1-connect
		unsigned CompCarIn:	1;			// Bit12: 0-disconnect 1-connect
		unsigned Rsvd:		6;
		unsigned Alarm:		1;			// bit14:
		unsigned Fault:		1;			// bit14:
		unsigned Toggle:	1;			// bit15: 0-low 1-high
	}	DataBit;
};
/***********************************************************/
union NPR_Status_Word2_Type
{
	unsigned int DataInt;
	struct
	{
		unsigned SingQR1:	1;
		unsigned SingQF1:	1;
		unsigned ParaQR2:	1;
		unsigned ParaQF2:	1;
		unsigned WorkLpQS1:	1;
		unsigned CheckLpQS1:1;
		unsigned CompCapQS2:1;
		unsigned DgssQS3:	1;
		unsigned DgssKM1:	1;
		unsigned Rsvd:		4;
		unsigned ComSync:	1;
		unsigned ComPwr:	1;
		unsigned ComHMI:	1;
	}	DataBit;
};
/***********************************************************/
union Control_Mode_Word_Type
{
	unsigned int DataInt;
	struct
	{
		unsigned Control_GivenMode:3;
		unsigned Active_GivenMode:3;
		unsigned Reactive_GivenMode:3;
		unsigned Unused:1;
		unsigned PowModu1En:1;
		unsigned PowModu2En:1;
		unsigned PowModu3En:1;
		unsigned PowModu4En:1;
		unsigned Unused6:1;
		unsigned Unused7:1;
	}DataBit;
};
/***********************************************************/
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
	}	DataBit;
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

union Com_Modbus_To_HMI_Word1_Type
{
    unsigned int DataInt;
    struct
    {
        unsigned DO_REMOTE_SINGLE_CTRL_FEED:1;
        unsigned DO_REMOTE_PARALEL_CTRL_FEED:1;
        unsigned DO_A3QF1:1;
        unsigned DO_A3QF2:1;
        unsigned DO_A3QR1:1;
        unsigned DO_A3QR2:1;
        unsigned DO_A3QR3:1;
        unsigned DO_A3QR4:1;
        unsigned DO_A3QR5:1;
        unsigned DO_A3QR6:1;
        unsigned DO_A3QR7:1;
        unsigned DO_RUNLED:1;
        unsigned DO_FLTLED:1;
        unsigned RESERVE:3;
    }DataBit;
};

union Com_Modbus_To_HMI_Word2_Type
{
    unsigned int DataInt;
    struct
    {
        unsigned DI_ID_LOCALREMOTE:1;
        unsigned DI_ID_EMERGENCY_STOP:1;
        unsigned DI_ID_RESET:1;
        unsigned DI_ID_A3QF1:1;
        unsigned DI_ID_A3QF2:1;
        unsigned DI_ID_A3QS1:1;  //S1接通回路
        unsigned DI_ID_A3K17:1;  //S1接通检修
        unsigned DI_ID_A3QS2:1;
        unsigned DI_ID_UPS:1;
        unsigned DI_ID_MAINS_ELECT:1; //市电
        unsigned DI_ID_KM1:1;
        unsigned DI_ID_PANEL_SINGLE:1;
        unsigned DI_ID_PANEL_PARALLEL:1;
        unsigned DI_ID_PANEL_SHORT:1;
        unsigned DI_ID_IMPACT_VEHICLE:1;
        unsigned DI_ID_COMPENSATION_VEHICLE:1;
    }DataBit;
};

union Com_Modbus_To_HMI_Word3_Type
{
    unsigned int DataInt;
    struct
    {
        unsigned DI_ID_REMOTE_SINGLE_START:1;
        unsigned DI_ID_REMOTE_SINGLE_END:1;
        unsigned DI_ID_REMOTE_PARALLEL_START:1;
        unsigned DI_ID_REMOTE_PARALLEL_END:1;
        unsigned DI_ID_REMOTE_DEGAUSS_START:1;
        unsigned DI_ID_REMOTE_DEGAUSS_END:1;
        unsigned DI_ID_A3QR1:1;
        unsigned DI_ID_A3QR2:1;
        unsigned DI_ID_A3QS3:1;
        unsigned DI_ID_A3QR4:1;
        unsigned DI_ID_A3QR5:1;
        unsigned DI_ID_A3QR6:1;
        unsigned DI_ID_A3QR7:1;
        unsigned RESERVE:5;
    }DataBit;
};

#endif



