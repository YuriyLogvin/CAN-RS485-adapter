/*
 * ChargerSaeJ1939.cpp
 *
 *  Created on: 22 лип. 2020 р.
 *      Author: banz
 */

#include <CanDevices/ChargerSaeJ1939.h>
#include "Hal.h"

ChargerSaeJ1939::ChargerSaeJ1939()
	: CanDevice(0,0,1000,CanMode::Extended),
	ICharger()
{
}


ChargerSaeJ1939::~ChargerSaeJ1939() {
	// TODO Auto-generated destructor stub
}

void ChargerSaeJ1939::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 500)
		return;

	_TxData[0] = (uint16_t)_VoltageNeed >> 8;
	_TxData[1] = (uint8_t)_VoltageNeed;
	_TxData[2] = (uint16_t)_CurrentNeed >> 8;
	_TxData[3] = (uint8_t)_CurrentNeed;
	_TxData[4] = _ChargerEnable ? 0x0 : 0x1;
	_TxData[5] = 0;
	_TxData[6] = 0;
	_TxData[7] = 0;

	_IdDest = 0x1806E5F4;
	if (CanDevice::Transmit())
		_RequestsTicks = Hal::GetTickCount();

}

bool ChargerSaeJ1939::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (rxHeader.ExtId != 0x18FF50E5)
		return false;

	_VoltageHave = (uint16_t)(data[0]) << 8 | data[1];
	_CurrentHave = (uint16_t)(data[2]) << 8 | data[3];
	_ChargerFlags = data[4];

	return true;
}

bool ChargerSaeJ1939::IsOnline()
{
	return CanDevice::IsOnline();
}
