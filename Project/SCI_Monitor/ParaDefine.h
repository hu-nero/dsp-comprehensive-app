
#ifndef _PARA_H_
#define _PARA_H_
/*
*********************************************************************************************************
*                                           Pre-defines
*********************************************************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif

#if      defined(PARA_FOR_GLOBAL)
#define  PARA_EXT
#else
#define  PARA_EXT  extern 
#endif
/*
*********************************************************************************************************
*                                           Include files
*********************************************************************************************************
*/
#include "DSP2833x_Device.h"
/*
*********************************************************************************************************
*                                           �����ֽ���
*********************************************************************************************************
*/
union Monitor_CtrlWord_Type
{
	int DataInt;
	struct
	{
		unsigned Rw:2;
		unsigned SourceAddr:3;
		unsigned ParaType:3;
		unsigned DataObject:4;
		unsigned DestAddr:3;
		unsigned Unused:1;
	}	DataBit;
};          
extern int BOARD_ADDR;
extern int Monitor_ADDR[];
/*
*********************************************************************************************************
*                                   RTM define
*********************************************************************************************************
*/
struct RTMType 
{
	unsigned int SubNum;
	union
	{
		int all;
		struct
		{
		  unsigned WrFlag:1;// 1:write enable 
		  unsigned LogSaveFlag:1;//1:need to write eventlog
		  unsigned MemoryFlag:3;//0:RAM;  1:EEPROM;  2:RTC 3:CAN�����ڵ�0��ʹ��
		  unsigned DataTyep:3; //000������Ͳ���;  001-int;  010-unsigned int;  011-long;  100-unsigned long;  101-float;  110-string
		  unsigned Unused2:4;
		  unsigned Destaddr:3;//100:���;   101:���
		  unsigned Unused3:1;
		}Bit;
	}Para;
	unsigned int *Obj;
};

struct RTMSummaryType
{
	int GroupNum;
	struct  RTMType *SubGroup;
}; 

extern const struct RTMSummaryType sParaData_Summary[];
/*****************************************************************************/
#endif

