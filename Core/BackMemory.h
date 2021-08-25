#pragma once

/*
实现目标/目的： 纯软件实现后备寄存器 BackupRegister  （见Lib_L2目录）
单片机重启（中途不断电）。数据不丢。

实现方式：
内存中指定一段内存不做初始化。
手动填充内容进去，并做校验，避免数据有误。
有三种方式：
	1.MDK配置的内存头尾地址，保留一段空间。
		优点：BOOT,APP 等方便统一配置。
		缺点：应用说明需要说清楚，不了解情况的人容易误解，一旦将配置修改，程序会跑飞。
	2.使用不初始化值的全局变量数组。
		优点：使用方便，想多大一个define就可以修改完成。
		缺点：有BOOT的时候，无法统一配置。（变量分配地址是编译器决定的）
		有人会说，可以用 attitude 指定内存区位置。 但是这个太高深，不容易理解。
	3.使用分散加载文件.sct文件描述指定段。（clang、gcc 等使用.lds文件）
		优点：正统方式，linux  uboot等都这么用。
		缺点：难度很大。

目前决定使用方式 1 。 并挑选内存的首部位置，避免与freertos 冲突。
	freertos 启动的时候会从硬件寄存器读取内存大小，将栈地址设置为内存最大地址。并不听从编译器设置的值。

需要配合的文件：分散加载文件  .sct
在 RW_IRAM1 内添加一行。
*.o (BKMASK,+First)

将 BKMASK 加入到 RAM 区的首位置。

*/

#include "Type.h"

// 硬件上的内存首地址
#define MemoryBeginAddr 0x20000000

// 初始化
void BackupRegInit(void);

// 写
// regOffset 单位字节。不是后备寄存器的寄存器编号。
bool BackupWrite(uint regOffset, byte* data, int len);

// 读
// 返回实际读取长度
// regOffset 单位字节。不是后备寄存器的寄存器编号。
int BackupRead(uint regOffset, byte* data, int len);
