
#include "HardwareVersion.h"

#include "Type.h"
#include "Debug.h"

#include "time.h"
#include "stdio.h"

// Ӳ��ʱ��
const char* HardwareTime = HARDWARETIME;

// ����ʱ����ͨ���ⲿ����ֱ���滻bin�ļ��ڵ� yyyy-MM-dd HH:mm:ss �ַ�����
static bool GetTime(struct tm* time)
{
	sscanf(HardwareTime, "%d-%d-%d", &time->tm_year, &time->tm_mon, &time->tm_mday);

	// ���� struct tm ȡֵ��Χ��
	time->tm_year -= 1900;
	time->tm_mon -= 1;

	// DebugPrintf("Publishes Time : %d-%d-%d %d:%d:%d\r\n", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
	// 	time->tm_hour, time->tm_min, time->tm_sec);

	return true;
}

// ��ȡ�汾
uint GetHardwareVersion(void)
{
	struct tm start =
	{
		.tm_year = 100,		// ��ֵ����ʵ����ݼ�ȥ1900
		.tm_mon = 0,		// �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
		.tm_mday = 1,		// һ�����е����� - ȡֵ����Ϊ[1,31] 
		.tm_hour = 0,		// ʱ - ȡֵ����Ϊ[0,23]
		.tm_min = 0,		// �� - ȡֵ����Ϊ[0,59]
		.tm_sec = 0,		// �� - ȡֵ����Ϊ[0,59]
	};

	// ����ʱ�䡣
	struct tm time;
	memset(&time, 0, sizeof(time));

	// ���Ի�ȡ�����汾ʱ�����ʧ�ܻ�ȡ����ʱ���
	GetTime(&time);

	// �������/60=���Ӳ
	double min = difftime(mktime(&time), mktime(&start))/60.0;
	uint minuint = (int)min;

	return minuint;
}
