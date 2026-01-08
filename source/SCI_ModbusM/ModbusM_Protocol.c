#include "Main\Variable.H"
#include "stdio.h"
#include "string.h"
#include "Custom_ModbusM.h"

POWER_MSG_REGS sPowerDataGroup = {0};
ENERGY_MSG_REGS sEnergyDataGroup = {0};
BALANCE_MSG_REGS sBalanceDataGroup = {0};
THD_MSG_REGS sThdDataGroup = {0};

void MBM_EnergyDataProcess(u8 *pFrame, s16 *Data);

void Lit_4CharBuf_Long32(u8 *pBuf, l32 *lData)
{
	u32 ltmp =  ( ((u32)pBuf[0] << 24) & 0xFF000000 ) | \
				( ((u32)pBuf[1] << 16) & 0x00FF0000 ) | \
				( ((u32)pBuf[2] <<  8) & 0x0000FF00 ) | \
				( ((u32)pBuf[3] <<  0) & 0x000000FF );
	*lData = *(f32 *)&ltmp;
}

void Lit_2CharBuf_Int16(u8 *pBuf, s16 *iData)
{
	u16 itmp = 	( ((u16)pBuf[0] << 8) & 0xFF00 ) | \
				( ((u16)pBuf[1] << 0) & 0x00FF );
	*iData = *(s16 *)&itmp;
}


u16 Modbus_CRC_Check(u8 *pCrcBuf, u16 LEN, u8 *CRCByteH, u8 *CRCByteL)
{
	u16 CRCRetn;
	u16  i, j;

	CRCRetn = 0xFFFF;
	for(i=0; i<LEN; i++)
	{
		CRCRetn ^= ( (*(pCrcBuf + i)) & 0xFF);
		for(j=0; j<8; j++)
		{
			if(CRCRetn & 0x0001)
				CRCRetn = (CRCRetn >> 1) ^ 0xA001;
			else
				CRCRetn = (CRCRetn >> 1);
		}
	}

	*CRCByteH = (CRCRetn>>0) & 0xFF;
	*CRCByteL = (CRCRetn>>8) & 0xFF;

	return CRCRetn;
}

