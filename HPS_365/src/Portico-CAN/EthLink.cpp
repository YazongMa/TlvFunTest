
/*
 * File Name: EthLink.cpp
 * 
 * Author: Alan.Ren
 * 
 * Create Date: 2018.12.14
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <unistd.h>
#include <time.h>

#include "dbghlp.h"

#define Init_Eth_Timeout (0xE001)

/*
 * Public.
 */
extern WORD OpenEthLink(void)
{
    // Open ethernet
    WORD wErr = CTOS_EthernetOpenEx();
    if (wErr == d_ETHERNET_ALREADY_OPENED)
    {
        return d_OK;
    }
    else if (d_OK != wErr)
    {
        return wErr;
    }

    wErr = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, (BYTE *)"1", 1);
    if (d_OK != wErr)
    {
        return wErr;
    }

    DWORD dwTickStart = GetTickCount();
    wErr = Init_Eth_Timeout;
    while ((GetTickCount() - dwTickStart) < 30 * 1000)
    {
        DWORD dwStatus = 0x00;
        BYTE byBuf[32] = {0};
        BYTE byBufLen = sizeof(byBuf);

        wErr = CTOS_EthernetStatus(&dwStatus);
        if (d_OK == wErr)
        {
            wErr = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, byBuf, &byBufLen);
            if (wErr == d_OK && strcmp("0.0.0.0", (char *)byBuf) != 0)
            {
                TraceMsg("Info: Initialize ethernet success.");
                wErr = d_OK;
                break;
            }
            wErr = Init_Eth_Timeout;
        }
        usleep(20 * 1000); // 20 ms
    }
    return wErr;    
}