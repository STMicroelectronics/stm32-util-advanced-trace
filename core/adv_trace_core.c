/**
  ******************************************************************************
  * @file    adv_trace_core.c
  * @brief   This file contains the advanced trace utility functions.
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
#include "adv_trace_core.h"
#include <stdarg.h>

/** @addtogroup ADV_TRACE
  * @{
  */

/* Private defines -----------------------------------------------------------*/

/** @defgroup ADV_TRACE_PRIVATE_DEFINES ADV_TRACE Privates Defines
  *  @{
  */

/**
  *  @brief  Macro to override the default memory location of the trace buffer.
  *
  */
#if !defined(ADV_TRACE_MEMLOCATION)
#define ADV_TRACE_MEMLOCATION
#endif /* ADV_TRACE_MEMLOCATION */

#if defined(ADV_TRACE_OVERRUN)
/**
  *  @brief  List of status codes for trace overrun handling.
  *
  *  @note Applicable only if ADV_TRACE_OVERRUN is set in the configuration.
  */
typedef enum
{
  TRACE_OVERRUN_NONE = 0,   /*!<No overrun detected                            */
  TRACE_OVERRUN_INDICATION, /*!<Overrun detected; an indication must be sent   */
  TRACE_OVERRUN_TRANSFER,   /*!<Overrun data transfer in progress              */
  TRACE_OVERRUN_EXECUTED,   /*!<Overrun data transfer finished                 */
} TRACE_OVERRUN_STATUS;
#endif /* ADV_TRACE_OVERRUN */

#if defined(ADV_TRACE_UNCHUNK_MODE)
/**
  *  @brief  List of status codes for trace overrun handling.
  *
  *  @note Applicable only if ADV_TRACE_UNCHUNK_MODE is set in the configuration.
  */
typedef enum
{
  TRACE_UNCHUNK_NONE = 0,     /*!<No unchunk event detected          */
  TRACE_UNCHUNK_DETECTED,     /*!<Unchunk event detected             */
  TRACE_UNCHUNK_TRANSFER      /*!<Unchunk data transfer in progress  */
} TRACE_UNCHUNK_STATUS;
#endif /* ADV_TRACE_UNCHUNK_MODE */
/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/** @defgroup ADV_TRACE_PRIVATE_TYPEDEF ADV_TRACE Private Typedef
  *  @{
  */

/**
  *  @brief  ADV TRACE Context structure.
  *  This structure contains all the data to handle the trace context.
  *
  *  @note Some members are included only if specific configuration options are enabled
  *  ADV_TRACE_UNCHUNK_MODE, ADV_TRACE_OVERRUN, ADV_TRACE_CONDITIONAL .
  */
typedef struct
{
#if defined(ADV_TRACE_UNCHUNK_MODE)
  uint16_t unchunk_enabled;                  /*!<Unchunk mode activation indicator                    */
  TRACE_UNCHUNK_STATUS unchunk_status;       /*!<Unchunk transfer process status                      */
#endif /* ADV_TRACE_UNCHUNK_MODE */
#if defined(ADV_TRACE_OVERRUN)
  TRACE_OVERRUN_STATUS OverRunStatus;        /*!<Overrun status                                       */
  cb_overrun *overrun_func;                  /*!<Pointer to the overrun callback function             */
#endif /* ADV_TRACE_OVERRUN */
#if defined(ADV_TRACE_CONDITIONAL)
  cb_timestamp *timestamp_func;              /*!<Pointer to the insertion timestamp callback function */
  uint8_t  CurrentVerboseLevel;              /*!<Current trace verbose level                          */
  uint32_t RegionMask;                       /*!<Mask of the enabled trace regions                    */
#endif /* ADV_TRACE_CONDITIONAL */
  uint16_t TraceRdPtr;                       /*!<Read pointer of the trace system                     */
  uint16_t TraceWrPtr;                       /*!<Write pointer of the trace system                    */
  uint16_t TraceSentSize;                    /*!<Size of the latest transfer data                     */
  uint16_t TraceLock;                        /*!<Lock counter of the trace system                     */
} adv_trace_context_t;

/**
  *  @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup ADV_TRACE_PRIVATE_VARIABLES ADV_TRACE Private Variables
  *  @{
  */

/**
  * @brief Advanced trace context instance
  * Variable contains all internal state and data for trace operations.
  */
