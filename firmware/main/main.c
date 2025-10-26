#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "accelerometer.h"


void app_main(void) {
	xTaskCreate(accelerometer_task, "Accelerometer Task", 2100, NULL, 5, NULL); 

	while(1) vTaskDelay(pdMS_TO_TICKS(10)); // Yields to other tasks; 
}

