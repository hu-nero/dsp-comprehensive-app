
#include "DSP2833x_Device.h"
#include "Custom_GlobalMacro.H"
#include "Comm_Variable.H"
#include "Variable.h"
#include "Arithmetic\Arithmetic.H"
#include "Arithmetic\NprCtrl.h"
#include "IQmathLib.h"
#include "string.h"


float FpgaIdRef=0, FpgaIqRef=0;
int FpgaVerCode=0;
int DeviceNum=0;

float MotorVolt_Imbalance=0;
float MotorCurr_Imbalance=0;
int ParaSaveFlag=0, ParaSaveFlagOld=0;
int FanIgbt_HMI=0;

float FlowRate=0;

float UdcOffset_Outer=0;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
union MASTER_TO_SLAVE_Type MasToSlaWord, Mas1ToSlaWord, Mas2ToSlaWord;
union SLAVE_TO_MASTER_Type SlaToMasWord, SlaToMas1Word, SlaToMas2Word;
union DataBit_Type FpgaSofaFault;
struct Heartbeat_Check_Type HeartbeatCheck1, HeartbeatCheck2;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int FltCnt_OverTemp_Transformer=0,FltCnt_OverTemp_Inverter=0,FltCnt_OverTemp_Rectifier=0;
float FltThr_OverTemp_Transformer=0,FltThr_OverTemp_Inverter=0,FltThr_OverTemp_Rectifier=0;
int Zqwl_Temp_Error = 0;
float Rectifier_reactor_temp =0;
float Inverter_reactor_temp=0;
float Transformer_temp = 0;
int Can_Temp_Flag = 0;
float CtrlThr_ReactorOn_TempH=0, CtrlThr_ReactorOff_TempH=0;
////////////////////////////////////////////////////////////////////
///////////////////////// SOGI Para      ///////////////////////////
////////////////////////////////////////////////////////////////////
Uint16 Test_Time1ms = 0;
float SOGI_FreqMea=50, SOGI_FreqMeaf=50;
float SOGI_Freqff=50;
float SOGI_wFreqErr=0;
float UgalDire=0, UgalQuar=0, UgbeDire=0, UgbeQuar=0;
float UgalErr=0, UgbeErr=0;
////////////////////////////////////////////////////////////////////
///////////////////////// Realtime Para  ///////////////////////////
////////////////////////////////////////////////////////////////////
// DC Voltage
float mpr_udc=0, mpr_udcf=0;
////////////////////////////////////////////////////////////////////
// Grid Voltage 3_Phase_Static
float mpr_ua=0, mpr_ub=0, mpr_uc=0;
float mpr_uab=0, mpr_ubc=0, mpr_uca=0;
float mpr_uabf=0, mpr_ubcf=0, mpr_ucaf=0;
// Grid Voltage 2_Phase_Static
float mpr_ual=0, mpr_ube=0;
float UGal_D90=0,UGbe_D90=0;
float mpr_ual_p=0, mpr_ube_p=0;
float mpr_ual_n=0, mpr_ube_n=0;
// Grid Voltage 2_Phase_Rotate
float mpr_ud=0, mpr_uq=0;
float mpr_udf=0, mpr_uqf=0;
float mpr_udp=0, mpr_uqp=0;
float mpr_udn=0, mpr_uqn=0;
float mpr_udpf=0, mpr_uqpf=0;
float mpr_udnf=0, mpr_uqnf=0;
////////////////////////////////////////////////////////////////////
// Grid Current 3_Phase_Static
float mpr_ia=0, mpr_ib=0, mpr_ic=0;
float mpr_isum=0;
float mpr_ipeak=0;
// Grid Current 2_Phase_Static
float mpr_ial=0, mpr_ibe=0;
float Igal_D90=0, Igbe_D90=0;
float Igal_N=0, Igbe_N=0;
// Grid Current 2_Phase_Rotate
float mpr_id=0, mpr_iq=0;
float mpr_iod=0, mpr_ioq=0;
float mpr_iodf=0, mpr_ioqf=0;
float mpr_idn=0, mpr_iqn=0;
float mpr_idf=0, mpr_iqf=0;
float mpr_idnf=0, mpr_iqnf=0;
////////////////////////////////////////////////////////////////////
// Filter Current 3_Phase_Static
float npr_ifa=0, npr_ifb=0, npr_ifc=0;
// Grid Current 2_Phase_Rotate
float npr_ifal=0, npr_ifbe=0;
// Grid Current 2_Phase_Static
float npr_ifd=0, npr_ifq=0;
////////////////////////////////////////////////////////////////////
//////////////////////// Ctrl Freq Angle ///////////////////////////
////////////////////////////////////////////////////////////////////
float mpr_freq=0, mpr_freqf=0;
float GridVolt_Angle=0;
float GridCurrent_Angle=0, GridUconvert_Angle=0;
float MotorVolt_Angle=0;
float MotorVolt_Freq=0;
float MotorVolt_AngleComp=0;
////////////////////////////////////////////////////////////////////
///////////////////////// Ctrl Voltage  ////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Current 2_Phase_Rotate
float UsCtrl_Al=0, UsCtrl_Be=0;
float UsCtrl_Aln=0, UsCtrl_Ben=0;
float UsCtrl_AlG=0, UsCtrl_BeG=0;
float UsCtrl_AlGn=0, UsCtrl_BeGn=0;
float UsCtrl_AlAll=0, UsCtrl_BeAll=0;
// Grid Current 2_Phase_Static
float UsCtrl_d=0, UsCtrl_q=0;
float UsCtrl_dn=0, UsCtrl_qn=0;
////////////////////////////////////////////////////////////////////
///////////////////////// RMS Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage RMS
float UGab_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float UGbc_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float UGca_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float Ua_Rms=0, Ub_Rms=0, Uc_Rms=0;
float Ua_Rmsf=0, Ub_Rmsf=0, Uc_Rmsf=0;
float Uab_Rms=0, Ubc_Rms=0, Uca_Rms=0;
float Uab_Rmsf=0, Ubc_Rmsf=0, Uca_Rmsf=0;
float vLine_max=0, vLine_min=0;
float vLine_avg=0;
float UalbeP_Rms=0, UalbeN_Rms=0;
float UalbeP_Rmsf=0, UalbeN_Rmsf=0;
// Grid Current RMS
float IGa_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float IGb_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float IGc_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float Ia_Rms=0, Ib_Rms=0, Ic_Rms=0;
float Ia_Rmsf=0, Ib_Rmsf=0, Ic_Rmsf=0;
float Iphase_min=0, Iphase_max=0;
float Iphase_avg=0;
// Filter Current RMS
float IGaf_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float IGbf_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float IGcf_HalfMemory[10]={0,0,0,0,0,0,0,0,0,0};
float Iaf_Rms=0, Ibf_Rms=0, Icf_Rms=0;
float Iaf_Rmsf=0, Ibf_Rmsf=0, Icf_Rmsf=0;
float Ifphase_min=0, Ifphase_max=0;
// Power RMS
float NPR_Power=0, NPR_ReactPower=0;
float Power_Factor=0;
float GridVolt_Imbalance=0;
float MPR_Ps=0, MPR_Qs=0, MPR_Ss=0;
float MPR_PsACK=0, MPR_QsACK=0, MPR_SsACK=0;
float Volt_Imbalance=0, Curr_Imbalance=0;

