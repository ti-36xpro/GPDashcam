#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "accelerometer.h"
#include "gps.h"
#include "display.h"

QueueHandle_t accel_queue; 
QueueHandle_t gps_queue; 
accel_t accel_received; 
rmc_statement gps_received;


void app_main(void) {
	ESP_LOGI("Main", "Initialize I2C bus");
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

	gps_queue = xQueueCreate(10, sizeof(rmc_statement)); 
	accel_queue = xQueueCreate(10, sizeof(accel_t)); 

	accel_params_t accel_params = {
		.accel_queue = &accel_queue, 
		.i2c_bus = &i2c_bus
	}; 

	display_params_t display_params= {
		.accel_queue = &accel_queue, 
		.gps_queue = &gps_queue, 
		.i2c_bus = &i2c_bus
	}; 

	xTaskCreate(accelerometer_task, "Accelerometer task", 4500, &accel_params, 5, NULL); 
	xTaskCreate(gps_task, "GPS task", 4500, gps_queue, 5, NULL);
    xTaskCreate(sensor_display_task, "Display task", 4096, &display_params, 4, NULL);

	while(1){ 
		/*if(xQueueReceive(accel_queue, &accel_received, portMAX_DELAY)){*/
		/*	ESP_LOGI("Accelerometer", "x=%.2f, y=%.2f, z=%.2f", accel_received.x, accel_received.y, accel_received.z); */
		/*}*/
		/*if(xQueueReceive(gps_queue, &gps_received, portMAX_DELAY)){*/
		/*	ESP_LOGI("GPS", "%02d-%02d-%04d %02d:%02d:%02d %f, %f, %fm/s, %f degrees, heading %s", */
		/*		gps_received.day, */
		/*		gps_received.month, */
		/*		gps_received.year, */
		/*		gps_received.hour, */
		/*		gps_received.minute, */
		/*		gps_received.second, */
		/*		gps_received.latitude,*/
		/*		gps_received.longitude,*/
		/*		gps_received.speed,*/
		/*		gps_received.cog,*/
		/*		gps_received.direction*/
		/*	); */
		/**/
		/*}*/
		vTaskDelay(pdMS_TO_TICKS(10000));

	}
}

