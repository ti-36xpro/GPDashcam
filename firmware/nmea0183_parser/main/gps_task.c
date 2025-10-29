/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#define RXD_PIN 21
#define BAUD_RATE 9600
#define QUEUE_SIZE 16
#define RMC_SIZE 13
#define TIME_ZONE -4

static const int RX_BUF_SIZE = 1024;
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

typedef struct { 
	uint8_t valid; 
	char raw_time[10]; 
	uint8_t hour; 
	uint8_t minute;  
	uint8_t second; 
	char raw_date[7]; 
	uint8_t day; 
	uint8_t month; 
	uint16_t year; 
	float latitude; 
	float longitude; 
	float speed; 
	float cog; 
	char* direction; 
} rmc_statement;

static inline uint8_t convert_two_digit2number(const char *digit_char)
{
    return 10 * (digit_char[0] - '0') + (digit_char[1] - '0');
}

static uint8_t get_days_of_month(rmc_statement* gps){
	switch (gps->month) { 
		case 1: 
			return 31; 
		case 2: 
			return 28; 
		case 3: 
			return 31;
		case 4: 
			return 30;
		case 5: 
			return 31;
		case 6: 
			return 30; 
		case 7: 
			return 31;
		case 8: 
			return 31; 
		case 9: 
			return 30; 
		case 10: 
			return 31; 
		case 11: 
			return 30; 
		case 12: 
			return 31; 
	}
	return 0; 
}

static void parse_date_time(rmc_statement* gps) {
	gps->day = convert_two_digit2number(gps->raw_date + 0); 
	gps->month = convert_two_digit2number(gps->raw_date + 2); 
	gps->year = convert_two_digit2number(gps->raw_date + 4) + 2000; 

	int8_t adjusted_time = convert_two_digit2number(gps->raw_time) + TIME_ZONE;
	if(adjusted_time < 0) { // Wrap around midnight 
		gps-> hour = adjusted_time + 24; 
		if(gps->day == 1){ // Rollback the month
			gps->day = get_days_of_month(gps); 
			gps->month -= 1; 
		} else { 
			gps->day -= 1; 
		}
	}
	gps->hour = adjusted_time; 
    gps->minute = convert_two_digit2number(gps->raw_time + 2);
    gps->second = convert_two_digit2number(gps->raw_time + 4);
}

static uint8_t degrees_to_compass_direction(float degrees){
	return (uint8_t)((uint16_t)((degrees + 22.5) / 45) % 8);
}

static float parse_lat_long(const char* value, const char* ll_direction) {
    float ll = strtof(value, NULL);
    int deg = ((int)ll) / 100;
    float min = ll - (deg * 100);
    ll = deg + min / 60.0f;
	if(ll_direction[0] == 'N' || ll_direction[0] == 'E') return ll;
	return -ll;
}

static void parse_fields(char *fields[], rmc_statement *gps){ 
	// Parse for time and date 
	if(fields[1] && fields[1][0] != '\0' && fields[9] && fields[9][0] != '\0') {
		strncpy(gps->raw_time, fields[1], 9); 
		gps->raw_time[9] = '\0';
		strncpy(gps->raw_date, fields[9], 6); 
		gps->raw_date[6] = '\0';
	parse_date_time(gps); 
	}
	// Parse for latitude and longitude 
	if(fields[3] && fields[3][0] != '\0' &&
	   fields[4] && fields[4][0] != '\0' &&
	   fields[5] && fields[5][0] != '\0' &&
	   fields[6] && fields[6][0] != '\0') {
		gps->latitude = parse_lat_long(fields[3], fields[4]); 
		gps->longitude = parse_lat_long(fields[5], fields[6]); 
	}
	// Parse for speed 
	if(fields[7] && fields[7][0] != '\0') gps->speed = strtof(fields[7], NULL) * 0.514444; 
	// Parse for heading 
	if(fields[8] && fields[8][0] != '\0') {
		gps->cog = strtof(fields[8], NULL);
		gps->direction = (char *)direction_str[degrees_to_compass_direction(gps->cog)]; 
	} else {
		gps->cog = 0;
		gps->direction = ""; 
	}
}

static void rx_task(void *arg) {
	static const char *RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

	QueueHandle_t event_queue;
	// Initalize UART 
    const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, RX_BUF_SIZE, 0, QUEUE_SIZE, &event_queue, 0));
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_flush(UART_NUM_1); 


    char *buffer = (char*) malloc(RX_BUF_SIZE + 1); // +1 to ensure '\0' can always be appended
	rmc_statement *gps = malloc(sizeof(rmc_statement)); 
	char *fields[RMC_SIZE]; 
	char *sentence_remainder; 
	uint8_t field_count; 

	ESP_LOGI(RX_TASK_TAG, "Initialized"); 

	while(1) {
		uint8_t len = uart_read_bytes(UART_NUM_1, buffer, RX_BUF_SIZE, pdMS_TO_TICKS(100));
		if(len > 0) {
			char *sentence = buffer;
			// Loop through all received bytes 
			for(uint8_t i=0;i<len;i++) {
				if(buffer[i]=='\n') {
					// Replace detected newline with the string complete character 
					buffer[i]='\0';
					if (sentence[5] == 'C') { 
						field_count = 0; 
						memset(gps, 0, sizeof(rmc_statement));
						sentence_remainder = sentence; 
						// Parse RMC statement into an array using comma separation 
						// Empty fields are still recorded into array to keep consistent
						// indexing
						for (char *p = sentence; ; ++p) {
							if (*p == ',' || *p == '\0') {
								// temporarily terminate current field
								char temp = *p;
								*p = '\0';

								// store pointer to start of field (even if empty)
								fields[field_count++] = sentence_remainder;

								// restore character if not end of string
								if (temp == '\0' || field_count >= RMC_SIZE)
									break;

								// next field starts after comma
								sentence_remainder = p + 1;
							}
						}

						/*ESP_LOGI(RX_TASK_TAG, "%s", fields[1]);*/
						parse_fields(fields, gps); 
						ESP_LOGI(RX_TASK_TAG, "%02d-%02d-%04d %02d:%02d:%02d %f, %f, %fm/s, %f degrees, heading %s", 
							gps->day, 
							gps->month, 
							gps->year, 
							gps->hour, 
							gps->minute, 
							gps->second, 
							gps->latitude,
							gps->longitude,
							gps->speed,
							gps->cog,
							gps->direction
						); 
						UBaseType_t remaining = uxTaskGetStackHighWaterMark(NULL);
						printf("Task stack high-water mark: %u words\n", remaining);
					}
					sentence = buffer+i+1; // Shift line read pointer over 
				}
			}

		}
	}
    free(buffer);
	free(gps); 
}

void app_main(void)
{
    xTaskCreate(rx_task, "uart_rx_task", 2500, NULL, configMAX_PRIORITIES - 1, NULL);
}