float Ioa_Rms=0, Iob_Rms=0, Ioc_Rms=0;
float UabBus1_Rms=0,UbcBus1_Rms=0,UcaBus1_Rms=0;
float UabBus2_Rms=0,UbcBus2_Rms=0,UcaBus2_Rms=0;

float Ioa_Rmsf=0, Iob_Rmsf=0, Ioc_Rmsf=0;
float UabBus1_Rmsf=0,UbcBus1_Rmsf=0,UcaBus1_Rmsf=0;
float UabBus2_Rmsf=0,UbcBus2_Rmsf=0,UcaBus2_Rmsf=0;
////////////////////////////////////////////////////////////////////
////////////////////// Filter Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage
float UGab_BPF_X[3]={0,0,0}, UGab_BPF_Y[3]={0,0,0};
float UGbc_BPF_X[3]={0,0,0}, UGbc_BPF_Y[3]={0,0,0};
float UGal_PDF_X[3]={0,0,0}, UGal_PDF_Y[3]={0,0,0};
float UGbe_PDF_X[3]={0,0,0}, UGbe_PDF_Y[3]={0,0,0};
// Grid Current
float Igal_PDF_X[3]={0,0,0}, Igal_PDF_Y[3]={0,0,0};
float Igbe_PDF_X[3]={0,0,0}, Igbe_PDF_Y[3]={0,0,0};
float Idnx_table[3]={0,0,0}, Idny_table[3]={0,0,0};
float Iqnx_table[3]={0,0,0}, Iqny_table[3]={0,0,0};
// Filter Current
float Ifal_BPF_X[3]={0,0,0}, Ifal_BPF_Y[3]={0,0,0};
float Ifbe_BPF_X[3]={0,0,0}, Ifbe_BPF_Y[3]={0,0,0};
////////////////////////////////////////////////////////////////////
////////////////////// Current Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
float Idq_Loop_Kp=0, Idq_Loop_Ki=0;
float Id_Loop_Lim=0, Iq_Loop_Lim=0, Idq_Loop_Lim=0;

