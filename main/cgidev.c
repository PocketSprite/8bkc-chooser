/*
PocketSprite dev cgi handlers
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

#include <esp8266.h>
#include "cgiappfs.h"
#include "espfs.h"
#include "cgiflash.h"
#include "espfs.h"
#include "esp_partition.h"
#include "8bkc-vfs-stdout.h"

//#include <osapi.h>
#include "cgiflash.h"
#include "espfs.h"
#include "httpd-platform.h"
#include "esp32_flash.h"
#include "appfs.h"
#include "hexdump.h"
#include "nvs.h"

typedef struct {
	int size;
	int pos;
} DownloadFlashState;

#define DOWNLOAD_CHUNK_SIZE 1024
int ICACHE_FLASH_ATTR cgiDownloadFlash(HttpdConnData *connData) {
	DownloadFlashState *state=(DownloadFlashState *)connData->cgiData;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		if (state!=NULL) free(state);
		return HTTPD_CGI_DONE;
	}

	if (state==NULL) {
		//First call.
		httpd_printf("Flash download cgi start.\n");
		state=calloc(sizeof(DownloadFlashState), 1);
		if (state==NULL) {
			httpd_printf("Can't allocate firmware download struct!\n");
			return HTTPD_CGI_DONE;
		}
		connData->cgiData=state;

		char abuf[16];
		int len;
		len=httpdFindArg(connData->getArgs, "size", abuf, 15);
		if (len>0) state->size=strtol(abuf, NULL, 0);
		len=httpdFindArg(connData->getArgs, "off", abuf, 15);
		if (len>0) state->pos=strtol(abuf, NULL, 0);
		strcpy(abuf, "data.dat");
		len=httpdFindArg(connData->getArgs, "name", abuf, 15);

		if (!state->size) {
			//error retrieving idx data
			httpdStartResponse(connData, 404);
			httpdHeader(connData, "Content-Type", "text/plain");
			httpdEndHeaders(connData);
			httpdSend(connData, "Invalid args (size, off, name)\n", -1);
			return HTTPD_CGI_DONE;
		}

		//No error: start file
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "application/octet-stream");
		char buff[256];
		sprintf(buff, "attachment; filename=\"%s\"", abuf);
		httpdHeader(connData, "Content-Disposition", buff);
		sprintf(buff, "%d", state->size);
		httpdHeader(connData, "Content-Length", buff);
		httpdEndHeaders(connData);
	}

	char data[DOWNLOAD_CHUNK_SIZE];
	int len=state->size-state->pos;
	if (len>DOWNLOAD_CHUNK_SIZE) len=DOWNLOAD_CHUNK_SIZE;
	printf("Download: at %d, pushing %d bytes.\n", state->pos, len);
	spi_flash_read(state->pos, data, len);
	httpdSend(connData, data, len);
	state->pos+=len;
	if (state->pos==state->size) {
		free(state);
		return HTTPD_CGI_DONE;
	} else {
		return HTTPD_CGI_MORE;
	}
}


typedef struct {
	int size;
	int pos;
	int ptr;
} DownloadLogState;

int ICACHE_FLASH_ATTR cgiDownloadLog(HttpdConnData *connData) {
	kchal_stdout_rtc_buf_t *rb=kchal_stdout_rtc_buf; //for quicker typing
	DownloadLogState *state=(DownloadLogState *)connData->cgiData;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		if (state!=NULL) free(state);
		return HTTPD_CGI_DONE;
	}

	if (state==NULL) {
		//First call.
		httpd_printf("Log download cgi start.\n");
		state=calloc(sizeof(DownloadLogState), 1);
		if (state==NULL) {
			httpd_printf("Can't allocate firmware download struct!\n");
			return HTTPD_CGI_DONE;
		}
		connData->cgiData=state;

		if (rb->magic!=KCHAL_STDOUT_MAGIC) {
			//error retrieving idx data
			httpdStartResponse(connData, 404);
			httpdHeader(connData, "Content-Type", "text/plain");
			httpdEndHeaders(connData);
			httpdSend(connData, "Bad RTC mem magic.\n", -1);
			return HTTPD_CGI_DONE;
		}
		printf("RTC rb: buf sz %d write ptr %d has_wrapped %d\n", rb->bufsz, rb->writeptr, rb->has_wrapped);

		if (rb->has_wrapped) {
			state->size=rb->bufsz;
			state->ptr=rb->writeptr;
		} else {
			state->size=rb->writeptr;
			state->ptr=0;
		}

		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "application/octet-stream");
		char buff[256];
		httpdHeader(connData, "Content-Disposition", "attachment; filename=\"log.txt\"");
		sprintf(buff, "%d", state->size);
		httpdHeader(connData, "Content-Length", buff);
		httpdEndHeaders(connData);
	}

	char data[DOWNLOAD_CHUNK_SIZE];
	int len=state->size-state->pos;
	if (len>DOWNLOAD_CHUNK_SIZE) len=DOWNLOAD_CHUNK_SIZE;
	printf("Download: at %d, pushing %d bytes.\n", state->pos, len);
	for (int i=0; i<len; i++) {
		data[i]=rb->buffer[state->ptr++];
		if (state->ptr==rb->bufsz) state->ptr=0;
	}
	httpdSend(connData, data, len);
	state->pos+=len;
	if (state->pos==state->size) {
		free(state);
		return HTTPD_CGI_DONE;
	} else {
		return HTTPD_CGI_MORE;
	}
}

