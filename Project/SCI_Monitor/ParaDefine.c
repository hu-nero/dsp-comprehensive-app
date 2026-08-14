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
/* Line -  31 */  4, 0x0000, (void*)&GroupCtrl[2],
/* Line -  31 */  1, 0x40c0, (void*)&Conv_Type_Ver,
/* Line -  31 */  2, 0x40c0, (void*)&Soft_Ver,
/* Line -  31 */  3, 0x40c0, (void*)&RTM_File_Ver,
/* Line -  31 */  4, 0x40c0, (void*)&FPGA_Ver,
};
const struct RTMType sParaData_Group3[]=
{
/* Line -  31 */  1,0x0000,(void*)&GroupCtrl[3],
/* Line -  31 */  1,0x0041,(void*)&MainCtrlWord_PC.DataInt,
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
/* Line - 16 */42,0x0004,(void*)&GroupCtrl[6],
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
/* Line -  29 */ 29,0x4043,(void*)&FltCntThr_OverLow_Flow1,
/* Line -  30 */ 30,0x40A3,(void*)&FltThr_OverLow_Flow1,
/* Line - 31 */ 31,0x4043,(void*)&FltCntThr_OverLow_Flow2,
/* Line - 32 */ 32,0x40A3,(void*)&FltThr_OverLow_Flow2,
/* Line - 32 */ 33,0x40A3,(void*)&Auto_Restarttime_Flow,
/* Line - 33 */ 34,0x4043,(void*)&Cap_Ctrl_Word,
/* Line - 33 */ 35,0x4043,(void*)&Can_External_Addr,
/* Line - 33 */ 36,0x40A3,(void*)&FltThr_UnderCurr_Silicon,
/* Line - 33 */ 37,0x40A3,(void*)&FltThr_OverCurr_Silicon,
/* Line - 33 */ 38,0x40A3,(void*)&Auto_Restarttime_Self,
/* Line - 33 */ 39,0x40A3,(void*)&Auto_Restarttime_Para_Request,
/* Line - 33 */ 40,0x4043,(void*)&SysTimeser_back,
/* Line - 33 */ 41,0x40A3,(void*)&Auto_SingParaConv_Time,
/* Line - 33 */ 42,0x4043,(void*)&PS_Monitor_ID,

};
const struct RTMType sParaData_Group7[]=
{
/* Line -  31 */ 0,0x0000,(void*)&GroupCtrl[7],
};
const struct RTMType sParaData_Group8[]=
{
/* Line -  31 */ 0,0x0000,(void*)&GroupCtrl[8],
};
const struct RTMType sParaData_Group9[]=
{
/* Line - 31 */ 0,0x0000,(void*)&GroupCtrl[9],
};
const struct RTMType sParaData_Group10[]=
{
/* Total- 32 */  8,0x0000,(void*)&GroupCtrl[10],
/* Line - 32 */  1,0x40A0,(void*)&P_Work,
/* Line - 32 */  2,0x40A0,(void*)&Q_Work,
/* Line - 32 */  3,0x40A0,(void*)&S_Work,
/* Line - 32 */  4,0x40A0,(void*)&PF_Work,
/* Line - 32 */  5,0x40A0,(void*)&P_Ovh,
/* Line - 32 */  6,0x40A0,(void*)&Q_Ovh,
/* Line - 32 */  7,0x40A0,(void*)&S_Ovh,
/* Line - 32 */  8,0x40A0,(void*)&PF_Ovh,
};
const struct RTMType sParaData_Group11[]=
{
/* Line - 32 */ 18,0x0000,(void*)&GroupCtrl[11],
/* Line - 32 */  1,0x40A0,(void*)&UabSl_rmsf,
/* Line - 32 */  2,0x40A0,(void*)&UbcSl_rmsf,
/* Line - 32 */  3,0x40A0,(void*)&UcaSl_rmsf,
/* Line - 32 */  4,0x40A0,(void*)&UabPar_rmsf,
/* Line - 32 */  5,0x40A0,(void*)&UbcPar_rmsf,
/* Line - 32 */  6,0x40A0,(void*)&UcaPar_rmsf,
/* Line - 32 */  7,0x40A0,(void*)&IaPar_rmsf,
/* Line - 32 */  8,0x40A0,(void*)&IbPar_rmsf,
/* Line - 32 */  9,0x40A0,(void*)&IcPar_rmsf,
/* Line - 32 */ 10,0x40A0,(void*)&IaOvh_rmsf,
/* Line - 32 */ 11,0x40A0,(void*)&IbOvh_rmsf,
/* Line - 32 */ 12,0x40A0,(void*)&IcOvh_rmsf,
/* Line - 32 */ 13,0x40A0,(void*)&UaOut_rmsf,
/* Line - 32 */ 14,0x40A0,(void*)&UbOut_rmsf,
/* Line - 32 */ 15,0x40A0,(void*)&UcOut_rmsf,
/* Line - 32 */ 16,0x40A0,(void*)&UabOut_rmsf,
/* Line - 32 */ 17,0x40A0,(void*)&UbcOut_rmsf,
/* Line - 32 */ 18,0x40A0,(void*)&UcaOut_rmsf,
};
const struct RTMType sParaData_Group12[]=
{
 /* Line -  32 */ 22,0x0000,(void*)&GroupCtrl[12],
 /* Line -   6 */ 1,0x4020,(void*)&Status_NPRRun,
 /* Line -   6 */ 2,0x4040,(void*)&MainStusWord.DataInt,
 /* Line -   6 */ 3,0x4040,(void*)&MainSta2Word.DataInt,
 /* Line - 32 */  4,0x40A0,(void*)&IaPar,
 /* Line - 32 */  5,0x40A0,(void*)&IbPar,
 /* Line - 32 */  6,0x40A0,(void*)&IcPar,
 /* Line - 32 */  7,0x40A0,(void*)&IaOvh,
 /* Line - 32 */  8,0x40A0,(void*)&IbOvh,
 /* Line - 32 */  9,0x40A0,(void*)&IcOvh,
 /* Line - 32 */ 10,0x40A0,(void*)&UabSl,
 /* Line - 32 */ 11,0x40A0,(void*)&UbcSl,
 /* Line - 32 */ 12,0x40A0,(void*)&UcaSl,
 /* Line - 32 */ 13,0x40A0,(void*)&UabPar,
 /* Line - 32 */ 14,0x40A0,(void*)&UbcPar,
 /* Line - 32 */ 15,0x40A0,(void*)&UcaPar,
 /* Line - 32 */ 16,0x40A0,(void*)&UaOut,
 /* Line - 32 */ 17,0x40A0,(void*)&UbOut,
 /* Line - 32 */ 18,0x40A0,(void*)&UcOut,
 /* Line - 32 */ 19,0x40A0,(void*)&UabOut,
 /* Line - 32 */ 20,0x40A0,(void*)&UbcOut,
 /* Line - 32 */ 21,0x40A0,(void*)&UcaOut,
 /* Line - 32 */ 22,0x40A0,(void*)&uFlowRate,
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
/* Line -   6 */  0,0x0000,(void*)&GroupCtrl[15],
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

};
const struct RTMType sParaData_Group20[]=
{
/* Total-  6 */  0,0x0000,(void*)&GroupCtrl[20],
};

