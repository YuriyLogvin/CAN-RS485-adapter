/*
 * CanCurrentSensor.cpp
 *
 *  Created on: 26 черв. 2020 р.
 *      Author: banz
 */

#include <CanCurrentSensor.h>
#include <sys/_stdint.h>
#include "Hal.h"

CanCurrentSensor::CanCurrentSensor(uint32_t destNodeId)
	: CanDevice(0,destNodeId,0,CanMode::Standart) {
	_RequestsTicks = 0;
	_Current = 0;
	_Temp = 0;
}

CanCurrentSensor::~CanCurrentSensor() {
	// TODO Auto-generated destructor stub
}

void CanCurrentSensor::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 50)
		return;

	_TxData[0] = 0;
	_TxData[1] = (uint8_t)_Current;
	_TxData[2] = (uint16_t)_Current >> 8;
	_TxData[3] = (uint8_t)_Temp;
	_TxData[4] = (uint16_t)_Temp >> 8;
	_TxData[5] = 0;
	_TxData[6] = 0;
	_TxData[7] = 0;

	CanDevice::Transmit();

	_RequestsTicks = Hal::GetTickCount();

}

bool CanCurrentSensor::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	/*if (rxHeader.StdId != 11)
	{
		return true;
	}*/

	return true;
}


void CanCurrentSensor::SetCurrent(int16_t curr)
{
	_Current = curr;
}

void CanCurrentSensor::SetTemp(int16_t temp)
{
	_Temp = temp;
}
