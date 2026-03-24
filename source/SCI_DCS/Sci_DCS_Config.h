
#ifndef _SCI_DCS_CONFIG_H_
#define _SCI_DCS_CONFIG_H_

extern void Sci_DCS_init(volatile struct SCI_REGS *p, float LSPCLK_FREQ, float SCI_FREQ);
extern void Sci_DCS_SendData(volatile struct SCI_REGS *p, Uint16 dat);
extern void Sci_DCS_RxData(volatile struct SCI_REGS *p, Uint16 *pRxBuffer,Uint16 *pRear,Uint16 RevSize);
extern void Sci_DCS_SendArray(volatile struct SCI_REGS *p, Uint16 * p_Arr, Uint16 Len);

#endif
//===========================================================================
// End of file.
//===========================================================================

