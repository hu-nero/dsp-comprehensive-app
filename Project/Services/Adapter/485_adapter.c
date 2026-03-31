/*
 * 485_adapter.c
 *
 *  Created on: 2026年3月31日
 *      Author: huxl
 */
#include "485_adapter.h"
#include "main\Variable.h"

static RemoteHold_t gRemHoldSum,gDcsHoldSum;


//// 适配器初始化
//void
//485_Adapter_Init(void)
//{
//    // 初始化远控和DCS控制字
//	gRemHoldSum.CtrlWord.all = 0;
//	gDcsHoldSum.CtrlWord.all = 0;
//}
//
//void
//485_Adapter_MainLoop(void)
//{
//	if(gRemHoldSum != RemHoldSum)
//	{
//		gRemHoldSum = RemHoldSum;
//
//	}
//}