static adv_trace_context_t ADV_TRACE_Ctx;

/**
  * @brief Advanced trace buffer
  * FIFO storage for trace data with configurable size.
  */
static ADV_TRACE_MEMLOCATION uint8_t ADV_TRACE_Buffer[ADV_TRACE_FIFO_SIZE];

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup ADV_TRACE_PRIVATE_FUNCTIONS ADV_TRACE Private Functions
  *  @{
  */
static void TRACE_TxCpltCallback(void *ptr);
static int16_t TRACE_AllocateBuffer(uint16_t size, uint16_t *pos);
static adv_trace_status TRACE_Send(void);

static void TRACE_Lock(void);
static void TRACE_UnLock(void);
static uint32_t TRACE_IsLocked(void);

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

/** @addtogroup ADV_TRACE_EXPORTED_FUNCTIONS
  *  @{
  */

/**
  * @brief  Initialize the advanced trace logging system.
  * @param p_obj Pointer to the peripheral interface handle
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_Init(void *p_obj)
{
  adv_trace_status ret_status = ADV_TRACE_OK;
  /* Initialize the read/write pointers and buffer contents */
  (void)ADV_TRACE_MEMSET8(&ADV_TRACE_Ctx, 0x0, (uint16_t)sizeof(adv_trace_context_t));
  (void)ADV_TRACE_MEMSET8(&ADV_TRACE_Buffer, 0x0, (uint16_t)sizeof(ADV_TRACE_Buffer));

#if defined(ADV_TRACE_UNCHUNK_MODE)
  ADV_TRACE_DEBUG("\nUNCHUNK_MODE\n");
#endif /* ADV_TRACE_UNCHUNK_MODE */
  /* Allocate Lock resource */
  ADV_TRACE_INIT_CRITICAL_SECTION();

  /* Initialize the Low Level interface */
  if (ADVTRACE_ITF_DATA_Init(p_obj, TRACE_TxCpltCallback) != ADVTRACE_ITF_DATA_OK)
  {
    ret_status = ADV_TRACE_ERROR;
  }
  return ret_status;
}

/**
  * @brief Deinitialize the advanced trace logging system.
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_DeInit(void)
{
  adv_trace_status ret_status = ADV_TRACE_OK;
  /* Deinitialize the Low Level interface */
  if (ADVTRACE_ITF_DATA_DeInit() != ADVTRACE_ITF_DATA_OK)
  {
    ret_status = ADV_TRACE_ERROR;
  }
  return ret_status;
}

/**
  * @brief Check if the trace buffer is empty.
  * @retval uint8_t Returns 1 if empty, 0 otherwise.
  */
uint8_t ADV_TRACE_IsBufferEmpty(void)
{
  /* Check if the trace buffer is empty */
  if (ADV_TRACE_Ctx.TraceWrPtr == ADV_TRACE_Ctx.TraceRdPtr)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief Start the trace reception process.
  * @param UserCallback Pointer to the RX data retrieval callback function
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_StartRxProcess(void (*UserCallback)(uint8_t *p_data, uint16_t size_byte, uint8_t error))
{
  adv_trace_status ret_status = ADV_TRACE_OK;

  /* Start the receive process */
  if (ADVTRACE_ITF_DATA_StartRx(UserCallback) != ADVTRACE_ITF_DATA_OK)
  {
    ret_status = ADV_TRACE_ERROR;
  }
  return ret_status;
}

/**
  * @brief Decode and post a trace message to the circular queue.
  * @param strFormat Pointer to the format string used to decode the trace message
  * @retval  Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_FSend(const char *str_Format, ...)
{
  uint8_t buf[ADV_TRACE_MAX_FORMAT_BUF_SIZE];
  va_list vaArgs;

  va_start(vaArgs, str_Format);
  uint16_t bufSize = (uint16_t) ADV_TRACE_VSNPRINTF((char *)buf, ADV_TRACE_MAX_FORMAT_BUF_SIZE, str_Format, vaArgs);
  va_end(vaArgs);

  return ADV_TRACE_Send(buf, bufSize);
}

/**
  * @brief Post a trace message to the circular queue.
  * @param p_data Pointer to the data to transmit
  * @param length Number of bytes to send
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_Send(const uint8_t *p_data, uint16_t length)
{
  adv_trace_status ret;
  uint16_t write_pos_tmp;
  uint32_t idx;

  TRACE_Lock();

  /* If buffer allocation succeeds, write data */
  if (TRACE_AllocateBuffer(length, &write_pos_tmp) != -1)
  {
    /* Fill the buffer with provided data bytes */
    for (idx = 0u; idx < length; idx++)
    {
      ADV_TRACE_Buffer[write_pos_tmp] = p_data[idx];
      write_pos_tmp = (uint16_t)((write_pos_tmp + 1u) % ADV_TRACE_FIFO_SIZE);
    }
    TRACE_UnLock();
    ret = TRACE_Send();
  }
  else
  {
    TRACE_UnLock();
    ret = ADV_TRACE_MEM_FULL;
  }

  return ret;
}

