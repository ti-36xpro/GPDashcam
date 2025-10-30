#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"

#define I2C_FREQUENCY 100000
#define I2C_ACCEL_ADDR 0x53

/**
 * @brief Struct representing acceleration data in 3 axes (x, y, z).
 */
typedef struct {
    float x;  // Acceleration in the X axis (g) 
    float y;  // Acceleration in the Y axis (g) 
    float z;  // Acceleration in the Z axis (g) 
} accel_t;

/**
 * @brief FreeRTOS task for continuously reading accelerometer data over I2C.
 *
 * @param pvParameters Optional task parameter (unused by default).
 *
 * The task initializes the I2C bus, configures the accelerometer, and
 * periodically reads acceleration data.
 */
void accelerometer_task(void *pvParameters);

#endif // ACCELEROMETER_H

