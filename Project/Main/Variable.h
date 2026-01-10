#ifndef _VARIBALE_H_
#define _VARIBALE_H_


#include "DSP2833x_Device.h"
#include "Arithmetic\Arithmetic.h"
#include "Arithmetic\NprCtrl.h"
#include "SCI_monitor\softscope.h"
#include "Custom_GlobalMacro.H"

////////////////////////////////////////////////////////////////
// SYSCLKOUT = 150000000
// x = HSPCLKDIV = 1
// y = CLKDIV = 0
// x = 0;  TBCLK = SYSCLKOUT / (2^y)
// x > 0;  TBCLK = SYSCLKOUT / (2x X 2^y)
#define PWM_BlockClock     75000000
////////////////////////////////////////////////////////////////
#define PWM_SwitchFreq     9300
#define PWM_ControlFreq    9300
#define PWM_CountMode      TB_COUNT_UPDOWN

#define PWM_PeriodReg      (PWM_BlockClock/PWM_SwitchFreq/PWM_CountMode)
#define PWM_MinPulse       50*(PWM_BlockClock/1e6)
#define PWM_DeadSpace      3*(PWM_BlockClock/1e6)

#define PI       3.141592654
#define _2PI     6.283185307
#define _PIdiv6  0.523598776
#define _2PIdiv3 2.09439510
#define PIdiv3   1.047197551
#define Sqrt_2   1.414213562
#define Sqrt_3   1.732050808
#define SOGI_k   1.414213562
#define SOGI_Gamma  50
extern int FltCnt_OverTemp_Transformer,FltCnt_OverTemp_Inverter,FltCnt_OverTemp_Rectifier;
extern float FltThr_OverTemp_Transformer,FltThr_OverTemp_Inverter,FltThr_OverTemp_Rectifier;
extern float Rectifier_reactor_temp;
extern float Inverter_reactor_temp;
extern float Transformer_temp;
extern int Zqwl_Temp_Error;
extern int Can_Temp_Flag;

extern float FpgaIdRef, FpgaIqRef;
extern int FpgaVerCode;
extern int DeviceNum;

