#pragma once

#include "Type.h"

// 来源于阿里IOT代码  base64.h

// 输出缓冲区不够
#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL               (-0x002A)
// 输入中的字符无效
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER              (-0x002C)


// dst  目标指针
// dlen 目标缓冲大小
// olen 实际使用长度
// src  源指针
// slen 源长度

// 编码
// BASE64 Encode 结果的长度计算:  (srclen / 3 向上取整) * 4
// int mbedtls_base64_encode(unsigned char* dst, int dlen, int* olen, const unsigned char* src, int slen);
int Base64Encode(byte* dst, int dlen, int* olen, byte* src, int slen);

// 解编码
// int mbedtls_base64_decode(unsigned char* dst, int dlen, int* olen, const unsigned char* src, int slen);
int Base64Decode(byte* dst, int dlen, int* olen, byte* src, int slen);
