#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "Type.h"

typedef enum
{
	DebugOff		= 	0,
	DebugErr 		=	1,
	DebugWarning	=	2,
	DebugNormal		=	3,
} DebugLevel;

// 使用注册fputc的形式实现 console重映射。
// 仅实现输出方式。 输入复杂暂且忽略。
// typedef int (*FGetcFunc)();
typedef int(*FPutcFunc)(int ch, FILE* f);
bool FputcRegister(FPutcFunc func);
bool FputcUnRegister(FPutcFunc func);

// 原来的串口输出方式。 拉出来。 使用新的方式的时候注册进去即可。
// int ComFputc(int ch, FILE* f);

// 设置日志输出的串口
void SetDebugLevel(DebugLevel level);
DebugLevel GetDebugLevel(void);
bool GetDebugStat(void);

// 栈跟踪
void Trace(const char* str);
void UnTrace(void);
void TraceShow(void);
#define TraceCall(expr)			Trace(#expr);expr;UnTrace()

// KEIL 5.1 版本以上拥有 Arcc Clang 两套编译器 此处做出适配。
#if  !(defined ( __CC_ARM ) || defined ( __GNUC__ ))
#error "Only __CC_ARM  __GNUC__  Compiler"
#endif

#ifdef DEBUG
#if defined ( __CC_ARM )
#define DEBUG_LEVEL( level, ... )	do{\
	if(GetDebugLevel()>=level)\
	{\
		if(level == DebugErr)\
		{\
			printf("\r\nError\r\nStack:\r\n");\
			TraceShow();\
			printf("From %s %d :\r\n",##__func__,##__LINE__);\
		}\
		if(level == DebugWarning)\
		{\
			printf("\r\nWarning ");\
			printf("From %s %d :\r\n",##__func__,##__LINE__);\
		}\
		printf( ##__VA_ARGS__ );\
	}\
}while(0)

#define ErrPrintf( ... )		DEBUG_LEVEL(DebugErr,  ##__VA_ARGS__ )
#define WarningPrintf( ... )	DEBUG_LEVEL(DebugWarning,  ##__VA_ARGS__ )
#define DebugPrintf( ... )		DEBUG_LEVEL(DebugNormal,  ##__VA_ARGS__ )

#define TraceThis()				Trace(##__func__)

#endif

#if defined ( __GNUC__ )
#define DEBUG_LEVEL( level, ... )	do{\
	if(GetDebugLevel()>=level)\
	{\
		if(level == DebugErr)\
		{\
			printf("\r\nError\r\nStack:\r\n");\
			TraceShow();\
			printf("From %s %d :\r\n",__func__,__LINE__);\
		}\
		if (level == DebugWarning)\
		{\
			printf("\r\nWarning "); \
			printf("From %s %d :\r\n", __func__, __LINE__); \
		}\
		printf( __VA_ARGS__ );\
	}\
}while(0)

#define ErrPrintf( ... )		DEBUG_LEVEL(DebugErr,  		__VA_ARGS__ )
#define WarningPrintf( ... )	DEBUG_LEVEL(DebugWarning,  	__VA_ARGS__ )
#define DebugPrintf( ... )		DEBUG_LEVEL(DebugNormal,  	__VA_ARGS__ )

#define TraceThis()		Trace(__func__)

#endif

#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

#endif