/**
  * @brief Allocate memory inside the FIFO buffer to write the trace data to be sent.
  * @param length Number of bytes to allocate in the buffer
  * @param p_data Pointer to the allocated buffer address
  * @param fifo_size Total size of the FIFO buffer
  * @param write_pos Pointer to the current write position in the FIFO buffer
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_ZCSend_Allocation(uint16_t length, uint8_t **p_data, uint16_t *fifo_size,
                                             uint16_t *write_pos)
{
  adv_trace_status ret = ADV_TRACE_OK;
  uint16_t write_pos_tmp;

  TRACE_Lock();

  /* If buffer allocation succeeds, prepare user access */
  if (TRACE_AllocateBuffer(length, &write_pos_tmp)  != -1)
  {
    /* Set output pointers for buffer, size, and write position */
    *p_data = ADV_TRACE_Buffer;
    *fifo_size = ADV_TRACE_FIFO_SIZE;
    *write_pos = (uint16_t)write_pos_tmp;
  }
  else
  {
    TRACE_UnLock();
    ret = ADV_TRACE_MEM_FULL;
  }

  return ret;
}

/**
  * @brief Finalize the trace data transmission process.
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_ZCSend_Finalize(void)
{
  TRACE_UnLock();
  return TRACE_Send();
}

/**
  * @brief Pre-send hook for the trace system.
  */
__WEAK void ADV_TRACE_PreSendHook(void)
{
}

/**
  * @brief Post-send hook for the trace system.
  */
__WEAK void ADV_TRACE_PostSendHook(void)
{
}

#if defined(ADV_TRACE_OVERRUN)
/**
  * @brief Register an overrun callback function.
  * @param cb Pointer to the overrun callback function
  */
void ADV_TRACE_RegisterOverRunFunction(cb_overrun *cb)
{
  ADV_TRACE_Ctx.overrun_func = *cb;
}
#endif /* ADV_TRACE_OVERRUN */

#if defined(ADV_TRACE_CONDITIONAL)
/**
  * @brief Register a timestamp insertion callback function.
  * @param cb Pointer to the timestamp insertion callback function
  */
void ADV_TRACE_RegisterTimeStampFunction(cb_timestamp *cb)
{
  ADV_TRACE_Ctx.timestamp_func = *cb;
}

/**
  * @brief Set the trace verbose level.
  * @param level Verbose level value to apply (from 0 to 255)
  */
void ADV_TRACE_SetVerboseLevel(uint8_t level)
{
  ADV_TRACE_Ctx.CurrentVerboseLevel = level;
}

/**
  * @brief Get the trace verbose level.
  * @retval uint8_t The current verbose level value
  */
uint8_t ADV_TRACE_GetVerboseLevel(void)
{
  return ADV_TRACE_Ctx.CurrentVerboseLevel;
}

/**
  * @brief Enable a trace region by setting its bitfield in the mask.
  * @param region Bitfield for the region to be enabled
  */
void ADV_TRACE_SetRegion(uint32_t region)
{
  ADV_TRACE_Ctx.RegionMask |= region;
}

/**
  * @brief Get the bitmask of all currently enabled trace regions.
  * @retval uint32_t The bitmask of enabled regions
  */
uint32_t ADV_TRACE_GetRegion(void)
{
  return ADV_TRACE_Ctx.RegionMask;
}

/**
  * @brief Disable the specified trace region.
  * @param region Bitfield of the region to be disabled
  */
void ADV_TRACE_ResetRegion(uint32_t region)
{
  ADV_TRACE_Ctx.RegionMask &= ~region;
}

