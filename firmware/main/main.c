#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "accelerometer.h"
#include "gps.h"
#include "display.h"
#include "sd_card.h"

#define MAIN_TAG "MAIN_TASK"

void app_main(void) {
	// Initialize I2C bus 
	ESP_LOGI(MAIN_TAG, "Initialize I2C bus");
	i2c_master_bus_handle_t i2c_bus;
	i2c_master_bus_config_t bus_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = 0,
		.scl_io_num = CONFIG_SCL_PIN_NUM,
		.sda_io_num = CONFIG_SDA_PIN_NUM,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = true,
	};
	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

	// Initialize queues 
	ESP_LOGI(MAIN_TAG, "Create queues");
	QueueHandle_t accel_queue; 
	QueueHandle_t gps_queue; 
	gps_queue = xQueueCreate(10, sizeof(gps_data_t)); 
	accel_queue = xQueueCreate(1, sizeof(accel_data_t)); 

	// Set up task arguments
	ESP_LOGI(MAIN_TAG, "Assemble task arguments");
	accel_args_t accel_args = {
		.accel_queue = &accel_queue, 
		.i2c_bus = &i2c_bus
	}; 

	display_args_t display_args= {
		.accel_queue = &accel_queue, 
		.gps_queue = &gps_queue, 
		.i2c_bus = &i2c_bus
	}; 

	// Create tasks 
	ESP_LOGI(MAIN_TAG, "Creating tasks");
	xTaskCreate(accelerometer_task, ACCEL_TAG, 2500, &accel_args, 4, NULL); 
	xTaskCreate(gps_task, GPS_TAG, 4500, gps_queue, 4, NULL);
    xTaskCreate(sensor_display_task, DISPLAY_TAG, 4096, &display_args, 4, NULL);
    xTaskCreate(sd_card_task, SD_CARD_TAG, 4096, NULL, 4, NULL);

	while(1){ 
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
	free(gps_queue); 
	free(accel_queue); 
}

