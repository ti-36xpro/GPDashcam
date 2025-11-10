idf_component_register(
	SRCS 
	"main.c"
	"accelerometer_task.c"
	"gps_task.c"
	"display_task.c"
	"i2c_tools.c"
	"sd_card_task.c"
	INCLUDE_DIRS "../include"
	REQUIRES fatfs sd_card esp_lcd
	WHOLE_ARCHIVE)
set(COMPONENT_KCONFIG main/Kconfig)
