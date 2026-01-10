#include "Main\Variable.H"
#include "RTU_ParaDefine.h"
/**************************************************************/
Uint16 RTU_Reserved=0;
Uint16 RTUGroup[4] = {0,0,0,0};
int InputRegBuf[32]={0};
int HoldRegBuf[50]={0};
/**************************************************************/
const struct CoilRegType sCoilReg_Group[]=
{
/* Total-  4 */ 0x0000, (void*)&RTUGroup[0],
};
/*********************************************************************************/
const struct DiscreteRegType sDiscReg_Group[]=
{
/* Total-  0 */ 0x0000, (void*)&RTUGroup[1],
};
/*********************************************************************************/
const struct InputRegType sInputReg_Group[]=
{
/* Total- 12 */     32, (void*)&RTUGroup[2],
/* Line -  1 */ 0x0000, (void*)&InputRegBuf[0],
/* Line -  2 */ 0x0001, (void*)&InputRegBuf[1],
/* Line -  3 */ 0x0002, (void*)&InputRegBuf[2],
/* Line -  4 */ 0x0003, (void*)&InputRegBuf[3],
/* Line -  5 */ 0x0004, (void*)&InputRegBuf[4],
/* Line -  6 */ 0x0005, (void*)&InputRegBuf[5],
/* Line -  7 */ 0x0006, (void*)&InputRegBuf[6],
/* Line -  8 */ 0x0007, (void*)&InputRegBuf[7],
/* Line -  9 */ 0x0008, (void*)&InputRegBuf[8],
/* Line - 10 */ 0x0009, (void*)&InputRegBuf[9],
/* Line - 11 */ 0x000A, (void*)&InputRegBuf[10],
/* Line - 12 */ 0x000B, (void*)&InputRegBuf[11],
/* Line - 13 */ 0x000C, (void*)&InputRegBuf[12],
/* Line - 14 */ 0x000D, (void*)&InputRegBuf[13],
/* Line - 15 */ 0x000E, (void*)&InputRegBuf[14],
/* Line - 16 */ 0x000F, (void*)&InputRegBuf[15],
/* Line - 17 */ 0x0010, (void*)&InputRegBuf[16],
/* Line - 18 */ 0x0011, (void*)&InputRegBuf[17],
/* Line - 19 */ 0x0012, (void*)&InputRegBuf[18],
/* Line - 20 */ 0x0013, (void*)&InputRegBuf[19],
/* Line - 21 */ 0x0014, (void*)&InputRegBuf[20],
/* Line - 22 */ 0x0015, (void*)&InputRegBuf[21],
/* Line - 23 */ 0x0016, (void*)&InputRegBuf[22],
/* Line - 24 */ 0x0017, (void*)&InputRegBuf[23],
/* Line - 25 */ 0x0018, (void*)&InputRegBuf[24],
/* Line - 26 */ 0x0019, (void*)&InputRegBuf[25],
/* Line - 27 */ 0x001A, (void*)&InputRegBuf[26],
/* Line - 28 */ 0x001B, (void*)&InputRegBuf[27],
/* Line - 29 */ 0x001C, (void*)&InputRegBuf[28],
/* Line - 30 */ 0x001D, (void*)&InputRegBuf[29],
/* Line - 31 */ 0x001E, (void*)&InputRegBuf[30],
/* Line - 32 */ 0x001F, (void*)&InputRegBuf[31],
};
///*********************************************************************************/
const struct HoldRegType sHoldReg_Group[]=
{
/* Total- 32 */     48, (void*)&RTUGroup[3],
/* Line -  1 */ 0x0000, (void*)&HoldRegBuf[0],
/* Line -  2 */ 0x0001, (void*)&HoldRegBuf[1],
/* Line -  3 */ 0x0002, (void*)&HoldRegBuf[2],
/* Line -  4 */ 0x0003, (void*)&HoldRegBuf[3],
/* Line -  5 */ 0x0004, (void*)&HoldRegBuf[4],
/* Line -  6 */ 0x0005, (void*)&HoldRegBuf[5],
/* Line -  7 */ 0x0006, (void*)&HoldRegBuf[6],
/* Line -  8 */ 0x0007, (void*)&HoldRegBuf[7],
/* Line -  9 */ 0x0008, (void*)&HoldRegBuf[8],
/* Line - 10 */ 0x0009, (void*)&HoldRegBuf[9],
/* Line - 11 */ 0x000A, (void*)&HoldRegBuf[10],
/* Line - 12 */ 0x000B, (void*)&HoldRegBuf[11],
/* Line - 13 */ 0x000C, (void*)&HoldRegBuf[12],
/* Line - 14 */ 0x000D, (void*)&HoldRegBuf[13],
/* Line - 15 */ 0x000E, (void*)&HoldRegBuf[14],
/* Line - 16 */ 0x000F, (void*)&HoldRegBuf[15],
/* Line - 17 */ 0x0010, (void*)&HoldRegBuf[16],
/* Line - 18 */ 0x0011, (void*)&HoldRegBuf[17],
/* Line - 19 */ 0x0012, (void*)&HoldRegBuf[18],
/* Line - 20 */ 0x0013, (void*)&HoldRegBuf[19],
/* Line - 21 */ 0x0014, (void*)&HoldRegBuf[20],
/* Line - 22 */ 0x0015, (void*)&HoldRegBuf[21],
/* Line - 23 */ 0x0016, (void*)&HoldRegBuf[22],
/* Line - 24 */ 0x0017, (void*)&HoldRegBuf[23],
/* Line - 25 */ 0x0018, (void*)&HoldRegBuf[24],
/* Line - 26 */ 0x0019, (void*)&HoldRegBuf[25],
/* Line - 27 */ 0x001A, (void*)&HoldRegBuf[26],
/* Line - 28 */ 0x001B, (void*)&HoldRegBuf[27],
/* Line - 29 */ 0x001C, (void*)&HoldRegBuf[28],
/* Line - 30 */ 0x001D, (void*)&HoldRegBuf[29],
/* Line - 31 */ 0x001E, (void*)&HoldRegBuf[30],
/* Line - 32 */ 0x001F, (void*)&HoldRegBuf[31],
/* Line - 28 */ 0x0020, (void*)&HoldRegBuf[32],
/* Line - 29 */ 0x0021, (void*)&HoldRegBuf[33],
/* Line - 30 */ 0x0022, (void*)&HoldRegBuf[34],
/* Line - 31 */ 0x0023, (void*)&HoldRegBuf[35],
/* Line - 32 */ 0x0024, (void*)&HoldRegBuf[36],
/* Line - 30 */ 0x0025, (void*)&HoldRegBuf[37],
/* Line - 31 */ 0x0026, (void*)&HoldRegBuf[38],
/* Line - 32 */ 0x0027, (void*)&HoldRegBuf[39],
/* Line - 31 */ 0x0028, (void*)&HoldRegBuf[40],
/* Line - 32 */ 0x0029, (void*)&HoldRegBuf[41],
/* Line - 28 */ 0x002A, (void*)&HoldRegBuf[42],
/* Line - 29 */ 0x002B, (void*)&HoldRegBuf[43],
/* Line - 30 */ 0x002C, (void*)&HoldRegBuf[44],
/* Line - 31 */ 0x002D, (void*)&HoldRegBuf[45],
/* Line - 32 */ 0x002E, (void*)&HoldRegBuf[46],
/* Line - 30 */ 0x002F, (void*)&HoldRegBuf[47],
};
/*********************************************************************************/



