extern void Flash_Init(int *SendBuf,int *RecvBuf,int *Flash_step,int *Flash_Substep,unsigned int *Time_1ms);
extern int Flash_Read(long PageAddr,int Offset,int Max,int *SendBuf,int *RecvBuf,int *Flash_Substep);
extern int Flash_Write(long PageAddr,int Offset,int Max,int *SendBuf,int *RecvBuf,int *Flash_Substep,unsigned int *Time_1ms);
