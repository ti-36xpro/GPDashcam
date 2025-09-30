/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "cmd_i2ctools.h"
#include "driver/i2c_master.h"

#define I2C_FREQUENCY 100000
#define I2C_ACCEL_ADDR 0x53


static gpio_num_t i2c_gpio_sda = CONFIG_EXAMPLE_I2C_MASTER_SDA;
static gpio_num_t i2c_gpio_scl = CONFIG_EXAMPLE_I2C_MASTER_SCL;
static i2c_port_t i2c_port = I2C_NUM_0;
static uint8_t *data_byte; 
static int16_t raw_x; 



void app_main(void) {
    i2c_master_bus_config_t i2c_master_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = i2c_port,
        .scl_io_num = i2c_gpio_scl,
        .sda_io_num = i2c_gpio_sda,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus_handle));

	// Instantiate I2C accelerometer 
    i2c_device_config_t i2c_accel_conf = {
        .scl_speed_hz = I2C_FREQUENCY,
        .device_address = I2C_ACCEL_ADDR,
    };
    i2c_master_dev_handle_t i2c_accel_handle;
    if (i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_accel_conf, &i2c_accel_handle) != ESP_OK) {
        return;
    }



	data_byte = malloc(1); 
	i2cget(i2c_accel_handle, 0x32, data_byte);
	i2cset(i2c_accel_handle, 0x2d, 0x00);
	i2cset(i2c_accel_handle, 0x2d, 0x08);
	i2cset(i2c_accel_handle, 0x31, 0x01);

	i2cget(i2c_accel_handle, 0x32, data_byte);
	printf("\n0x32: 0x%x\n",*data_byte); 
	raw_x = *data_byte; 

	i2cget(i2c_accel_handle, 0x33, data_byte);
	printf("\n0x33: 0x%x\n",*data_byte); 
	raw_x = raw_x | *data_byte << 8; 

	printf("\nx-axis: %f\n", (float)raw_x/128); 

	if (i2c_master_bus_rm_device(i2c_accel_handle) != ESP_OK) {
		return;
	}
	free(data_byte);
}


