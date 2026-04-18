#ifndef _VARIBALE_H_
#define _VARIBALE_H_

#include "DSP2833x_Device.h"
#include "SCI_monitor\softscope.h"
#include "Custom_GlobalMacro.H"


typedef struct
{
	union DataBit_Type StaWord1, StaWord2;
	union DataBit_Type FltWord1, FltWord2;
	union DataBit_Type AlmWord1, AlmWord2;

	s16 UabSl, UbcSl, UcaSl;
	s16 UabPar, UbcPar, UcaPar;
	s16 UaOut, UbOut, UcOut;
	s16 UabOut, UbcOut, UcaOut;
	s16 IaOut, IbOut, IcOut;
	s32 P_Sys, Q_Sys, S_Sys, PF_Sys;
	s16 Ublk, Iblk;
	s16 FLwater;
}	RemoteInput_t;

extern RemoteInput_t RemInputSum, DcsInputSum;

typedef struct
{
	union Remote_Ctrl_Word_Type CtrlWord;
}	RemoteHold_t;

extern RemoteHold_t RemHoldSum, DcsHoldSum;
////////////////////////////////////////////////////////////////////
extern float Iout_Imbalance, Uout_Imbalance;

extern float UabSl, UbcSl, UcaSl;
extern float UabPar, UbcPar, UcaPar;
extern float IaPar, IbPar, IcPar;
extern float IaOvh, IbOvh, IcOvh;
extern float UaOut, UbOut, UcOut;
extern float UabOut, UbcOut, UcaOut;
extern float FpgaMotorFreq;

extern float UabSl_rmsf, UbcSl_rmsf, UcaSl_rmsf;
extern float UabPar_rmsf, UbcPar_rmsf, UcaPar_rmsf;
extern float IaPar_rmsf, IbPar_rmsf, IcPar_rmsf;
extern float IaOvh_rmsf, IbOvh_rmsf, IcOvh_rmsf;
extern float UaOut_rmsf, UbOut_rmsf, UcOut_rmsf;
extern float UabOut_rmsf, UbcOut_rmsf, UcaOut_rmsf;
extern float P_Workf, Q_Workf, S_Workf;
extern float P_Ovhf, Q_Ovhf, S_Ovhf;
extern float IaOut_rmsf, IbOut_rmsf, IcOut_rmsf;

extern float UabSl_sum, UbcSl_sum, UcaSl_sum;
extern float UabPar_sum, UbcPar_sum, UcaPar_sum;
extern float IaPar_sum, IbPar_sum, IcPar_sum;
extern float IaOvh_sum, IbOvh_sum, IcOvh_sum;
extern float UaOut_sum, UbOut_sum, UcOut_sum;
extern float UabOut_sum, UbcOut_sum, UcaOut_sum;
extern float work1_sum,work2_sum,work3_sum,work4_sum;

extern float UabSl_rms, UbcSl_rms, UcaSl_rms;
extern float UabPar_rms, UbcPar_rms, UcaPar_rms;
extern float IaPar_rms, IbPar_rms, IcPar_rms;
extern float IaOvh_rms, IbOvh_rms, IcOvh_rms;
extern float UaOut_rms, UbOut_rms, UcOut_rms;
extern float UabOut_rms, UbcOut_rms, UcaOut_rms;
extern float work1_rms,work2_rms,work3_rms,work4_rms;

extern float P_Work, Q_Work, S_Work, PF_Work;
extern float P_Ovh, Q_Ovh, S_Ovh, PF_Ovh;
extern float P_Sys, Q_Sys, S_Sys, PF_Sys;

extern int Flag_RmsBufFull;

extern unsigned int AlmCnt_UnderVolt_Phase;
extern unsigned int AlmCntThr_UnderVolt_Phase;

extern unsigned int AlmCnt_OverVolt_Work1;
extern unsigned int AlmCntThr_OverVolt_Work1;
extern unsigned int AlmCnt_OverVolt_Work2;
extern unsigned int AlmCntThr_OverVolt_Work2;

extern unsigned int AlmCnt_UnderVolt_Work1;
extern unsigned int AlmCntThr_UnderVolt_Work1;
extern unsigned int AlmCnt_UnderVolt_Work2;
extern unsigned int AlmCntThr_UnderVolt_Work2;

