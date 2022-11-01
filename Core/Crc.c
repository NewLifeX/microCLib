﻿#include "stdint.h"
#include "Type.h"
#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

#define CRC16_IBM

#ifdef CRC16_IBM

static uint16_t const wCRC16Table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

void CRC16(const unsigned char* pDataIn, int iLenIn, unsigned short* pCRCOut)
{
	unsigned short wResult = 0;
	unsigned short wTableNo = 0;
	int i = 0;
	for (i = 0; i < iLenIn; i++)
	{
		wTableNo = ((wResult & 0xff) ^ (pDataIn[i] & 0xff));
		wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
	}

	*pCRCOut = wResult;
}

uint16_t CaclcCRC16(const unsigned char* pDataIn, int iLenIn)
{
	unsigned short wResult = 0;
	unsigned short wTableNo = 0;
	int i = 0;
	for (i = 0; i < iLenIn; i++)
	{
		wTableNo = ((wResult & 0xff) ^ (pDataIn[i] & 0xff));
		wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
	}

	return wResult;
}

#endif

static const uint32_t crc32tab[] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

uint32_t CaclcCRC32(const unsigned char* buf, uint32_t size)
{
	uint32_t i, crc;
	crc = 0xFFFFFFFF;

	for (i = 0; i < size; i++)
		crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFF;
}

uint CaclcCRC32_Accumulate(const byte* buf, uint size,uint crc)
{
	crc = crc ^ 0xFFFFFFFF;

	for (uint i = 0; i < size; i++)
		crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFF;
}

uint CaclcCRC32_AccumulateB(const byte* data, uint len, uint crc32)
{
	crc32 = crc32 ^ 0xFFFFFFFF;

	int CrcPoly = 0xedb88320;

	for (int i = 0; i < len; i++)
	{
		uint temp = data[i];
		for (int j = 0; j < 8; j++)
		{
			if (((crc32 ^ temp) & 1) != 0)
				crc32 = ((crc32 >> 1) & 0x7fffffff) ^ CrcPoly;
			else
				crc32 = ((crc32 >> 1) & 0x7fffffff);

			temp = (temp >> 1) & 0x7fffffff;
		}
	}

	return crc32 ^ 0xffffffff;
}

uint CaclcCRC32B(const byte* data, int len)
{
	int CrcPoly = 0xedb88320;

	uint crc32 = 0xffffffff;
	for (int i = 0; i < len; i++)
	{
		uint temp = data[i];
		for (int j = 0; j < 8; j++)
		{
			if (((crc32 ^ temp) & 1) != 0)
				crc32 = ((crc32 >> 1) & 0x7fffffff) ^ CrcPoly;
			else
				crc32 = ((crc32 >> 1) & 0x7fffffff);

			temp = (temp >> 1) & 0x7fffffff;
		}
	}

	return crc32 ^ 0xffffffff;
}

static void InvertUint8(byte* dBuf, byte* srcBuf)
{
	int i;
	byte tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 8; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (7 - i);
	}
	dBuf[0] = tmp[0];
}

static void InvertUint16(ushort* dBuf, ushort* srcBuf)
{
	int i;
	ushort tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 16; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (15 - i);
	}
	dBuf[0] = tmp[0];
}

// static void InvertUint32(uint* dBuf, uint* srcBuf)
// {
// 	int i;
// 	uint tmp[4];
// 
// 	tmp[0] = 0;
// 
// 	for (i = 0; i < 32; i++)
// 	{
// 		if (srcBuf[0] & (1 << i))
// 			tmp[0] |= 1 << (15 - i);
// 	}
// 	dBuf[0] = tmp[0];
// }

ushort CaclcCRC16_CCITT(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0x0000;
	ushort wCPoly = 0x1021;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin);
}

ushort CaclcCRC16_CCITT_FALSE(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0xFFFF;
	ushort wCPoly = 0x1021;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

ushort CaclcCRC16_XMODEM(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0x0000;
	ushort wCPoly = 0x1021;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

ushort CaclcCRC16_X25(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0xFFFF;
	ushort wCPoly = 0x1021;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}

	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin ^ 0xFFFF);
}

ushort CaclcCRC16_IBM(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0x0000;
	ushort wCPoly = 0x8005;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin);
}

ushort CaclcCRC16_MAXIM(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0x0000;
	ushort wCPoly = 0x8005;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin ^ 0xFFFF);
}

ushort CaclcCRC16_USB(const byte* puchMsg, uint usDataLen)
{
	ushort wCRCin = 0xFFFF;
	ushort wCPoly = 0x8005;
	byte wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin ^ 0xFFFF);
}

ushort CaclcCRC16_MODBUS(const byte* cmd, uint len)
{
	uint i, j;
		
	ushort tmp;
	ushort CRC16 = 0xFFFF;             //CRC寄存器初始值
	for (i = 0; i < len; i++)
	{
		CRC16 ^= cmd[i];
		for (j = 0; j < 8; j++)
		{
			tmp = (ushort)(CRC16 & 0x0001);
			CRC16 >>= 1;
			if (tmp == 1)
			{
				CRC16 ^= 0xA001;    //异或多项式
			}
		}
	}

	return CRC16;
}

typedef struct
{
	const char* name;
	ushort(*Crc)(const byte*, uint);
}CheckCrc_t;

static const CheckCrc_t CheckCrc[] =
{
	{"CRC16_CCITT      ",		CaclcCRC16_CCITT},
	{"CRC16_CCITT_FALSE",		CaclcCRC16_CCITT_FALSE},
	{"CRC16_XMODEM     ",		CaclcCRC16_XMODEM},
	{"CRC16_X25        ",		CaclcCRC16_X25},
	{"CRC16_MODBUS     ",		CaclcCRC16_MODBUS},
	{"CRC16_IBM        ",		CaclcCRC16_IBM},
	{"CRC16_MAXIM      ",		CaclcCRC16_MAXIM},
	{"CRC16_USB        ",		CaclcCRC16_USB},
	// {NULL,			NULL},
};

void TestCrc16(byte* data, uint len)
{
	DebugPrintf("TestCrc16\r\n");
	for (int i = 0; i < sizeof(CheckCrc)/sizeof(CheckCrc[0]); i++)
	{
		ushort crc = CheckCrc[i].Crc(data, len);
		DebugPrintf("%s\t  %04X\r\n", CheckCrc[i].name,crc);
	}
	DebugPrintf("\r\n");
}

