//Ver2.0,modified data 2011.04.25 by DSLV
//SPI Interface
extern void InitSpi(void);
extern int NWordReadEEPROMFIFO(unsigned int *SPIBuffer,unsigned int Page,unsigned int offset,unsigned int *Ctrl);
extern int NWordWriteEEPROMFIFO(unsigned int *SPIBuffer,unsigned int Page,unsigned int offset,unsigned int *Ctrl);
extern int EEPROMPageErase(unsigned int Page,unsigned int *Ctrl);
extern int EEPROMSectorErase(unsigned int Sector,unsigned int *Ctrl);
extern int EEPROMChipErase(unsigned int *Ctrl);
extern int EEPROMESIDRead(unsigned int *Ctrl,int *ID);

/*
return status referance
0: 空操作，调用程序但没有执行任何操作
1：正在执行相关操作
2：执行完毕，如是读操作则返回到缓冲区的数据有效
11：SPI FIFO缓冲区溢出，操作没有被执行
21：写入的页码超范围
22：写入的页内地址偏置超范围
23：写入的页内地址和长度检验超范围
*/
///////////////////////////////////////////////////////////////////////////////////
//应用层
extern int EEPROMAccess(unsigned int *CtrlWord,unsigned int *GroupNum);
