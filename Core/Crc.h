#ifndef __BSP_CRC_H
#define __BSP_CRC_H 	


#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include "Type.h"

// IBM 方式
void CRC16(const unsigned char* pDataIn, int iLenIn, unsigned short* pCRCOut);
uint16_t CaclcCRC16(const unsigned char* pDataIn, int iLenIn);

// 计算CRC
uint32_t CaclcCRC32(const unsigned char* buf, uint32_t size);

// 累积的CRC方式。
// 衔接上一个CRC的时候需要 crc^= 0xffffffff;
// 例程
// uint crc = CaclcCRC32_Accumulate( buf1, buf1size, 0xFFFFFFFF);  // 或者 uint crc = CaclcCRC32( buf1, buf1size);
// crc^= 0xffffffff;
// crc = CaclcCRC32_Accumulate( buf2, buf2size, crc);
uint CaclcCRC32_Accumulate(const byte* buf, uint size, uint crc);


// 测试是哪种CRC
void TestCrc16(byte* data, uint len);
// MODBUS 计算方式。
ushort CaclcCRC16_MODBUS(const byte* cmd, uint len);

// 未放到头文件的计算方式是未优化的，计算非常耗费CPU。 需要使用的时候找优化过的再放出来。

#endif