extern unsigned int FltCnt_UnderVolt_Work1;
extern unsigned int FltCntThr_UnderVolt_Work1;
extern unsigned int FltCnt_UnderVolt_Work2;
extern unsigned int FltCntThr_UnderVolt_Work2;

extern unsigned int AlmCnt_OverImba_Volt;
extern unsigned int AlmCntThr_OverImba_Volt;
extern unsigned int FltCnt_OverImba_Volt;
extern unsigned int FltCntThr_OverImba_Volt;

extern unsigned int AlmCnt_OverImba_Curr;
extern unsigned int AlmCntThr_OverImba_Curr;
extern unsigned int FltCnt_OverImba_Curr;
extern unsigned int FltCntThr_OverImba_Curr;

extern unsigned int AlmCnt_OverCurr_Work1;
extern unsigned int FltCnt_OverCurr_Work1;
extern unsigned int AlmCntThr_OverCurr_Work1;
extern unsigned int AlmCnt_OverCurr_Work2;
extern unsigned int FltCnt_OverCurr_Work2;
extern unsigned int AlmCntThr_OverCurr_Work2;

extern unsigned int FltCnt_OverLow_Flow1;
extern unsigned int FltCntThr_OverLow_Flow1;
extern unsigned int FltCnt_OverLow_Flow2;
extern unsigned int FltCntThr_OverLow_Flow2;


extern float AlmThr_UnderVolt_Phase;
extern float AlmThr_OverVolt_Work1;
extern float FltCnt_OverVolt_Work1;
extern float FltThr_OverVolt_Work1;
extern float AlmThr_OverVolt_Work2;
extern float FltCnt_OverVolt_Work2;
extern float FltThr_OverVolt_Work2;
extern float AlmThr_UnderVolt_Work1;
extern float FltThr_UnderVolt_Work1;
extern float AlmThr_UnderVolt_Work2;
extern float FltThr_UnderVolt_Work2;

extern float AlmThr_OverImba_Volt;
extern float FltThr_OverImba_Volt;
extern float AlmThr_OverImba_Curr;
extern float FltThr_OverImba_Curr;

extern float AlmThr_OverCurr_Work1;
extern float FltThr_OverCurr_Work1;
extern float AlmThr_OverCurr_Work2;
extern float FltThr_OverCurr_Work2;
extern float FltThr_OverLow_Flow1;
extern float FltThr_OverLow_Flow2;
extern float Auto_Restarttime_Flow;
extern union DataBit_Type Cap_Ctrl_Word;
extern unsigned int Can_External_Addr;

extern float UabSl_HalfMemory[20], UbcSl_HalfMemory[20], UcaSl_HalfMemory[20];
extern float UabPar_HalfMemory[20], UbcPar_HalfMemory[20], UcaPar_HalfMemory[20];
extern float IaPar_HalfMemory[20], IbPar_HalfMemory[20], IcPar_HalfMemory[20];
extern float IaOvh_HalfMemory[20], IbOvh_HalfMemory[20], IcOvh_HalfMemory[20];
extern float UaOut_HalfMemory[20], UbOut_HalfMemory[20], UcOut_HalfMemory[20];
extern float UabOut_HalfMemory[20], UbcOut_HalfMemory[20], UcaOut_HalfMemory[20];
extern float work1[20], work2[20], work3[20], work4[20];
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
typedef struct
{
//	union REC_Status1_Word_Type RecStaWord1;
	union DataBit_Type RecStaWord1, RecStaWord2;
	union DataBit_Type FltWord1, FltWord2, FltWord3;
	union DataBit_Type AlmWord1, AlmWord2, AlmWord3;

	s16 Uab_in, Ubc_in, Uca_in;
	s16 Ia_in, Ib_in, Ic_in;
	s16 F_in;
	s16 Ublk_in, Iblk_in;
	s16 S_in, P_in, Q_in;
	s16 PF_in;
	u16 E_inH, E_inL;
	s16 Ttrans_in, Treact_in, Treact_out, Tigbt_in;
}	CanCommVar_t;