/**********************************************************************************/
void MBM_Protocol(ModbusMasterDevice_t *pNode)
{
	u8 crcH, crcL;
	if(pNode->RunStatus == MBM_STA_RecvCheck)
	{
		if( (pNode->RecvCount>=MBM_RTU_MIN_SIZE) && (Modbus_CRC_Check(pNode->RxBuf, pNode->RecvCount, &crcH, &crcL)==0))
		{
			if((pNode->TxBuf[0]==pNode->RxBuf[0]) && (pNode->TxBuf[1]==pNode->RxBuf[1]))
			{
				pNode->RunStatus = MBM_STA_RecvFinish;
			}
			else
			{
				pNode->RunStatus = MBM_STA_RecvError;
			}
		}
		else
		{
			pNode->RunStatus = MBM_STA_RecvError;
		}
	}
	else if(pNode->RunStatus == MBM_STA_RecvFinish)
	{
		switch(pNode->RxBuf[1])
		{
			case FuncCode01_RCoilReg:{};break;
			case FuncCode02_RDiscReg:{};break;
			case FuncCode03_RHoldReg:
			{
				if(pNode->AccessAddr == ENERGY_FRAME_ADDR)
				{
					if(pNode->RxBuf[2] == ENERGY_FRAME_LEN*2)
					{
						Lit_4CharBuf_Long32(&(pNode->RxBuf[3]),  &sEnergyDataGroup.ADD00H_Ept);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[7]),  &sEnergyDataGroup.ADD02H_Ept1);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[11]), &sEnergyDataGroup.ADD04H_Ept2);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[15]), &sEnergyDataGroup.ADD06H_Ept3);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[19]), &sEnergyDataGroup.ADD08H_Ept4);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[23]), &sEnergyDataGroup.ADD0AH_EptP);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[27]), &sEnergyDataGroup.ADD0CH_Ept1P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[31]), &sEnergyDataGroup.ADD0EH_Ept2P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[35]), &sEnergyDataGroup.ADD10H_Ept3P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[39]), &sEnergyDataGroup.ADD12H_Ept4P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[43]), &sEnergyDataGroup.ADD14H_EptN);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[47]), &sEnergyDataGroup.ADD16H_Ept1N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[51]), &sEnergyDataGroup.ADD18H_Ept2N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[55]), &sEnergyDataGroup.ADD1AH_Ept3N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[59]), &sEnergyDataGroup.ADD1CH_Ept4N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[63]), &sEnergyDataGroup.ADD1EH_Eqt);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[67]), &sEnergyDataGroup.ADD20H_Eqt1);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[71]), &sEnergyDataGroup.ADD22H_Eqt2);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[75]), &sEnergyDataGroup.ADD24H_Eqt3);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[79]), &sEnergyDataGroup.ADD26H_Eqt4);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[83]), &sEnergyDataGroup.ADD28H_EqtP);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[87]), &sEnergyDataGroup.ADD2AH_Eqt1P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[91]), &sEnergyDataGroup.ADD2CH_Eqt2P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[95]), &sEnergyDataGroup.ADD2EH_Eqt3P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[99]), &sEnergyDataGroup.ADD30H_Eqt4P);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[103]), &sEnergyDataGroup.ADD32H_EqtN);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[107]), &sEnergyDataGroup.ADD34H_Eqt1N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[111]), &sEnergyDataGroup.ADD36H_Eqt2N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[115]), &sEnergyDataGroup.ADD38H_Eqt3N);
						Lit_4CharBuf_Long32(&(pNode->RxBuf[119]), &sEnergyDataGroup.ADD3AH_Eqt4N);
					}
				}
				else if(pNode->AccessAddr == POWER_FRAME_ADDR)
				{
					if(pNode->RxBuf[2] == POWER_FRAME_LEN*2)
					{
//						MBM_EnergyDataProcess(pNode->RxBuf, &sPowerDataGroup.ADD61H_Ua);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[3]), &sPowerDataGroup.ADD61H_Ua);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[5]), &sPowerDataGroup.ADD62H_Ub);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[7]), &sPowerDataGroup.ADD63H_Uc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[9]), &sPowerDataGroup.ADD64H_Ia);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[11]), &sPowerDataGroup.ADD65H_Ib);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[13]), &sPowerDataGroup.ADD66H_Ic);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[15]), &sPowerDataGroup.ADD67H_Pa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[17]), &sPowerDataGroup.ADD68H_Pb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[19]), &sPowerDataGroup.ADD69H_Pc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[21]), &sPowerDataGroup.ADD6AH_Pt);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[23]), &sPowerDataGroup.ADD6BH_Qa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[25]), &sPowerDataGroup.ADD6CH_Qb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[27]), &sPowerDataGroup.ADD6DH_Qc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[29]), &sPowerDataGroup.ADD6EH_Qt);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[31]), &sPowerDataGroup.ADD6FH_Sa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[33]), &sPowerDataGroup.ADD70H_Sb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[35]), &sPowerDataGroup.ADD71H_Sc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[37]), &sPowerDataGroup.ADD72H_St);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[39]), &sPowerDataGroup.ADD73H_PFa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[41]), &sPowerDataGroup.ADD74H_PFb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[43]), &sPowerDataGroup.ADD75H_PFc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[45]), &sPowerDataGroup.ADD76H_PFt);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[47]), &sPowerDataGroup.ADD77H_F);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[49]), &sPowerDataGroup.ADD78H_Uab);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[51]), &sPowerDataGroup.ADD79H_Ubc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[53]), &sPowerDataGroup.ADD7AH_Uca);
					}
				}
				else if(pNode->AccessAddr == BALANCE_FRAME_ADDR)
				{
					if(pNode->RxBuf[2] == BALANCE_FRAME_LEN*2);
					{
						Lit_2CharBuf_Int16(&(pNode->RxBuf[3]), &sBalanceDataGroup.ADD92h_I0);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[5]), &sBalanceDataGroup.ADD93h_Ublk);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[7]), &sBalanceDataGroup.ADD94h_Iblk);
					}
				}
				else if(pNode->AccessAddr == THD_FRAME_ADDR)
				{
					if(pNode->RxBuf[2] == THD_FRAME_LEN*2);
					{
						Lit_2CharBuf_Int16(&(pNode->RxBuf[3]), &sThdDataGroup.ADD5DDh_THDUa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[5]), &sThdDataGroup.ADD5DEh_THDUb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[7]), &sThdDataGroup.ADD5DFh_THDUc);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[9]), &sThdDataGroup.ADD5E0h_THDIa);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[11]), &sThdDataGroup.ADD5E1h_THDIb);
						Lit_2CharBuf_Int16(&(pNode->RxBuf[13]), &sThdDataGroup.ADD5E2h_THDIc);
					}
				}
			}	break;
			case FuncCode04_RInputReg:{};break;
			case FuncCode05_WCoilReg: {};break;
			case FuncCode06_WHoldReg: {};break;
			case FuncCode0F_WCoilRegs:{};break;
			case FuncCode10_WHoldRegs:{};break;
		}
		pNode->RunStatus = MBM_STA_Idle;
	}
}

