#pragma once

#include "Type.h"

/*
热数据（自己命名的）。热重启数据读取。
重启后，拿到重启前的数据。

底层有两种形式，硬件后备寄存器和软件内存数据。
*/

// 使用硬件后备寄存器
// #define HardwareBackReg

#ifdef HardwareBackReg
#include "BackupRegister.h"
#else
#include "BackMemory.h"
#endif

// 具体HotData 放到这个头文件定义。  自行实现。
#include "HotData.h"

// 获取
bool GetHotData(HotData_t* hot);

// 保存
bool SaveHotData(HotData_t* hot);

// 删除
bool CleanHotData(void);

/*
使用方式：
1. 添加 HotData.h 文件

#include "Type.h"

typedef struct
{
	ushort RebootCnt;
	ushort MsgCnt;
}HotData_t;

2.
// 使用样例，
在 SysInfo_t 结构体里面加入。
HotData_t HotData;

// 在系统初始化阶段加入。
if(!GetHotData(&SysInfo.HotData))
{
	//写默认数值 
	SysInfo.HotData.RebootCnt = 0;
	...
	SaveHotData(&SysInfo.HotData);
}
else
{
	SysInfo.HotData.RebootCnt++;
}

// 在错误中断函数里面加入。
// 在 main 循环里面加入。或者 在周期任务里加入。
SaveHotData(&SysInfo.HotData);

*/

