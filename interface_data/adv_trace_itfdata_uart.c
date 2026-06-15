/**
  ******************************************************************************
  * @file  : adv_trace_itfdata_uart.c
  * @brief : This file contains the interface functions based on UART.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024-2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "adv_trace_itf_data.h"

/** @addtogroup ADV_TRACE_ITF
  * @{
  */

/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private typedef ------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/

/** @defgroup ADV_TRACE_ITF_UART_PRIVATE_VARIABLES ADV_TRACE ITF Uart Private Variables
  *  @{
  */

/**
  * @brief buffer to receive 1 character
  */
static uint8_t charRx;

/**
  * @brief UART handle used to initialize the context of UART interface
  */
static hal_uart_handle_t *itf_huart = NULL;

/**
  * @}
  */

/* External variables ---------------------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup ADV_TRACE_ITF_UART_PRIVATE_FUNCTIONS  ADV_TRACE ITF UART Private Functions
  * @{
  */

/**
  * @brief  TX complete callback.
  * @return none
  */
static void (*TxCpltCallback)(void *);
/**
  * @brief  RX complete callback.
  * @param  p_data ptr of chars buffer sent by user
  * @param  size_byte buffer size
  * @param  error error code
  * @return none
  */
static void (*RxCpltCallback)(uint8_t *p_data, uint16_t size_byte, uint8_t error);

/* Functions allowing the user to configure dynamically the UART callbacks instead of weak functions */
/**
  * @brief  RX TransferComplete callback, This function is executed when
  * the Rx transfer complete interrupt is generated.
  * @param  pUART ptr to uart handle
  */
static void TxTransferCpltCallback(hal_uart_handle_t *pUART);

/**
  * @brief  RX TransferComplete callback, This function is executed when
  * the Rx transfer complete interrupt is generated.
  * @param  pUART ptr to uart handle
  * @param  size_byte buffer size, not used
  * @param  rx_event error code, not used
  */
static void RxTransferCpltCallback(hal_uart_handle_t *pUART, uint32_t size_byte, hal_uart_rx_event_types_t rx_event);

/**
  * @brief  Tx TransferComplete callback, This function is executed when
  * the Tx transfer complete interrupt is generated.
  * @param  pUART ptr to uart handle
  */
static void TxTransferCpltCallback(hal_uart_handle_t *pUART)
{
  (void)pUART;
  TxCpltCallback(NULL);
}

/**
  * @brief  RX TransferComplete callback, This function is executed when
  * the Rx transfer complete interrupt is generated.
  * @param  pUART ptr to uart handle
  * @param  size_byte buffer size, not used
  * @param  rx_event error code, not used
  */
static void RxTransferCpltCallback(hal_uart_handle_t *pUART, uint32_t size_byte, hal_uart_rx_event_types_t rx_event)
{
  (void)pUART;
  (void)size_byte;
  (void)rx_event;
  /* forward the received data */
  RxCpltCallback(&charRx, 1, 0);
  /* starts a new reception */
  (void)HAL_UART_Receive_IT(itf_huart, &charRx, 1U);
}

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/**
  * @addtogroup ADV_TRACE_ITF_APIS
  * @{
  */

/**
  * @defgroup  ADV_TRACE_ITF_UART_APIS ADV_TRACE UART Interface APIs
  * @{
  */

/**
  * @brief Initialize the advanced trace UART interface.
  * @param pObj Pointer to the UART handle
  * @param  cb  Pointer to the callback function
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_Init(void *pObj, void (*cb)(void *))
{
  itfdata_status_t ret_status = ADVTRACE_ITF_DATA_ERROR;

  /* save the uart handle */
  itf_huart = (hal_uart_handle_t *)pObj;
  TxCpltCallback = cb;

  /* Register the UART callbacks */
  if (HAL_UART_RegisterTxCpltCallback(itf_huart, TxTransferCpltCallback) != HAL_OK)
  {
    return  ret_status;
  }
  ret_status = ADVTRACE_ITF_DATA_OK;

  return ret_status;
}

/**
  * @brief  Deinitialize the advanced trace UART interface.
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully.
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors.
  */
itfdata_status_t ADVTRACE_ITF_DATA_DeInit(void)
{
  /* stop the rx if started or not */
  (void)HAL_UART_AbortReceive(itf_huart);

  return ADVTRACE_ITF_DATA_OK;
}

/**
  * @brief  Start the UART receive process.
  * @param  cb Pointer to the UART RX handler function
  * @retval ADVTRACE_ITF_DATA_OK     Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR  Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_StartRx(void (*cb)(uint8_t *p_data, uint16_t size_byte, uint8_t error))
{
  itfdata_status_t ret_status = ADVTRACE_ITF_DATA_OK;

  if (HAL_UART_RegisterRxCpltCallback(itf_huart, RxTransferCpltCallback) != HAL_OK)
  {
    ret_status = ADVTRACE_ITF_DATA_ERROR;
  }
  else
  {
    /*record call back*/
    if (cb != NULL)
    {
      RxCpltCallback = cb;
    }

    if (HAL_UART_Receive_IT(itf_huart, &charRx, 1U) != HAL_OK)
    {
      ret_status = ADVTRACE_ITF_DATA_ERROR;
    }
  }

  return ret_status;
}

/**
  * @brief  Send a data buffer over the UART interface (using DMA).
  * @param  p_data Pointer to the buffer to send
  * @param  size_byte Number of bytes to transmit
  * @retval ADVTRACE_ITF_DATA_OK Operation completed successfully
  * @retval ADVTRACE_ITF_DATA_ERROR Operation completed with errors
  */
itfdata_status_t ADVTRACE_ITF_DATA_Transmit(uint8_t *p_data, uint16_t size_byte)
{
  itfdata_status_t ret_status = ADVTRACE_ITF_DATA_OK;

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /*
   * D-Cache maintenance works on whole cache lines.
   * Round start address down to 32-byte boundary and round covered length up
   * so all lines touched by [p_data, p_data + size_byte) are cleaned.
   */
  uint32_t addr = (uint32_t)p_data;
  uint32_t aligned_addr = addr & ~(32U - 1U);
  uint32_t aligned_size = ((uint32_t)size_byte + (addr - aligned_addr) + 31U) & ~(32U - 1U);

  SCB_CleanDCache_by_Addr((uint32_t *)aligned_addr, (int32_t)aligned_size);
#endif /* __DCACHE_PRESENT */

  if (HAL_UART_Transmit_DMA(itf_huart, p_data, size_byte) != HAL_OK)
  {
    ret_status = ADVTRACE_ITF_DATA_ERROR;
  }
  return ret_status;
}

/**
  *  @}
  */

/**
  *  @}
  */

/**
  *  @}
  */
