/*************************************************************
�ļ����:SciConfig.c/.h
�ļ�����:����SCI��ʵ�ּ����ݶ��е��������պͷ��͡�
�汾˵��:Checkin $Date: August 21, 2012   10:02:38 $,    Written by :Liu Qi.
�޸ļ�¼:
1.
2.
3.
**************************************************************/
#include "DSP2833x_Device.h"
#include "SciConfig.h"
#include "Version.h"
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "ParaDefine.h"	/*�����뵥���ַ��ض���*/       
#include "MessageParse.h"	/*��������շ�����ݶ��е���ض���*/ 
#include "GPIO\Custom_IOMacro.H"
/*************************************************************/
int MonTx_EndReal=0;
int MonTx_1msValueOld=0;
int MonTx_Flag=0;
int MonTx_1msCount=0;
int MonTx_BusyCount=0;
int TxPINFlag=0;

void Sci_Monitor_TxMulData(volatile struct SCI_REGS *p,Uint16 *pTxBuffer,Uint16 *pFront,Uint16 nRear);
/**********************************************************/
void SCIMon_TxEn(int Board,int data)
{

}
/************************************************************/
int SCIMon_TxBusy(int Board)
{
	(void)Board;
	return (0);
}
/*************************************************************/
void Sci_Monitor_init(volatile struct SCI_REGS *p)
{
	/*********InitSciGPIO*********/
	/*
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO14 for SCITXDB operation
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO15 for SCIRXDB operation
	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPBDIR.bit.GPIO63=1;
	EDIS;
	*/
	p->SCICCR.all =0x0007;/*0000,0000,0000,0111     
	  bit7:STOP BITS,0,one stop bit
	  bit6:PARITY bit,0,Odd parity
	  bit5:PARITY ENABLE bit,0,PARITY disable
	  bit4:LOOP BACK ENA,0,disable loopback
	  bit3:ADDR/IDLE MODE,0,Idle-line mode protocol
	  bit2~0:	Character-length control bits,111,8 bit

	*/
	p->SCICTL1.all =0x0003; /*0000,0000,0000,0011
	  bit7:Reserved,0
	  bit6:RX ERR INT ENA,0,disable
	  bit5:SW RESET,0,reset
	  bit4:Reserved,0
	  bit3:TXWAKE,0
	  bit2:SLEEP,0,Sleep mode disabled
	  bit1:TXENA,1
	  bit0:RXENA,1
	*/
	p->SCICTL2.all =0x00C0; /*0000,0000,1100,0000
	  bit7:TXRDY,1
	  bit6:TX EMPTY,1
	  bit5~2,reserved:0000
	  bit1:RX/BK INT ENA;0,Disable RXRDY/BRKDT interrupt
	  bit0:TX INT ENA,0,Disable TXRDY interrupt
	*/
	
	p->SCIHBAUD    = 0x0000;
	p->SCILBAUD    = BAUD_921600;
	p->SCITXBUF = 0;
	p->SCIFFTX.all=0x4040;  /*0100,0000,0100,0000
	  bit15:SCI Reset;0,SCI Reset
	  bit14:SCI FIFO enable;1,enable
	  bit13:Transmit FIFO reset;0,reset
	  bit12~8:TXFFST4-0,00000,Transmit FIFO is empty
	  bit7:Transmit FIFO interrupt;0,
	  bit6:Transmit FIFO int clear,1
	  bit5:Transmit FIFO interrrupt enable;0
	  bit4~0:TXFFIL4�C0 Transmit FIFO interrupt level bits,00000
	*/
  p->SCIFFRX.all=0x4041;   /*0100,0000,0100,0001
    bit15:Receive FIFO overflow,0
    bit14:RXFFOVF clear,1
    bit13:RXFIFO Reset,Receive FIFO reset,0
    bit12~8:RXFFST4�C0,00000
    bit7:Receive FIFO interrupt flag,0
    bit6:Receive FIFO interrupt clear,1
    bit5:Receive FIFO interrupt enable,0
    bit4~0:Receive FIFO interrupt level bits,00001
  */
  
  p->SCIFFCT.all=0x4000;       /*
    bit15:Auto-baud detect (ABD) bit,0
    bit14:ABD-clear bit,1
    bit13:CDC calibrate A-detect bit,0
    bit12~8:reserved;00000
    bit7~0,FIFO transfer delay,00000000
  */
  p->SCIPRI.all=0x0008;/*0000,1000
    bit7~5,reserved;000
    bit4~3:SOFT and FREE,01
    bit2~0,reserved,000
  */
  p->SCIFFTX.bit.TXFIFOXRESET=1;
  p->SCIFFTX.bit.SCIRST=1;
  p->SCIFFRX.bit.RXFIFORESET=1;
  p->SCICTL1.bit.SWRESET=1;
}
/*************************************************************
***********************************************************/
void Sci_Monitor_RxData(volatile struct SCI_REGS *p,Uint16 *pRxBuffer,Uint16 *pRear)
{
	Uint16  i;      
	if ((!p->SCIRXST.bit.RXERROR))
	{
		if(p->SCIFFRX.bit.RXFFINT==1)
		{
			if((*pRear)>=SCI_RX_QUE_BUFF_SIZE)
			  (*pRear)=0;
			i=0;
			while(i<p->SCIFFRX.bit.RXFFST)
			{
				pRxBuffer[(*pRear)++]=p->SCIRXBUF.all;
				if((*pRear)>=SCI_RX_QUE_BUFF_SIZE)
				  (*pRear)-=SCI_RX_QUE_BUFF_SIZE;
				i++;
			}
			p->SCIFFRX.bit.RXFFINTCLR=1;
		}
	}
	else
	{
		p->SCICTL1.bit.SWRESET = 0;   
		p->SCICTL1.bit.SWRESET = 1;

		p->SCIFFRX.bit.RXFIFORESET = 0;
		p->SCIFFRX.bit.RXFIFORESET = 1; 
	}       
	p->SCIFFRX.bit.RXFFOVRCLR = 1;
}
/**********************************************************/
void Sci_Monitor_TxMulData(volatile struct SCI_REGS *p,Uint16 *pTxBuffer,Uint16 *pFront,Uint16 nRear)
{
	int temp;
	int FIFOEmptyByte=0;
	if(p->SCIFFTX.bit.TXFFINT==1)
	{
		if((*pFront)>=SCI_TX_QUE_BUFF_SIZE)
			(*pFront)=0;
		temp=nRear - *pFront;
		if(temp<0)
			temp+=SCI_TX_QUE_BUFF_SIZE;
		if(temp>0)
		{
			FIFOEmptyByte=16-p->SCIFFTX.bit.TXFFST;
			FIFOEmptyByte>>=1;
			if(temp>FIFOEmptyByte)
				temp=FIFOEmptyByte;
			while(temp>0)
			{
				p->SCITXBUF = pTxBuffer[(*pFront)]&0x00FF;
				p->SCITXBUF = (pTxBuffer[(*pFront)++]>>8)&0x00FF;
				if((*pFront)>=SCI_TX_QUE_BUFF_SIZE)
					(*pFront)-=SCI_TX_QUE_BUFF_SIZE;
				temp--;
			}
			p->SCIFFTX.bit.TXFFINTCLR=1;
		}
	}
}
/******************************************************************/
void Sci_Monitor_TxData(volatile struct SCI_REGS *p,Uint16 *pTxBuffer,Uint16 *pFront,Uint16 nRear,Uint16 nTimeFlag)
{
	if(MonTx_1msValueOld!=nTimeFlag)
	{
		MonTx_1msValueOld=nTimeFlag;
		if(MonTx_Flag==0)
			MonTx_BusyCount=0;
		else
		{
			if(MonTx_BusyCount<1000)
				MonTx_BusyCount++;
			else
			{
				MonTx_BusyCount=0;
				MonTx_Flag=0;
			}  
		}
		MonTx_1msCount++;
	}
//////////////////////////////////////////////////////
	if(MonTx_Flag == 0)
	{
		if((*pFront) != nRear)
		{
			if(SCIMon_TxBusy(BOARD_ADDR) == 0)
			{
				SCIMon_TxEn(BOARD_ADDR, 1);
				MonTx_Flag = 1;
				MonTx_1msCount=0;
			}
		}
	}
///////////////////////////////////////////	
	if(MonTx_Flag==1)
	{
		if(BOARD_ADDR == 4)
		{
			if(SCIMon_TxBusy(BOARD_ADDR) == 1)
			{
				SCIMon_TxEn(BOARD_ADDR, 0);
				MonTx_Flag = 0;
			}
		}
		if(MonTx_1msCount>=2)
		if(SCIMon_TxBusy(BOARD_ADDR)==0)
		{
			MonTx_1msCount=0;
			MonTx_Flag=2;
		}
	}
////////////////////////////////////////////
	if(MonTx_Flag==2)
	{
		if((*pFront) != nRear)
			Sci_Monitor_TxMulData(p,pTxBuffer,pFront,nRear);
		if(MonTx_1msCount>3)
		{
			MonTx_EndReal=nRear;
			MonTx_Flag=3;
			MonTx_1msCount=0;
		}
	}
/////////////////////////////////////////////////	
	if(MonTx_Flag==3)
	{
		if((*pFront) != MonTx_EndReal)
			Sci_Monitor_TxMulData(p,pTxBuffer,pFront,MonTx_EndReal);
		else
		{
			SCIMon_TxEn(BOARD_ADDR,0);
			MonTx_Flag=4;
			MonTx_1msCount=0;
		}
	}
///////////////////////////////////////////////////	
	if(MonTx_Flag==4)
	{
		if(BOARD_ADDR==4)
		{
			MonTx_Flag=0;
			MonTx_1msCount=0;
		}
		if(BOARD_ADDR==5)
		{
			if(MonTx_1msCount>=1)
			{
				MonTx_Flag=0;
				MonTx_1msCount=0;
			}
		}
	}
}
//===========================================================================
// End of file.
//===========================================================================
