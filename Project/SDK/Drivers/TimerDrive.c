/*
 * TimerDrive.c
 *
 *  Created on: 2025年12月27日
 *      Author: huxl
 */

#include "TimerDrive.h"



void TIM0_Init(float Freq, float Period)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
	EDIS;

	EALLOW;
	PieVectTable.TINT0 = &TIM0_ISR;
	EDIS;

	// CPU Timer 0
	// Initialize address pointers to respective timer registers:
	CpuTimer0.RegsAddr = &CpuTimer0Regs;
	// Initialize timer period to maximum:
	CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer0Regs.TPR.all  = 0;
	CpuTimer0Regs.TPRH.all = 0;
	// Make sure timer is stopped:
	CpuTimer0Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer0Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer0.InterruptCount = 0;

	ConfigCpuTimer(&CpuTimer0, Freq, Period);

	CpuTimer0Regs.TCR.bit.TSS=0;

	IER |= M_INT1;

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	EINT;
	ERTM;

}

void TIM0_DeInit(void)
{
    // 停止定时器
    CpuTimer0Regs.TCR.bit.TSS = 1;
    
    // 禁用中断
    IER &= ~M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 0;
    
    // 禁用时钟
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 0;
    EDIS;
}


