
#include "Type.h"
#include "Debug.h"

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

bool GetBuildTime(struct tm* time)
{
	char monthStr[8];
	// DebugPrintf("%s %s\r\n", __DATE__, __TIME__); => Dec 20 2019 14:04:04
	sscanf(__DATE__, "%s %d %d", monthStr, &time->tm_mday, &time->tm_year);
	sscanf(__TIME__, "%d:%d:%d", &time->tm_hour, &time->tm_min, &time->tm_sec);

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

const char* PublishesTime = "PublishesTime/yyyy-MM-dd HH:mm:ss";
// static const char* PublishesTime = "2019-12-20 15:22:30";

// 发布时间是通过外部工具直接替换bin文件内的 yyyy-MM-dd HH:mm:ss 字符串。
bool GetPublishesTime(struct tm* time)
{
	// 发现没有被修改。说明没有通过工具发布过。
	// 不能再出现 PublishesTime/ 否则上位机小工具找到的内容会出问题。
	// if (memcmp("PublishesTime/yyyy", PublishesTime, 18) == 0)return false;
	if (strstr(PublishesTime,"yyyy") != 0)return false;

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