float Id_Loop_Kp=0, Id_Loop_Ki=0, Id_Loop_Kd=0;
float Errtotal_Id=0, Id_Loop_Out=0;
float Iq_Loop_Kp=0, Iq_Loop_Ki=0, Iq_Loop_Kd=0;
float Errtotal_Iq=0, Iq_Loop_Out=0;

float Idn_Loop_Kp=0, Idn_Loop_Ki=0, Idn_Loop_Kd=0;
float Errtotal_Idn=0, Idn_Loop_Out=0, Idn_Loop_Lim=0;
float Iqn_Loop_Kp=0, Iqn_Loop_Ki=0, Iqn_Loop_Kd=0;
float Errtotal_Iqn=0, Iqn_Loop_Lim=0, Iqn_Loop_Out=0;
////////////////////////////////////////////////////////////////////
////////////////////// DCVoltage Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
float Udc_Loop_Kp1=0, Udc_Loop_Ki1=0, Udc_Loop_Kd1=0;
float Udc_Loop_Kp2=0, Udc_Loop_Ki2=0, Udc_Loop_Kd2=0;

float Errtotal_Udc=0;
////////////////////////////////////////////////////////////////////
////////////////////// ACVoltage Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
float Udq_Loop_Kp=0, Udq_Loop_Ki=0;
float Udq_Loop_Lim=0;

float Ud_Loop_Kp=0, Ud_Loop_Ki=0;
float Errtotal_Ud=0, Ud_Loop_Out=0;
float Uq_Loop_Kp=0, Uq_Loop_Ki=0;
float Errtotal_Uq=0, Uq_Loop_Out=0;

float Udn_Loop_Kp=0, Udn_Loop_Ki=0;
float Errtotal_Udn=0, Udn_Loop_Out=0;
float Uqn_Loop_Kp=0, Uqn_Loop_Ki=0;
float Errtotal_Uqn=0, Uqn_Loop_Out=0;
float Udqn_Loop_Lim=0;

float Ud_FeedFwd=0, Uq_FeedFwd=0;
////////////////////////////////////////////////////////////////////
//////////////////////   PLL Loop    ///////////////////////////////
////////////////////////////////////////////////////////////////////
float PLL_Kp=0, PLL_Ki=0, PLL_Kd=0;
float PLL_Lim=0;
float PLL_ErrTotal=0;
////////////////////////////////////////////////////////////////////
/////////////////////// Hardware Ctrl  /////////////////////////////
////////////////////////////////////////////////////////////////////
// DI
union DataBit_Type IOInputFpga1,IOInputFpga2, IOInputFpga3;
union DataBit_Type IOInput1, IOInput2;
union DataBit_Type IOInputEn;
union DataBit_Type IOInputLogic;
union DataBit_Type IOInputDisValue;
// DO
union DataBit_Type IOOutFpga1, IOOutFpga1Old;
union DataBit_Type IOOutput1, IOOutput1Old;
union DataBit_Type IOOutCtrl1Word;
// InterADC Tempr
float npr_temphigh_reactor=0,npr_templow_reactor=0;
float mpr_temphigh=0, mpr_templow=0;
float Motor_Temphigh=0, Motor_Templow=0;
int FPGACD4067_SelFlag=0;
float Filter_Cap_Temphigh=0,Filter_Cap_TempLow=0;
int InterADValue[16]={0};
int NTC_temp[16]={0};
int NTC_tempf[16]={0};
float VolSample[16]={0};
float ResSample[16]={0};
float TemprCalc[16]={0};
//pwm
int Globle_CMPTable[6]={0,0,0,0,0,0};
int PWMTestA=0, PWMTestB=0, PWMTestC=0;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
// Alarm MaskWord
union DataBit_Type AlarmWord1;
union DataBit_Type AlarmWord2;
union DataBit_Type AlarmWord3;
union DataBit_Type AlarmWord4;
union DataBit_Type AlarmWord5;
union DataBit_Type AlarmWord6;
union DataBit_Type AlarmWord7;
union DataBit_Type AlarmWord8;
// alarm thr
float AlmThr_OverVolt_Work1 = 450;
float AlmThr_UnderVolt_Work1 = 320;
float AlmThr_OverVolt_Work2 = 450;
float AlmThr_UnderVolt_Work2 = 320;
float AlmThr_OverDvalue_Freq=5;
float AlmThr_OverTemp_IGBT=60;
float AlmThr_OverVolt_Udc = 720;
float AlmThr_UnderVolt_Udc = 720;
float AlmThr_OverCurr_Irms=0;
float AlmThr_OverImba_Volt=0.2;
float AlmThr_OverImba_Curr=0.2;

