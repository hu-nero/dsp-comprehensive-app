/*
 * hal_can.h
 *
 *  Created on: 2025年12月16日
 *      Author: huxl
 */

#ifndef SDK_HAL_CAN_HAL_CAN_H_
#define SDK_HAL_CAN_HAL_CAN_H_

#include "stdint.h"
#include "stdint_fix.h"
#include "CanDrive.h"
#include "can_datatypes.h"
#include "can_defines.h"
#include "can_fifo.h"


#define CAN_DRIVER_MAX_DLC       (8u)
#define CAN_DRIVER_RX_NUM        (20u)


typedef enum {                         /*!< Type specifying the CAN frame type */
    HAL_CAN_DATA_FRAME,                  /*!< Data frame type received or transmitted */
    HAL_CAN_REMOTE_FRAME,                /*!< Remote frame type  */
    HAL_CAN_RESPONSE_FRAME               /*!< Response frame type - Tx buffer send data after receiving remote frame with the same ID */
} HAL_CAN_TFrameType;

typedef enum{
    HAL_CAN_ER_ERROR,
    HAL_CAN_ER_BUSOFF
} HAL_CAN_ER_TYPE;

typedef struct{
    uint8_t u8IdxHead;
    uint8_t u8IdxTail;
    uint32_t u32CanRxCount;
    S_CANMSG sCanRxTFrame[CAN_DRIVER_RX_NUM];
} HAL_CAN_Rx_Struct;

extern TfpCanHalCallbackTx SpCAN_CallbackTx[eCanPort_Count];
extern uint32_t gHalCanBaudValue;

extern uint16_t hal_can_init(uint8_t PortId, TeCanPortSpeed Baud);
extern uint16_t hal_can_deinit(uint8_t PortId);
extern TeErrorEnum hal_can_send(uint8_t PortId, uint8_t Mb, TsCanFrame* Frame);

extern TsCanFrame* hal_can_rx_queue_de(uint8_t PortId);
extern void hal_can_tx_callback_set(uint8_t PortId, TfpCanHalCallbackTx TxFunc);
extern void hal_can_tx_callback(uint8_t PortId, uint8_t Mb);
extern void hal_can_rx_callback(uint8_t PortId, uint8_t Mb);
extern void hal_can_error_callback(uint8_t PortId, HAL_CAN_ER_TYPE u8CanErrorType);





#endif /* SDK_HAL_CAN_HAL_CAN_H_ */
