#pragma once

#include "Type.h"
#include "BootLoadConfig.h"

// dst �����ǵ�һ chip�� 
// �����Ƿ�ɹ���
bool CopyBin(uint dst, uint src, int size);

// �����APP �Ƿ�������������
bool CheckNewApp(BootLoadConfig_t* cfg);

// ����������
bool CheckAddUpdate(BootLoadConfig_t* cfg);

// BootLoad ��Ҫ�߼���
void BootLoadMain(void);

/*  BootLoad ����ʵ����

// ����Main����ֻ��Ҫ����һ��������Ϣ Ȼ����� BootLoadMain() ����
int main(void)
{
	DebugInit();
	// NVIC_SetPriorityGroup(4);

	BootLoadMain();
}

*/

// APP ���õ��ĺ�����

// ���� BootLoad ������Ϣ��
bool SaveUpdateConfig(uint appSaveAddr, uint appLoadAddr, int appSize, uint64 version);
