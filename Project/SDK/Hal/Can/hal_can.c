/*
 * hal_can.c
 *
 *  Created on: 2025年12月16日
 *      Author: huxl
 */
#include "hal_can.h"
#include "string.h"

static Can_TypeDef *halCanDevicePtr = NULL;
static uint16_t canRxBuf[CAN_DRIVER_RX_NUM][CAN_DRIVER_MAX_DLC];
static HAL_CAN_Rx_Struct gHalCanRxStruct;
uint32_t gHalCanBaudValue = 500;
uint32_t gHalCanErrorCount = 0;
uint32_t gHalCanBusoffCount = 0;

//callback func
TfpCanHalCallbackTx SpCAN_CallbackTx[eCanPort_Count] = {NULL};

uint16_t
hal_can_init(uint8_t PortId, TeCanPortSpeed Baud)
{
	int i;
    if (PortId >= eCanPort_Count)
    {
        return 1;
    }
    CAN_FIFO_Init((TeCanPort)PortId);
    CAN_FIFO_InitCallbacks((TeCanPort)PortId);
    CAN_InitWrite((TeCanPort)PortId);

    switch(PortId)
    {
        case eCanPort_0:
            {
                halCanDevicePtr->MsgChn = CANCHNA;
                //init can device
                //sample points = 75%
            	switch (Baud)
            	{
                    case eCanPortSpeed_125kbps:
                        {
                            gHalCanBaudValue = 125;
                        }
                        break;
                    case eCanPortSpeed_250kbps:
                        {
                            gHalCanBaudValue = 250;
                        }
                        break;
                    case eCanPortSpeed_500kbps:
                        {
                            gHalCanBaudValue = 500;
                        }
                        break;
                    case eCanPortSpeed_1000kbps:
                        {
                            gHalCanBaudValue = 1000;
                        }
                        break;
                    default:
                    	{
                            gHalCanBaudValue = 500;
                    	}
                		break;
            	}
                InitCanDrive(halCanDevicePtr, gHalCanBaudValue);
            }
            break;
        default:return 3;
    }
    gHalCanRxStruct.u8IdxHead = 0;
    gHalCanRxStruct.u8IdxTail = 0;
    gHalCanRxStruct.u32CanRxCount = 0;
    memset(gHalCanRxStruct.sCanRxTFrame, 0, sizeof(gHalCanRxStruct.sCanRxTFrame));
    //init can TxMsg
    for(i=0; i<CAN_DRIVER_RX_NUM; i++)
    {
        gHalCanRxStruct.sCanRxTFrame[i].data = canRxBuf[i];
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
                DeinitCanDrive(halCanDevicePtr);
                // 重置状态变量
                gHalCanRxStruct.u8IdxHead = 0;
                gHalCanRxStruct.u8IdxTail = 0;
                gHalCanRxStruct.u32CanRxCount = 0;

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
    switch(PortId)
    {
        case eCanPort_0:
            {
                CanSendDrive(halCanDevicePtr, Mb, &sCanMsg);
            }
            break;
        default:break;
    }
    return eErrorOk;
}

TsCanFrame*
hal_can_rx_queue_de(void)
{
    static TsCanFrame halCanFrameTmp = {};

    ENTER_CRITICAL();
    //is empty
    if (gHalCanRxStruct.u32CanRxCount == 0)
    {
        EXIT_CRITICAL();
        return NULL;
    }
    //copy data
    memcpy(&halCanFrameTmp.data, gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxHead].data, CAN_DRIVER_MAX_DLC);
    halCanFrameTmp.dlc = gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxHead].dlc;
    halCanFrameTmp.is_ext_id =(gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxHead].rid.all & CAN_DRIVER_EID_FLAG)!=0 ? 1 : 0;
    if (halCanFrameTmp.is_ext_id)
    {
        halCanFrameTmp.id = gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxHead].rid.all & CAN_DRIVER_EXT_M;
    }
    else
    {
        halCanFrameTmp.id = gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxHead].rid.all & CAN_DRIVER_STD_M;
    }
    //不适用CANFD
    halCanFrameTmp.is_can_fd = 0;
    //dequeue
    gHalCanRxStruct.u8IdxHead = (gHalCanRxStruct.u8IdxHead + 1)%CAN_DRIVER_RX_NUM;
    gHalCanRxStruct.u32CanRxCount--;

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
    switch(PortId)
    {
        case eCanPort_0:
            {
                (*SpCAN_CallbackTx[(TeCanPort)PortId])((TeCanPort)PortId, Mb);
            }
            break;
        default:break;
    }
}


void
hal_can_rx_callback(uint8_t PortId, uint8_t Mb)
{
    switch(PortId)
    {
        case eCanPort_0:
            {
                if(gHalCanRxStruct.u32CanRxCount >= CAN_DRIVER_RX_NUM)
                {
                    gHalCanErrorCount++;  // 溢出错误
                    return;
                }
                CanRecvDrive(halCanDevicePtr, Mb, &gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxTail]);
                if(gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxTail].dlc > CAN_DRIVER_MAX_DLC)
                {
                    gHalCanRxStruct.sCanRxTFrame[gHalCanRxStruct.u8IdxTail].dlc = CAN_DRIVER_MAX_DLC;
                }
                gHalCanRxStruct.u8IdxTail = (gHalCanRxStruct.u8IdxTail + 1)%CAN_DRIVER_RX_NUM;
                gHalCanRxStruct.u32CanRxCount++;
            }
            break;
        default:break;
    }
}

void
hal_can_error_callback(uint8_t PortId, HAL_CAN_ER_TYPE u8CanErrorType)
{
    switch(u8CanErrorType)
    {
        case HAL_CAN_ER_ERROR:
            {
                //CAN_GetError(halCanDevicePtr, &u32ErrorMask);
                gHalCanErrorCount ++;
            }
            break;
        case HAL_CAN_ER_BUSOFF:
            {
                gHalCanBusoffCount++;
                //TODO:判断标志，并在main中调用以下进行can重启
                hal_can_deinit(PortId);
                hal_can_init(PortId, eCanPortSpeed_500kbps);
            }
            break;
        default:break;
    }
}

