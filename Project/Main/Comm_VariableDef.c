
#include "Comm_Variable.H"
/**********************************************************************************************/
/////////////////////////////////////////////////////
int NODE_STATE=0;
int PDO_SEND_FLAG = 0;
int NMTTimeInterval=0;

int CANARPDO[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int CANATPDO[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int CANBRPDO[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int CANBTPDO[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
///////////////////////////////////////////////
int device_name=0x0000;
int data_1005=0;
int data_1006=0;
int data_100C=0;
int data_100D=0;
int Guard_time = 1000;
int Life_factor = 1;
//////////////////////////////////////////////////////

////////
                     //CAN
////////
unsigned int CANSend_1ms =0,CANSend_1msOld=0;
unsigned int ECAN_CommFlag=0;
//////////////////////////////////////////////////////
/////////
                     //CANA
/////////
unsigned int CANAMB0SendBuffer[16][6];
unsigned int CANAMB0SendFront=0;
unsigned int CANAMB0SendTeal=0;
unsigned int ECANA_CH1_SDOIndex=1;
unsigned int ECANA_CH1_SDOSubIndex=1;
///////////////////////////////
unsigned int CANAMB4RecvBuffer[4][6];
unsigned int CANAMB4RecvFront=0;
unsigned int CANAMB4RecvTeal=0;
unsigned int CANA_CH1_HCount=0,CANA_CH1_LCount=0;
/////////////////////////////
unsigned int CANAMB5RecvBuffer[4][6];
unsigned int CANAMB5RecvFront=0;
unsigned int CANAMB5RecvTeal=0;
unsigned int CANA_CH2_HCount=0,CANA_CH2_LCount=0;
/////////////////////////////
unsigned int CANAMB6RecvBuffer[4][6];
unsigned int CANAMB6RecvFront=0;
unsigned int CANAMB6RecvTeal=0;
unsigned int CANA_CH3_HCount=0,CANA_CH3_LCount=0;
/////////////////////////////
unsigned int CANAMB7RecvBuffer[4][6];
unsigned int CANAMB7RecvFront=0;
unsigned int CANAMB7RecvTeal=0;
unsigned int CANA_CH4_HCount=0,CANA_CH4_LCount=0;
/////////////////////////////
unsigned int CANAMB8RecvBuffer[4][6];
unsigned int CANAMB8RecvFront=0;
unsigned int CANAMB8RecvTeal=0;
unsigned int CANA_CH5_HCount=0,CANA_CH5_LCount=0;
/////////
                     //CANB
/////////
//////////////////////////////////////////
unsigned int CANBMB0PDOSendBuffer[8][6];
unsigned int CANBMB0PDOSendFront=0;
unsigned int CANBMB0PDOSendTeal=0;

unsigned int CANBMB0SDOSendBuffer[8][6] ;
unsigned int CANBMB0SDOSendFront=0;
unsigned int CANBMB0SDOSendTeal=0;

unsigned int CANBMB4RecvBuffer[8][6];
unsigned int CANBMB4RecvFront=0;
unsigned int CANBMB4RecvTeal=0;

unsigned int CANBMB5RecvBuffer[8][6];
unsigned int CANBMB5RecvFront=0;
unsigned int CANBMB5RecvTeal=0;
/**********************************************************************************************/
int SCICRecvBuffer[100],SCICRecvTail=0,SCICRecvFront=0;
int SCICSendBuffer[100],SCICSendTail=0,SCICSendFront=0;
int Modbus_1ms=0;



///////////////////////////////////////////////////////////
                     //Monitor
//////////////////////////////////////////////////////////
int Monitor_Ver[20];
int Mon_Recv_Status=0;
UINT SCIBSendBuffer[SCIBSendBufferSIZE];
UINT SCIBRecvBuffer[SCIBRecvBufferSIZE];
UINT SCIBSendFront=0;
UINT SCIBSendTail=0;
UINT SCIBRecvFront=0;
UINT SCIBRecvTail=0;
UINT SCIBSend_1ms=0;

UINT SoftScope_sts = 0;
UINT SoftScope_Counter = 0;
UINT SoftScope_DataLen = 0;
UINT SoftScope_OutQueueNum = 0;
UINT SoftScope_SendDataNum = 0;
ULONG SoftScope_FSNCount = 0;
UINT SoftScope_buffer[50];
UINT *SoftScope_AnalogChaddr[ANALOGY_CHANNEL_NUM];
UINT *SoftScope_DigitChaddr[DIGIT_CHANNEL_NUM];

UINT SoftScope_OnOffCmd = 0;
UINT SoftScope_SampleInterval = 0;
UINT SoftScope_AnalogCHNum = 0;
UINT SoftScope_DigitCHNum = 0;
UINT SoftScope_AnalogChID[ANALOGY_CHANNEL_NUM];
UINT SoftScope_DigitChID[DIGIT_CHANNEL_NUM];
UINT SoftScope_DigitChBit[DIGIT_CHANNEL_NUM];
UINT SoftScope_SampleFreq = 0;
//////////////////////////////////////////////////////
unsigned int EventLog_ReadCmd=0;
unsigned long EventLog_ReadFromNum=0;
unsigned long EventLog_ReadEndNum=0;
unsigned long EventLog_ReadCurrNum=0;
unsigned long EventLog_ReadDeltaNum=0;
unsigned long EventLog_SendedNum=0;
unsigned long EventLog_TotalNum=0;
unsigned long EventLog_NewestNum=0,EventLog_NewestNumOld=0;
unsigned int EventLog_ReadBuf[8][16];
unsigned int EventLog_SaveBuf[8][16];
unsigned int EventLog_SaveFront=0;
unsigned int EventLog_SaveEnd=0;
int EventLog_Test=0;
long EventLog_PermitMaxNum=0;
int EventLogStep=0;
int EventLog_HardReset=1;
////////////////////////////////////////////////////
unsigned int DataLog_CtrlCmd=1;
unsigned int DataLog_TrigMode=0;
unsigned int DataLog_ManTrigCmd=0;
unsigned int DataLog_AnalagCHNum=0;
unsigned int DataLog_DigitCHNum=0;
unsigned int DataLog_SampPeriod=1;
unsigned int DataLog_Savepers=30;
unsigned int DataLog_SaveAnalogCHID[32]={0x107,0xE02,0xE03,0xE04,0xE05,0xE06,0xE07,0xE08,
	                                       0xE09,0xE0A,0xE0B,0xE0C,0xE0D,0xE0E,0xE0F,0xE10,
	                                       0,0,0,0,0,0,0,0,
	                                       0,0,0,0,0,0,0,0,};
unsigned int DataLog_SaveDigitCHID[32]={0x33E,0,0,0,0, 0,0,0,0,0,
	                                      0,0,0,0,0, 0,0,0,0,0, 
	                                      0,0,0,0,0, 0,0,0,0,0,
	                                      0,0};
unsigned int DataLog_SaveDigitCHBit[32]={13,0,0,0,0, 0,0,0,0,0, 
	                                       0,0,0,0,0, 0,0,0,0,0, 
	                                       0,0,0,0,0, 0,0,0,0,0, 
	                                       0,0};
unsigned int DataLog_TrigAnalogCHEnSet=0;
unsigned int DataLog_TrigAnalogCHModeSet=0;
unsigned int DataLog_TrigAnalogCHID[4]={0,0,0,0};
int DataLog_TrigAnalogCHThr[4]={0,0,0,0};
unsigned int DataLog_TrigDigitCHEnSet=1;
unsigned int DataLog_TrigDigitCHModeSet=0x0001;
unsigned int DataLog_TrigDigitCHID[4]={0x33E,0,0,0};
unsigned int DataLog_TrigDigitCHBit[4]={13,0,0,0};

unsigned long DataLog_ReadEndNum=0;
unsigned long DataLog_ReadFromNum=0;
unsigned int DataLog_ReadCmd=0;

int DataLog_PermitMaxNum=0;
int DataLog_TotalNum=0;
int DataLog_NewestNum=0,DataLog_NewestNumOld=0;
int DataLog_IndexSendNum=0;
int DataLog_DataSendNum=0;

int *DataLog_AnalogAddr[32]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0};
unsigned int *DataLog_DigitAddr[32]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0};
unsigned int DataLog_NumCount=0;
int DataLog_Trig[130]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                     0,0,0,0,0, 0,0,0,0,0};
unsigned int DataLog_ChannelSend[130]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
	                            0,0,0,0,0, 0,0,0,0,0};

