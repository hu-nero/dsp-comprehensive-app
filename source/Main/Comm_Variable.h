#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#ifndef _Comm_Variable_H_
#define _Comm_Variable_H_


/**********************************************************************************************/
////////////////////////////////////////////////////
extern unsigned int MCS2MPR_NodeID;
extern int ECANA_NodeID;

extern int NODE_STATE;
extern int PDO_SEND_FLAG;
extern int NMTTimeInterval;

//////////////////////////////////////////////
extern int device_name;
extern int data_1005;
extern int data_1006;
extern int data_100C;
extern int data_100D;
extern int Guard_time;
extern int Life_factor;

/**********************************************************************************************/
extern int SCICRecvBuffer[100],SCICRecvTail,SCICRecvFront;
extern int SCICSendBuffer[100],SCICSendTail,SCICSendFront;
extern int Modbus_1ms;



///////////////////////////////////////////////////////////
                     //Monitor
//////////////////////////////////////////////////////////
extern int Monitor_Ver[20];
extern int Mon_Recv_Status;
#define SCIBSendBufferSIZE 1024 
#define SCIBRecvBufferSIZE 512 
extern UINT SCIBSendBuffer[SCIBSendBufferSIZE];
extern UINT SCIBRecvBuffer[SCIBRecvBufferSIZE];
extern UINT SCIBSendFront;
extern UINT SCIBSendTail;
extern UINT SCIBRecvFront;
extern UINT SCIBRecvTail;
extern UINT SCIBSend_1ms;

#define ANALOGY_CHANNEL_NUM 8
#define DIGIT_CHANNEL_NUM   8
extern UINT SoftScope_sts;
extern UINT SoftScope_Counter;
extern UINT SoftScope_DataLen;
extern UINT SoftScope_OutQueueNum;
extern UINT SoftScope_SendDataNum;
extern ULONG SoftScope_FSNCount;
extern UINT SoftScope_buffer[50];
extern UINT *SoftScope_AnalogChaddr[ANALOGY_CHANNEL_NUM];
extern UINT *SoftScope_DigitChaddr[DIGIT_CHANNEL_NUM];

#define ANALOGY_CHANNEL_NUM 8
#define DIGIT_CHANNEL_NUM 8
extern UINT SoftScope_OnOffCmd;    
extern UINT SoftScope_SampleInterval;
extern UINT SoftScope_AnalogCHNum; 
extern UINT SoftScope_DigitCHNum;
extern UINT SoftScope_AnalogChID[ANALOGY_CHANNEL_NUM];
extern UINT SoftScope_DigitChID[DIGIT_CHANNEL_NUM];
extern UINT SoftScope_DigitChBit[DIGIT_CHANNEL_NUM];
extern UINT SoftScope_SampleFreq;
///////////////////////////////////////////////////////////////////////////////
extern unsigned int EventLog_ReadCmd;
extern unsigned long EventLog_ReadFromNum;
extern unsigned long EventLog_ReadEndNum;
extern unsigned long EventLog_ReadDeltaNum;
extern unsigned long EventLog_ReadCurrNum;
extern unsigned long EventLog_SendedNum;
extern unsigned long EventLog_TotalNum;
extern unsigned long EventLog_NewestNum,EventLog_NewestNumOld;
extern unsigned int EventLog_ReadBuf[8][16];
extern unsigned int EventLog_SaveBuf[8][16];
extern unsigned int EventLog_SaveFront;
extern unsigned int EventLog_SaveEnd;
extern int EventLog_Test;
extern long EventLog_PermitMaxNum;
extern int EventLogStep;
extern int EventLog_HardReset;
////////////////////////////////////////
extern unsigned int DataLog_CtrlCmd;
extern unsigned int DataLog_RunStatus;
extern unsigned int DataLog_TrigMode;
extern unsigned int DataLog_ManTrigCmd;
extern unsigned int DataLog_AnalagCHNum;
extern unsigned int DataLog_DigitCHNum;
extern unsigned int DataLog_SampPeriod;
extern unsigned int DataLog_Savepers;
extern unsigned int DataLog_SaveAnalogCHID[32];
extern unsigned int DataLog_SaveDigitCHID[32];
extern unsigned int DataLog_SaveDigitCHBit[32];
extern unsigned int DataLog_TrigAnalogCHEnSet;
extern unsigned int DataLog_TrigAnalogCHModeSet;
extern unsigned int DataLog_TrigAnalogCHID[4];
extern int DataLog_TrigAnalogCHThr[4];
extern unsigned int DataLog_TrigDigitCHEnSet;
extern unsigned int DataLog_TrigDigitCHModeSet;
extern unsigned int DataLog_TrigDigitCHID[4];
extern unsigned int DataLog_TrigDigitCHBit[4];

