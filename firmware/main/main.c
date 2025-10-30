#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "accelerometer.h"
#include "gps.h"

QueueHandle_t accel_queue; 
QueueHandle_t gps_queue; 
accel_t accel_received; 
rmc_statement gps_received;


void app_main(void) {
	gps_queue = xQueueCreate(10, sizeof(rmc_statement)); 
	accel_queue = xQueueCreate(10, sizeof(accel_t)); 
	xTaskCreate(accelerometer_task, "Accelerometer Task", 4500, accel_queue, 5, NULL); 
	xTaskCreate(gps_task, "GPS task", 4500, gps_queue, 5, NULL);

	while(1){ 
		if(xQueueReceive(accel_queue, &accel_received, portMAX_DELAY)){
			ESP_LOGI("Accelerometer", "x=%.2f, y=%.2f, z=%.2f", accel_received.x, accel_received.y, accel_received.z); 
		}
		if(xQueueReceive(gps_queue, &gps_received, portMAX_DELAY)){
			ESP_LOGI("GPS", "%02d-%02d-%04d %02d:%02d:%02d %f, %f, %fm/s, %f degrees, heading %s", 
				gps_received.day, 
				gps_received.month, 
				gps_received.year, 
				gps_received.hour, 
				gps_received.minute, 
				gps_received.second, 
				gps_received.latitude,
				gps_received.longitude,
				gps_received.speed,
				gps_received.cog,
				gps_received.direction
			); 
			
		}

	}
}

