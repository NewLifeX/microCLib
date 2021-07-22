
#include "AsyncData.h"
#ifdef DEBUG
#include "Debug.h"
#endif

__inline void InitAsyncData(AsyncData_t* pkt)
{
	if (pkt == NULL)return;
	memset(pkt, 0x00, sizeof(Buffer_t));
}

bool InitAsyncData2(AsyncData_t* pkt, int len)
{
	if (pkt == NULL)return false;
	memset(pkt, 0x00, sizeof(Buffer_t));
	if (len < 1)return false;

	byte* p = (byte*)GlobleMalloc(len);
	if (p == NULL)return false;

	pkt->Data = p;
	pkt->BuffLen = len;
	pkt->Datalen = 0;
	pkt->AsyncTime = 0;
	pkt->free = GlobleFree;
	pkt->needFree = true;

	return true;
}

void RemoveAsyncData(AsyncData_t* pkt)
{
	if (pkt == NULL)return;
	pkt->Datalen = 0;
}

__inline void TryFreeAsyncData(AsyncData_t* pkt)
{
	if (pkt == NULL)return;
	if (pkt->needFree)
	{
#ifdef DEBUG
		if (pkt->free == NULL)ErrPrintf("TryCleanAsyncData Free = NULL");
		if (pkt->Data == NULL)ErrPrintf("TryCleanAsyncData Data = NULL");
#endif
		pkt->free(pkt->Data);
		pkt->needFree = false;
	}
}

int GetAsyncData(AsyncData_t* pkt, byte** data)
{
	if (pkt == NULL)return 0;
	if (pkt->Data == NULL)return 0;
	if (data == NULL)return 0;

	if (pkt->Data != NULL)*data = pkt->Data;
	return pkt->Datalen;
}

int PushAsyncData(AsyncData_t* pkt, byte* data, int len, uint atime)
{
	if (pkt == NULL)return 0;
	if (pkt->Data == NULL)return 0;
	if (data == NULL)return 0;

	if (len > pkt->BuffLen)len = pkt->BuffLen;
	if (pkt->Data != data)
	{
		memcpy(pkt->Data, data, len);
	}

	pkt->Datalen = len;
	pkt->AsyncTime = atime;
	return len;
}
