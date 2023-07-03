/*
 * CanKellyAdapter.cpp
 *
 *  Created on: 1 черв. 2020 р.
 *      Author: banz
 */

#include <CanKellyAdapter.h>
#include "SwitchesValues.h"

CanKellyAdapter::CanKellyAdapter()
		: CanDevice(0,0,0,CanMode::Extended)
{
	// TODO Auto-generated constructor stub
	_MotorRpm = 0;
	_CurrentRms = 0;
	_ControllerTemperature = 0;
	_MotorTemperature = 0;
	_ErrorCode = 0;
	_Switches = 0;
}

bool CanKellyAdapter::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (rxHeader.ExtId == 0x0CF11E05)
	{ // Message 1
		_MotorRpm = (data[1] << 8 | data[0]);
		_CurrentRms = (data[3] << 8 | data[2]);
		_ErrorCode = (data[7] << 8 | data[6]);
		return true;
	}

	if (rxHeader.ExtId == 0x0CF11F05)
	{
		_ControllerTemperature = (int16_t)-40 + data[1];
		_MotorTemperature =  (int16_t)-30 + data[2];
		_Switches = 0;
		/*_Switches |= (data[4] & 0x01) ? (uint8_t)SwitchesValues::Forward : 0;
		_Switches |= (data[4] & 0x02) ? (uint8_t)SwitchesValues::Backward : 0;
		_Switches |= (data[5] & 0x08) ? (uint8_t)SwitchesValues::Break : 0;*/
		_Switches |= (data[5] & 0x08) ? (uint8_t)SwitchesValues::Break : 0;
		_Switches |= (data[5] & 0x10) ? (uint8_t)SwitchesValues::Backward : 0;
		_Switches |= (data[5] & 0x20) ? (uint8_t)SwitchesValues::Forward : 0;
		return true;
	}

	return false;
}

int16_t CanKellyAdapter::Rpm()
{
	return _MotorRpm;
}

int16_t CanKellyAdapter::Current()
{
	return _CurrentRms;
}

int16_t CanKellyAdapter::TempConstroller()
{
	return _ControllerTemperature * 10;
}

int16_t CanKellyAdapter::TempMotor()
{
	return _MotorTemperature * 10;
}

uint16_t CanKellyAdapter::Switches()
{
	return _Switches;
}

