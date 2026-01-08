extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void EnableInterrupts(void);


extern void InitSysCtrl(void);
extern void InitFlash(void);
extern void ServiceDog(void);
extern void DisableDog(void);
extern void InitPll(Uint16 val, Uint16 divsel);
extern void InitPeripheralClocks(void);
//extern void DSP28x_usDelay(Uint32 Count);
//extern void ADC_cal(void);//bai2012-4-14
#define KickDog ServiceDog     // For compatiblity with previous versions
extern void ServiceDog(void);
extern void DisableDog(void);
extern void EnableDog(void);
extern Uint16 CsmUnlock(void);
