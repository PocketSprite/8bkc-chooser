/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

/*
This is example code for the esphttpd library. It's a small-ish demo showing off 
the server, including WiFi connection management capabilities, some IO and
some pictures of cats.
*/

#include "sdkconfig.h"
#include "httpd.h"
#include "httpdespfs.h"
#include "cgiwifi.h"
#include "cgiflash.h"
#include "cgiromflash.h"
#include "auth.h"
#include "espfs.h"
#include "captdns.h"
#include "webpages-espfs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "soc/timer_group_struct.h"
#include "soc/dport_reg.h"

#include "rom/rtc.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

#include "appfs.h"
#include "8bkc-hal.h"

#include "gui.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

/*
This is the main url->function dispatching data struct.
In short, it's a struct with various URLs plus their handlers. The handlers can
be 'standard' CGI functions you wrote, or 'special' CGIs requiring an argument.
They can also be auth-functions. An asterisk will match any url starting with
everything before the asterisks; "*" matches everything. The list will be
handled top-down, so make sure to put more specific rules above the more
general ones. Authorization things (like authBasic) act as a 'barrier' and
should be placed above the URLs they protect.
*/
HttpdBuiltInUrl builtInUrls[]={
	{"*", cgiRedirectApClientToHostname, "esp8266.nonet"},
	{"/", cgiRedirect, "/index.html"},
	{"/reboot.cgi", cgiRebootFirmware, NULL},
	{"/upload.cgi", cgiUploadRom, NULL},
	{"/romidx.cgi", cgiRomIdx, NULL},
	{"/delete.cgi", cgiDelete, NULL},
	{"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
	{NULL, NULL, NULL}
};


void handleCharging() {
	int r;

	guiInit();
	guiCharging();


	//Disable app cpu
	DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
	//Speed down
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_2M);

	do {
		r=kchal_get_chg_status();
		if (r==KC_CHG_CHARGING) {
			guiCharging();
			printf("Charging...\n");
		} else if (r==KC_CHG_FULL) {
			guiFull();
			printf("Full!\n");
		}
		vTaskDelay(1);
	} while (r!=KC_CHG_NOCHARGER);
	printf("Charger gone. Shutting down.\n");

	rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
	kchal_power_down();
}

//Main routine. Initialize stdout, the I/O, filesystem and the webserver and we're done.
int app_main(void)
{
	kchal_init();
//ToDo: make this into a menuconfig thing
#if CONFIG_CHARGE_MODE
	if (ioGetChgStatus()!=IO_CHG_NOCHARGER) handleCharging();
#endif

	appfsDump();

	printf("Starting webserver...\n");
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
	wifi_config_t ap_config = {
		.ap = {
			.ssid = "gbfemto",
			.authmode=WIFI_AUTH_OPEN,
			.max_connection = 2,
		}
	};
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
//	ESP_ERROR_CHECK( esp_wifi_connect() );

//	captdnsInit();

	espFsInit((void*)(webpages_espfs_start));
	httpdInit(builtInUrls, 80);

	guiInit();

	printf("\nReady\n");

	guiMenu();

	return 0;
}

