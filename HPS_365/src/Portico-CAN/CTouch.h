/* 
 * File:   CTouch.h
 * Author: Sword
 *
 * Created on 2018.11.09
 */

#ifndef _H_CTOUCH_
#define _H_CTOUTH_

#define _IN_
#define _OUT_
#define _IN_OUT_
#define INVALID_TOUCH_DEV_HANDLE   (-1)
#define TOUCH_DEVICE_NAME          "/dev/input/event0"
#define EVT_BUF_COUNT              (5)

class CTouch
{
public:
    CTouch();
    ~CTouch();

public:
    BOOL TouchInit(void);
    BOOL TouchGetPosition(long &lDownX, long &lDownY, long &lUpX,long &lUpY);
    BOOL Setnonblock();
    void TouchClose(void);
    BOOL bTouchOK;

private:
    int m_TouchDevHandle;
    long m_lPressX, m_lPressY, m_lUpX, m_lUpY;
};

#endif /* _H_CTOUCH_ */

