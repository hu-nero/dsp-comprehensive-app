// -----------------------------------------------------------------------------
//
// can_fifo.c - CAN FIFO Buffer Management Functions
//
// -----------------------------------------------------------------------------

#include "can_fifo.h"
#include "can_datatypes.h"
#include "basic_types.h"
#include "can_defines.h"
#include "string.h"

#include "hal_can.h"

// These are the port semaphores.
static bool SbTxPortAvailable[eCanPort_Count];
static TsCanFrame GsCANx_BufferTx[eCanPort_Count][CAN_BUFFER_TX_LENGTH];

// These variables are similar to the ones explained above.  There's only one
// TX FIFO per port so the arrays are 1-dimensional.
static TsCanFrame* SsCAN_TxBuffer[eCanPort_Count];
static uint8_t Su8CAN_TxHead[eCanPort_Count];
static uint8_t Su8CAN_TxTail[eCanPort_Count];
static uint8_t Su8CAN_TxSize[eCanPort_Count];
static uint8_t Su8CAN_TxMaxSize[eCanPort_Count];
static uint8_t Su8CAN_TxLength[eCanPort_Count];

// -----------------------------------------------------------------------------
// Static Function Prototypes
static void Callback_Tx(TeCanPort port, uint8_t mb);

static TeCanFifoWriteResult Write_Buffer(const TsCanFrame* frame,
        TsCanFrame* buffer, uint8_t* head, uint8_t* size, uint8_t* max_size,
        uint8_t length);

// -----------------------------------------------------------------------------


static void Callback_Tx(TeCanPort port, uint8_t mb)
{
    // If the buffer is not NULL...
    if (SsCAN_TxBuffer[port] != NULL)
    {
        // If the buffer has something in it...
        if (Su8CAN_TxSize[port] > 0u)
        {
            // Mark the port as unavailable for transmission.
            SbTxPortAvailable[port] = false;

            // Transmit the frame using the CAN hardware abstraction layer.
            (void) hal_can_send(port, mb,
                    &SsCAN_TxBuffer[port][Su8CAN_TxTail[port]]);

            // Decrement the number of messages in the buffer.
            Su8CAN_TxSize[port]--;

            // Increment the tail pointer.
            Su8CAN_TxTail[port]++;

            // If the tail has moved past the end of the buffer, wrap it around.
            if (!(Su8CAN_TxTail[port] < Su8CAN_TxLength[port]))
            {
                Su8CAN_TxTail[port] = 0u;
            }
        }

        // The buffer has nothing in it.
        else
        {
            // The port is available again.
            SbTxPortAvailable[port] = true;
        }
    }
}

/** Write CAN frame data to a FIFO buffer.
 *
 * @param[in]   frame   A pointer to the target CAN frame data structure.
 * @param[in]   buffer  A pointer to the FIFO buffer to be read
 * @param[in]   head    A pointer to the FIFO buffer head value.
 * @param[in]   size    A pointer to the FIFO buffer size value.
 * @param[in]   max_size    A pointer to the FIFO buffer high-water mark.
 * @param[in]   length  The static length of the FIFO buffer.
 *
 * @return      FIFO buffer status.
 */
static TeCanFifoWriteResult Write_Buffer(const TsCanFrame* frame,
        TsCanFrame* buffer, uint8_t* head, uint8_t* size, uint8_t* max_size,
        uint8_t length)
{
    TeCanFifoWriteResult ret = eCanFifoWriteResult_NotFull;

    ENTER_CRITICAL();
    // If none of the pointers are NULL...
    if ((frame != NULL) && (buffer != NULL) && (head != NULL) && (size != NULL)
            && (max_size != NULL))
    {
        // If the buffer is not full...
        if ((*size) < length)
        {
            // Copy to the message at the buffer's head pointer.
            memcpy(&buffer[(*head)], frame, sizeof(TsCanFrame));

            // Increment the number of messages in the buffer.
            (*size)++;

            // Increment the head pointer.
            (*head)++;

            // Set the high-water mark (all-time highest number of messages).
            if ((*max_size) < (*size))
            {
                (*max_size) = (*size);
            }

            // If the head has moved past the end of the buffer, wrap it around.
            if (!((*head) < length))
            {
                (*head) = 0u;
            }

            // If the buffer is full...
            if (!((*size) < length))
            {
                ret = eCanFifoWriteResult_FullAfterWrite;
            }
        }

        // The buffer is full.
        else
        {
            ret = eCanFifoWriteResult_FullBefore_Write;
            //LOG(eLOG_LEVEL_WARN,"fifo is full");
        }
    }
    EXIT_CRITICAL();  // 退出临界区

    return ret;
}

/**
 * Return the high-water mark for the TX FIFO buffer.
 *
 * @param[in]   port    The enumeration of the port that services the FIFO
 *                      buffer.
 *
 * @return      the highest number of messages that were ever backed up within
 *              the buffer.
 */
uint8_t CAN_FIFO_GetHighWaterMarkTx(TeCanPort port)
{
    uint8_t ret = 0u;

    // If the port is valid...
    if (port < eCanPort_Count)
    {
        ret = Su8CAN_TxMaxSize[port];
    }

    return ret;
}

/**
 * Initialize internal buffers and variables.
 */
