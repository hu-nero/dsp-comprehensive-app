/*
 * hal_can.c
 *
 *  Created on: 2025年12月16日
 *      Author: huxl
 */
#include "hal_can.h"
#include "string.h"

static Can_TypeDef *gHalCanDevPtr[eCanPort_Count] = {NULL,NULL};
static Can_TypeDef gHalCanDevStruct[eCanPort_Count];
static uint16_t canRxBuf[eCanPort_Count][CAN_DRIVER_RX_NUM][CAN_DRIVER_MAX_DLC];
static HAL_CAN_Rx_Struct gHalCanRxStruct[eCanPort_Count];
uint32_t gHalCanErrorCount = 0;
uint32_t gHalCanBusoffCount = 0;

//callback func
TfpCanHalCallbackTx SpCAN_CallbackTx[eCanPort_Count] = {NULL};

uint16_t
hal_can_init(uint8_t PortId, TeCanPortSpeed Baud)
{
	int i;
    uint32_t u32CanBaudValue = 500;
    if (PortId >= eCanPort_Count)
    {
        return 1;
    }
    // init fifo
    CAN_FIFO_Init((TeCanPort)PortId);
    CAN_FIFO_InitCallbacks((TeCanPort)PortId);
    CAN_InitWrite((TeCanPort)PortId);

    // init can struct
    gHalCanDevPtr[PortId] = &gHalCanDevStruct[PortId];

    switch(PortId)
    {
        case eCanPort_0:
            {
                gHalCanDevPtr[eCanPort_0]->MsgChn = CANCHNA;
                //init can device
            	switch (Baud)
            	{
                    case eCanPortSpeed_125kbps:
                        {
                            u32CanBaudValue = 125;
                        }
                        break;
                    case eCanPortSpeed_250kbps:
                        {
                            u32CanBaudValue = 250;
                        }
                        break;
                    case eCanPortSpeed_500kbps:
                        {
                            u32CanBaudValue = 500;
                        }
                        break;
                    case eCanPortSpeed_1000kbps:
                        {
                            u32CanBaudValue = 1000;
                        }
                        break;
                    default:
                    	{
                            u32CanBaudValue = 500;
                    	}
                		break;
            	}
                InitCanDrive(gHalCanDevPtr[eCanPort_0], u32CanBaudValue);
            }
            break;
        case eCanPort_1:
            {
                gHalCanDevPtr[eCanPort_1]->MsgChn = CANCHNB;
                //init can device
                switch (Baud)
                {
                    case eCanPortSpeed_125kbps:
                        {
                            u32CanBaudValue = 125;
                        }
                        break;
                    case eCanPortSpeed_250kbps:
                        {
                            u32CanBaudValue = 250;
                        }
                        break;
                    case eCanPortSpeed_500kbps:
                        {
                            u32CanBaudValue = 500;
                        }
                        break;
                    case eCanPortSpeed_1000kbps:
                        {
                            u32CanBaudValue = 1000;
                        }
                        break;
                    default:
                        {
                            u32CanBaudValue = 500;
                        }
                        break;
                }
                InitCanDrive(gHalCanDevPtr[eCanPort_1], u32CanBaudValue);
            }
            break;
        default:return 3;
    }

    gHalCanRxStruct[PortId].u8IdxHead = 0;
    gHalCanRxStruct[PortId].u8IdxTail = 0;
    gHalCanRxStruct[PortId].u32CanRxCount = 0;
    memset(gHalCanRxStruct[PortId].sCanRxTFrame, 0, sizeof(gHalCanRxStruct[PortId].sCanRxTFrame));
    //init can TxMsg
    for(i=0; i<CAN_DRIVER_RX_NUM; i++)
    {
        gHalCanRxStruct[PortId].sCanRxTFrame[i].data = canRxBuf[PortId][i];
    }


    return 0;
}

uint16_t
hal_can_deinit(uint8_t PortId)
{
    switch(PortId)
    {
        case eCanPort_0:
            {
                //deinit can device
                DeinitCanDrive(gHalCanDevPtr[eCanPort_0]);
                // 重置状态变量
                gHalCanRxStruct[eCanPort_0].u8IdxHead = 0;
                gHalCanRxStruct[eCanPort_0].u8IdxTail = 0;
                gHalCanRxStruct[eCanPort_0].u32CanRxCount = 0;

                // 重置全局错误计数
                gHalCanErrorCount = 0;
                gHalCanBusoffCount = 0;
            }
            break;
        case eCanPort_1:
            {
                //deinit can device
                DeinitCanDrive(gHalCanDevPtr[eCanPort_1]);
                // 重置状态变量
                gHalCanRxStruct[eCanPort_1].u8IdxHead = 0;
                gHalCanRxStruct[eCanPort_1].u8IdxTail = 0;
                gHalCanRxStruct[eCanPort_1].u32CanRxCount = 0;

                // 重置全局错误计数
                gHalCanErrorCount = 0;
                gHalCanBusoffCount = 0;
            }
            break;
        default:return 1;
    }
    return 0;
}

