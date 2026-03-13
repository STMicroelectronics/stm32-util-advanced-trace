/**
  **********************************************************************************************************************
  * @file    adv_trace_user_conf.h
  * @author  MCD Application Team
  * @brief   advanced trace configuration file
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */


/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef ADV_TRACE_USER_CONF_H
#define ADV_TRACE_USER_CONF_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include <string.h>
#include <stdio.h>

/* Exported macros ---------------------------------------------------------------------------------------------------*/
/* the define option
*    ADV_TRACE_CONDITIONAL  must be defined if you want use conditional function
*    ADV_TRACE_UNCHUNK_MODE must be defined if you want use the unchunk mode
*    ADV_TRACE_MEMLOCATION must be defined if you want manage trace buffer location
******************************************************************************/

/**
  * @brief macro used to initialize the critical section
  */
#define ADV_TRACE_INIT_CRITICAL_SECTION()

/**
  * @brief macro used to enter the critical section
  */
#define ADV_TRACE_ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
  __disable_irq()

/**
  * @brief macro used to exit the critical section
  */
#define ADV_TRACE_EXIT_CRITICAL_SECTION()           __set_PRIMASK(primask_bit)


#define ADV_TRACE_CONDITIONAL                                                       /*!< not used */
#define ADV_TRACE_UNCHUNK_MODE                                                      /*!< not used */
#define ADV_TRACE_DEBUG(...)                                                        /*!< not used */

#define ADV_TRACE_MAX_FORMAT_BUF_SIZE              (128U)                                /*!< trace formatting temporary buffer size */
#define ADV_TRACE_MAX_TIMESTAMP_SIZE               (128U)                                /*!< trace timestamp size */
#define ADV_TRACE_FIFO_SIZE                        (1024U)                               /*!< trace fifo size */

#define ADV_TRACE_MEMSET8(dest, value, size)       memset((dest),(value),(size))         /*!< memset utilities interface to trace feature */

#define ADV_TRACE_VSNPRINTF(...)                   vsnprintf(__VA_ARGS__)                /*!< vsnprintf utilities interface to trace feature */

#ifdef __cplusplus
}
#endif

#endif /* ADV_TRACE_USER_CONF_H */
