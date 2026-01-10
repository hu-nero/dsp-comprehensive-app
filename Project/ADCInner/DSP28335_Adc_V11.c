#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Main\Variable.h"
#include "IQmathLib.h"
#include "Xintf\Custom_Xintf_ADC&IO.H"



#if 1

float Cal_ADReg2Vol(int ADReg);
float Cal_NTC_Volt2R(float Vsamp);

float Cal_NTC5K_R2Temp(float R);
float Cal_NTC10K_R2Temp(float R);
float Cal_PT100_Volt2Temp(float Vsamp);
/**************************************************************************/
void InitAdc(void)
{
	int i;
	// *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;

	AdcRegs.ADCREFSEL.bit.REF_SEL = 0;
	//AdcRegs.ADCTRL3.bit.ADCBGRFDN =0X11;   // Power up bandgap/reference/ADC circuits

	// To powerup the ADC the ADCENCLK bit should be set first to enable
	// clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
	// Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

	// Please note that for the delay function below to operate correctly the
	// CPU_RATE define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.
	AdcRegs.ADCTRL3.all = 0x00E2;
	// AdcRegs.ADCTRL3.bit.ADCPWDN=0X01;

	for(i=0; i<20000; i++);

	AdcRegs.ADCTRL1.bit.CPS=1;
	AdcRegs.ADCTRL1.bit.ACQ_PS = 4;  // Sequential mode: Sample rate   = 1/[(2+ACQ_PS)*ADC clock in ns]
	//                     = 1/(3*40ns) =8.3MHz (for 150 MHz SYSCLKOUT)
	//                     = 1/(3*80ns) =4.17MHz (for 100 MHz SYSCLKOUT)
	// If Simultaneous mode enabled: Sample rate = 1/[(3+ACQ_PS)*ADC clock in ns]
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 5;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
	AdcRegs.ADCTRL1.bit.CONT_RUN=0;//.CONT_RUN = 0;       // Start-stop mode
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1=1;
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2=1;
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x00F;  // convert and store in 8 results registers
	AdcRegs.ADCCHSELSEQ1.all = 0x3210;
	AdcRegs.ADCCHSELSEQ2.all = 0x7654;
	AdcRegs.ADCCHSELSEQ3.all = 0xBA98;
	AdcRegs.ADCCHSELSEQ4.all = 0xFEDC;
}

/**************************************************************************************/
void Inner_ADCScan(void)
{
	int i;

	/////////////////////////////////////////////////////
	if(AdcRegs.ADCST.bit.INT_SEQ1)
	{
		AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

		InterADValue[0]  = ((AdcRegs.ADCRESULT0>>4));
		InterADValue[1]  = ((AdcRegs.ADCRESULT1>>4));
		InterADValue[2]  = ((AdcRegs.ADCRESULT2>>4));
		InterADValue[3]  = ((AdcRegs.ADCRESULT3>>4));
		InterADValue[4]  = ((AdcRegs.ADCRESULT4>>4));
		InterADValue[5]  = ((AdcRegs.ADCRESULT5>>4));
		InterADValue[6]  = ((AdcRegs.ADCRESULT6>>4));
		InterADValue[7]  = ((AdcRegs.ADCRESULT7>>4)); //cd4067 sel
		InterADValue[8]  = ((AdcRegs.ADCRESULT8>>4));
		InterADValue[9]  = ((AdcRegs.ADCRESULT9>>4));
		InterADValue[10] = ((AdcRegs.ADCRESULT10>>4));
		InterADValue[11] = ((AdcRegs.ADCRESULT11>>4));
		InterADValue[12] = ((AdcRegs.ADCRESULT12>>4));
		InterADValue[13] = ((AdcRegs.ADCRESULT13>>4));
		InterADValue[14] = ((AdcRegs.ADCRESULT14>>4));
		InterADValue[15] = ((AdcRegs.ADCRESULT15>>4));
		(void)NTC_temp[0];
		AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
		AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
	}
/////////////////////////////////////////////////////
	// CD4067 select adc chx

//	NTC_temp[FPGACD4067_SelFlag] = InterADValue[7];
//	FPGACD4067_SelFlag ++;
//	if(FPGACD4067_SelFlag > 5)
//	{
//		FPGACD4067_SelFlag = 0;
//	}
//	FPGAWTSELTEMP = FPGACD4067_SelFlag;

	for(i=0; i<6; i++)
	{
		NTC_tempf[i] = ((long)NTC_tempf[i]*7 + InterADValue[i]) >> 3;
		VolSample[i] = Cal_ADReg2Vol(NTC_tempf[i]);

		if(i<2)
		{
			ResSample[i] = Cal_NTC_Volt2R(VolSample[i]);
			TemprCalc[i] = Cal_NTC5K_R2Temp(ResSample[i]);
		}
		else
		{
			TemprCalc[i] = Cal_PT100_Volt2Temp(VolSample[i]);
		}
	}

	mpr_templow = TemprCalc[0]; //NTCL
	mpr_temphigh = TemprCalc[1]; //NTCH
//	Transformer_temp = TemprCalc[2];  //PT1
//	Rectifier_reactor_temp = TemprCalc[3]; //PT2
//	Inverter_reactor_temp = TemprCalc[4]; //PT3
}
/******************************************************************************************/
float Cal_ADReg2Vol(int ADReg)
{
	float Vsamp;
	Vsamp = ADReg *3.0 /4095;
	return Vsamp;
}

