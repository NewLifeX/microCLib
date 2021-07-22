
#include "BackMemory.h"
#include "Debug.h"

// 内存不需要初始化
void BackupRegInit(void)
{
	int Size = BackMemoryEnd - MemoryBeginAddr;
	if (Size < 4)
	{
		ErrPrintf("Not setting IRAM1 start address !!!");
		/*
		* 没有配置有效的保留内存，无法使用此模块。
		*/
		while (1);
	}

	DebugPrintf("BackupReg Size %d\r\n",Size);
	DebugPrintf("\t0x%08X -- 0x%08X\r\n", MemoryBeginAddr, BackMemoryEnd);
}


// 写
// regOffset 单位字节。不是后备寄存器的寄存器编号。
bool BackupWrite(uint regOffset, byte* data, int len)
{
	if (len > (BackMemoryEnd - MemoryBeginAddr))return false;

	memcpy((byte*)(MemoryBeginAddr + regOffset), data, len);
	return true;
}

// 读
// 返回实际读取长度
// regOffset 单位字节。不是后备寄存器的寄存器编号。
int BackupRead(uint regOffset, byte* data, int len)
{
	int Size = BackMemoryEnd - MemoryBeginAddr;
	if (Size < 1)
	{
		ErrPrintf("Not setting IRAM1 start address !!!");
	}

	if (len > Size)len = Size;

	memcpy(data ,(byte*)(MemoryBeginAddr + regOffset), len);
	return len;
}



