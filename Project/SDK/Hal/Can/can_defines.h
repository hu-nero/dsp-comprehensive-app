// -----------------------------------------------------------------------------
//
// can_defines.h - CAN Driver Definitions
//
// -----------------------------------------------------------------------------

#ifndef __CAN_DEFINES_H__
#define __CAN_DEFINES_H__

#include "basic_types.h"

// -----------------------------------------------------------------------------
#define CAN_NUMBER_OF_PORTS                      (2u)
// This is the index of the mailbox used for transmission.
#define CAN_TX_STD_MAILBOX_INDEX                 (0u)
#define CAN_RX_STD_MAILBOX_INDEX                 (4u)
#define CAN_TX_EXT_MAILBOX_INDEX                 (5u)
#define CAN_RX_EXT_MAILBOX_INDEX                 (6u)

#define CAN_BUFFER_TX_DEFAULT_LENGTH             (20u)
// This is the maximum number of TX callbacks that are supported by the HAL.
#define CAN_NO_OF_TX_CALLBACKS                   (1u)
#define CAN_BUFFER_TX_LENGTH                     CAN_BUFFER_TX_DEFAULT_LENGTH

#endif  /* __CAN_DEFINES_H__ */
