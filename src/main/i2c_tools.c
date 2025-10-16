/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* cmd_i2ctools.c

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "i2ctools";

#define I2C_TOOL_TIMEOUT_VALUE_MS (50)
i2c_master_bus_handle_t i2c_master_bus_handle;

uint8_t i2cget(i2c_master_dev_handle_t *i2c_dev_handle, uint8_t data_addr, uint8_t *data) {
    uint8_t len = 1;

    esp_err_t ret = i2c_master_transmit_receive(*i2c_dev_handle, (uint8_t*)&data_addr, 1, data, len, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret == ESP_OK) {
    } else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "Bus is busy");
		return 1; 
    } else {
        ESP_LOGW(TAG, "Read failed");
		return 1; 
    }
    return 0;
}

uint8_t i2cset(i2c_master_dev_handle_t *i2c_dev_handle, uint8_t data_addr, uint8_t data) {
    int len = 1;

    uint8_t *register_data = malloc(len + 1);
    register_data[0] = data_addr;
	register_data[1] = data;
    esp_err_t ret = i2c_master_transmit(*i2c_dev_handle, register_data, len + 1, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret == ESP_OK) {
		ESP_LOGI(TAG, "Write OK");
	} else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "Bus is busy");
		return 1; 
    } else {
        ESP_LOGW(TAG, "Write Failed");
		return 1; 
    }

    free(register_data);
    return 0;
}
