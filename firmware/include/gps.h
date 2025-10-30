#ifndef GPS_H
#define GPS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Configuration constants
// -----------------------------------------------------------------------------
#define BAUD_RATE   9600
#define QUEUE_SIZE  16
#define RMC_SIZE    13
#define TIME_ZONE   -4
#define RX_BUF_SIZE 1024

// -----------------------------------------------------------------------------
// Direction strings (N, NE, E, SE, etc.)
// -----------------------------------------------------------------------------
static const char* direction_str[] = {
    "N",
    "NE",
    "E",
    "SE",
    "S",
    "SW",
    "W",
    "NW"
};

// -----------------------------------------------------------------------------
// RMC (Recommended Minimum Specific GNSS Data) structure
// -----------------------------------------------------------------------------
typedef struct {
    uint8_t  valid;
    char     raw_time[10];
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    char     raw_date[7];
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
    float    latitude;
    float    longitude;
    float    speed;
    float    cog;         // course over ground
    char*    direction;   // human-readable compass direction
} rmc_statement;

// -----------------------------------------------------------------------------
// Function declarations
// -----------------------------------------------------------------------------

/**
 * @brief Convert two ASCII digits (e.g. "45") to a uint8_t number (45).
 */
uint8_t convert_two_digit2number(const char *digit_char);

/**
 * @brief Get the number of days in a given month (non-leap year).
 */
uint8_t get_days_of_month(rmc_statement* gps);

/**
 * @brief Parse and adjust time/date fields in an RMC sentence.
 */
void parse_date_time(rmc_statement* gps);

/**
 * @brief Convert course in degrees to compass direction index (0–7).
 */
uint8_t degrees_to_compass_direction(float degrees);

/**
 * @brief Convert latitude/longitude string (DDMM.MMMM) to decimal degrees.
 */
float parse_lat_long(const char* value, const char* ll_direction);

/**
 * @brief Parse all RMC fields into the provided rmc_statement structure.
 */
void parse_fields(char *fields[], rmc_statement *gps);

/**
 * @brief FreeRTOS task to read GPS NMEA sentences and parse RMC data.
 *
 * The task initializes UART1, reads incoming NMEA data,
 * splits the RMC sentence into comma-separated fields (including empty ones),
 * and logs the parsed data.
 */
void gps_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif // GPS_H

