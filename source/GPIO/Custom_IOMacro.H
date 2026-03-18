extern void InitXintf();
extern void GPIO_init(void);
extern void IO_Scan(void);
extern int CPLDSWVerRead(void);

#define IO_CPLDIO_BUFF_DIS              asm(" NOP") //GpioDataRegs.GPADAT.bit.GPIO24
#define IO_CPLDIO_EN                    asm(" NOP") //GpioDataRegs.GPACLEAR.bit.GPIO24 = 1
#define IO_CPLDIO_DIS                   asm(" NOP") //GpioDataRegs.GPASET.bit.GPIO24 = 1

#define IO_Unload_Cutin                 GpioDataRegs.GPACLEAR.bit.GPIO25 = 1 //IOOutCtrl1Word.DataBit.Bit8
#define IO_Unload_Cutout                GpioDataRegs.GPASET.bit.GPIO25 = 1

//   RS485 PIN
#define IO_EN1RS485_SEND                RS485_Send_En();
#define IO_EN1RS485_RECV                RS485_Send_Dis();
//----------------------------------------------------------
//   LED PIN
#define IO_DSPLED_ON                    GpioDataRegs.GPBSET.bit.GPIO52=1
#define IO_DSPLED_OFF                   GpioDataRegs.GPBCLEAR.bit.GPIO52=1
#define IO_DSPLED_TOG                   GpioDataRegs.GPBDAT.bit.GPIO52^=1
//----------------------------------------------------------
//   with FPGA PIN
#define IO_FPGARST_DIS					GpioDataRegs.GPASET.bit.GPIO24=1
#define IO_FPGARST_EN					GpioDataRegs.GPACLEAR.bit.GPIO24=1
//   RS485 PIN
#define IO_EN2RS485_SEND                RS485_Send_En2();  //gpio26
#define IO_EN2RS485_RECV                RS485_Send_Dis2(); //gpio26
//   PWM 74ACT245-OE
#define IO_PWM_LOCK						GpioDataRegs.GPACLEAR.bit.GPIO13=1
#define IO_PWM_UNLOCK					GpioDataRegs.GPASET.bit.GPIO13=1

//----------------------------------------------------------
//   AD7606 PIN
#define IO_ADCONVSTSTART                GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1
#define IO_ADCONVSTSTOP                 GpioDataRegs.GPBSET.bit.GPIO58 = 1

#define IO_ADNotBusy                    (GpioDataRegs.GPBDAT.bit.GPIO59^1)
//////////////////////////////////////IOOutputMap//////////////////////////////////////////
#define IO_RunAck_ON					IOOutput1.DataBit.Bit0 = 1
#define IO_RunAck_OFF					IOOutput1.DataBit.Bit0 = 0

#define IO_FltAck_ON					IOOutput1.DataBit.Bit1 = 1
#define IO_FltAck_OFF					IOOutput1.DataBit.Bit1 = 0

#define IO_PreChar_ON					IOOutput1.DataBit.Bit2 = 1
#define IO_PreChar_OFF					IOOutput1.DataBit.Bit2 = 0

#define IO_ACT_ON						IOOutput1.DataBit.Bit3 = 1
#define IO_ACT_OFF						IOOutput1.DataBit.Bit3 = 0

//#define IO_ACBON_En					IOOutput1.DataBit.Bit3 = 1
//#define IO_ACBON_Dis					IOOutput1.DataBit.Bit3 = 0
//
//#define IO_ACBOFF_En					IOOutput1.DataBit.Bit4 = 1
//#define IO_ACBOFF_Dis					IOOutput1.DataBit.Bit4 = 0

#define IO_FCC_ON			            IOOutput1.DataBit.Bit5 = 1
#define IO_FCC_OFF			            IOOutput1.DataBit.Bit5 = 0

#define IO_FanIgbt_ON					IOOutput1.DataBit.Bit6 = 1
#define IO_FanIgbt_OFF					IOOutput1.DataBit.Bit6 = 0

#define IO_FanTransf_ON                	IOOutput1.DataBit.Bit7 = 1
#define IO_FanTransf_OFF               	IOOutput1.DataBit.Bit7 = 0

//#define IO_FanCap_ON					IOOutput1.DataBit.Bit9 = 1
//#define IO_FanCap_OFF					IOOutput1.DataBit.Bit9 = 0
/////////////////////////////////////IOInputMap////////////////////////////////////////
#define IO_ACB_Feed						IOInput1.DataBit.Bit1
#define IO_FCC_Feed						IOInput1.DataBit.Bit2
#define IO_FanReact_Feed				IOInput1.DataBit.Bit3
#define IO_FanIgbt_Feed                 IOInput1.DataBit.Bit4
#define IO_RunRef_Feed					IOInput1.DataBit.Bit8
#define IO_RstRef_Feed					IOInput1.DataBit.Bit9
#define IO_Estop_Feed					IOInput1.DataBit.Bit10
#define IO_ACT_Feed						IOInput1.DataBit.Bit11
#define IO_UPS_Feed						IOInput1.DataBit.Bit12
#define IO_Grid_Feed					IOInput1.DataBit.Bit13
///////////////////////////////////////////////////////////////////////////////////////
extern void Flash_CS_En(void);
extern void Flash_CS_Dis(void);
extern void Flash_HardResetEn(void);
extern void Flash_HardResetDis(void);
extern void RS485_Send_En(void);
extern void RS485_Send_Dis(void);
extern void RS485_Send_En2(void);
extern void RS485_Send_Dis2(void);
/*********************************************************************************/

