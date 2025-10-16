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

uint8_t i2cget(i2c_master_dev_handle_t *i2c_dev_handle, uint8_t data_addr, uint8_t *data);
uint8_t i2cset(i2c_master_dev_handle_t *i2c_dev_handle, uint8_t data_addr, uint8_t data);
extern i2c_master_bus_handle_t i2c_master_bus_handle;

#ifdef __cplusplus
}
#endif