/**
  * @brief Conditionally decode and post a trace message to the circular queue using the provided parameters.
  * @param verbose_level The trace verbosity level
  * @param region The trace region
  * @param time_stamp_state Flag to include a timestamp (0: exclude, 1: include)
  * @param str_Format Pointer to the format string used to decode the trace message
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_COND_FSend(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                      const char *str_Format, ...)
{
  va_list vaArgs;
#if defined(ADV_TRACE_UNCHUNK_MODE)
  uint8_t buf[ADV_TRACE_MAX_TIMESTAMP_SIZE] = {0};
  uint16_t timestamp_size = 0u;
  static uint8_t sztmp[ADV_TRACE_MAX_FORMAT_BUF_SIZE];
  uint16_t write_pos_tmp;
  uint16_t idx;
#else
  uint8_t buf[ADV_TRACE_MAX_FORMAT_BUF_SIZE + ADV_TRACE_MAX_TIMESTAMP_SIZE];
#endif /* ADV_TRACE_UNCHUNK_MODE */
  uint16_t buff_size = 0;

  /* Check if verbose level is sufficient for operation */
  if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= verbose_level))
  {
    return ADV_TRACE_GIVEUP;
  }

  if ((region & ADV_TRACE_Ctx.RegionMask) != region)
  {
    return ADV_TRACE_REGIONMASKED;
  }

#if defined(ADV_TRACE_UNCHUNK_MODE)
  if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (time_stamp_state != 0u))
  {
    ADV_TRACE_Ctx.timestamp_func(buf, &timestamp_size);
  }

  va_start(vaArgs, str_Format);
  buff_size = (uint16_t)ADV_TRACE_VSNPRINTF((char *)sztmp, ADV_TRACE_MAX_FORMAT_BUF_SIZE, str_Format, vaArgs);

  TRACE_Lock();

  /* If buffer allocation succeeds, proceed to buffer data writing */
  if (TRACE_AllocateBuffer((buff_size + timestamp_size), &write_pos_tmp) != -1)
  {
#if defined(ADV_TRACE_OVERRUN)
    ADV_TRACE_ENTER_CRITICAL_SECTION();
    if (ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_EXECUTED)
    {
      /* Clear the overrun status */
      ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_NONE;
    }
    ADV_TRACE_EXIT_CRITICAL_SECTION();
#endif /* ADV_TRACE_OVERRUN */

    /* Insert the timestamp data into the buffer */
    for (idx = 0u; idx < timestamp_size; idx++)
    {
      ADV_TRACE_Buffer[write_pos_tmp] = buf[idx];
      write_pos_tmp = write_pos_tmp + 1u;
    }

    /* Insert the trace message into the buffer */
    (void)ADV_TRACE_VSNPRINTF((char *)(&ADV_TRACE_Buffer[write_pos_tmp]), ADV_TRACE_MAX_FORMAT_BUF_SIZE, str_Format,
                              vaArgs);
    va_end(vaArgs);

    TRACE_UnLock();

    return TRACE_Send();
  }

  va_end(vaArgs);
  TRACE_UnLock();
#if defined(ADV_TRACE_OVERRUN)
  ADV_TRACE_ENTER_CRITICAL_SECTION();
  if ((ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_NONE) && (NULL != ADV_TRACE_Ctx.overrun_func))
  {
    ADV_TRACE_DEBUG("ADV_TRACE_Send:TRACE_OVERRUN_INDICATION");
    ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_INDICATION;
  }
  ADV_TRACE_EXIT_CRITICAL_SECTION();
#endif /* ADV_TRACE_OVERRUN */

  return ADV_TRACE_MEM_FULL;

#else
  if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (time_stamp_state != 0u))
  {
    ADV_TRACE_Ctx.timestamp_func(buf, &buff_size);
  }

  va_start(vaArgs, str_Format);
  buff_size += (uint16_t) ADV_TRACE_VSNPRINTF((char *)(buf + buff_size), ADV_TRACE_MAX_FORMAT_BUF_SIZE, str_Format,
                                              vaArgs);
  va_end(vaArgs);

  return ADV_TRACE_Send(buf, buff_size);
#endif /* ADV_TRACE_UNCHUNK_MODE */
}

