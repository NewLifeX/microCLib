#pragma once

#include "Type.h"
#include "time.h"

// 版本号使用发布/编译时间 减去 2000-1-1 00:00:00 得到的分钟数。

// 可以给外部使用，打印出来编译时间。
extern const char* FirmwareBuildTime;

// 可以给外部使用，打印出来发布时间。
extern const char* PublishesTime;

// 用 yyyy-MM-dd HH:mm:ss 格式显示时间
void structtmShow(struct tm* time);

// 获取编译时间，从 ASCII C 支持的 __DATE__  __TIME__
bool GetBuildTime(struct tm* time);

// 获取发布时间。 发布时间从另外小工具获得。
// 小工具功能， 替换bin文件中的 yyyy-MM-dd HH:mm:ss
bool GetPublishesTime(struct tm* time);

// 获取版本号。
// uint 类型表示分钟可以支持 8171 年。 足够。
uint GetVersion(void);

/*
struct tm	描述
tm_year		// 其值等于实际年份减去1900
tm_mon,		// 月份（从一月开始，0代表一月） - 取值区间为[0,11]
tm_mday		// 一个月中的日期 - 取值区间为[1,31] 
tm_hour		// 时 - 取值区间为[0,23]
tm_min		// 分 - 取值区间为[0,59]
tm_sec		// 秒 - 取值区间为[0,59]
*/

/// <summary>获取软件版本</summary>
/// <param name="addr">固件起始地址</param>
/// <param name="len">固件大小</param>
/// <returns></returns>
uint GetFwVersion(uint addr, int len);

