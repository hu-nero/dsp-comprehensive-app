/////////////////////////////////////////////////////////////////
//*		Read Address
//**	[0x000 ~ 0x00F] Degital main function
//**	[0x010 ~ 0x07F] Analog  main function
//**	[0x080 ~ 0x08F] Degital interrupt function
//**	[0x090 ~ 0x0FF] Analog  interrupt function
//*		Write Address
//**	[0x100 ~ 0x10F] Degital main function
//**	[0x110 ~ 0x17F] Analog  main function
//**	[0x180 ~ 0x18F] Degital interrupt function
//**	[0x190 ~ 0x1FF] Analog  interrupt function
/////////////////////////////////////////////////////////////////
// [0x000 ~ 0x00F] Main function Read Degital
#define FPGARD_TOGGLE	*(unsigned  int *)0x4000
#define FPGARD_IOINPUT	*(unsigned  int *)0x4001
#define FPGARD_STATACK	*(unsigned  int *)0x4002

#define FPGARD_HARDFLT	*(unsigned  int *)0x4009
#define FPGARD_SOFAFLT	*(unsigned  int *)0x400A
#define FPGARD_SWVER1	*(unsigned  int *)0x400B
#define FPGARD_DEVICE	*(unsigned  int *)0x400C
#define FPGARD_FREQH	*(unsigned  int *)0x400D
#define FPGARD_FREQL	*(unsigned  int *)0x400E
#define FPGARD_FREQCNT	*(unsigned  int *)0x400F
// [0x080 ~ 0x09F] Interrupt function Read Degital
#define FPGARD_MASTOSLA1 *(unsigned  int *)0x4080
#define FPGARD_MASTOSLA2 *(unsigned  int *)0x4081
// [0x100 ~ 0x11F] Main function Write Degital
#define FPGAWR_TOGGLE	*(unsigned  int *)0x4100
#define FPGAWR_IOOUTPUT	*(unsigned  int *)0x4101
#define FPGAWR_CTRLREF	*(unsigned  int *)0x4102
// [0x180 ~ 0x19F] Interrupt function Write Degital
#define FPGAWR_SLATOMAS1 *(unsigned  int *)0x4180
#define FPGAWR_SLATOMAS2 *(unsigned  int *)0x4181
/**************************************************************/
extern void InitXintf(void);
extern void IO_Scan(void);
//#pragma CODE_SECTION(Palarell_Extern_ADCScan, "ramfuncs");
extern void Palarell_Extern_ADCScan(void);
extern void Zone7_Init(void);
/**************************************************************/