/**
  * @brief Conditionally allocate memory inside the FIFO buffer to write the trace data to be sent.
  * @param verbose_level The trace verbosity level
  * @param region The trace region
  * @param time_stamp_state Flag to include a timestamp (0: exclude, 1: include)
  * @param length Number of bytes to allocate in the buffer
  * @param p_data Pointer to the allocated buffer address
  * @param fifo_size Total size of the FIFO buffer
  * @param write_pos Pointer to the current write position in the FIFO buffer
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_COND_ZCSend_Allocation(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                                  uint16_t length, uint8_t **p_data, uint16_t *fifo_size,
                                                  uint16_t *write_pos)
{
  adv_trace_status ret = ADV_TRACE_OK;
  uint16_t write_pos_tmp;
  uint8_t timestamp_ptr[ADV_TRACE_MAX_TIMESTAMP_SIZE] = {0};
  uint16_t timestamp_size = 0u;

  /* Check if verbose level is sufficient for operation */
  if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= verbose_level))
  {
    return ADV_TRACE_GIVEUP;
  }

  if ((region & ADV_TRACE_Ctx.RegionMask) != region)
  {
    return ADV_TRACE_REGIONMASKED;
  }

  if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (time_stamp_state != 0u))
  {
    ADV_TRACE_Ctx.timestamp_func(timestamp_ptr, &timestamp_size);
  }

  TRACE_Lock();

  /* If buffer allocation succeeds, proceed to buffer data writing */
  if (TRACE_AllocateBuffer(length + timestamp_size, &write_pos_tmp) != -1)
  {
    /* Insert the timestamp data into the buffer */
    for (uint16_t index = 0u; index < timestamp_size; index++)
    {
      ADV_TRACE_Buffer[write_pos_tmp] = timestamp_ptr[index];
      write_pos_tmp = (uint16_t)((write_pos_tmp + 1u) % ADV_TRACE_FIFO_SIZE);
    }

    /* Set output pointers for buffer, size, and write position */
    *p_data = ADV_TRACE_Buffer;
    *fifo_size = (uint16_t) ADV_TRACE_FIFO_SIZE;
    *write_pos = write_pos_tmp;
  }
  else
  {
    TRACE_UnLock();
    ret = ADV_TRACE_MEM_FULL;
  }
  return ret;
}
/**
  * @brief Conditionally finalize the trace data transmission process.
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_COND_ZCSend_Finalize(void)
{
  return ADV_TRACE_ZCSend_Finalize();
}

/**
  * @brief Conditionally post a trace message to the circular queue.
  * @param verbose_level The trace verbose level
  * @param region The trace region
  * @param time_stamp_state Flag to include a timestamp (0: exclude, 1: include)
  * @param p_data Pointer to the data to transmit
  * @param length Number of bytes to send
  * @retval Status code from the adv_trace_status enumeration
  */
adv_trace_status ADV_TRACE_COND_Send(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                     const uint8_t *p_data, uint16_t length)
{
  adv_trace_status ret;
  uint16_t write_pos_tmp;
  uint32_t idx;
  uint8_t timestamp_ptr[ADV_TRACE_MAX_TIMESTAMP_SIZE] = {0};
  uint16_t timestamp_size = 0u;

  /* Check if verbose level is sufficient for operation */
  if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= verbose_level))
  {
    return ADV_TRACE_GIVEUP;
  }

  if ((region & ADV_TRACE_Ctx.RegionMask) != region)
  {
    return ADV_TRACE_REGIONMASKED;
  }

  if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (time_stamp_state != 0u))
  {
    ADV_TRACE_Ctx.timestamp_func(timestamp_ptr, &timestamp_size);
  }

  TRACE_Lock();

  /* If buffer allocation succeeds, proceed to buffer data writing */
  if (TRACE_AllocateBuffer(length + timestamp_size, &write_pos_tmp) != -1)
  {
    /* Insert the timestamp data into the buffer */
    for (idx = 0; idx < timestamp_size; idx++)
    {
      ADV_TRACE_Buffer[write_pos_tmp] = timestamp_ptr[idx];
      write_pos_tmp = (uint16_t)((write_pos_tmp + 1u) % ADV_TRACE_FIFO_SIZE);
    }

    for (idx = 0u; idx < length; idx++)
    {
      ADV_TRACE_Buffer[write_pos_tmp] = p_data[idx];
      write_pos_tmp = (uint16_t)((write_pos_tmp + 1u) % ADV_TRACE_FIFO_SIZE);
    }

    TRACE_UnLock();
    ret = TRACE_Send();
  }
  else
  {
    TRACE_UnLock();
    ret = ADV_TRACE_MEM_FULL;
  }

  return ret;
}
#endif /* ADV_TRACE_CONDITIONAL */