void MBM_EnergyDataProcess(u8 *pFrame, s16 *Data)
{
	u16 i;
	for(i=0; i<POWER_FRAME_LEN; i++)
	{
		Lit_2CharBuf_Int16(&pFrame[3+i], &(*(Data+i)) );
	}
}

/**********************************************************************************/
void ModbusM_StatusUpdate(ModbusMasterDevice_t *pNode)
{
	if(pNode->RecvTimeCount > 0)
	{
		pNode->RecvTimeCount ++;
		if(pNode->RecvTimeCount >= MBM_REC_TIMEOUT)
		{
			pNode->RecvTimeCount = 0;
			if(pNode->RunStatus == MBM_STA_Recving)
			{
				pNode->RunStatus = MBM_STA_RecvCheck;
			}
		}
	}

	if(pNode->RunStatus == MBM_STA_SendEnd)
	{
		pNode->OverTimeCount ++;
		if(pNode->OverTimeCount > 50)
		{
			pNode->OverTimeCount = 0;

			pNode->RunStatus = MBM_STA_TimesErr;
		}
	}
	else
	{
		pNode->OverTimeCount = 0;
	}

	if( (pNode->RunStatus == MBM_STA_TimesErr) || (pNode->RunStatus == MBM_STA_RecvError) )
	{
		pNode->ErrorCount ++;
		if(pNode->ErrorCount > 10000)
		{
			pNode->ErrorCount = 0;
			pNode->RunStatus = MBM_STA_Idle;
		}
	}
}

void ModbusMaster_Init(ModbusMasterDevice_t *pNode)
{
	memset(pNode, 0, sizeof(*pNode));
	memset(&sPowerDataGroup, 0, sizeof(sPowerDataGroup));
	memset(&sEnergyDataGroup, 0, sizeof(sEnergyDataGroup));
	memset(&sBalanceDataGroup, 0, sizeof(sBalanceDataGroup));
	memset(&sThdDataGroup, 0, sizeof(sThdDataGroup));

	pNode->SendCount = 0;
	pNode->RecvCount = 0;
	pNode->ErrorCount = 0;
	pNode->RecvTimeCount = 0;
	pNode->RunStatus = MBM_STA_Idle;
	pNode->CtrlWord.bit.SlaveAddr = 0x01;
	pNode->CtrlWord.bit.FuncCode  = FuncCode03_RHoldReg;
	pNode->AccessAddr = ENERGY_FRAME_ADDR;
}
/**********************************************************************************/
void MBS_SendData(volatile struct SCI_REGS *pSCI, ModbusMasterDevice_t *pNode)
{
	if(pNode->RunStatus == MBM_STA_Idle)
	{
		if(NodeEnergy.AccessAddr == ENERGY_FRAME_ADDR)
		{
			NodeEnergy.AccessAddr = POWER_FRAME_ADDR;
			NodeEnergy.RegsLength = POWER_FRAME_LEN;
		}
		else if(NodeEnergy.AccessAddr == POWER_FRAME_ADDR)
		{
			NodeEnergy.AccessAddr = BALANCE_FRAME_ADDR;
			NodeEnergy.RegsLength = BALANCE_FRAME_LEN;
		}
		else if(NodeEnergy.AccessAddr == BALANCE_FRAME_ADDR)
		{
			NodeEnergy.AccessAddr = THD_FRAME_ADDR;
			NodeEnergy.RegsLength = THD_FRAME_LEN;
		}
		else if(NodeEnergy.AccessAddr == THD_FRAME_ADDR)
		{
			NodeEnergy.AccessAddr = ENERGY_FRAME_ADDR;
			NodeEnergy.RegsLength = ENERGY_FRAME_LEN;
		}

		pNode->TxBuf[0] = pNode->CtrlWord.bit.SlaveAddr;
		pNode->TxBuf[1] = pNode->CtrlWord.bit.FuncCode;
		pNode->TxBuf[2] = (pNode->AccessAddr >> 8);
		pNode->TxBuf[3] = (pNode->AccessAddr >> 0);
		pNode->TxBuf[4] = (pNode->RegsLength >> 8);
		pNode->TxBuf[5] = (pNode->RegsLength >> 0);
		Modbus_CRC_Check(pNode->TxBuf, 6, &pNode->TxBuf[6], &pNode->TxBuf[7]);
		Sci_ModbusM_SendArray(pSCI, pNode->TxBuf, 8);
		pNode->RunStatus = MBM_STA_SendEnd;
		pNode->RecvCount = 0;
		pNode->RecvTimeCount = 0;
	}
}