int AlmCnt_OpearatError=0;
////////////////////////////////////////////////////////////////////
////////////////////// Falut Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
u16 MPR_FaultCode=0, MPR_AlarmCode=0;
union DataBit_Type ECANA_FaultCode;
union DataBit_Type ECANA_FaultMask;
union DataBit_Type NPR_ConvHWAMWord;
// Fault Check Timer
int FltCnt_OverTime_Unload=0;
int FltCnt_OverTime_PreChar=0;
int FltCnt_OverDvalue_Tempr=0;
int FltCnt_OverTemp_IGBT=0;
int FltCnt_OverTime_MasShift=0;

int FltCnt_OverVolt_Work1=0;
int FltCnt_UnderVolt_Work1=0;
int FltCnt_OverVolt_Work2=0;
int FltCnt_UnderVolt_Work2=0;
int FltCnt_UnderVolt_Udc=0;
int FltCnt_OverVolt_Udc=0;
int FltCnt_OverTime_ACB=0;
int FltCnt_OverTime_FCC=0;
int FltCnt_OverTime_FanIgbt=0;
int FltCnt_OverTime_FanCap=0;
int FltCnt_OverCurr_Ipeak=0;
int FltCnt_OverCurr_Irms=0;
int FltCnt_OverCurr_CapIrms=0;
int FltCnt_OverDvalue_Freq=0;
int FltCnt_UnderFreq_Grid=0;
int FltCnt_OverCurr_Isum=0;
int FltCnt_VolgImba_Grid=0;
int FltCnt_OverTime_LockPWM=0;
int FltCnt_ToggHigh_CAN=0;
int FltCnt_ToggLow_CAN=0;
int FltCnt_OverTime_Cap40uF=0;
int FltCnt_OverTime_Cap20uF=0;
int FltCnt_OverTime_NprRun=0;
int FltCnt_OverImba_Volt=0;
int FltCnt_OverImba_Curr=0;
int FltCnt_OverTime_ACB1Out=0;
int FltCnt_OverTime_ACB2Out=0;
int FltCnt_OverTime_ACB3Out=0;
int FltCnt_OverLow_FlowRate=0;
// Fault threshold
float FltThr_OverVolt_Work = 450;
float FltThr_UnderVolt_Work = 320;
float FltThr_OverVolt_Work1 = 450;
float FltThr_UnderVolt_Work1 = 320;
float FltThr_OverVolt_Work2 = 450;
float FltThr_UnderVolt_Work2 = 320;

float FltThr_OverDvalue_Freq = 53;
float FltThr_UnderFreq_Grid = 47;
float FltThr_OverVolt_Udc = 720;
float FltThr_OverVoltEst_Udc = 730;
float FltThr_UnderVolt_Udc = 450;
float FltThr_OverTime_PreChar=400;
float FltThr_OverCurr_Ipeak=600;
float FltThr_OverCurrEst_Ipeak=650;
float FltThr_OverCurr_Irms=300;
float FltThr_OverCurr_Isum=40;
float FltThr_OverCurr_CapIrms=55;
float FltThr_OverTemp_IGBT=60;
Uint16 FltThr_OverTimt_UnloadIn=1000;
float FltThr_OverImba_Volt=0.2;
float FltThr_OverImba_Curr=0.2;
// Fault Word
union DataBit_Type MPR_HWFMWord0;
union DataBit_Type MPR_HWFMWord1;
union DataBit_Type MPR_HWFMWord2;
union DataBit_Type MPR_HWFMWord3;
union DataBit_Type MPR_HWFMWord4;
union DataBit_Type MPR_HWFMWord5;
union DataBit_Type MPR_HWFMWord6;
union DataBit_Type MPR_HWFMWord7;
// Fault MaskWord
union DataBit_Type FaultWord1;
union DataBit_Type FaultWord2;
union DataBit_Type FaultWord3;
union DataBit_Type FaultWord4;
union DataBit_Type FaultWord5;
union DataBit_Type FaultWord6;
union DataBit_Type FaultWord7;
union DataBit_Type FaultWord8;
////////////////////////////////////////////////////////////////////
////////////////////// Current  Har      ///////////////////////////
////////////////////////////////////////////////////////////////////
float Curr_Har1_d_LPF_X[3]={0,0,0},Curr_Har1_d_LPF_Y[3]={0,0,0};
float Curr_Har1_q_LPF_X[3]={0,0,0},Curr_Har1_q_LPF_Y[3]={0,0,0};
int Curr_Har1_En=0;
int Curr_Har1_Order=0;
float Curr_Har1_d=0,Curr_Har1_q=0;
float Curr_Har1_df=0,Curr_Har1_qf=0;
float Curr_Har1_dErr=0,Curr_Har1_qErr=0;
float Curr_Har1_dErrtotal=0,Curr_Har1_qErrtotal=0;
float Curr_Har1_alOut=0,Curr_Har1_beOut=0;
float Curr_Har1_Loop_Kp=0,Curr_Har1_Loop_Ki=0,Curr_Har1_Loop_Lim=0;
float Curr_Har1_dLoop_Out=0,Curr_Har1_qLoop_Out=0;

