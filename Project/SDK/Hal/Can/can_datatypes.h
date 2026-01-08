// -----------------------------------------------------------------------------
//
// can_datatypes.h - CAN-Related Datatypes
//
// -----------------------------------------------------------------------------

#ifndef __CAN_DATATYPES_H__
#define __CAN_DATATYPES_H__
#include <stdint.h>
#include "stdint_fix.h"
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Datatypes, Definitions & Enumerations (New Driver)

// This value is used to validate DLCs before looping through the Data[] array.
#if BOARD_CANFD_ENABLE
#define CAN_DRIVER_MAX_DLC       (64u)
#else
#define CAN_DRIVER_MAX_DLC       (8u)
#endif
#define CAN_OVER_UDP_MAX_DLC     (4+1+8)
#define CAN_OVER_UDP_MIN_DLC     (4+1+1)
/** Extended length CAN IDs are 29 bits long and thus are stored as 32 bit
 * unsigned ints.  That leaves 3 bits left over to do whatever with.  This
 * CAN driver uses the MSB of the CAN ID to distinguish between standard and
 * extended IDs - the bit is cleared for standard length and set for extended.
 */
#define CAN_DRIVER_EID_FLAG     (0x80000000UL)

// The are standard and extended-length CAN ID masks.
#define CAN_DRIVER_STD_M    (0x7ffUL)
#define CAN_DRIVER_EXT_M    (0x1fffffffUL)

typedef enum
{
    FAIL_CANx_RX_BUFFER_FULL = 0U,
    MESSAGE_RECEIVED,
    MESSAGE_NOT_RECEIVED,
    MESSAGES_AVAILABLE,

} can_rx_result;

typedef enum
{
    TX_OK = 0U,
    FAIL_CANx,
    FAIL_CANx_DISABLED,
    FAIL_CANx_TX_BUFFER_FULL,
    FAIL_NETWORK_MANAGEMENT

} can_tx_result;

typedef enum
{
    eCanFifoReadResult_EmptyAfterRead,
    eCanFifoReadResult_EmptyBeforeRead,
    eCanFifoReadResult_NotEmpty,
} TeCanFifoReadResult;

typedef enum
{
    eCanFifoWriteResult_FullAfterWrite,
    eCanFifoWriteResult_FullBefore_Write,
    eCanFifoWriteResult_NotFull,
} TeCanFifoWriteResult;

typedef enum
{
    eCanPort_0 = 0x00u,
    eCanPort_1,
    eCanPort_Count,
} TeCanPort;

typedef enum
{
    eCanPortSpeed_125kbps,
    eCanPortSpeed_250kbps,
    eCanPortSpeed_500kbps,
    eCanPortSpeed_1000kbps,
} TeCanPortSpeed;

typedef enum
{
    eCanRxResult_Fail_InternalDriverError,
    eCanRxResult_OK_MessageReceived,
    eCanRxResult_OK_MessageNotReceived,
} TeCanRxResult;

typedef enum
{
    eCanTxResult_Fail,
    eCanTxResult_Fail_BufferFull,
    eCanTxResult_Fail_CanDisabled,
    eCanTxResult_Fail_InternalDriverError,
    eCanTxResult_Fail_NetworkManagement,
    eCanTxResult_OK,
} TeCanTxResult;

typedef  struct
{
    uint32_t id;
    uint8_t dlc;
    uint8_t is_can_fd:2;
    uint8_t is_ext_id:2;
    uint8_t data[CAN_DRIVER_MAX_DLC];
} TsCanFrame;

typedef  struct
{
    uint8_t src_bus;
    TsCanFrame canFrame;
} TuCanOverUDPFrame;

//typedef struct
//{
//    uint32_t id;
//    uint8_t dlc;
//    uint8_t is_can_fd:2;
//    uint8_t is_ext_id:2;
//    uint8_t data[CAN_DRIVER_MAX_DLC];
//} __attribute__((packed)) TsCanFrame;

//typedef struct
//{
//    uint8_t src_bus;
//    TsCanFrame canFrame;
//} __attribute__((packed)) TuCanOverUDPFrame;

typedef struct
{
    uint32_t mask;
    uint32_t filter;
} TsCanMaskFilter;

// These typedefs are for use with the CAN ISR callback setter functions.
typedef void (*TfpCanHalCallbackRx)(TeCanPort, uint8_t, const TsCanFrame *);
typedef void (*TfpCanHalCallbackTx)(TeCanPort, uint8_t);

// This typedef is for use with the CAN_Help_SetHookInvMapMsgID() helper function.
typedef uint32_t (*TfpCanFifoHookInvMapMsgID)(uint32_t, TeCanPort);

// This typedef is for use with the CAN_Help_SetHookMapMsgID() helper function.
typedef uint32_t (*TfpCanFifoHookMapMsgID)(uint32_t, TeCanPort);

// This typedef is for use with the CAN_Help_SetHookNMStatus() helper function.
typedef bool (*TfpCanDrvHookNMStatus)(TeCanPort);

// This typedef is for use with the timestamp service setter function.
typedef uint32_t (*TfpCanHalTimestamp)(void);

typedef void (*TfpCanHalCallbackError)(uint8_t);
typedef void (*TfpCanHalCallbackBusOff)(uint8_t);

#endif  /* __CAN_DATATYPES_H__ */
