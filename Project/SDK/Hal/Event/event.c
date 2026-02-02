/*
 * event.c
 *
 *  Created on: 2025年12月17日
 *      Author: huxl
 */
#include "event.h"

interrupt void
CANA_TxRx_ISR(void)
{
    // 接收中断标志
	if(ECanaRegs.CANGIF0.bit.GMIF0 == 1)
	{
		uint32_t miv = ECanaRegs.CANGIF0.bit.MIV0;
        switch (miv)
        {
			case 0:  // 发送
            {
				hal_can_tx_callback(0, miv);
                EALLOW;
                ECanaRegs.CANTA.bit.TA0 = 1;  // 写1清除TA0标志
                EDIS;
            }
            break;
            case 4:  // 邮箱4接收中断
			{
				hal_can_rx_callback(0, miv);
				EALLOW;
				ECanaRegs.CANRMP.bit.RMP4 = 1;  // 写1清除RMP4标志
				EDIS;
			}
			break;
            default:
                break;
        }
    }
    
    // TODO:总线关闭、错误被动等

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}


interrupt void
CANA_Error_ISR(void)
{
	// 处理总线关闭中断
	if(ECanaRegs.CANGIF1.bit.BOIF1 == 1)
    {
        hal_can_error_callback(0, HAL_CAN_ER_BUSOFF);
		EALLOW;
		ECanaRegs.CANGIF1.bit.BOIF1 = 1;  // 写1清除标志
		EDIS;
	}

	// 处理错误被动中断
	if(ECanaRegs.CANGIF1.bit.EPIF1 == 1)
	{
        hal_can_error_callback(0, HAL_CAN_ER_ERROR);
		EALLOW;
		ECanaRegs.CANGIF1.bit.EPIF1 = 1;  // 写1清除标志
		EDIS;
	}



	// 清除PIE中断标志
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void
CANB_TxRx_ISR(void)
{
    // 接收中断标志
	if(ECanbRegs.CANGIF0.bit.GMIF0 == 1)
	{
		uint32_t miv = ECanbRegs.CANGIF0.bit.MIV0;
        switch (miv)
        {
			case 0:  // 发送
            {
				hal_can_tx_callback(1, miv);
                EALLOW;
                ECanbRegs.CANTA.bit.TA0 = 1;  // 写1清除TA0标志
                EDIS;
            }
            break;
            case 4:  // 邮箱4接收中断
			{
				hal_can_rx_callback(1, miv);
				EALLOW;
				ECanbRegs.CANRMP.bit.RMP4 = 1;  // 写1清除RMP4标志
				EDIS;
			}
			break;
            default:
                break;
        }
    }
    
    // TODO:总线关闭、错误被动等

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void
CANB_Error_ISR(void)
{
	// 处理总线关闭中断
	if(ECanbRegs.CANGIF1.bit.BOIF1 == 1)
    {
        hal_can_error_callback(1, HAL_CAN_ER_BUSOFF);
		EALLOW;
		ECanbRegs.CANGIF1.bit.BOIF1 = 1;  // 写1清除标志
		EDIS;
	}

	// 处理错误被动中断
	if(ECanbRegs.CANGIF1.bit.EPIF1 == 1)
	{
        hal_can_error_callback(1, HAL_CAN_ER_ERROR);
		EALLOW;
		ECanbRegs.CANGIF1.bit.EPIF1 = 1;  // 写1清除标志
		EDIS;
	}

	// 清除PIE中断标志
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void
TIM0_ISR(void)
{
    hal_timer0_callback();
	EALLOW;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	EDIS;

}

interrupt void
WDG_ISR(void)
{
	hal_wdg_callback();
	EALLOW;
	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	EDIS;
}
