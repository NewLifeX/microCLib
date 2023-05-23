
#include "BackMemory.h"
#include "Debug.h"

/*
// 从map文件抠出来的，MDK设置页面设置的IRAM1 start地址值。
extern uint __microlib_freelist;
#define BackMemoryEnd ((uint)&__microlib_freelist)
*/

byte BKMask[64]   __attribute__((at(MemoryBeginAddr)));

// 内存不需要初始化
void BackupRegInit(void)
{
	// int a = BKMask;
	// if (a == 0xffffffff)return;
	if (sizeof(BKMask) < 4)
	{
		ErrPrintf("BKMask size error");
		/*
		* 没有配置有效的保留内存，无法使用此模块。
		*/
		while (1);
	}

	DebugPrintf("BackupReg Size %d\r\n", sizeof(BKMask));
	DebugPrintf("\t0x%08X -- 0x%08X\r\n", BKMask, BKMask + sizeof(BKMask));
}


// 写
// regOffset 单位字节。不是后备寄存器的寄存器编号。
bool BackupWrite(uint regOffset, byte* data, int len)
{
	if (len > sizeof(BKMask))return false;

	memcpy((byte*)(BKMask + regOffset), data, len);
	return true;
}

// 读
// 返回实际读取长度
// regOffset 单位字节。不是后备寄存器的寄存器编号。
int BackupRead(uint regOffset, byte* data, int len)
{
	int Size = sizeof(BKMask);
	if (Size < 1)
	{
		ErrPrintf("Not setting IRAM1 start address !!!");
	}

	if (len > Size)len = Size;

	memcpy(data ,(byte*)(BKMask + regOffset), len);
	return len;
}



