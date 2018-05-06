/*
Misc cgi routines
*/
#include <esp8266.h>
#include "cgiappfs.h"
#include "espfs.h"
#include "cgiflash.h"
#include "espfs.h"
#include "esp_partition.h"

//#include <osapi.h>
#include "cgiflash.h"
#include "espfs.h"
#include "httpd-platform.h"
#include "esp32_flash.h"
#include "appfs.h"
#include "8bkc-hal.h"
#include "esp_system.h"

static void cbPowerOff(TimerHandle_t xTimer) {
	kchal_power_down();
}

static void cbReset(TimerHandle_t xTimer) {
	system_restart();
}

/*

int cgiPowerOff(HttpdConnData *connData) {
	TimerHandle_t t=xTimerCreate("t", pdMS_TO_TICKS(500), pdFALSE, NULL, cbPowerOff);
	xTimerStart(t, 0);
	httpdStartResponse(connData, 302);
	httpdHeader(connData, "Content-Type", "text/html");
	httpdEndHeaders(connData);
	httpdSend(connData, "OK", -1);
	return HTTPD_CGI_DONE;
}


int cgiReset(HttpdConnData *connData) {
	TimerHandle_t t=xTimerCreate("t", pdMS_TO_TICKS(500), pdFALSE, NULL, cbReset);
	xTimerStart(t, 0);
	httpdStartResponse(connData, 302);
	httpdHeader(connData, "Content-Type", "text/html");
	httpdEndHeaders(connData);
	httpdSend(connData, "OK", -1);
	return HTTPD_CGI_DONE;
}

*/