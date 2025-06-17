

#include "TimeHelper.h"
#include "Type.h"

#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... ) printf(__VA_ARGS__)
#define TraceThis( ... )
#endif


#include "time.h"
void print_tm(const struct tm *tm) 
{
    DebugPrintf("%04d-%02d-%02d %02d:%02d:%02d\r\n",
           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec);
}

time_t GeTimeStampBase(void)
{
	struct tm time = {0};
	const char* bStr = "2000-01-01";
	sscanf(bStr, "%d-%d-%d", &time.tm_year, &time.tm_mon, &time.tm_mday);
	// 处理 struct tm 取值范围。
	time.tm_year -= 1900;
	time.tm_mon -= 1;

	return mktime(&time);
}

void ShowTimeStamp_Sec(int sec)
{
	time_t tsec = GeTimeStampBase();
	tsec += sec;

	struct tm time = {0};
	localtime_r(&tsec, &time);

	print_tm(&time);
}

void ShowTimeStamp_Min(int min)
{
	time_t tsec = GeTimeStampBase();
	tsec += (min * 60);

	struct tm time = {0};
	localtime_r(&tsec, &time);

	print_tm(&time);
}
