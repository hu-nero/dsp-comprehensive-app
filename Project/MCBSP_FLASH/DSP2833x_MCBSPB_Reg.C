#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "IQmathLib.h"
/**************************************************************************/
void MCBSPB_Init(void)
{
	McbspbRegs.DRR1.all=0;//Data Receive Register 
	McbspbRegs.DRR2.all=0;//Data Receive Register 
	McbspbRegs.DXR1.all=0;//Data Transmit Registers
	McbspbRegs.DXR2.all=0;//Data Transmit Registers
	McbspbRegs.SPCR1.all=0x1000;/*0001,1000,0000,0000
	  bit15:DLB(Digital loopback mode bit).disable,0
	  bit14~13:RJUST(Receive sign-extension and justification mode bits).00,Right justify the data and zero fill the MSBs	
	  bit12~11:CLKSTP(Clock stop mode bits).10, Clock stop mode, without clock delay
	  bit10~8:Reserved,000
	  bit7:DXENA(DX delay enabler mode bit).DX delay enabler off,0
	  bit6:Reserved.0	
	  bit5~4:RINTM(Receive interrupt mode bit).00,when the RRDY bit changes from 0 to 1,send int to cpu
	  bit3:RSYNCERR(Receive frame-sync error bit).0,No error
	  bit2:RFULL(Receiver full bit).0,No receiver-full condition	
	  bit1:RRDY(Receiver ready bit).0,Receiver not ready,		
	  bit0:RRST(Receiver reset bit).0,the receiver is in its reset state		
  */
  McbspbRegs.SPCR2.all=0x0200;/*0000,0010,0000,0000
    bit15~10:Reserved,0000,00
    bit9:FREE(Free run bit),1
    bit8:SOFT(Soft stop bit),0
    bit7:FRST(Frame-synchronization logic reset bit),0
    bit6:GRST(Sample rate generator reset bit).reset,0
    bit5~4:XINTM(Transmit interrupt mode bits).XRDY bit changes from 0 to 1,0
    bit3:XSYNCERR(Transmit frame-synchronization error bit).No error,0
    bit2:XEMPTY(Transmitter empty bit).Transmitter-empty condition,0
    bit1:XRDY(Transmitter ready bit).Transmitter ready,0
    bit0:XRST(Transmitter reset bit).reset,0
    */		
  McbspbRegs.RCR1.all=0x0000;/*0000,0000,0000,0000  	
    bit15:Reserved,0
    bit14~8:RFRLEN1(Receive frame length 1).phase 1 has one word data,0000,000
    bit7~5:RWDLEN1(Receive word length 1).word of phase1 has 8 bit,000
    bit4~0:Reserved,00000
  */	
  McbspbRegs.RCR2.all=0x0004;/*0000,0000,0000,0100  
    bit15:RPHASE(Receive phase number bit).Single-phase frame,0
    bit14~8:RFRLEN2(Receive frame length 2).phase2 has one word,0000,000
    bit7~5:RWDLEN2(Receive word length 2).word of phase2 has 8 bit,000
    bit4~3:RCOMPAND(Receive companding mode bits).No companding, any size data, MSB received first,00	
    bit2:RFIG(Receive frame-synchronization ignore bit).Frame-synchronization ignore,1
    bit1~0:RDATDLY(Receive data delay bits).0-bit data delay,00			
  */			
  McbspbRegs.XCR1.all=0x0000;/*0000,0000,0000,0000 
    bit15:Reserved,0 
    bit14~8:XFRLEN1(Transmit frame length 1).phase 1 has 1 word,0000000	
    bit7~5: XWDLEN1(Transmit word length 1).word of phase1 has 8 bit,000
    bit4~0:reserved,00000
  */	
  McbspbRegs.XCR2.all=0x0004;/*0000,0000,0000,0100 
    bit15:XPHASE(Transmit phase number bit).Single-phase frame,0
    bit14~8:XFRLEN2(Transmit frame length 2).phase 2 has 1 word,0000000	
    bit7~5: XWDLEN2(Transmit word length 2).word of phase2 has 8 bit,000
    bit4~3:XCOMPAND(Transmit companding mode bits).No companding, any size data, MSB transmitted first,00
    bit2:XFIG(Transmit frame-synchronization ignore bit).Frame-synchronization ignore,1
    bit1~0:XDATDLY(Transmit data delay bits).	0-bit data delay,00	
  */	
  McbspbRegs.SRGR1.all=0x0004;/*0000,0000,0000,0010 
    bit15~8:FWID(Frame-synchronization pulse width bits for FSG).00000000
    bit7~0:CLKGDV(Divide-down value for CLKG).LSPCLK is 150M/4=37.5M,div (4+1) is about 7.5MHz	
  */
  McbspbRegs.SRGR2.all=0x2000;/*0010,0000,0000,0000 
    bit15:GSYNC(Clock synchronization mode bit for CLKG).No clock synchronization,0
    bit14:Reserved,0
    bit13:CLKSM(Sample rate generator input clock mode bit).CLKG come from LSPCLK,1
    bit12:FSGM(Sample rate generator transmit frame-synchronization mode bit).McBSP generates a transmit frame-synchronization pulse when the content of DXR[1,2] is copied to XSR[1,2],0	
    bit11~0:FPER(Frame-synchronization period bits for FSG).0			
  */
  McbspbRegs.MCR1.all=0x0001;/*0000,0000,0000,0001
    bit15~10:Reserved,0000,00
    bit9:RMCME(Receive multichannel partition mode bit).2-partition mode,0
    bit8~7:RPBBLK(Receive partition B block bits).Block 1,00
    bit6~5:RPABLK(Receive partition A block bits).Block 0,00	
    bit4~2:RCBLK(Receive current block indicator).000
    bit1:Reserved,0
    bit0:RMCM(Receive multichannel selection mode bit).Multichanneled selection mode,1
  */
  McbspbRegs.MCR2.all=0x0003;/*0000,0000,0000,0011
    bit15~10:Reserved,0000,00
    bit9:XMCME(Transmit multichannel partition mode bit).2-partition mode,0
    bit8~7:XPBBLK(Transmit partition B block bits).	Block 1,00
    bit6~5:XPABLK(Transmit partition A block bits). Block 0,00
    bit4~2:XCBLK(Transmit current block indicator). Block 0,000
    bit1~0:XMCM(Transmit multichannel selection mode bits).symmetric transmission and reception,11		
  */
    McbspbRegs.PCR.all=0x0F32;/*0000,1111,0011,0010
    bit15~12:Reserved,0000
    bit11:FSXM(Transmit frame-synchronization mode bit).TFS is generate from SRG,1
    bit10:FSRM(Receive frame-synchronization mode bit).	RFS is generate from SRG,1
    bit9:CLKXM(Transmit clock mode bit).Internal CLK,MCLKX Pin output,1
    bit8:CLKRM(Receive clock mode bit).	Internal CLK,MCLKR Pin output,1
    bit7:SCLKME(Sample rate generator input clock mode bit).Clock from LSPCLK,0
    bit6:Reseved,0	
    bit5:DXSTAT(DX pin status bit).Drive the signal on the DX pin high,1
    bit4:DRSTAT(DR pin status bit),read only.The signal on DR pin is high,1	
    bit3:FSXP(Transmit frame-synchronization polarity bit).Transmit frame-synchronization pulses are active high,0
    bit2:FSRP(Receive frame-synchronization polarity bit).Receive frame-synchronization pulses are active high,0	
    bit1:CLKXP(Transmit clock polarity bit).Transmit data is sampled on the falling edge of CLKX,1
    bit0:CLKRP(Receive clock polarity bit).	Receive data is sampled on the falling edge of MCLKR.,0
  */
  McbspbRegs.RCERA.all=0xFFFF;
  McbspbRegs.RCERB.all=0x0000;	
  McbspbRegs.RCERC.all=0x0000;
  McbspbRegs.RCERD.all=0x0000;
  McbspbRegs.RCERE.all=0x0000;
  McbspbRegs.RCERF.all=0x0000;
  McbspbRegs.RCERG.all=0x0000;
  McbspbRegs.RCERH.all=0x0000;
  McbspbRegs.XCERA.all=0xFFFF;
  McbspbRegs.XCERB.all=0x0000;
  McbspbRegs.XCERC.all=0x0000;
  McbspbRegs.XCERD.all=0x0000;
  McbspbRegs.XCERE.all=0x0000;
  McbspbRegs.XCERF.all=0x0000;
  McbspbRegs.XCERG.all=0x0000;
  McbspbRegs.XCERH.all=0x0000;
  
  McbspbRegs.MFFINT.all=0x0000;/*0000,0000,0000,0000
    bit15~3:Reserved,0000,0000,0000,0
    bit2:RINT ENA(Enable for Receive Interrupt).disable,0	
    bit1:Reserved,0
    bit0:XINT ENA(Enable for transmit Interrupt).disable,0		
  */	
  McbspbRegs.SPCR1.bit.RRST=1;
  McbspbRegs.SPCR2.bit.GRST=1;
  McbspbRegs.SPCR2.bit.XRST=1;
}
/************************************************************************/
void MCBSP_DataBit8(void)
{
  McbspbRegs.RCR1.bit.RWDLEN1=0;
  McbspbRegs.XCR1.bit.XWDLEN1=0;
}
/***********************************************************************/
void MCBSP_DataBit16(void)
{
  McbspbRegs.RCR1.bit.RWDLEN1=2;
  McbspbRegs.XCR1.bit.XWDLEN1=2;
}
/************************************************************************/
