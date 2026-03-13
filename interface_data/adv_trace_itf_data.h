/**
  ******************************************************************************
  * @file  : adv_trace_itf_data.h
  * @brief : Header file for adv_trace interface file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADV_TRACE_ITF_DATA_H
#define ADV_TRACE_ITF_DATA_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"


/** @defgroup ADV_TRACE_ITF ADV_TRACE Interface
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup ADV_TRACE_ITF_EXPORTED_TYPEDEF ADV_TRACE_ITF Exported Typedef
  *  @{
  */

/**
  *  @brief  List the Advanced trace interface function status.
  *  list of the returned status value, any negative value is corresponding to an error.
  */
typedef enum
{
  ADVTRACE_ITF_DATA_OK                   =  0,     /*!< Operation completed successfully  */
  ADVTRACE_ITF_DATA_ERROR                = -1,     /*!< Operation completed with errors   */
} itfdata_status_t;

/**
  *  @}
  */

/* Exported functions ------------------------------------------------------- */


/** @defgroup ADV_TRACE_ITF_APIS ADV_TRACE_ITF Private Functions
  * @{
 */

itfdata_status_t ADVTRACE_ITF_DATA_Init(void *pObj, void (*cb)(void *));
itfdata_status_t ADVTRACE_ITF_DATA_DeInit(void);
itfdata_status_t ADVTRACE_ITF_DATA_Transmit(uint8_t *p_data, uint16_t size_byte);
itfdata_status_t ADVTRACE_ITF_DATA_StartRx(void (*cb)(uint8_t *p_data, uint16_t size_byte, uint8_t error));

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  *  @}
  */

#endif /* ADV_TRACE_ITF_DATA_H*/
