#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "Custom_SPI_EEPROM.H"
/**************************************************************************/
void InitSpiaGpio()
{
   EALLOW;
   GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
   GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
   GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as 25Lc1024_CS-
    
   GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
   GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
   GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
   GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (25Lc1024_CS-)
   
   GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO16 (SPISIMOA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO17 (SPISOMIA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO18 (SPICLKA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO19 (25Lc1024_CS-)
   
   GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;   // SPISIMOA Direction:0,in;1,out
   GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;   // SPISOMIA Direction:0,in;1,out
   GpioCtrlRegs.GPBDIR.bit.GPIO56 = 0;   // SPICLKA Direction:0,in;1,out
   GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;   // 25LC1024_cs,Direction:0,in;1,out
   EDIS;
}

/************************************************/
// Initialize SPI FIFO registers
void spi_fifo_init()
{
   SpiaRegs.SPIFFTX.all=0x4040;        /*0100,0000,0100,0000
   bit15: 0,SPIRST,SPI reset(reset enable)
   bit14: 1,SPIFFENA SPI FIFO enhancements enable
   bit13: 0,TXFIFO Transmit FIFO reset(Re-enable Transmit FIFO operation)
   bit8~12: 00000,Transmit FIFO status
   bit7: 0,TXFIFO interrupt flag
   bit6: 1,TXFIFO clear
   bit5: 0,TX FIFO interrupt enable
   bit4~0: 00000,transmit FIFO interrupt level bits.    
   */
   SpiaRegs.SPIFFRX.all=0x405F;         /*0100,0000,0101,1111
   bit15: 0,Receive FIFO overflow flag,read only
   bit14: 1,Receive FIFO overflow clear,write 1 clear flag to 0
   bit13: 0,RXFIFO,Receive FIFO reset,1 Re-enable transmit FIFO operation
   bit12~8: 00000,RXFFST4~0,Receive FIFO is empty
   bit7: 0,Receive FIFO interrupt flag,readonly
   bit6: 1,Receive FIFO interrupt clear,1 clear flag(bit7)
   bit5: 0, RXFFIENA RX FIFO interrupt disable
   bit4~0: 11111,Receive FIFO interrupt level bits
   */
   SpiaRegs.SPIFFCT.all=2;          //2 clock-cycle delay between two transfer
   SpiaRegs.SPIPRI.all=0x0010;       /*0001,0000
   bit7~6: 00,reserved
   bit5~4: 01,emulation suspend free run
   bit3~0: 0000,reserved
   */
   SpiaRegs.SPIFFTX.bit.SPIRST=1;
   SpiaRegs.SPIFFTX.bit.TXFIFO=1;
   SpiaRegs.SPIFFTX.bit.SPIFFENA=1;
   SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
   
}
/********************************************/
// This function initializes the SPI(s) to a known state.
void InitSpi(void)
{
   //InitSpiaGpio();
   spi_fifo_init();
   SpiaRegs.SPICCR.all=0X0007;/*0000,0111
   bit7:0,reset
   bit6:0,SPI Clock Null is low
   bit5:0,reserved
   bit4:0,SPI lookback mode disable
   bit3~0:0111,8bit data  */	                                                                                                                                
   SpiaRegs.SPICTL.all=0X000E; /*0000,1110
   bit7~5:000,reserved
   bit4:0,disable overrun int
   bit3:1,clock phase has half-cycle delay
   bit2:1,master
   bit1:1,enable talk(Tx)
   bit0:0,disable int  */
   SpiaRegs.SPISTS.all=0;
   SpiaRegs.SPIBRR=0X00007; //37.5M/(SPIBRR+1),4.6875M
   SpiaRegs.SPICCR.bit.SPISWRESET=1; //SPI enable  
}
/**************************************************************************/