float Curr_Har2_d_LPF_X[3]={0,0,0},Curr_Har2_d_LPF_Y[3]={0,0,0};
float Curr_Har2_q_LPF_X[3]={0,0,0},Curr_Har2_q_LPF_Y[3]={0,0,0};
int Curr_Har2_En=0;
int Curr_Har2_Order=0;
float Curr_Har2_d=0, Curr_Har2_q=0;
float Curr_Har2_df=0, Curr_Har2_qf=0;
float Curr_Har2_dErr=0, Curr_Har2_qErr=0;
float Curr_Har2_dErrtotal=0, Curr_Har2_qErrtotal=0;
float Curr_Har2_alOut=0, Curr_Har2_beOut=0;
float Curr_Har2_Loop_Kp=0, Curr_Har2_Loop_Ki=0, Curr_Har2_Loop_Lim=0;
float Curr_Har2_dLoop_Out=0, Curr_Har2_qLoop_Out=0;
////////////////////////////////////////////////////////////////////
///////////////////////// System Para  /////////////////////////////
////////////////////////////////////////////////////////////////////
union DSP_TO_FPGA_Word_Type DspToFpgaWord;
union FPGA_TO_DSP_Word_Type FpgaToDspWord;
int Start_WaitTime , Start_WaitTimeSet;
int Flag_DspRdFpga=0x5555, Flag_DspWrFpga=0x5555;
// Ctrl Status
union Power_Ctrl_Word_BitType PowerCtrlWord;
union Power_Status_Word_BitType PowerStatusWord;
union Main_Control_Word_Type MainCtrlWord;
union Main_Control_Word_Type MainCtrlWord_CAN;
union Main_Control_Word_Type MainCtrlWord_PC;
union Main_Control_Word_Type MainCtrlWord_HMI;
union Main_Status_Word_Type MainStusWord,MainStusWordOld;
union Control_Mode_Word_Type CtrlModeWord;
union Break_Ctrl_Word_Type BreakCtrlWord;
union Break_Status_Word_Type BreakStatusWord;

int UnloadCtrl_PC=0;
union DataBit_Type FPGACtrlWord;
float VoltFrontfeed_Coeff=0, CurrFrontfeed_Coeff=0;
int Manu_Oper_Force=0;
// PowerRef
float KVARRef_Sys=0, KWRefRef_Sys=0;
float KVARRef_CAN=0, KWRefRef_CAN=0;
float KVARRef_PC=0, KWRefRef_PC=0;
float CurrRef_InductLoad=0;
// ActivePower Ref
float KwRef=0;
float KW_Lim=0;
float kW_cal_step=0;
// ReactivePower Ref
float KVar_Lim=0;
float KVar_filter_Comp=0;
// Curr PriorityMode
int Id_Priority=0;
int IdqStep_CountMax=0;
float Ig_Lim=0;
int IdqStep_Count=0;
// Active Current Ref
float IdRef_Outer=0, IqRef_Outer=0;
float UdRefRef_Outer=0, UdRefRefOld_Outer=0;
float IgdRefRef=0, IgdRef=0;
float Igd_Lim=0;
float Id_cal_step=0;
// Reactive Current Ref
float IgqRefRef=0, IgqRef=0;
float Igq_Lim=0;
float Iq_cal_step=0;
//
float UsdRefRef_Buf=0;
float MotorFreqRef_PC=517, MotorFreqRef_CAN=517, MotorFreqRef_HMI=517;
float UsdRefRef_PC=190, UsdRefRef_CAN=190, UsdRefRef_HMI=190;
float UsdRefRefOld_PC=0, UsdRefRefOld_CAN=0, UsdRefRefOld_HMI=0;
float MotorFreqRef=0, MotorFreqRefOld=0;
float FpgaMotorFreq=0;
float Ud_cal_step=0;
//
u16 UdqStep_Count=0, UdqStep_CountMax=0;
float Usd_Lim=0;
float UdRef_All=0, UqRef_All=0;
float UsdRefRef=0, UsdRef=0;
float UsqRefRef=0, UsqRef=0;
// Fastchar Ref
Uint16 FastChar_PhaseSel=0;
Uint16 FastChar_RiseTime=0, FastChar_HoleTime=0, FastChar_DownTime=0;
float FastChar_HighVolt=0, FastChar_AfterVolt=0;
float FastChar_ExitCurrSet=0;
int FastChar_RunStatus=0;
int FastChar_CtrlWord=0, FastChar_StopWord=0;
float FastChar_Rate=0;
float FastChar_RiseStep=0, FastChar_DownStep=0;
Uint16 FastChar_TimeCount=0;
float FastChar_UdRefLock=0;
float FastChar_UdRefRef=0, FastChar_UdRef=0;

