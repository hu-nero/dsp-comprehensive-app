/*
 * hal_wdg.c
 *
 *  Created on: 2026年2月2日
 *      Author: huxl
 */
#include "hal_wdg.h"
#include "stdint_fix.h"
#include "string.h"
#include "DSP2833x_Device.h"
#include "System\Custom_System.H"

halWdgCallbackFunc ghalWdgCallback = NULL;
static volatile uint32_t gu32WdgCounter = 0;

/**
 * @brief 看门狗初始化函数
 */
void
hal_wdg_init(void)
{
	hal_wdg_enable();
}

/**
 * @brief 看门狗使能函数
 */
void
hal_wdg_enable(void)
{
    EALLOW;
    // 设置SCSR寄存器
    SysCtrlRegs.SCSR = 0x0002;
    // 设置WDCR寄存器启用看门狗
    EnableDog();

    EALLOW;
    PieVectTable.WAKEINT = &WDG_ISR;
    EDIS;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER1.bit.INTx8 = 1;   // Enable PIE Gropu 1 INT8
	IER |= M_INT1;                       // Enable CPU int1
	EINT;                                // Enable Global Interrupts
}

/**
 * @brief 看门狗禁用函数
 */
void
hal_wdg_disable(void)
{
    // 使用与SysCtrl.c中DisableDog函数相同的配置
    DisableDog();
}

/**
 * @brief 看门狗喂狗函数
 */
void
hal_wdg_feed(void)
{
    // 重置看门狗计数器
    ServiceDog();
}

/**
 * @brief :set timer0 callback
 */
void
hal_wdg_callback_set(halWdgCallbackFunc Func)
{
	ghalWdgCallback = Func;
}
/**
 * @brief :timer0 callback
 */
void
hal_wdg_callback(void)
{
	gu32WdgCounter++;
	if (gu32WdgCounter >= 7)
	{
		gu32WdgCounter = 0;
		//复位
		// TODO:复位
	}
}
