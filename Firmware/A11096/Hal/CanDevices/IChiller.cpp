/*
 * IChiller.cpp
 *
 *  Created on: 25 лист. 2021 р.
 *      Author: banz
 */

#include <IChiller.h>

IChiller::IChiller() {
	_ChillerEnable = false;
	_TempHaveMin = 0;
    _TempHaveMax = 0;
    _TempTarget = 0;
    _VoltageHave = 0;
    _ControlModeNeed = ControlModes::Manual;
    _ChillerModeNeed = ChillerModes::Waiting;
    _BatteryModeNeed = BatteryModes::Discharging;
    _HvModeNeed = HvModes::Discharge;
    _ControlModeHave = ControlModes::Manual;
    _ChillerModeHave = ChillerModes::Waiting;
    _BatteryModeHave = BatteryModes::Discharging;
    _HvModeHave = HvModes::Discharge;
    _ErrorCode = 44;
    _TempOut = 11;
    _TempIn = 22;
    _COP = 33;
}

void IChiller::TurnOn(bool value)
{
	_ChillerEnable = value;
}

void IChiller::TempHaveMinMaxp(int16_t min, int16_t max)
{
	_TempHaveMin = min;
    _TempHaveMax = max;
}

void IChiller::TempTarget(int16_t value)
{
    _TempTarget = value;
}

void IChiller::VoltageHave(int16_t value)
{
	_VoltageHave = value;
}

void  IChiller::ControlMode(ControlModes value)
{
	_ControlModeNeed = value;
}

void  IChiller::ChillerMode(ChillerModes value)
{
	_ChillerModeNeed = value;
}

void  IChiller::BatteryMode(BatteryModes value)
{
	_BatteryModeNeed = value;
}

void IChiller::HvMode(HvModes value)
{
	_HvModeNeed = value;
}


IChiller::ControlModes IChiller::ControlMode()
{
	return _ControlModeHave;
}

IChiller::ChillerModes IChiller::ChillerMode()
{
	return _ChillerModeHave;
}

IChiller::BatteryModes IChiller::BatteryMode()
{
	return _BatteryModeHave;
}

IChiller::HvModes IChiller::HvMode()
{
	return _HvModeHave;
}

int16_t IChiller::TempIn()
{
	return _TempIn;
}

int16_t IChiller::TempOut()
{
	return _TempOut;
}

uint8_t IChiller::COP()
{
	return _COP;
}

uint32_t IChiller::ErrorCode()
{
	return _ErrorCode;
}
