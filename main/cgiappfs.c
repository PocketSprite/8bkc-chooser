/*
Some flash handling cgi routines. Used for updating the appfs.
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

//#include <osapi.h>
#include "cgiflash.h"
#include "espfs.h"
#include "httpd-platform.h"
#include "esp32_flash.h"
#include "appfs.h"
#include "hexdump.h"
#include "nvs.h"

#ifndef UPGRADE_FLAG_FINISH
#define UPGRADE_FLAG_FINISH     0x02
#endif



//Cgi that allows the firmware to be replaced via http POST This takes
//a direct POST from e.g. Curl or a Javascript AJAX call with either the
//firmware given by cgiGetFirmwareNext or an OTA upgrade image.

//Because we don't have the buffer to allocate an entire sector but will 
//have to buffer some data because the post buffer may be misaligned, we 
//write SPI data in pages. The page size is a software thing, not
//a hardware one.
#define PAGELEN 4096*4
#define SPI_FLASH_ERASE_SIZE 32768 //32K - flash has a cmd for this

#define FLST_START 0
#define FLST_WRITE 1
#define FLST_DONE 3
#define FLST_ERROR 4

typedef struct {
	appfs_handle_t fd;
	int state;
	int filetype;
	int flashPos;
	char pageData[PAGELEN];
	int pagePos;
	int address;
	int len;
	char *err;
	char name[512];
} UploadState;


int ICACHE_FLASH_ATTR cgiUploadFile(HttpdConnData *connData) {
	UploadState *state=(UploadState *)connData->cgiData;
	esp_err_t err;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		if (state!=NULL) free(state);
		return HTTPD_CGI_DONE;
	}

	if (state==NULL) {
		//First call. Allocate and initialize state variable.
		httpd_printf("Firmware upload cgi start.\n");
		state=calloc(sizeof(UploadState), 1);
		if (state==NULL) {
			httpd_printf("Can't allocate firmware upload struct!\n");
			return HTTPD_CGI_DONE;
		}
		state->state=FLST_START;
		connData->cgiData=state;
		state->err="Premature end";
	}
	
	char *data=connData->post->buff;
	int dataLen=connData->post->buffLen;
	
	while (dataLen!=0) {
		if (state->state==FLST_START) {
			int len=httpdFindArg(connData->getArgs, "name", state->name, sizeof(state->name));
			if (len<=0) {
				state->err="No name";
				state->state=FLST_ERROR;
			}
			printf("Creating temp file %s for upload, size %d\n", UPLOAD_TEMP_NAME, connData->post->len);
			err=appfsCreateFile(UPLOAD_TEMP_NAME, connData->post->len, &state->fd);
			if (err!=ESP_OK) {
				state->err="App too large for free space";
				state->state=FLST_ERROR;
			} else {
				state->len=connData->post->len;
				state->address=0;
				state->state=FLST_WRITE;
			}
		} else if (state->state==FLST_WRITE) {
			//Copy bytes to page buffer, and if page buffer is full, flash the data.
			//First, calculate the amount of bytes we need to finish the page buffer.
			int lenLeft=PAGELEN-state->pagePos;
			if (state->len<lenLeft) lenLeft=state->len; //last buffer can be a cut-off one
			//See if we need to write the page.
			if (dataLen<lenLeft) {
				//Page isn't done yet. Copy data to buffer and exit.
				memcpy(&state->pageData[state->pagePos], data, dataLen);
				state->pagePos+=dataLen;
				state->len-=dataLen;
				dataLen=0;
			} else {
				//Finish page; take data we need from post buffer
				memcpy(&state->pageData[state->pagePos], data, lenLeft);
				data+=lenLeft;
				dataLen-=lenLeft;
				state->pagePos+=lenLeft;
				state->len-=lenLeft;
				//Erase sector, if needed
				if ((state->address&(SPI_FLASH_ERASE_SIZE-1))==0) {
					printf("Erasing %d bytes at 0x%x...\n", SPI_FLASH_ERASE_SIZE, state->address);
					err=appfsErase(state->fd, state->address, SPI_FLASH_ERASE_SIZE);
					if (err!=ESP_OK) {
						printf("AppFs erase failed: %x\n", err);
						state->err="AppfsErase failed";
						state->state=FLST_ERROR;
					}
					printf("Erase done.\n");
				}
				//Write page
				httpd_printf("Writing %d bytes (adr %p) of data to SPI pos 0x%x...\n", state->pagePos, state->pageData, state->address);
				err=appfsWrite(state->fd, state->address, (uint8_t*)state->pageData, state->pagePos);
				if (err!=ESP_OK) {
					printf("AppFs write failed: %d\n", err);
					state->err="AppfsWrite failed";
					state->state=FLST_ERROR;
				}
				printf("Write done.\n");
				state->address+=PAGELEN;
				state->pagePos=0;
				if (state->len==0) {
					printf("Done. Renaming temp file %s to %s\n", UPLOAD_TEMP_NAME, state->name);
					state->state=FLST_DONE;
					err=appfsRename(UPLOAD_TEMP_NAME, state->name);
					if (err!=ESP_OK) {
						printf("Rename failed: %d\n", err);
						state->err="Rename failed";
						state->state=FLST_ERROR;
					}
				}
			}
		} else if (state->state==FLST_DONE) {
			httpd_printf("Huh? %d bogus bytes received after data received.\n", dataLen);
			//Ignore those bytes.
			dataLen=0;
		} else if (state->state==FLST_ERROR) {
			//Just eat up any bytes we receive.
			dataLen=0;
		}
	}
	
	if (connData->post->len==connData->post->received) {
		//We're done! Format a response.
		httpd_printf("Upload done. Sending response.\n");
		httpdStartResponse(connData, state->state==FLST_ERROR?400:200);
		httpdHeader(connData, "Content-Type", "text/plain");
		httpdEndHeaders(connData);
		if (state->state!=FLST_DONE) {
			httpdSend(connData, "Upload error:", -1);
			httpdSend(connData, state->err, -1);
			httpdSend(connData, "\n", -1);
		}
		free(state);
		return HTTPD_CGI_DONE;
	}
	return HTTPD_CGI_MORE;
}

typedef struct {
	appfs_handle_t fd;
	int size;
	int pos;
} DownloadState;


#define DOWNLOAD_CHUNK_SIZE 1024
int ICACHE_FLASH_ATTR cgiDownloadFile(HttpdConnData *connData) {
	DownloadState *state=(DownloadState *)connData->cgiData;
	esp_err_t err;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		if (state!=NULL) free(state);
		return HTTPD_CGI_DONE;
	}

	if (state==NULL) {
		//First call. Open file, error out if impossible.
		httpd_printf("Firmware download cgi start.\n");
		state=calloc(sizeof(DownloadState), 1);
		if (state==NULL) {
			httpd_printf("Can't allocate firmware download struct!\n");
			return HTTPD_CGI_DONE;
		}
		connData->cgiData=state;

		char sidx[16];
		int idx=-1;
		int len=httpdFindArg(connData->getArgs, "idx", sidx, 16);
		const char *name=NULL;
		if (len>0) {
			idx=atoi(sidx);
			if (appfsFdValid(idx)) {
				appfsEntryInfo(idx, &name, &state->size);
			}
		}

		if (!name) {
			//error retrieving idx data
			httpdStartResponse(connData, 404);
			httpdHeader(connData, "Content-Type", "text/plain");
			httpdEndHeaders(connData);
			httpdSend(connData, "Invalid idx var\n", -1);
			return HTTPD_CGI_DONE;
		}

		//No error: start file
		state->fd=idx;
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "application/octet-stream");
		char buff[256];
		strcpy(buff, "attachment; filename=\"");
		//Replace quotes with underscores. Lazy I know.
		char *p=&buff[strlen(buff)];
		for (int i=0; i<strlen(name); i++) {
			if (name[i]=='\'' || name[i]=='"' || name[i]&0x80) {
				*p++='_';
			} else {
				*p++=name[i];
			}
		}
		strcpy(p, "\"");
		httpdHeader(connData, "Content-Disposition", buff);
		sprintf(buff, "%d", state->size);
		httpdHeader(connData, "Content-Length", buff);
		httpdEndHeaders(connData);
	}

	char data[DOWNLOAD_CHUNK_SIZE];
	int len=state->size-state->pos;
	if (len>DOWNLOAD_CHUNK_SIZE) len=DOWNLOAD_CHUNK_SIZE;
	printf("Download: at %d, pushing %d bytes.\n", state->pos, len);
	appfsRead(state->fd, state->pos, data, len);
	httpdSend(connData, data, len);
	state->pos+=len;
	if (state->pos==state->size) {
		free(state);
		return HTTPD_CGI_DONE;
	} else {
		return HTTPD_CGI_MORE;
	}
}

static void json_esc(char *out, const char *in, int bufsz) {
	int j=0;
	for (int i=0; in[i]!=0; i++) {
		if (j>=bufsz-3) break;
		int esc=-1;
		if (in[i]==8) esc='b';
		if (in[i]==12) esc='f';
		if (in[i]==10) esc='n';
		if (in[i]==13) esc='r';
		if (in[i]==9) esc='t';
		if (in[i]==34) esc='"';
		if (in[i]==92) esc='\\';
		if (esc==-1) {
			out[j++]=in[i];
		} else {
			out[j++]='\\';
			out[j++]=esc;
		}
	}
	out[j++]=0;
}

int ICACHE_FLASH_ATTR cgiFileIdx(HttpdConnData *connData) {
	int *idx=(int*)&connData->cgiData;
	char buff[256];
	int fd;

	printf("cgiFileIdx: run %d\n", *idx-0x100);
	if (*idx==0) {
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/json");
		httpdEndHeaders(connData);
		httpdSend(connData, "{\n\"files\": [\n", -1);
		fd=APPFS_INVALID_FD;
	} else {
		fd=*idx-0x100;
	}

	printf("Grabbing entry after %d\n", fd);
	fd=appfsNextEntry(fd);
	printf("Next appfs fd: %d\n", fd);
	if (fd!=APPFS_INVALID_FD) {
		const char *name=NULL;
		int size;
		appfsEntryInfo(fd, &name, &size);
		//no need to check name for NULL; we checked if fd is valid before.
		char name_esc[128];
		json_esc(name_esc, name, 128);
		sprintf(buff, "%s{\"index\": %d, \"name\": \"%s\", \"size\": %d, \"addr\": \"0x%X\" }\n", *idx?",":"", 
			fd, name_esc, size, 0);
		printf(" - %s\n", buff);
		httpdSend(connData, buff, -1);
		*idx=fd+0x100;
		return HTTPD_CGI_MORE;
	} else {
		httpdSend(connData, "],\n\"free\": ", -1);
		sprintf(buff, "%d", appfsGetFreeMem());
		printf(" - %s\n", buff);
		httpdSend(connData, buff, -1);
		httpdSend(connData, "}\n", -1);
		return HTTPD_CGI_DONE;
	}
}

int ICACHE_FLASH_ATTR cgiDelete(HttpdConnData *connData) {
	//idx contains the fd of the file
	char fdText[16];
	const char *name;
	int len=httpdFindArg(connData->getArgs, "idx", fdText, sizeof(fdText));
	if (len>0) {
		int fd=atoi(fdText);
		appfsEntryInfo(fd, &name, NULL);
		//Try to delete nvs storage as well.
		esp_err_t r;
		nvs_handle nvsh;
		r=nvs_open(name, NVS_READWRITE, &nvsh);
		if (r==ESP_OK) {
			nvs_erase_all(nvsh);
			nvs_commit(nvsh);
			nvs_close(nvsh);
			printf("Removed appfs entry for %s as well.\n", name);
		}
		//Kill appfs file
		appfsDeleteFile(name);
		httpdStartResponse(connData, 302);
		httpdHeader(connData, "Content-Type", "text/html");
		httpdHeader(connData, "Location", "index.html");
		httpdEndHeaders(connData);
		httpdSend(connData, "Killed", -1);
	} else {
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/html");
		httpdEndHeaders(connData);
		httpdSend(connData, "Invalid idx var\n", -1);
	}
	return HTTPD_CGI_DONE;
}
