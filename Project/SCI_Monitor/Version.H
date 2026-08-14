/*************************************************************

文件名称:Version.c/.h
文件功能:对版本信息等字符串变量按照通信协议进行定义，并对版本信息打包上传。
版本说明:Checkin $Date: August 3, 2012   14:02:38 $,    Written by :Liu Qi.
修改记录:
1.
2.
3.

**************************************************************/
#ifndef _VERSION_H
#define _VERSION_H
#include "DSP2833x_Device.h"

#define STRING_NUM 52
#define RTM_VER_SIZE  10
#define TYPE_VER_SIZE 7
#define LIST_VER_SIZE  8

extern Uint16  Conv_Type_Ver[STRING_NUM] ;
extern Uint16  Conv_List_Ver[STRING_NUM] ;
extern Uint16  RTM_File_Ver[STRING_NUM] ;
extern Uint16  FPGA_Ver[STRING_NUM] ;

extern Uint16  CPUboard_Ver[STRING_NUM] ; 
extern Uint16  CPUboard_List[STRING_NUM] ;
extern Uint16  Soft_Ver[STRING_NUM] ;

extern Uint16  Conv_Mast_Type[STRING_NUM];
extern Uint16  MBAdapterVariable[STRING_NUM]; 

extern void Monitor_Software_Read(unsigned int *PTxBuf,unsigned int *PTxTail);
#endif

//===========================================================================
// No more.
//===========================================================================
