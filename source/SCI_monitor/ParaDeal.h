
/*************************************************************

文件名称:ParaDeal.c/.h
文件功能:实现了单帧参数的分类型处理功能。
版本说明:Checkin $Date: August 3, 2012   10:02:38 $,    Written by :Liu Qi.
修改记录:
1.
2.
3.

**************************************************************/
#ifndef _PARADEAL_H
#define _PARADEAL_H

#define PARA_DATA_BUF_SIZE        128 //word型，一帧数据允许最长256 byte

#include "DSP2833x_Device.h"
extern Uint16 Monitor_SingleFrame_Access(Uint16 uiID,Uint16 uiCtrlWord,Uint16 nLen,Uint16 *PData,Uint16 *PTDataBuf,Uint16 *PTDataTail);

extern const int ParaDeal_Ver[3];

#endif
//===========================================================================
// End of file.
//===========================================================================
