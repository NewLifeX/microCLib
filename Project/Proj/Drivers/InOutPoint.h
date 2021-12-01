#pragma once

#include "Type.h"
#include "HC165.h"
#include "HC595.h"

// ���������״̬
typedef struct
{
	HC595_t HC595;
	HC165_t HC165;

	byte Out[2];
	byte In[1];
}InOutInfo_t;
// ���⹫��������ֱ�ӷ��� Out In ��չ��������
extern InOutInfo_t InOutInfo;

typedef enum
{
	/// <summary>ǰ��</summary>
	Door = 0,
}InPointName_e;

typedef enum
{
	/// <summary>ǰ��</summary>
	Door_Out = 0,
}OutPointName_e;

/// <summary>��ʼ�����������</summary>
void InOutPointInit(void);

/// <summary>��������</summary>
/// <param name="idx">�±�[0,7]</param>
/// <param name="value">ֵ����©�����©Ϊtrue </param>
/// <param name="flashOut">ˢ�����</param>
void SetOutPoint(OutPointName_e idx, bool value, bool flashOut);

/// <summary>�����״̬</summary>
/// <param name="idx"></param>
/// <returns></returns>
bool GetOutPoint(OutPointName_e idx);

/// <summary>����IO�����Ч</summary>
/// <param name="flashOut">ˢ�����</param>
void OutPointClear(bool flashOut);

/// <summary>ˢ�����</summary>
void OutPointFlash(void);

/// <summary>�������</summary>
/// <param name="idx">�±�[0,7]</param>
/// <returns>ֵ���ӵط���true </returns>
bool ReadInPoint(InPointName_e idx);

/// <summary>ˢ������״̬</summary>
void InPointFlash(void);


/**************************************************/


/// <summary>״ָ̬ʾ�����</summary>
/// <param name="stat">8����һ��</param>
/// <param name="flashOut">�Ƿ�����ˢ��</param>
void OutLedsStat(byte stat, bool flashOut);


