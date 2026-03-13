/**
  ******************************************************************************
  * @file    adv_trace_core.h
  * @brief   Header for adv_trace_core.c
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
#ifndef ADV_TRACE_CORE_H
#define ADV_TRACE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef ADV_TRACE_USER_CONF
#include "adv_trace_user_conf.h"
#else
#include "cmsis_compiler.h"
#include "stdio.h"
#include "string.h"
#endif /* ADV_TRACE_USER_CONF */
#include "adv_trace_itf_data.h"


/** @defgroup ADV_TRACE Advanced Trace
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADV_TRACE_EXPORTED_TYPEDEF ADV_TRACE Exported Typedef
  *  @{
  */

/**
  *  @brief Timestamp callback prototype.
  *  @param p_data Pointer to the data buffer to be timestamped
  *  @param size Pointer to the buffer size
  */
typedef void cb_timestamp(uint8_t *p_data, uint16_t *size);

#if defined(ADV_TRACE_OVERRUN)
/**
  *  @brief Overrun callback prototype.
  *  @param p_data Pointer to the data that could not be sent due to FIFO overflow
  *  @param size Pointer to the size of unsent data
  */
typedef void cb_overrun(uint8_t **p_data, uint16_t *size);
#endif /* ADV_TRACE_OVERRUN */

/**
  *  @brief  List of status codes for Advanced Trace operations.
  *  Any negative value corresponds to an error condition.
  */
typedef enum
{
  ADV_TRACE_OK              =  0,     /*!< Operation completed successfully.*/
  ADV_TRACE_MEM_FULL        = -1,     /*!< Insufficient memory in FIFO      */
  ADV_TRACE_ERROR           = -2,     /*!< Unexpected error                 */
#if defined(ADV_TRACE_CONDITIONAL)
  ADV_TRACE_GIVEUP          = -3,     /*!< Trace give up.                    */
  ADV_TRACE_REGIONMASKED    = -4      /*!< Trace region masked.              */
#endif /* ADV_TRACE_CONDITIONAL */
} adv_trace_status;

/**
  *  @}
  */

/* External variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup ADV_TRACE_EXPORTED_CONSTANTS ADV_TRACE Exported Constants
  *  @{
  */
#ifndef ADV_TRACE_MAX_FORMAT_BUF_SIZE
#define ADV_TRACE_MAX_FORMAT_BUF_SIZE           (128U)        /*!< Default trace formatting temporary buffer size . */
#endif /* ADV_TRACE_MAX_FORMAT_BUF_SIZE */

#ifndef ADV_TRACE_MAX_TIMESTAMP_SIZE
#define ADV_TRACE_MAX_TIMESTAMP_SIZE            (15U)         /*!< Default trace timestamp size */
#endif /* ADV_TRACE_MAX_TIMESTAMP_SIZE */

#ifndef ADV_TRACE_FIFO_SIZE
#define ADV_TRACE_FIFO_SIZE                     (1024U)       /*!< Default trace fifo size */
#endif /* ADV_TRACE_FIFO_SIZE */

/**
  *  @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup ADV_TRACE_EXPORTED_MACROS ADV_TRACE Exported Macros
  *  @{
  */

/**
  * @brief Default macro for initializing the critical section.
  */
#ifndef ADV_TRACE_INIT_CRITICAL_SECTION
#define ADV_TRACE_INIT_CRITICAL_SECTION( )
#endif /* ADV_TRACE_INIT_CRITICAL_SECTION */

/**
  * @brief Default macro for entering the critical section.
  */
#ifndef ADV_TRACE_ENTER_CRITICAL_SECTION
#define ADV_TRACE_ENTER_CRITICAL_SECTION( )      uint32_t primask_bit = __get_PRIMASK( ); \
  __disable_irq( )
#endif /* ADV_TRACE_ENTER_CRITICAL_SECTION */

/**
  * @brief Default macro for exiting a critical section.
  */
#ifndef ADV_TRACE_EXIT_CRITICAL_SECTION
#define ADV_TRACE_EXIT_CRITICAL_SECTION( )             __set_PRIMASK( primask_bit )
#endif /* ADV_TRACE_EXIT_CRITICAL_SECTION */

/**
  * @brief Default macro for calling memset to fill memory
  */
#ifndef ADV_TRACE_MEMSET8
#define ADV_TRACE_MEMSET8(dest, value, size)          memset((dest),(value),(size));
#endif /* ADV_TRACE_MEMSET8 */

/**
  * @brief Default macro for calling vsnprintf to format strings.
  */
#ifndef ADV_TRACE_VSNPRINTF
#define ADV_TRACE_VSNPRINTF(...)       vsnprintf(__VA_ARGS__)
#endif /* ADV_TRACE_VSNPRINTF */

/**
  *  @brief Advanced macro to override and enable debug mode.
  */
#ifndef ADV_TRACE_DEBUG
#define ADV_TRACE_DEBUG(...)
#endif /* ADV_TRACE_DEBUG */

/**
  *  @}
  */

/* Exported functions ------------------------------------------------------- */
/** @defgroup ADV_TRACE_EXPORTED_FUNCTIONS ADV_TRACE Exported Functions
  *  @{
  */

adv_trace_status ADV_TRACE_Init(void *p_obj);
adv_trace_status ADV_TRACE_DeInit(void);
uint8_t ADV_TRACE_IsBufferEmpty(void);
adv_trace_status ADV_TRACE_StartRxProcess(void (*UserCallback)(uint8_t *p_data, uint16_t size_byte,
                                                               uint8_t error));
adv_trace_status ADV_TRACE_FSend(const char *str_Format, ...);
adv_trace_status ADV_TRACE_Send(const uint8_t *p_data, uint16_t length);
adv_trace_status ADV_TRACE_ZCSend_Allocation(uint16_t length, uint8_t **p_data, uint16_t *fifo_size,
                                             uint16_t *write_pos);
adv_trace_status ADV_TRACE_ZCSend_Finalize(void);
void ADV_TRACE_PreSendHook(void);
void ADV_TRACE_PostSendHook(void);
#if defined(ADV_TRACE_OVERRUN)
void ADV_TRACE_RegisterOverRunFunction(cb_overrun *cb);
#endif /* ADV_TRACE_OVERRUN */

#if defined(ADV_TRACE_CONDITIONAL)
adv_trace_status ADV_TRACE_COND_FSend(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                      const char *str_Format, ...);
adv_trace_status ADV_TRACE_COND_ZCSend_Allocation(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                                  uint16_t length, uint8_t **p_data, uint16_t *fifo_size,
                                                  uint16_t *write_pos);
adv_trace_status ADV_TRACE_COND_ZCSend_Finalize(void);
adv_trace_status ADV_TRACE_COND_Send(uint32_t verbose_level, uint32_t region, uint32_t time_stamp_state,
                                     const uint8_t *p_data, uint16_t length);
void ADV_TRACE_RegisterTimeStampFunction(cb_timestamp *cb);
void ADV_TRACE_SetVerboseLevel(uint8_t level);
uint8_t ADV_TRACE_GetVerboseLevel(void);
void ADV_TRACE_SetRegion(uint32_t region);
uint32_t ADV_TRACE_GetRegion(void);
void ADV_TRACE_ResetRegion(uint32_t region);

#endif /* ADV_TRACE_CONDITIONAL */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*ADV_TRACE_CORE_H */
