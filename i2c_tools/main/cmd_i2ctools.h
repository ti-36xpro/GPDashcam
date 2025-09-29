/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t i2cget(uint8_t chip_addr, uint8_t data_addr, uint8_t *data); 
uint8_t i2cset(uint8_t chip_addr, uint8_t data_addr, uint8_t data);
extern i2c_master_bus_handle_t tool_bus_handle;

#ifdef __cplusplus
}
#endif