const struct RTMType sParaData_Group21[]=
{
/* Total-  6 */  0,0x0000,(void*)&GroupCtrl[21],
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
/* Total-  28 */  8,0x0004,(void*)&GroupCtrl[24],
/* Line -   1 */  1,0x4043,(void*)&IOOut2CtrlWord.DataInt,
/* Line -   1 */  2,0x4040,(void*)&IOOutput2.DataInt,
/* Line -   1 */  3,0x4040,(void*)&IOOutFpga2.DataInt,

/* Line -   2 */  4,0x4043,(void*)&IOInput2En.DataInt,
/* Line -   3 */  5,0x4043,(void*)&IOInput2Logic.DataInt,
/* Line -   4 */  6,0x4043,(void*)&IOInput2DisValue.DataInt,
/* Line -   5 */  7,0x4040,(void*)&IOInput2Fpga.DataInt,
/* Line -   5 */  8,0x4040,(void*)&IOInput2.DataInt,
};

const struct  RTMType sParaData_Group25[]=
{
/* Total-  10 */ 8,0x0004,(void*)&GroupCtrl[25],
/* Line -   1 */  1,0x4043,(void*)&IOOut1CtrlWord.DataInt,
/* Line -   1 */  2,0x4040,(void*)&IOOutput1.DataInt,
/* Line -   1 */  3,0x4040,(void*)&IOOutFpga1.DataInt,

/* Line -   2 */  4,0x4043,(void*)&IOInput1En.DataInt,
/* Line -   3 */  5,0x4043,(void*)&IOInput1Logic.DataInt,
/* Line -   4 */  6,0x4043,(void*)&IOInput1DisValue.DataInt,
/* Line -   5 */  7,0x4040,(void*)&IOInput1Fpga.DataInt,
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

