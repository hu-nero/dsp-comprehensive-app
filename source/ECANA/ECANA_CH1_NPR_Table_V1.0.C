#include "main\Variable.H"
#include "main\Comm_Variable.H"
#include "ECANA\Pub_ECAN_Macro.H"
/***************************************************************************************************/
//���������ݸ���û���Ҫ�޸�
unsigned int ECANA_CH2_NodeID=10;

unsigned int ECANA_MPR2NPR_NodeID=1;

unsigned int ECANA_Reserved=0;
unsigned int ECANA_Toggle=0;
//////////////////////////////////
//ʱ���������
unsigned int ECANA_PDOTimeInterval=20;
unsigned int ECANA_NMTTimeInterval=100;

///////////////////////////////////////
//PDO defination
const struct CANPDO MPR2NPR_TPDOObj[]=
{
	0x003, 0x0000, &ECANA_MPR2NPR_NodeID , (Uint16 *)(&ECANA_Reserved),(Uint16 *)(&ECANA_Reserved),(Uint16 *)(&ECANA_Reserved),(Uint16 *)(&ECANA_Reserved),
	TPDO1, 0x0003, &ECANA_PDOTimeInterval, (Uint16 *)(&CANATPDO[0][0]),(Uint16 *)(&CANATPDO[0][1]),(Uint16 *)(&CANATPDO[0][2]),(Uint16 *)(&CANATPDO[0][3]),
	TPDO2, 0x0008, &ECANA_PDOTimeInterval, (Uint16 *)(&CANATPDO[1][0]),(Uint16 *)(&CANATPDO[1][1]),(Uint16 *)(&CANATPDO[1][2]),(Uint16 *)(&CANATPDO[1][3]),
	TPDO3, 0x0008, &ECANA_PDOTimeInterval, (Uint16 *)(&CANATPDO[2][0]),(Uint16 *)(&CANATPDO[2][1]),(Uint16 *)(&CANATPDO[2][2]),(Uint16 *)(&CANATPDO[2][3]),
	NMTEC, 0x0001, &ECANA_NMTTimeInterval, (Uint16 *)(&ECANA_Toggle)  ,(Uint16 *)(&CANATPDO[3][1]),(Uint16 *)(&CANATPDO[3][2]),(Uint16 *)(&CANATPDO[3][3]),
};
const struct CANPDO MPR2NPR_RPDOObj[]=
{
	0x004, 0x0000, &ECANA_MPR2NPR_NodeID , (Uint16 *)(&ECANA_Reserved),(Uint16 *)(&ECANA_Reserved),(Uint16 *)(&ECANA_Reserved) ,(Uint16 *)(&ECANA_Reserved),
	RPDO1, 0x0005, &ECANA_PDOTimeInterval, (Uint16 *)(&CANARPDO[0][0]),(Uint16 *)(&CANARPDO[0][1]),(Uint16 *)(&CANARPDO[0][2]),(Uint16 *)(&CANARPDO[0][3]),
	RPDO2, 0x0008, &ECANA_PDOTimeInterval, (Uint16 *)(&CANARPDO[1][0]),(Uint16 *)(&CANARPDO[1][1]),(Uint16 *)(&CANARPDO[1][2]),(Uint16 *)(&CANARPDO[1][3]),
	RPDO3, 0x0008, &ECANA_PDOTimeInterval, (Uint16 *)(&CANARPDO[2][0]),(Uint16 *)(&CANARPDO[2][1]),(Uint16 *)(&CANARPDO[2][2]),(Uint16 *)(&CANARPDO[2][3]),
	RPDO4, 0x0008, &ECANA_PDOTimeInterval, (Uint16 *)(&CANARPDO[3][0]),(Uint16 *)(&CANARPDO[3][1]),(Uint16 *)(&CANARPDO[3][2]),(Uint16 *)(&CANARPDO[3][3]),
};

/////////////////////////////////////////
//SDO defination
const struct CANSDO MPR2NPR_SDOObject_0x4012[]=
{
	0x0000, 0x0002, 0x0000,(void*)(&ECANA_Reserved),
	0x0008, 0x0001, 0x0203,(void *)(&Time_H),
	0x0008, 0x0002, 0x0203,(void *)(&Time_L),
};
const struct CANSDO MPR2NPR_SDOObject_0x4013[]=
{
	0x0000, 0x0002, 0x0000,(void*)(&ECANA_Reserved),
	0x0008, 0x0001, 0x810B,(void*)(&mpr_temphigh),
	0x0008, 0x0002, 0x810B,(void*)(&mpr_templow),
};
/****************************************************************/

const struct SDOSummaryType MPR2NPR_CANSDOSummary[]=
{
	0x0002, (void*)(&ECANA_MPR2NPR_NodeID),
	0x4012, (void*)(&MPR2NPR_SDOObject_0x4012),
	0x4013, (void*)(&MPR2NPR_SDOObject_0x4013),
};
/****************************************************************/
