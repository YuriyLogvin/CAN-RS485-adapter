/*
 * CanCurtisAdapterSdo.cpp
 *
 *  Created on: 27 ëþò. 2020 ð.
 *      Author: banz
 */

#include <CanCurtisAdapterSdo.h>
#include <sys/_stdint.h>
#include "Hal.h"

enum class SDOTypes
{
	EMERGENCY = 0x1,
	SDO_MISO = 0xB,
	SDO_MOSI = 0xC
};

CanCurtisAdapterSdo::CanCurtisAdapterSdo(uint8_t nodeId)
	: CanDevice(0,0,0,CanMode::Standart)
{
	_NodeId = nodeId;
	_RequestsState = RequestsStates::None;
	_RequestsTicks = Hal::GetTickCount();

	_Rpm = 0;
	_Current = 0;
	_Voltage = 0;
	_TempConstroller = 0;
	_TempMotor = 0;
	_ErrorMessage = false;

	_Switches = 0;
}

void CanCurtisAdapterSdo::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 50)
		return;

	switch (_RequestsState)
	{
	case RequestsStates::Rpm:
		_RequestsState = RequestsStates::Voltage;
		break;
	case RequestsStates::Voltage:
		_RequestsState = RequestsStates::TempController;
		break;
	case RequestsStates::TempController:
		_RequestsState = RequestsStates::TempMotor;
		break;
	case RequestsStates::TempMotor:
		_RequestsState = RequestsStates::CurrentRms;
		break;
	case RequestsStates::CurrentRms:
		_RequestsState = RequestsStates::Switches;
		break;
	case RequestsStates::Switches:
		_RequestsState = RequestsStates::ErrorRegister;
		break;
	case RequestsStates::ErrorRegister:
		/*if (_ErrorMessage)
			_RequestsState = RequestsStates::EmergencyObject;
		else*/
			_RequestsState = RequestsStates::Rpm;
		break;
	case RequestsStates::EmergencyObject:
		_RequestsState = RequestsStates::Rpm;
		break;
	default:
		_RequestsState = RequestsStates::Rpm;
		break;
	}

	_TxData[0] = 0x42; /* 010(CS) 000 1 0 */

	_TxData[1] = (uint8_t)_RequestsState;
	_TxData[2] = (uint16_t)_RequestsState >> 8;
	_TxData[3] = 0;
	_TxData[4] = 0;
	_TxData[5] = 0;
	_TxData[6] = 0;
	_TxData[7] = 0;

	_IdDest = _NodeId | ((uint16_t)SDOTypes::SDO_MOSI << 7);
	CanDevice::Transmit();

	_RequestsTicks = Hal::GetTickCount();

}

void CanCurtisAdapterSdo::_ProcessErr(uint8_t data[])
{

}

bool CanCurtisAdapterSdo::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (rxHeader.StdId != (_NodeId | ((uint16_t)SDOTypes::SDO_MISO << 7)))
	{
		if (rxHeader.StdId != (_NodeId | ((uint16_t)SDOTypes::EMERGENCY << 7)))
			return false;

		_ProcessErr(data);
		return true;
	}

	if ((data[0] & 0xE0) != 0x40)
	{
		if ((data[0] & 0xE0) != 0x80)
			return false; /*Check CS*/

		//The “Error” (Abort SDO Transfer) Message
		//1. 0x06020000 – “Object does not exist in dictionary”.
		return true;
	}

	if (data[1] != (uint8_t)_RequestsState ||
		data[2] != (uint16_t)_RequestsState >> 8)
		return false;

	uint32_t sdoValue = 0;
	sdoValue |= (uint32_t)data[4] << 16;
	sdoValue |= (uint32_t)data[5] << 24;
	sdoValue |= (uint32_t)data[6];
	sdoValue |= (uint32_t)data[7] << 8;

	switch (_RequestsState)
	{
	case RequestsStates::Voltage:
		_Voltage = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::Rpm:
		_Rpm = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::TempController:
		_TempConstroller = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::TempMotor:
		_TempMotor = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::CurrentRms:
		_Current = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::ErrorRegister:
		_ErrorMessage = (sdoValue >> 16) == 1;
		break;
	case RequestsStates::EmergencyObject:
		_Current = (int16_t)(sdoValue >> 16);
		break;
	case RequestsStates::Switches:
		_Switches = (uint16_t)(sdoValue >> 16);
		_Switches &= 0x7fff;
		if (_ErrorMessage)
			_Switches |= 0x8000;
		break;
	default:
		break;
	};



	return true;
}


int16_t CanCurtisAdapterSdo::Rpm()
{
	return _Rpm;
}

int16_t CanCurtisAdapterSdo::Current()
{
	return _Current;
}

int16_t CanCurtisAdapterSdo::Voltage()
{
	return _Voltage/10;

}

int16_t CanCurtisAdapterSdo::TempConstroller()
{
	return _TempConstroller;
}

int16_t CanCurtisAdapterSdo::TempMotor()
{
	return _TempMotor;
}

uint16_t CanCurtisAdapterSdo::Switches()
{
	return _Switches;
}
