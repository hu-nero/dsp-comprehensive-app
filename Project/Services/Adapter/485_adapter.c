/*
 * 485_adapter.c
 *
 *  Created on: 2026年3月31日
 *      Author: huxl
 */
#include "485_adapter.h"
#include "Main\Variable.h"

static RemoteHold_t gRemHoldSum,gDcsHoldSum;
static RemoteHold_t gRemSwitchCtrlWord;

uint16_t
UART485_Adapter_GetRemoteSwitchCtrlStatus(UART485_Adapter_SwitchType_t Type)
{
    uint16_t state;
    switch (Type)
    {
        case UART485_ID_REM_SINGLE:
            {
                if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_single_start == 1) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_single_stop == 0))
                {
                    state = 1;
                }
                else if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_single_start == 0) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_single_stop == 1))
                {
                    state = 2;
                }
                else
                {
                    state = 0;
                }
                gRemSwitchCtrlWord.CtrlWord.bit.rem_single_start = 0;
                gRemSwitchCtrlWord.CtrlWord.bit.rem_single_stop = 0;
            }
            break;
        case UART485_ID_REM_PARALLEL:
            {
                if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_start == 1) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_stop == 0))
                {
                    state = 1;
                }
                else if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_start == 0) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_stop == 1))
                {
                    state = 2;
                }
                else
                {
                    state = 0;
                }
                gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_start = 0;
                gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_stop = 0;
            }
            break;
        case UART485_ID_REM_SHORT:
            {
                if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_short_start == 1) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_short_stop == 0))
                {
                    state = 1;
                }
                else if ((gRemSwitchCtrlWord.CtrlWord.bit.rem_short_start == 0) &&
                    (gRemSwitchCtrlWord.CtrlWord.bit.rem_short_stop == 1))
                {
                    state = 2;
                }
                else
                {
                    state = 0;
                }
                gRemSwitchCtrlWord.CtrlWord.bit.rem_short_start = 0;
                gRemSwitchCtrlWord.CtrlWord.bit.rem_short_stop = 0;
            }
            break;
        case UART485_ID_REM_RESET:
            {
                if (gRemSwitchCtrlWord.CtrlWord.bit.Reset == 1)
                {
                    state = 1;
                }
                else if (gRemSwitchCtrlWord.CtrlWord.bit.Reset == 0)
                {
                    state = 2;
                }
                gRemSwitchCtrlWord.CtrlWord.bit.Reset = 0;
            }
            break;
        default:return 0;
    }
    return state;
}

// 适配器初始化
void
UART485_Adapter_Init(void)
{
    // 初始化远控和DCS控制字
    gRemHoldSum.CtrlWord.all = 0;
    gDcsHoldSum.CtrlWord.all = 0;
    gRemSwitchCtrlWord.CtrlWord.all = 0;
}

void
UART485_Adapter_MainLoop(void)
{
    if(gRemHoldSum.CtrlWord.all != RemHoldSum.CtrlWord.all)
    {
        // 取出不同的按钮
        if ((RemHoldSum.CtrlWord.bit.rem_single_start == 1) &&
            (RemHoldSum.CtrlWord.bit.rem_single_stop == 0))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_single_start = 1;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_single_stop = 0;
        }
        else if ((RemHoldSum.CtrlWord.bit.rem_single_start == 0) &&
            (RemHoldSum.CtrlWord.bit.rem_single_stop == 1))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_single_start = 0;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_single_stop = 1;
        }

        if ((RemHoldSum.CtrlWord.bit.rem_parallel_start == 1) &&
            (RemHoldSum.CtrlWord.bit.rem_parallel_stop == 0))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_start = 1;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_stop = 0;
        }
        else if ((RemHoldSum.CtrlWord.bit.rem_parallel_start == 0) &&
            (RemHoldSum.CtrlWord.bit.rem_parallel_stop == 1))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_start = 0;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_parallel_stop = 1;
        }

        if ((RemHoldSum.CtrlWord.bit.rem_short_start == 1) &&
            (RemHoldSum.CtrlWord.bit.rem_short_stop == 0))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_short_start = 1;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_short_stop = 0;
        }
        else if ((RemHoldSum.CtrlWord.bit.rem_short_start == 0) &&
            (RemHoldSum.CtrlWord.bit.rem_short_stop == 1))
        {
            gRemSwitchCtrlWord.CtrlWord.bit.rem_short_start = 0;
            gRemSwitchCtrlWord.CtrlWord.bit.rem_short_stop = 1;
        }

        if (RemHoldSum.CtrlWord.bit.Reset == 1)
        {
            gRemSwitchCtrlWord.CtrlWord.bit.Reset = 1;
        }
        else if (RemHoldSum.CtrlWord.bit.Reset == 0)
        {
            gRemSwitchCtrlWord.CtrlWord.bit.Reset = 0;
        }

        RemHoldSum.CtrlWord.all = gRemHoldSum.CtrlWord.all;
    }
}












