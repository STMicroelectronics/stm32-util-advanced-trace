/**
  ******************************************************************************
  * @file  : adv_trace_itfdata_template.c
  * @brief : This file is a template interface functions.
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

/* Includes ------------------------------------------------------------------*/
#include "adv_trace_itf_data.h"

/**
  * @addtogroup ADV_TRACE_ITF
  * @{
  */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @addtogroup ADV_TRACE_ITF_APIS
  * @{
  */

/**
  * @defgroup  ADV_TRACE_ITF_TEMPLATE_APIS ADV_TRACE Template Interface APIs
  * @{
  */

/**
  * @brief  Initialize the advanced trace interface.
  * @param pObj Pointer to the peripheral interface handle
  * @param  cb  Pointer to the callback function
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_Init(void *pObj, void (*cb)(void *))
{
  /* user code */
  return ADVTRACE_ITF_DATA_ERROR;
}

/**
  * @brief  Deinitialize the advanced trace interface.
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully.
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors.
  */
itfdata_status_t ADVTRACE_ITF_DATA_DeInit(void)
{
  /* user code */
  return ADVTRACE_ITF_DATA_ERROR;
}

/**
  * @brief  Start the receive process.
  * @param  cb Pointer to RX handler function
  * @retval ADVTRACE_ITF_DATA_OK     Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR  Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_StartRx(void (*cb)(uint8_t *p_data, uint16_t size_byte, uint8_t error))
{
  /* user code */
  return ADVTRACE_ITF_DATA_ERROR;
}

/**
  * @brief  Send a data buffer over the interface (using DMA).
  * @param  p_data Pointer to the buffer to send
  * @param  size_byte Number of bytes to transmit
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_Transmit(uint8_t *p_data, uint16_t size_byte)
{
  /* user code */
  return ADVTRACE_ITF_DATA_ERROR;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
