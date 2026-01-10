#include "main\Variable.H"
#include "main\Comm_Variable.H"
#include "ECANA\Pub_ECAN_Macro.H"
/***************************************************************************************************/
int SendFlag1=0, SendFlag2=0, SendFlag3=0, SendFlag4=0;
///////////////////////////////////////////////////////////////////////////////////
void CANB_NPR_Send_MCS(void)
{
#ifdef PMSG
//+-------+------------+-----------+----------+----------+
//| TPDO1 |    16bit   |   16bit   |  16bit   |  16bit   |
//+-------+------------+-----------+----------+----------+
//|   0   | Sendflag1  |StatusWord | npr_id   | UgCtrl_d |
//+-------+------------+-----------+----------+----------+
//|   1   | Sendflag1  | FaultCode | npr_iq   | UgCtrl_q |
//+-------+------------+-----------+----------+----------+
//|   2   | Sendflag1  | temphigh  | npr_freq | npr_udc  |
//+-------+------------+-----------+----------+----------+
	if(SendFlag1 == 0)
	{
		CANBTPDO[0][0] = SendFlag1;
		CANBTPDO[0][1] = MainStusWord.DataInt;
		CANBTPDO[0][2] = (int)(npr_id *256);
		CANBTPDO[0][3] = (int)(UgCtrl_d *8);
		SendFlag1 = 1;
	}
	else if(SendFlag1 == 1)
	{
		CANBTPDO[0][0] = SendFlag1;
		CANBTPDO[0][1] = ( (CAN_MPRFaultCode<<8)&0xFF00) | (NPR_FaultCode & 0x00FF) ;
		CANBTPDO[0][2] = (int)(npr_iq *256);
		CANBTPDO[0][3] = (int)(UgCtrl_q *256);
		SendFlag1 = 2;
	}
	else if(SendFlag1 == 2)
	{
		CANBTPDO[0][0] = SendFlag1;
		CANBTPDO[0][1] = (int)(npr_temphigh *10);
		CANBTPDO[0][2] = (int)(npr_freq *64);
		CANBTPDO[0][3] = (int)(npr_udc *8);
		SendFlag1 = 0;
	}
//+-------+------------+-----------+-----------+----------+
//| TPDO2 |    16bit   |   16bit   |  16bit    |  16bit   |
//+-------+------------+-----------+-----------+----------+
//|   0   | Sendflag2  | Ia1_Rmsf  | Uab1_Rmsf | NPR_Power |
//+-------+------------+-----------+-----------+----------+
//|   1   | Sendflag2  | Ib1_Rmsf  | Ubc1_Rmsf | NPR_ReactPower |
//+-------+------------+-----------+-----------+----------+
//|   2   | Sendflag2  | Ic1_Rmsf  | Uca1_Rmsf | RtlabStatus |
//+-------+------------+-----------+-----------+----------+
	if(SendFlag2 == 0)
	{
		CANBTPDO[1][0] = SendFlag2;
		CANBTPDO[1][1] = (int)(Ia1_Rmsf *256);
		CANBTPDO[1][2] = (int)(Uab1_Rmsf *8);
		CANBTPDO[1][3] = (int)(NPR_Power *256);
		SendFlag2 = 1;
	}
	else if(SendFlag2 == 1)
	{
		CANBTPDO[1][0] = SendFlag2;
		CANBTPDO[1][1] = (int)(Ib1_Rmsf *256);
		CANBTPDO[1][2] = (int)(Ubc1_Rmsf *8);
		CANBTPDO[1][3] = (int)(NPR_ReactPower *256);
		SendFlag2 = 2;
	}
	else if(SendFlag2 == 2)
	{
		CANBTPDO[1][0] = SendFlag2;
		CANBTPDO[1][1] = (int)(Ic1_Rmsf *256);
		CANBTPDO[1][2] = (int)(Uca1_Rmsf *8);
		CANBTPDO[1][3] = 0;
		SendFlag2 = 0;
	}
//+-------+------------+---------+---------+----------+
//| TPDO3 |    16bit   | 16bit   |  16bit  |  16bit   |
//+-------+------------+---------+---------+----------+
//|   0   | Sendflag3  | IsaRms  | USabRms | SpeedRPM |
//+-------+------------+---------+---------+----------+
//|   1   | Sendflag3  | IsbRms  | USbcRms | MTemp    |
//+-------+------------+---------+---------+----------+
//|   2   | Sendflag3  | IscRms  | UScaRms | MPRStatusWord |
//+-------+------------+---------+---------+----------+
	if(SendFlag3 == 0)
	{
		CANBTPDO[2][0] = SendFlag3;
		CANBTPDO[2][1] = CAN_IsaRms;
		CANBTPDO[2][2] = CAN_UsabRms;
		CANBTPDO[2][3] = CAN_SpeedRPM;
		SendFlag3 = 1;
	}
	else if(SendFlag3 == 1)
	{
		CANBTPDO[2][0] = SendFlag3;
		CANBTPDO[2][1] = CAN_IsbRms;
		CANBTPDO[2][2] = CAN_UsbcRms;
		CANBTPDO[2][3] = CAN_MTemp;
		SendFlag3 = 2;
	}
	else if(SendFlag3 == 2)
	{
		CANBTPDO[2][0] = SendFlag3;
		CANBTPDO[2][1] = CAN_IscRms;
		CANBTPDO[2][2] = CAN_UscaRms;
		CANBTPDO[2][3] = CAN_MPRStatusWord;
		SendFlag3 = 0;
	}
//+-------+------------+---------+---------+----------+
//| TPDO4 |    16bit   | 16bit   |  16bit  |  16bit   |
//+-------+------------+---------+---------+----------+
//|   0   | Sendflag4  | CAN_Isd | CAN_Isq | Reserved |
//+-------+------------+---------+---------+----------+
//|   1   | Sendflag4  | CAN_Ird | CAN_Irq | Reserved |
//+-------+------------+---------+---------+----------+
//|   2   | Sendflag4  | CAN_Usd | CAN_Usq | Reserved |
//+-------+------------+---------+---------+----------+

	if(SendFlag4 == 0)
	{
		CANBTPDO[3][0] = SendFlag4;
		CANBTPDO[3][1] = CAN_Isd;
		CANBTPDO[3][2] = CAN_Isq;
		CANBTPDO[3][3] = KVar_P;
		SendFlag4 = 1;
	}
	else if(SendFlag4 == 1)
	{
		CANBTPDO[3][0] = SendFlag4;
		CANBTPDO[3][1] = CAN_TorqueACK;
		CANBTPDO[3][2] = 0;
		CANBTPDO[3][3] = 0;
		SendFlag4 = 2;
	}
	else if(SendFlag4 == 2)
	{
		CANBTPDO[3][0] = SendFlag4;
		CANBTPDO[3][1] = CAN_Usd;
		CANBTPDO[3][2] = CAN_Usq;
		CANBTPDO[3][3] = 0;
		SendFlag4 = 0;
	}
#endif

#ifdef DFIG
//+-------+------------+-----------+----------+----------+
//| TPDO1 |    16bit   |   16bit   |  16bit   |  16bit   |
//+-------+------------+-----------+----------+----------+
//|   0   | Sendflag1  |StatusWord |  npr_id  | UgCtrl_d |
//+-------+------------+-----------+----------+----------+
//|   1   | Sendflag1  | FaultCode | npr_iq   | UgCtrl_q |
//+-------+------------+-----------+----------+----------+
//|   2   | Sendflag1  | temphigh  | npr_freq | npr_udc  |
//+-------+------------+-----------+----------+----------+
	if(SendFlag1 == 0)
	{
		CanOpen1TPDO[0][0] = SendFlag1;
		CanOpen1TPDO[0][1] = MainStusWord.DataInt;
		CanOpen1TPDO[0][2] = npr_id;
		CanOpen1TPDO[0][3] = UgCtrl_d;
		SendFlag1 = 1;
	}
	else if(SendFlag1 == 1)
	{
		CanOpen1TPDO[0][0] = SendFlag1;
		CanOpen1TPDO[0][1] = ( (CAN_MPRFaultCode<<8)&0xFF00) | (NPR_FaultCode & 0x00FF) ;
		CanOpen1TPDO[0][2] = npr_iq;
		CanOpen1TPDO[0][3] = UgCtrl_q;
		SendFlag1 = 2;
	}
	else if(SendFlag1 == 2)
	{
		CanOpen1TPDO[0][0] = SendFlag1;
		CanOpen1TPDO[0][1] = npr_temphigh;
		CanOpen1TPDO[0][2] = npr_freq;
		CanOpen1TPDO[0][3] = npr_udc;
		SendFlag1 = 0;
	}
//+-------+------------+-----------+-----------+----------+
//| TPDO2 |    16bit   |   16bit   |  16bit    |  16bit   |
//+-------+------------+-----------+-----------+----------+
//|   0   | Sendflag2  | Ia1_Rmsf  | Uab1_Rmsf | NPR_Power |
//+-------+------------+-----------+-----------+----------+
//|   1   | Sendflag2  | Ib1_Rmsf  | Ubc1_Rmsf | NPR_ReactPower |
//+-------+------------+-----------+-----------+----------+
//|   2   | Sendflag2  | Ic1_Rmsf  | Uca1_Rmsf | Reserved  |
//+-------+------------+-----------+-----------+----------+
	if(SendFlag2 == 0)
	{
		CanOpen1TPDO[1][0] = SendFlag2;
		CanOpen1TPDO[1][1] = Ia1_Rmsf;
		CanOpen1TPDO[1][2] = Uab1_Rmsf;

		CAN_All_P = CAN_MPR_Ps - NPR_Power;
		CanOpen1TPDO[1][3] = CAN_All_P;
		SendFlag2 = 1;
	}
	else if(SendFlag2 == 1)
	{
		CanOpen1TPDO[1][0] = SendFlag2;
		CanOpen1TPDO[1][1] = Ib1_Rmsf;
		CanOpen1TPDO[1][2] = Ubc1_Rmsf;

		CAN_All_Q = CAN_MPR_Qs - NPR_ReactPower;
		CanOpen1TPDO[1][3] = CAN_All_Q;
		SendFlag2 = 2;
	}
	else if(SendFlag2 == 2)
	{
		CanOpen1TPDO[1][0] = SendFlag2;
		CanOpen1TPDO[1][1] = Ic1_Rmsf;
		CanOpen1TPDO[1][2] = Uca1_Rmsf;

		KW_P = CAN_MPR_Ps - NPR_KW_P; //�����й�����
		CanOpen1TPDO[1][3] = KW_P;
		SendFlag2 = 0;
	}
//+-------+------------+---------+---------+----------+
//| TPDO3 |    16bit   | 16bit   |  16bit  |  16bit   |
//+-------+------------+---------+---------+----------+
//|   0   | Sendflag3  | IsaRms  | USabRms | SpeedRPM |
//+-------+------------+---------+---------+----------+
//|   1   | Sendflag3  | IsbRms  | USbcRms | MTemp    |
//+-------+------------+---------+---------+----------+
//|   2   | Sendflag3  | IscRms  | UScaRms | MPRStatusWord |
//+-------+------------+---------+---------+----------+
	if(SendFlag3 == 0)
	{
		CanOpen1TPDO[2][0] = SendFlag3;
		CanOpen1TPDO[2][1] = CAN_IsaRms;
		CanOpen1TPDO[2][2] = CAN_UsabRms;
		CanOpen1TPDO[2][3] = CAN_SpeedRPM;
		SendFlag3 = 1;
	}
	else if(SendFlag3 == 1)
	{
		CanOpen1TPDO[2][0] = SendFlag3;
		CanOpen1TPDO[2][1] = CAN_IsbRms;
		CanOpen1TPDO[2][2] = CAN_UsbcRms;
		CanOpen1TPDO[2][3] = CAN_MTemp;
		SendFlag3 = 2;
	}
	else if(SendFlag3 == 2)
	{
		CanOpen1TPDO[2][0] = SendFlag3;
		CanOpen1TPDO[2][1] = CAN_IscRms;
		CanOpen1TPDO[2][2] = CAN_UscaRms;
		CanOpen1TPDO[2][3] = CAN_MPRStatusWord;
		SendFlag3 = 0;
	}
//+-------+------------+---------+---------+----------+
//| TPDO4 |    16bit   | 16bit   |  16bit  |  16bit   |
//+-------+------------+---------+---------+----------+
//|   0   | Sendflag4  | CAN_Isd | CAN_Isq | Reserved |
//+-------+------------+---------+---------+----------+
//|   1   | Sendflag4  | CAN_Ird | CAN_Irq | Reserved |
//+-------+------------+---------+---------+----------+
//|   2   | Sendflag4  | CAN_Usd | CAN_Usq | Reserved |
//+-------+------------+---------+---------+----------+

	if(SendFlag4 == 0)
	{
		CanOpen1TPDO[3][0] = SendFlag4;
		CanOpen1TPDO[3][1] = CAN_Isd;
		CanOpen1TPDO[3][2] = CAN_Isq;
		CanOpen1TPDO[3][3] = CAN_MPRTorqACK;
		SendFlag4 = 1;
	}
	else if(SendFlag4 == 1)
	{
		CanOpen1TPDO[3][0] = SendFlag4;
		CanOpen1TPDO[3][1] = CAN_Ird;
		CanOpen1TPDO[3][2] = CAN_Irq;

		////////////////////////////////////////////////
		CAN_RtlabStatus.DataBit.NPRACK = RTLABPWMStatus;
		if(Conv_MainCtrlWord.DataBit.Mode1 == 1)
		{
			CAN_RtlabStatus.DataBit.Mode1 = 1;
			CAN_RtlabStatus.DataBit.Mode2 = 0;
			CAN_RtlabStatus.DataBit.Mode3 = 0;
		}
		if(Conv_MainCtrlWord.DataBit.Mode2 == 1)
		{
			//�й�����
			CAN_RtlabStatus.DataBit.Mode1 = 0;
			CAN_RtlabStatus.DataBit.Mode2 = 1;
			CAN_RtlabStatus.DataBit.Mode3 = 0;

			Id_Priority = 1;
		}
		if(Conv_MainCtrlWord.DataBit.Mode3 == 1)
		{
			//�޹�����
			CAN_RtlabStatus.DataBit.Mode1 = 0;
			CAN_RtlabStatus.DataBit.Mode2 = 0;
			CAN_RtlabStatus.DataBit.Mode3 = 1;

			Id_Priority = 0;
		}
		CanOpen1TPDO[3][3] = CAN_RtlabStatus.DataInt;
		////////////////////////////////////////////////
		SendFlag4 = 2;
	}
	else if(SendFlag4 == 2)
	{
		CanOpen1TPDO[3][0] = SendFlag4;
		CanOpen1TPDO[3][1] = CAN_Usd;
		CanOpen1TPDO[3][2] = CAN_Usq;

		KW_N = NPR_KW_N;
		CanOpen1TPDO[3][3] = KW_N;  //�����й�����
		SendFlag4 = 3;
	}
	else if(SendFlag4 == 3)
	{
		CanOpen1TPDO[3][0] = SendFlag4;

		KVar_P = CAN_MPR_Qs - NPR_KVar_P; //�����޹�����
		CanOpen1TPDO[3][1] = KVar_P;

		KVar_N = NPR_KVar_N;
		CanOpen1TPDO[3][2] = KVar_N;	//�����޹�����

		SendFlag4 = 0;
	}
#endif
}

