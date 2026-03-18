#include "main\Variable.h"
#include "Main\Comm_Variable.H"
#include "Main\Custom_GlobalMacro.H"
#include "ParaDefine.h"
#include "MessageParse.h"
#include "DSP28x_Project.h" 
#include "version.h"
#include "softscope.h"
#include "SCI_ModbusM\Custom_Remote.H"

extern int ADValue[16];
extern int InterADValue[16];
/**************************************************************/
int BOARD_ADDR=4;
//int Monitor_ADDR=0;
int Monitor_ADDR[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
int RTM_Reserved;
int GroupCtrl[35] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
/**************************************************************/
const struct RTMType sParaData_Group1[]=
{
/* Line -  31 */  1,0x0000,(void*)&GroupCtrl[1],
/* Line -  31 */  10,0x4080,(void*)&ulRealTimeSecond,
};
const struct RTMType sParaData_Group2[]=
{
/* Line -  31 */  3, 0x0000, (void*)&GroupCtrl[2],
/* Line -  31 */  1, 0x40c0, (void*)&Conv_Type_Ver,
/* Line -  31 */  2, 0x40c0, (void*)&Soft_Ver,
/* Line -  31 */  3, 0x40c0, (void*)&RTM_File_Ver,
};
const struct RTMType sParaData_Group3[]=
{
/* Line -  31 */  5,0x0000,(void*)&GroupCtrl[3],
/* Line -  31 */  1,0x0041,(void*)&MainCtrlWord_PC.DataInt,
/* Line -  31 */  2,0x00A0,(void*)&KWRefRef_PC,
/* Line -  31 */  3,0x00A1,(void*)&KVARRef_PC,
/* Line -  31 */  4,0x00A1,(void*)&CurrRef_InductLoad,
/* Line -  31 */  5,0x0021,(void*)&UnloadCtrl_PC,
};
const struct RTMType sParaData_Group4[]=
{
/* Line - 11 */ 10,0x0000,(void*)&GroupCtrl[4],
/* Line -  1 */  1,0x4040,(void*)&NPR_FaultCode,
/* Line -  3 */  2,0x0040,(void*)&FaultWord1.DataInt,
/* Line -  4 */  3,0x0040,(void*)&FaultWord2.DataInt,
/* Line -  5 */  4,0x0040,(void*)&FaultWord3.DataInt,
/* Line -  6 */  5,0x0040,(void*)&FaultWord4.DataInt,
/* Line -  7 */  6,0x0040,(void*)&FaultWord5.DataInt,
/* Line -  8 */  7,0x0040,(void*)&FaultWord6.DataInt,
/* Line -  9 */  8,0x0040,(void*)&FaultWord7.DataInt,
/* Line - 10 */  9,0x0040,(void*)&FaultWord8.DataInt,
/* Line - 11 */ 10,0x0040,(void*)&IOInput2.DataInt,
};
const struct RTMType sParaData_Group5[]=
{
/* Total-  8 */  8,0x0004,(void*)&GroupCtrl[5],
/* Line -  1 */  1,0x4043,(void*)&NPR_HWFMWord0,
/* Line -  2 */  2,0x4043,(void*)&NPR_HWFMWord1,
/* Line -  3 */  3,0x4043,(void*)&NPR_HWFMWord2,
/* Line -  4 */  4,0x4043,(void*)&NPR_HWFMWord3,
/* Line -  5 */  5,0x4043,(void*)&NPR_HWFMWord4,
/* Line -  6 */  6,0x4043,(void*)&NPR_HWFMWord5,
/* Line -  7 */  7,0x4043,(void*)&NPR_HWFMWord6,
/* Line -  8 */  8,0x4043,(void*)&NPR_HWFMWord7,
};
const struct RTMType sParaData_Group6[]=
{
/* Line - 16 */32,0x0004,(void*)&GroupCtrl[6],
/* Line -  1 */  1,0x4043,(void*)&AlmCntThr_UnderVolt_Phase,
/* Line -  2 */  2,0x40A3,(void*)&AlmThr_UnderVolt_Phase,
/* Line -  3 */  3,0x4043,(void*)&AlmCntThr_OverVolt_Work1,
/* Line -  4 */  4,0x40A3,(void*)&AlmThr_OverVolt_Work1,
/* Line -  5 */  5,0x40A3,(void*)&FltThr_OverVolt_Work1,
/* Line - 6 */   6,0x4043,(void*)&AlmCntThr_OverVolt_Work2,
/* Line - 7 */  7,0x40A3,(void*)&AlmThr_OverVolt_Work2,
/* Line - 8 */  8,0x40A3,(void*)&FltThr_OverVolt_Work2,
/* Line - 9 */  9,0x4043,(void*)&AlmCntThr_UnderVolt_Work1,
/* Line -  10 */ 10,0x40A3,(void*)&AlmThr_UnderVolt_Work1,
/* Line - 11 */ 11,0x40A3,(void*)&FltThr_UnderVolt_Work1,
/* Line - 12 */ 12,0x4043,(void*)&AlmCntThr_UnderVolt_Work2,
/* Line -  13 */ 13,0x40A3,(void*)&AlmThr_UnderVolt_Work2,
/* Line - 14 */ 14,0x40A3,(void*)&FltThr_UnderVolt_Work2,
/* Line -  15 */ 15,0x4043,(void*)&AlmCntThr_OverImba_Curr,
/* Line -  16 */ 16,0x40A3,(void*)&AlmThr_OverImba_Curr ,
/* Line -  17 */ 17,0x4043,(void*)&FltCntThr_OverImba_Curr,
/* Line -  18 */ 18,0x40A3,(void*)&FltThr_OverImba_Curr ,
/* Line -  19 */ 19,0x4043,(void*)&AlmCntThr_OverImba_Volt  ,
/* Line -  20 */ 20,0x40A3,(void*)&AlmThr_OverImba_Volt ,
/* Line -  21 */ 21,0x4043,(void*)&FltCntThr_OverImba_Volt  ,
/* Line -  22 */ 22,0x40A3,(void*)&FltThr_OverImba_Volt  ,
/* Line -  23 */ 23,0x4043,(void*)&AlmCntThr_OverCurr_Work1,
/* Line -  24 */ 24,0x40A3,(void*)&AlmThr_OverCurr_Work1,
/* Line -  25 */ 25,0x40A3,(void*)&FltThr_OverCurr_Work1,
/* Line - 26 */ 26,0x4043,(void*)&AlmCntThr_OverCurr_Work2,
/* Line - 27 */ 27,0x40A3,(void*)&AlmThr_OverCurr_Work2,
/* Line - 28 */ 28,0x40A3,(void*)&FltThr_OverCurr_Work2,
/* Line -  29 */ 29,0x4043,(void*)&AlmCntThr_OverLow_Flow1,
/* Line -  30 */ 30,0x40A3,(void*)&FltThr_OverLow_Flow1,
/* Line - 31 */ 31,0x4043,(void*)&AlmCntThr_OverLow_Flow2,
/* Line - 32 */ 32,0x40A3,(void*)&FltThr_OverLow_Flow2,
};
const struct RTMType sParaData_Group7[]=
{
/* Line -  31 */ 0,0x0004,(void*)&GroupCtrl[7],
};
const struct RTMType sParaData_Group8[]=
{
/* Line -  31 */ 0,0x0000,(void*)&GroupCtrl[8],
};
const struct RTMType sParaData_Group9[]=
{
/* Line - 31 */ 0,0x0004,(void*)&GroupCtrl[9],
};
const struct RTMType sParaData_Group10[]=
{
/* Total- 32 */ 0,0x0004,(void*)&GroupCtrl[10],
};
const struct RTMType sParaData_Group11[]=
{
/* Line -  32 */ 17,0x0000,(void*)&GroupCtrl[11],
/* Line -  32 */  1,0x40A0,(void*)&Uab_Rmsf,
/* Line -  32 */  2,0x40A0,(void*)&Ubc_Rmsf,
/* Line -  32 */  3,0x40A0,(void*)&Uca_Rmsf,
/* Line -  32 */  4,0x40A0,(void*)&Ia_Rmsf,
/* Line -  32 */  5,0x40A0,(void*)&Ib_Rmsf,
/* Line -  32 */  6,0x40A0,(void*)&Ic_Rmsf,
/* Line -  32 */  7,0x40A0,(void*)&NPR_Power,
/* Line -  32 */  8,0x40A0,(void*)&NPR_ReactPower,
/* Line -  32 */  9,0x40A0,(void*)&Power_Factor,
/* Line -  32 */ 10,0x40A0,(void*)&Iaf_Rmsf,
/* Line -  32 */ 11,0x40A0,(void*)&Ibf_Rmsf,
/* Line -  32 */ 12,0x40A0,(void*)&Icf_Rmsf,
/* Line -  32 */ 13,0x40A0,(void*)&Temp_IgbtH,
/* Line -  32 */ 14,0x40A0,(void*)&Temp_IgbtL,
/* Line -  32 */ 15,0x40A0,(void*)&Temp_ReactIn,
/* Line -  32 */ 16,0x40A0,(void*)&Temp_ReactOut,
/* Line -  32 */ 17,0x40A0,(void*)&Temp_TransIn,
};

const struct RTMType sParaData_Group12[]=
{
/* Line -   6 */  39,0x0000,(void*)&GroupCtrl[12],
/* Line -   6 */   1,0x4020,(void*)&Status_NPRRun,
/* Line -   6 */   2,0x4040,(void*)&MainStusWord.DataInt,
/* Line -   6 */   3,0x40A0,(void*)&npr_udc,
/* Line -   6 */   4,0x40A0,(void*)&npr_uab,
/* Line -   6 */   5,0x40A0,(void*)&npr_ubc,
/* Line -   6 */   6,0x40A0,(void*)&npr_uca,
/* Line -   6 */   7,0x40A0,(void*)&GridVolt_Angle,
/* Line -   6 */   8,0x40A0,(void*)&npr_freq,
/* Line -   6 */   9,0x40A0,(void*)&GridVolt_Imbalance,
/* Line -   6 */  10,0x40A0,(void*)&npr_udp,
/* Line -   6 */  11,0x40A0,(void*)&npr_uqp,
/* Line -   6 */  12,0x40A0,(void*)&npr_udn,
/* Line -   6 */  13,0x40A0,(void*)&npr_uqn,
/* Line -   6 */  14,0x40A0,(void*)&npr_ia,
/* Line -   6 */  15,0x40A0,(void*)&npr_ib,
/* Line -   6 */  16,0x40A0,(void*)&npr_ic,
/* Line -   6 */  17,0x40A0,(void*)&npr_isum,
/* Line -   6 */  18,0x40A0,(void*)&npr_id,
/* Line -   6 */  19,0x40A0,(void*)&npr_iq,
/* Line -   6 */  20,0x40A0,(void*)&npr_idn,
/* Line -   6 */  21,0x40A0,(void*)&npr_iqn,
///* Line -   6 */  22,0x40A0,(void*)&Udc_Set,
/* Line -   6 */  23,0x40A0,(void*)&IgdRefRef,
/* Line -   6 */  24,0x40A0,(void*)&IgqRefRef,
/* Line -   6 */  25,0x40A0,(void*)&IgdRef,
/* Line -   6 */  26,0x40A0,(void*)&IgqRef,
/* Line -   6 */  27,0x40A0,(void*)&Id_Loop_Out,
/* Line -   6 */  28,0x40A0,(void*)&Iq_Loop_Out,
/* Line -   6 */  29,0x40A0,(void*)&Idn_Loop_Out,
/* Line -   6 */  30,0x40A0,(void*)&Iqn_Loop_Out,
/* Line -   6 */  31,0x40A0,(void*)&UgCtrl_d,
/* Line -   6 */  32,0x40A0,(void*)&UgCtrl_q,
/* Line -   6 */  33,0x40A0,(void*)&UgCtrl_dn,
/* Line -   6 */  34,0x40A0,(void*)&UgCtrl_qn,
/* Line -   6 */  35,0x40A0,(void*)&UgCtrl_AlAll,
/* Line -   6 */  36,0x40A0,(void*)&UgCtrl_BeAll,
/* Line -   6 */  37,0x40A0,(void*)&npr_ifa,
/* Line -   6 */  38,0x40A0,(void*)&npr_ifb,
/* Line -   6 */  39,0x40A0,(void*)&npr_ifc,
/* Line -   6 */  40,0x40A0,(void*)&FlowRate,
};
const struct RTMType sParaData_Group13[]=
{
// energy
/* Line-  6 */ 42,0x0000,(void*)&GroupCtrl[13],
};
const struct RTMType sParaData_Group14[]=
{
/* Line -   6 */   0,0x0000,(void*)&GroupCtrl[14],
};
const struct RTMType sParaData_Group15[]=
{
/* Line -   6 */  3,0x0004,(void*)&GroupCtrl[15],
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
/* Line -  10 */  0,0x0000,(void*)&GroupCtrl[17],
};
const struct RTMType sParaData_Group18[]=
{
/* Line -  10 */ 0,0x0000,(void*)&GroupCtrl[18],
};
const struct RTMType sParaData_Group19[]=
{
/* Line -   6 */ 18,0x0000,(void*)&GroupCtrl[19],

/* Line -  10 */  1,0x0020,(void*)&asd,
/* Line -  10 */  2,0x0020,(void*)&InterADValue[1],
/* Line -  10 */  3,0x0020,(void*)&InterADValue[2],
/* Line -  10 */  4,0x0020,(void*)&InterADValue[3],
/* Line -  10 */  5,0x0020,(void*)&InterADValue[4],
/* Line -  10 */  6,0x0020,(void*)&InterADValue[5],
/* Line -  10 */  7,0x0020,(void*)&InterADValue[6],
/* Line -  10 */  8,0x0020,(void*)&InterADValue[7],
/* Line -  10 */  9,0x0020,(void*)&InterADValue[8],
/* Line -  10 */ 10,0x0020,(void*)&InterADValue[9],
/* Line -  10 */ 11,0x0020,(void*)&InterADValue[10],
/* Line -  10 */ 12,0x0020,(void*)&InterADValue[11],
/* Line -  10 */ 13,0x0020,(void*)&InterADValue[12],
/* Line -  10 */ 14,0x0020,(void*)&InterADValue[13],
/* Line -  10 */ 15,0x0020,(void*)&InterADValue[14],
/* Line -  10 */ 16,0x0020,(void*)&InterADValue[15],
///* Line -  10 */  1,0x0020,(void*)&ADValue[0],
///* Line -  10 */  2,0x0020,(void*)&ADValue[1],
///* Line -  10 */  3,0x0020,(void*)&ADValue[2],
///* Line -  10 */  4,0x0020,(void*)&ADValue[3],
///* Line -  10 */  5,0x0020,(void*)&ADValue[4],
///* Line -  10 */  6,0x0020,(void*)&ADValue[5],
///* Line -  10 */  7,0x0020,(void*)&ADValue[6],
///* Line -  10 */  8,0x0020,(void*)&ADValue[7],
///* Line -  10 */  9,0x0020,(void*)&ADValue[8],
///* Line -  10 */ 10,0x0020,(void*)&ADValue[9],
///* Line -  10 */ 11,0x0020,(void*)&ADValue[10],
///* Line -  10 */ 12,0x0020,(void*)&ADValue[11],
///* Line -  10 */ 13,0x0020,(void*)&ADValue[12],
///* Line -  10 */ 14,0x0020,(void*)&ADValue[13],
///* Line -  10 */ 15,0x0020,(void*)&ADValue[14],
///* Line -  10 */ 16,0x0020,(void*)&ADValue[15],
/* Line -  18 */ 17,0x0040,(void*)&IOInput1.DataInt,
/* Line -  18 */ 18,0x0040,(void*)&IOInput2.DataInt,
};
const struct RTMType sParaData_Group20[]=
{
/* Total-  6 */  0,0x0000,(void*)&GroupCtrl[20],
};

const struct RTMType sParaData_Group21[]=
{
/* Line -   1 */  15,0x0004,(void*)&GroupCtrl[21],
/* Line -   1 */   1,0x0023,(void*)&Curr_Har1_En,
/* Line -   2 */   2,0x0023,(void*)&Curr_Har1_Order,
/* Line -   3 */   3,0x00A3,(void*)&Curr_Har1_Loop_Kp,
/* Line -   4 */   4,0x00A3,(void*)&Curr_Har1_Loop_Ki,
/* Line -   5 */   5,0x00A3,(void*)&Curr_Har1_Loop_Lim,
/* Line -   6 */   6,0x00A0,(void*)&Curr_Har1_dErr,
/* Line -   7 */   7,0x00A0,(void*)&Curr_Har1_qErr,
/* Line -   8 */   8,0x00A0,(void*)&Curr_Har1_d,
/* Line -   9 */   9,0x00A0,(void*)&Curr_Har1_q,
/* Line -  10 */  10,0x00A0,(void*)&Curr_Har1_df,
/* Line -  11 */  11,0x00A0,(void*)&Curr_Har1_qf,
/* Line -  12 */  12,0x00A0,(void*)&Curr_Har1_dLoop_Out,
/* Line -  13 */  13,0x00A0,(void*)&Curr_Har1_qLoop_Out,
/* Line -  14 */  14,0x00A0,(void*)&Curr_Har1_alOut,
/* Line -  15 */  15,0x00A0,(void*)&Curr_Har1_beOut,

/* Line -  16 */  16,0x0023,(void*)&Curr_Har2_En,
/* Line -  17 */  17,0x0023,(void*)&Curr_Har2_Order,
/* Line -  18 */  18,0x00A3,(void*)&Curr_Har2_Loop_Kp,
/* Line -  19 */  19,0x00A3,(void*)&Curr_Har2_Loop_Ki,
/* Line -  20 */  20,0x00A3,(void*)&Curr_Har2_Loop_Lim,

/* Line -  21 */  21,0x00A0,(void*)&Curr_Har2_dErr,
/* Line -  22 */  22,0x00A0,(void*)&Curr_Har2_qErr,
/* Line -  23 */  23,0x00A0,(void*)&Curr_Har2_d,
/* Line -  24 */  24,0x00A0,(void*)&Curr_Har2_q,
/* Line -  25 */  25,0x00A0,(void*)&Curr_Har2_df,
/* Line -  26 */  26,0x00A0,(void*)&Curr_Har2_qf,
/* Line -  27 */  27,0x00A0,(void*)&Curr_Har2_dLoop_Out,
/* Line -  28 */  28,0x00A0,(void*)&Curr_Har2_qLoop_Out,
/* Line -  29 */  29,0x00A0,(void*)&Curr_Har2_alOut,
/* Line -  30 */  30,0x00A0,(void*)&Curr_Har2_beOut,
};
const struct RTMType sParaData_Group22[]=
{
/*Total- 1 - */  0,0x0000,(void*)&GroupCtrl[22],
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
/* Total-  28 */ 14,0x0000,(void*)&GroupCtrl[24],
/* Line -   1 */  1,0x4041,(void*)&TestFlag,
/* Line -   1 */  2,0x4041,(void*)&TestIOFlag,
/* Line -   1 */  3,0x4040,(void*)&IOInputFpga1.DataInt,
/* Line -   1 */  4,0x4041,(void*)&TestIOout.DataInt,
/* Line -   6 */  5,0x40A0,(void*)&npr_udc,
/* Line -   6 */  6,0x40A0,(void*)&npr_uab,
/* Line -   6 */  7,0x40A0,(void*)&npr_ubc,
/* Line -   6 */  8,0x40A0,(void*)&npr_uca,
/* Line -   6 */  9,0x40A0,(void*)&npr_uin,
/* Line -   6 */ 10,0x40A0,(void*)&npr_ia,
/* Line -   6 */ 11,0x40A0,(void*)&npr_ib,
/* Line -   6 */ 12,0x40A0,(void*)&npr_ic,
/* Line -   6 */ 13,0x40A0,(void*)&npr_ifa,
/* Line -   6 */ 14,0x40A0,(void*)&npr_ifb,
/* Line -   6 */ 15,0x40A0,(void*)&npr_ifc,
/* Line -   1 */ 17,0x4041,(void*)&TestPWMFlag,
/* Line -   6 */ 18,0x40A1,(void*)&Test_CurrRef,
};

const struct  RTMType sParaData_Group25[]=
{
/* Total-  10 */ 10,0x0004,(void*)&GroupCtrl[25],
/* Line -   1 */  1,0x4043,(void*)&IOOutCtrl1Word.DataInt,
/* Line -   1 */  2,0x4040,(void*)&IOOutput1.DataInt,
/* Line -   1 */  3,0x4040,(void*)&IOOutFpga1.DataInt,
/* Line -   2 */  4,0x4043,(void*)&IOInputEn.DataInt,
/* Line -   3 */  5,0x4043,(void*)&IOInputLogic.DataInt,
/* Line -   4 */  6,0x4043,(void*)&IOInputDisValue.DataInt,
/* Line -   5 */  7,0x4040,(void*)&IOInputFpga1.DataInt,
/* Line -   5 */  8,0x4040,(void*)&IOInput1.DataInt,
/* Line -   1 */  9,0x4043,(void*)&IOOutFpga2.DataInt,
/* Line -   5 */ 10,0x4040,(void*)&IOInputFpga3.DataInt,
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

