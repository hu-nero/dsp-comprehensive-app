extern void InitXintf();
extern void GPIO_init(void);
extern void IO_Scan(void);
extern int CPLDSWVerRead(void);
extern void MotorFreq_FpgaScan(void);
//----------------------------------------------------------
//----------------------------------------------------------
//   RS485 PIN
#define IO_EN1RS485_SEND                RS485_Send_En();
#define IO_EN1RS485_RECV                RS485_Send_Dis();
//   RS485 PIN
#define IO_EN2RS485_SEND                RS485_Send_En2();  //gpio26
#define IO_EN2RS485_RECV                RS485_Send_Dis2(); //gpio26
//----------------------------------------------------------
//   LED PIN
#define IO_LEDFPGA_ON					GpioDataRegs.GPASET.bit.GPIO27=1
#define IO_LEDFPGA_OFF					GpioDataRegs.GPACLEAR.bit.GPIO27=1
#define IO_LEDFPGA_TOG					GpioDataRegs.GPADAT.bit.GPIO27^=1

#define IO_LEDR_ON						GpioDataRegs.GPBCLEAR.bit.GPIO49=1
#define IO_LEDR_OFF						GpioDataRegs.GPBSET.bit.GPIO49=1
#define IO_LEDR_TOG						GpioDataRegs.GPBDAT.bit.GPIO49^=1

#define IO_LEDG_ON						GpioDataRegs.GPBCLEAR.bit.GPIO48=1
#define IO_LEDG_OFF						GpioDataRegs.GPBSET.bit.GPIO48=1
#define IO_LEDG_TOG						GpioDataRegs.GPBDAT.bit.GPIO48^=1
//------------------------------------------------------------------------
//   with FPGA PIN
#define IO_FPGARST_DIS					GpioDataRegs.GPASET.bit.GPIO24=1
#define IO_FPGARST_EN					GpioDataRegs.GPACLEAR.bit.GPIO24=1
//   PWM 74ACT245-OE
#define IO_PWM_LOCK						GpioDataRegs.GPACLEAR.bit.GPIO13=1
#define IO_PWM_UNLOCK					GpioDataRegs.GPASET.bit.GPIO13=1
//   PWM EN
#define IO_PWM_EN						GpioDataRegs.GPBCLEAR.bit.GPIO58=1
#define IO_PWM_DIS						GpioDataRegs.GPBSET.bit.GPIO58=1
//------------------------------------------------------------------------
//////////////////////////////////////IOOutputMap//////////////////////////////////////////
#define IO_Run_ON						IOOutput1.DataBit.Bit0 = 1
#define IO_Run_OFF						IOOutput1.DataBit.Bit0 = 0
#define IO_Run_TOG						IOOutput1.DataBit.Bit0 = IOOutput1.DataBit.Bit0^1

#define IO_Fault_ON						IOOutput1.DataBit.Bit1 = 1
#define IO_Fault_OFF					IOOutput1.DataBit.Bit1 = 0

#define IO_NprRun_En					IOOutput1.DataBit.Bit2 = 1
#define IO_NprRun_Dis	  				IOOutput1.DataBit.Bit2 = 0

#define IO_NprRst_En					IOOutput1.DataBit.Bit3 = 1
#define IO_NprRst_Dis  					IOOutput1.DataBit.Bit3 = 0

#define IO_RSVD1_ON						IOOutput1.DataBit.Bit4 = 1
#define IO_RSVD1_OFF	  				IOOutput1.DataBit.Bit4 = 0

#define IO_RSVD2_ON						IOOutput1.DataBit.Bit5 = 1
#define IO_RSVD2_OFF	  				IOOutput1.DataBit.Bit5 = 0

#define IO_ACB1OFF_En					IOOutput1.DataBit.Bit6 = 1
#define IO_ACB1OFF_Dis	  				IOOutput1.DataBit.Bit6 = 0

#define IO_ACB2OFF_En					IOOutput1.DataBit.Bit7 = 1
#define IO_ACB2OFF_Dis	  				IOOutput1.DataBit.Bit7 = 0

#define IO_ACB1ON_En					IOOutput1.DataBit.Bit8 = 1
#define IO_ACB1ON_Dis	  				IOOutput1.DataBit.Bit8 = 0

#define IO_ACB2ON_En					IOOutput1.DataBit.Bit9 = 1
#define IO_ACB2ON_Dis	  				IOOutput1.DataBit.Bit9 = 0

#define IO_RSVD3_ON						IOOutput1.DataBit.Bit10 = 1
#define IO_RSVD3_OFF	  				IOOutput1.DataBit.Bit10 = 0
/////////////////////////////////////IOInputMap////////////////////////////////////////
#define IO_SingPara_Feed   				IOInput1.DataBit.Bit0
#define IO_LocRem_Feed					IOInput1.DataBit.Bit1
#define IO_RunRef_Feed					IOInput1.DataBit.Bit2
#define IO_StopRef_Feed					IOInput1.DataBit.Bit3
#define IO_FastRef_Feed					IOInput1.DataBit.Bit4
#define IO_CompCar_Feed					IOInput1.DataBit.Bit5
#define IO_WorkRef_Feed					IOInput1.DataBit.Bit6
#define IO_ACBBus1_Feed	  				IOInput1.DataBit.Bit7
#define IO_ACBBus2_Feed   				IOInput1.DataBit.Bit8
#define IO_ACBSwitch_Feed   			IOInput1.DataBit.Bit9
#define IO_NprRun_Feed					IOInput1.DataBit.Bit10
#define IO_NprFault_Feed				IOInput1.DataBit.Bit11
///////////////////////////////////////////////////////////////////////////////////////
extern void Flash_CS_En(void);
extern void Flash_CS_Dis(void);
extern void Flash_HardResetEn(void);
extern void Flash_HardResetDis(void);
extern void RS485_Send_En(void);
extern void RS485_Send_Dis(void);
extern void RS485_Send_En2(void);
extern void RS485_Send_Dis2(void);
///////////////////////////////////////////////////////////////////////////////////////