extern float MotorVolt_Imbalance;
extern float MotorCurr_Imbalance;
extern int ParaSaveFlag, ParaSaveFlagOld;
extern int FanIgbt_HMI;
extern float FlowRate;
extern float UdcOffset_Outer;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
extern union MASTER_TO_SLAVE_Type MasToSlaWord, Mas1ToSlaWord, Mas2ToSlaWord;
extern union SLAVE_TO_MASTER_Type SlaToMasWord, SlaToMas1Word, SlaToMas2Word;
extern union DataBit_Type FpgaSofaFault;
extern struct Heartbeat_Check_Type HeartbeatCheck1, HeartbeatCheck2;;
////////////////////////////////////////////////////////////////////
///////////////////////// SOGI Para      ///////////////////////////
////////////////////////////////////////////////////////////////////
extern float SOGI_FreqMea, SOGI_FreqMeaf;
extern float SOGI_Freqff;
extern float SOGI_wFreqErr;
extern float UgalDire, UgalQuar, UgbeDire, UgbeQuar;
extern float UgalErr, UgbeErr;
////////////////////////////////////////////////////////////////////
///////////////////////// Realtime Para  ///////////////////////////
////////////////////////////////////////////////////////////////////
// DC Voltage
extern Uint16 Test_Time1ms;
extern float mpr_udc, mpr_udcf;
////////////////////////////////////////////////////////////////////
// Grid Voltage 3_Phase_Static
extern float mpr_ua, mpr_ub, mpr_uc;
extern float mpr_uab, mpr_ubc, mpr_uca;
extern float mpr_uabf, mpr_ubcf, mpr_ucaf;
// Grid Voltage 2_Phase_Rotate
extern float mpr_ual, mpr_ube;
extern float UGal_D90,UGbe_D90;
extern float mpr_ual_p, mpr_ube_p;
extern float mpr_ual_n, mpr_ube_n;
// Grid Voltage 2_Phase_Static
extern float mpr_ud, mpr_uq;
extern float mpr_udf, mpr_uqf;
extern float mpr_udp, mpr_uqp;
extern float mpr_udn, mpr_uqn;
extern float mpr_udpf, mpr_uqpf;
extern float mpr_udnf, mpr_uqnf;
////////////////////////////////////////////////////////////////////
// Grid Current 3_Phase_Static
extern float mpr_ia, mpr_ib, mpr_ic;
extern float mpr_isum;
extern float mpr_ipeak;
// Grid Current 2_Phase_Rotate
extern float mpr_ial, mpr_ibe;
extern float Igal_D90, Igbe_D90;
extern float Igal_N, Igbe_N;
// Grid Current 2_Phase_Static
extern float mpr_id, mpr_iq;
extern float mpr_iod, mpr_ioq;
extern float mpr_iodf, mpr_ioqf;
extern float mpr_idn, mpr_iqn;
extern float mpr_idf, mpr_iqf;
extern float mpr_idnf, mpr_iqnf;
////////////////////////////////////////////////////////////////////
// Filter Current 3_Phase_Static
extern float npr_ifa, npr_ifb, npr_ifc;
// Grid Current 2_Phase_Rotate
extern float npr_ifal, npr_ifbe;
// Grid Current 2_Phase_Static
extern float npr_ifd, npr_ifq;
////////////////////////////////////////////////////////////////////
//////////////////////// Ctrl Freq Angle ///////////////////////////
////////////////////////////////////////////////////////////////////
extern float mpr_freq, mpr_freqf;
extern float GridVolt_Angle;
extern float GridCurrent_Angle, GridUconvert_Angle;
extern float MotorVolt_Angle;
extern float MotorVolt_Freq;
extern float MotorVolt_AngleComp;
////////////////////////////////////////////////////////////////////
///////////////////////// Ctrl Voltage  ////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Current 2_Phase_Rotate
extern float UsCtrl_Al, UsCtrl_Be;
extern float UsCtrl_Aln, UsCtrl_Ben;
extern float UsCtrl_AlG, UsCtrl_BeG;
extern float UsCtrl_AlGn, UsCtrl_BeGn;
extern float UsCtrl_AlAll, UsCtrl_BeAll;
// Grid Current 2_Phase_Static
extern float UsCtrl_d, UsCtrl_q;
extern float UsCtrl_dn, UsCtrl_qn;
////////////////////////////////////////////////////////////////////
///////////////////////// RMS Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage RMS
extern float UGab_HalfMemory[10];
extern float UGbc_HalfMemory[10];
extern float UGca_HalfMemory[10];
extern float Ua_Rms, Ub_Rms, Uc_Rms;
extern float Ua_Rmsf, Ub_Rmsf, Uc_Rmsf;
extern float Uab_Rms, Ubc_Rms, Uca_Rms;
extern float Uab_Rmsf, Ubc_Rmsf, Uca_Rmsf;
extern float vLine_max, vLine_min;
extern float vLine_avg;
extern float UalbeP_Rms, UalbeN_Rms;
extern float UalbeP_Rmsf, UalbeN_Rmsf;
// Grid Current RMS
extern float IGa_HalfMemory[10];
extern float IGb_HalfMemory[10];
extern float IGc_HalfMemory[10];
extern float Ia_Rms, Ib_Rms, Ic_Rms;
extern float Ia_Rmsf, Ib_Rmsf, Ic_Rmsf;
extern float Iphase_min, Iphase_max;
extern float Iphase_avg;
// Filter Current RMS
extern float IGaf_HalfMemory[10];
extern float IGbf_HalfMemory[10];
extern float IGcf_HalfMemory[10];
extern float Iaf_Rms, Ibf_Rms, Icf_Rms;
extern float Iaf_Rmsf, Ibf_Rmsf, Icf_Rmsf;
extern float Ifphase_min, Ifphase_max;
// Power RMS
extern float NPR_Power, NPR_ReactPower;
extern float Power_Factor;
extern float GridVolt_Imbalance;
extern float MPR_Ps, MPR_Qs, MPR_Ss;
extern float MPR_PsACK, MPR_QsACK, MPR_SsACK;
extern float Volt_Imbalance, Curr_Imbalance;

