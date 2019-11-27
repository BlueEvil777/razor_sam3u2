cmake_minimum_required(VERSION 3.15)
project(Rasor_Sam3u2 C)

set(CMAKE_C_STANDARD 11)

include_directories(firmware_ascii/bsp)
include_directories(firmware_ascii/drivers)
include_directories(firmware_common/application)
include_directories(firmware_common/bsp)
include_directories(firmware_common/cmsis)
include_directories(firmware_common/drivers)

add_executable(Rasor_Sam3u2
        firmware_ascii/bsp/eief1-pcb-01.c
        firmware_ascii/bsp/eief1-pcb-01.h
        firmware_ascii/drivers/lcd_nhd-c0220biz.c
        firmware_ascii/drivers/lcd_nhd-c0220biz.h
        firmware_common/application/debug.c
        firmware_common/application/debug.h
        firmware_common/application/main.c
        firmware_common/application/main.h
        firmware_common/application/music.h
        firmware_common/application/output_manager.c
        firmware_common/application/output_manager.h
        firmware_common/application/password_lock.c
        firmware_common/application/password_lock.h
        firmware_common/application/user_app3.c
        firmware_common/application/user_app3.h
        firmware_common/bsp/AT91SAM3U4.h
        firmware_common/bsp/board_cstartup_iar.c
        firmware_common/bsp/configuration.h
        firmware_common/bsp/typedefs.h
        firmware_common/cmsis/core_cm3.h
        firmware_common/drivers/adc12.c
        firmware_common/drivers/adc12.h
        firmware_common/drivers/ant.c
        firmware_common/drivers/ant.h
        firmware_common/drivers/ant_api.c
        firmware_common/drivers/ant_api.h
        firmware_common/drivers/antdefines.h
        firmware_common/drivers/antmessage.h
        firmware_common/drivers/buttons.c
        firmware_common/drivers/buttons.h
        firmware_common/drivers/exceptions.c
        firmware_common/drivers/exceptions.h
        firmware_common/drivers/interrupts.c
        firmware_common/drivers/interrupts.h
        firmware_common/drivers/leds.c
        firmware_common/drivers/leds.h
        firmware_common/drivers/messaging.c
        firmware_common/drivers/messaging.h
        firmware_common/drivers/sam3u_i2c.c
        firmware_common/drivers/sam3u_i2c.h
        firmware_common/drivers/sam3u_spi.c
        firmware_common/drivers/sam3u_spi.h
        firmware_common/drivers/sam3u_ssp.c
        firmware_common/drivers/sam3u_ssp.h
        firmware_common/drivers/sam3u_uart.c
        firmware_common/drivers/sam3u_uart.h
        firmware_common/drivers/timer.c
        firmware_common/drivers/timer.h
        firmware_common/drivers/utilities.c
        firmware_common/drivers/utilities.h)