float FastChar_RiseGiven=0, FastChar_DownGiven=0;
float FastChar_UsdOld=0, FastChar_UsqOld=0;
float FastChar_DCOffset=0;
long FastChar_CtrlCount=0, FastChar_HoleCount=0, FastChar_DownCount=0;
float FastChar_UdCompon = 0,FastChar_UqCompon = 0;
int FastChar_1msCount = 0;
// DC Voltage Ref
float Udc_SetSet=0;
Uint16 Udc_SetCount=0, UdcStep_CountMax=0;
float Udc_cal_step=0;
// Rated Para
float RatedPara_GridVoltage = 380;
float RatedPara_GridCurrent = 45;
float RatedPara_GridFrequency = 50;

float MotorSideCapacity=500;
float MotorSideVoltage=380;
float MotorSideCurrent=760;
// Timer Base
int SysCnt_1msBase=0;
int SysCnt_1ms=0, SysCnt_1msOld=0;
int SysCnt_1sBase=0;
int SysCnt_1s=0, SysCnt_1sOld=0;
int SysCnt_10msRmsCalc=0;
// Device Hardware Para
float K_IfcForw=0;
float FilterPara_Induct=0;
float FilterPara_Capacity=0;
float NPR_AngleCompensation=0;
float Para_IoFwdCoeff=0;
// count
float CtrlThr_ChopIn_UdcH=0;
float CtrlThr_ChopOut_UdcL=0;
float CtrlThr_FanIgbtOn_TempH=0;
float CtrlThr_FanIgbtOff_TempL=0;
float CtrlThr_LockPwm_Ipeak=0;
int   CtrlThr_LockPwm_Time=0;
float CtrlThr_UnLockPwm_Ipeak=0;
// Status
int Status_MPRRun=0;
// Ctrl count