extern float Ioa_Rms, Iob_Rms, Ioc_Rms;
extern float UabBus1_Rms, UbcBus1_Rms, UcaBus1_Rms;
extern float UabBus2_Rms, UbcBus2_Rms, UcaBus2_Rms;

extern float Ioa_Rmsf, Iob_Rmsf, Ioc_Rmsf;
extern float UabBus1_Rmsf, UbcBus1_Rmsf, UcaBus1_Rmsf;
extern float UabBus2_Rmsf, UbcBus2_Rmsf, UcaBus2_Rmsf;
////////////////////////////////////////////////////////////////////
////////////////////// Filter Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage
extern float UGab_BPF_X[3], UGab_BPF_Y[3];
extern float UGbc_BPF_X[3], UGbc_BPF_Y[3];
extern float UGal_PDF_X[3], UGal_PDF_Y[3];
extern float UGbe_PDF_X[3], UGbe_PDF_Y[3];
// Grid Current
extern float Igal_PDF_X[3], Igal_PDF_Y[3];
extern float Igbe_PDF_X[3], Igbe_PDF_Y[3];
extern float Idnx_table[3], Idny_table[3];
extern float Iqnx_table[3], Iqny_table[3];
// Filter Current
extern float Ifal_BPF_X[3], Ifal_BPF_Y[3];
extern float Ifbe_BPF_X[3], Ifbe_BPF_Y[3];
////////////////////////////////////////////////////////////////////
////////////////////// Current Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
extern float Idq_Loop_Kp, Idq_Loop_Ki;
extern float Id_Loop_Lim, Iq_Loop_Lim, Idq_Loop_Lim;
extern float Id_Loop_Kp, Id_Loop_Ki, Id_Loop_Kd;
extern float Errtotal_Id, Id_Loop_Out;

extern float Iq_Loop_Kp, Iq_Loop_Ki, Iq_Loop_Kd;
extern float Errtotal_Iq, Iq_Loop_Out;

extern float Idn_Loop_Kp, Idn_Loop_Ki, Idn_Loop_Kd;
extern float Errtotal_Idn, Idn_Loop_Out, Idn_Loop_Lim;
extern float Iqn_Loop_Kp, Iqn_Loop_Ki, Iqn_Loop_Kd;
extern float Errtotal_Iqn, Iqn_Loop_Lim, Iqn_Loop_Out;
////////////////////////////////////////////////////////////////////
////////////////////// Voltage Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
extern float Udc_Loop_Kp1, Udc_Loop_Ki1, Udc_Loop_Kd1;
extern float Udc_Loop_Kp2, Udc_Loop_Ki2, Udc_Loop_Kd2;

extern float Errtotal_Udc;
////////////////////////////////////////////////////////////////////
////////////////////// dcVoltage Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
extern float Udq_Loop_Kp, Udq_Loop_Ki;
extern float Ud_Loop_Kp, Ud_Loop_Ki;
extern float Errtotal_Ud, Ud_Loop_Out;
extern float Uq_Loop_Kp, Uq_Loop_Ki;
extern float Errtotal_Uq, Uq_Loop_Out;
extern float Udq_Loop_Lim;

extern float Udn_Loop_Kp, Udn_Loop_Ki;
extern float Errtotal_Udn, Udn_Loop_Out;
extern float Uqn_Loop_Kp, Uqn_Loop_Ki;
extern float Errtotal_Uqn, Uqn_Loop_Out;
extern float Udqn_Loop_Lim;

