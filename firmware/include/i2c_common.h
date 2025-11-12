#ifndef I2C_COMMON_H
#define I2C_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct { 
	i2c_master_bus_handle_t *i2c_bus;
	QueueHandle_t *queues[]; 
} i2c_task_args_t;

#endif // I2C_COMMON_H
