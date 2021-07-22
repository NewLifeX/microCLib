#pragma once

#include "Type.h"

// ��Դ�ڰ���IOT����  base64.h

// �������������
#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL               (-0x002A)
// �����е��ַ���Ч
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER              (-0x002C)


// dst  Ŀ��ָ��
// dlen Ŀ�껺���С
// olen ʵ��ʹ�ó���
// src  Դָ��
// slen Դ����

// ����
// BASE64 Encode ����ĳ��ȼ���:  (srclen / 3 ����ȡ��) * 4
// int mbedtls_base64_encode(unsigned char* dst, int dlen, int* olen, const unsigned char* src, int slen);
int Base64Encode(byte* dst, int dlen, int* olen, byte* src, int slen);

// �����
// int mbedtls_base64_decode(unsigned char* dst, int dlen, int* olen, const unsigned char* src, int slen);
int Base64Decode(byte* dst, int dlen, int* olen, byte* src, int slen);
