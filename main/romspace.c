#include "nvs.h"
#include "esp_partition.h"
#include "romspace.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXENT 256

static nvs_handle nvs;

void romspaceInit() {
	esp_err_t r;
	r=nvs_open("gbrom", NVS_READWRITE, &nvs);
	if (r!=ESP_OK) printf("Gbrom namespace open fail.\n");
}

/*
NVS format:
N = 0-255
adrN - address of ROM (int32) (within ROM partition)
sizeN - size of ROM (int32)
typeN - type of ROM (unused now) (int32)
*/

/*
Get the n'th valid entry.
Code is slightly derpy, maybe use some caching for the standard case of increasing no...
*/
int romspaceGetEnt(int no, RomspaceEnt *ret) {
	static int lastno=0, lastidx=0;
	int found=0;
	esp_err_t r;
	int x, adr=0, size=0;
	char key[16];
	if (lastno>no) {
		lastno=0;
		lastidx=0;
	}
	found=lastno;
	for (x=lastidx; x<MAXENT; x++) {
		sprintf(key, "adr%d", x);
		r=nvs_get_i32(nvs, key, &adr);
		assert(r==ESP_OK || r==ESP_ERR_NVS_NOT_FOUND);
		if (r==ESP_OK) {
			printf("romspaceGetEnt: Found %d %s (is %d, want %d)\n", x, key, found, no);
			if (found==no) break;
			found++;
		}
	}
	if (x==MAXENT) return 0;
	sprintf(key, "size%d", x);
	r=nvs_get_i32(nvs, key, &size);
	ret->index=x;
	ret->address=adr;
	ret->size=size;
	const esp_partition_t* part=esp_partition_find_first(40, 2, NULL);
	esp_partition_read(part, adr+0x0134, ret->name, 16);
	ret->name[16]=0;
	lastidx=x+1; lastno=no+1;

	//HACK!
	nvs_close(nvs);
	nvs_open("gbrom", NVS_READWRITE, &nvs);


	return 1;
}


/*
 Deletes the ROM stored at the nth index.
 Does this by killing the NVS entry, then moving up all ROMS under this ROM.
*/
void romspaceDelEnt(int index) {
	esp_err_t r;
	char key[16];
	int adr, size;
	sprintf(key, "adr%d", index);
	r=nvs_get_i32(nvs, key, &adr);
	if (r!=ESP_OK) return;
	nvs_erase_key(nvs, key);

	sprintf(key, "size%d", index);
	r=nvs_get_i32(nvs, key, &size);
	nvs_erase_key(nvs, key);

	

	//ToDo: Move lower ROMs up
}

//Address of start of free space. Address is within the ROM partition.
uint32_t romspaceGetFreeAdr() {
	int adr, size;
	char key[16];
	int highestAdr=0;
	int x;
	esp_err_t r;
	for (x=0; x<MAXENT; x++) {
		sprintf(key, "adr%d", x);
		r=nvs_get_i32(nvs, key, &adr);
		if (r==ESP_OK) {
			sprintf(key, "size%d", x);
			r=nvs_get_i32(nvs, key, &size);
			adr+=size;
			//Round to 64K pages
			adr=(adr+((1<<16)-1))&~((1<<16)-1);
			if (highestAdr<adr) highestAdr=adr;
		}
	}
	return highestAdr;
}

//Amount of bytes still free
uint32_t romspaceGetFreeMem() {
	uint32_t adr=romspaceGetFreeAdr();
	const esp_partition_t* part=esp_partition_find_first(40, 2, NULL);
	return part->size-adr;
}


void romspaceRegisterRom(uint32_t size) {
	esp_err_t r;
	int x;
	int adr;
	char key[16];
	int ulAdr=romspaceGetFreeAdr();
	size=(size+((1<<16)-1))&~((1<<16)-1);
	for (x=0; x<MAXENT; x++) {
		sprintf(key, "adr%d", x);
		r=nvs_get_i32(nvs, key, &adr);
		if (r!=ESP_OK) break;
	}
	printf("Found free key: %s\n", key);
	nvs_set_i32(nvs, key, ulAdr);
	sprintf(key, "size%d", x);
	nvs_set_i32(nvs, key, size);
	nvs_commit(nvs);
}

uint32_t romspaceGetPartOff() {
	const esp_partition_t* part=esp_partition_find_first(40, 2, NULL);
	return part->address;
}

int romGetCurr() {
	int no=0;
	nvs_get_i32(nvs, "cidx", &no);
	return no;
}

int romSetCurr(int idx) {
	char key[16];
	esp_err_t r;
	int adr;
	sprintf(key, "adr%d", idx);
	r=nvs_get_i32(nvs, key, &adr);
	if (r!=ESP_OK) return 0;
	nvs_set_i32(nvs, "cidx", idx);
	nvs_set_u32(nvs, "cadr", adr);
	nvs_commit(nvs);
	return 1;
}

void romspaceKillAll() {
	printf("KILL KILL KILL\n");
	nvs_erase_all(nvs);
	nvs_commit(nvs);
}
