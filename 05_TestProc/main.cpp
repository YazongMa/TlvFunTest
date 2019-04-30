#include <Windows.h>
#include <stdio.h>
#include <string>
#include "paramwrite.h"


using namespace std;

ParamWrite paramWrite;

BOOL isCompressedTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen);
char* UnCompressTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen);
void TestAmt();

DWORD Bcd2Str(string *pcSrc, string &cOut)
{
	if (pcSrc == NULL)
		return 1;

	PBYTE src_ptr = (PBYTE)pcSrc->c_str();
	int nLen = pcSrc->length();
	cOut.clear();

	while (nLen--)
	{
		cOut += ((BYTE)(((*src_ptr & 0xf0) <= 0x90 ? '0' : 'A') + ((*src_ptr >> 4) % 10)));
		cOut += ((BYTE)(((*src_ptr & 0x0f) <= 0x09 ? '0' : 'A') + ((*src_ptr & 0x0f) % 10)));
		src_ptr++;
	}

	return 0;
}

int main()
{
	TestAmt();
	return 0;
	//visa:03, discover:12, mc:02, AMEX:04 
	char KernelID[2] = { 0 };
	char KernelID_Out[4] = { 0 };
	KernelID[0] = 0x00;
	KernelID[1] = 0x03;
	paramWrite.Str2Hex((const char*)KernelID, (BYTE *)&KernelID_Out[0]);

	char* pTrackData = ";374245001781003=200170212050299800508?";
	isCompressedTrack2Data((BYTE *)pTrackData, strlen((char *)pTrackData));

	pTrackData = "374245001781003=200170212050299800508?";
	isCompressedTrack2Data((BYTE *)pTrackData, strlen((char *)pTrackData));

	char TrackData[] = { 0x54, 0x13, 0x33, 0x00, 0x89, 0x60, 0x41, 0x11, 0xd2, 0x21, 0x22, 0x01, 0x01, 0x23, 0x40, 0x91, 0x72, 0x02, 0x9f};
	isCompressedTrack2Data((BYTE *)TrackData, 19);

	pTrackData = UnCompressTrack2Data((BYTE *)TrackData, 19);

	char TrackOut[256] = { 0 };
	paramWrite.Hex2Str((BYTE *)TrackOut, (BYTE *)TrackData, 19);

	return 0;
}




BOOL isCompressedTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen)
{
	int nLoop = 0;
	char* pTmp = NULL;
	BOOL bRet = FALSE;
	do
	{
		if (!pbyTrack2Data)
		{
			break;
		}

		if (*pbyTrack2Data == ';')
		{
			break;
		}

		if (*pbyTrack2Data < '0' || *pbyTrack2Data > '9')
		{
			bRet = TRUE;
			break;
		}

		while (nLoop < bTrack2DataLen)
		{
			if (pbyTrack2Data[nLoop] == '=')
			{
				pTmp = (char *)&pbyTrack2Data[nLoop];
				break;
			}
			++nLoop;
		}

		if (!pTmp)
		{
			bRet = TRUE;
			break;
		}

		while ((char *)pbyTrack2Data != pTmp)
		{
			if (*pbyTrack2Data < '0' || *pbyTrack2Data > '9')
			{
				bRet = TRUE;
			}
			pbyTrack2Data++;
		}
	} while (0);

	return bRet;
}


char* UnCompressTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen)
{
	int nLoop = 0;
	static char Track2Data[256] = { 0 };
	
	while (nLoop != bTrack2DataLen)
	{
		printf("%x ", pbyTrack2Data[nLoop]);
		sprintf(&Track2Data[nLoop * 2], "%02x", pbyTrack2Data[nLoop]);
		++nLoop;
	}

	return Track2Data;
}


void TestAmt()
{
	unsigned long long ullAmt = 1100;
	char szAmt[6] = { 0 };
	szAmt[5] = ullAmt;
	szAmt[4] = ullAmt >> 8;
	szAmt[3] = ullAmt >> 16;
	szAmt[2] = ullAmt >> 24;
	szAmt[1] = ullAmt >> 32;
	szAmt[0] = ullAmt >> 40;
}