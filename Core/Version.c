
#include "Type.h"
#include "Debug.h"
#include "Array.h"

#include "Version.h"

#include "time.h"
#include "stdio.h"

// 显示时间
void structtmShow(struct tm* time)
{
	DebugPrintf("%d-%d-%d %d:%d:%d",
		time->tm_year + 1900,
		time->tm_mon + 1,
		time->tm_mday,
		time->tm_hour,
		time->tm_min,
		time->tm_sec
	);
}

static int StrMonToInt(char* str)
{
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	int mon = (strstr(month_names, str) - month_names) / 3;

	return mon;
}

// C语言语法，多个字符串放一起，空格隔开。会自动拼接成一个。
const char* FirmwareBuildTime = "FirmwareBuildTime/" __DATE__ "," __TIME__;
bool GetBuildTime(struct tm* time)
{
	char monthStr[3];
	// sscanf(__DATE__, "%s %d %d", monthStr, &time->tm_mday, &time->tm_year);
	// sscanf(__TIME__, "%d:%d:%d", &time->tm_hour, &time->tm_min, &time->tm_sec);
	// DebugPrintf("%s\r\n", FirmwareBuildTime);
	sscanf(FirmwareBuildTime, "%*[^/]/%s %d %d,%d:%d:%d", monthStr, &time->tm_mday, &time->tm_year,
		&time->tm_hour, &time->tm_min, &time->tm_sec);

	time->tm_mon = StrMonToInt(monthStr);

	// static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	// time->tm_mon = (strstr(month_names, monthStr) - month_names) / 3;

	// char* const mon[] = { "Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec" };
	// for (int i = 0; i < sizeof(mon) / sizeof(mon[0]); i++)
	// {
	// 	if (memcmp(monthStr, mon[i], 3) == 0)
	// 	{
	// 		time->tm_mon = i;
	// 		break;
	// 	}
	// }

	// 处理 struct tm 取值范围。
	time->tm_year -= 1900;

	// DebugPrintf("Build Time : %d-%d-%d %d:%d:%d\r\n", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
	// 	time->tm_hour, time->tm_min, time->tm_sec);

	return true;
}

const char* PublishesTime = "PublishesTime/yyyy-MM-dd HH:mm:ss";
// static const char* PublishesTime = "2019-12-20 15:22:30";

// 发布时间是通过外部工具直接替换bin文件内的 yyyy-MM-dd HH:mm:ss 字符串。
bool GetPublishesTime(struct tm* time)
{
	// 发现没有被修改。说明没有通过工具发布过。
	// 不能再出现 PublishesTime/ 否则上位机小工具找到的内容会出问题。
	// if (memcmp("PublishesTime/yyyy", PublishesTime, 18) == 0)return false;
	if (strstr(PublishesTime, "yyyy") != 0)return false;

	sscanf(PublishesTime, "%*[^/]/%d-%d-%d %d:%d:%d", &time->tm_year, &time->tm_mon, &time->tm_mday,
		&time->tm_hour, &time->tm_min, &time->tm_sec);

	// 处理 struct tm 取值范围。
	time->tm_year -= 1900;
	time->tm_mon -= 1;

	// DebugPrintf("Publishes Time : %d-%d-%d %d:%d:%d\r\n", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
	// 	time->tm_hour, time->tm_min, time->tm_sec);

	return true;
}

// 获取版本
uint GetVersion(void)
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

	// 尝试获取发布版本时间戳，失败获取编译时间戳
	if (!GetPublishesTime(&time))
	{
		GetBuildTime(&time);
	}

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;

	return minuint;
}

/// <summary>获取软件版本,参考 Version.c</summary>
/// <param name="addr">固件起始地址</param>
/// <param name="len">固件大小</param>
/// <returns></returns>
uint GetFwVersion(uint addr, int len)
{
	char* head = "FirmwareBuildTime/";
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

	char monthStr[4];
	sscanf(p, "%*[^/]/%s %d %d,%d:%d:%d", monthStr, &time.tm_mday, &time.tm_year,
		&time.tm_hour, &time.tm_min, &time.tm_sec);

	time.tm_mon = StrMonToInt(monthStr);

	// static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	// time.tm_mon = (strstr(month_names, monthStr) - month_names) / 3;

	// char* const mon[] = { "Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec" };
	// for (int i = 0; i < sizeof(mon) / sizeof(mon[0]); i++)
	// {
	// 	if (memcmp(monthStr, mon[i], 3) == 0)
	// 	{
	// 		time.tm_mon = i;
	// 		break;
	// 	}
	// }

	// 处理 struct tm 取值范围。
	time.tm_year -= 1900;

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;

	return minuint;
}