extern CanCommVar_t CanNprSum;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
extern Uint16 TestFlag, TestIOFlag, TestPWMFlag;
extern float Test_CurrRef;
extern float Test_VoltAngle;
extern union DataBit_Type TestIOout, TestIOoutOld;
extern int asd;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
extern int FltThr_OverWaterCount;
extern float FltThr_OverWater;
extern int Modbus_Flag,Modbus_SendFlag;
extern union DataBit_Type AlarmWord1;
extern union DataBit_Type AlarmWord2;
extern float ChopIn_UdcHThr;
extern float ChopOut_UdcLThr;
extern float CoolFan_On_TempThr, CoolFan_Off_TempThr;
extern float INDCoolFan_On_TempThr, INDCoolFan_Off_TempThr;
extern float Cumulative_Power;
extern int FanRes_HMI, FanInd_HMI;
extern float Flow_Alarm_Low;
extern float uFlowRate, FlowRate;
extern float Ia1_Rmsf, Ib1_Rmsf, Ic1_Rmsf;
extern float NPR_ApparentPower;
extern float NPR_Curr_HighestlimitAlarm;
extern float NPR_Curr_Highlimit;
extern float NPR_GridVol_Imbal_Thr;
extern float NPR_GridVolt_Highlimit;
extern float NPR_GridVolt_Lowlimit;
extern float NPR_Temp_AlarmHighlimit;
extern float NPR_Temp_Highlimit;
extern float NPR_Udc_AlarmHighlimit,NPR_Udc_AlarmLowlimit;
extern float NPR_Udc_Highlimit;
extern float NPR_Udc_LowThr;
extern float npr_udc, npr_udcf, npr_udcff;
extern float NPR_VoltImbalance_Alarm;
extern u16 ParaDownloadCtrlWord;
extern int ParaSaveFlag, ParaSaveFlagOld;
extern int RegZH_Energy, RegZL_Energy, RegDP_Energy;
extern float Temp_High,Temp_Low;
extern float Uab1_Rmsf,Ubc1_Rmsf,Uca1_Rmsf;
extern float vLine_Alarm_Low,vLine_Alarm_High;
////////////////////////////////////////////////////////////////
// SYSCLKOUT = 150000000
// x = HSPCLKDIV = 1
// y = CLKDIV = 0
// x = 0;  TBCLK = SYSCLKOUT / (2^y)
// x > 0;  TBCLK = SYSCLKOUT / (2x X 2^y)
#define PWM_BlockClock     75000000
////////////////////////////////////////////////////////////////
#define PWM_SwitchFreq     15500
#define PWM_ControlFreq    15500
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
extern float FltThr_OverTemp_TransIn,FltThr_OverTemp_ReactOut,FltThr_OverTemp_ReactIn;
extern float Temp_ReactIn;
extern float Temp_ReactOut;
extern float Temp_TransIn;
extern int Zqwl_Temp_Error;
extern int Can_Temp_Flag;
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
extern float npr_udc, npr_udcf;
////////////////////////////////////////////////////////////////////
extern float npr_uin;
// Grid Voltage 3_Phase_Static
extern float npr_uab, npr_ubc, npr_uca;
extern float npr_uabf, npr_ubcf, npr_ucaf;
// Grid Voltage 2_Phase_Rotate
extern float npr_ual, npr_ube;
extern float UGal_D90,UGbe_D90;
extern float npr_ual_p, npr_ube_p;
extern float npr_ual_n, npr_ube_n;
// Grid Voltage 2_Phase_Static
extern float npr_udp, npr_uqp;
extern float npr_udn, npr_uqn;
extern float npr_udpf, npr_uqpf;
extern float npr_udnf, npr_uqnf;
////////////////////////////////////////////////////////////////////
// Grid Current 3_Phase_Static
extern float npr_ia, npr_ib, npr_ic;
extern float npr_isum;
extern float npr_ipeak;
// Grid Current 2_Phase_Rotate
extern float npr_ial, npr_ibe;
extern float Igal_D90, Igbe_D90;
extern float Igal_N, Igbe_N;
// Grid Current 2_Phase_Static
extern float npr_id, npr_iq;
extern float npr_idn, npr_iqn;
extern float npr_idf, npr_iqf;
extern float npr_idnf, npr_iqnf;
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
extern float npr_freq, npr_freqf;
extern float GridVolt_Angle;
extern float GridCurrent_Angle, GridUconvert_Angle;
////////////////////////////////////////////////////////////////////
///////////////////////// Ctrl Voltage  ////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Current 2_Phase_Rotate
extern float UgCtrl_Al, UgCtrl_Be;
extern float UgCtrl_Aln, UgCtrl_Ben;
extern float UgCtrl_AlG, UgCtrl_BeG;
extern float UgCtrl_AlGn, UgCtrl_BeGn;
extern float UgCtrl_AlAll, UgCtrl_BeAll;
// Grid Current 2_Phase_Static
extern float UgCtrl_d, UgCtrl_q;
extern float UgCtrl_dn, UgCtrl_qn;
////////////////////////////////////////////////////////////////////
///////////////////////// RMS Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage RMS
extern float UGab_HalfMemory[20];
extern float UGbc_HalfMemory[20];
extern float UGca_HalfMemory[20];
extern float Uab_Rms, Ubc_Rms, Uca_Rms;
extern float Uab_Rmsf, Ubc_Rmsf, Uca_Rmsf;
extern float vphase_min;
extern float vLine_max, vLine_min, vLine_avg;
extern float UalbeP_Rms, UalbeN_Rms;
extern float UalbeP_Rmsf, UalbeN_Rmsf;
// Grid Current RMS
extern float IGa_HalfMemory[20];
extern float IGb_HalfMemory[20];
extern float IGc_HalfMemory[20];
extern float Ia_Rms, Ib_Rms, Ic_Rms;
extern float Ia_Rmsf, Ib_Rmsf, Ic_Rmsf;
extern float Iphase_min, Iphase_max;
extern float Iphase_avg;
// Filter Current RMS
extern float IGaf_HalfMemory[20];
extern float IGbf_HalfMemory[20];
extern float IGcf_HalfMemory[20];
extern float Iaf_Rms, Ibf_Rms, Icf_Rms;
extern float Iaf_Rmsf, Ibf_Rmsf, Icf_Rmsf;
extern float Ifphase_min, Ifphase_max;
// Power RMS
extern float NPR_Power, NPR_ReactPower;
extern float Power_Factor;
extern float GridVolt_Imbalance;
extern float GridCurr_Imbalance;
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
extern float Id_Loop_Kp, Id_Loop_Ki, Id_Loop_Kd;
extern float Errtotal_Id, Id_Loop_Out, Id_Loop_Lim;
extern float Iq_Loop_Kp, Iq_Loop_Ki, Iq_Loop_Kd;
extern float Errtotal_Iq, Iq_Loop_Lim, Iq_Loop_Out;
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
//////////////////////   PLL Loop    ///////////////////////////////
////////////////////////////////////////////////////////////////////
extern float PLL_Kp, PLL_Ki, PLL_Kd;
extern float PLL_Lim;
extern float PLL_ErrTotal;
////////////////////////////////////////////////////////////////////
/////////////////////// Hardware Ctrl  /////////////////////////////
////////////////////////////////////////////////////////////////////
// DI
extern union DataBit_Type IOInput1Fpga, IOInput2Fpga;
extern union DataBit_Type IOInput1, IOInput2;
extern union DataBit_Type IOInput1En;
extern union DataBit_Type IOInput1Logic;
extern union DataBit_Type IOInput1DisValue;

