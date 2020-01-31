
/*
 * File Name: CtrlThread.cpp
 * 
 * Author: Alan.Ren
 * Create Date: 2018.11.15
 */

#include <ctosapi.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dbghlp.h"
#include "AppMain.h"
#include "Ui.h"

#define CMD_EXIT "Exit"
#define Nop      (10 * 1000)

extern CUi *getUiModuleObj(void);

static void * ctrlThread(void *pVoid)
{
    pthread_detach(pthread_self());
    USHORT usErr;
    USHORT usRxDataLen;
    BYTE byBuf[1024];
    BYTE byPort = d_COM3;
    CUi *pUi = getUiModuleObj();

    usErr = CTOS_RS232Open(byPort, 115200, 'N', 8, 1);
    if (usErr == d_OK)
    {
        while (usErr == d_OK)
        {
            usErr = CTOS_RS232RxReady(byPort, &usRxDataLen);
            if (usRxDataLen == 0)
            {
                usleep(Nop);
            }
            else
            {
                TraceMsg("RxData");
                usRxDataLen = sizeof(byBuf);
                bzero(byBuf, usRxDataLen);
                usErr = CTOS_RS232RxData(byPort, byBuf, &usRxDataLen);
                if (usRxDataLen != 0)
                {
                    CTOS_RS232TxData(byPort, byBuf, usRxDataLen);
                    if (0 == strcmp(reinterpret_cast<char *>(byBuf), CMD_EXIT))
                    {
                        // exit(0);
                        CTOS_KBDBufPut(d_KBD_CANCEL);
                    }
                }
            }
            usleep(Nop);
        }
        TraceMsg("Error: Recv data error, ErrorCode=-0x%04X", usErr);
    }
    else
    {
        TraceMsg("Error: Open COM Port failed, ErrorCode=0x%04X", usErr);
    }

    TraceMsg("Info: Exit thread.");
    return NULL;
}

void InitCtrlThread(void)
{
    pthread_t cThreadId;
    int re;

    re = pthread_create(&cThreadId, NULL, ctrlThread, NULL);
    if (re != 0)
    {
        TraceMsg("Error: call pthread_create() failed, ErrorCode=%d, ErrorInfo=%s",
                 errno, strerror(errno));
    }
}
