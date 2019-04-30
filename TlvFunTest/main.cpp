#include <Windows.h>
#include <time.h>
#include "ByteStream.h"
#include "FunBase.h"
#include "TlvFun.h"


void TestTlv();
void TestHex();


int main()
{
	TestHex();

	char* pszEMVRespInfo = "91086CE1A981008000007105E2D4AA900A7202A9BC9F10031a2b3c";
	char szHexInfop[256] = { 0 };
	int nLength = strlen(pszEMVRespInfo);
	for (int i = 0, j = 0; i < nLength; i += 2, ++j)
	{
		char l1 = *(pszEMVRespInfo + i) - '0';
		char l2 = *(pszEMVRespInfo + i + 1) - '0';

		szHexInfop[j] = (l1 << 4 | l2);
	}

	return 0;
}


void TestTlv()
{
	char* pszEMVRespInfo = "91086CE1A981008000007105E2D4AA900A7202A9BC9F10031a2b3c";

	CByteStream cData;
	CFunBase::Str2Hex(pszEMVRespInfo, cData);

	int nBufferLength = cData.GetLength();
	char szBuffer[1024] = { 0 };
	memcpy(szBuffer, cData.GetBuffer(), nBufferLength);

	WORD wLen;
	unsigned char* pbyData = NULL;
	CTlvFun::TLVListReadTagValue("91", (unsigned char*)szBuffer, nBufferLength, &pbyData, &wLen);

	CTlvFun::TLVListReadTagValue("71", (unsigned char*)szBuffer, nBufferLength, &pbyData, &wLen);

	BYTE byTag[4] = { 0 };
	byTag[0] = 0x72;
	CTlvFun::TLVListReadTagPkt((unsigned char*)byTag, (unsigned char*)szBuffer, nBufferLength, &pbyData, &wLen);

	byTag[0] = 0x9F;
	byTag[1] = 0x10;
	CTlvFun::TLVListReadTagPkt((unsigned char*)byTag, (unsigned char*)szBuffer, nBufferLength, &pbyData, &wLen);
}


typedef struct
{
	BYTE                    bDate[3];
	BYTE                    bTime[3];
} HEARTLAND_TRANS_PARAM;

void TestHex()
{
	HEARTLAND_TRANS_PARAM cTransParam = { 0 };
	BYTE bYear, bMonth, bDay, bHour, bMinute, bSecond;

	time_t stNowTime = time(NULL);
	struct tm *pStTimeVal = localtime(&stNowTime);

	bYear = (pStTimeVal->tm_year + 1900 - 2000);
	bMonth = pStTimeVal->tm_mon + 1;
	bDay = pStTimeVal->tm_mday;
	bHour = pStTimeVal->tm_hour;
	bMinute = pStTimeVal->tm_min;
	bSecond = pStTimeVal->tm_sec;

	cTransParam.bDate[0] = (bYear / 10) << 4 | (bYear % 10);
	cTransParam.bDate[1] = (bMonth / 10) << 4 | (bMonth % 10);
	cTransParam.bDate[2] = (bDay / 10) << 4 | (bDay % 10);

	cTransParam.bTime[0] = (bHour / 10) << 4 | (bHour % 10);
	cTransParam.bTime[1] = (bMinute / 10) << 4 | (bMinute % 10);
	cTransParam.bTime[2] = (bSecond / 10) << 4 | (bSecond % 10);
}