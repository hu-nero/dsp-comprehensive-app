/*
 * 485_adapter.h
 *
 *  Created on: 2026年3月31日
 *      Author: huxl
 */

#ifndef SERVICES_ADAPTER_485_ADAPTER_H_
#define SERVICES_ADAPTER_485_ADAPTER_H_


#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    UART485_ID_REM_SINGLE,    // 远控单机启动
    UART485_ID_REM_PARALLEL,  // 远控并机启动
    UART485_ID_REM_SHORT,     // 远控消磁启动
    UART485_ID_REM_RESET,     // 远控消磁停止
    UART485_ID_GRID,          // 远控一键解列
} UART485_Adapter_SwitchType_t;


extern void UART485_Adapter_Init(void);
extern uint16_t UART485_Adapter_GetRemoteSwitchCtrlStatus(UART485_Adapter_SwitchType_t Type);
extern uint16_t UART485_Adapter_GetDcsSwitchCtrlStatus(UART485_Adapter_SwitchType_t Type);
void UART485_Adapter_MainLoop(void);

#endif /* SERVICES_ADAPTER_485_ADAPTER_H_ */







