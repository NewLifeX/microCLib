

#ifndef __Convert_H__
#define __Convert_H__

#include "Type.h"

#include "stdio.h"

// 请直接使用 sscanf sprintf
/*
	sscan 教程：

	int sscanf(char* str, char* format, [argument]... );

	返回值：
	如果成功，该函数返回成功匹配和赋值的个数。如果到达文件末尾或发生读错误，则返回 EOF。
	#define EOF      (-1)

	参数:
	str		: 需要操作的输入内存地址。
	format	: 字符串描述的匹配格式。 
		%[*][width][modifiers]type

		*			表示过滤掉
		width		长度
		modifiers	参数的 size  =>  {h | I | I64 | L}
					h		表示单字节size
					I		表示2字节 size,
					L		表示4字节size(double例外),
					I64		表示8字节size
		type		匹配格式  c  d  o   s  u  x  X ....


	常见形式
		%c				取一个字符
		%d				取10进制
		%e %E %f %g %G	取浮点数
		%o				取8进制
		%s				取子字符串
		%u				无符号十进制
		%x %X			取16进制

		%[a-z]			表示匹配a到z中任意字符，贪婪性(尽可能多的匹配)
		%[aB']			匹配a、B、'中一员，贪婪性
		%[^a]			匹配非a的任意字符，并且停止读入，贪婪性

		%f				单精度
		%lf				双精度


	例子：
		sscanf("Saturday March 25 1989", "%s %s %d  %d", weekday, month, &day, &year );

		sscanf("123456 ", "%4s", buf);
			取4字节 buff = "1234"

		sscanf("123456 abcdedf", "%[^ ]", buf);
			取到指定字符为止的字符串。 buff = "123456"

		sscanf("123456abcdedfBCDEF", "%[1-9a-z]", buf);
			取仅包含指定字符集的字符串。取仅包含1到9和小写字母的字符串。 buff = "123456abcdedf"

		sscanf("123456abcdedfBCDEF", "%[^A-Z]", buf);
			 取到指定字符集为止的字符串。取遇到大写字母为止的字符串。buff = "123456abcdedf"

		sscanf("ios<android>wp7", "%*[^<]<%[^>]", buf1);
			buf1=android

		sscanf("0123abcABC", "%[0-9]%[a-z]%[A-Z]", buf1, buf2, buf3);
			buf1= "0123" buf2="abc" buf3="ABC"
		
		sscanf("iosVSandroid", "%[a-z]VS%[a-z]", buf1, buf2);
			buf1="ios" buf2="android"

		sscanf("Email:beijing@sina.com.cn", "%[^:]:%[^@]@%[^.].%s", buf1, buf2, buf3, buf4);
			buf1=Email, buf2=beijing, buf3=sina, buf4=com.cn

		int year, month, day, hour, minute, second;
		sscanf("2013/02/13 14:55:34","%d/%d/%d %d:%d:%d",&year, &month, &day, &hour, &minute, &second);
		printf("time=%d-%d-%d %d:%d:%d\n", year, month, day, hour, minute, second);
			time=2013-2-13 14:55:34

		P.S.	%*s表示第一个匹配到的%s被过滤掉。

		sscanf("hello,  world", "%*s%s", buf);
			仅保留"world"。 buff = "world"

		sscanf("iios/12DDWDFF@122", "%*[^/]/%[^@]", buf);
			获取 / 和 @ 之间的字符串，先将 "iios/"过滤掉，再将非'@'的一串内容送到buf中。 buff = "12DDWDFF"
*/

/*
	int sprintf(char *string, char *format [,argument,...]);

	返回值：
		如果成功，则返回写入的字符总数，不包括字符串追加在字符串末尾的空字符。如果失败，则返回一个负数。
*/

#define CONVNUM "0,1,2,3,4,5,6,7,8,9"

#endif
