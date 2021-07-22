#include "Debug.h"

#ifdef DEBUG

// 全局调试等级
static volatile DebugLevel  PubLevel = DebugOff;

// 最多4种方式进行。
static FPutcFunc Fputs[4] = {NULL};

// 注册
bool FputcRegister(FPutcFunc func)
{
	if (func == NULL)return false;

	for (int i = 0; i < sizeof(Fputs)/sizeof(Fputs[0]); i++)
	{
		if (Fputs[i] == NULL)
		{
			Fputs[i] = func;
			return true;
		}
	}

	return false;
}

// 取消注册
bool FputcUnRegister(FPutcFunc func)
{
	if (func == NULL)return true;

	for (int i = 0; i < sizeof(Fputs) / sizeof(Fputs[0]); i++)
	{
		if (Fputs[i] == func)
		{
			Fputs[i] = NULL;
			return true;
		}
	}

	return true;
}

// 逐字节输出。 printf 的最终输出。
int fputc(int ch, FILE* f)
{
	bool out = false;

	for (int i = 0; i < sizeof(Fputs) / sizeof(Fputs[0]); i++)
	{
		if (Fputs[i] != NULL)
		{
			Fputs[i](ch,f); 
			out = true;
		}
	}

	if (out)return ch;

	// 兼容以前的方式。 避免底层修改影响上层。
	return ComFputc(ch, f);
}


static ComName DebugPort = COM_NONE;
int ComFputc(int ch, FILE* f)
{
	// 此处只判空   判断是否合法失败也输出不出去 无意义
	if (DebugPort == COM_NONE)return -1;
	UartPutChar(DebugPort, ch, true);

	return	ch;
}

#ifdef TRACE

// 堆栈跟踪
static struct
{
	const char* Table[16];
	int Index;
}TraceStack;

#endif

void SetDebugPort(ComName port)
{
	// assert_param(IS_USART_ALL_PERIPH(port));
	DebugPort = port;
	PubLevel = DebugErr;

#ifdef TRACE
	TraceStack.Index = 0;
	for (int i = 0; i < sizeof(TraceStack.Table) / sizeof(char*); i++)
	{
		TraceStack.Table[i] = NULL;
	}
#endif

}

void SetDebugLevel(DebugLevel level)
{
	// 只有一条汇编 （传送指令） 无需上锁
	PubLevel = level;
}

DebugLevel GetDebugLevel(void)
{
	return PubLevel;
}

bool GetDebugStat(void)
{
	return PubLevel != DebugOff;
}

#ifdef TRACE
void Trace(const char* str)
{
	if (TraceStack.Index <= (sizeof(TraceStack.Table) / sizeof(char*) - 1))
	{
		TraceStack.Table[TraceStack.Index] = str;
		TraceStack.Index++;
	}
	else
	{
		printf("TraceStackTable Full\r\n");
		TraceShow();
	}
}

void UnTrace()
{
	if (TraceStack.Index > 0)
	{
		TraceStack.Index--;
		TraceStack.Table[TraceStack.Index] = NULL;
	}
	else
	{
		printf("Error! UnTrace too much\r\n");
	}
}

inline void TraceShow()
{
	if (TraceStack.Index == 0)printf("TraceStack empty\r\n");
	for (int i = 0; i < TraceStack.Index; i++)
	{
		if (TraceStack.Table[i] != NULL)
			printf("%s\r\n", TraceStack.Table[i]);
	}
}
#else
void Trace(const char* str) {}
void UnTrace(void) {}
void TraceShow(void) {}
#endif

#else

void SetDebugPort(ComName port) {}
void SetDebugLevel(DebugLevel level) {}
DebugLevel GetDebugLevel(void) { return DebugOff; }
bool GetDebugStat(void) { return false; }

void Trace(const char* str) {}
void UnTrace(void) {}
void TraceShow(void) {}

#endif