extern float Ud_FeedFwd, Uq_FeedFwd;
////////////////////////////////////////////////////////////////////
//////////////////////   PLL Loop    ///////////////////////////////
////////////////////////////////////////////////////////////////////
extern float PLL_Kp, PLL_Ki, PLL_Kd;
extern float PLL_Lim;
extern float PLL_ErrTotal;
////////////////////////////////////////////////////////////////////
/////////////////////// Hardware Ctrl  /////////////////////////////
////////////////////////////////////////////////////////////////////
// DI
extern union DataBit_Type IOInputFpga1,IOInputFpga2,IOInputFpga3;
extern union DataBit_Type IOInput1, IOInput2;
extern union DataBit_Type IOInputEn;
extern union DataBit_Type IOInputLogic;
extern union DataBit_Type IOInputDisValue;
// DO
extern union DataBit_Type IOOutFpga1, IOOutFpga1Old;
extern union DataBit_Type IOOutput1, IOOutput1Old;
extern union DataBit_Type IOOutCtrl1Word;
//DSP InterADC Tempr
extern float npr_temphigh_reactor,npr_templow_reactor;
extern float mpr_temphigh, mpr_templow;
extern float Motor_Temphigh, Motor_Templow;
extern int FPGACD4067_SelFlag;
extern float Filter_Cap_Temphigh,Filter_Cap_TempLow;
extern float CtrlThr_ReactorOn_TempH,CtrlThr_ReactorOff_TempH;
extern int InterADValue[16];
extern int NTC_temp[16];
extern int NTC_tempf[16];
extern float IGBT_Temp[16];
extern float VolSample[16];
extern float ResSample[16];
extern float TemprCalc[16];
//pwm
extern int Globle_CMPTable[6];
extern int PWMTestA, PWMTestB, PWMTestC;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
// Alarm MaskWord
extern union DataBit_Type AlarmWord1;
extern union DataBit_Type AlarmWord2;
extern union DataBit_Type AlarmWord3;
extern union DataBit_Type AlarmWord4;
extern union DataBit_Type AlarmWord5;
extern union DataBit_Type AlarmWord6;
extern union DataBit_Type AlarmWord7;
extern union DataBit_Type AlarmWord8;

extern float AlmThr_OverVolt_Work1;
extern float AlmThr_UnderVolt_Work1;
extern float AlmThr_OverVolt_Work2;
extern float AlmThr_UnderVolt_Work2;
extern float AlmThr_OverDvalue_Freq;
extern float AlmThr_OverTemp_IGBT;
extern float AlmThr_OverVolt_Udc;
extern float AlmThr_UnderVolt_Udc;
extern float AlmThr_OverCurr_Irms;
extern float AlmThr_OverImba_Volt;
extern float AlmThr_OverImba_Curr;

