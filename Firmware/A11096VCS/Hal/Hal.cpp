/*
 * Hal.cpp
 *
 *  Created on: Feb 25, 2017
 *      Author: banz
 */

#include "Hal.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "WiFiDataTransmitter.h"
//#include "Log.h"
#include "main.h"
#include "CanDevices/CanSniffer.h"

extern "C"
{
UART_HandleTypeDef huart1;

CAN_HandleTypeDef hcan;

IWDG_HandleTypeDef hiwdg;

//TIM_HandleTypeDef htim2;
uint32_t HAL_GetTick(void);
//void HAL_IncTick(void);
}

bool _UseSniffer = false;


/*void HAL_IncTick(void)
{
	_uwTickSub++;
	if (_uwTickSub > 9)
	{
		_uwTick++;
		_uwTickSub = 0;
	}

	_HalTicker++;
}*/

/*uint32_t HAL_GetTick(void)
{
	return _uwTick;
}*/

Stm32UsartDma* Hal::UsartExt;

#define _ReferenceVoltage 3265
#define _InputDivider 11
#define _AdcBits 12


void Hal::Init()
{
	UsartExt = new Stm32UsartDma(&huart1, 0x200, 0x100, RS485_EN_GPIO_Port, RS485_EN_Pin);

	auto canSpeed = CanDevice::Speeds::s250k;

	CanDevice::Init(&hcan, canSpeed, false);

}

void Hal::Tick()
{
	CanDevice::Tick();
}

bool Hal::LedBlue()
{
	return (HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET);
}

void Hal::LedBlue(bool value)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, value?GPIO_PIN_SET:GPIO_PIN_RESET);
}

short Hal::GetTicksInSecond()
{
	return 1000;
}

short Hal::GetTicksInMilliSecond()
{
	return 1;
}

int32_t Hal::GetTickCount()
{
	return HAL_GetTick();
}

int32_t Hal::GetSpendTicks(int32_t fromTicks)
{
	int32_t nowT = GetTickCount();
	if (nowT >= fromTicks)
		return nowT - fromTicks;

	return (0xffffffff - fromTicks + nowT);
	//return embedded_get_spent_ms(fromTicks);
}

void Hal::Sleep(uint16_t sleepMs)
{
	int32_t t = Hal::GetTickCount();
	while (Hal::GetSpendTicks(t) < Hal::GetTicksInMilliSecond() * sleepMs);
}

void Hal::UpdateWdt()
{
	HAL_IWDG_Refresh(&hiwdg);
}

