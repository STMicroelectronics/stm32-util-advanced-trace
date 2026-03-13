# Advanced Trace Software Pack

![tag](https://img.shields.io/badge/tag-2.0.0-brightgreen.svg)
[![release note](https://img.shields.io/badge/release_note-view_html-gold.svg)](https://htmlpreview.github.io/?https://github.com/STMicroelectronics/stm32-util-advanced-trace/blob/main/Release_Notes.html)

## Overview

The Advanced Trace utility is a trace management system for STM32 microcontrollers.


## Description and Usage

### Description

This utility is used for debugging or for communication with another system, such as a console or monitoring tool, over UART or a custom input/output interface.

The main features of Advanced Trace are:

- Adjustable FIFO size
- Custom FIFO memory address
- Conditional tracing (verbosity, region masks)
- Trace timestamping
- Overrun detection and reporting
- Unchunked continuous streaming
- Trace data reception by the application

### Usage

The start-up sequence can be split into three steps:

### Step 1: Advanced Trace initialization

Initialize the Advanced Trace with the selected interface handle by calling `ADV_TRACE_Init(&io_handle)`.

### Step 2: Reception initialization (optional)

Optionally enable reception by calling `ADV_TRACE_StartRxProcess(RxCallback)`

### Step 3: Trace sending

Send trace data using `ADV_TRACE_FSend(const char *fmt, ...)` over the configured interface.