void CANB_NPR_Recv_MCS(void)
{
#ifdef PMSG
	Conv_MainCtrlWord = CANBRPDO[0][0];
	Conv_PowerKWRef   = CANBRPDO[0][1] / 256;
	Conv_PowerKVarRef = CANBRPDO[0][2] / 256;
#else
//	Conv_MainCtrlWord = CANBRPDO[0][0];
//	Conv_PowerKWRef   = CANBRPDO[0][1] / 256;
//	Conv_PowerKVarRef = CANBRPDO[0][2] / 256;
#endif
}



void CANB_Sector_Send_Power(void)
{
	    if(Can_Temp_Flag == 0)
		{
			CANBTPDO[0][0] = 0x01A0;
			CANBTPDO[0][1] = 0x0000;

			Can_Temp_Flag++;
		}

		else if(Can_Temp_Flag == 1)
		{
			CANBTPDO[0][0] = 0x01A0;
			CANBTPDO[0][1] = 0x0001;

			Can_Temp_Flag++;
		}

		else if(Can_Temp_Flag == 2)
		{
			CANBTPDO[0][0] = 0x01A0;
			CANBTPDO[0][1] = 0x0002;

			Can_Temp_Flag++;
		}

		else if(Can_Temp_Flag == 3)
		{
			CANBTPDO[0][0] = 0x01A0;
			CANBTPDO[0][1] = 0x0003;

			Can_Temp_Flag = 0;
		}
}




void CANB_Sector_Recv_Power(void)
{
	int itmp;
	if(CANBRPDO[0][0] == 0x01A0)
	{
		if((CANBRPDO[0][1] % 256) ==0)
		{
			itmp = ((CANBRPDO[0][1]/256)<<8) | (CANBRPDO[0][2]%256);
			if(itmp == -5000)
			{
				Zqwl_Temp_Error = 1;
			}
			Rectifier_reactor_temp = (float)itmp / 10;
		}
		else if((CANBRPDO[0][1] % 256) ==1)
		{
			itmp = ((CANBRPDO[0][1]/256)<<8) | (CANBRPDO[0][2]%256);
			if(itmp == -5000)
			{
				Zqwl_Temp_Error = 1;
			}
			Inverter_reactor_temp = (float)itmp / 10;
		}
		else if((CANBRPDO[0][1] % 256) ==2)
		{
			itmp = ((CANBRPDO[0][1]/256)<<8) | (CANBRPDO[0][2]%256);
			if(itmp == -5000)
			{
				Zqwl_Temp_Error = 1;
			}
			Transformer_temp = (float)itmp / 10;
		}
	}

}
