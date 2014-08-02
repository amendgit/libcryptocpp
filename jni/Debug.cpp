#include "Debug.h"

#include <stdio.h>
#include <string.h>

void LogByteArray(const char *data, const size_t len)
{
    char buf[1024];
    char *p = buf;

    for (int i=0; i<len; ++i) {
        sprintf(p, "0x%x ", data[i]);
        p += strlen(p);
    }
    
    LOGI("%s", buf);
}