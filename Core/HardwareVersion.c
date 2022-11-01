
#include "HardwareVersion.h"

#include "Type.h"
#include "Array.h"

#include "time.h"
#include "stdio.h"

#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

// 硬件时间
const char* HardwareTime = "HardwareTime/" HARDWARETIME;

// 发布时间是通过外部工具直接替换bin文件内的 yyyy-MM-dd HH:mm:ss 字符串。
static bool GetTime(struct tm* time)
{
	sscanf(HardwareTime, "%*[^/]/%d-%d-%d", &time->tm_year, &time->tm_mon, &time->tm_mday);

	// 处理 struct tm 取值范围。
	time->tm_year -= 1900;
	time->tm_mon -= 1;

	// DebugPrintf("Publishes Time : %d-%d-%d %d:%d:%d\r\n", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
	// 	time->tm_hour, time->tm_min, time->tm_sec);

	return true;
}

// 获取版本
uint GetHardwareVersion(void)
{
	struct tm start =
	{
		.tm_year = 100,		// 其值等于实际年份减去1900
		.tm_mon = 0,		// 月份（从一月开始，0代表一月） - 取值区间为[0,11]
		.tm_mday = 1,		// 一个月中的日期 - 取值区间为[1,31] 
		.tm_hour = 0,		// 时 - 取值区间为[0,23]
		.tm_min = 0,		// 分 - 取值区间为[0,59]
		.tm_sec = 0,		// 秒 - 取值区间为[0,59]
	};

	// 编译时间。
	struct tm time;
	memset(&time, 0, sizeof(time));

	// 尝试获取发布版本时间戳，失败获取编译时间戳
	GetTime(&time);

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;

	return minuint;
}

/// <summary>获取硬件版本,参考 HardwareVersion.c</summary>
/// <param name="addr">固件起始地址</param>
/// <param name="len">固件大小</param>
/// <returns></returns>
uint GetHardwareVersion2(uint addr, int len)
{
	char* head = "HardwareTime/";
	int headlen = strlen(head);
	int idx = ArrayIndexOf((byte*)addr, len, (byte*)head, headlen);
	if (idx == -1)return 0;

	char* p = (char*)(addr + idx);
	if (strlen(p) == headlen)
	{
		// 因为本函数的问题，字符串会多一份。需要过滤。
		idx = ArrayIndexOf_Offset((byte*)addr, len, (byte*)head, headlen, idx + headlen);

		if (idx == -1)return 0;
		p = (char*)(addr + idx);
	}

	struct tm start =
	{
		.tm_year = 100,		// 其值等于实际年份减去1900
		.tm_mon = 0,		// 月份（从一月开始，0代表一月） - 取值区间为[0,11]
		.tm_mday = 1,		// 一个月中的日期 - 取值区间为[1,31] 
		.tm_hour = 0,		// 时 - 取值区间为[0,23]
		.tm_min = 0,		// 分 - 取值区间为[0,59]
		.tm_sec = 0,		// 秒 - 取值区间为[0,59]
	};

	// 编译时间。
	struct tm time;
	memset(&time, 0, sizeof(time));
	sscanf(p, "%*[^/]/%d-%d-%d", &time.tm_year, &time.tm_mon, &time.tm_mday);

	// 处理 struct tm 取值范围。
	time.tm_year -= 1900;
	time.tm_mon -= 1;

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;

	return minuint;
}
