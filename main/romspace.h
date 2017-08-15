#ifndef ROMSPACE_H
#define ROMSPACE_H

#include <stdint.h>

typedef struct {
	int index;
	char name[17];
	uint32_t address;
	uint32_t size;
} RomspaceEnt;

void romspaceInit();
int romspaceGetEnt(int no, RomspaceEnt *ret);
void romspaceDelEnt(int index);
uint32_t romspaceGetFreeAdr();
uint32_t romspaceGetFreeMem();
void romspaceRegisterRom(uint32_t size);
uint32_t romspaceGetPartOff();
int romGetCurr();
int romSetCurr(int idx);
void romspaceKillAll();

#endif