#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_

/********************************************************************************************/
extern void FunU3s2sConv(float Uab, float Ubc, float *Ual, float *Ube);
extern void FunI3s2sConv(float Ia, float Ib, float Ic, float *Ial, float *Ibe);
extern void Fun2s3sConv(float Ux, float Uy, float *Ua, float *Ub, float *Uc);
extern void Fun2s2rConv(float Ux, float Uy, float Theta, float *Ud, float *Uq);
extern void Fun2r2sConv(float Ud, float Uq, float Theta, float *Ux, float *Uy);
/********************************************************************************************/
extern void SVPWM(float Ual, float Ube, float Udc, int Time_Per, int *PCmp1, int *PCmp2, int *PCmp3);
extern float FunPIComCtrl(float Kp, float Ki, float err, float *Perrtotal, float IMaxLim);
extern float FunPIComCtrl_DLim(float Kp, float Ki, float err, float *Perrtotal, float MinLim, float MaxLim);
/********************************************************************************************/
extern float Freq100LPFilter(float *x, float *y, float in, int Freq);
extern float Freq100_700BPFilter(float *x, float *y, float in, int Freq_Samp);
extern float Freq40_62d5BPFilter(float *x, float *y, float in, int Freq_Samp);
extern float Phase_90Delay_Filter(float *x, float *x_90, float in, int Freq_Samp);
/********************************************************************************************/
extern void PWM_Lock(void);
extern void PWM_Unlock(void);
/********************************************************************************************/
extern void Source_Select(void);
extern void Udq_Ramp_Limit(int PWM_Flag);
extern void Udq_FastChar(void);
/********************************************************************************************/

#endif

