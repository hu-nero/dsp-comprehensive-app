#ifndef _CUSTOM_MODBUSM_H_
#define _CUSTOM_MODBUSM_H_

#include "Main\Variable.H"
#include "GPIO\Custom_IOMacro.H"

#define MBM_RTU_MIN_SIZE	5
#define MBM_RTU_MAX_SIZE	255
#define MBM_ERR_MAX_TIMES	5
#define MBM_REC_TIMEOUT		5

#define ENERGY_FRAME_ADDR   (u16)(0x0000)
#define ENERGY_FRAME_LEN    (u16)(0x003C - 0X0000 + 2)

#define POWER_FRAME_ADDR    (u16)(0x0061)
#define POWER_FRAME_LEN     (u16)(0x007A - 0X0061 + 1)

#define BALANCE_FRAME_ADDR  (u16)(0x0092)
#define BALANCE_FRAME_LEN   (u16)(0x0094 - 0X0092 + 1)

#define THD_FRAME_ADDR    	(u16)(0x05DD)
#define THD_FRAME_LEN     	(u16)(0x05E2 - 0X05DD + 1)

typedef union
{
	Uint16	all;
	struct
	{
		Uint16 SlaveAddr: 8;
		Uint16 FuncCode:  8;
	}	bit;
}	Modbus_CtrlWord;

typedef enum
{
	FuncCode01_RCoilReg  = (u8)0x01,
	FuncCode02_RDiscReg  = (u8)0x02,
	FuncCode03_RHoldReg  = (u8)0x03,
	FuncCode04_RInputReg = (u8)0x04,
	FuncCode05_WCoilReg  = (u8)0x05,
	FuncCode06_WHoldReg  = (u8)0x06,
	FuncCode0F_WCoilRegs = (u8)0x0F,
	FuncCode10_WHoldRegs = (u8)0x10,
}	ModbusFuncCode_t;

typedef enum
{
	MBM_STA_Idle      = (u8)0x00,
	MBM_STA_Sending   = (u8)0x01,
	MBM_STA_SendEnd   = (u8)0x02,
	MBM_STA_Recving   = (u8)0x03,
	MBM_STA_RecvCheck = (u8)0x04,
	MBM_STA_RecvFinish= (u8)0x05,
	MBM_STA_RecvError = (u8)0x06,
	MBM_STA_TimesErr  = (u8)0x07,
}	ModbusMasterStatus_t;

typedef struct
{
	Modbus_CtrlWord CtrlWord;
	u16 AccessAddr;
	u16 RegsLength;

	ModbusMasterStatus_t RunStatus;
	u8 ErrorCount;
	u8 SendCount;
	u8 TxBuf[32];
	u8 RecvCount;
	u8 RxBuf[256];
	u16 RecvTimeCount;
	u16 OverTimeCount;
}	ModbusMasterDevice_t;

typedef struct
{
	//ALL
	long ADD00H_Ept;
	long ADD02H_Ept1;
	long ADD04H_Ept2;
	long ADD06H_Ept3;
	long ADD08H_Ept4;
	//positive
	long ADD0AH_EptP;
	long ADD0CH_Ept1P;
	long ADD0EH_Ept2P;
	long ADD10H_Ept3P;
	long ADD12H_Ept4P;
	//negative
	long ADD14H_EptN;
	long ADD16H_Ept1N;
	long ADD18H_Ept2N;
	long ADD1AH_Ept3N;
	long ADD1CH_Ept4N;
	//ALL
	long ADD1EH_Eqt;
	long ADD20H_Eqt1;
	long ADD22H_Eqt2;
	long ADD24H_Eqt3;
	long ADD26H_Eqt4;
	//positive
	long ADD28H_EqtP;
	long ADD2AH_Eqt1P;
	long ADD2CH_Eqt2P;
	long ADD2EH_Eqt3P;
	long ADD30H_Eqt4P;
	//negative
	long ADD32H_EqtN;
	long ADD34H_Eqt1N;
	long ADD36H_Eqt2N;
	long ADD38H_Eqt3N;
	long ADD3AH_Eqt4N;
}	ENERGY_MSG_REGS;

typedef struct
{
	int ADD61H_Ua;
	int ADD62H_Ub;
	int ADD63H_Uc;

	int ADD64H_Ia;
	int ADD65H_Ib;
	int ADD66H_Ic;

	int ADD67H_Pa;
	int ADD68H_Pb;
	int ADD69H_Pc;
	int ADD6AH_Pt;

	int ADD6BH_Qa;
	int ADD6CH_Qb;
	int ADD6DH_Qc;
	int ADD6EH_Qt;

	int ADD6FH_Sa;
	int ADD70H_Sb;
	int ADD71H_Sc;
	int ADD72H_St;

	int ADD73H_PFa;
	int ADD74H_PFb;
	int ADD75H_PFc;
	int ADD76H_PFt;

	int ADD77H_F;

	int ADD78H_Uab;
	int ADD79H_Ubc;
	int ADD7AH_Uca;

}	POWER_MSG_REGS;

typedef struct
{
	int ADD92h_I0;
	int ADD93h_Ublk;
	int ADD94h_Iblk;
}	BALANCE_MSG_REGS;

typedef struct
{
	int ADD5DDh_THDUa;
	int ADD5DEh_THDUb;
	int ADD5DFh_THDUc;
	int ADD5E0h_THDIa;
	int ADD5E1h_THDIb;
	int ADD5E2h_THDIc;
}	THD_MSG_REGS;
//////////////////////////////////////////////////////////////////////////////
extern ModbusMasterDevice_t NodeEnergy;
extern POWER_MSG_REGS sPowerDataGroup;
extern ENERGY_MSG_REGS sEnergyDataGroup;
extern BALANCE_MSG_REGS sBalanceDataGroup;
extern THD_MSG_REGS sThdDataGroup;
//////////////////////////////////////////////////////////////////////////////
extern void Sci_ModbusM_init(volatile struct SCI_REGS *p, float LSPCLK_FREQ, float SCI_FREQ);
extern void Sci_ModbusM_SendArray(volatile struct SCI_REGS *p, u8 * p_Arr, Uint16 Len);
extern void Sci_ModbusM_RxData(volatile struct SCI_REGS *p, ModbusMasterDevice_t *pNode);
extern void MBM_Protocol(ModbusMasterDevice_t *pNode);
//////////////////////////////////////////////////////////////////////////////
extern u16 Modbus_CRC_Check(u8 *pCrcBuf, u16 LEN, u8 *CRCByteH, u8 *CRCByteL);
//////////////////////////////////////////////////////////////////////////////
extern void ModbusMaster_Init(ModbusMasterDevice_t *pNode);
extern void MBS_SendData(volatile struct SCI_REGS *pSCI, ModbusMasterDevice_t *pNode);
extern void ModbusM_StatusUpdate(ModbusMasterDevice_t *pNode);
//////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////

