
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
// 按照后备寄存器的 2 字节对齐计算长度。
#define HotLen  ((sizeof(Magic) + sizeof(HotData_t) + sizeof(byte) + 1) & 0xfffffffe)

static byte CalcCrcXX(byte* data, int len)
{
	byte crc = 0;
	for (int i = 0; i < len; i++)crc += data[i];
	return crc;
}

static bool ReadHot(int offset, HotData_t* hot)
{
	byte temp[HotLen];
	BackupRead(offset, temp, sizeof(temp));

	uint magic = *(uint*)&temp[0];
	if (magic != Magic)return false;

	byte crc = temp[sizeof(magic) + sizeof(HotData_t)];
	if (crc != CalcCrcXX(&temp[sizeof(magic)], sizeof(HotData_t)))return false;

	memcpy(hot, temp + sizeof(magic), sizeof(HotData_t));
	return true;
}

// 获取
bool GetHotData(HotData_t* hot)
{
	if (ReadHot(0, hot))return true;
	if (ReadHot(HotLen, hot))return true;
	return false;
}

/*
static bool WriteHot(int offset, HotData_t* hot)
{
	byte crc = CalcCrcXX(hot, sizeof(HotData_t));
	byte temp[HotLen];

	memcpy(temp, (byte*)&Magic, sizeof(Magic));
	memcpy(temp + sizeof(Magic), hot, sizeof(HotData_t));
	memcpy(temp + sizeof(Magic) + sizeof(HotData_t), &crc, sizeof(crc));

	return BackupWrite(offset, temp, sizeof(temp));
}
*/

// 保存
bool SaveHotData(HotData_t* hot)
{
	byte crc = CalcCrcXX((byte*)hot, sizeof(HotData_t));
	byte temp[HotLen];

	memcpy(temp, (byte*)&Magic, sizeof(Magic));
	memcpy(temp + sizeof(Magic), hot, sizeof(HotData_t));
	memcpy(temp + sizeof(Magic) + sizeof(HotData_t), &crc, sizeof(crc));

	// 写两遍，只判断第一次结果。
	bool rs = BackupWrite(0, temp, sizeof(temp));
	BackupWrite(HotLen, temp, sizeof(temp));

	return rs;
}

// 删除
bool CleanHotData(void)
{
	uint errmagic = 0xffffffff;
	BackupWrite(0, (byte*)&errmagic, sizeof(errmagic));
	return true;
}
