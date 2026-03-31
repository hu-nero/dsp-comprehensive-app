
#include "DSP2833x_Device.h"
#include "Custom_GlobalMacro.H"
#include "Comm_Variable.H"
#include "Variable.h"
#include "Arithmetic\Arithmetic.H"
#include "Arithmetic\NprCtrl.h"
#include "IQmathLib.h"
#include "string.h"

////////////////////////////////////////////////////////////////////
RemoteInput_t RemInputSum, DcsInputSum;
RemoteHold_t RemHoldSum, DcsHoldSum;
////////////////////////////////////////////////////////////////////
float Iout_Imbalance=0, Uout_Imbalance=0;

float UabSl, UbcSl, UcaSl;
float UabPar, UbcPar, UcaPar;
float IaPar, IbPar, IcPar;
float IaOvh, IbOvh, IcOvh;
float UaOut, UbOut, UcOut;
float UabOut, UbcOut, UcaOut;

float UabSl_rmsf=0, UbcSl_rmsf=0, UcaSl_rmsf=0;
float UabPar_rmsf=0, UbcPar_rmsf=0, UcaPar_rmsf=0;
float IaPar_rmsf=0, IbPar_rmsf=0, IcPar_rmsf=0;
float IaOvh_rmsf=0, IbOvh_rmsf=0, IcOvh_rmsf=0;
float UaOut_rmsf=0, UbOut_rmsf=0, UcOut_rmsf=0;
float UabOut_rmsf=0, UbcOut_rmsf=0, UcaOut_rmsf=0;
float P_Workf, Q_Workf, S_Workf;
float P_Ovhf, Q_Ovhf, S_Ovhf;
float IaOut_rmsf, IbOut_rmsf, IcOut_rmsf;

float UabSl_sum=0, UbcSl_sum=0, UcaSl_sum=0;
float UabPar_sum=0, UbcPar_sum=0, UcaPar_sum=0;
float IaPar_sum=0, IbPar_sum=0, IcPar_sum=0;
float IaOvh_sum=0, IbOvh_sum=0, IcOvh_sum=0;
float UaOut_sum=0, UbOut_sum=0, UcOut_sum=0;
float UabOut_sum=0, UbcOut_sum=0, UcaOut_sum=0;
float UabSl_rms, UbcSl_rms, UcaSl_rms;
float UabPar_rms, UbcPar_rms, UcaPar_rms;
float IaPar_rms, IbPar_rms, IcPar_rms;
float IaOvh_rms, IbOvh_rms, IcOvh_rms;
float UaOut_rms, UbOut_rms, UcOut_rms;
float UabOut_rms, UbcOut_rms, UcaOut_rms;
float P_Work, Q_Work, S_Work, PF_Work;
float P_Ovh, Q_Ovh, S_Ovh, PF_Ovh;
float P_Sys, Q_Sys, S_Sys, PF_Sys;

int Flag_RmsBufFull=0;
float UabSl_HalfMemory[20], UbcSl_HalfMemory[20], UcaSl_HalfMemory[20];
float UabPar_HalfMemory[20], UbcPar_HalfMemory[20], UcaPar_HalfMemory[20];
float IaPar_HalfMemory[20], IbPar_HalfMemory[20], IcPar_HalfMemory[20];
float IaOvh_HalfMemory[20], IbOvh_HalfMemory[20], IcOvh_HalfMemory[20];
float UaOut_HalfMemory[20], UbOut_HalfMemory[20], UcOut_HalfMemory[20];
float UabOut_HalfMemory[20], UbcOut_HalfMemory[20], UcaOut_HalfMemory[20];

unsigned int AlmCnt_UnderVolt_Phase;
unsigned int AlmCntThr_UnderVolt_Phase;

unsigned int AlmCnt_OverVolt_Work1=0;
unsigned int AlmCntThr_OverVolt_Work1;
unsigned int AlmCnt_OverVolt_Work2;
unsigned int AlmCntThr_OverVolt_Work2;

unsigned int AlmCnt_UnderVolt_Work1;
unsigned int AlmCntThr_UnderVolt_Work1;
unsigned int AlmCnt_UnderVolt_Work2;
unsigned int AlmCntThr_UnderVolt_Work2;

