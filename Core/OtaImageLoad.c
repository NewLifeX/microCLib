

#include "OtaImageLoad.h"
#include "BootLoadConfig.h"


#include "Crc.h"
// CRC ʹ�õĺ���
#define OTACRCFUNC CaclcCRC16_MODBUS

#ifndef OTACRCFUNC
#define OTACRCFUNC CaclcCRC32
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

uint OtaImageCrc(uint offset, int len)
{
	int baseaddr = OTAFLASHADDRESS;
	int patitionSize = OTAFLASHSIZE;

	// Խ����
	if (offset + len > patitionSize)return false;

	uint crc = OTACRCFUNC((byte*)(baseaddr + offset), len);
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



