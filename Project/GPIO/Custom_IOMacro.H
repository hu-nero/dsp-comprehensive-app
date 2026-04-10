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
#define IO_EN1RS485_SEND                RS485_Send_En2();
#define IO_EN1RS485_RECV                RS485_Send_Dis2();

//   RS485 PIN
#define IO_EN2RS485_SEND                RS485_Send_En();  //gpio26
#define IO_EN2RS485_RECV                RS485_Send_Dis(); //gpio26
//----------------------------------------------------------
//   LED PIN
#define IO_DSPLED_ON                    GpioDataRegs.GPBSET.bit.GPIO52=1
#define IO_DSPLED_OFF                   GpioDataRegs.GPBCLEAR.bit.GPIO52=1
#define IO_DSPLED_TOG                   GpioDataRegs.GPBDAT.bit.GPIO52^=1
//----------------------------------------------------------
//   with FPGA PIN
#define IO_FPGARST_DIS					GpioDataRegs.GPASET.bit.GPIO24=1
#define IO_FPGARST_EN					GpioDataRegs.GPACLEAR.bit.GPIO24=1

//   PWM 74ACT245-OE
#define IO_PWM_LOCK						GpioDataRegs.GPACLEAR.bit.GPIO13=1
#define IO_PWM_UNLOCK					GpioDataRegs.GPASET.bit.GPIO13=1
//----------------------------------------------------------
//   AD7606 PIN
#define IO_ADCONVSTSTART                GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1
#define IO_ADCONVSTSTOP                 GpioDataRegs.GPBSET.bit.GPIO58 = 1

#define IO_ADNotBusy                    (GpioDataRegs.GPBDAT.bit.GPIO59^1)
//////////////////////////////////////IOOutputMap//////////////////////////////////////////
#define IO_REMOTE_SINGLE_CTRL_FEED_ON	IOOutput1.DataBit.Bit0 = 1
#define IO_REMOTE_SINGLE_CTRL_FEED_OFF	IOOutput1.DataBit.Bit0 = 0

#define IO_DgssAck_ON					IOOutput1.DataBit.Bit1 = 1
#define IO_DgssAck_OFF					IOOutput1.DataBit.Bit1 = 0

#define IO_QF1SingOn_En					IOOutput1.DataBit.Bit2 = 1
#define IO_QF1SingOn_Dis				IOOutput1.DataBit.Bit2 = 0

#define IO_QF1SingOff_En				IOOutput1.DataBit.Bit3 = 1
#define IO_QF1SingOff_Dis				IOOutput1.DataBit.Bit3 = 0

#define IO_REMOTE_PARALEL_CTRL_FEED_ON	IOOutput1.DataBit.Bit4 = 1
#define IO_REMOTE_PARALEL_CTRL_FEED_OFF	IOOutput1.DataBit.Bit4 = 0
//
#define IO_QF2ParaOn_En					IOOutput1.DataBit.Bit5 = 1
#define IO_QF2ParaOn_Dis				IOOutput1.DataBit.Bit5 = 0

#define IO_QF2ParaOff_En			    IOOutput1.DataBit.Bit6 = 1
#define IO_QF2ParaOff_Dis			    IOOutput1.DataBit.Bit6 = 0

//#define 					IOOutput1.DataBit.Bit7 = 1
//#define 					IOOutput1.DataBit.Bit7 = 0

#define IO_QR3Dgss_ON                	IOOutput1.DataBit.Bit8 = 1
#define IO_QR3Dgss_OFF               	IOOutput1.DataBit.Bit8 = 0

#define IO_QR4CompCap_ON                IOOutput1.DataBit.Bit9 = 1
#define IO_QR4CompCap_OFF               IOOutput1.DataBit.Bit9 = 0

#define IO_QR5CompCap_ON                IOOutput1.DataBit.Bit10 = 1
#define IO_QR5CompCap_OFF               IOOutput1.DataBit.Bit10 = 0

#define IO_QR6CompCap_ON                IOOutput1.DataBit.Bit11 = 1
#define IO_QR6CompCap_OFF               IOOutput1.DataBit.Bit11 = 0

#define IO_QR7CompCap_ON                IOOutput1.DataBit.Bit12 = 1
#define IO_QR7CompCap_OFF               IOOutput1.DataBit.Bit12 = 0

//#define                 IOOutput1.DataBit.Bit13 = 1
//#define                IOOutput1.DataBit.Bit13 = 0

#define IO_RunLed_ON                IOOutput1.DataBit.Bit14 = 1
#define IO_RunLed_OFF               IOOutput1.DataBit.Bit14 = 0

#define IO_FltLed_ON                IOOutput1.DataBit.Bit15 = 1
#define IO_FltLed_OFF               IOOutput1.DataBit.Bit15 = 0

#define IO_QR1Sing_ON				IOOutput2.DataBit.Bit6 = 0
#define IO_QR1Sing_OFF				IOOutput2.DataBit.Bit6 = 1

#define IO_QR2Para_ON				IOOutput2.DataBit.Bit7 = 0
#define IO_QR2Para_OFF				IOOutput2.DataBit.Bit7 = 1
/////////////////////////////////////IOInputMap////////////////////////////////////////
#define IO_LocRem_Feed					IOInput1.DataBit.Bit0
#define IO_Estop_Feed					IOInput1.DataBit.Bit1
#define IO_RstRef_Feed					IOInput1.DataBit.Bit2
#define IO_SingQF1_Feed					IOInput1.DataBit.Bit3
#define IO_ParaQF2_Feed                	IOInput1.DataBit.Bit4
#define IO_WorkLpQS1_Feed				IOInput1.DataBit.Bit5
#define IO_CheckLpQS1_Feed				IOInput1.DataBit.Bit6
#define IO_CompCapQS2_Feed				IOInput1.DataBit.Bit7
#define IO_PwrUps_Feed					IOInput1.DataBit.Bit8
#define IO_PwrGrid_Feed					IOInput1.DataBit.Bit9
#define IO_DgssKM1_Feed					IOInput1.DataBit.Bit10
#define IO_SingCtrl_Feed				IOInput1.DataBit.Bit11
#define IO_ParaCtrl_Feed				IOInput1.DataBit.Bit12
#define IO_DgssCtrl_Feed				IOInput1.DataBit.Bit13
#define IO_ShockCar_Feed				IOInput1.DataBit.Bit14
#define IO_CompCar_Feed					IOInput1.DataBit.Bit15

#define IORem_SingCtrl_Feed				IOInput2.DataBit.Bit0
#define IORem_ParaCtrl_Feed				IOInput2.DataBit.Bit1
#define IORem_ParaBusOn_Feed			IOInput2.DataBit.Bit2
#define IORem_ParaBusOff_Feed			IOInput2.DataBit.Bit3
#define IORem_DegaussOn_Feed            IOInput2.DataBit.Bit4
#define IORem_DegaussOff_Feed			IOInput2.DataBit.Bit5
#define IO_SingQR1_Feed					IOInput2.DataBit.Bit6
#define IO_ParaQR2_Feed					IOInput2.DataBit.Bit7
#define IO_DgssQS3_Feed					IOInput2.DataBit.Bit8
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