extern union DataBit_Type IOInput2En;
extern union DataBit_Type IOInput2Logic;
extern union DataBit_Type IOInput2DisValue;
// DO
extern union DataBit_Type IOOutFpga1, IOOutFpga1Old;
extern union DataBit_Type IOOutFpga2, IOOutFpga2Old;
extern union DataBit_Type IOOutput1, IOOutput2;
extern union DataBit_Type IOOut1CtrlWord, IOOut2CtrlWord;
//DSP InterADC Tempr
extern float CtrlThr_TransfOn_TempH,CtrlThr_TransfOff_TempL;
extern float CtrlThr_ReactorOn_TempH,CtrlThr_TransfOff_TempH;
extern float npr_temphigh_reactor,npr_templow_reactor;
extern float Temp_IgbtH, Temp_IgbtL;
extern float Motor_Temphigh, Motor_Templow;
extern int FPGACD4067_SelFlag;
extern float Filter_Cap_Temphigh,Filter_Cap_TempLow;
extern float CtrlThr_FanTransfOn_TempH,CtrlThr_FanTransfOff_TempL;
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

extern union DataBit_Type FanCtrlWord;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
////////////////////// Falut Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
extern UINT NPR_FaultCode;
extern union DataBit_Type ECANA_FaultMask;
extern union DataBit_Type NPR_ConvHWAMWord;
extern union DataBit_Type ECANA_FaultCode;
// Fault Check Timer
extern int FltCnt_OverTime_Unload;
extern int FltCnt_OverTime_PreChar;
extern int FltCnt_OverDvalue_Tempr;
extern int FltCnt_OverTemp_IGBT;
extern int FltCnt_UnderVolt_Grid;
extern int FltCnt_OverVolt_Grid;
extern int FltCnt_UnderVolt_Udc;
extern int FltCnt_OverVolt_Udc;
extern int FltCnt_OverTime_ACB;
extern int FltCnt_OverTime_ACT;
extern int FltCnt_OverTime_FCC;
extern int FltCnt_OverTime_FanIgbt;
extern int FltCnt_OverTime_FanCap;
extern int FltCnt_OverTime_FanReactor;
extern int FltCnt_OverCurr_Ipeak;
extern int FltCnt_OverCurr_Irms;
extern int FltCnt_OverCurr_CapIrms;
extern int FltCnt_OverFreq_Grid;
extern int FltCnt_UnderFreq_Grid;
extern int FltCnt_OverCurr_Isum;
extern int FltCnt_VolgImba_Grid;
extern int FltCnt_CurrImba_Grid;
extern int FltCnt_OverTime_LockPWM;
extern int FltCnt_ToggHigh_CAN;
extern int FltCnt_ToggLow_CAN;
// Fault threshold
extern float FltThr_OverVolt_Grid;
extern float FltThr_UnderVolt_Grid;
extern float FltThr_VoltImba_Grid;
extern float FltThr_CurrImba_Grid;
extern float FltThr_OverFreq_Grid;
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
extern Uint16 FltThr_OverTimt_UnloadIn;
extern float FltThr_OverDvalue_Freq;
// Fault Word
extern union DataBit_Type NPR_HWFMWord0;
extern union DataBit_Type NPR_HWFMWord1;
extern union DataBit_Type NPR_HWFMWord2;
extern union DataBit_Type NPR_HWFMWord3;
extern union DataBit_Type NPR_HWFMWord4;
extern union DataBit_Type NPR_HWFMWord5;
extern union DataBit_Type NPR_HWFMWord6;
extern union DataBit_Type NPR_HWFMWord7;
// Fault MaskWord
extern union DataBit_Type FaultWord1;
extern union DataBit_Type FaultWord2;
extern union DataBit_Type FaultWord3;
extern union DataBit_Type FaultWord4;
extern union DataBit_Type FaultWord5;
extern union DataBit_Type FaultWord6;
extern union DataBit_Type FaultWord7;
extern union DataBit_Type FaultWord8;

