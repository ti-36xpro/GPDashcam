#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "accelerometer.h"
#include "gps.h"
#include "display.h"

QueueHandle_t accel_queue; 
QueueHandle_t gps_queue; 
accel_data_t accel_received; 
gps_data_t gps_received;


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

	gps_queue = xQueueCreate(10, sizeof(gps_data_t)); 
	accel_queue = xQueueCreate(10, sizeof(accel_data_t)); 

	accel_args_t accel_args = {
		.accel_queue = &accel_queue, 
		.i2c_bus = &i2c_bus
	}; 

	display_args_t display_args= {
		.accel_queue = &accel_queue, 
		.gps_queue = &gps_queue, 
		.i2c_bus = &i2c_bus
	}; 

	xTaskCreate(accelerometer_task, "Accelerometer task", 4500, &accel_args, 5, NULL); 
	xTaskCreate(gps_task, "GPS task", 4500, gps_queue, 5, NULL);
    xTaskCreate(sensor_display_task, "Display task", 4096, &display_args, 4, NULL);

	while(1){ 
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