extern int AlmCnt_OpearatError;
////////////////////////////////////////////////////////////////////
////////////////////// Falut Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
extern u16 MPR_FaultCode, MPR_AlarmCode;
extern union DataBit_Type ECANA_FaultMask;
extern union DataBit_Type NPR_ConvHWAMWord;
extern union DataBit_Type ECANA_FaultCode;
// Fault Check Timer
extern int FltCnt_OverTime_Unload;
extern int FltCnt_OverTime_PreChar;
extern int FltCnt_OverDvalue_Tempr;
extern int FltCnt_OverTemp_IGBT;
extern int FltCnt_OverTime_MasShift;
extern int FltCnt_OverVolt_Work1;
extern int FltCnt_UnderVolt_Work1;
extern int FltCnt_OverVolt_Work2;
extern int FltCnt_UnderVolt_Work2;
extern int FltCnt_UnderVolt_Udc;
extern int FltCnt_OverVolt_Udc;
extern int FltCnt_OverTime_ACB;
extern int FltCnt_OverTime_FCC;
extern int FltCnt_OverTime_FanIgbt;
extern int FltCnt_OverTime_FanCap;
extern int FltCnt_OverCurr_Ipeak;
extern int FltCnt_OverCurr_Irms;
extern int FltCnt_OverCurr_CapIrms;
extern int FltCnt_OverDvalue_Freq;
extern int FltCnt_UnderFreq_Grid;
extern int FltCnt_OverCurr_Isum;
extern int FltCnt_VolgImba_Grid;
extern int FltCnt_OverTime_LockPWM;
extern int FltCnt_ToggHigh_CAN;
extern int FltCnt_ToggLow_CAN;
extern int FltCnt_OverTime_Cap40uF;
extern int FltCnt_OverTime_Cap20uF;
extern int FltCnt_OverTime_NprRun;
extern int FltCnt_OverImba_Volt;
extern int FltCnt_OverImba_Curr;
extern int FltCnt_OverTime_ACB1Out;
extern int FltCnt_OverTime_ACB2Out;
extern int FltCnt_OverTime_ACB3Out;
extern int FltCnt_OverLow_FlowRate;
// Fault threshold
extern float FltThr_OverVolt_Work;
extern float FltThr_UnderVolt_Work;
extern float FltThr_OverVolt_Work1;
extern float FltThr_UnderVolt_Work1;
extern float FltThr_OverVolt_Work2;
extern float FltThr_UnderVolt_Work2;
extern float FltThr_VoltImba_Grid;
extern float FltThr_OverDvalue_Freq;
extern float FltThr_UnderFreq_Grid;
extern float FltThr_OverVolt_Udc;
extern float FltThr_OverVoltEst_Udc;
extern float FltThr_UnderVolt_Udc;
extern float FltThr_OverTime_PreChar;
extern float FltThr_OverCurr_Ipeak;
extern float FltThr_OverCurrEst_Ipeak;
extern float FltThr_OverCurr_Irms;
extern float FltThr_OverCurr_Isum;
extern float FltThr_OverCurr_CapIrms;
extern float FltThr_OverTemp_IGBT;
extern float FltThr_OverImba_Volt;
extern float FltThr_OverImba_Curr;
// Fault Word
extern union DataBit_Type MPR_HWFMWord0;
extern union DataBit_Type MPR_HWFMWord1;
extern union DataBit_Type MPR_HWFMWord2;
extern union DataBit_Type MPR_HWFMWord3;
extern union DataBit_Type MPR_HWFMWord4;
extern union DataBit_Type MPR_HWFMWord5;
extern union DataBit_Type MPR_HWFMWord6;
extern union DataBit_Type MPR_HWFMWord7;
// Fault MaskWord
extern union DataBit_Type FaultWord1;
extern union DataBit_Type FaultWord2;
extern union DataBit_Type FaultWord3;
extern union DataBit_Type FaultWord4;
extern union DataBit_Type FaultWord5;
extern union DataBit_Type FaultWord6;
extern union DataBit_Type FaultWord7;
extern union DataBit_Type FaultWord8;
////////////////////////////////////////////////////////////////////
////////////////////// Current  Har      ///////////////////////////
////////////////////////////////////////////////////////////////////
extern float Curr_Har1_d_LPF_X[3],Curr_Har1_d_LPF_Y[3];
extern float Curr_Har1_q_LPF_X[3],Curr_Har1_q_LPF_Y[3];
extern int Curr_Har1_En;
extern int Curr_Har1_Order;
extern float Curr_Har1_d,Curr_Har1_q;
extern float Curr_Har1_df,Curr_Har1_qf;
extern float Curr_Har1_dErr,Curr_Har1_qErr;
extern float Curr_Har1_dErrtotal,Curr_Har1_qErrtotal;
extern float Curr_Har1_alOut,Curr_Har1_beOut;
extern float Curr_Har1_Loop_Kp,Curr_Har1_Loop_Ki,Curr_Har1_Loop_Lim;
extern float Curr_Har1_dLoop_Out,Curr_Har1_qLoop_Out;

