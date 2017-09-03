#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include "gui.h"
#include "ugui.h"
#include "8bkc-hal.h"
#include "8bkc-ugui.h"
#include "appfs.h"

void guiCharging() {
	kcugui_cls();
	UG_FontSelect(&FONT_6X8);
	UG_SetForecolor(C_WHITE);
	UG_PutString(0, 0, "CHARGING");
	kcugui_flush();
}

void guiFull() {
	kcugui_cls();
	UG_FontSelect(&FONT_6X8);
	UG_SetForecolor(C_GREEN);
	UG_PutString(0, 0, "CHARGED");
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

static int fccallback(int button, char **glob, char **desc, void *usrptr) {
	if (button & KC_BTN_POWER) kchal_power_down();
	return 0;
}


void guiMenu() {
	int fd=kcugui_filechooser("*.app,*.bin", "CHOOSE APP", fccallback, NULL);
	kchal_set_new_app(fd);
	kchal_boot_into_new_app();
}