extern union DataBit_Type FltMapWord1;
extern union DataBit_Type FltMapWord2;
extern union DataBit_Type FltMapWord3;

extern union Com_Modbus_To_HMI_Word1_Type SwitchWord1;
extern union Com_Modbus_To_HMI_Word2_Type SwitchWord2;
extern union Com_Modbus_To_HMI_Word3_Type SwitchWord3;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
extern float AlmThr_OverCurr_Ipeak;
extern float AlmThr_OverTemp_IGBT;
extern float AlmThr_OverVolt_Grid;
extern float AlmThr_UnderVolt_Grid;
extern float AlmThr_OverVolt_Udc;
extern float AlmThr_UnderVolt_Udc;
extern float AlmThr_UnderLow_Flow;
extern float AlmThr_VoltImba_Grid;
extern float AlmThr_CurrImba_Grid;
extern float AlmThr_OverDvalue_Freq;

extern int AlmCnt_VoltImba_Grid;
extern int AlmCnt_CurrImba_Grid;

extern union DataBit_Type AlarmWord1;
extern union DataBit_Type AlarmWord2;
extern union DataBit_Type AlarmWord3;
extern union DataBit_Type AlarmWord4;
extern union DataBit_Type AlarmWord5;
extern union DataBit_Type AlarmWord6;
extern union DataBit_Type AlarmWord7;
extern union DataBit_Type AlarmWord8;


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
// Ctrl Status

