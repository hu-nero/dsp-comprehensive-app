/*
 * basic_types.h
 *
 *  Created on: 2025年12月16日
 *      Author: huxl
 */

#ifndef DEV_MCAL_SHAREDCAN_BASIC_TYPES_H_
#define DEV_MCAL_SHAREDCAN_BASIC_TYPES_H_

#include <stdint.h>
#include "stdint_fix.h"
#include <stdbool.h>


typedef enum
{
    eErrorOk,
    eErrorNotOk,
    eErrorBusy,
    eErrorUninit,
    eErrorOutOfRange,
    eErrorSNA,
    eErrorMIA,
    eErrorTimeout,
    eErrorNullPtr,
    eErrorExists,
    eErrorNoExist
} TeErrorEnum;

#endif /* DEV_MCAL_SHAREDCAN_BASIC_TYPES_H_ */