extern float Curr_Har2_d_LPF_X[3], Curr_Har2_d_LPF_Y[3];
extern float Curr_Har2_q_LPF_X[3], Curr_Har2_q_LPF_Y[3];
extern int Curr_Har2_En;
extern int Curr_Har2_Order;
extern float Curr_Har2_d, Curr_Har2_q;
extern float Curr_Har2_df, Curr_Har2_qf;
extern float Curr_Har2_dErr, Curr_Har2_qErr;
extern float Curr_Har2_dErrtotal, Curr_Har2_qErrtotal;
extern float Curr_Har2_alOut, Curr_Har2_beOut;
extern float Curr_Har2_Loop_Kp, Curr_Har2_Loop_Ki, Curr_Har2_Loop_Lim;
extern float Curr_Har2_dLoop_Out, Curr_Har2_qLoop_Out;
////////////////////////////////////////////////////////////////////
///////////////////////// System Para  /////////////////////////////
////////////////////////////////////////////////////////////////////
extern union DSP_TO_FPGA_Word_Type DspToFpgaWord;
extern union FPGA_TO_DSP_Word_Type FpgaToDspWord;
extern int Start_WaitTime, Start_WaitTimeSet;
extern int Flag_DspRdFpga, Flag_DspWrFpga;
// Ctrl Status
extern union Power_Ctrl_Word_BitType PowerCtrlWord;
extern union Power_Status_Word_BitType PowerStatusWord;
extern union Main_Control_Word_Type MainCtrlWord;
extern union Main_Control_Word_Type MainCtrlWord_CAN;
extern union Main_Control_Word_Type MainCtrlWord_PC;
extern union Main_Control_Word_Type MainCtrlWord_HMI;
extern union Main_Status_Word_Type MainStusWord,MainStusWordOld;
extern union Control_Mode_Word_Type CtrlModeWord;
extern union Break_Ctrl_Word_Type BreakCtrlWord;
extern union Break_Status_Word_Type BreakStatusWord;
extern int UnloadCtrl_PC;
extern union DataBit_Type FPGACtrlWord;
extern float VoltFrontfeed_Coeff, CurrFrontfeed_Coeff;
extern int Manu_Oper_Force;
// PowerRef
extern float KVARRef_Sys, KWRefRef_Sys;
extern float KVARRef_CAN, KWRefRef_CAN;
extern float KVARRef_PC, KWRefRef_PC;
extern float CurrRef_InductLoad;
// ActivePower Ref
extern float KwRef;
extern float KW_Lim;
extern float kW_cal_step;
// ReactivePower Ref
extern float KVar_Lim;
extern float KVar_filter_Comp;
// Curr PriorityMode
extern int Id_Priority;
extern int IdqStep_CountMax;
extern float Ig_Lim;
extern int IdqStep_Count;
// Active Current Ref
extern float IdRef_Outer, IqRef_Outer;
extern float UdRefRef_Outer, UdRefRefOld_Outer;
extern float UdRefRef_Outer;
extern float IgdRefRef, IgdRef;
extern float Igd_Lim;
extern float Id_cal_step;
// Reactive Current Ref
extern float IgqRefRef, IgqRef;
extern float Igq_Lim;
extern float Iq_cal_step;
//
extern float MotorFreqRef_PC, MotorFreqRef_CAN, MotorFreqRef_HMI;

extern float UsdRefRef_Buf;
extern float UsdRefRef_PC, UsdRefRef_CAN, UsdRefRef_HMI;
extern float UsdRefRefOld_PC, UsdRefRefOld_CAN, UsdRefRefOld_HMI;
extern float MotorFreqRef, MotorFreqRefOld;
extern float FpgaMotorFreq;
extern float Ud_cal_step;
//
extern u16 UdqStep_Count, UdqStep_CountMax;
extern float Usd_Lim;
extern float UdRef_All, UqRef_All;
extern float UsdRefRef, UsdRef;
extern float UsqRefRef, UsqRef;
// Fastchar Ref
// Fastchar Ref
extern Uint16 FastChar_PhaseSel;
extern Uint16 FastChar_RiseTime, FastChar_HoleTime, FastChar_DownTime;
extern float FastChar_HighVolt, FastChar_AfterVolt;
extern float FastChar_ExitCurrSet;
extern int FastChar_RunStatus;
extern int FastChar_CtrlWord, FastChar_StopWord;
extern float FastChar_Rate;
extern float FastChar_RiseStep, FastChar_DownStep;
extern Uint16 FastChar_TimeCount;
extern float FastChar_UdRefLock;
extern float FastChar_UdRefRef, FastChar_UdRef;

