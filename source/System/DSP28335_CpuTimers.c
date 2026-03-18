// TI File $Revision: /main/4 $
// Checkin $Date: July 9, 2009   10:51:59 $
//###########################################################################
//
// FILE:    DSP2833x_CpuTimers.c
//
// TITLE:   CPU 32-bit Timers Initialization & Support Functions.
//
// NOTES:   CpuTimer2 is reserved for use with DSP BIOS and
//          other realtime operating systems.
//
//          Do not use these this timer in your application if you ever plan
//          on integrating DSP-BIOS or another realtime OS.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File

struct CPUTIMER_VARS CpuTimer0;

// When using DSP BIOS & other RTOS, comment out CPU Timer 2 code.
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;

//---------------------------------------------------------------------------
// InitCpuTimers:
//---------------------------------------------------------------------------
// This function initializes all three CPU timers to a known state.
//
void InitCpuTimers(void)
{
   CpuTimer0.RegsAddr=&CpuTimer0Regs;
   CpuTimer0Regs.PRD.all  = 1000;//0x0FFFFFFFF;
   CpuTimer0Regs.TPR.all  = 0x9595;//0x0F495;//150分频500us周期
   CpuTimer0Regs.TPRH.all = 0;//0x0100;
   // Make sure timers are stopped:
   CpuTimer0Regs.TCR.bit.TSS = 1;

   // Reload all counter register with period value:
    
    CpuTimer0Regs.TCR.bit.TRB = 1;
	CpuTimer0Regs.TCR.bit.FREE=1;
    CpuTimer0Regs.TCR.bit.SOFT=1;

	 // Reset interrupt counters:
   
    CpuTimer0.InterruptCount = 0;

	CpuTimer0Regs.TCR.bit.TIF=1;
	CpuTimer0Regs.TCR.bit.TIE=0;  //enable TIMER0中断

    CpuTimer0Regs.TCR.bit.TSS = 0;  //start TIMER0


// CpuTimer2 is reserved for DSP BIOS & other RTOS
// Do not use this timer if you ever plan on integrating
// DSP-BIOS or another realtime OS.

	CpuTimer1.RegsAddr = &CpuTimer1Regs;
	// Initialize timer period to maximum:
	CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer1Regs.TPR.all  = 0;
	CpuTimer1Regs.TPRH.all = 0;
    // Make sure timers are stopped:
	CpuTimer1Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer1Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer1.InterruptCount = 0;
    

    //CpuTimer2Regs.PRD.all  = 0x0BFFB895;//150分频，3ms周期
	CpuTimer2Regs.PRD.all  = 0x0BB8;
	CpuTimer2Regs.TPR.all  = 0x9595;//150分频，3ms周期
    CpuTimer2Regs.TPRH.all = 0x0000;
    // Make sure timers are stopped:
    
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
   
    CpuTimer2.InterruptCount = 0;

	
	CpuTimer2Regs.TCR.bit.TIF=1;
	CpuTimer2Regs.TCR.bit.TIE=0;
    CpuTimer2Regs.TCR.bit.TSS = 0;  //start TIMER2
	//COMCONA_MARK=0X8200;baijj8-5-21:11
	//EALLOW;12-2 baijj
    // SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

}

//---------------------------------------------------------------------------
// ConfigCpuTimer:
//---------------------------------------------------------------------------
// This function initializes the selected timer to the period specified
// by the "Freq" and "Period" parameters. The "Freq" is entered as "MHz"
// and the period in "uSeconds". The timer is held in the stopped state
// after configuration.
//
void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq, float Period)
{
	Uint32 	temp;

	// Initialize timer period:
	Timer->CPUFreqInMHz = Freq;
	Timer->PeriodInUSec = Period;
	temp = (long) (Freq * Period);
	Timer->RegsAddr->PRD.all = temp;

	// Set pre-scale counter to divide by 1 (SYSCLKOUT):
	Timer->RegsAddr->TPR.all  = 0;
	Timer->RegsAddr->TPRH.all  = 0;

	// Initialize timer control register:
	Timer->RegsAddr->TCR.bit.TSS = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
	Timer->RegsAddr->TCR.bit.TRB = 1;      // 1 = reload timer
	Timer->RegsAddr->TCR.bit.SOFT = 0;
	Timer->RegsAddr->TCR.bit.FREE = 0;     // Timer Free Run Disabled
	Timer->RegsAddr->TCR.bit.TIE = 1;      // 0 = Disable/ 1 = Enable Timer Interrupt

	// Reset interrupt counter:
	Timer->InterruptCount = 0;
}


//===========================================================================
// End of file.
//===========================================================================
