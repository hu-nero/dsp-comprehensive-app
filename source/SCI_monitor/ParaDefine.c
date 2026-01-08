#include "main\Variable.h"
#include "Main\Comm_Variable.H"
#include "Main\Custom_GlobalMacro.H"
#include "ParaDefine.h"
#include "MessageParse.h"
#include "DSP28x_Project.h" 
#include "version.h"
#include "softscope.h"
#include "SCI_ModbusM\Custom_ModbusM.H"

extern int ADValue[16];
extern int InterADValue[16];

/**************************************************************/
int BOARD_ADDR=4;
//int Monitor_ADDR=0;
int Monitor_ADDR[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
float RTM_fReserved;
int RTM_Reserved;
int GroupCtrl[35] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
/**************************************************************/
/**************************************************************/
const struct RTMType sParaData_Group1[]=
{
/* Total- 10 */  7,0x0000,(void*)&GroupCtrl[1],
/* Line -  1 */  1,0x0140,(void*)&SysTime_Second,
/* Line -  2 */  2,0x0140,(void*)&SysTime_Minute,
/* Line -  3 */  3,0x0140,(void*)&SysTime_Hour,
/* Line -  4 */  4,0x0140,(void*)&SysTime_Day,
/* Line -  5 */  5,0x0140,(void*)&SysTime_Month,
/* Line -  6 */  6,0x0140,(void*)&SysTime_Year,
/* Line -  7 */  7,0x0140,(void*)&SysTime_MicroSecond,
};
const struct RTMType sParaData_Group2[]=
{
/* Total-  3 */ 4,0x0000,(void*)&GroupCtrl[2],
/* Line -  1 */ 1,0x41c0,(void*)&Conv_Type_Ver,
/* Line -  2 */ 2,0x40c0,(void*)&Soft_Ver,
/* Line -  3 */ 3,0x41c0,(void*)&RTM_File_Ver,
/* Line -  3 */ 4,0x41c0,(void*)&FPGA_Ver,
};
const struct RTMType sParaData_Group3[]=
{
/* Total-  6 */ 3,0x0000,(void*)&GroupCtrl[3],
/* Line -  1 */ 1,0x0020,(void*)&DeviceNum,
/* Line -  1 */ 2,0x0041,(void*)&MainCtrlWord_PC.DataInt,
/* Line -  2 */ 3,0x00A3,(void*)&UsdRefRef_PC,
};

const struct RTMType sParaData_Group4[]=
{
/* Total- 12 */ 10,0x0000,(void*)&GroupCtrl[4],
/* Line -  1 */ 1,0x4040,(void*)&MPR_FaultCode,
/* Line -  3 */ 2,0x0040,(void*)&FaultWord1.DataInt,
/* Line -  4 */ 3,0x0040,(void*)&FaultWord2.DataInt,
/* Line -  5 */ 4,0x0040,(void*)&FaultWord3.DataInt,
/* Line -  6 */ 5,0x0040,(void*)&FaultWord4.DataInt,
/* Line -  7 */ 6,0x0040,(void*)&FaultWord5.DataInt,
/* Line -  8 */ 7,0x0040,(void*)&FaultWord6.DataInt,
/* Line -  9 */ 8,0x0040,(void*)&FaultWord7.DataInt,
/* Line - 10 */ 9,0x0040,(void*)&FaultWord8.DataInt,
/* Line - 11 */ 10,0x0040,(void*)&IOInputFpga2.DataInt,
};
const struct RTMType sParaData_Group5[]=
{
/* Total- 12 */  8,0x0004,(void*)&GroupCtrl[5],
/* Line -  1 */ 1,0x4047,(void*)&MPR_HWFMWord0,
/* Line -  2 */ 2,0x4047,(void*)&MPR_HWFMWord1,
/* Line -  3 */ 3,0x4047,(void*)&MPR_HWFMWord2,
/* Line -  4 */ 4,0x4047,(void*)&MPR_HWFMWord3,
/* Line -  5 */ 5,0x4047,(void*)&MPR_HWFMWord4,
/* Line -  6 */ 6,0x4047,(void*)&MPR_HWFMWord5,
/* Line -  7 */ 7,0x4047,(void*)&MPR_HWFMWord6,
/* Line -  8 */ 8,0x4047,(void*)&MPR_HWFMWord7,
};

const struct RTMType sParaData_Group6[]=
{
/* Total- 20 */15,0x0004,(void*)&GroupCtrl[6],
/* Line -  1 */ 1,0x50A3,(void*)&FltThr_OverVolt_Udc,
/* Line -  2 */ 2,0x50A3,(void*)&FltThr_OverVoltEst_Udc,
/* Line -  3 */ 3,0x50A3,(void*)&FltThr_UnderVolt_Udc,
/* Line -  4 */ 4,0x50A3,(void*)&FltThr_OverImba_Volt,
/* Line -  5 */ 5,0x50A3,(void*)&FltThr_OverImba_Curr,
/* Line -  6 */ 6,0x50A3,(void*)&FltThr_OverCurr_Ipeak,
/* Line -  7 */ 7,0x50A3,(void*)&FltThr_OverCurrEst_Ipeak,
/* Line -  8 */ 8,0x50A3,(void*)&FltThr_OverCurr_Isum,
/* Line -  9 */ 9,0x50A3,(void*)&FltThr_OverDvalue_Freq,
/* Line - 11 */ 10,0x50A3,(void*)&FltThr_OverTemp_IGBT,
/* Line - 12 */ 11,0x50A3,(void*)&FltThr_OverCurr_Irms,
/* Line - 14 */ 12,0x50A3,(void*)&FltThr_OverVolt_Work1,
/* Line - 18 */ 13,0x50A3,(void*)&FltThr_UnderVolt_Work1,
/* Line - 16 */ 14,0x50A3,(void*)&FltThr_OverVolt_Work2,
/* Line - 20 */ 15,0x50A3,(void*)&FltThr_UnderVolt_Work2,
};

const struct RTMType sParaData_Group7[]=
{
/* Total-  5 */ 3,0x0004,(void*)&GroupCtrl[7],
/* Line - 61 */ 1,0x50A3,(void*)&MotorSideCapacity,
/* Line - 62 */ 2,0x50A3,(void*)&MotorSideVoltage,
/* Line - 63 */ 3,0x50A3,(void*)&MotorSideCurrent,
};

const struct RTMType sParaData_Group8[]=
{
/* Total- 12 */ 11,0x0004,(void*)&GroupCtrl[8],
/* Line -  1 */  1,0x50A3,(void*)&AlmThr_OverVolt_Work1,  //129
/* Line -  1 */  2,0x50A3,(void*)&AlmThr_UnderVolt_Work1, //130
/* Line -  1 */  3,0x50A3,(void*)&AlmThr_OverVolt_Work2, //131
/* Line -  1 */  4,0x50A3,(void*)&AlmThr_UnderVolt_Work2,//132
/* Line -  1 */  5,0x50A3,(void*)&AlmThr_OverDvalue_Freq,//133
/* Line -  1 */  6,0x50A3,(void*)&AlmThr_OverTemp_IGBT,//134
/* Line -  1 */  7,0x50A3,(void*)&AlmThr_OverVolt_Udc,//135
/* Line -  1 */  8,0x50A3,(void*)&AlmThr_UnderVolt_Udc,//136
/* Line -  1 */  9,0x50A3,(void*)&AlmThr_OverCurr_Irms,//137
/* Line -  1 */  10,0x50A3,(void*)&AlmThr_OverImba_Volt,//138
/* Line -  1 */  11,0x50A3,(void*)&AlmThr_OverImba_Curr,//139
};

const struct RTMType sParaData_Group9[]=
{
/* Total-  7 */ 7,0x0004,(void*)&GroupCtrl[9],
/* Line -  1 */ 1,0x0043,(void*)&CtrlModeWord.DataInt,
/* Line -  2 */ 2,0x00A3,(void*)&FilterPara_Induct,
/* Line -  3 */ 3,0x00A3,(void*)&Usd_Lim,
/* Line -  4 */ 4,0x0023,(void*)&UdqStep_CountMax,
/* Line -  5 */ 5,0x00A3,(void*)&Ud_cal_step,
/* Line -  6 */ 6,0x00A3,(void*)&CtrlThr_FanIgbtOn_TempH,
/* Line -  7 */ 7,0x00A3,(void*)&CtrlThr_FanIgbtOff_TempL,
};

const struct RTMType sParaData_Group10[]=
{
/* Line - 15 */ 10,0x0004,(void*)&GroupCtrl[10],
/* Line -  6 */  1,0x50A3,(void*)&Udq_Loop_Kp,
/* Line -  7 */  2,0x50A3,(void*)&Udq_Loop_Ki,
/* Line - 10 */  3,0x50A3,(void*)&Udq_Loop_Lim,
/* Line - 11 */  4,0x50A3,(void*)&Udq_Loop_Lim,
/* Line - 11 */  5,0x50A0,(void*)&Udq_Loop_Lim,
/* Line - 11 */  6,0x40A3,(void*)&Idq_Loop_Kp,
/* Line - 12 */  7,0x40A3,(void*)&Idq_Loop_Ki,
/* Line - 14 */  8,0x40A3,(void*)&Id_Loop_Lim,
/* Line - 14 */  9,0x40A3,(void*)&Idq_Loop_Lim,
/* Line - 14 */ 10,0x40A0,(void*)&Iq_Loop_Lim,
};

const struct RTMType sParaData_Group11[]=
{
/* Total- 12 */ 6,0x0004,(void*)&GroupCtrl[11],
/* Line -  1 */ 1,0x0040,(void*)&DspToFpgaWord.DataInt,
/* Line -  1 */ 2,0x0040,(void*)&FpgaToDspWord.DataInt,
/* Line -  1 */ 3,0x0040,(void*)&Mas1ToSlaWord.all,
/* Line -  1 */ 4,0x0040,(void*)&SlaToMas1Word.all,
/* Line -  1 */ 5,0x0040,(void*)&Mas2ToSlaWord.all,
/* Line -  1 */ 6,0x0040,(void*)&SlaToMas2Word.all,
};

const struct RTMType sParaData_Group12[]=
{
/* Total- 12 */ 0,0x0000,(void*)&GroupCtrl[12],
};

const struct RTMType sParaData_Group13[]=
{
/* Line -  9 */ 27,0x0000,(void*)&GroupCtrl[13],
/* Line -  1 */  1,0x50A0,(void*)&MPR_PsACK,
/* Line -  2 */  2,0x50A0,(void*)&MPR_QsACK,
/* Line -  2 */  3,0x50A0,(void*)&MPR_SsACK,
/* Line -  3 */  4,0x50A0,(void*)&Power_Factor,
/* Line -  4 */  5,0x50A0,(void*)&Ua_Rmsf,
/* Line -  5 */  6,0x50A0,(void*)&Ub_Rmsf,
/* Line -  6 */  7,0x50A0,(void*)&Uc_Rmsf,
/* Line -  4 */  8,0x50A0,(void*)&Uab_Rmsf,
/* Line -  5 */  9,0x50A0,(void*)&Ubc_Rmsf,
/* Line -  6 */ 10,0x50A0,(void*)&Uca_Rmsf,
/* Line -  7 */ 11,0x50A0,(void*)&Ia_Rmsf,
/* Line -  8 */ 12,0x50A0,(void*)&Ib_Rmsf,
/* Line -  9 */ 13,0x50A0,(void*)&Ic_Rmsf,
/* Line - 10 */ 14,0x50A0,(void*)&mpr_temphigh,
/* Line - 11 */ 15,0x50A0,(void*)&mpr_templow,
/* Line - 10 */ 16,0x50A0,(void*)&MotorVolt_Imbalance,
/* Line - 11 */ 17,0x50A0,(void*)&MotorCurr_Imbalance,
/* Line -  4 */ 18,0x50A0,(void*)&Ioa_Rmsf,
/* Line -  5 */ 19,0x50A0,(void*)&Iob_Rmsf,
/* Line -  6 */ 20,0x50A0,(void*)&Ioc_Rmsf,
/* Line -  4 */ 21,0x50A0,(void*)&UabBus1_Rmsf,
/* Line -  5 */ 22,0x50A0,(void*)&UbcBus1_Rmsf,
/* Line -  6 */ 23,0x50A0,(void*)&UcaBus1_Rmsf,
/* Line -  7 */ 24,0x50A0,(void*)&UabBus2_Rmsf,
/* Line -  8 */ 25,0x50A0,(void*)&UbcBus2_Rmsf,
/* Line -  9 */ 26,0x50A0,(void*)&UcaBus2_Rmsf,
/* Line -  9 */ 27,0x50A0,(void*)&FlowRate,
};

const struct RTMType sParaData_Group14[]=
{
/* Total-  31 */ 37,0x0000,(void*)&GroupCtrl[14],
/* Line -   1 */  1,0x5020,(void*)&Status_MPRRun,
/* Line -   2 */  2,0x5040,(void*)&MainStusWord.DataInt,
/* Line -   3 */  3,0x50A0,(void*)&mpr_udc,
/* Line -   4 */  4,0x50A0,(void*)&mpr_uab,
/* Line -   5 */  5,0x50A0,(void*)&mpr_ubc,
/* Line -   6 */  6,0x50A0,(void*)&mpr_uca,
/* Line -  10 */  7,0x50A0,(void*)&mpr_ia,
/* Line -  11 */  8,0x50A0,(void*)&mpr_ib,
/* Line -  12 */  9,0x50A0,(void*)&mpr_ic,
/* Line -   7 */ 10,0x50A0,(void*)&MotorVolt_Angle,
/* Line -   8 */ 11,0x50A0,(void*)&FpgaMotorFreq,//MotorVolt_Freq,
/* Line -  20 */ 12,0x50A0,(void*)&UsdRefRef,
/* Line -  21 */ 13,0x50A0,(void*)&UsdRef,
/* Line -  28 */ 14,0x50A0,(void*)&UdRef_All,
/* Line -  29 */ 15,0x50A0,(void*)&UqRef_All,
/* Line -   9 */ 16,0x50A0,(void*)&mpr_ipeak,
/* Line -  13 */ 17,0x50A0,(void*)&mpr_isum,
/* Line -  14 */ 18,0x50A0,(void*)&mpr_udf,
/* Line -  15 */ 19,0x50A0,(void*)&mpr_uqf,
/* Line -  22 */ 20,0x50A0,(void*)&Ud_Loop_Out,
/* Line -  23 */ 21,0x50A0,(void*)&Uq_Loop_Out,
/* Line -  22 */ 22,0x50A0,(void*)&IgdRef,
/* Line -  23 */ 23,0x50A0,(void*)&IgqRef,
/* Line -  18 */ 24,0x50A0,(void*)&mpr_idf,
/* Line -  19 */ 25,0x50A0,(void*)&mpr_iqf,
/* Line -  16 */ 26,0x50A0,(void*)&Id_Loop_Out,
/* Line -  17 */ 27,0x50A0,(void*)&Iq_Loop_Out,
/* Line -  26 */ 28,0x50A0,(void*)&IdRef_Outer,
/* Line -  27 */ 29,0x50A0,(void*)&IqRef_Outer,
/* Line -  26 */ 30,0x50A0,(void*)&UsCtrl_d,
/* Line -  27 */ 31,0x50A0,(void*)&UsCtrl_q,
/* Line -  31 */ 32,0x50A0,(void*)&IdRef_Outer,
/* Line -  32 */ 33,0x50A0,(void*)&IqRef_Outer,
/* Line -  33 */ 34,0x50A0,(void*)&UdcOffset_Outer,
/* Line -  18 */ 35,0x50A0,(void*)&mpr_iodf,
/* Line -  19 */ 36,0x50A0,(void*)&mpr_ioqf,
/* Line -  19 */ 37,0x50A0,(void*)&UdRefRef_Outer,
};

const struct RTMType sParaData_Group15[]=
{
/* Total-  0 */ 8,0x0004,(void*)&GroupCtrl[15],
/* Line -  1 */ 1,0x0021,(void*)&FastChar_CtrlWord,
/* Line -  1 */ 2,0x0043,(void*)&FastChar_PhaseSel,
/* Line -  1 */ 3,0x0043,(void*)&FastChar_RiseTime,
/* Line -  1 */ 4,0x50A3,(void*)&FastChar_DCOffset, //FastChar_HighVolg,
/* Line -  1 */ 5,0x0043,(void*)&FastChar_HoleTime,
/* Line -  1 */ 6,0x0043,(void*)&FastChar_DownTime,
/* Line -  1 */ 7,0x50A3,(void*)&FastChar_AfterVolt,
/* Line - 64 */ 8,0x50A3,(void*)&FastChar_ExitCurrSet, //MotorSideFrequency,
};

const struct RTMType sParaData_Group16[]=
{
/* Line -   6 */  31,0x0000,(void*)&GroupCtrl[16],
/* Line -   6 */   1,0x0041,(void*)&SoftScope_OnOffCmd,
/* Line -   6 */   2,0x0041,(void*)&SoftScope_SampleInterval,
/* Line -   6 */   3,0x4040,(void*)&SoftScope_AnalogCHNum,
/* Line -   6 */   4,0x4040,(void*)&SoftScope_DigitCHNum,
/* Line -   6 */   5,0x5040,(void*)&SoftScope_AnalogCHNum,
/* Line -   6 */   6,0x5040,(void*)&SoftScope_DigitCHNum,
/* Line -   6 */   7,0x0041,(void*)&SoftScope_AnalogChID[0],
/* Line -   6 */   8,0x0041,(void*)&SoftScope_AnalogChID[1],
/* Line -   6 */   9,0x0041,(void*)&SoftScope_AnalogChID[2],
/* Line -   6 */  10,0x0041,(void*)&SoftScope_AnalogChID[3],
/* Line -   6 */  11,0x0041,(void*)&SoftScope_AnalogChID[4],
/* Line -   6 */  12,0x0041,(void*)&SoftScope_AnalogChID[5],
/* Line -   6 */  13,0x0041,(void*)&SoftScope_AnalogChID[6],
/* Line -   6 */  14,0x0041,(void*)&SoftScope_AnalogChID[7],
/* Line -   6 */  15,0x0041,(void*)&SoftScope_DigitChID[0],
/* Line -   6 */  16,0x0041,(void*)&SoftScope_DigitChBit[0],
/* Line -   6 */  17,0x0041,(void*)&SoftScope_DigitChID[1],
/* Line -   6 */  18,0x0041,(void*)&SoftScope_DigitChBit[1],
/* Line -   6 */  19,0x0041,(void*)&SoftScope_DigitChID[2],
/* Line -   6 */  20,0x0041,(void*)&SoftScope_DigitChBit[2],
/* Line -   6 */  21,0x0041,(void*)&SoftScope_DigitChID[3],
/* Line -   6 */  22,0x0041,(void*)&SoftScope_DigitChBit[3],
/* Line -   6 */  23,0x0041,(void*)&SoftScope_DigitChID[4],
/* Line -   6 */  24,0x0041,(void*)&SoftScope_DigitChBit[4],
/* Line -   6 */  25,0x0041,(void*)&SoftScope_DigitChID[5],
/* Line -   6 */  26,0x0041,(void*)&SoftScope_DigitChBit[5],
/* Line -   6 */  27,0x0041,(void*)&SoftScope_DigitChID[6],
/* Line -   6 */  28,0x0041,(void*)&SoftScope_DigitChBit[6],
/* Line -   6 */  29,0x0041,(void*)&SoftScope_DigitChID[7],
/* Line -   6 */  30,0x0041,(void*)&SoftScope_DigitChBit[7],
/* Line -   6 */  31,0x0040,(void*)&SoftScope_SampleFreq,
};

const struct RTMType sParaData_Group17[]=
{
/* Line-  6 */ 0,0x0000,(void*)&GroupCtrl[17],
};

const struct RTMType sParaData_Group18[]=
{
/* Line-  6 */ 0,0x0000,(void*)&GroupCtrl[18],
};



const struct RTMType sParaData_Group19[]=
{
/* Line-  6 */ 0,0x0000,(void*)&GroupCtrl[19],
};

const struct RTMType sParaData_Group20[]=
{
/* Line-  6 */ 0,0x0000,(void*)&GroupCtrl[20]
};

const struct RTMType sParaData_Group21[]=
{
/* Line -   6 */ 0,0x0000,(void*)&GroupCtrl[21],
};

const struct RTMType sParaData_Group22[]=
{
/* Line -   6 */  0,0x0000,(void*)&GroupCtrl[22],
};

const struct RTMType sParaData_Group23[]=
{
/* Line- 4 - */  5,0x0000,(void*)&GroupCtrl[23],
/* Line- 4 - */  1,0x4041,(void*)&ParaDownloadCtrlWord,
/* Line- 4 - */  2,0x4020,(void*)&ParaDownloadFaultNum,
/* Line- 4 - */  4,0x4061,(void*)&SRAM_RandomRead_Addr,
/* Line- 4 - */  5,0x4020,(void*)&SRAM_RandomRead_Value,
/* Line- 4 - */  6,0x4060,(void*)&SRAM_RandomRead_LValue,
};
const struct  RTMType sParaData_Group24[]=
{
/* Total-  28 */  0,0x0000,(void*)&GroupCtrl[24],
};

const struct  RTMType sParaData_Group25[] =
{
/* Total-  10 */  8,0x0004,(void*)&GroupCtrl[25],
/* Line -   1 */  1,0x4043,(void*)&IOOutCtrl1Word.DataInt,
/* Line -   1 */  2,0x4040,(void*)&IOOutput1.DataInt,
/* Line -   1 */  3,0x4040,(void*)&IOOutFpga1.DataInt,
/* Line -   2 */  4,0x4043,(void*)&IOInputEn.DataInt,
/* Line -   3 */  5,0x4043,(void*)&IOInputLogic.DataInt,
/* Line -   4 */  6,0x4043,(void*)&IOInputDisValue.DataInt,
/* Line -   5 */  7,0x4040,(void*)&IOInputFpga1.DataInt,
/* Line -   5 */  8,0x4040,(void*)&IOInput1.DataInt,
};

const struct RTMSummaryType sParaData_Summary[]=
{                              
/* Line -  28 */ 25,(void*)&RTM_Reserved,
/* Line -  28 */  1,(void*)(&sParaData_Group1),
/* Line -  28 */  2,(void*)(&sParaData_Group2),
/* Line -  28 */  3,(void*)(&sParaData_Group3),
/* Line -  28 */  4,(void*)(&sParaData_Group4),
/* Line -  28 */  5,(void*)(&sParaData_Group5),
/* Line -  28 */  6,(void*)(&sParaData_Group6),
/* Line -  28 */  7,(void*)(&sParaData_Group7),
/* Line -  28 */  8,(void*)(&sParaData_Group8),
/* Line -  28 */  9,(void*)(&sParaData_Group9),
/* Line -  28 */ 10,(void*)(&sParaData_Group10),
/* Line -  28 */ 11,(void*)(&sParaData_Group11),
/* Line -  28 */ 12,(void*)(&sParaData_Group12),
/* Line -  28 */ 13,(void*)(&sParaData_Group13),
/* Line -  28 */ 14,(void*)(&sParaData_Group14),
/* Line -  28 */ 15,(void*)(&sParaData_Group15),
/* Line -  28 */ 16,(void*)(&sParaData_Group16),
/* Line -  28 */ 17,(void*)(&sParaData_Group17),
/* Line -  28 */ 18,(void*)(&sParaData_Group18),
/* Line -  28 */ 19,(void*)(&sParaData_Group19),
/* Line -  28 */ 20,(void*)(&sParaData_Group20),
/* Line -  28 */ 21,(void*)(&sParaData_Group21),
/* Line -  28 */ 22,(void*)(&sParaData_Group22),
/* Line -  28 */ 23,(void*)(&sParaData_Group23),
/* Line -  28 */ 24,(void*)(&sParaData_Group24),
/* Line -  28 */ 25,(void*)(&sParaData_Group25),
};
/**********************************************************************************************/

