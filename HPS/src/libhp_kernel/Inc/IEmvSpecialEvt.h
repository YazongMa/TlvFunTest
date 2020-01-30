
/*
 * File Name: IEmvSpecialEvt.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.11
 */

#ifndef _H_H_EMV_SPECIAL_EVT_H_H_
#define _H_H_EMV_SPECIAL_EVT_H_H_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef IN_OUT
#define IN_OUT
#endif

/*
 * Special EVENT mask.
 */
typedef enum
{
    kOutCardApdu = 0x00000001,
    kDisablePINNull = 0x00000002,
    kFstGACTermdecision = 0x00000004,
    kDisAppList = 0x00000008,
    kEnableInternalGetPINCancelKeyBypass = 0x00000010,
    kAppListEx = 0x00000020,
    kNonEmvCard = 0x00000040,
    kMCCSDynamicTerminalSupAid = 0x00000080,
    kNsiccPayment = 0x00000100,
    kTxnForceOnline = 0x00000200,
    kInterAcAsfMode = 0x00000400,
    kGetPinDone = 0x00000800
} SpecEvtMask;



/*
 * Interface.
 */
class IEMVSpecialEvt
{
public:
    virtual ~IEMVSpecialEvt() { }
    
    virtual BYTE GetVersion() { return 0x01; }
    
    virtual void OUTPUTCARDAPDU(IN BYTE *pTxAPDU, IN USHORT TxAPUDLen, IN BYTE *pRxAPDU, IN USHORT RxAPDULen)
    { }

    // variable for d_EVENTID_DISABLE_PINNULL
    virtual void DISABLE_PINNULL(OUT BOOL *IsDisablePINNull)
    { }

    // variable for d_EVENTID_FIRSTGENAC_TERMDECISION
    virtual void FIRSTGENAC_TERMDECISION(INOUT BYTE *TermDecision)
    { }

    // variable for d_EVENTID_CONTINUE_APP_SELECT
    // using EMV_TxnAppSpecificSelect function, when specified application label is not match, call this callback function and return true to display application list to select. default return d_EMVAPLIB_ERR_NO_AP_FOUND when is not match.
    virtual BOOL DISPLAY_APPLICATION_LIST(void)
    {
        return FALSE;
    }

    // variable for d_EVENTID_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS
    virtual void ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS(OUT BYTE *isEnableGetPINBypassByCancelKey)
    { }

    // variable for d_EVENTID_APP_LIST_EX
    virtual USHORT APP_LIST_EX(IN BYTE bAppNum, IN EMV_APP_LIST_EX_DATA *pstAppListExData, OUT BYTE *pbAppSelectedIndex)
    {
        return -1;
    }

    // variable for d_EVENTID_NON_EMV_CARD
    virtual void NON_EMV_CARD(OUT BOOL *IsNonEMVCard)
    { }

    // variable for d_EVENTID_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID
    virtual void MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID(OUT BYTE *pbAID, OUT BYTE *pbAIDLen, OUT BYTE *pbAIDNoMatchOption)
    { }

    // variable for d_EVENTID_NSICC_PAYMENT
    virtual void NSICCPayment(IN BYTE *pbAID, IN BYTE bAIDLen, OUT BYTE *pbNSICCPayment)
    { }

    // variable for d_EVENTID_TXN_FORCED_ONLINE
    // pbForcedONL :
    // 0x01 : Txn Forced Online
    virtual void TxnForcedOnline(OUT BYTE *pbForcedONL)
    { }

    // variable for d_EVENTID_INTERAC_ASF_MODE
    virtual void InteracASFMode(OUT BYTE *pbMode)
    { }

    // variable for d_EVENTID_GET_PIN_DONE
    virtual USHORT GET_PIN_DONE(void)
    {
        return 0x00;
    }

protected:
    IEMVSpecialEvt() { }
};

#endif
 