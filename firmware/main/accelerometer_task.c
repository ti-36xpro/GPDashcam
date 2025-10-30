#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "i2c_tools.h"
#include "accelerometer.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define I2C_FREQUENCY 100000
#define I2C_ACCEL_ADDR 0x53

void accelerometer_task(void *arg) { 
	static const char *TASK_TAG = "ACCELEROMETER_TASK";
	esp_log_level_set(TASK_TAG, ESP_LOG_INFO);

	static i2c_port_t i2c_port = I2C_NUM_0;

    i2c_master_bus_config_t i2c_master_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = i2c_port,
        .scl_io_num = CONFIG_SCL_PIN_NUM,
        .sda_io_num = CONFIG_SDA_PIN_NUM,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus_handle));

	// Instantiate I2C accelerometer 
    i2c_device_config_t i2c_accel_conf = {
        .scl_speed_hz = I2C_FREQUENCY,
        .device_address = I2C_ACCEL_ADDR,
    };
    i2c_master_dev_handle_t *i2c_accel_handle = malloc(sizeof(i2c_master_dev_handle_t));
    if (i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_accel_conf, i2c_accel_handle) != ESP_OK) {
        return;
    }
	
	QueueHandle_t queue = (QueueHandle_t)arg; 
	accel_t* accelerometer = malloc(sizeof(accel_t)); 
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
		
		accelerometer->x = (float)raw_x/128;
		accelerometer->y = (float)raw_y/128;
		accelerometer->z = (float)raw_z/128;

		if(xQueueSend(queue, accelerometer, pdMS_TO_TICKS(100)) != pdPASS){
			ESP_LOGW(TASK_TAG, "Queue full, dropping accelerometer data."); 
		}
		
		/*ESP_LOGI(TASK_TAG, "x=%.2f, y=%.2f, z=%.2f", (float)raw_x/128, (float)raw_y/128, (float)raw_z/128); */
		vTaskDelay(pdMS_TO_TICKS(500));
	}

	i2c_master_bus_rm_device(*i2c_accel_handle);
}

