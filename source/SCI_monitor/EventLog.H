extern void ComToEventRead(unsigned int *PData);
extern void EventLog_Read(long EventBufPageAddr,long EventStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,int *EventStep,unsigned int *Time_1ms,unsigned int *pTxBuffer,Uint16 *pTxTail);
extern void EventLog_BufSave(long IndexPageAddr,long EventStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,int *EventStep,unsigned int *Time_1ms);
extern void EventLog_SectorSave(long EventBufPageAddr,long EventStartPageAddr,int *SendBuf,int *RecvBuf,int *SubStep,int *Flash_Step,int *EventStep,unsigned int *Time_1ms);
extern void EventLog_Pack(long OldValue,long NewValue,int EventID,int ParaID);
extern int EventLog_VariebInit(long PageAddr,int Offset,int *SendBuf,int *RecvBuf,int *SubStep,int *EventStep,unsigned int *Time_1ms);

extern const int EventLog_Ver[3];
