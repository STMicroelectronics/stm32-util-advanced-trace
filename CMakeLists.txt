# origin-pack: STMicroelectronics::advanced_trace@2.0.0
# file-format: 1.0.0
project(STMicroelectronics_advanced_trace_2_0_0)
cmake_minimum_required(VERSION 3.20)
add_library(STMicroelectronics_advanced_trace_2_0_0 INTERFACE)

# List of all CMSIS properties that influence conditions for this package
if(NOT DEFINED CMSIS_Dname)
  set(CMSIS_Dname "CMSIS_Dname not set")
  message(DEBUG "CMSIS_Dname is not set to any value")
endif()

if(NOT DEFINED CMSIS_Dvendor)
  set(CMSIS_Dvendor "CMSIS_Dvendor not set")
  message(DEBUG "CMSIS_Dvendor is not set to any value")
endif()


# Device specific defined symbols








# Enable all components in this package
if(CMSIS_ENTIRE_STMicroelectronics_advanced_trace_2_0_0)
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:Core#Cversion:1.0.0")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:itf_data#Cvariant:UART#Cversion:1.0.0")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:itf_data#Cvariant:Template#Cversion:1.0.0")
endif()

# All conditions used by this package

# condition: STMicroelectronics.advanced_trace.2.0.0
# description: 
if((CMSIS_Dname MATCHES "STM32.*" AND CMSIS_Dvendor STREQUAL "STMicroelectronics:13"))
  set(STMicroelectronics.advanced_trace.2.0.0 "1")
  message(DEBUG "CMSIS condition STMicroelectronics.advanced_trace.2.0.0 enabled")
else()
  set(STMicroelectronics.advanced_trace.2.0.0 0)
endif()


# condition: STMicroelectronics.advanced_trace.2.0.0:ADVANCED_TRACE Config
# description: STMicroelectronics Utility advanced_trace Config
set(STMicroelectronics.advanced_trace.2.0.0_ADVANCED_TRACE_Config "$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Utility#.*Cgroup:ADVANCED_TRACE#.*Csub:Core(#.*|$)>,>>")
message(DEBUG "CMSIS condition STMicroelectronics.advanced_trace.2.0.0_ADVANCED_TRACE_Config enabled")


# condition: STMicroelectronics.advanced_trace.2.0.0:STM32 HAL UART
# description: STMicroelectronics UART HAL Driver as low level component
set(STMicroelectronics.advanced_trace.2.0.0_STM32_HAL_UART "$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:UART(#.*|$)>,>>")
message(DEBUG "CMSIS condition STMicroelectronics.advanced_trace.2.0.0_STM32_HAL_UART enabled")


# condition: STMicroelectronics.advanced_trace.2.0.0:interface_data
# description: selection of interface data is required to use the component
set(STMicroelectronics.advanced_trace.2.0.0_interface_data "$<OR:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Utility#.*Cgroup:ADVANCED_TRACE#.*Csub:itf_data#.*Cvariant:UART(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Utility#.*Cgroup:ADVANCED_TRACE#.*Csub:itf_data#.*Cvariant:Template(#.*|$)>,>>>")
message(DEBUG "CMSIS condition STMicroelectronics.advanced_trace.2.0.0_interface_data enabled")

# Files and components in this package
if("Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:Core#Cversion:1.0.0" IN_LIST CMSIS_COMPONENTS_LIST)  # Advanced_Trace driver 
  message(DEBUG "Using component Utility_ADVANCED_TRACE_Core_1_0_0")
  target_compile_definitions(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_interface_data}:-DCMSIS_USE_Utility_ADVANCED_TRACE_Core_1_0_0=1>")
  target_include_directories(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_interface_data}:${CMAKE_CURRENT_LIST_DIR}/core>")
  target_include_directories(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_interface_data}:${CMAKE_CURRENT_LIST_DIR}/interface_data>")
  target_include_directories(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_interface_data}:${CMAKE_CURRENT_LIST_DIR}/templates>")
  target_sources(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_interface_data}:${CMAKE_CURRENT_LIST_DIR}/core/adv_trace_core.c>")
endif()

if("Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:itf_data#Cvariant:UART#Cversion:1.0.0" IN_LIST CMSIS_COMPONENTS_LIST)  # UART-based ready to use interface (no initialization and configuration)
  message(DEBUG "Using component Utility_ADVANCED_TRACE_itf_data_UART_1_0_0")
  target_compile_definitions(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_STM32_HAL_UART}:-DCMSIS_USE_Utility_ADVANCED_TRACE_itf_data_UART_1_0_0=1>")
  target_sources(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "$<${STMicroelectronics.advanced_trace.2.0.0_STM32_HAL_UART}:${CMAKE_CURRENT_LIST_DIR}/interface_data/adv_trace_itfdata_uart.c>")
endif()

if("Cvendor:STMicroelectronics#Cclass:Utility#Cgroup:ADVANCED_TRACE#Csub:itf_data#Cvariant:Template#Cversion:1.0.0" IN_LIST CMSIS_COMPONENTS_LIST)  # template interface: up to you to implement the services based on the template
  message(DEBUG "Using component Utility_ADVANCED_TRACE_itf_data_Template_1_0_0")
  target_compile_definitions(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "-DCMSIS_USE_Utility_ADVANCED_TRACE_itf_data_Template_1_0_0=1")
  if(EXISTS ${CMSIS_RTE_FOLDER}/Utility/adv_trace_itfdata_template.c)
    target_sources(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "${CMSIS_RTE_FOLDER}/Utility/adv_trace_itfdata_template.c")
  else()
    target_sources(STMicroelectronics_advanced_trace_2_0_0 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/interface_data/adv_trace_itfdata_template.c")
  endif()
endif()