extern unsigned long DataLog_ReadEndNum;
extern unsigned long DataLog_ReadFromNum;
extern unsigned int DataLog_ReadCmd;

extern int DataLog_PermitMaxNum;
extern int DataLog_TotalNum;
extern int DataLog_NewestNum,DataLog_NewestNumOld;
extern int DataLog_IndexSendNum;
extern int DataLog_DataSendNum;

extern int *DataLog_AnalogAddr[32];
extern unsigned int *DataLog_DigitAddr[32];
extern int DataLog_Buf[10000][18];
extern int DataLog_Trig[130];
extern unsigned int DataLog_ChannelSend[130];
extern unsigned int DataLog_SendBuf[180000];
extern unsigned int DataLog_NumCount;

extern int *DataLog_TrigAnalogAddr[4];
extern unsigned int *DataLog_TrigDigitAddr[4];
extern int DataLog_TrigAnalogValueOld[4];
extern unsigned int DataLog_TrigDigitValueOld;

extern int DataLog_SaveStepCount;
extern int DataLogStep;

extern UINT ParaDownloadCtrlWord;
extern int ParaDownloadFaultNum;           
extern long SRAM_RandomRead_Addr;     
extern int SRAM_RandomRead_Value; 
extern long SRAM_RandomRead_LValue;   
/************************************************************************************************************/
////////////////////////////
////////////////////////////
extern unsigned int SPI_EEPRPM_BusyStatus;
extern unsigned int SPIaddrPage;
extern unsigned int SPIaddrOffset;
extern unsigned int SPIBuffer[128];
extern unsigned int EEPROMWriteCtrl;
extern unsigned int EEPROMReadCtrl;
extern unsigned int EEPROMAceNum;
/************************************************************************************************************/
////////////////////////////
/////////////////////////////////////
extern UINT RTCAccessStatus;
extern UINT SysTime_Second;
extern UINT SysTime_Minute;
extern UINT SysTime_Hour;
extern UINT SysTime_Day;
extern UINT SysTime_Month;
extern UINT SysTime_Year;
extern UINT SysTime_TimeSecond;
extern UINT SysTime_MicroSecond;
extern int RealTime1ms;
extern long ulRealTimeSecond;
extern ULONG ulRealTimeSecondSet;
extern long Time_H;
extern long Time_L;
/************************************************************************************************************/
#define Modbus_Send_BUFF_SIZE  128
#define Modbus_Recv_BUFF_SIZE  128
extern u16 Modbus_RecvFront;
extern u16 Modbus_RecvTail;
extern u16 Modbus_SendFront;
extern u16 Modbus_SendTail;
extern u16 Modbus_SendBuffer[Modbus_Send_BUFF_SIZE];
extern u16 Modbus_RecvBuffer[Modbus_Recv_BUFF_SIZE];
extern u16 SlaveAckBuf[256];

extern u16 MbsRem_RecvFront;
extern u16 MbsRem_RecvTail;
extern u16 MbsRem_SendFront;
extern u16 MbsRem_SendTail;
extern u16 MbsRem_SendBuffer[Modbus_Send_BUFF_SIZE];
extern u16 MbsRem_RecvBuffer[Modbus_Recv_BUFF_SIZE];

extern u16 MbsDcs_RecvFront;
extern u16 MbsDcs_RecvTail;
extern u16 MbsDcs_SendFront;
extern u16 MbsDcs_SendTail;
extern u16 MbsDcs_SendBuffer[Modbus_Send_BUFF_SIZE];
extern u16 MbsDcs_RecvBuffer[Modbus_Recv_BUFF_SIZE];
/************************************************************************************************************/


#endif
