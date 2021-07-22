

#include "OtaImageLoad.h"
#include "BootLoadConfig.h"


#include "Crc.h"
// CRC 使用的函数
#define OTACRCFUNC CaclcCRC16_MODBUS

#ifndef OTACRCFUNC
#define OTACRCFUNC CaclcCRC32
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

uint OtaImageCrc(uint offset, int len)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// 越界检查
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
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



