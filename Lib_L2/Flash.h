
#ifndef __FLASH_H__
#define __FLASH_H__

typedef char					sbyte;
typedef unsigned char			byte;
typedef unsigned short			ushort;
typedef unsigned int			uint;
typedef long long int			int64;
typedef unsigned long long int	uint64;

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cType.h>

// 写一个 Block  长度不能大于 BlockSize		不禁用中断，外部需要处理禁用中断
bool WriteBlock(uint address, byte* buf, int len, bool closeHsi);
// 擦除   Block								不禁用中断，外部需要处理禁用中断
bool EraseBlock(uint address, bool closeHsi);

/// <summary>检查是不是满足直接写（不需要擦除）要求</summary>
/// <param name="address"></param>
/// <param name="buf"></param>
/// <param name="len"></param>
/// <returns></returns>
bool FlashCheckDirectWrite(uint address, byte* buf, int len);

/// <summary>直接写数据</summary>
/// <param name="address"></param>
/// <param name="buf"></param>
/// <param name="len"></param>
/// <param name="closeHsi">执行完是否需要关闭HSI时钟。（STM32 FLASH操作需要HSI，其他MCU没有要求）</param>
/// <returns></returns>
bool DirectWrite(uint address, byte* buf, int len, bool closeHsi);

/// <summary>写入Flash，自动处理擦除。对齐访问等</summary>
/// <param name="address"></param>
/// <param name="buf"></param>
/// <param name="len"></param>
/// <param name="closeHsi"></param>
/// <returns></returns>
bool FlashWrite(uint address, byte* buf, int len, bool closeHsi);

// 批量写 Flash 。 暂时只支持写第一片Flash。 
// 不校验saveaddr合法性。
// 内部已经处理禁用中断
bool FlashSaveBin(uint saveaddr, byte* buff, uint len);



// chip 片。不论是STM32 还是GD32都有多种 BlockSize
// 这里直接将不同BlockSize 当成不同片！!
int GetFlashSize(void);
uint GetChipStartAddr(byte chip);
int GetChipSize(byte chip);
int GetFlashBlockSize(byte chip);

// 测试代码
// uint FlashAddr = 0x8000000 + (31 << 10);
// const char* str = "abcdefg\r\n\0";
// const char* str2 = "gfedcba\r\n\0";
// 
// RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_FMC, ENABLE);
// EraseBlock(FlashAddr, false);
// WriteBlock(FlashAddr, (byte*)str, strlen(str), false);
// 
// 
// DebugPrintf("%s", (const char*)FlashAddr);
// 
// EraseBlock(FlashAddr, false);
// WriteBlock(FlashAddr, (byte*)str2, strlen(str2), false);
// RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_FMC, DISABLE);
// 
// DebugPrintf("%s", (const char*)FlashAddr);


#endif

