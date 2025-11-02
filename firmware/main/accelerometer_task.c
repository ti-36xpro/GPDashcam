#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "i2c_tools.h"
#include "accelerometer.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

void accelerometer_task(void *args) { 
	static const char *TASK_TAG = "ACCELEROMETER_TASK";
	esp_log_level_set(TASK_TAG, ESP_LOG_INFO);

	// Grab passed in arguments 
	QueueHandle_t *accel_queue = ((accel_args_t *)args)->accel_queue; 
	i2c_master_bus_handle_t *i2c_bus = ((accel_args_t *)args)->i2c_bus; 

	// Instantiate I2C accelerometer and add to bus 
    i2c_device_config_t i2c_accel_conf = {
        .scl_speed_hz = I2C_FREQUENCY,
        .device_address = I2C_ACCEL_ADDR,
    };
    i2c_master_dev_handle_t *i2c_accel_handle = malloc(sizeof(i2c_master_dev_handle_t));
    if (i2c_master_bus_add_device(*i2c_bus, &i2c_accel_conf, i2c_accel_handle) != ESP_OK) {
        return;
    }
	
	accel_data_t* accel_data = malloc(sizeof(accel_data_t)); 
	int16_t raw_x; 
	int16_t raw_y; 
	int16_t raw_z; 

	uint8_t* data_byte = malloc(sizeof(uint8_t)); 

	// TODO: Figure out why I2C device needs to be woken up. 
	// Current inelegant solution is to poke it with a read. 
	i2cget(i2c_accel_handle, 0x32, data_byte);

	// Configure accelerometer 
	i2cset(i2c_accel_handle, 0x2d, 0x00);
	i2cset(i2c_accel_handle, 0x2d, 0x08);
	i2cset(i2c_accel_handle, 0x31, 0x01);

	// Continuously retreive accel data 
	while (1) { 
		i2cget(i2c_accel_handle, 0x32, data_byte);
		raw_x = *data_byte; 
		i2cget(i2c_accel_handle, 0x33, data_byte);
		raw_x = raw_x | *data_byte << 8; 
		i2cget(i2c_accel_handle, 0x34, data_byte);
		raw_y = *data_byte; 
		i2cget(i2c_accel_handle, 0x35, data_byte);
		raw_y = raw_y | *data_byte << 8; 
		i2cget(i2c_accel_handle, 0x36, data_byte);
		raw_z = *data_byte; 
		i2cget(i2c_accel_handle, 0x37, data_byte);
		raw_z = raw_z | *data_byte << 8; 
		
		accel_data->x = (float)raw_x/128;
		accel_data->y = (float)raw_y/128;
		accel_data->z = (float)raw_z/128;

		/*ESP_LOGI(TASK_TAG, "x=%.2f, y=%.2f, z=%.2f", (float)raw_x/128, (float)raw_y/128, (float)raw_z/128); */
		if(xQueueSend(*accel_queue, accel_data, pdMS_TO_TICKS(100)) != pdPASS){
			xQueueReset(*accel_queue); 
		}
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
	}

	// If task exits somehow, clean up 
	free(accel_data); 
	free(data_byte); 
	i2c_master_bus_rm_device(*i2c_accel_handle);
}

