
#include "Debug.h"
#include "Type.h"

#if defined ( __CC_ARM )
__weak MallocFunc	GlobleMalloc = malloc;
__weak FreeFunc		GlobleFree = free;
#endif

#if defined ( __GNUC__ )
__attribute__((weak)) MallocFunc	 GlobleMalloc = malloc;
__attribute__((weak)) FreeFunc    GlobleFree = free;
#endif

__inline void InitBuffer(Buffer_t* data)	{ memset(data, 0x00, sizeof(Buffer_t));	data->free = GlobleFree;}
__inline void InitBuffer2(Buffer2_t* data)	{ memset(data, 0x00, sizeof(Buffer2_t));data->free = GlobleFree;}

__inline void TryCleanBuffer(Buffer_t* data)
{
	if (data->needFree)
	{
#ifdef DEBUG
		if (data->free == NULL)ErrPrintf("TryCleanBuffer Free = NULL");
		if (data->p == NULL)ErrPrintf("TryCleanBuffer p = NULL");
#endif
		data->free(data->p);
		data->needFree = false;
	}
}

__inline void TryCleanBuffer2(Buffer2_t* data)
{
	if (data->needFree)
	{
#ifdef DEBUG
		if (data->free == NULL)ErrPrintf("TryCleanBuffer2 Free = NULL");
		if (data->freePoint == NULL)ErrPrintf("TryCleanBuffer2 p = NULL");
#endif
		data->free(data->freePoint);
		data->needFree = false;
	}
}

void ShowArray(char* name, byte* pdata, int datalen, int showlen)
{
#ifdef DEBUG
	uint printlen = datalen > showlen ? showlen : datalen;
	DebugPrintf("%s [%d] = ", name, datalen);

	if (printlen > 32)
	{
		for (int i = 0; i < printlen; i++)
		{
			if (i % 32 == 0)DebugPrintf("\r\n");
			DebugPrintf("%02X ", pdata[i]);
		}
	}
	else
	{
		for (int i = 0; i < printlen; i++)DebugPrintf("%02X ", pdata[i]);
	}

	DebugPrintf(printlen == datalen ? "\r\n" : "...\r\n");
#endif
}

void ShowBuff(Buffer_t* buff, int len)
{
	ShowArray("",buff->p,buff->len,len);
}

void ShowBuff2(Buffer2_t* buff, int len)
{
	ShowArray("", buff->data, buff->len, len);
}