TeErrorEnum
hal_can_send(uint8_t PortId, uint8_t Mb, TsCanFrame* Frame)
{
	S_CANMSG sCanMsg = {0};

	if (Frame->is_ext_id)
	{
	    sCanMsg.rid.all = (Frame->id) | CAN_DRIVER_EID_FLAG;
	}
	else
	{
	    sCanMsg.rid.all = Frame->id;
	}
    //sCanMsg.FrameType = HAL_CAN_DATA_FRAME;
    if (Frame->is_can_fd)
    {
        //TODO:28335 series not support
    }
    else
    {
        sCanMsg.dlc = Frame->dlc <= 8 ? Frame->dlc : 8;
    }
    memcpy(sCanMsg.data, Frame->data, sCanMsg.dlc);
    CanSendDrive(gHalCanDevPtr[PortId], Mb, &sCanMsg);

    return eErrorOk;
}

TsCanFrame*
hal_can_rx_queue_de(uint8_t PortId)
{
    static TsCanFrame halCanFrameTmp = {};

    if (PortId >= eCanPort_Count)
    {
        return NULL;
    }

    ENTER_CRITICAL();
    //is empty
    if (gHalCanRxStruct[PortId].u32CanRxCount == 0)
    {
        EXIT_CRITICAL();
        return NULL;
    }
    //copy data
    memcpy(&halCanFrameTmp.data, gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxHead].data, CAN_DRIVER_MAX_DLC);
    halCanFrameTmp.dlc = gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxHead].dlc;
    halCanFrameTmp.is_ext_id =(gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxHead].rid.all & CAN_DRIVER_EID_FLAG)!=0 ? 1 : 0;
    if (halCanFrameTmp.is_ext_id)
    {
        halCanFrameTmp.id = gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxHead].rid.all & CAN_DRIVER_EXT_M;
    }
    else
    {
        halCanFrameTmp.id = gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxHead].rid.all & CAN_DRIVER_STD_M;
    }
    //涓嶉傜敤CANFD
    halCanFrameTmp.is_can_fd = 0;
    //dequeue
    gHalCanRxStruct[PortId].u8IdxHead = (gHalCanRxStruct[PortId].u8IdxHead + 1)%CAN_DRIVER_RX_NUM;
    gHalCanRxStruct[PortId].u32CanRxCount--;

    EXIT_CRITICAL();
    return &halCanFrameTmp;
}

void
hal_can_tx_callback_set(uint8_t PortId, TfpCanHalCallbackTx Func)
{
    SpCAN_CallbackTx[PortId] = Func;
}

void
hal_can_tx_callback(uint8_t PortId, uint8_t Mb)
{
    (*SpCAN_CallbackTx[(TeCanPort)PortId])((TeCanPort)PortId, Mb);
}


void
hal_can_rx_callback(uint8_t PortId, uint8_t Mb)
{
    if(gHalCanRxStruct[PortId].u32CanRxCount >= CAN_DRIVER_RX_NUM)
    {
        gHalCanErrorCount++;  // 溢出错误
        return;
    }
    CanRecvDrive(gHalCanDevPtr[PortId], Mb, &gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxTail]);
    if(gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxTail].dlc > CAN_DRIVER_MAX_DLC)
    {
        gHalCanRxStruct[PortId].sCanRxTFrame[gHalCanRxStruct[PortId].u8IdxTail].dlc = CAN_DRIVER_MAX_DLC;
    }
    gHalCanRxStruct[PortId].u8IdxTail = (gHalCanRxStruct[PortId].u8IdxTail + 1)%CAN_DRIVER_RX_NUM;
    gHalCanRxStruct[PortId].u32CanRxCount++;
}

void
hal_can_error_callback(uint8_t PortId, HAL_CAN_ER_TYPE u8CanErrorType)
{
    switch(u8CanErrorType)
    {
        case HAL_CAN_ER_ERROR:
            {
                //CAN_GetError(gHalCanDevPtr[PortId], &u32ErrorMask);
                gHalCanErrorCount ++;
            }
            break;
        case HAL_CAN_ER_BUSOFF:
            {
                gHalCanBusoffCount++;
                //TODO:判断标志，并在Main中调用以下进行can重启
                hal_can_deinit(PortId);
                hal_can_init(PortId, eCanPortSpeed_500kbps);
            }
            break;
        default:break;
    }
}
