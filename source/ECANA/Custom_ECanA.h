/******************************************************************/
//�汾��¼
////////////////////////////
//V1.2�汾
//��V1.1�汾���϶Կ��ƼĴ��������޸ģ������ϵ縴λ����
///////////////////////////
#include "Pub_ECAN_Macro.H"
extern void InitECana(void);
extern void CANAMB0HardwareSend(unsigned int Buffer[][6],unsigned int teal,unsigned int *front,int NumMax,unsigned int Time_ms);
extern void CANAMB4HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB5HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB6HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB7HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB8HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB9HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
extern void CANAMB10HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal,int NumMax);
//////////////////////////////////////////////////////////////
extern const struct CANPDO MPR2NPR_RPDOObj[];
extern const struct CANPDO MPR2NPR_TPDOObj[];
extern const struct SDOSummaryType MPR2NPR_CANSDOSummary[];
extern unsigned int ECANA_Toggle;//����λ

extern const struct CANPDO ECANA_CH2_RPDOObj[];
extern const struct CANPDO ECANA_CH2_TPDOObj[];
extern unsigned int ECANA_CH2_Toggle;//����λ

extern const struct CANPDO ECANA_CH3_RPDOObj[];
extern const struct CANPDO ECANA_CH3_TPDOObj[];
extern unsigned int ECANA_CH3_Toggle;//����λ
/////////////////////////////////////////////////////////////
extern void CANA_NPR_Send_MPR(void);
extern void CANA_NPR_Recv_MPR(void);
extern void CANA_Sector_Send_Power(void);
extern void CANA_Sector_Recv_Power(void);
////////////////////////////////////////////////////////////////////
