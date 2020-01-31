/* 
 * File:   CTouch.h
 * Author: Sword
 *
 * Created on 2018.11.09
 */

#include <stdlib.h>
#include <ctosapi.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include<linux/input.h>
#include <unistd.h>
#include "CTouch.h"

#define TouchEventSize 10

CTouch::CTouch()
{
    m_TouchDevHandle = INVALID_TOUCH_DEV_HANDLE;
    m_lPressX = m_lPressY = m_lUpX = m_lUpY = 0;
}

CTouch::~CTouch()
{

}

BOOL CTouch::Setnonblock()
{
    int nFlags;
    nFlags = fcntl(m_TouchDevHandle, F_GETFL);
    if(nFlags < 0)
    {
        return FALSE;
    }
    
    nFlags |= O_NONBLOCK;
    if(fcntl(m_TouchDevHandle, F_SETFL, nFlags) < 0)
    {
        return FALSE;
    }    
    return TRUE;
}
BOOL CTouch::TouchInit(void)
{
    bTouchOK = FALSE;
    m_TouchDevHandle = open(TOUCH_DEVICE_NAME, O_RDONLY | O_NONBLOCK);
    if (m_TouchDevHandle < 0)
    {
        m_TouchDevHandle = INVALID_TOUCH_DEV_HANDLE;
        return bTouchOK;
    }

    bTouchOK = TRUE;
    return TRUE;
}

BOOL CTouch::TouchGetPosition(long &lDownX, long &lDownY, long &lUpX, long &lUpY)
{
    struct input_absinfo absI;
    int nRet;
    struct input_event byData[TouchEventSize];

    nRet = read(m_TouchDevHandle, byData, sizeof(struct input_event)*TouchEventSize);
    if(nRet <= 0)
        return FALSE;
    
    for(int nItem=0; nItem<nRet/sizeof(struct input_event); nItem++)
    {
        if(byData[nItem].type != EV_KEY)
            continue;;
    
        if((byData[nItem].code == BTN_TOUCH) & (byData[nItem].value == 1))
        {
            ioctl(m_TouchDevHandle, EVIOCGABS(ABS_X), &absI);
            m_lPressX = absI.value;
            ioctl(m_TouchDevHandle, EVIOCGABS(ABS_Y), &absI);
            m_lPressY = absI.value;
            return FALSE;
        }
        if((byData[nItem].code == BTN_TOUCH) & (byData[nItem].value == 0))
        {
            ioctl(m_TouchDevHandle, EVIOCGABS(ABS_X), &absI);
            lUpX = absI.value;
            ioctl(m_TouchDevHandle, EVIOCGABS(ABS_Y), &absI);
            lUpY = absI.value;

            lDownX = m_lPressX;
            lDownY = m_lPressY;
            return TRUE;
        }
    }
    
    return FALSE;
}        

void CTouch::TouchClose(void)
{
    close(m_TouchDevHandle);
    m_TouchDevHandle = INVALID_TOUCH_DEV_HANDLE;
}
