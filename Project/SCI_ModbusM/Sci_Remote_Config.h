
#ifndef _SCI_REMOTE_CONFIG_H_
#define _SCI_REMOTE_CONFIG_H_

extern void Sci_Remote_init(volatile struct SCI_REGS *p, float LSPCLK_FREQ, float SCI_FREQ);
extern void Sci_Remote_SendData(volatile struct SCI_REGS *p, Uint16 dat);
extern void Sci_Remote_RxData(volatile struct SCI_REGS *p, Uint16 *pRxBuffer,Uint16 *pRear,Uint16 RevSize);
extern void Sci_Remote_SendArray(volatile struct SCI_REGS *p, Uint16 * p_Arr, Uint16 Len);
extern void Sci_Remote_SendArray_loop(volatile struct SCI_REGS *p, Uint16 * p_Arr);
extern void Sci_Remote_SendArray_new(Uint16 Len);

#endif
//===========================================================================
// End of file.
//===========================================================================

