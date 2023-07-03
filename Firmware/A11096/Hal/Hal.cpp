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

Stm32UsartDmaIt* Hal::UsartExt;

#define _ReferenceVoltage 3265
#define _InputDivider 11
#define _AdcBits 12


void Hal::Init(bool canSilentMode)
{
	UsartExt = new Stm32UsartDmaIt(&huart1, 0x200, 0x100, RS485_EN_GPIO_Port, RS485_EN_Pin);

	auto canSpeed = CanDevice::Speeds::s125k;

#if (MODE == MODE_EXT_BMS)
	canSpeed = CanDevice::Speeds::s500k;
#else
#if (MODE == MODE_SNIFFER)
	canSpeed = CanDevice::Speeds::s125k;
#else
#if (MODE == MODE_CURTIS_SDO)
	canSpeed = CanDevice::Speeds::s250k;
#else
#if (MODE == MODE_KELLY)
	canSpeed = CanDevice::Speeds::s250k;
#else
#if (MODE == MODE_CHILLER)
	canSpeed = CanDevice::Speeds::s250k;
#else
#if (MODE == MODE_EMULATOR)
	canSpeed = CanDevice::Speeds::s250k;
#endif //MODE == MODE_EMULATOR
#endif //MODE == MODE_CHILLER
#endif //MODE == MODE_KELLY
#endif //MODE == MODE_CURTIS_SDO
#endif //MODE == MODE_SNIFFER
#endif //MODE == MODE_EXT_BMS
	CanDevice::Init(&hcan, canSpeed, canSilentMode);

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

bool Hal::GetSwValue(uint8_t ind)
{
	switch (ind)
	{
	case 1:
		return HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) != GPIO_PIN_SET;
	case 2:
		return HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) != GPIO_PIN_SET;
	case 3:
		return HAL_GPIO_ReadPin(SW3_GPIO_Port, SW2_Pin) != GPIO_PIN_SET;
	}

	return false;
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