int *DataLog_TrigAnalogAddr[4]={0,0,0,0};
unsigned int *DataLog_TrigDigitAddr[4]={0,0,0,0};
int DataLog_TrigAnalogValueOld[4]={0,0,0,0};
unsigned int DataLog_TrigDigitValueOld=0;

int DataLog_SaveStepCount=0;
int DataLogStep=0;

UINT ParaDownloadCtrlWord = 0;
int ParaDownloadFaultNum=0;
long SRAM_RandomRead_Addr=0x8000;
int SRAM_RandomRead_Value=0;
long SRAM_RandomRead_LValue=0;
/************************************************************************************************************/
/////////////////////
/////////////////////
unsigned int SPI_EEPRPM_BusyStatus=0;
unsigned int SPIaddrPage=0;
unsigned int SPIaddrOffset=0;
unsigned int SPIBuffer[128];
unsigned int EEPROMWriteCtrl=0;
unsigned int EEPROMReadCtrl=0;
unsigned int EEPROMAceNum=0;
/************************************************************************************************************/
/////////////////////////////
/////////////////////////////
UINT RTCAccessStatus=0;
UINT SysTime_Second = 0;
UINT SysTime_Minute = 0;
UINT SysTime_Hour  = 0;
UINT SysTime_Day  = 0;
UINT SysTime_Month  = 0;
UINT SysTime_Year = 0;
UINT SysTime_MicroSecond = 0;
int RealTime1ms=0;
long ulRealTimeSecond = 0;
ULONG ulRealTimeSecondSet = 0;
long Time_H = 0;
long Time_L = 0;
/************************************************************************************************************/
//////////////////////////////////////////////////////////////
int Flash_Capacity=0;
int Flash_MID[3];
int Flash_PSNSerial[9];

int Flash_Step=0;
int Flash_SubStep=0;

unsigned int Flash_Access_1ms=0;
long Flash_Access_Ctrl=0;
long Flash_Fault_Count1ms=0;
/************************************************************************************************************/
u16 Modbus_RecvFront=0;
u16 Modbus_RecvTail=0;
u16 Modbus_SendFront=0;
u16 Modbus_SendTail=0;
u16 Modbus_SendBuffer[Modbus_Send_BUFF_SIZE]={0};
u16 Modbus_RecvBuffer[Modbus_Recv_BUFF_SIZE]={0};













