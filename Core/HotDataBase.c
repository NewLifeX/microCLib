
#include "HotDataBase.h"
#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

// "HotD"
const uint Magic = 0x486f7444;

static byte CalcCrcXX(byte* data, int len)
{
	byte crc = 0;
	for (int i = 0; i < len; i++)crc += data[i];
	return crc;
}

// 获取
bool GetHotData(HotData_t* hot)
{
	uint magic;
	BackupRead(0, (byte*)&magic, sizeof(magic));
	if (magic != Magic)return false;

	byte crc;
	BackupRead(sizeof(Magic), (byte*)hot, sizeof(HotData_t));
	BackupRead(sizeof(Magic) + sizeof(HotData_t), (byte*)&crc, sizeof(crc));
	if (crc != CalcCrcXX((byte*)hot, sizeof(HotData_t)))return false;
	return true;
}

// 保存
bool SaveHotData(HotData_t* hot)
{
	BackupWrite(0, (byte*)&Magic, sizeof(Magic));
	byte crc = CalcCrcXX((byte*)hot, sizeof(HotData_t));
	BackupWrite(sizeof(Magic), (byte*)hot, sizeof(HotData_t));
	return  BackupWrite(sizeof(Magic) + sizeof(HotData_t), (byte*)&crc, sizeof(crc));
}

// 删除
bool CleanHotData(void)
{
	uint errmagic = 0xffffffff;
	BackupWrite(0, (byte*)&errmagic, sizeof(errmagic));
	return true;
}
