
#include "OtaImageLoad.h"
#include "BootLoadConfig.h"
#include "Crc.h"
#include "Debug.h"

#ifndef OTACRCFUNC
#define OTACRCFUNC CaclcCRC32B
#endif

#ifndef OTAFLASHSIZE
//(��һƬ FLASH ��С  -  (BOOTLOAD��С + 1K������))/ 2
// ����APPλ��ƽ�ֳ�ȥBOOTLOAD����������ʣ��ռ䡣
#define OTAFLASHSIZE ((GetChipSize(0)-(BootLoadFlashSize+1024))/2)
#endif

#ifndef OTAFLASHADDRESS
// BOOTLOAD APP  ����λ�ú������  OTA����洢�ռ��ˡ�
#define OTAFLASHADDRESS (GetChipStartAddr(0)+OTAFLASHSIZE+BootLoadFlashSize)
#endif


bool OtaImageSet(uint offset, byte* data, int len)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// Խ����
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

	// Խ����
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
	DebugPrintf("OtaImageCrc %d [%d] %04X\r\n", offset, len, crc);
	return crc;
}

bool OtaImageCrcCheck(uint offset, int len, uint crc2)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// Խ����
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
	return crc == crc2;
}


#include "Debug.h"
#include "Version.h"
#include "HardwareVersion.h"

/// <summary>����ota��Ϣ����������ǰ�Ķ���</summary>
/// <param name="size">��app��С</param>
/// <param name="crc">��app crc</param>
/// <param name="version">�汾������0</param>
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

	// BOOTLOAD ���� CRC32��
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

