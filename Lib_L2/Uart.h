
#ifndef __UART_H__
#define __UART_H__

#include "Pin.h"
#include "Type.h"
#include "Include.h"
// #include "Dma.h"

// 串口中断类型，用于注册中断（可以 | 操作）
#define UART_IT_FLAG_TX                      ((uint16_t)0x0001)
#define UART_IT_FLAG_RX                      ((uint16_t)0x0002)
#define UART_IT_FLAG_CTS                     ((uint16_t)0x0004)
#define UART_IT_FLAG_PE                      ((uint16_t)0x0008)

// 串口中断的类型，用户串口中断回调
typedef enum
{
	UART_NOTIFY_TX = 0,		// 发送完字节
	UART_NOTIFY_RX,			// 收到字节
	UART_NOTIFY_CTS,		// 异步信号
	UART_NOTIFY_PE			// 奇偶校验错误
}UartNotify;

typedef void (*UartIrqHandler)(UartNotify, uint16_t, void*);

typedef enum
{
	TX_ONLY = 0,
	RX_ONLY,
	RX_TX
}UartMode;

typedef enum
{
	UART_1_STOP_BIT = 0,
	UART_0_5_STOP_BIT,
	UART_2_STOP_BIT,
	UART_1_5_STOP_BIT
}StopBits;

// 奇偶校验
typedef enum
{
	NO_PARITY = 0,
	EVEN_PARITY,
	ODD_PARITY
}Parity;

typedef struct
{
	// ComName		Port;	// 默认值
	// 引脚不使用remap  如果使用请外部单独调用相关remap函数
	PinName 	Tx;			// P0
	PinName 	Rx;			// P0
	UartMode 	Mode;		// TX_RX

	uint	 	Baudrate;	// 115200

	StopBits 	Bits;		// 1
	Parity 		_Parity;	// none
}UsartCfg_t;

// 串口名称数组  ComName 作为下标即可。
// extern USART_TypeDef* const ComArray[];

UsartCfg_t* UsartCfgCreateOrGet(ComName com);

void UsartCfgDelete(ComName com);

void UartInit(ComName com);

void UartDeInit(ComName com);

uint16_t UartGetChar(ComName com);

void UartPutChar(ComName com, uint16_t data, bool delay);

void UartSetInterrupt(ComName com, uint16_t ITFlag, uint8_t prePriority,
	uint8_t subPriority, UartIrqHandler irqHandler, void* param);

void UartITConfig(ComName com, uint16_t ITFlag, bool enable);

void UartRemoveInterrupt(ComName com);

// 两个强行改状态的函数，尽量避免使用。使用后需要重新初始化才能根据UsartCfg_t配置引脚。
void UartSetRts(ComName com, bool value);

void UartSetCts(ComName com, bool value);

// 使用方式
/*
*	UsartCfg_t* cfg =  UsartCfgCreateOrGet(COM1);
*	cfg->Tx = PA9;		// 可省略，仅默认引脚可省略
*	cfg->Rx = PA10;		// 可省略，仅默认引脚可省略
*	cfg->Baudrate = 9600;
*	UartInit(COM1);		// 内部对引脚只做了判空，没有校验是否是相应COM口的引脚
*	UartSetInterrupt( COM1, UART_IT_FLAG_TX | UART_IT_FLAG_RX, 0, 0, UartTransparentRxTx );
*/

#endif  // __UART_H__
