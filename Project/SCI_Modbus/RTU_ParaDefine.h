#ifndef _RTU_ParaDefine_H_
#define _RTU_ParaDefine_H_

#include "main\Variable.h"
/*********************************************************************/
// Coil register
// 0x00001 - 0x10000
// 0x01 0x05 0x0F
struct CoilRegType
{
	u16 RegAddrNum;
	u16 *Obj;
};
/*********************************************************************/
// Discrete input register
// 0x10001 - 0x20000
// 0x02
struct DiscreteRegType
{
	u16 RegAddrNum;
	u16 *Obj;
};
/*********************************************************************/
// Input register
// 0x20001 - 0x30000
// 0x03 0x06 0x10
struct InputRegType
{
	u16 RegAddrNum;
	u16 *Obj;
};
/*********************************************************************/
// Hold register
// 0x30001 - 0x40000
// 0x04
struct HoldRegType
{
	u16 RegAddrNum;
	u16 *RegObj;
};

/////////////////////////////////////////////////////////////////
extern const struct CoilRegType sCoilReg_Group[];
extern const struct DiscreteRegType sDiscReg_Group[];
extern const struct InputRegType sInputReg_Group[];
extern const struct HoldRegType sHoldReg_Group[];
/*****************************************************************************/
#endif

