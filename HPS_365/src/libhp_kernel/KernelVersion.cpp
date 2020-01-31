
/*
 * File Name: KernelVersion.h
 * Author: Alan.Ren
 * 
 * Creat Date: 2019.03.14
 */

#include "BasicTypeDef.h"
#include "KernelVersion.h"
#include "Inc\DateMacro.h"

#define KERNEL_VERSION_STR "1.2.0.006" 

/*
 * Public.
 */
PUBLIC_EXPORT const char* GetHLKerVer(void)
{
    static char szHLVerBuf[64] = {0};
    static BOOL bInitVer = FALSE;

    if (!bInitVer)
    {
        sprintf(szHLVerBuf, "%s_%04d.%02d.%02d",
                KERNEL_VERSION_STR, 
                YEAR(),
                MONTH() + 1, 
                DAY());
        // sprintf(szHLVerBuf, "%s", KERNEL_VERSION_STR);
        bInitVer = TRUE;
    }
    return szHLVerBuf;
}
