#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

EXTRA_COMPONENT_DIRS := $(POCKETSPRITE_PATH)/8bkc-components/
IDF_PATH := $(POCKETSPRITE_PATH)/esp-idf

PROJECT_NAME := chooser

COMPONENTS := 8bkc-hal app_update appfs aws_iot bootloader bootloader_support bt console \
cxx driver esp32 esptool_py freertos gui-util heap micro-ecc\
libesphttpd log lwip main mbedtls mkappfs newlib nvs_flash app_trace\
partition_table soc spi_flash tcpip_adapter ugui ulp vfs wpa_supplicant \
xtensa-debug-module ethernet pthread 

# jsmn json wear_levelling fatfs expat nghttp spiffs libsodium mdns openssl sdmmc


include $(IDF_PATH)/make/project.mk