/**
  * @}
  */

/** @addtogroup ADV_TRACE_PRIVATE_FUNCTIONS
  *  @{
  */

/**
  * @brief Send the trace data to the low layer.
  * @retval Status code from the adv_trace_status enumeration
  */
static adv_trace_status TRACE_Send(void)
{
  adv_trace_status ret = ADV_TRACE_OK;
  uint8_t *ptr;

  ADV_TRACE_ENTER_CRITICAL_SECTION();

  if (TRACE_IsLocked() == 0U)
  {
    TRACE_Lock();

    if (ADV_TRACE_Ctx.TraceRdPtr != ADV_TRACE_Ctx.TraceWrPtr)
    {
#if defined(ADV_TRACE_UNCHUNK_MODE)
      if (TRACE_UNCHUNK_DETECTED == ADV_TRACE_Ctx.unchunk_status)
      {
        ADV_TRACE_Ctx.TraceSentSize = (uint16_t)(ADV_TRACE_Ctx.unchunk_enabled - ADV_TRACE_Ctx.TraceRdPtr);
        ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_TRANSFER;
        ADV_TRACE_Ctx.unchunk_enabled = 0;

        ADV_TRACE_DEBUG("\nTRACE_TxCpltCallback::unchunk start(%d,%d)\n", ADV_TRACE_Ctx.unchunk_enabled,
                        ADV_TRACE_Ctx.TraceRdPtr);

        if (0u == ADV_TRACE_Ctx.TraceSentSize)
        {
          ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_NONE;
          ADV_TRACE_Ctx.TraceRdPtr = 0;
        }
      }

      if (TRACE_UNCHUNK_NONE == ADV_TRACE_Ctx.unchunk_status)
      {
#endif /* ADV_TRACE_UNCHUNK_MODE */
        if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
        {
          ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_Ctx.TraceWrPtr - ADV_TRACE_Ctx.TraceRdPtr;
        }
        else /* Read pointer is ahead of write pointer */
        {
          ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceRdPtr;
        }
#if defined(ADV_TRACE_UNCHUNK_MODE)
      }
#endif /* ADV_TRACE_UNCHUNK_MODE */
      ptr = &ADV_TRACE_Buffer[ADV_TRACE_Ctx.TraceRdPtr];

      ADV_TRACE_EXIT_CRITICAL_SECTION();
      ADV_TRACE_PreSendHook();

      ADV_TRACE_DEBUG("\n--TRACE_Send(%d-%d)--\n", ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceSentSize);
      if (ADVTRACE_ITF_DATA_Transmit(ptr, ADV_TRACE_Ctx.TraceSentSize) == ADVTRACE_ITF_DATA_OK)
      {
        ret = ADV_TRACE_OK;
      }
      else
      {
        ret = ADV_TRACE_ERROR;
      }
    }
    else
    {
      TRACE_UnLock();
      ADV_TRACE_EXIT_CRITICAL_SECTION();
    }
  }
  else
  {
    ADV_TRACE_EXIT_CRITICAL_SECTION();
  }

  return ret;
}

/**
  * @brief Tx callback triggered by the low-level layer to signal transfer completion.
  * @param p_hal_handle Unused pointer, present for HAL interface compatibility
  */
