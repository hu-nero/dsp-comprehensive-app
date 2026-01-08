#include "main\Variable.h"
#include "Main\Comm_Variable.H"
/***************************************************************************************************/
//void CANA_Sector_Send_Power(void)
//{
//	    if(Can_Temp_Flag == 0)
//		{
//			CANATPDO[0][0] = 0x01A0;
//			CANATPDO[0][1] = 0x0000;
//
//			Can_Temp_Flag++;
//		}
//
//		else if(Can_Temp_Flag == 1)
//		{
//			CANATPDO[0][0] = 0x01A0;
//			CANATPDO[0][1] = 0x0001;
//
//			Can_Temp_Flag++;
//		}
//
//		else if(Can_Temp_Flag == 2)
//		{
//			CANATPDO[0][0] = 0x01A0;
//			CANATPDO[0][1] = 0x0002;
//
//			Can_Temp_Flag++;
//		}
//
//		else if(Can_Temp_Flag == 3)
//		{
//			CANATPDO[0][0] = 0x01A0;
//			CANATPDO[0][1] = 0x0003;
//
//			Can_Temp_Flag = 0;
//		}
//}
///////////////////////////////////////////////////////////////////////////////////

//void CANA_Sector_Recv_Power(void)
//{
//	int itmp;
//	if(CANARPDO[0][0] == 0x01A0)
//	{
//		if((CANARPDO[0][1] % 256) ==0)
//		{
//			itmp = ((CANARPDO[0][1]/256)<<8) | (CANARPDO[0][2]%256);
//			if(itmp == -5000)
//			{
//				Zqwl_Temp_Error = 1;
//			}
//			Rectifier_reactor_temp = (float)itmp / 10;
//		}
//		else if((CANARPDO[0][1] % 256) ==1)
//		{
//			itmp = ((CANARPDO[0][1]/256)<<8) | (CANARPDO[0][2]%256);
//			if(itmp == -5000)
//			{
//				Zqwl_Temp_Error = 1;
//			}
//			Inverter_reactor_temp = (float)itmp / 10;
//		}
//		else if((CANARPDO[0][1] % 256) ==2)
//		{
//			itmp = ((CANARPDO[0][1]/256)<<8) | (CANARPDO[0][2]%256);
//			if(itmp == -5000)
//			{
//				Zqwl_Temp_Error = 1;
//			}
//			Transformer_temp = (float)itmp / 10;
//		}
//	}
//
//}
void CANA_NPR_Send_MPR(void)
{
#ifdef PMSG
	CANATPDO[0][0] = MainStusWord.DataInt;
	CANATPDO[0][1] = (int)(Uab1_Rmsf*8);
	CANATPDO[0][2] = (int)(Ia1_Rmsf*256);
	CANATPDO[0][3] = (int)(NPR_Power*256);

	CANATPDO[1][0] = (int)(NPR_ReactPower*8);
	CANATPDO[1][1] = NPR_FaultCode;
	CANATPDO[1][2] = MainAlarmWord.DataInt;
	CANATPDO[1][3] = (int)(npr_freq*64);

	CANATPDO[2][0] = Conv_MainCtrlWord;
	CANATPDO[2][1] = Conv_PowerKWRef;
	CANATPDO[2][2] = Conv_PowerKVarRef;
	CANATPDO[2][3] = 0;

#endif

#ifdef DFIG
	if(SendFlag3 == 0)
	{
		CanATPDO[2][0] = SendFlag3;
		CanATPDO[2][1] = Conv_MainCtrlWord.DataInt;
		CanATPDO[2][2] = Conv_TorqueRef;
		CanATPDO[2][3] = Conv_PowerKVarRef;
		SendFlag3 = 1;
	}
	else if(SendFlag3 == 1)
	{
		CanATPDO[2][0] = SendFlag3;
		CanATPDO[2][1] = KW_P;
		CanATPDO[2][2] = KW_N;
		CanATPDO[2][3] = IgqRefRef;
		SendFlag3 = 2;
	}
	else if(SendFlag3 == 2)
	{
		CanATPDO[2][0] = SendFlag3;
		CanATPDO[2][1] = KVar_P;
		CanATPDO[2][2] = KVar_N;
		CanATPDO[2][3] = 0;
		SendFlag3 = 0;
	}
#endif
}

void CANA_NPR_Recv_MPR(void)
{
#ifdef PMSG
	MainCtrlWord_CAN.DataInt = CANARPDO[0][0];
	KVARRef_CAN         = CANARPDO[0][1];
	SysTime_MicroSecond = CANARPDO[0][2];
	MPR_TempH           = CANARPDO[0][3];

#endif

#ifdef DFIG
	MainCtrlWord_CAN.DataInt = CANARPDO[0][0];
	KVARRef_CAN         = CANARPDO[0][1];
	SysTime_MicroSecond = CANARPDO[0][2];
	MPR_TempH           = CANARPDO[0][3];
#endif
}



