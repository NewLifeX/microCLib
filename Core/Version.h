#pragma once

#include "Type.h"
#include "time.h"

// �汾��ʹ�÷���/����ʱ�� ��ȥ 2000-1-1 00:00:00 �õ��ķ�������

// ���Ը��ⲿʹ�ã���ӡ��������ʱ�䡣
extern const char* FirmwareBuildTime;

// ���Ը��ⲿʹ�ã���ӡ��������ʱ�䡣
extern const char* PublishesTime;

// �� yyyy-MM-dd HH:mm:ss ��ʽ��ʾʱ��
void structtmShow(struct tm* time);

// ��ȡ����ʱ�䣬�� ASCII C ֧�ֵ� __DATE__  __TIME__
bool GetBuildTime(struct tm* time);

// ��ȡ����ʱ�䡣 ����ʱ�������С���߻�á�
// С���߹��ܣ� �滻bin�ļ��е� yyyy-MM-dd HH:mm:ss
bool GetPublishesTime(struct tm* time);

// ��ȡ�汾�š�
// uint ���ͱ�ʾ���ӿ���֧�� 8171 �ꡣ �㹻��
uint GetVersion(void);

/*
struct tm	����
tm_year		// ��ֵ����ʵ����ݼ�ȥ1900
tm_mon,		// �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
tm_mday		// һ�����е����� - ȡֵ����Ϊ[1,31] 
tm_hour		// ʱ - ȡֵ����Ϊ[0,23]
tm_min		// �� - ȡֵ����Ϊ[0,59]
tm_sec		// �� - ȡֵ����Ϊ[0,59]
*/

/// <summary>��ȡ����汾</summary>
/// <param name="addr">�̼���ʼ��ַ</param>
/// <param name="len">�̼���С</param>
/// <returns></returns>
uint GetFwVersion(uint addr, int len);

