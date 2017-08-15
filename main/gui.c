#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include "gui.h"
#include "ugui.h"
#include "hw.h"
#include "ssd1331.h"
#include "romspace.h"

static uint16_t *fb;
static UG_GUI ugui;

static void oled_pset(UG_S16 x, UG_S16 y, UG_COLOR c) {
	if (x<0 || x>=80) return;
	if (y<0 || y>=64) return;
	fb[(x+8)+(y*96)]=(c>>8)|(c<<8);
}

static void guiFlush() {
	ssd1331SendFB(fb);
}

static void guiCls() {
	memset(fb, 0, 96*64*2);
}

void guiCharging() {
	guiCls();
	UG_FontSelect(&FONT_6X8);
	UG_SetForecolor(C_WHITE);
	UG_PutString(0, 0, "CHARGING");
	guiFlush();
}

void guiFull() {
	guiCls();
	UG_FontSelect(&FONT_6X8);
	UG_SetForecolor(C_GREEN);
	UG_PutString(0, 0, "CHARGED");
	guiFlush();
}

void guiInit() {
	fb=malloc(96*64*2);
	guiCls();
	memset(fb, 0, 96*64*2);
	UG_Init(&ugui, oled_pset, 80, 64);
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

	guiFlush();
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
		if (key==PAD_UP) {
			if (pos==0) {
				if (scpos!=0) scpos--;
			} else {
				pos--;
			}
		} else if (key==PAD_DOWN) {
			if (pos==4) {
				if (scpos<maxPos-4) scpos++;
			} else {
				pos++;
			}
		} else if (key==PAD_A) {
			romSetCurr(currIdx);
		}
		printf("pos %d scpos %d\n", pos, scpos);
	}

	//Display handling
	guiCls();
	if (state==ST_SELECT) {
		RomspaceEnt ent;
		for (x=0; x<8; x++) {
			r=romspaceGetEnt(scpos+x, &ent);
			if (!r) {
				maxPos=scpos+x;
				break;
			}
			if (x==pos) {
				UG_SetForecolor(C_RED);
				currIdx=ent.index;
				UG_PutString(0, x*8, ">");
			} else if (ent.index==romGetCurr()) {
				UG_SetForecolor(C_YELLOW);
			} else {
				UG_SetForecolor(C_WHITE);
			}
			strncpy(rname, ent.name, 12);
			rname[12]=0;
			UG_PutString(6, x*8, rname);
		}
	}
	guiFlush();
}


