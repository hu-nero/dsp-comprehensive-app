//Ver1.2,Programmed by lds,20121025
#include "Pub_ECAN_Macro.H"
/******************************************************************************/
extern void CAN_Pub_Low_CHx_ProtocalSend(unsigned int PDOBuffer[][6],unsigned int *PDOTeal,int NumMax,const struct CANPDO *P,unsigned int Send_1ms);
extern void CAN_Pub_High_CHx_ProtocalSend(unsigned int PDOBuffer[][6],unsigned int *PDOTeal,int NumMax,const struct CANPDO *P,const struct CANPDO *P_Old,unsigned int Send_1ms);
extern void CAN_Pub_CHx_ProtocalRecv(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal,int NumMax,const struct  CANPDO *P);
extern void CAN_Pub_CHx_NMTRecv_Slaver(unsigned int RecvBuffer[][6],unsigned int *RecvFront,unsigned int RecvTeal,int RecvNumMax,
            unsigned int *NMTMess,unsigned int SendBuffer[][6],unsigned int *SendTeal,int SendNumMax);
extern void CAN_Pub_CHx_SDO_ProtocalSend(unsigned int SDOBuffer[][6],unsigned int *SDOTeal,int SDNumMax,const struct SDOSummaryType *PS,
            unsigned int *Index,unsigned int *SubIndex,unsigned int Send_Interval,unsigned int Send_1ms);
extern void CAN_Pub_CHx_SDO_ProtocalRecv_Master(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal,int SDNumMax,
            const struct SDOSummaryType *PS);
extern void CAN_Pub_CHx_SDO_ProtocalRecv_Slaver(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal,int RecvNumMax,
            const struct SDOSummaryType *PS,unsigned int SendBuffer[][6],unsigned int *SendTeal,int SendNumMax);




