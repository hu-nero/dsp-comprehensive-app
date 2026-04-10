#define FPGAWTIOSTS1	*(unsigned  int *)0x4010
#define FPGAWTSELTEMP	*(unsigned  int *)0x4011
#define FPGAWTIOSTS2	*(unsigned  int *)0x4012

#define FPGARDIOSTS1	*(unsigned  int *)0x4004
#define FPGARDIOSTS2	*(unsigned  int *)0x4005
#define FPGARDIOSTS3	*(unsigned  int *)0x4006  // est di
#define FPGARDSWVER1	*(unsigned  int *)0x4008
#define FPGARDFREQ      *(unsigned  int *)0x4009;
#define FPGARDCLKCNTL   *(unsigned  int *)0x400A;
#define FPGARDCLKCNTH   *(unsigned  int *)0x400B;
#define AD1				*(unsigned  int *)0x400C
#define AD2				*(unsigned  int *)0x400D
/************************************/
extern void InitXintf(void);
extern void IO_Scan(void);
//#pragma CODE_SECTION(Palarell_Extern_ADCScan, "ramfuncs");
extern void Palarell_Extern_ADCScan(void);
extern void Zone7_Init(void);
/*************************************/