float Cal_NTC_Volt2R(float Vsamp)
{
	float Rsamp, ftemp1, ftemp2;
	ftemp1 = 38.54 *Vsamp;
	ftemp2 = 41 - 12.9*Vsamp;

	if(ftemp2 > 0)
	{
		Rsamp = fabs(ftemp1/ftemp2);
	}

	if(Rsamp > 200)
		Rsamp = 200;
	if(Rsamp < 0.1)
		Rsamp = 0.1;
	return Rsamp;
}
/*************************************************************************/
float Cal_NTC10K_R2Temp(float R)
{
	float Tempra=0;
	int i;

	const float RTable[14]  = {200.875, 105.656, 58.3594, 33.6641, 20.1875, 12.5391, 8.03910, 5.29687, 3.58594, 2.48438, 1.75781, 1.26563, 0.92969, 0.69531};
	const float KpTable[14] = {-0.1063, -0.2125, -0.4063, -0.7406,  -1.306,  -2.222,  -3.653,  -5.834,  -9.075,  -13.78,  -20.45,  -29.73,  -42.43,  -59.49};
	const float OffTable[14]= {   -8.9,     2.3,    13.6,    24.9,    36.3,    47.8,    59.3,    70.9,    82.5,    94.2,   105.9,   117.7,   129.5,   141.4};

	if(R > RTable[0])
		Tempra = R *KpTable[0] + OffTable[0];
	else if(R < RTable[13])
		Tempra = R*KpTable[13] + OffTable[13];
	else
	{
		for(i=0; i<13; i++)
		{
			if( (R<=RTable[i]) && (R>RTable[i+1]) )
			{
				Tempra = R*KpTable[i] + OffTable[i];
				break;
			}
		}
	}
	return Tempra;
}
/*************************************************************************************/
float Cal_NTC5K_R2Temp(float R)
{
	float Tempra=0;
	int i;

	const float RTable[13]  = {34.948, 21.5, 13.681, 8.972, 6.041, 4.16, 2.923, 2.09, 1.519, 1.121, 0.816, 0.618, 0.475};
	const float KpTable[13] = {-0.4190, -0.7436, -1.2789, -2.1236, -3.4118, -5.3163, -8.0841, -12.0048, -17.5131, -25.1256, -32.7869, -50.5051, -69.9301};
	const float OffTable[13]= {-5.3578, 5.9875, 17.4971, 29.0529, 40.6107, 52.1159, 63.6297, 75.0900, 86.6025, 98.1658, 106.7541, 121.2121, 133.2168};
	if(R > RTable[0])
		Tempra = R *KpTable[0] + OffTable[0];
	else if(R < RTable[12])
		Tempra = R*KpTable[12] + OffTable[12];
	else
	{
		for(i=0; i<12; i++)
		{
			if( (R<=RTable[i]) && (R>RTable[i+1]) )
			{
				Tempra = R*KpTable[i] + OffTable[i];
				break;
			}
		}
	}
	return Tempra;
}
/*************************************************************************************/
// pt100 Calc
/*************************************************************************/
float Cal_PT100_Volt2Temp(float Vsamp)
{
	float Rsamp, ftemp1, ftemp2, Tempr;

	ftemp1 =  4.99*Vsamp + 9.07953454;
	ftemp2 = 121.0604544 - Vsamp;

	if(ftemp2)
	{
		Rsamp = ftemp1/ftemp2*1000;
	}

	// -200℃~400℃
	if(Rsamp > 247.04)
	{
		Rsamp = 247.04;
	}
	else if(Rsamp < 18.49)
	{
		Rsamp = 18.49;
	}

	Tempr = (Rsamp - 100) *2.5974025;

	return Tempr;
}

