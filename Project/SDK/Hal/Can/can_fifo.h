// -----------------------------------------------------------------------------
//
// can_fifo.h - CAN FIFO Buffer Management Functions
//
// -----------------------------------------------------------------------------

#ifndef __CAN_FIFO_H__
#define __CAN_FIFO_H__

#include "can_datatypes.h"
#include "can_defines.h"

// -----------------------------------------------------------------------------
// Function Prototypes

uint8_t CAN_FIFO_GetHighWaterMarkTx(TeCanPort port);

void CAN_FIFO_Init(TeCanPort port);
void CAN_FIFO_InitCallbacks(TeCanPort port);

can_tx_result CAN_FIFO_Write(TeCanPort port, const TsCanFrame* frame);

TeErrorEnum CAN_InitWrite(TeCanPort port);

#endif  /* __CAN_FIFO_H__ */
