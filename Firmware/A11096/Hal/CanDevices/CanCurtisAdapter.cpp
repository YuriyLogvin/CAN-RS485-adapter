/*
 * CanCurtisAdapter.cpp
 *
 *  Created on: 5 ñ³÷. 2020 ð.
 *      Author: banz
 */

#include <CanCurtisAdapter.h>

enum class CurtisMessType
{
	Nmt = 0,
	Emergency = 0x1,
	Pdo1Miso = 0x3,
	Pdo1Mosi = 0x4,
	Pdo2Miso = 0x5,
	Pdo2Mosi = 0x6,
	Sdo1Miso = 0xB,
	Sdo1Mosi = 0xC,
	HeartBeat = 0xE
};

CanCurtisAdapter::CanCurtisAdapter(uint8_t nodeId)
	: CanDevice(0,0,0,0)
{
	_NodeId = nodeId;
	_MotorRpm = 0;
	_CurrentRms = 0;
	_ControllerTemperature = 0;
	_MotorTemperature = 0;
}

bool CanCurtisAdapter::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (rxHeader.StdId == ((uint8_t)_NodeId | ((uint8_t)(CurtisMessType::Pdo1Miso) << 7)))
	{
		_MotorRpm = (data[5] << 8 | data[4]);
		_CurrentRms = (data[7] << 8 | data[6]);
		return true;
	}

	if (rxHeader.StdId == ((uint8_t)_NodeId | ((uint8_t)(CurtisMessType::Pdo2Miso) << 7)))
	{
		_MotorTemperature = (data[5] << 8 | data[4]);
		_ControllerTemperature = (data[3] << 8 | data[2]);
		return true;
	}

	if (rxHeader.StdId == ((uint8_t)_NodeId | ((uint8_t)(CurtisMessType::Sdo1Miso) << 7)))
	{
		return true;
	}

	return false;
}

int16_t CanCurtisAdapter::Rpm()
{
	return _MotorRpm;
}

int16_t CanCurtisAdapter::Current()
{
	return _CurrentRms;
}

int16_t CanCurtisAdapter::TempConstroller()
{
	return _ControllerTemperature;
}

int16_t CanCurtisAdapter::TempMotor()
{
	return _MotorTemperature;
}
