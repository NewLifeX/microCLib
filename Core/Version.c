﻿
#include "Type.h"
#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif
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

// C语言语法，多个字符串放一起，空格隔开。会自动拼接成一个。
const char* FirmwareBuildTime = "FirmwareBuildTime/" __DATE__ "," __TIME__;
bool GetBuildTime(struct tm* time)
{
	char monthStr[8];
	// sscanf(__DATE__, "%s %d %d", monthStr, &time->tm_mday, &time->tm_year);
	// sscanf(__TIME__, "%d:%d:%d", &time->tm_hour, &time->tm_min, &time->tm_sec);
	// DebugPrintf("%s\r\n", FirmwareBuildTime);
	sscanf(FirmwareBuildTime, "%*[^/]/%s %d %d,%d:%d:%d", monthStr, &time->tm_mday, &time->tm_year,
		&time->tm_hour, &time->tm_min, &time->tm_sec);

	char* const mon[] = { "Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec" };
	for (int i = 0; i < sizeof(mon) / sizeof(mon[0]); i++)
	{
		if (memcmp(monthStr, mon[i], 3) == 0)
		{
			time->tm_mon = i;
			break;
		}
	}
	// 处理 struct tm 取值范围。
	time->tm_year -= 1900;

	// DebugPrintf("Build Time : %d-%d-%d %d:%d:%d\r\n", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
	// 	time->tm_hour, time->tm_min, time->tm_sec);

	return true;
}

#ifdef PUBLISHTIME

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

#endif

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

#ifdef PUBLISHTIME
	// 尝试获取发布版本时间戳，失败获取编译时间戳
	if (!GetPublishesTime(&time))
	{
		GetBuildTime(&time);
	}
#else
	GetBuildTime(&time);
#endif

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

	char monthStr[8];
	sscanf(p, "%*[^/]/%s %d %d,%d:%d:%d", monthStr, &time.tm_mday, &time.tm_year,
		&time.tm_hour, &time.tm_min, &time.tm_sec);

	char* const mon[] = { "Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec" };
	for (int i = 0; i < sizeof(mon) / sizeof(mon[0]); i++)
	{
		if (memcmp(monthStr, mon[i], 3) == 0)
		{
			time.tm_mon = i;
			break;
		}
	}
	// 处理 struct tm 取值范围。
	time.tm_year -= 1900;

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;

	return minuint;
}


#ifdef GITVERSION

static byte ChToByte(char ch)
{
	if ((ch >= '0') && (ch <= '9'))return (byte)ch - (byte)'0';
	if ((ch >= 'a') && (ch <= 'f'))return (byte)ch - (byte)'a' + 10;
	if ((ch >= 'A') && (ch <= 'F'))return (byte)ch - (byte)'A' + 10;

	return 0;
}

static byte ChsToByte(char* chs)
{
	byte n0 = ChToByte(chs[0]);
	byte n1 = ChToByte(chs[1]);

	return (n0 << 4) + n1;
}

bool GetGitVersion(GitVersion_t* ver)
{
	if (ver == NULL)return false;
	if (memcmp("NO GIT", GITINFO, 6) == 0)return false;
	int glen = strlen(GITINFO);
	if (glen < 70)return false;


	struct tm time;
	int num = sscanf(GITINFO, "%d-%d-%d %d:%d:%d ",&time.tm_year, &time.tm_mon, &time.tm_mday,
		&time.tm_hour, &time.tm_min, &time.tm_sec);
	if (num < 6) return false;

	// 处理 struct tm 取值范围。
	time.tm_year -= 1900;
	time.tm_mon -= 1;

	struct tm start =
	{
		.tm_year = 100,		// 其值等于实际年份减去1900
		.tm_mon = 0,		// 月份（从一月开始，0代表一月） - 取值区间为[0,11]
		.tm_mday = 1,		// 一个月中的日期 - 取值区间为[1,31] 
		.tm_hour = 0,		// 时 - 取值区间为[0,23]
		.tm_min = 0,		// 分 - 取值区间为[0,59]
		.tm_sec = 0,		// 秒 - 取值区间为[0,59]
	};

	// 计算秒差/60=分钟差。
	double min = difftime(mktime(&time), mktime(&start)) / 60.0;
	uint minuint = (int)min;
	ver->TimeMin = minuint;

	// SHA-1   字符串转数组
	int offset = ArrayIndexOf((byte*)GITINFO, glen, (byte*)"\r\n", 2);
	if (offset < 0)return false;
	offset += 2;
	char* psha = (char*) &GITINFO[offset];
	for (int i = 0; i < 20; i++)
	{
		ver->Md5[i] = ChsToByte(psha);
		psha += 2;
	}

	// 从后面往前查
	offset = ArrayLastIndexOf((byte*)GITINFO, glen, (byte*)"\r\n", 2);
	offset += 2;

	ver->Branch = (char*) &GITINFO[offset];
	// ver->Branch += 2;
	
	return true;
}

#else

bool GetGitVersion(GitVersion_t* ver) { return false; }

#endif
