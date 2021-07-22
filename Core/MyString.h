#pragma once

#include "Type.h"

// ����λ��
int StringIndexOf(char* src, char* sub);
int StringLastIndexOf(char* src, char* sub);
// �����ַ�λ�ã����� chars �е�һ���ͷ�������ֵ��chlen �� chars ������
int StringIndexOfChars(char* src, char* chars, int chlen);

// ͳ�Ƶ� \r \n ������
// ���û����Щ�������ַ���ȫ����
int StringLineLen(char* str);

// ���� Trim �ĳ��ȡ�
// Trim ͳ��ͷ�� \r \n \0 �ַ��ĸ�����
// ��Ϊ \0 ���ַ��Ľ�β�� �˴������ô�ֵ��Ϊ���ݣ���
int StringTrim(char* str,int len);

// ������ʵ���г��ȡ�
// �� src û�л��з���ʱ��,����ȫ��;
// ���г����ǰ����������ݺͺ���Ļ��з��ģ�\r \n \0 ���㡣
// ���г����������ǲ������еġ�
// ���� dst ΪNULL����ʾ������
// ��������ȷ���� -1;
// �� dstlen ���Ȳ���ʱ��		���� -2;
int StringCutLine(char* src, char* dst, int dstlen);

// ���ַ������ҡ��������ȳ��ֵ����ַ���������
int StringFindStrs(char* src, char** subs, int subslen);

// src �� chars �е���һ����chlen �� chars ������
int CharIsChars(char src, char* chars, int chlen);