#else

float Cal_ADReg2Vol(int ADReg);
float Cal_Vol2R(float Vsamp);
float Cal_R2Temp(float R);

/**************************************************************************/
void InitAdc(void)
{
	int i;
	// *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;

	AdcRegs.ADCREFSEL.bit.REF_SEL = 0;
	//AdcRegs.ADCTRL3.bit.ADCBGRFDN =0X11;   // Power up bandgap/reference/ADC circuits

	// To powerup the ADC the ADCENCLK bit should be set first to enable
	// clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
	// Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

	// Please note that for the delay function below to operate correctly the
	// CPU_RATE define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.
	AdcRegs.ADCTRL3.all = 0x00E2;
	// AdcRegs.ADCTRL3.bit.ADCPWDN=0X01;

	for(i=0; i<20000; i++);

	AdcRegs.ADCTRL1.bit.CPS=1;
	AdcRegs.ADCTRL1.bit.ACQ_PS = 4;  // Sequential mode: Sample rate   = 1/[(2+ACQ_PS)*ADC clock in ns]
	//                     = 1/(3*40ns) =8.3MHz (for 150 MHz SYSCLKOUT)
	//                     = 1/(3*80ns) =4.17MHz (for 100 MHz SYSCLKOUT)
	// If Simultaneous mode enabled: Sample rate = 1/[(3+ACQ_PS)*ADC clock in ns]
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 5;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
	AdcRegs.ADCTRL1.bit.CONT_RUN=0;//.CONT_RUN = 0;       // Start-stop mode
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1=1;
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2=1;
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x00F;  // convert and store in 8 results registers
	AdcRegs.ADCCHSELSEQ1.all = 0x3210;
	AdcRegs.ADCCHSELSEQ2.all = 0x7654;
	AdcRegs.ADCCHSELSEQ3.all = 0xBA98;
	AdcRegs.ADCCHSELSEQ4.all = 0xFEDC;
}
/**************************************************************************************/
void CD4067_ChSel(int CH)
{
//	GPIO10, 00, GPIO(O), ADSel3
//	GPIO9, 00, GPIO(O), ADSel2
//	GPIO8, 00, GPIO(O), ADSel1
	switch(CH)
	{
		case 0:
		{
			GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
		}break;
		case 1:
		{
			GpioDataRegs.GPASET.bit.GPIO8 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
		}break;
		case 2:
		{
			GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
		}break;
		case 3:
		{
			GpioDataRegs.GPASET.bit.GPIO8 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
		}break;
		case 4:
		{
			GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPASET.bit.GPIO10 = 1;
		}break;
		case 5:
		{
			GpioDataRegs.GPASET.bit.GPIO8 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPASET.bit.GPIO10 = 1;
		}break;
	}
}

