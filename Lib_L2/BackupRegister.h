
#ifndef __BKPREG_H__
#define __BKPREG_H__

#include "Type.h"

// 初始化
void BackupRegInit(void);

// 写
// regOffset 单位字节。不是后备寄存器的寄存器编号。
bool BackupWrite(uint regOffset,byte* data,int len);

// 读
// 返回实际读取长度
// regOffset 单位字节。不是后备寄存器的寄存器编号。
int BackupRead(uint regOffset, byte* data, int len);

#if 1
// 测试例程
void BackupTest(void);
#endif

#endif
