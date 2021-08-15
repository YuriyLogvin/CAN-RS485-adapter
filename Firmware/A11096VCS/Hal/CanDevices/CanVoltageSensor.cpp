/*
 * CanVoltageSensor.cpp
 *
 *  Created on: 23 вер. 2020 р.
 *      Author: banz
 */

#include <CanVoltageSensor.h>
#include <sys/_stdint.h>
#include "Hal.h"

CanVoltageSensor::CanVoltageSensor(uint32_t destNodeId)
	: CanDevice(0,destNodeId,0,CanMode::Standart) {
	_RequestsTicks = 0;
	_Voltage = 0;
}

CanVoltageSensor::~CanVoltageSensor() {
}

void CanVoltageSensor::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 50)
		return;

	if (!_Enable)
		return;

	_TxData[0] = 0;
	_TxData[1] = (uint8_t)_Voltage;
	_TxData[2] = (uint16_t)_Voltage >> 8;
	_TxData[3] = 0;
	_TxData[4] = 0;
	_TxData[5] = 0;
	_TxData[6] = 0;
	_TxData[7] = 0;

	if (CanDevice::Transmit())
		_RequestsTicks = Hal::GetTickCount();;



}

bool CanVoltageSensor::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	/*if (rxHeader.StdId != 11)
	{
		return true;
	}*/

	return false;
}


void CanVoltageSensor::SetVoltage(int16_t v)
{
	_Voltage = v;
}

