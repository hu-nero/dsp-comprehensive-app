/**************************************************************/
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Custom_ModbusM.h"

ModbusMasterDevice_t NodeEnergy = {0};
/*************************************************************/
void Sci_ModbusM_init(volatile struct SCI_REGS *p, float LSPCLK_FREQ, float SCI_FREQ)
{
	long SCI_PRD;

	p->SCICCR.all = 0x0007;/*0000,0000,0000,0111
	bit7:STOP BITS,0,one stop bit
	bit6:PARITY bit,0,Odd parity
	bit5:PARITY ENABLE bit,0,PARITY disable
	bit4:LOOP BACK ENA,0,disable loopback
	bit3:ADDR/IDLE MODE,0,Idle-line mode protocol
	bit2~0:	Character-length control bits,111,8 bit
	*/
	p->SCICTL1.all = 0x0003; /*0000,0000,0000,0011
	bit7:Reserved,0
	bit6:RX ERR INT ENA,0,disable
	bit5:SW RESET,0,reset
	bit4:Reserved,0
	bit3:TXWAKE,0
	bit2:SLEEP,0,Sleep mode disabled
	bit1:TXENA,1
	bit0:RXENA,1
	*/
	p->SCICTL2.all = 0x00C0; /*0000,0000,1100,0000
	bit7:TXRDY,1
	bit6:TX EMPTY,1
	bit5~2,reserved:0000
	bit1:RX/BK INT ENA;0,Disable RXRDY/BRKDT interrupt
	bit0:TX INT ENA,0,Disable TXRDY interrupt
	*/

	SCI_PRD = (long) ((LSPCLK_FREQ / (SCI_FREQ * 8)) - 1);
	p->SCIHBAUD = (SCI_PRD >> 8) & 0x00FF;
	p->SCILBAUD = (SCI_PRD >> 0) & 0x00FF;
	p->SCITXBUF = 0;
	p->SCIFFTX.all = 0x4040; /*0100,0000,0100,0000
	bit15:SCI Reset;0,SCI Reset
	bit14:SCI FIFO enable;1,enable
	bit13:Transmit FIFO reset;0,reset
	bit12~8:TXFFST4-0,00000,Transmit FIFO is empty
	bit7:Transmit FIFO interrupt;0,
	bit6:Transmit FIFO int clear,1
	bit5:Transmit FIFO interrrupt enable;0
	bit4~0:TXFFIL40 Transmit FIFO interrupt level bits,00000
	*/
	p->SCIFFRX.all = 0x4041; /*0100,0000,0100,0001
	bit15:Receive FIFO overflow,0
	bit14:RXFFOVF clear,1
	bit13:RXFIFO Reset,Receive FIFO reset,0
	bit12~8:RXFFST40,00000
	bit7:Receive FIFO interrupt flag,0
	bit6:Receive FIFO interrupt clear,1
	bit5:Receive FIFO interrupt enable,0
	bit4~0:Receive FIFO interrupt level bits,00001
	*/

	p->SCIFFCT.all = 0x4000; /*
	bit15:Auto-baud detect (ABD) bit,0
	bit14:ABD-clear bit,1
	bit13:CDC calibrate A-detect bit,0
	bit12~8:reserved;00000
	bit7~0,FIFO transfer delay,00000000
	*/
	p->SCIPRI.all = 0x0008;/*0000,1000
	bit7~5,reserved;000
	bit4~3:SOFT and FREE,01
	bit2~0,reserved,000
	*/
	p->SCIFFTX.bit.TXFIFOXRESET = 1;
	p->SCIFFTX.bit.SCIRST = 1;
	p->SCIFFRX.bit.RXFIFORESET = 1;
	p->SCICTL1.bit.SWRESET = 1;
}

/***********************************************************/
void Sci_ModbusM_SendArray(volatile struct SCI_REGS *pSCI, u8 * p_Arr, Uint16 Len)
{
	Uint16 i;
	IO_EN2RS485_SEND;
	for(i=0; i<Len; i++)
	{
		pSCI->SCITXBUF = *(p_Arr + i);
		while (pSCI->SCICTL2.bit.TXEMPTY == 0);
	}
	IO_EN2RS485_RECV;
}

/**************************************************************/
void Sci_ModbusM_RxData(volatile struct SCI_REGS *p, ModbusMasterDevice_t *pNode)
{
	Uint16  i, temp;

	if (!p->SCIRXST.bit.RXERROR)
	{
		if(p->SCIFFRX.bit.RXFFINT == 1)
		{
			if(pNode->RunStatus == MBM_STA_SendEnd)
			{
				i = 0;
				while(i < p->SCIFFRX.bit.RXFFST)
				{
					pNode->RxBuf[pNode->RecvCount ++] = p->SCIRXBUF.all;;
					i++;
				}
				pNode->RunStatus = MBM_STA_Recving;
				pNode->RecvTimeCount = 1;
			}
			else if(pNode->RunStatus == MBM_STA_Recving)
			{
				if(pNode->RecvCount < MBM_RTU_MAX_SIZE)
				{
					i = 0;
					while(i < p->SCIFFRX.bit.RXFFST)
					{
						pNode->RxBuf[pNode->RecvCount ++] = p->SCIRXBUF.all;;
						i++;
					}
				}
				pNode->RecvTimeCount = 1;
			}
			else
			{
				i = 0;
				while(i < p->SCIFFRX.bit.RXFFST)
				{
					temp = p->SCIRXBUF.all;
					(void) temp;
					i++;
				}
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

/************************************************************************/

/************************************************************************/


