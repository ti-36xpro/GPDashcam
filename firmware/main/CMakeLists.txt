idf_component_register(
	SRCS 
	"main.c"
	"accelerometer_task.c"
	"gps_task.c"
	"display_task.c"
	"sd_task.c"
	"camera.c"
	INCLUDE_DIRS "../include"
	REQUIRES fatfs sd_card esp_lcd esp32-camera
	WHOLE_ARCHIVE)
set(COMPONENT_KCONFIG main/Kconfig)