static void TRACE_TxCpltCallback(void *p_hal_handle)
{
  (void)p_hal_handle;
  uint8_t *ptr = NULL;
  ADV_TRACE_ENTER_CRITICAL_SECTION();

#if defined(ADV_TRACE_OVERRUN)
  if (ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_TRANSFER)
  {
    ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_EXECUTED;
    ADV_TRACE_DEBUG("\n--TRACE_Send overrun complete--\n");
    ADV_TRACE_Ctx.TraceSentSize = 0u;
  }
#endif /* ADV_TRACE_OVERRUN */

#if defined(ADV_TRACE_UNCHUNK_MODE)
  if (TRACE_UNCHUNK_TRANSFER == ADV_TRACE_Ctx.unchunk_status)
  {
    ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_NONE;
    ADV_TRACE_Ctx.TraceRdPtr = 0;
    ADV_TRACE_DEBUG("\nTRACE_TxCpltCallback::unchunk complete\n");
  }
  else
  {
    ADV_TRACE_Ctx.TraceRdPtr = (ADV_TRACE_Ctx.TraceRdPtr + ADV_TRACE_Ctx.TraceSentSize) % ADV_TRACE_FIFO_SIZE;
  }
#else
  ADV_TRACE_Ctx.TraceRdPtr = (ADV_TRACE_Ctx.TraceRdPtr + ADV_TRACE_Ctx.TraceSentSize) % ADV_TRACE_FIFO_SIZE;
#endif /* ADV_TRACE_UNCHUNK_MODE */

#if defined(ADV_TRACE_OVERRUN)
  if (ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_INDICATION)
  {
    ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_TRANSFER;
    ADV_TRACE_EXIT_CRITICAL_SECTION();
    ADV_TRACE_Ctx.overrun_func(&ptr, &ADV_TRACE_Ctx.TraceSentSize);
    ADV_TRACE_DEBUG("\n--Driver_Send overrun(%d)--\n", ADV_TRACE_Ctx.TraceSentSize);
    (void)ADVTRACE_ITF_DATA_Transmit(ptr, ADV_TRACE_Ctx.TraceSentSize);
    return;
  }
#endif /* ADV_TRACE_OVERRUN */

  if ((ADV_TRACE_Ctx.TraceRdPtr != ADV_TRACE_Ctx.TraceWrPtr) && (1u == ADV_TRACE_Ctx.TraceLock))
  {
#if defined(ADV_TRACE_UNCHUNK_MODE)
    if (TRACE_UNCHUNK_DETECTED == ADV_TRACE_Ctx.unchunk_status)
    {
      ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_Ctx.unchunk_enabled - ADV_TRACE_Ctx.TraceRdPtr;
      ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_TRANSFER;
      ADV_TRACE_Ctx.unchunk_enabled = 0;

      ADV_TRACE_DEBUG("\nTRACE_TxCpltCallback::unchunk start(%d,%d)\n", ADV_TRACE_Ctx.unchunk_enabled,
                      ADV_TRACE_Ctx.TraceRdPtr);

      if (0u == ADV_TRACE_Ctx.TraceSentSize)
      {
        /* This case occurs when a write operation matches the read and chunk positions */
        /* Unchunk event is discarded in this scenario */
        ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_NONE;
        ADV_TRACE_Ctx.TraceRdPtr = 0;
      }
    }

    if (TRACE_UNCHUNK_NONE == ADV_TRACE_Ctx.unchunk_status)
    {
#endif /* ADV_TRACE_UNCHUNK_MODE*/
      if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
      {
        ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_Ctx.TraceWrPtr - ADV_TRACE_Ctx.TraceRdPtr;
      }
      else /* Read pointer is ahead of write pointer  */
      {
        ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceRdPtr;
      }
#if defined(ADV_TRACE_UNCHUNK_MODE)
    }
#endif /* ADV_TRACE_UNCHUNK_MODE */
    ptr = &ADV_TRACE_Buffer[ADV_TRACE_Ctx.TraceRdPtr];
    ADV_TRACE_EXIT_CRITICAL_SECTION();
    ADV_TRACE_DEBUG("\n--TRACE_Send(%d-%d)--\n", ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceSentSize);
    (void)ADVTRACE_ITF_DATA_Transmit(ptr, ADV_TRACE_Ctx.TraceSentSize);
  }
  else
  {
    ADV_TRACE_EXIT_CRITICAL_SECTION();
    ADV_TRACE_PostSendHook();
    TRACE_UnLock();
  }
}

/**
  * @brief  Allocate memory in the FIFO buffer to push data.
  * @param  Size Number of bytes to reserve in the FIFO buffer
  * @param  Pos Pointer to the position in the FIFO buffer
  * @retval Write position in the buffer, or -1 if insufficient space
  */
