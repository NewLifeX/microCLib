
#ifndef __GPIO_H__
#define __GPIO_H__

#include "Pin.h"
#include "Type.h"
// #include "Include.h"

// Operation Mode for the GPIO
typedef enum
{
	PIN_INPUT = 0,
	PIN_OUTPUT,
	PIN_AF,
	PIN_ANALOGIC
}PinMode;

// Define the GPIO as Push-pull type or Open Drain
typedef enum
{
	PIN_OUTPUT_PP = 0,
	PIN_OUTPUT_OD
}PinOutput;

// Add a pull-up, a pull-down or nothing on the GPIO line
typedef enum
{
	PIN_NO_PULL = 0,
	PIN_PULL_UP,
	PIN_PULL_DOWN
}PinPull;

// Define the GPIO IRQ on a rising, falling or both edges
typedef enum
{
	// 上升沿
	IRQ_RISING_EDGE = 0,
	// 下降沿
	IRQ_FALLING_EDGE,
	// 上升&下降沿
	IRQ_RISING_FALLING_EDGE
}IrqEdge;

// gpio irq callback
typedef void(*GpioIrqHandler)(void* param);

// Init gpio
void GpioInit(PinName pinName, PinMode mode, PinOutput output, PinPull pull, byte GPIO_AF, byte value);

// Write output
void GpioWrite(PinName pinName, bool value);

// Read input
bool GpioRead(PinName pinName);

// Set irq
void GpioSetInterrupt(PinName pinName, IrqEdge irqEdge, byte prePriority, 
		byte subPriority, GpioIrqHandler irqHandler,void* irqParam);

// Disable irq
void GpioDisableInterrupt(PinName pinName);


#endif // __GPIO_H__
