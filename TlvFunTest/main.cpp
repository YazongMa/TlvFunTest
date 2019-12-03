#include <Windows.h>
#include <time.h>
#include "ByteStream.h"
#include "FunBase.h"
#include "TlvFun.h"


void TestTlv();
void TestHex();
void TestAry();
void TestCopy();

int main()
{
	//TestHex();
	TestTlv();
	//TestAry();
	//TestCopy();

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

	pszEMVRespInfo = "91086CE1A981008000007105E2D4AA900A7202A9BC9F10031a2b3c";
	char szHexInfop[256] = { 0 };
	int nLength = strlen(pszEMVRespInfo);
	for (int i = 0, j = 0; i < nLength; i += 2, ++j)
	{
		char l1 = *(pszEMVRespInfo + i) - '0';
		char l2 = *(pszEMVRespInfo + i + 1) - '0';

		szHexInfop[j] = (l1 << 4 | l2);
	}
	nLength = 0;
	memset(szHexInfop, 0, sizeof(szHexInfop)); 
	CTlvFun::TLVFormatAndUpdateList("4F", 7, (unsigned char*)"\xA0\x00\x00\x00\x03\x10\x10", (unsigned short*)&nLength, (unsigned char*)szHexInfop);
	CTlvFun::TLVFormatAndUpdateList("9F02", 6, (unsigned char*)"123456", (unsigned short*)&nLength, (unsigned char*)szHexInfop);
	CTlvFun::TLVFormatAndUpdateList("4F", 2, (unsigned char*)"\xB1\x23", (unsigned short*)&nLength, (unsigned char*)szHexInfop);
	CTlvFun::TLVFormatAndUpdateList("9F03", 6, (unsigned char*)"654321", (unsigned short*)&nLength, (unsigned char*)szHexInfop);
	CTlvFun::TLVFormatAndUpdateList("4F", 9, (unsigned char*)"\xA0\x00\x00\x00\x03\x10\x10\x20\x30", (unsigned short*)&nLength, (unsigned char*)szHexInfop);
	CTlvFun::TLVFormatAndUpdateList("9F05", 6, (unsigned char*)"654321", (unsigned short*)&nLength, (unsigned char*)szHexInfop);

	char* pszTag = "9F27";

	CByteStream cHexData;
	CFunBase::Str2Hex(pszTag, cHexData);
	int l = atoi((char *)cHexData.GetBuffer());
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



class CTest
{
public:
	void SetAidList(IN char szAppLabel[][17], IN int nAppNum)
	{
		memset(m_szAppLabel, 0, sizeof(char) * 10 * 17);
		memcpy(m_szAppLabel, szAppLabel, sizeof(char) * 17 * nAppNum);

		m_nAppNum = nAppNum;
	}

	const int GetAidList(OUT char szAppLabel[][17]) const
	{
		memcpy(szAppLabel, m_szAppLabel, sizeof(char) * 17 * m_nAppNum);
		return m_nAppNum;
	}
private:
	int		m_nAppNum;
	char	m_szAppLabel[10][17];
};


void TestAry()
{
	BYTE byAppNum = 5;
	BYTE bySelIndex = 0;
	char(*pszAppLabel)[17] = new char[byAppNum][17];
	memset(pszAppLabel, 0, sizeof(char) * (17) * byAppNum);

	for (int i = 0; i<byAppNum; ++i)
	{
		sprintf(pszAppLabel[i], "AID Test %03d", i);
	}

	CTest cTest;
	cTest.SetAidList(pszAppLabel, byAppNum);

	char szAppLabel[5][17] = { 0 };
	int n = cTest.GetAidList(szAppLabel);
}


void TestCopy()
{
	unsigned int uCount = 0;
	unsigned char ucTag[8] = { 0 };
	ucTag[0] = 0x9F;
	ucTag[1] = 0x41;
	ucTag[2] = 0x04;

	for (unsigned int i = 0; i < 0xFFFFFFFF; ++i)
	{
		sprintf((char *)&ucTag[3], "%04x", i);

		if (i > 0xFFFFFF)
		{
			printf("0\n");
		}
	}
}