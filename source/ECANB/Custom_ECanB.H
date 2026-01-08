//版本记录
////////////////////////////
//V1.2版本
//在V1.1版本基础上对控制寄存器进行修改，增加上电复位功能
/*****************************************************************************************/
//第一层：硬件相关层
//采用邮箱0发送，邮箱4/5接收，发送采用查询方式，接收采用GIF0中断
extern void InitECanb(void);

extern int CANBMB0HardwareSend(unsigned int PDOBuffer[][6],unsigned int PDOteal,unsigned int *PDOfront,unsigned int SDOBuffer[][6],unsigned int SDOteal,unsigned int *SDOfront,unsigned int Time_ms);

#pragma CODE_SECTION(CANBMB4HardwareRecv, "ramfuncs");
extern void CANBMB4HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal);

#pragma CODE_SECTION(CANBMB5HardwareRecv, "ramfuncs");
extern void CANBMB5HardwareRecv(unsigned int Buffer[][6],unsigned int *Teal);
/**************************************************************************************/
//第二层，应用相关层，主控站点为主机
//主控程序接口
extern void MCS2MPR_ProtocalSend_Slaver(unsigned int Buffer[][6],unsigned int *Teal,unsigned int Send_1ms,unsigned int Permit);
extern void MCS2MPR_ProtocalRecv_Slaver(unsigned int Buffer[][6],unsigned int *Front,unsigned int Teal,unsigned int SendBuffer[][6],unsigned int *SendTeal);
extern void MCS2MPR_ProtocalSend_Status(unsigned int Buffer[][6],unsigned int *Teal);
/**************************************************************************************/
extern void CANB_NPR_Send_MCS(void);
extern void CANB_NPR_Recv_MCS(void);
/**************************************************************************************/


