/*
 * IChargerController.cpp
 *
 *  Created on: 21 вер. 2020 р.
 *      Author: banz
 */

#include <CanDevices/ICharger.h>

ICharger::ICharger()
{
	_VoltageNeed = 0;
	_CurrentNeed = 0;
	_VoltageHave = 0;
	_CurrentHave = 0;
	_ChargerFlags = 0;
	_VoltageOver = 0;
	_ChargerEnable = false;
	_RequestsTicks = 0;
}

void ICharger::SetVoltage(uint16_t val)
{
	_VoltageNeed = val;
}

uint16_t ICharger::GetVoltage()
{
	return _VoltageHave;
}

void ICharger::SetOverVoltage(uint16_t val)
{
	_VoltageOver = val;
}

void ICharger::SetCurrent(uint16_t val)
{
	_CurrentNeed = val;
}

uint16_t ICharger::GetCurrent()
{
	return _CurrentHave;
}

uint8_t ICharger::GetChargerFlags()
{
	return _ChargerFlags;
}

void ICharger::TurnCharger(bool value)
{
	_ChargerEnable = value;
}