unsigned int AlmCnt_OverImba_Volt;
unsigned int AlmCntThr_OverImba_Volt;
unsigned int FltCnt_OverImba_Volt;
unsigned int FltCntThr_OverImba_Volt;

unsigned int AlmCnt_OverImba_Curr;
unsigned int AlmCntThr_OverImba_Curr;
unsigned int FltCnt_OverImba_Curr;
unsigned int FltCntThr_OverImba_Curr;

unsigned int AlmCnt_OverCurr_Work1;
unsigned int AlmCntThr_OverCurr_Work1;
unsigned int AlmCnt_OverCurr_Work2;
unsigned int AlmCntThr_OverCurr_Work2;

unsigned int AlmCnt_OverLow_Flow1;
unsigned int AlmCntThr_OverLow_Flow1;
unsigned int AlmCnt_OverLow_Flow2;
unsigned int AlmCntThr_OverLow_Flow2;

float AlmThr_UnderVolt_Phase;
float AlmThr_OverVolt_Work1;
float FltThr_OverVolt_Work1;
float AlmThr_OverVolt_Work2;
float FltThr_OverVolt_Work2;
float AlmThr_UnderVolt_Work1;
float FltThr_UnderVolt_Work1;
float AlmThr_UnderVolt_Work2;
float FltThr_UnderVolt_Work2;
float AlmThr_OverImba_Volt;
float FltThr_OverImba_Volt;
float AlmThr_OverImba_Curr;
float FltThr_OverImba_Curr;
float AlmThr_OverCurr_Work1;
float FltThr_OverCurr_Work1;
float AlmThr_OverCurr_Work2;
float FltThr_OverCurr_Work2;
float FltThr_OverLow_Flow1;
float FltThr_OverLow_Flow2;
float Auto_Restarttime_Flow;
union DataBit_Type Cap_Ctrl_Word;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
CanCommVar_t CanNprSum;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int asd = 0;
Uint16 TestFlag=0, TestIOFlag=0, TestPWMFlag=0;
float Test_CurrRef=0;
float Test_VoltAngle=0;
union DataBit_Type TestIOout, TestIOoutOld;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int Modbus_Flag = 0,Modbus_SendFlag = 0;
float ChopIn_UdcHThr=0;
float ChopOut_UdcLThr=0;
float CoolFan_On_TempThr=0, CoolFan_Off_TempThr=0;
float IGBTCoolFan_On_TempThr=0, IGBTCoolFan_Off_TempThr=0;
float INDCoolFan_On_TempThr=0, INDCoolFan_Off_TempThr=0;
float Cumulative_Power = 0;
int FanRes_HMI=0, FanInd_HMI=0;
float Flow_Alarm_Low = 0;
float uFlowRate = 0, FlowRate = 5;
float Ia1_Rmsf=0, Ib1_Rmsf=0, Ic1_Rmsf=0;
float NPR_ApparentPower = 0;
float NPR_Curr_HighestlimitAlarm = 0;
float NPR_Curr_Highlimit = 0;
float NPR_GridVol_Imbal_Thr = 0;
float NPR_GridVolt_Highlimit = 0;
float NPR_GridVolt_Lowlimit = 0;
float NPR_Temp_AlarmHighlimit = 0;
float NPR_Temp_Highlimit = 0;
float NPR_Udc_AlarmHighlimit = 0,NPR_Udc_AlarmLowlimit = 0;
float NPR_Udc_Highlimit = 0;
float NPR_Udc_LowThr = 0;
float npr_udc = 0, npr_udcf = 0, npr_udcff = 0;
float NPR_VoltImbalance_Alarm = 0;
float NRectifier_Temp_CAN = 0,Transformer_Temp_CAN = 0;
u16 ParaDownloadCtrlWord = 0;
int ParaSaveFlag = 0,ParaSaveFlagOld =0;
float Temp_High = 0,Temp_Low =0;
float Uab1_Rmsf = 0,Ubc1_Rmsf = 0,Uca1_Rmsf =0;
float vLine_Alarm_Low = 0,vLine_Alarm_High = 0;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int FltCnt_OverTemp_Transformer=0,FltCnt_OverTemp_Inverter=0,FltCnt_OverTemp_Rectifier=0;
int FltThr_OverWaterCount=0;
float FltThr_OverTemp_TransIn=0,FltThr_OverTemp_ReactOut=0,FltThr_OverTemp_ReactIn=0;
float FltThr_OverWater = 0;
int Zqwl_Temp_Error = 0;
float Temp_ReactIn;
float Temp_ReactOut=0;
float Temp_TransIn = 0;
int Can_Temp_Flag = 0;
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

