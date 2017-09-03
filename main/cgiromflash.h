#ifndef CGIROMFLASH_H
#define CGIROMFLASH_H

#include "httpd.h"

#define UPLOAD_TEMP_NAME "__upload_temp_file__.tmp"

int cgiUploadRom(HttpdConnData *connData);
int cgiRomIdx(HttpdConnData *connData);
int cgiDelete(HttpdConnData *connData);

#endif