void CAN_FIFO_Init(TeCanPort port)
{
    if (port >= eCanPort_Count)
    {
        return;
    }
    (void) memset(&SsCAN_TxBuffer[port], 0u, sizeof(TsCanFrame*));
    (void) memset(&Su8CAN_TxHead[port], 0u, 1);
    (void) memset(&Su8CAN_TxTail[port], 0u, 1);
    (void) memset(&Su8CAN_TxSize[port], 0u, 1);
    (void) memset(&Su8CAN_TxMaxSize[port], 0u, 1);
    (void) memset(&Su8CAN_TxLength[port], 0u, 1);
    SbTxPortAvailable[port] = true;
}

/**
 * Initialize RX/TX callback functions.
 */
void CAN_FIFO_InitCallbacks(TeCanPort port)
{
    if (port >= eCanPort_Count)
    {
        return;
    }
    (void) hal_can_tx_callback_set(port, Callback_Tx);
}

/**
 * Register a TX FIFO buffer.
 *
 * @details     This function works just like CAN_FIFO_SetupRx(), but without
 *              mailbox initialization.
 *
 * @param[in]   port    The enumeration of the port that services the FIFO
 *                      buffer.
 * @param[in]   b       A pointer to the FIFO buffer / CAN message array.
 * @param[in]   buffer_id    The buffer unique ID.
 * @param[in]   b_len   The buffer's length.
 *
 * @return      "eErrorOk" if buffer registration was successful;
 *              "eErrorOutOfRange" if the port is out of range;
 *              "eErrorNotOk" if this block of mailboxes is already setup.
 */
TeErrorEnum CAN_FIFO_SetupTx(TeCanPort port, TsCanFrame* buffer, uint32_t buffer_len)
{
    TeErrorEnum ret = eErrorOk;

    // Check for a valid port index.
    if (port >= eCanPort_Count)
    {
        return eErrorOutOfRange;
    }

    // If a buffer is already set up, don't do it again.
    if (SsCAN_TxBuffer[port] != NULL)
    {
        ret = eErrorNotOk;
    }

    // If everything above checks out OK...
    if (ret == eErrorOk)
    {
        // Store TX FIFO buffer info.
        SsCAN_TxBuffer[port] = buffer;
        Su8CAN_TxLength[port] = buffer_len;
    }

    return ret;
}

/** Configure a single mailbox for transmission.
 *
 * @param[in]   port    The enumeration of the port that services the FIFO
 *                      buffer.
 *
 * @return      "eErrorOk" if all mailboxes were successfully configured;
 *              "eErrorNotOk" otherwise.
 */
TeErrorEnum CAN_InitWrite(TeCanPort port)
{
    TeErrorEnum ret = eErrorNotOk;

    ret = CAN_FIFO_SetupTx(port, GsCANx_BufferTx[port],
                    CAN_BUFFER_TX_LENGTH);
    return ret;
}

/**
 * Write a message to the TX FIFO buffer.
 *
 * @param[in]   port    The enumeration of the port that services the FIFO
 *                      buffer.
 * @param[out]  frame   A pointer to the source CAN frame data structure.
 *
 * @return      CAN transmit status.
 */
can_tx_result CAN_FIFO_Write(TeCanPort port, const TsCanFrame* frame)
{
    TeCanFifoWriteResult fifo_res;
    can_tx_result tx_res =  TX_OK;
    TeErrorEnum hal_res;

    TsCanFrame temp_frame;

    // If the port and frame pointer are valid...
    if ((port < eCanPort_Count) && (frame != NULL))
    {
        // Make temporary copy of frame
        memcpy(&temp_frame, frame, sizeof(TsCanFrame));

        // If the port is available for transmission...
        if (SbTxPortAvailable[port] == true)
        {
            // Mark the port as not available.
            SbTxPortAvailable[port] = false;

            // Use the HAL to transmit a frame.
            hal_res = hal_can_send(port, CAN_TX_STD_MAILBOX_INDEX,
                    &temp_frame);

            if (hal_res == eErrorOk)
            {
                tx_res = TX_OK;
            }else
            {
                SbTxPortAvailable[port] = true;  // 释放端口
                fifo_res = Write_Buffer(&temp_frame, SsCAN_TxBuffer[port],
                        &Su8CAN_TxHead[port], &Su8CAN_TxSize[port],
                        &Su8CAN_TxMaxSize[port], Su8CAN_TxLength[port]);

                if ((SbTxPortAvailable[port] == true)&&(SpCAN_CallbackTx[port] != NULL))
                {
                    (*SpCAN_CallbackTx[port])(port, CAN_TX_STD_MAILBOX_INDEX);
                }
            }
        }
		// The port isn't available, so use the FIFO buffer instead.
        else
        {
            fifo_res = Write_Buffer(&temp_frame, SsCAN_TxBuffer[port],
                    &Su8CAN_TxHead[port], &Su8CAN_TxSize[port],
                    &Su8CAN_TxMaxSize[port], Su8CAN_TxLength[port]);

            switch (fifo_res)
            {
            case eCanFifoWriteResult_FullBefore_Write:
                tx_res = FAIL_CANx_TX_BUFFER_FULL;
                break;

            case eCanFifoWriteResult_FullAfterWrite:
            case eCanFifoWriteResult_NotFull:
            default:
                tx_res = TX_OK;
                break;
            }
        }
    }
    else
    {
        tx_res = FAIL_CANx;
    }

    return tx_res;
}