static int16_t TRACE_AllocateBuffer(uint16_t Size, uint16_t *Pos)
{
  uint16_t freesize;
  int16_t ret = -1;

  ADV_TRACE_ENTER_CRITICAL_SECTION();

  if (ADV_TRACE_Ctx.TraceWrPtr == ADV_TRACE_Ctx.TraceRdPtr)
  {
#if defined(ADV_TRACE_UNCHUNK_MODE)
    freesize = (uint16_t)(ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceWrPtr);
    if ((Size >= freesize) && (ADV_TRACE_Ctx.TraceRdPtr > Size))
    {
      ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_DETECTED;
      ADV_TRACE_Ctx.unchunk_enabled = ADV_TRACE_Ctx.TraceWrPtr;
      freesize = ADV_TRACE_Ctx.TraceRdPtr;
      ADV_TRACE_Ctx.TraceWrPtr = 0;
    }
#else
    /* need to add buffer full management*/
    freesize = (int16_t)ADV_TRACE_FIFO_SIZE;
#endif /* ADV_TRACE_UNCHUNK_MODE */
  }
  else
  {
#if defined(ADV_TRACE_UNCHUNK_MODE)
    if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
    {
      freesize = (uint16_t)(ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceWrPtr);
      if ((Size >= freesize) && (ADV_TRACE_Ctx.TraceRdPtr > Size))
      {
        ADV_TRACE_Ctx.unchunk_status = TRACE_UNCHUNK_DETECTED;
        ADV_TRACE_Ctx.unchunk_enabled = ADV_TRACE_Ctx.TraceWrPtr;
        freesize = ADV_TRACE_Ctx.TraceRdPtr;
        ADV_TRACE_Ctx.TraceWrPtr = 0;
      }
    }
    else
    {
      freesize = (uint16_t)(ADV_TRACE_Ctx.TraceRdPtr - ADV_TRACE_Ctx.TraceWrPtr);
    }
#else
    if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
    {
      freesize = ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceWrPtr + ADV_TRACE_Ctx.TraceRdPtr;
    }
    else
    {
      freesize = ADV_TRACE_Ctx.TraceRdPtr - ADV_TRACE_Ctx.TraceWrPtr;
    }
#endif /* ADV_TRACE_UNCHUNK_MODE */
  }

  if (freesize > Size)
  {
    *Pos = ADV_TRACE_Ctx.TraceWrPtr;
    ADV_TRACE_Ctx.TraceWrPtr = (ADV_TRACE_Ctx.TraceWrPtr + Size) % ADV_TRACE_FIFO_SIZE;
    ret = 0;
#if defined(ADV_TRACE_OVERRUN)
    if (ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_EXECUTED)
    {
      /* clear the over run */
      ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_NONE;
    }
#endif /* ADV_TRACE_OVERRUN */

#if defined(ADV_TRACE_UNCHUNK_MODE)
    ADV_TRACE_DEBUG("\n--TRACE_AllocateBuffer(%d-%d-%d::%d-%d)--\n", freesize - Size, Size,
                    ADV_TRACE_Ctx.unchunk_enabled, ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceWrPtr);
#else
    ADV_TRACE_DEBUG("\n--TRACE_AllocateBuffer(%d-%d::%d-%d)--\n", freesize - Size, Size, ADV_TRACE_Ctx.TraceRdPtr,
                    ADV_TRACE_Ctx.TraceWrPtr);
#endif /* ADV_TRACE_UNCHUNK_MODE */
  }
#if defined(ADV_TRACE_OVERRUN)
  else
  {
    if ((ADV_TRACE_Ctx.OverRunStatus == TRACE_OVERRUN_NONE) && (NULL != ADV_TRACE_Ctx.overrun_func))
    {
      ADV_TRACE_DEBUG(":TRACE_OVERRUN_INDICATION");
      ADV_TRACE_Ctx.OverRunStatus = TRACE_OVERRUN_INDICATION;
    }
  }
#endif /* ADV_TRACE_OVERRUN */

  ADV_TRACE_EXIT_CRITICAL_SECTION();
  return ret;
}

/**
  * @brief  Lock the trace buffer.
  */
static void TRACE_Lock(void)
{
  ADV_TRACE_ENTER_CRITICAL_SECTION();
  ADV_TRACE_Ctx.TraceLock++;
  ADV_TRACE_EXIT_CRITICAL_SECTION();
}

/**
  * @brief  UnLock the trace buffer.
  */
static void TRACE_UnLock(void)
{
  ADV_TRACE_ENTER_CRITICAL_SECTION();
  ADV_TRACE_Ctx.TraceLock--;
  ADV_TRACE_EXIT_CRITICAL_SECTION();
}

/**
  * @brief  Return the lock status of the trace buffer.
  * @retval Lock status: 1 = locked, 0 = unlocked
  */
static uint32_t TRACE_IsLocked(void)
{
  return ((ADV_TRACE_Ctx.TraceLock == 0U) ? 0U : 1U);
}

/**
  * @}
  */

/**
  * @}
  */

