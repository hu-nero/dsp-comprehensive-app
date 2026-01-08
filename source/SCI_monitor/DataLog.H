extern void ComToDataLogCmd(unsigned int *PData);
extern void DataLog_Data_FlashRead(long DataStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,int *Count);
extern void DataLog_Index_FlashRead(long DataStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,unsigned int *pTxBuf,unsigned int *pTxTail);
extern void DataLog_Send(unsigned int Time_1ms,unsigned int *pTxBuf,unsigned int *pTxTail);
extern int DataLog_TrigCtrl(void);
extern void DataLog_Cache(int Data_Buf[][18],unsigned int *Num,unsigned int Ts_1ms,int *DataStep);
extern void DataLog_Save(long IndexPageAddr,long DataStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,int *DataStep,int *Count,int Num,unsigned int *Time_1ms);
extern int DataLog_VariebInit(long PageAddr,int Offset,int *SendBuf,int *RecvBuf,int *SubStep,int *DataLogStep,unsigned int *Time_1ms);

extern const int DataLog_Ver[3];
