#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "accelerometer.h"
#include "gps.h"


void app_main(void) {
	xTaskCreate(accelerometer_task, "Accelerometer Task", 2500, NULL, 5, NULL); 
    xTaskCreate(gps_task, "GPS task", 2500, NULL, 5, NULL);

	/*while(1) vTaskDelay(pdMS_TO_TICKS(10000)); // Yields to other tasks; */
}

