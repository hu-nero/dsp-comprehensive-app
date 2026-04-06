#include "Flash_CMD_Macro.H"

extern void CMD06_Wr_en(int *SendBuf,int *RecvBuf);
extern void CMD_Wr_Status(int Index,int *SendBuf,int *RecvBuf);
extern void CMD_Rd_Status(int Index,int *SendBuf,int *RecvBuf);
extern void CMD66_ResetEn(int *SendBuf,int *RecvBuf);
extern void CMD99_Reset(int *SendBuf,int *RecvBuf);
extern void CMDB7_Enter_4ByteAddr(int *SendBuf,int *RecvBuf);
extern void CMD03_ReadData(long Addr,int *SendBuf,int *RecvBuf);
extern void CMD02_WriteData(long Addr,int *SendBuf,int *RecvBuf);
extern void CMDC7_ChipErase(int *SendBuf,int *RecvBuf);
extern void CMD9F_Read_JEDECID(int *SendBuf,int *RecvBuf);
extern void CMD4B_Read_UniqueID(int *SendBuf,int *RecvBuf);
extern void CMD20_SectorErase(long Addr,int *SendBuf,int *RecvBuf);



























union SDCard_R1_Reg_Type
{
	int DataInt;
	struct
	{
		unsigned In_Idle_State:1;
		unsigned Erase_Reset:1;
		unsigned Illegal_Command:1;
		unsigned Com_CRC_Error:1;
		unsigned Erase_Seq_Error:1;
		unsigned Address_Error:1;
		unsigned Parameter_Error:1;
		unsigned Const_Flag:1;
		unsigned Unused:8;
	}DataBit;
};
extern union SDCard_R1_Reg_Type SDCard_R1_Reg;
///////////////////////////////////////////////
union SDCard_R2_Reg_Type
{
	int DataInt;
	struct
	{
		unsigned Card_is_Locked:1;
		unsigned WP_Erase_Skip_or_LockUnlock_Cmd_Failed:1;
		unsigned Error:1;
		unsigned CC_Error:1;
		unsigned Card_ECC_Failed:1;
		unsigned WP_Violation:1;
		unsigned Erase_Param:1;
		unsigned Out_of_ange_or_CSD_Overwrite:1;
	}DataBit;
};
extern union SDCard_R2_Reg_Type SDCard_R2_Reg;		
////////////////////////////////////////////////
union SDCard_OCR_Reg_Type
{
	long DataLong;
	struct
	{
	  unsigned int OCR0;
	  unsigned int OCR1;
	}DataInt;
	struct
	{
		unsigned Unused:4;
		unsigned V1d6:1;	
		unsigned V1d7:1;		
		unsigned V1d8:1;		
		unsigned V1d9:1;	
		unsigned V2d0:1;		
		unsigned V2d1:1;	
		unsigned V2d2:1;	
		unsigned V2d3:1;		
		unsigned V2d4:1;		
		unsigned V2d5:1;	
		unsigned V2d6:1;		
		unsigned V2d7:1;		
		unsigned V2d8:1;	
		unsigned V2d9:1;		
		unsigned V3d0:1;		
		unsigned V3d1:1;	
		unsigned V3d2:1;		
		unsigned V3d3:1;	
		unsigned V3d4:1;	
		unsigned V3d5:1;	
		unsigned Unused2:6;		
		unsigned Ver:1;
		unsigned Busy:1;			
	}DataBit;
};
////////////////////////////////////////////////
union SDCard_CID_Reg_Type
{
	int DataInt[8];
	struct
	{
		unsigned Unused:1;
		unsigned CRC:7;
		unsigned MDT_Month:4;
		unsigned MDT_YearL:4;
		/////////////////
		unsigned MDT_YearH:4;	
		unsigned Unused2:4;
		unsigned PSNL:8;
		//////////////////
		unsigned PSNM:16;	
    //////////////////
		unsigned PSNH:8;	
		unsigned PRV:8;	
		//////////////////
		unsigned PNM5:8;
		unsigned PNM4:8;
		/////////////////
		unsigned PNM3:8;
		unsigned PNM2:8;
		/////////////////
		unsigned PNM1:8;
		unsigned OID2:8;
		//////////////////
		unsigned OID1:8;
		unsigned MID:8;
	}DataBit;
};		
////////////////////////////////////////////////
union SDCard_CSD_Reg_Type
{
  int DataInt[8];
	struct
	{
		unsigned Unused:1;
		unsigned CRC:7;
		unsigned Unused2:2;
		unsigned FILE_FORMAT:2;	
		unsigned TMP_WRITE_PROTECT:1;
		unsigned PERM_WRITE_PROTECT:1;
		unsigned COPY:1;
		unsigned FILE_FORMAT_GRP:1;
		//////////////////////////////////////
		unsigned Unused3:5;
		unsigned WRITE_BL_PARTIAL:1;
		unsigned WRITE_BL_LEN:4;
		unsigned R2W_FACTOR:3;
		unsigned Unused4:2;
		unsigned WP_GRP_ENABLE:1;
		//////////////////////////////////////
		unsigned WP_GRP_SIZE:7;
		unsigned SECTOR_SIZE:7;
		unsigned ERASE_BLK_EN:1;
		unsigned C_SIZE_MULTL1:1;
		/////////////////////////////////////
		unsigned C_SIZE_MULTH2:2;
		unsigned VDD_W_CURR_MAX:3;
		unsigned VDD_W_CURR_MIN:3;
		unsigned VDD_R_CURR_MAX:3;
		unsigned VDD_R_CURR_MIN:3;
		unsigned C_SIZEL2:2;	
		////////////////////////////////////	
		unsigned C_SIZEH10:10;
		unsigned Unused5:2;
		unsigned DSR_IMP:1;
		unsigned READ_BLK_MISALIGN:1;
		unsigned WRITE_BLK_MISALIGN:1;
		unsigned READ_BL_PARTIAL:1;
		/////////////////////////////////////
		unsigned READ_BL_LEN:4;
		unsigned CCC:12;
		////////////////////////////////////
		unsigned TRAN_SPEED:8;
		unsigned NSAC:8;
		////////////////////////////////////
		unsigned TAAC:8;
		unsigned Unused6:6;
		unsigned CSD_STRUCTURE:2;
	}DataBit10;
	struct
	{
		unsigned Unused:1;
		unsigned CRC:7;
		unsigned Unused2:2;
		unsigned FILE_FORMAT:2;	
		unsigned TMP_WRITE_PROTECT:1;
		unsigned PERM_WRITE_PROTECT:1;
		unsigned COPY:1;
		unsigned FILE_FORMAT_GRP:1;
		//////////////////////////////////////
		unsigned Unused3:5;
		unsigned WRITE_BL_PARTIAL:1;
		unsigned WRITE_BL_LEN:4;
		unsigned R2W_FACTOR:3;
		unsigned Unused4:2;
		unsigned WP_GRP_ENABLE:1;
		//////////////////////////////////////
		unsigned WP_GRP_SIZE:7;
		unsigned SECTOR_SIZE:7;
		unsigned ERASE_BLK_EN:1;
		unsigned Unused5:1;//C_SIZE_MULTL1:1;//ÓÐÇø±ð
		/////////////////////////////////////
		unsigned C_SIZEL16:16;//C_SIZE_MULTH2:2;
		//unsigned VDD_W_CURR_MAX:3;
		//unsigned VDD_W_CURR_MIN:3;
		//unsigned VDD_R_CURR_MAX:3;
		//unsigned VDD_R_CURR_MIN:3;
		//unsigned C_SIZEL2:2;	
		////////////////////////////////////	
		unsigned C_SIZEH6:6;//C_SIZEH10:10;
		unsigned Unused6:6;//Unused5:2;
		unsigned DSR_IMP:1;
		unsigned READ_BLK_MISALIGN:1;
		unsigned WRITE_BLK_MISALIGN:1;
		unsigned READ_BL_PARTIAL:1;
		/////////////////////////////////////
		unsigned READ_BL_LEN:4;
		unsigned CCC:12;
		////////////////////////////////////
		unsigned TRAN_SPEED:8;
		unsigned NSAC:8;
		////////////////////////////////////
		unsigned TAAC:8;
		unsigned Unused7:6;
		unsigned CSD_STRUCTURE:2;
	}DataBit20;
};
///////////////////////////////////////
union SDCard_Flag_Type
{
	int DataInt;
	struct
	{
		unsigned HCFlag:2;
		unsigned Unused:14;
	}DataBit;
};
extern union SDCard_Flag_Type SDCard_Flag;
//////////////////////////////////////
#define Data_Token_1R1WMR_StartBlock 0xFE
#define Data_Token_MW_StartBlock 0xFC
#define Data_Token_StopBlock 0xFD
/////////////////////////////////////////////////
extern int CMD0_Reset(int *SubStep);
extern int CMD1_Activate(int *SubStep,int *Time_1ms);
extern int CMD8_Activate(int *SubStep);
extern int CMD9_SendCSD(int *SubStep);
extern int CMD10_SendCID(int *SubStep);
extern int CMD12_STOPTrans(int *SubStep);
extern int CMD13_Send_Status(int *SubStep);
extern int CMD16_SET_BlockLen(int *SubStep,long BlockLen);
extern int CMD17_BlockRead(int *SubStep,long BlockAddr);
extern int CMD18_Read_MultiBlock_First(int *SubStep,long BlockAddr);
extern int CMD18_Read_MultiBlock_Continue(int *SubStep);
extern int CMD24_BlockWrite(int *SubStep,long BlockAddr);
extern int CMD25_Write_MultiBlock_First(int *SubStep,long BlockAddr);
extern int CMD25_Write_MultiBlock_Continue(int *SubStep);
extern int CMD25_Write_MultiBlock_End(int *SubStep);
extern int CMD58_ReadOCR(int *SubStep,int *Time_1ms);
///////////////////////////////////////////////////////////////
extern int ACMD23_Set_EraseNum(int *SubStep,int Num,int *ACMDFlag);
extern int ACMD41_Activite(int *SubStep,int *ACMDFlag);
//////////////////////////////////////////////////