int CtrlCnt_StartCheck=0;
int CtrlCnt_IgbtFanOn=0;
int CtrlCnt_LedTogg=0;
int CtrlCnt_RstDelay=0;
int CtrlCnt_ManUnload=0;
int CtrlCnt_IORunRefFeed=0;
int CtrlCnt_IORstRefFeed=0;
int CtrlCnt_IOFltDelay=0;
int CtrlCnt_ReactorFanOn=0;
int CtrlCnt_SofaStart=0;
int CtrlCnt_StartNpr=0;
int CtrlCnt_FltDlyUV1=0;
int CtrlCnt_FltDlyUV2=0;
int CtrlCnt_AddBusOk=0;
// Flag
int Flag_LockPWM=0;
int Flag_IORunRef=0;
int Flag_IORstRef=0;
int Flag_IORstRefOld=0;
int Flag_FanCapOn=0;
int Flag_FanIgbtOn=0;
int Flag_EEPROMCheck=0;
int Flag_RstCmdOld=0;
int Flag_PwmModuMode=1;
int Flag_PowerOn=0;
int Flag_WorkMode=0, Flag_WorkModeOld=1;
int CAN_SectorRst_Flag=0, CAN_SectorRst_FlagOld=0;
int Flag_VoltRef=0, FlagOld_VoltRef=0;
int Flag_FreqRef=0, FlagOld_FreqRef=0;
int Flag_PowerOnRef=0;
////////////////////////////////////////////////////////////////////
int Operation_ineffective=0;
int Rst_Flag_HMI=0, Rst_Flag_HMIOld=0;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/******************************************************************/
void variable_init(void)
{
	CtrlModeWord.DataInt=0;
	MainCtrlWord.DataInt=0;
	MainCtrlWord_CAN.DataInt=0;
	MainCtrlWord_PC.DataInt=0;
	MainStusWord.DataInt=0;
	MainStusWordOld.DataInt=0;


	DspToFpgaWord.DataInt = 0;

	MasToSlaWord.all = 0;
	Mas1ToSlaWord.all = 0;
	Mas2ToSlaWord.all = 0;

	SlaToMasWord.all = 0;
	SlaToMas1Word.all = 0;
	SlaToMas2Word.all = 0;

	memset(&HeartbeatCheck1, 0, sizeof(HeartbeatCheck1));
	memset(&HeartbeatCheck2, 0, sizeof(HeartbeatCheck2));

	FaultWord1.DataInt=0;
	FaultWord2.DataInt=0;
	FaultWord3.DataInt=0;
	FaultWord4.DataInt=0;
	FaultWord5.DataInt=0;
	FaultWord6.DataInt=0;
	FaultWord7.DataInt=0;
	FaultWord8.DataInt=0;

	MPR_HWFMWord0.DataInt=0;
	MPR_HWFMWord1.DataInt=0;
	MPR_HWFMWord2.DataInt=0;
	MPR_HWFMWord3.DataInt=0;
	MPR_HWFMWord4.DataInt=0;
	MPR_HWFMWord5.DataInt=0;
	MPR_HWFMWord6.DataInt=0;
	MPR_HWFMWord7.DataInt=0;
}
/*******************************************************************************************/
void variable_algorithm_init(void)
{
	Errtotal_Ud = 0;
	Errtotal_Uq = 0;
	Errtotal_Udn = 0;
	Errtotal_Uqn = 0;

	Ud_Loop_Out = 0;
	Uq_Loop_Out = 0;
	Udn_Loop_Out = 0;
	Uqn_Loop_Out = 0;

	UsdRef = 0;

	Errtotal_Udc = 0;
	Errtotal_Id = 0; 
	Errtotal_Iq = 0;
	Errtotal_Idn = 0; 
	Errtotal_Iqn = 0;

	IgdRef=0;
	IgqRef=0;

	KVARRef_Sys=0;
    KVARRef_PC=0;
    KWRefRef_Sys=0;
    KWRefRef_PC=0;
    KwRef=0;
    KVar_filter_Comp=0;
    CurrRef_InductLoad=0;

    Id_Loop_Out = 0;
	Iq_Loop_Out = 0;
	Idn_Loop_Out = 0;
	Iqn_Loop_Out = 0;
	UnloadCtrl_PC = 0;
	//
	FltCnt_OverCurr_Ipeak = 0;
	FltCnt_OverVolt_Udc = 0;
	FltCnt_OverTime_Unload = 0;
	FltCnt_OverTime_PreChar = 0;
	FltCnt_OverTime_ACB = 0;
	FltCnt_OverTime_FCC = 0;
	FltCnt_OverTime_FanCap = 0;
	FltCnt_OverTime_FanIgbt = 0;
	FltCnt_OverDvalue_Tempr = 0;
	FltCnt_OverCurr_Irms = 0;
	FltCnt_OverCurr_CapIrms = 0;
	FltCnt_OverCurr_Isum = 0;
	FltCnt_UnderVolt_Udc = 0;
	FltCnt_OverDvalue_Freq = 0;
	FltCnt_UnderFreq_Grid = 0;
	FltCnt_VolgImba_Grid = 0;
	FltCnt_OverTemp_IGBT = 0;
	FltCnt_OverTime_LockPWM = 0;
	FltCnt_OverTemp_Rectifier = 0;
	FltCnt_OverTemp_Inverter = 0;
	FltCnt_OverTemp_Transformer = 0;
	FltCnt_OverLow_FlowRate = 0;
	FltCnt_OverTime_MasShift = 0;
	////////////////////////////////////////////
	MPR_FaultCode = 0;
	FaultWord1.DataInt = 0;
	FaultWord2.DataInt = 0;
	FaultWord3.DataInt = 0;
	FaultWord4.DataInt = 0;
	FaultWord5.DataInt = 0;
	FaultWord6.DataInt = 0;
	FaultWord7.DataInt = 0;
	FaultWord8.DataInt = 0;

	AlarmWord1.DataInt = 0;
	AlarmWord2.DataInt = 0;
	AlarmWord3.DataInt = 0;
	AlarmWord4.DataInt = 0;
	AlarmWord5.DataInt = 0;
	AlarmWord6.DataInt = 0;
	AlarmWord7.DataInt = 0;
	AlarmWord8.DataInt = 0;
	////////////////////////////////////////////
	MainCtrlWord.DataInt = 0;
	MainCtrlWord_PC.DataInt = 0;
	MainCtrlWord_CAN.DataInt = 0;

	CtrlCnt_IgbtFanOn = 0;
	////////////////////////////////////////////
	Status_MPRRun = 0;
}
/********************variable_init end*****************************/
void Flash_Fault_ResetVarieble(void)
{
	EventLog_ReadCmd=0;
	EventLogStep=0;

	DataLog_ReadCmd=0;
	DataLog_CtrlCmd=1;
	DataLogStep=0;

	Flash_Capacity=0;
}

