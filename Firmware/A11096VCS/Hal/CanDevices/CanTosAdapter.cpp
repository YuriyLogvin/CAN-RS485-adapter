/*
 * CanTosAdapter.cpp
 *
 *  Created on: 2 лют. 2020 р.
 *      Author: banz
 */

#include <CanTosAdapter.h>

CanTosAdapter::CanTosAdapter()
	: CanDevice(0,0,0,CanMode::Standart)
{
	_MotorRpm = 111;
	_CurrentDc = 1201; //120.1V
	_CurrentRms = 0;
	_ControllerTemperature = 0;
	_MotorTemperature = 0;
	_Voltage = 3201; //320.1 V
	_Voltage12V = 0;
	_OvercurrentBit = false;
	_OperationalMode = 0;
	_GearMode = 0;

}

CanTosAdapter::~CanTosAdapter()
{
}

bool CanTosAdapter::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (rxHeader.StdId == 0x135)
	{
		_CurrentDc = (data[1] << 8 | data[0]);
		_Voltage = (data[3] << 8 | data[2]);
		_Voltage12V = data[4];
		_OvercurrentBit = (data[5] & 0x1) != 0;
		_CurrentRms = (data[7] << 8 | data[6]);
		return true;
	}

	if (rxHeader.StdId == 0x136)
	{
		_OperationalMode = data[0] & 0xf;
		_GearMode = data[1] & 0xf;
		_MotorRpm = (data[3] << 8 | data[2]);
		_ControllerTemperature = data[6];
		_MotorTemperature = data[7];
		return true;
	}

	return false;

}

int16_t CanTosAdapter::Rpm()
{
	return _MotorRpm;
}
int16_t CanTosAdapter::Current()
{
	return _CurrentDc;
}

int16_t CanTosAdapter::Voltage()
{
	return _Voltage;
}

int16_t CanTosAdapter::TempConstroller()
{
	return (int16_t)_ControllerTemperature * 10;
}

int16_t CanTosAdapter::TempMotor()
{
	return (int16_t)_MotorTemperature * 10;
}


