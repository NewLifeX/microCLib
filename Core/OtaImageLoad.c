
#include "OtaImageLoad.h"
#include "BootLoadConfig.h"
#include "Crc.h"

#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

#ifndef OTACRCFUNC
#define OTACRCFUNC CaclcCRC32B
#endif

#ifndef OTAFLASHSIZE
//(第一片 FLASH 大小  -  (BOOTLOAD大小 + 1K配置区))/ 2
// 两个APP位置平分除去BOOTLOAD、配置区的剩余空间。
#define OTAFLASHSIZE ((GetChipSize(0)-(BootLoadFlashSize+1024))/2)
#endif

#ifndef OTAFLASHADDRESS
// BOOTLOAD APP  两个位置后面就是  OTA程序存储空间了。
#define OTAFLASHADDRESS (GetChipStartAddr(0)+OTAFLASHSIZE+BootLoadFlashSize)
#endif


bool OtaImageSet(uint offset, byte* data, int len)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// 越界检查
	if (offset + len > patitionSize)return false;

	return FlashWrite(baseaddr + offset, data, len, true);
}

bool OtaImageClear(void)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	int bksize = GetFlashBlockSize(0);
	if ((baseaddr % bksize) != 0)
	{
		WarningPrintf("Please alignment\r\n");
		return false;
	}

	int bkcnt = OTAFLASHSIZE / bksize;
	for (int i = 0; i < bkcnt; i++)
	{
		EraseBlock(baseaddr + (bksize * i), false);
	}

	return true;
}

uint OtaImageCrc(uint offset, int len)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// 越界检查
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
	DebugPrintf("OtaImageCrc %d [%d] %04X\r\n", offset, len, crc);
	return crc;
}

bool OtaImageCrcCheck(uint offset, int len, uint crc2)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// 越界检查
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
	return crc == crc2;
}


#include "Debug.h"
#include "Version.h"
#include "HardwareVersion.h"

/// <summary>更新ota信息，启动升级前的动作</summary>
/// <param name="size">新app大小</param>
/// <param name="crc">新app crc</param>
/// <param name="version">版本，可填0</param>
/// <returns></returns>
bool OtaUpdateImageInfo(int size, uint crc, uint version)
{
	BootLoadConfig_t cfg;
	BootLoadGetConfig(&cfg);

	cfg.NewAppSaveAddr = OTAFLASHADDRESS;
	cfg.NewAppLoadAddr = BootLoadFlashSize + GetChipStartAddr(0);
	cfg.NewAppSize = size;
	cfg.NewAppVersion = version;

	if (!OtaImageCrcCheck(0, size, crc))
	{
		DebugPrintf("OtaUpdateImageInfo OtaImageCrcCheck Error\r\n");
		return false;
	}

	// BOOTLOAD 采用 CRC32。
	cfg.NewAppCrc32 = CaclcCRC32B((byte*)OTAFLASHADDRESS, size);

	uint newhdver = GetHardwareVersion2(OTAFLASHADDRESS, size);
	uint currhdver = GetHardwareVersion();
	if (newhdver != currhdver)
	{
		DebugPrintf("Hardware Version Error curr 0x%08X,new 0x%08X\r\n", currhdver, newhdver);
		return false;
	}

	if (version == 0) version = GetFwVersion(OTAFLASHADDRESS, size);

	// cfg.HardwareVersion = newhdver;
	cfg.NewAppVersion = version;

	if (!BootLoadSetConfig(&cfg))
	{
		DebugPrintf("OtaUpdateImageInfo BootLoadSetConfig Error\r\n");
		return false;
	}

	return true;
}

