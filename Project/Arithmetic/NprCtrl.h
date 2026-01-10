/// NprCtrl.h

#ifndef _NPRCTRL_H_
#define _NPRCTRL_H_

////////////////////////////////////////
extern char * NprCtrlVersion();
////////////////////////////////////////
extern void GetTheta(void);
extern void CurrCtrlAlgorithm();
extern void UdcCtrl(void);
extern void Grid_Volt_SOGI(float Ugal, float Ugbe);
extern void IsLand_UgCtrl(void);
////////////////////////////////////////
#endif