extern float FastChar_RiseGiven, FastChar_DownGiven;
extern float FastChar_UsdOld, FastChar_UsqOld;
extern float FastChar_DCOffset;
extern long FastChar_CtrlCount, FastChar_HoleCount, FastChar_DownCount;
extern float FastChar_UdCompon, FastChar_UqCompon;
extern int FastChar_1msCount;
// DC Voltage Ref
extern float Udc_SetSet;
extern Uint16 Udc_SetCount, UdcStep_CountMax;
extern float Udc_cal_step;
// Rated Para
extern float RatedPara_GridVoltage;
extern float RatedPara_GridCurrent;
extern float RatedPara_GridFrequency;

extern float MotorSideCapacity;
extern float MotorSideVoltage;
extern float MotorSideCurrent;
// Timer Base
extern int SysCnt_1msBase;
extern int SysCnt_1ms, SysCnt_1msOld;
extern int SysCnt_1sBase;
extern int SysCnt_1s, SysCnt_1sOld;
extern int SysCnt_10msRmsCalc;
// Hardware Para
extern float K_IfcForw;
extern float FilterPara_Induct;
extern float FilterPara_Capacity;
extern float NPR_AngleCompensation;
extern float Para_IoFwdCoeff;
// count
extern float CtrlThr_ChopIn_UdcH;
extern float CtrlThr_ChopOut_UdcL;
extern float CtrlThr_FanIgbtOn_TempH;
extern float CtrlThr_FanIgbtOff_TempL;
extern float CtrlThr_LockPwm_Ipeak;
extern int   CtrlThr_LockPwm_Time;
extern float CtrlThr_UnLockPwm_Ipeak;
// Status
extern int Status_MPRRun;
// Ctrl count
extern int CtrlCnt_StartCheck;
extern int CtrlCnt_IgbtFanOn;
extern int CtrlCnt_LedTogg;
extern int CtrlCnt_RstDelay;
extern int CtrlCnt_ManUnload;
extern int CtrlCnt_IORunRefFeed;
extern int CtrlCnt_IORstRefFeed;
extern int CtrlCnt_IOFltDelay;
extern int CtrlCnt_ReactorFanOn;
extern int CtrlCnt_SofaStart;
extern int CtrlCnt_StartNpr;
extern int CtrlCnt_FltDlyUV1;
extern int CtrlCnt_FltDlyUV2;
extern int CtrlCnt_AddBusOk;
// Flag
extern int Flag_LockPWM;
extern int Flag_IORunRef;
extern int Flag_IORstRef;
extern int Flag_IORstRefOld;
extern int Flag_FanCapOn;
extern int Flag_FanIgbtOn;
extern int Flag_EEPROMCheck;
extern int Flag_RstCmdOld;
extern int Flag_PwmModuMode;
extern int Flag_PowerOn;
extern int Flag_WorkMode, Flag_WorkModeOld;
extern int CAN_SectorRst_Flag, CAN_SectorRst_FlagOld;
extern int Flag_VoltRef, FlagOld_VoltRef;
extern int Flag_FreqRef, FlagOld_FreqRef;
extern int Flag_PowerOnRef;
////////////////////////////////////////////////////////////////////
extern int Rst_Flag_HMI, Rst_Flag_HMIOld;
extern int Operation_ineffective;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
extern int Flash_Capacity;
extern int SDCard_UseCapacity;
extern int Flash_MID[];
extern int Flash_PSNSerial[];

extern unsigned int Flash_Access_1ms;
extern long Flash_Access_Ctrl;
extern long Flash_Fault_Count1ms;
extern int Flash_Step;
extern int Flash_SubStep;
extern int SDSendBuff[],SDRecvBuff[];

/****************************************/
extern void variable_init(void);
extern void variable_NReset_init(void);
extern void Flash_Fault_ResetVarieble(void);
extern u16 FaultMask_Map(u16 Code);
extern void FaultWord_Map(u16 Code);
extern void AlarmWord_Map(u16 Code);
extern void AlarmWord_Clear(u16 Code);
/*******************************************************/
#endif

//===========================================================================
// No more.
//===========================================================================
