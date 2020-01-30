
/*
 * File Name: UnzipRes.cpp
 * 
 * Author: Alan.Ren
 * Create Date: 2018.11.15
 * 
 */

#include <ctosapi.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "dbghlp.h"

#define UNZIP_COMMAND    "unzip -o Res.zip"
#define LS_COMMAND       "ls"
#define ZIP_File_Name    "Res.zip"

void UpzipResFile(void)
{
    struct stat buf;
    const int re = stat(ZIP_File_Name, &buf);

    TraceMsg("Before: ");
    system( LS_COMMAND );
    if (-1 != re)
    {
        system(UNZIP_COMMAND);
    }
    if (re != -1)
    {
        remove(ZIP_File_Name);
    }
    TraceMsg("After:");
    system( LS_COMMAND );
}