/******************************************************************************************/
void FaultWord_Map(u16 Code)
{
	if((Code >= 129) && (Code < 145))
		FaultWord1.DataInt |= (1<<(Code-129));
	if((Code >= 145) && (Code < 161))
		FaultWord2.DataInt |= (1<<(Code-145));
	if((Code >= 161) && (Code < 177))
		FaultWord3.DataInt |= (1<<(Code-161));
	if((Code >= 177) && (Code < 193))
		FaultWord4.DataInt |= (1<<(Code-177));
	if((Code >= 193) && (Code < 209))
		FaultWord5.DataInt |= (1<<(Code-193));
	if((Code >= 209) && (Code < 225))
		FaultWord6.DataInt |= (1<<(Code-209));
	if((Code >= 225) && (Code < 241))
		FaultWord7.DataInt |= (1<<(Code-225));
	if((Code >= 241) && (Code < 257))
		FaultWord8.DataInt |= (1<<(Code-241));
}
/*****************************************************************************************/
u16 FaultMask_Map(u16 Code)
{
	u16 Remain = 0;
	u16 Quotient = 0;
	u16 Rrn = 0;

	Quotient = ((Code-129) >> 4);
	Remain = (Code-129) % 16 ;
	switch(Quotient)
	{
		case 0:
		{
			if(((MPR_HWFMWord0.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 1:
		{
			if(((MPR_HWFMWord1.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 2:
		{
			if(((MPR_HWFMWord2.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 3:
		{
			if(((MPR_HWFMWord3.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 4:
		{
			if(((MPR_HWFMWord4.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 5:
		{
			if(((MPR_HWFMWord5.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 6:
		{
			if(((MPR_HWFMWord6.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
		case 7:
		{
			if(((MPR_HWFMWord7.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
	    break;
		}
	  default:{break;}
	}
	return Rrn;
}
/********************************************************************************************/
void AlarmWord_Map(u16 Code)
{

	if((Code >= 129) && (Code < 145))
		AlarmWord1.DataInt |= (1<<(Code-129));
	if((Code >= 145) && (Code < 161))
		AlarmWord2.DataInt |= (1<<(Code-145));
	if((Code >= 161) && (Code < 177))
		AlarmWord3.DataInt |= (1<<(Code-161));
	if((Code >= 177) && (Code < 198))
		AlarmWord4.DataInt |= (1<<(Code-177));
	if((Code >= 193) && (Code < 208))
		AlarmWord5.DataInt |= (1<<(Code-193));
	if((Code >= 209) && (Code < 224))
		AlarmWord6.DataInt |= (1<<(Code-209));
	if((Code >= 225) && (Code < 240))
		AlarmWord7.DataInt |= (1<<(Code-225));
	if((Code >= 241) && (Code < 257))
		AlarmWord8.DataInt |= (1<<(Code-241));

}
/********************************************************************************************/

void AlarmWord_Clear(u16 Code)
{

	if((Code >= 129) && (Code < 145))
		AlarmWord1.DataInt &= (~ (1<<(Code-129)) );
	if((Code >= 145) && (Code < 161))
		AlarmWord2.DataInt &= (~ (1<<(Code-145)) );
	if((Code >= 161) && (Code < 177))
		AlarmWord3.DataInt &= (~ (1<<(Code-161)) );
	if((Code >= 177) && (Code < 198))
		AlarmWord4.DataInt &= (~ (1<<(Code-177)) );
	if((Code >= 193) && (Code < 208))
		AlarmWord5.DataInt &= (~ (1<<(Code-193)) );
	if((Code >= 209) && (Code < 224))
		AlarmWord6.DataInt &= (~ (1<<(Code-209)) );
	if((Code >= 225) && (Code < 240))
		AlarmWord7.DataInt &= (~ (1<<(Code-225)) );
	if((Code >= 241) && (Code < 257))
		AlarmWord8.DataInt &= (~ (1<<(Code-241)) );

}

//===========================================================================
// No more.
//===========================================================================



