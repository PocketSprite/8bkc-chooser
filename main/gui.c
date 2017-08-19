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


static int state=ST_START;
static int pos=0, scpos=0;

void guiKey(int key) {
	static int oldKey=1;
	if (oldKey!=0 || key==0) {
		oldKey=key;
		return;
	}
	oldKey=key;
	printf("Key %d\n", key);

	//Calculated during display phase
	static int currIdx, maxPos;
	int x, r;
	char rname[13];

	//Key handling
	if (state==ST_START) {
		state=ST_SELECT;
	} else if (state==ST_SELECT) {
		if (key==KC_BTN_UP) {
			if (pos==0) {
				if (scpos!=0) scpos--;
			} else {
				pos--;
			}
		} else if (key==KC_BTN_DOWN) {
			if (pos==4) {
				if (scpos<maxPos-5) scpos++;
			} else {
				pos++;
			}
		} else if (key==KC_BTN_A) {
//			romSetCurr(currIdx);
		}
		printf("pos %d scpos %d\n", pos, scpos);
	}

	//Display handling
	kcugui_cls();
	if (state==ST_SELECT) {
		const char *name;
		int fd=APPFS_INVALID_FD;
		
		for (x=0; x<scpos; x++) {
			fd=appfsNextEntry(fd);
		}
		
		for (x=0; x<8; x++) {
			if (fd!=APPFS_INVALID_FD || (scpos==0 && x==0)) {
				fd=appfsNextEntry(fd);
			}
			if (fd!=APPFS_INVALID_FD) {
				appfsEntryInfo(fd, &name, NULL);
			} else {
				maxPos=scpos+x;
				break;
			}
			if (x==pos) {
				UG_SetForecolor(C_RED);
				currIdx=fd;
				UG_PutString(0, x*8, ">");
//			} else if (ent.index==romGetCurr()) {
//				UG_SetForecolor(C_YELLOW);
			} else {
				UG_SetForecolor(C_WHITE);
			}
			strncpy(rname, name, 12);
			rname[12]=0;
			UG_PutString(6, x*8, rname);
		}
	}
	kcugui_flush();
}


