#pragma once

#include "Type.h"

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	bool isGw;
	uint LoadAddr;
	uint Version;
	uint Size;
	uint Crc32;
}UpdateBinInfo_t;

#pragma pack(pop)

// �����߼��� ��bin���浽�ض�λ�á�
// ������ BootLoadConfig ������Ϣ��Ȼ��������
// �� BootLoad �� bin �ŵ� LoadAddr��������������


// ������
// bin �ĳ����Ѿ��� info �����С�
// saveAddr  bin ����� Flash ��ַ
// һ���� ���֮���޸�һЩϵͳ���ã�Ȼ��������
bool DoUpdate(UpdateBinInfo_t* info, uint saveAddr, byte* bin);

