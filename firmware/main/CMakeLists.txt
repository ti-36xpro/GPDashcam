idf_component_register(SRCS "main.c"
                            "accelerometer_task.c"
							"gps_task.c"
                            "i2c_tools.c"
                       INCLUDE_DIRS "../include")
set(COMPONENT_KCONFIG main/Kconfig)
