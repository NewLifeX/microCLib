
#ifndef __CPU_CORE_H_
#define __CPU_CORE_H_

#include "Type.h"

typedef struct
{
	bool IsGD;
	// CPU 名称
	char Name[16];
	// CPU 唯一ID
	byte ID[12];
	// 单位 KByte
	ushort FlashSize;
	// 单位 KByte
	ushort RamSize;
	// 最大频率
	uint MaxFreq;
}CpuInfo_t;

// 重启
void SysReset(void);

// 启动另外一个 Bin。 要么启动成功，要么死机。反正不回了。
void Boot(uint binAddr);

uint GetCpuFreq(void);

uint GetRamSize(void);

void GetCpuid(byte* did, int len);

void GetCpuInfo(CpuInfo_t* info);

/// <summary>打印开机方式</summary>
void ShowPowerOnMode(void);

#endif
