/// NprCtrl.c

#include "NprCtrl.h"
#include "IQmathLib.h"
#include "main\Variable.H"
#include "main\Custom_GlobalMacro.H"
#include "Arithmetic.H"
#include "DSP28x_Project.h"

/////////////////////////////////////////////////////////////////////
void GetTheta(void)
{
	float ftemp;

	ftemp = npr_uqp;
	npr_freq = 50 + FunPIComCtrl(PLL_Kp, PLL_Ki, ftemp, &PLL_ErrTotal, PLL_Lim, PLL_Lim);

	ftemp = _2PI *npr_freq /PWM_ControlFreq;
	GridVolt_Angle += ftemp;

	while(GridVolt_Angle < 0)
		GridVolt_Angle += _2PI;
	while(GridVolt_Angle >= _2PI)
		GridVolt_Angle -= _2PI;

	GridCurrent_Angle = GridVolt_Angle + NPR_AngleCompensation;
	if(GridCurrent_Angle >= _2PI)
		GridCurrent_Angle -= _2PI;

	GridUconvert_Angle = GridVolt_Angle + NPR_AngleCompensation;
	if(GridUconvert_Angle >= _2PI)
		GridUconvert_Angle -= _2PI;

}
////////////////////////////////////////////////////////////////////////////////////////