////////////////////////////////////////////////////////////////////
float npr_uin=0;
// Grid Voltage 3_Phase_Static
float npr_uab=0, npr_ubc=0, npr_uca=0;
float npr_uabf=0, npr_ubcf=0, npr_ucaf=0;
// Grid Voltage 2_Phase_Static
float npr_ual=0, npr_ube=0;
float UGal_D90=0,UGbe_D90=0;
float npr_ual_p=0, npr_ube_p=0;
float npr_ual_n=0, npr_ube_n=0;
// Grid Voltage 2_Phase_Rotate
float npr_udp=0, npr_uqp=0;
float npr_udn=0, npr_uqn=0;
float npr_udpf=0, npr_uqpf=0;
float npr_udnf=0, npr_uqnf=0;
////////////////////////////////////////////////////////////////////
// Grid Current 3_Phase_Static
float npr_ia=0, npr_ib=0, npr_ic=0;
float npr_isum=0;
float npr_ipeak=0;
// Grid Current 2_Phase_Static
float npr_ial=0, npr_ibe=0;
float Igal_D90=0, Igbe_D90=0;
float Igal_N=0, Igbe_N=0;
// Grid Current 2_Phase_Rotate
float npr_id=0, npr_iq=0;
float npr_idn=0, npr_iqn=0;
float npr_idf=0, npr_iqf=0;
float npr_idnf=0, npr_iqnf=0;
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
float npr_freq=0, npr_freqf=0;
float GridVolt_Angle=0;
float GridCurrent_Angle=0, GridUconvert_Angle=0;
////////////////////////////////////////////////////////////////////
///////////////////////// Ctrl Voltage  ////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Current 2_Phase_Rotate
float UgCtrl_Al=0, UgCtrl_Be=0;
float UgCtrl_Aln=0, UgCtrl_Ben=0;
float UgCtrl_AlG=0, UgCtrl_BeG=0;
float UgCtrl_AlGn=0, UgCtrl_BeGn=0;
float UgCtrl_AlAll=0, UgCtrl_BeAll=0;
// Grid Current 2_Phase_Static
float UgCtrl_d=0, UgCtrl_q=0;
float UgCtrl_dn=0, UgCtrl_qn=0;
////////////////////////////////////////////////////////////////////
///////////////////////// RMS Para  ////////////////////////////////
////////////////////////////////////////////////////////////////////
// Grid Voltage RMS
float UGab_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float UGbc_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float UGca_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float Uab_Rms=0, Ubc_Rms=0, Uca_Rms=0;
float Uab_Rmsf=0, Ubc_Rmsf=0, Uca_Rmsf=0;
float vphase_min=0;
float vLine_max=0, vLine_min=0, vLine_avg=0;
float UalbeP_Rms=0, UalbeN_Rms=0;
float UalbeP_Rmsf=0, UalbeN_Rmsf=0;
// Grid Current RMS
float IGa_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float IGb_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float IGc_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float Ia_Rms=0, Ib_Rms=0, Ic_Rms=0;
float Ia_Rmsf=0, Ib_Rmsf=0, Ic_Rmsf=0;
float Iphase_min=0, Iphase_max=0;
float Iphase_avg=0;
// Filter Current RMS
float IGaf_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float IGbf_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float IGcf_HalfMemory[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float Iaf_Rms=0, Ibf_Rms=0, Icf_Rms=0;
float Iaf_Rmsf=0, Ibf_Rmsf=0, Icf_Rmsf=0;
float Ifphase_min=0, Ifphase_max=0;
// Power RMS
float NPR_Power=0, NPR_ReactPower=0;
float Power_Factor=0;
float GridVolt_Imbalance=0;
float GridCurr_Imbalance=0;
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
float Id_Loop_Kp=0, Id_Loop_Ki=0, Id_Loop_Kd=0;
float Errtotal_Id=0, Id_Loop_Out=0, Id_Loop_Lim=0;
float Iq_Loop_Kp=0, Iq_Loop_Ki=0, Iq_Loop_Kd=0;
float Errtotal_Iq=0, Iq_Loop_Lim=0, Iq_Loop_Out=0;
float Idn_Loop_Kp=0, Idn_Loop_Ki=0, Idn_Loop_Kd=0;
float Errtotal_Idn=0, Idn_Loop_Out=0, Idn_Loop_Lim=0;
float Iqn_Loop_Kp=0, Iqn_Loop_Ki=0, Iqn_Loop_Kd=0;
float Errtotal_Iqn=0, Iqn_Loop_Lim=0, Iqn_Loop_Out=0;
////////////////////////////////////////////////////////////////////
////////////////////// Voltage Loop  ///////////////////////////////
////////////////////////////////////////////////////////////////////
float Udc_Loop_Kp1=0, Udc_Loop_Ki1=0, Udc_Loop_Kd1=0;
float Udc_Loop_Kp2=0, Udc_Loop_Ki2=0, Udc_Loop_Kd2=0;

float Errtotal_Udc=0;
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
union DataBit_Type IOInput1Fpga, IOInput2Fpga;
union DataBit_Type IOInput1, IOInput2;

union DataBit_Type IOInput1En;
union DataBit_Type IOInput1Logic;
union DataBit_Type IOInput1DisValue;

union DataBit_Type IOInput2En;
union DataBit_Type IOInput2Logic;
union DataBit_Type IOInput2DisValue;
// DO
union DataBit_Type IOOutFpga1, IOOutFpga1Old;
union DataBit_Type IOOutFpga2, IOOutFpga2Old;
union DataBit_Type IOOutput1, IOOutput2;
union DataBit_Type IOOut1CtrlWord, IOOut2CtrlWord;
// InterADC Tempr
float CtrlThr_TransfOn_TempH = 0,CtrlThr_TransfOff_TempL = 0;
float CtrlThr_ReactorOn_TempH=0,CtrlThr_TransfOff_TempH = 0;
float npr_temphigh_reactor=0,npr_templow_reactor=0;
float Temp_IgbtH=0, Temp_IgbtL=0;
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

union DataBit_Type FanCtrlWord;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////// Falut Protection  ///////////////////////////
////////////////////////////////////////////////////////////////////
UINT NPR_FaultCode=0;
union DataBit_Type ECANA_FaultCode;
union DataBit_Type ECANA_FaultMask;
union DataBit_Type NPR_ConvHWAMWord;
// Fault Check Timer
int FltCnt_OverTime_Unload=0;
int FltCnt_OverTime_PreChar=0;
int FltCnt_OverDvalue_Tempr=0;
int FltCnt_OverTemp_IGBT=0;
int FltCnt_UnderVolt_Grid=0;
int FltCnt_OverVolt_Grid=0;
int FltCnt_UnderVolt_Udc=0;
int FltCnt_OverVolt_Udc=0;
int FltCnt_OverTime_ACB=0;
int FltCnt_OverTime_ACT=0;
int FltCnt_OverTime_FCC=0;
int FltCnt_OverTime_FanIgbt=0;
int FltCnt_OverTime_FanCap=0;
int FltCnt_OverTime_FanReactor = 0;
int FltCnt_OverCurr_Ipeak=0;
int FltCnt_OverCurr_Irms=0;
int FltCnt_OverCurr_CapIrms=0;
int FltCnt_OverFreq_Grid=0;
int FltCnt_UnderFreq_Grid=0;
int FltCnt_OverCurr_Isum=0;
int FltCnt_VolgImba_Grid=0;
int FltCnt_CurrImba_Grid=0;
int FltCnt_OverTime_LockPWM=0;
int FltCnt_ToggHigh_CAN=0;
int FltCnt_ToggLow_CAN=0;
// Fault threshold
float FltThr_OverVolt_Grid = 450;
float FltThr_UnderVolt_Grid = 320;
float FltThr_VoltImba_Grid =0.2;
float FltThr_CurrImba_Grid=0.2;
float FltThr_OverFreq_Grid = 53;
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
float FltThr_OverDvalue_Freq=60;
// Fault Word
union DataBit_Type NPR_HWFMWord0;
union DataBit_Type NPR_HWFMWord1;
union DataBit_Type NPR_HWFMWord2;
union DataBit_Type NPR_HWFMWord3;
union DataBit_Type NPR_HWFMWord4;
union DataBit_Type NPR_HWFMWord5;
union DataBit_Type NPR_HWFMWord6;
union DataBit_Type NPR_HWFMWord7;
// Fault MaskWord
union DataBit_Type FaultWord1;
union DataBit_Type FaultWord2;
union DataBit_Type FaultWord3;
union DataBit_Type FaultWord4;
union DataBit_Type FaultWord5;
union DataBit_Type FaultWord6;
union DataBit_Type FaultWord7;
union DataBit_Type FaultWord8;
// Fault Remap
union DataBit_Type FltMapWord1;
union DataBit_Type FltMapWord2;
union DataBit_Type FltMapWord3;

union Com_Modbus_To_HMI_Word1_Type SwitchWord1;
union Com_Modbus_To_HMI_Word2_Type SwitchWord2;
union Com_Modbus_To_HMI_Word3_Type SwitchWord3;
////////////////////////////////////////////////////////////////////
////////////////////// Alarm check        ///////////////////////////
////////////////////////////////////////////////////////////////////
float AlmThr_OverCurr_Ipeak=1500;
float AlmThr_OverTemp_IGBT=100;
float AlmThr_OverVolt_Grid=400;
float AlmThr_UnderVolt_Grid=350;
float AlmThr_OverVolt_Udc=680;
float AlmThr_UnderVolt_Udc=550;
float AlmThr_UnderLow_Flow=30;
float AlmThr_VoltImba_Grid=0.5;
float AlmThr_CurrImba_Grid=0.5;
float AlmThr_OverDvalue_Freq=0;

int AlmCnt_VoltImba_Grid=0;
int AlmCnt_CurrImba_Grid=0;

union DataBit_Type AlarmWord1;
union DataBit_Type AlarmWord2;
union DataBit_Type AlarmWord3;
union DataBit_Type AlarmWord4;
union DataBit_Type AlarmWord5;
union DataBit_Type AlarmWord6;
union DataBit_Type AlarmWord7;
union DataBit_Type AlarmWord8;
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
// Ctrl Status
union Main_Control_Word_Type MainCtrlWord;
union Main_Control_Word_Type MainCtrlWord_CAN;
union Main_Control_Word_Type MainCtrlWord_PC;
union NPR_Status_Word_Type MainStusWord,MainStusWordOld;
union NPR_Status_Word2_Type MainSta2Word;
union Control_Mode_Word_Type CtrlModeWord;
int UnloadCtrl_PC=0;
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
float IgdRefRef=0, IgdRef=0;
float Igd_Lim=0;
float Id_cal_step=0;
// Reactive Current Ref
float IgqRefRef=0, IgqRef=0;
float Igq_Lim=0;
float Iq_cal_step=0;
// DC Voltage Ref
float Udc_SetSet=0;
Uint16 Udc_SetCount=0, UdcStep_CountMax=0;
float Udc_cal_step=0,kpDC=0, kiDC=0;
// Rated Para
float RatedPara_GridVoltage = 380;
float RatedPara_GridCurrent = 45;
float RatedPara_GridFrequency = 50;
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
// count
float CtrlThr_ChopIn_UdcH=0;
float CtrlThr_ChopOut_UdcL=0;
float CtrlThr_FanIgbtOn_TempH=0;
float CtrlThr_FanIgbtOff_TempL=0;

float CtrlThr_FanTransfOn_TempH=0;
float CtrlThr_FanTransfOff_TempL=0;

float CtrlThr_LockPwm_Ipeak=0;
int   CtrlThr_LockPwm_Time=0;
float CtrlThr_UnLockPwm_Ipeak=0;
// Status
int Status_NPRRun=0;
// Ctrl count
int CtrlCnt_IgbtFanOn=0;
int CtrlCnt_LedTogg=0;
int CtrlCnt_RstDelay=0;
int CtrlCnt_ManUnload=0;
int CtrlCnt_IORunRefFeed=0;
int CtrlCnt_IORstRefFeed=0;
int CtrlCnt_IOFltDelay=0;
int CtrlCnt_ReactorFanOn=0;

u16 CtrlCnt_IOSingCtrl=0;
u16 CtrlCnt_IOParaCtrl=0;
u16 CtrlCnt_IODgssCtrl=0;
// Flag
int Flag_IOSingCtrl=0;
int Flag_IOParaCtrl=0;
int Flag_IODgssCtrl=0;
int Flag_IORstRef=0;

int FlagRem_Combine=0;
int FlagRem_Split=0;
int FlagRem_Reset=0;
int FlagRem_ParaOn=0;
int FlagRem_ParaOff=0;
int FlagRem_SingOn=0;
int FlagRem_SingOff=0;

int Flag_LockPWM=0;
int Flag_IORunRef=0;

int Flag_IORstRefOld=0;
int Flag_FanCapOn=0;
int Flag_EEPROMCheck=0;
int Flag_RstCmdOld=0;
int Flag_PwmModuMode=1;

// Fault Alarm Remap Word
union DataBit_Type FltRemapWord1;
union DataBit_Type FltRemapWord2;
union DataBit_Type FltRemapWord3;
union DataBit_Type AlmRemapWord1;
union DataBit_Type AlmRemapWord2;
union DataBit_Type AlmRemapWord3;
union DataBit_Type StaRemapWord1, StaRemapWord2;
////////////////////////////////////////////////////////////////////
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
	MainSta2Word.DataInt=0;

	FaultWord1.DataInt=0;
	FaultWord2.DataInt=0;
	FaultWord3.DataInt=0;
	FaultWord4.DataInt=0;
	FaultWord5.DataInt=0;
	FaultWord6.DataInt=0;
	FaultWord7.DataInt=0;
	FaultWord8.DataInt=0;

	SwitchWord1.DataInt=0;
	SwitchWord2.DataInt=0;
	SwitchWord3.DataInt=0;

	NPR_HWFMWord0.DataInt=0;
	NPR_HWFMWord1.DataInt=0;
	NPR_HWFMWord2.DataInt=0;
	NPR_HWFMWord3.DataInt=0;
	NPR_HWFMWord4.DataInt=0;
	NPR_HWFMWord5.DataInt=0;
	NPR_HWFMWord6.DataInt=0;
	NPR_HWFMWord7.DataInt=0;

	Cap_Ctrl_Word.DataInt=0;
	memset(&RemInputSum, 0, sizeof(RemoteInput_t));
	memset(&DcsInputSum, 0, sizeof(RemoteInput_t));
	RemHoldSum.CtrlWord.all = 0;
	DcsHoldSum.CtrlWord.all = 0;
}
/*******************************************************************************************/
void variable_algorithm_init(void)
{
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
	FltCnt_OverTime_ACT = 0;
	FltCnt_OverTime_FCC = 0;
	FltCnt_OverTime_FanCap = 0;
	FltCnt_OverTime_FanIgbt = 0;
	FltCnt_OverTime_FanReactor = 0;
	FltCnt_OverDvalue_Tempr = 0;
	FltCnt_OverCurr_Irms = 0;
	FltCnt_OverCurr_CapIrms = 0;
	FltCnt_OverCurr_Isum = 0;
	FltCnt_UnderVolt_Udc = 0;
	FltCnt_OverVolt_Grid = 0;
	FltCnt_UnderVolt_Grid = 0;
	FltCnt_OverFreq_Grid = 0;
	FltCnt_UnderFreq_Grid = 0;
	FltCnt_VolgImba_Grid = 0;
	FltCnt_OverTemp_IGBT = 0;
	FltCnt_OverTime_LockPWM = 0;
	FltCnt_OverTemp_Rectifier = 0;
	FltCnt_OverTemp_Inverter = 0;
	FltCnt_OverTemp_Transformer = 0;
	FltThr_OverWaterCount = 0;
	////////////////////////////////////////////
	NPR_FaultCode = 0;
	FaultWord1.DataInt = 0;
	FaultWord2.DataInt = 0;
	FaultWord3.DataInt = 0;
	FaultWord4.DataInt = 0;
	FaultWord5.DataInt = 0;
	FaultWord6.DataInt = 0;
	FaultWord7.DataInt = 0;
	FaultWord8.DataInt = 0;

	AlarmWord1.DataInt=0;
	AlarmWord2.DataInt=0;
	////////////////////////////////////////////
	MainCtrlWord.DataInt = 0;
	MainCtrlWord_PC.DataInt = 0;
	MainCtrlWord_CAN.DataInt = 0;

	CtrlCnt_IgbtFanOn = 0;
	////////////////////////////////////////////
	Status_NPRRun = 0;
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

	if((Code >= 1) && (Code <= 16))
	{
		FaultWord2.DataInt |= (1<<(Code-1));
	}
	else if((Code >= 17) && (Code <= 32))
	{
		FaultWord1.DataInt |= (1<<(Code-17));
	}
	else if((Code >= 33) && (Code <= 48))
	{
		FaultWord3.DataInt |= (1<<(Code-33));
	}
	else if((Code >= 49) && (Code <= 64))
	{
		FaultWord4.DataInt |= (1<<(Code-49));
	}
	else if((Code >= 65) && (Code <= 80))
	{
		FaultWord5.DataInt |= (1<<(Code-65));
	}
	else if((Code >= 81) && (Code <= 96))
	{
		FaultWord6.DataInt |= (1<<(Code-81));
	}
	else if((Code >= 97) && (Code <= 112))
	{
		FaultWord7.DataInt |= (1<<(Code-97));
	}
	else if((Code >= 113) && (Code <= 128))
	{
		FaultWord8.DataInt |= (1<<(Code-113));
	}

}
/*****************************************************************************************/
u16 FaultMask_Map(u16 Code)
{
	u16 Remain = 0;
	u16 Quotient = 0;
	u16 Rrn = 0;

	Quotient = ((Code-1) >> 4);
	Remain = (Code-1) % 16 ;
	switch(Quotient)
	{
		case 0:
		{
			if(((NPR_HWFMWord0.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 1:
		{
			if(((NPR_HWFMWord1.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 2:
		{
			if(((NPR_HWFMWord2.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 3:
		{
			if(((NPR_HWFMWord3.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 4:
		{
			if(((NPR_HWFMWord4.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 5:
		{
			if(((NPR_HWFMWord5.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 6:
		{
			if(((NPR_HWFMWord6.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;
		}break;
		case 7:
		{
			if(((NPR_HWFMWord7.DataInt >> Remain)&0x0001)==0x0001) Rrn = 1;

		}break;
		default:
		{
			break;
		}
	}
	return Rrn;
}

void AlarmWord_Map(u16 Code)
{
	if((Code >= 1) && (Code <= 16))
		AlarmWord1.DataInt |= (1<<(Code-1));
	if((Code >= 17) && (Code <= 32))
		AlarmWord2.DataInt |= (1<<(Code-17));
	if((Code >= 33) && (Code <= 48))
		AlarmWord3.DataInt |= (1<<(Code-33));
	if((Code >= 49) && (Code <= 64))
		AlarmWord4.DataInt |= (1<<(Code-49));
	if((Code >= 65) && (Code <= 80))
		AlarmWord5.DataInt |= (1<<(Code-65));
	if((Code >= 81) && (Code <= 96))
		AlarmWord6.DataInt |= (1<<(Code-81));
	if((Code >= 97) && (Code <= 112))
		AlarmWord7.DataInt |= (1<<(Code-97));
	if((Code >= 113) && (Code <= 128))
		AlarmWord8.DataInt |= (1<<(Code-113));
}
/********************************************************************************************/

void AlarmWord_Clear(u16 Code)
{
	if((Code >= 1) && (Code <= 16))
		AlarmWord1.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 17) && (Code <= 32))
		AlarmWord2.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 33) && (Code <= 48))
		AlarmWord3.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 49) && (Code <= 64))
		AlarmWord4.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 65) && (Code <= 80))
		AlarmWord5.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 81) && (Code <= 96))
		AlarmWord6.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 97) && (Code <= 112))
		AlarmWord7.DataInt &= (~ (1<<(Code-1)) );
	if((Code >= 113) && (Code <= 128))
		AlarmWord8.DataInt &= (~ (1<<(Code-1)) );
}

//===========================================================================
// No more.
//===========================================================================