void Inner_ADCScan(void)
{
	int i;
	
	/////////////////////////////////////////////////////
	if(AdcRegs.ADCST.bit.INT_SEQ1)
	{
		AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

		InterADValue[0]  = ((AdcRegs.ADCRESULT0>>4));
		InterADValue[1]  = ((AdcRegs.ADCRESULT1>>4));
		InterADValue[2]  = ((AdcRegs.ADCRESULT2>>4));
		InterADValue[3]  = ((AdcRegs.ADCRESULT3>>4));
		InterADValue[4]  = ((AdcRegs.ADCRESULT4>>4));
		InterADValue[5]  = ((AdcRegs.ADCRESULT5>>4));
		InterADValue[6]  = ((AdcRegs.ADCRESULT6>>4));
		InterADValue[7]  = ((AdcRegs.ADCRESULT7>>4)); //cd4067 sel
		InterADValue[8]  = ((AdcRegs.ADCRESULT8>>4));
		InterADValue[9]  = ((AdcRegs.ADCRESULT9>>4));
		InterADValue[10] = ((AdcRegs.ADCRESULT10>>4));
		InterADValue[11] = ((AdcRegs.ADCRESULT11>>4));
		InterADValue[12] = ((AdcRegs.ADCRESULT12>>4));
		InterADValue[13] = ((AdcRegs.ADCRESULT13>>4));
		InterADValue[14] = ((AdcRegs.ADCRESULT14>>4));
		InterADValue[15] = ((AdcRegs.ADCRESULT15>>4));
		(void)NTC_temp[0];
		AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
		AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
	}
/////////////////////////////////////////////////////
	// CD4067 select adc chx

//	NTC_temp[FPGACD4067_SelFlag] = InterADValue[15];
//	FPGACD4067_SelFlag ++;
//	if(FPGACD4067_SelFlag > 5)
//	{
//		FPGACD4067_SelFlag = 0;
//	}
//
//	CD4067_ChSel(FPGACD4067_SelFlag);

	mpr_temphigh = TemprCalc[0];
	mpr_templow = TemprCalc[0];
	for(i=0; i<6; i++)
	{
		NTC_temp[i] = InterADValue[i];
		NTC_tempf[i] = ((long)NTC_tempf[i]*7 + NTC_temp[i]) >> 3;
		VolSample[i] = Cal_ADReg2Vol(NTC_tempf[i]);
		ResSample[i] = Cal_Vol2R(VolSample[i]);
		TemprCalc[i] = Cal_R2Temp(ResSample[i]);


		if(mpr_temphigh < TemprCalc[i])
			mpr_temphigh = TemprCalc[i];
		if(mpr_templow > TemprCalc[i])
			mpr_templow = TemprCalc[i];
	}
}
/******************************************************************************************/
float Cal_ADReg2Vol(int ADReg)
{
	float Vsamp;
	Vsamp = ADReg *3.0 /4095;
	return Vsamp;
}

float Cal_Vol2R(float Vsamp)
{
	float Rsamp, ftemp1, ftemp2;
	ftemp1 = 38.54 *Vsamp;
	ftemp2 = 41 - 12.9*Vsamp;

	if(ftemp2 > 0)
	{
		Rsamp = fabs(ftemp1/ftemp2);
	}

	if(Rsamp > 200)
		Rsamp = 200;
	if(Rsamp < 0.1)
		Rsamp = 0.1;
	return Rsamp;
}
/*************************************************************************/
float Cal_R2Temp(float R)
{
	float Tempra=0;
	int i;

	const float RTable[14]  = {200.875, 105.656, 58.3594, 33.6641, 20.1875, 12.5391, 8.03910, 5.29687, 3.58594, 2.48438, 1.75781, 1.26563, 0.92969, 0.69531};
	const float KpTable[14] = {-0.1063, -0.2125, -0.4063, -0.7406,  -1.306,  -2.222,  -3.653,  -5.834,  -9.075,  -13.78,  -20.45,  -29.73,  -42.43,  -59.49};
	const float OffTable[14]= {   -8.9,     2.3,    13.6,    24.9,    36.3,    47.8,    59.3,    70.9,    82.5,    94.2,   105.9,   117.7,   129.5,   141.4};

	if(R > RTable[0])
		Tempra = R *KpTable[0] + OffTable[0];
	else if(R < RTable[13])
		Tempra = R*KpTable[13] + OffTable[13];
	else
	{
		for(i=0; i<13; i++)
		{
			if( (R<=RTable[i]) && (R>RTable[i+1]) )
			{
				Tempra = R*KpTable[i] + OffTable[i];
				break;
			}
		}
	}
	return Tempra;
}
/*************************************************************************************/
#endif
