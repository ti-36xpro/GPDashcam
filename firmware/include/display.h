#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"

#define TAG "lcd_simple"

// I2C address and LCD configuration
#define I2C_ADDR                0x3C
#define LCD_H                   128
#define LCD_V                   64
#define LCD_PIXEL_CLOCK_HZ      (400 * 1000)

typedef struct {
	QueueHandle_t *accel_queue;
	QueueHandle_t *gps_queue;
	i2c_master_bus_handle_t *i2c_bus;
} display_args_t;

// Public function declarations
void sensor_display_task(void *arg);
void draw_char(uint8_t x, uint8_t y, char c);
void draw_string(uint8_t x, uint8_t y, const char *str);

#endif // LCD_DISPLAY_H
