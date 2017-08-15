#ifndef CGIROMFLASH_H
#define CGIROMFLASH_H

#include "httpd.h"

int cgiUploadRom(HttpdConnData *connData);
int cgiRomIdx(HttpdConnData *connData);
int cgiDelete(HttpdConnData *connData);

#endif