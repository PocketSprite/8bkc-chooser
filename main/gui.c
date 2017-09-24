#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include "gui.h"
#include "ugui.h"
#include "8bkc-hal.h"
#include "8bkc-ugui.h"
#include "appfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "nvs.h"
#include "nvs_flash.h"

static const uint8_t gfx[]={
#include "graphics.inc"
};

#define GFX_W 41
#define GFX_H 33
#define GFX_IH 11

#define GFX_O_EMPTY 0
#define GFX_O_FULL 1
#define GFX_O_CHG 2


void drawIcon(int px, int py, int o) {
	const uint8_t *p=&gfx[o*GFX_IH*GFX_W*4];
	for (int y=0; y<GFX_IH; y++) {
		for (int x=0; x<GFX_W; x++) {
			UG_DrawPixel(x+px, y+py, kchal_ugui_rgb(p[0],p[1],p[2]));
			p+=4;
		}
	}
}


void guiCharging() {
	kcugui_cls();
	drawIcon(20, 26, GFX_O_CHG);
	kcugui_flush();
}

void guiFull() {
	kcugui_cls();
	drawIcon(20, 26, GFX_O_FULL);
	kcugui_flush();
}

void guiBatEmpty() {
	kcugui_cls();
	drawIcon(20, 26, GFX_O_EMPTY);
	kcugui_flush();
}

void guiInit() {
	kcugui_init();
	UG_FontSelect(&FONT_6X8);
	UG_SetForecolor(C_WHITE);
	UG_PutString(0, 0, "WIFI AP");
	UG_SetForecolor(C_YELLOW);
	UG_PutString(0, 8, "gbfemto");
	UG_SetForecolor(C_WHITE);
	UG_PutString(0, 16, "GO TO:");
	UG_SetForecolor(C_YELLOW);
	UG_PutString(0, 24, "HTTP://192.168.4.1/");

	UG_SetForecolor(C_BLACK);
	UG_SetBackcolor(C_WHITE);
	UG_PutString(30, 56, "OK");
	UG_SetBackcolor(C_BLACK);

	kcugui_flush();
}

#define ST_START 0
#define ST_SELECT 1
#define ST_REBOOT 2


#include "8bkcgui-widgets.h"

static void debug_screen() {
	char buf[32];
	int k, v;
	int selReleased=0;
	nvs_handle nvsHandle=NULL;
	uint32_t battFullAdcVal=0;
	esp_err_t r=nvs_open("8bkc", NVS_READONLY, &nvsHandle);
	if (r==ESP_OK && nvsHandle!=NULL) {
		nvs_get_u32(nvsHandle, "batadc", &battFullAdcVal);
	}
	nvs_close(nvsHandle);
	while(1) {
		kcugui_cls();
		UG_FontSelect(&FONT_6X8);
		UG_SetForecolor(C_WHITE);
		UG_PutString(0, 0, "DEBUG");
		k=kchal_get_keys();
		sprintf(buf, "KEYS: %X", k);
		UG_PutString(0, 16, buf);
		v=kchal_get_chg_status();
		sprintf(buf, "CHG: %X", v);
		UG_PutString(0, 24, buf);
		v=kchal_get_bat_mv();
		sprintf(buf, "VBATMV:%d", v);
		UG_PutString(0, 32, buf);
		sprintf(buf, "ADCCAL:%d", battFullAdcVal);
		UG_PutString(0, 40, buf);
		kcugui_flush();
		vTaskDelay(100/portTICK_RATE_MS);
		if (k&KC_BTN_SELECT) {
			if (selReleased) break;
		} else {
			selReleased=1;
		}
	}
}

static int fccallback(int button, char **glob, char **desc, void *usrptr) {
	if (button & KC_BTN_POWER) kchal_power_down();
	if (button & KC_BTN_SELECT) debug_screen();
	return 0;
}


void guiMenu() {
	int fd=kcugui_filechooser("*.app,*.bin", "CHOOSE APP", fccallback, NULL);
	while(kchal_get_keys()); //wait till btn released
	kchal_set_new_app(fd);
	kchal_boot_into_new_app();
}


