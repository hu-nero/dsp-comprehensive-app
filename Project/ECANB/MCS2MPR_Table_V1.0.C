#include "main\Variable.H"
#include "main\Comm_Variable.H"
#include "ECANB_Macro.H"
/***************************************************************************/
unsigned int MCS2MPR_NodeID = 1;	//����ͨѶ������վ��
unsigned int BaundNum = 3;	//����ͨѶ������
int ECANB_PDOTimeInterval = 10;
int MCS2MPR_SDOTimeInterval = 20;
int MCS2MPR_Reserved;


extern int device_name;
extern int data_1005;
extern int data_1006;
extern int data_100C;
extern int data_100D;
extern int Guard_time;
extern int Life_factor;
////////////////////�û��ӿڱ�������////////////////////////////////////
//����û���ͬ�����޸�
//canopen������֮��ͨ��
const struct CANPDO MCS2MPR_RPDOObj[4]=
{
	(RPDO1|0x4000), 0x0005, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBRPDO[0][0]),(Uint16 *)(&CANBRPDO[0][1]),(Uint16 *)(&CANBRPDO[0][2]),(Uint16 *)(&CANBRPDO[0][3]),
	(RPDO2|0x0000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBRPDO[1][0]),(Uint16 *)(&CANBRPDO[1][1]),(Uint16 *)(&CANBRPDO[1][2]),(Uint16 *)(&CANBRPDO[1][3]),
	(RPDO3|0x0000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBRPDO[2][0]),(Uint16 *)(&CANBRPDO[2][1]),(Uint16 *)(&CANBRPDO[2][2]),(Uint16 *)(&CANBRPDO[2][3]),
	(RPDO4|0x0000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBRPDO[3][0]),(Uint16 *)(&CANBRPDO[3][1]),(Uint16 *)(&CANBRPDO[3][2]),(Uint16 *)(&CANBRPDO[3][3]),
};
const struct CANPDO MCS2MPR_TPDOObj[4]=
{
	(TPDO1|0x4000), 0x0003, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBTPDO[0][0]),(Uint16 *)(&CANBTPDO[0][1]),(Uint16 *)(&CANBTPDO[0][2]),(Uint16 *)(&CANBTPDO[0][3]),
	(TPDO2|0x4000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBTPDO[1][0]),(Uint16 *)(&CANBTPDO[1][1]),(Uint16 *)(&CANBTPDO[1][2]),(Uint16 *)(&CANBTPDO[1][3]),
	(TPDO3|0x4000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBTPDO[2][0]),(Uint16 *)(&CANBTPDO[2][1]),(Uint16 *)(&CANBTPDO[2][2]),(Uint16 *)(&CANBTPDO[2][3]),
	(TPDO4|0x4000), 0x0008, &ECANB_PDOTimeInterval, (Uint16 *)(&CANBTPDO[3][0]),(Uint16 *)(&CANBTPDO[3][1]),(Uint16 *)(&CANBTPDO[3][2]),(Uint16 *)(&CANBTPDO[3][3]),
};
/////////////////////////////////////////////////////////////////////////
const struct CANSDO MCS2MPR_SDOObject_0x1000[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x810B,(void*)(&device_name),	// need 2bytes �豸���
};
const struct CANSDO MCS2MPR_SDOObject_0x1005[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x8103,(void*)(&data_1005),	// need 4bytes
};

const struct CANSDO MCS2MPR_SDOObject_0x1006[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x8103,(void*)(&data_1006),	// need 4bytes
};

const struct CANSDO MCS2MPR_SDOObject_0x100C[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x810B,(void*)(&Guard_time)	// ͨ������ need �ڵ�ʱ��
};

const struct CANSDO MCS2MPR_SDOObject_0x100D[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x810F,(void*)(&Life_factor),	// need ��������
};
const struct CANSDO MCS2MPR_SDOObject_0x1018[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x810B,(void*)(&Life_factor),//
};
/////////////////////////////////////////////////////
const struct CANSDO MCS2MPR_SDOObject_0x2000[]=
{
	0     ,0x0001,0x0000,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBRPDO[0][0]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2001[]=
{
	0     ,0x0001,0x0000,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBRPDO[0][1]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2002[]=
{
	0     ,0x0001,0x0000,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBRPDO[0][2]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2003[]=
{
	0     ,0x0001,0x0000,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBRPDO[0][3]),
};
//////////////////////////////////////
const struct CANSDO MCS2MPR_SDOObject_0x2010[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[0][0]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2011[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[0][1]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2012[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[0][2]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2013[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[0][3]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2014[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[1][0]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2015[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[1][1]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2016[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[1][2]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2017[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[1][3]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2018[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[2][0]),
};
const struct CANSDO MCS2MPR_SDOObject_0x2019[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[2][1]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201A[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[2][2]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201B[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[2][3]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201C[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[3][0]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201D[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[3][1]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201E[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[3][2]),
};
const struct CANSDO MCS2MPR_SDOObject_0x201F[]=
{
	0     ,0x0001,     0,(void*)(&MCS2MPR_Reserved),
	0x0008,0x0000,0x800B,(void*)(&CANBTPDO[3][3]),
};
////////////////////////////////////////
const struct SDOSummaryType MCS2MPR_CANSDOSummary[]=
{
	    26,(void*)(&MCS2MPR_Reserved),
	0x1000,(void*)(&MCS2MPR_SDOObject_0x1000),
	0x1005,(void*)(&MCS2MPR_SDOObject_0x1005),
	0x1006,(void*)(&MCS2MPR_SDOObject_0x1006),
	0x100C,(void*)(&MCS2MPR_SDOObject_0x100C),
	0x100D,(void*)(&MCS2MPR_SDOObject_0x100D),
	0x1018,(void*)(&MCS2MPR_SDOObject_0x1018),
	0x2000,(void*)(&MCS2MPR_SDOObject_0x2000),
	0x2001,(void*)(&MCS2MPR_SDOObject_0x2001),
	0x2002,(void*)(&MCS2MPR_SDOObject_0x2002),
	0x2003,(void*)(&MCS2MPR_SDOObject_0x2003),
	0x2010,(void*)(&MCS2MPR_SDOObject_0x2010),
	0x2011,(void*)(&MCS2MPR_SDOObject_0x2011),
	0x2012,(void*)(&MCS2MPR_SDOObject_0x2012),
	0x2013,(void*)(&MCS2MPR_SDOObject_0x2013),
	0x2014,(void*)(&MCS2MPR_SDOObject_0x2014),
	0x2015,(void*)(&MCS2MPR_SDOObject_0x2015),
	0x2016,(void*)(&MCS2MPR_SDOObject_0x2016),
	0x2017,(void*)(&MCS2MPR_SDOObject_0x2017),
	0x2018,(void*)(&MCS2MPR_SDOObject_0x2018),
	0x2019,(void*)(&MCS2MPR_SDOObject_0x2019),
	0x201A,(void*)(&MCS2MPR_SDOObject_0x201A),
	0x201B,(void*)(&MCS2MPR_SDOObject_0x201B),
	0x201C,(void*)(&MCS2MPR_SDOObject_0x201C),
	0x201D,(void*)(&MCS2MPR_SDOObject_0x201D),
	0x201E,(void*)(&MCS2MPR_SDOObject_0x201E),
	0x201F,(void*)(&MCS2MPR_SDOObject_0x201F),
//	0x3028,(void*)(&MCS2MPR_SDOObject_0x3028),
};
