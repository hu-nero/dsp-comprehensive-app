#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
/***************************************************************************/
#pragma DATA_SECTION(SDSendBuff, "DMARAML7")
#pragma DATA_SECTION(SDRecvBuff, "DMARAML7")

int SDSendBuff[130],SDRecvBuff[130];
/************************************************************************/
void DMA_Init(void)
{
	EALLOW;
	DmaRegs.DMACTRL.all=0x0003;/*0000,0000,0000,0011
	  bit15~2:Reserved,0000,0000,0000,00
	  bit1:PRIORITYRESET(The priority reset bit).Reset,1
	  bit0:HARDRESET(hard reset bit).Reset,1
	*/
	DmaRegs.DEBUGCTRL.all=0x8000;/*1000,0000,0000,0000
	  bit15:FREE(Emulation Control Bit).DMA is unaffected by emulation suspend,1
	  bit14~0:Reserved,0000,0000,0000,000
	*/
	DmaRegs.PRIORITYCTRL1.all=0x0001;/*0000,0000,0000,0001
	  bit15~1:Reserved,0000,0000,0000,000
	  bit0:CH1PRIORITY(DMA Ch1 Priority).Highest priority channel,1
	*/
	DmaRegs.PRIORITYSTAT.all=0x0000;/*0000,0000,0000,0000
	  bit15~7:Reserved,0000,0000,0
	  bit6~4:ACTIVESTS_SHADOW(Active Channel Status Shadow Bits).No channel pending,000
	  bit3:Reserved,0
	  bit2~0:ACTIVESTS(Active Channel Status Bits).no channel active.000
	*/
//////////////////////////CH1用于SD卡接收/////////////////////////
  DmaRegs.CH1.MODE.all=0x030F;/*0000,0011,0000,1111
	  bit15:CHINTE(Channel Interrupt Enable Bit).Interrupt disabled,0
	  bit14:DATASIZE(Data Size Mode Bit).16-bit data transfer size,0
	  bit13:SYNCSEL(Sync Mode Select Bit).SRC wrap counter controlled,0
	  bit12:SYNCE(Sync Enable Bit).disable,0
	  bit11:CONTINUOUS(Continuous Mode Bit).after complete,DMA stop 0
	  bit10:ONESHOT(One Shot Mode Bit).0
	  bit9:CHINTMODE(Channel Interrupt Generation Mode Bit).Generate interrupt at end of transfer,1
	  bit8:PERINTE(Peripheral Interrupt Trigger Enable Bit).Interrupt trigger enabled,1
	  bit7:OVRINTE(Overflow Interrupt Enable).Overflow interrupt disabled,0
	  bit6~5:Reserved,00
	  bit4~0:PERINTSEL(Peripheral Interrupt Source Select Bits),MXEVTA,0,1111
	  */
	DmaRegs.CH1.CONTROL.all=0x00D4;/*0000,0000,1101,0100
	  bit15:Reserved,0
	  bit14:OVRFLG(Overflow Flag Bit),read only.No overflow event,0
	  bit13:RUNSTS(Run Status Bit).0
	  bit12:BURSTSTS(Burst Status Bit:),read only.0
	  bit11:TRANSFERSTS(Transfer Status Bit),read only.0
	  bit10:SYNCERR(Sync ERR Bit),read only.0
	  bit9:SYNCFLG(Sync Flag Bit),read only.0
	  bit8:PERINTFLG(Peripheral Interrupt Trigger Flag Bit),read only.0	
	  bit7:ERRCLR(Error Clear Bit),write 1 clear,1
	  bit6:SYNCCLR(Sync Clear Bit),write 1 clear,1
	  bit5:SYNCFRC(Sync Force Bit:).0
	  bit4:PERINTCLR(Peripheral Interrupt Clear Bit),write 1 clear,1
	  bit3:PERINTFRC(Peripheral Interrupt Force Bit),0
	  bit2:SOFTRESET(Channel Soft Reset Bit),force reset,1
	  bit1:HALT(Channel Halt Bit:).not halt DMA,0
	  bit0:RUN(Channel Run Bit),after init,set to 1,0		
	*/
	DmaRegs.CH1.BURST_SIZE.all=0;/*0000,0000,0000,0000
	  bit15~5:Reserved.0000,0000,000
	  bit4~0:specify the burst transfer size,transfer 1 word in a burst,0,0000
	*/
	DmaRegs.CH1.BURST_COUNT.all=0;/*
	  bit15~5:reserved,0000,0000,000
	  bit4~0:indicate the current burst counter value,0 word left in a burst,0,0000
	*/
  DmaRegs.CH1.SRC_BURST_STEP=0;//No address change
  DmaRegs.CH1.DST_BURST_STEP=0;//No address change
  DmaRegs.CH1.TRANSFER_SIZE=127;//specify the number of bursts to transfer,Transfer 128 bursts,127
  DmaRegs.CH1.TRANSFER_COUNT=0;//specify the current transfer counter value,0 bursts left to transfer,0
  DmaRegs.CH1.SRC_TRANSFER_STEP=0;//No address change
  DmaRegs.CH1.DST_TRANSFER_STEP=1;//Add 1 to address
  DmaRegs.CH1.SRC_WRAP_SIZE=0xFFFF;//disable the wrap function
  DmaRegs.CH1.SRC_WRAP_COUNT=0;
  DmaRegs.CH1.SRC_WRAP_STEP=0;
  DmaRegs.CH1.DST_WRAP_SIZE=0xFFFF;//disable the wrap function
  DmaRegs.CH1.DST_WRAP_COUNT=0;
  DmaRegs.CH1.DST_WRAP_STEP=0;
  DmaRegs.CH1.SRC_BEG_ADDR_SHADOW=(Uint32)(&McbspaRegs.DRR1.all);
  DmaRegs.CH1.SRC_BEG_ADDR_ACTIVE=(Uint32)(&McbspaRegs.DRR1.all);
  DmaRegs.CH1.SRC_ADDR_SHADOW=(Uint32)(&McbspaRegs.DRR1.all);
  DmaRegs.CH1.SRC_ADDR_ACTIVE=(Uint32)(&McbspaRegs.DRR1.all);
  DmaRegs.CH1.DST_BEG_ADDR_SHADOW=(Uint32)(&SDRecvBuff[0]);
  DmaRegs.CH1.DST_BEG_ADDR_ACTIVE=(Uint32)(&SDRecvBuff[0]);
  DmaRegs.CH1.DST_ADDR_SHADOW=(Uint32)(&SDRecvBuff[0]);
  DmaRegs.CH1.DST_ADDR_ACTIVE=(Uint32)(&SDRecvBuff[0]);
////////////////////CH2用于SD卡发送/////////////////////
	DmaRegs.CH2.MODE.all=0x030E;/*0000,0011,0000,1110
	  bit15:CHINTE(Channel Interrupt Enable Bit).Interrupt disabled,0
	  bit14:DATASIZE(Data Size Mode Bit).16-bit data transfer size,0
	  bit13:SYNCSEL(Sync Mode Select Bit).SRC wrap counter controlled,0
	  bit12:SYNCE(Sync Enable Bit).disable,0
	  bit11:CONTINUOUS(Continuous Mode Bit).after complete,DMA stop 0
	  bit10:ONESHOT(One Shot Mode Bit).0
	  bit9:CHINTMODE(Channel Interrupt Generation Mode Bit).Generate interrupt at end of transfer,1
	  bit8:PERINTE(Peripheral Interrupt Trigger Enable Bit).Interrupt trigger disable,1
	  bit7:OVRINTE(Overflow Interrupt Enable).Overflow interrupt disabled,0
	  bit6~5:Reserved,00
	  bit4~0:PERINTSEL(Peripheral Interrupt Source Select Bits),MXEVTA,0,1110
	  */
	DmaRegs.CH2.CONTROL.all=0x00D4;/*0000,0000,1101,0100
	  bit15:Reserved,0
	  bit14:OVRFLG(Overflow Flag Bit),read only.No overflow event,0
	  bit13:RUNSTS(Run Status Bit).0
	  bit12:BURSTSTS(Burst Status Bit:),read only.0
	  bit11:TRANSFERSTS(Transfer Status Bit),read only.0
	  bit10:SYNCERR(Sync ERR Bit),read only.0
	  bit9:SYNCFLG(Sync Flag Bit),read only.0
	  bit8:PERINTFLG(Peripheral Interrupt Trigger Flag Bit),read only.0	
	  bit7:ERRCLR(Error Clear Bit),write 1 clear,1
	  bit6:SYNCCLR(Sync Clear Bit),write 1 clear,1
	  bit5:SYNCFRC(Sync Force Bit:).0
	  bit4:PERINTCLR(Peripheral Interrupt Clear Bit),write 1 clear,1
	  bit3:PERINTFRC(Peripheral Interrupt Force Bit),0
	  bit2:SOFTRESET(Channel Soft Reset Bit),force reset,1
	  bit1:HALT(Channel Halt Bit:).not halt DMA,0
	  bit0:RUN(Channel Run Bit),after init,set to 1,0		
	*/
	DmaRegs.CH2.BURST_SIZE.all=0;/*0000,0000,0000,0000
	  bit15~5:Reserved.0000,0000,000
	  bit4~0:specify the burst transfer size,transfer 1 word in a burst,0,0000
	*/
	DmaRegs.CH2.BURST_COUNT.all=0;/*
	  bit15~5:reserved,0000,0000,000
	  bit4~0:indicate the current burst counter value,0 word left in a burst,0,0000
	*/
  DmaRegs.CH2.SRC_BURST_STEP=0;//No address change
  DmaRegs.CH2.DST_BURST_STEP=0;//No address change
  DmaRegs.CH2.TRANSFER_SIZE=127;//specify the number of bursts to transfer,Transfer 128 bursts,127
  DmaRegs.CH2.TRANSFER_COUNT=0;//specify the current transfer counter value,0 bursts left to transfer,0
  DmaRegs.CH2.SRC_TRANSFER_STEP=1;//Add 1 to address
  DmaRegs.CH2.DST_TRANSFER_STEP=0;//No address change
  DmaRegs.CH2.SRC_WRAP_SIZE=0xFFFF;//disable the wrap function
  DmaRegs.CH2.SRC_WRAP_COUNT=0;
  DmaRegs.CH2.SRC_WRAP_STEP=0;
  DmaRegs.CH2.DST_WRAP_SIZE=0xFFFF;//disable the wrap function
  DmaRegs.CH2.DST_WRAP_COUNT=0;
  DmaRegs.CH2.DST_WRAP_STEP=0;
  DmaRegs.CH2.SRC_BEG_ADDR_SHADOW=(Uint32)(&SDSendBuff[0]);
  DmaRegs.CH2.SRC_BEG_ADDR_ACTIVE=(Uint32)(&SDSendBuff[0]);
  DmaRegs.CH2.SRC_ADDR_SHADOW=(Uint32)(&SDSendBuff[0]);
  DmaRegs.CH2.SRC_ADDR_ACTIVE=(Uint32)(&SDSendBuff[0]);
  DmaRegs.CH2.DST_BEG_ADDR_SHADOW=(Uint32)(&McbspaRegs.DXR1.all);
  DmaRegs.CH2.DST_BEG_ADDR_ACTIVE=(Uint32)(&McbspaRegs.DXR1.all);
  DmaRegs.CH2.DST_ADDR_SHADOW=(Uint32)(&McbspaRegs.DXR1.all);
  DmaRegs.CH2.DST_ADDR_ACTIVE=(Uint32)(&McbspaRegs.DXR1.all);
///////////////////////////////////////////////////////////////////////////////  
  DmaRegs.DMACTRL.all=0x0002;//Priority reset

  EDIS;	
}
/************************************************************************/
void StartDMA(void)
{
  EALLOW;
  DmaRegs.CH1.CONTROL.bit.RUN=1;
  DmaRegs.CH2.CONTROL.bit.RUN=1;	
  EDIS;
}
/***********************************************************************/
void DMAAccessNum(int BufCount)
{
	if(BufCount<1)
	  BufCount=1;
	EALLOW;
	DmaRegs.CH1.TRANSFER_SIZE=BufCount-1;
	DmaRegs.CH2.TRANSFER_SIZE=BufCount-1;
	EDIS;	
}
/***********************************************************************/
void DMA_ChannelReset(void)
{
  DmaRegs.CH1.CONTROL.bit.SOFTRESET=1;
  DmaRegs.CH2.CONTROL.bit.SOFTRESET=1;
  DmaRegs.CH3.CONTROL.bit.SOFTRESET=1;
  DmaRegs.CH4.CONTROL.bit.SOFTRESET=1;
  DmaRegs.CH5.CONTROL.bit.SOFTRESET=1;
  DmaRegs.CH6.CONTROL.bit.SOFTRESET=1;
}
/**************************************************************************/
