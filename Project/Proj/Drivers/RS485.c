
#include "Gpio.h"
#include "Rs485.h"
#include "CircularQueue.h"
#include "SysInfo.h"
#include "SysTime.h"
#include "Debug.h"

static void UartIrq(UartNotify mod, ushort data, void* param)
{
	uint now = GetCurrentTimeMs();

	// ������
	if (mod == UART_NOTIFY_RX)
	{
		CircularQueueWrite(&SysInfo.RxQueue, data);
	}
}

void Rs485Init(void)
{
	// ��ʼ�����ջ�����
	int rxsize = 96;
	byte* p = (byte*)GlobleMalloc(rxsize);
	if (p == NULL)
	{
		WarningPrintf("p=NULL\r\n");
		return;
	}
	CircularQueueInit(&SysInfo.RxQueue, p, rxsize);

	// ��ʼ��RS485���ͻ�������
	InitAsyncData2(&SysInfo.TxAsync, 128);

	// ��ʼ��COM��
	UsartCfg_t* cfg = UsartCfgCreateOrGet(SysInfo.Rs485Port);
	cfg->Baudrate = 9600;
	cfg->Mode = RX_TX;
	UartInit(SysInfo.Rs485Port);
	// UartSetInterrupt(SysInfo.Rs232Port,UART_IT_FLAG_TX | UART_IT_FLAG_RX, 0, 0, UartIrq, (void*)SysInfo.Rs232Port);
	UartSetInterrupt(SysInfo.Rs485Port, UART_IT_FLAG_RX, 0, 0, UartIrq, (void*)SysInfo.Rs485Port);

	GpioInit(SysInfo.Rs485Ctrl, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
}

void Rs485Send(byte* data, int len)
{
	GpioWrite(SysInfo.Rs485Ctrl, true);
	for (volatile int i = 0; i < 50; i++) {}

	for (int i = 0; i < len; i++)
	{
		UartPutChar(SysInfo.Rs485Port, data[i], true);
	}

	for (volatile int i = 0; i < 500; i++) {}
	GpioWrite(SysInfo.Rs485Ctrl, false);
}

void Rs485Send2(Buffer_t* buff)
{
	Rs485Send(buff->p, buff->len);
}


/// <summary>�첽�������ݡ�����RS485�ǰ�˫������������ֱ��ʹ��ͬ������</summary>
/// <param name=""></param>
bool AsyncSendData(void)
{
	uint now = GetCurrentTimeMs();

	AsyncData_t* adt = &SysInfo.TxAsync;
	// ��û�е����첽ʱ�䣬���ء� ����ʱ����ִ�С�
	if (adt->AsyncTime > now)return false;

	byte* tx;
	int txlen = GetAsyncData(adt, &tx);
	if (txlen < 1)return false;

	Rs485Send(tx, txlen);
	ShowArray("AsyncSend ", tx, txlen, txlen);
	RemoveAsyncData(adt);

	// DebugPrintf("AsyncData Delay %d\r\n", now - adt->AsyncTime);

	return true;
}