extern union Main_Control_Word_Type MainCtrlWord;
extern union Main_Control_Word_Type MainCtrlWord_CAN;
extern union Main_Control_Word_Type MainCtrlWord_PC;
extern union NPR_Status_Word_Type MainStusWord,MainStusWordOld;
extern union NPR_Status_Word2_Type MainSta2Word;
extern union Control_Mode_Word_Type CtrlModeWord;
extern int UnloadCtrl_PC;
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
extern float IgdRefRef, IgdRef;
extern float Igd_Lim;
extern float Id_cal_step;
// Reactive Current Ref
extern float IgqRefRef, IgqRef;
extern float Igq_Lim;
extern float Iq_cal_step;
// DC Voltage Ref
extern float Udc_SetSet;
extern Uint16 Udc_SetCount, UdcStep_CountMax;
extern float Udc_cal_step, kpDC,kiDC;
// Rated Para
extern float RatedPara_GridVoltage;
extern float RatedPara_GridCurrent;
extern float RatedPara_GridFrequency;
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
// count
extern float CtrlThr_ChopIn_UdcH;
extern float CtrlThr_ChopOut_UdcL;
extern float CtrlThr_FanIgbtOn_TempH;
extern float CtrlThr_FanIgbtOff_TempL;
extern float CtrlThr_FanTransfOn_TempH;
extern float CtrlThr_FanTransfOff_TempL;
extern float CtrlThr_LockPwm_Ipeak;
extern int   CtrlThr_LockPwm_Time;
extern float CtrlThr_UnLockPwm_Ipeak;
// Status
extern int Status_NPRRun;
// Ctrl count
extern int CtrlCnt_IgbtFanOn;
extern int CtrlCnt_LedTogg;
extern int CtrlCnt_RstDelay;
extern int CtrlCnt_ManUnload;
extern int CtrlCnt_IORunRefFeed;
extern int CtrlCnt_IORstRefFeed;
extern int CtrlCnt_IOFltDelay;
extern int CtrlCnt_ReactorFanOn;
extern int CtrlCnt_IORemSingStartFeed;
extern int CtrlCnt_IORemSingEndFeed;
extern int CtrlCnt_IORemParaStartFeed;
extern int CtrlCnt_IORemParaEndFeed;
extern int CtrlCnt_IORemDgssStartFeed;
extern int CtrlCnt_IORemDgssEndFeed;

extern u16 CtrlCnt_IOSingCtrl;
extern u16 CtrlCnt_IOParaCtrl;
extern u16 CtrlCnt_IODgssCtrl;
extern u16 CtrlCnt_IORemSingStartCtrl;
extern u16 CtrlCnt_IORemSingEndCtrl;
extern u16 CtrlCnt_IORemParaStartCtrl;
extern u16 CtrlCnt_IORemParaEndCtrl;
extern u16 CtrlCnt_IORemDgssStartCtrl;
extern u16 CtrlCnt_IORemDgssEndCtrl;

// Flag
extern int Flag_IOSingCtrl;
extern int Flag_IOParaCtrl;
extern int Flag_IODgssCtrl;
extern int Flag_IORstRef;

extern int FlagRem_Combine;
extern int FlagRem_Split;
extern int FlagRem_Reset;
extern int FlagRem_SingOn;
extern int FlagRem_SingOff;
extern int FlagRem_ParaOn;
extern int FlagRem_ParaOff;
extern int FlagRem_DgssOn;
extern int FlagRem_DgssOff;

extern int Flag_LockPWM;
extern int Flag_IORunRef;
extern int Flag_IORstRefOld;
extern int Flag_FanCapOn;
extern int Flag_EEPROMCheck;
extern int Flag_RstCmdOld;
extern int Flag_PwmModuMode;

// Fault Alarm Remap Word
extern union DataBit_Type FltRemapWord1;
extern union DataBit_Type FltRemapWord2;
extern union DataBit_Type FltRemapWord3;
extern union DataBit_Type AlmRemapWord1;
extern union DataBit_Type AlmRemapWord2;
extern union DataBit_Type AlmRemapWord3;
extern union DataBit_Type StaRemapWord1, StaRemapWord2;
////////////////////////////////////////////////////////////////////
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
extern void variable_algorithm_init(void);
extern void variable_NReset_init(void);
extern void Flash_Fault_ResetVarieble(void);
extern u16 FaultMask_Map(u16 Code);
extern void FaultWord_Map(u16 Code);
extern void AlarmWord_Clear(u16 Code);
extern void AlarmWord_Map(u16 Code);
/*******************************************************/
#endif

//===========================================================================
// No more.
//===========================================================================
