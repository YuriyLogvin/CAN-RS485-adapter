/*
 * ChargerFlatPack2.cpp
 *
 *  Created on: 11 серп. 2021 р.
 *      Author: banz
 */

#include <CanDevices/ChargerFlatPack2.h>
#include "Hal.h"
#include "string.h"

ChargerFlatPack2::ChargerFlatPack2()
	: CanDevice(0,0,2000,CanMode::Extended),
	  ICharger()
{
	_TransmitState = CanTransmitStates::None;
	_LastReceiving = 0;
	_TransmitTicker = 0;
	_TicksLogin = 0;
	_TicksVClimits = 0;
	for(int8_t i = 0; i < MaxContChargerFlatPack; i++)
		memset(_DetectedFlatPack2, 0, sizeof(_DetectedFlatPack2));
	_ActiveCharger = 0;

}

ChargerFlatPack2::~ChargerFlatPack2() {
	// TODO Auto-generated destructor stub
}


void ChargerFlatPack2::OnTick()
{
	if (Hal::GetSpendTicks(_TransmitTicker) < Hal::GetTicksInMilliSecond()*500)
		return; //таймаут между отправками
	_TransmitTicker = Hal::GetTickCount();

	//_TransmitState = CanTransmitStates::SendLogin;

	if (Hal::GetSpendTicks(_LastReceiving) > Hal::GetTicksInMilliSecond()*5000)
	{
		//_TransmitState = CanTransmitStates::Losted;
		//return;
	}

	switch (_TransmitState)
	{
	case CanTransmitStates::SendLogin:

		if (Hal::GetSpendTicks(_TicksLogin) < Hal::GetTicksInSecond() * 5)
		{
			//_TransmitState = CanTransmitStates::SendVClimits;
			break;
		}

		if (_DetectedFlatPack2[_ActiveCharger].Id == 0 || _ActiveCharger >= MaxContChargerFlatPack)
		{
			_ActiveCharger = 0;
			_TicksLogin = Hal::GetTickCount();
			_TransmitState = CanTransmitStates::SendVClimits;
			break;
		}

		_TxData[0] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[1];
		_TxData[1] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[2];
		_TxData[2] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[3];
		_TxData[3] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[4];
		_TxData[4] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[5];
		_TxData[5] = ((uint8_t*)(&(_DetectedFlatPack2[_ActiveCharger].Id)))[6];
		_TxData[6] = 0;
		_TxData[7] = 0;

		_IdDest = 0x05004800;
		_IdDest |= (_ActiveCharger + 1) * 4;
		if (CanDevice::Transmit())
		{
			_DetectedFlatPack2[_ActiveCharger].LoggedIn = true;
			if (!_DetectedFlatPack2[_ActiveCharger].CheckedDefaultVoltage)
			{
				_TransmitState = CanTransmitStates::SetDefaultVoltage;
				_TicksLogin = Hal::GetTickCount();
			}
			else
				_ActiveCharger++;
		}
		break;

	case CanTransmitStates::SetDefaultVoltage:
		//Дефолтное напряжение проверяется и устанавливается после первого логина
		if (!_DetectedFlatPack2[_ActiveCharger].LoggedIn || _DetectedFlatPack2[_ActiveCharger].CheckedDefaultVoltage || _DetectedFlatPack2[_ActiveCharger].Id == 0 || _VoltageNeed == 0)
		{
			_TransmitState = CanTransmitStates::SendLogin;
			break;
		}

		if (Hal::GetSpendTicks(_TicksLogin) < Hal::GetTicksInMilliSecond() * 20)
			break;

		_TxData[0] = 0x29;
		_TxData[1] = 0x15;
		_TxData[2] = 0;
		_TxData[3] = (_VoltageNeed * 10);
		_TxData[4] = (_VoltageNeed * 10) >> 8;
		_TxData[5] = 0;
		_TxData[6] = 0;
		_TxData[7] = 0;

		_IdDest = 0x05009C00;
		_IdDest |= (_ActiveCharger + 1) << 16;
		if (CanDevice::Transmit(5))
		{
			_DetectedFlatPack2[_ActiveCharger].CheckedDefaultVoltage = true;
			//_TransmitState = CanTransmitStates::SendLogin;
			_TransmitState = CanTransmitStates::SendVClimits;
		}

		break;

	case CanTransmitStates::SendVClimits:
		if (Hal::GetSpendTicks(_TicksVClimits) < Hal::GetTicksInSecond() * 2)
			break;

		if (_DetectedFlatPack2[_ActiveCharger].Id == 0 || _ActiveCharger >= MaxContChargerFlatPack)
		{
			_ActiveCharger = 0;
			_TicksVClimits = Hal::GetTickCount();
			_TransmitState = CanTransmitStates::SendLogin;
			break;
		}

		{
			uint16_t curr = _CurrentNeed;
			uint16_t volt = _VoltageNeed * 10;
			uint16_t voltOvp = _VoltageOver * 10;

			if (!_ChargerEnable)
			{
				curr = 0;
				volt = 0;
			};

			//curr = 50;
			//volt = 5100;
			//voltOvp = 5950;

			_TxData[0] = curr;
			_TxData[1] = curr >> 8;
			_TxData[2] = volt;
			_TxData[3] = volt >> 8;
			_TxData[4] = volt;
			_TxData[5] = volt >> 8;
			_TxData[6] = voltOvp;
			_TxData[7] = voltOvp >> 8;
		}

		_IdDest = 0x05004004;
		//установка лимитов работает почему то только через бродкаст-адрес
		//_IdDest |= (_ActiveCharger + 1) << 16;
		_IdDest |= 0xff << 16;
		if (CanDevice::Transmit())
			_ActiveCharger++;
		break;
	default:
		break;
	}

}

bool ChargerFlatPack2::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	auto messId = rxHeader.ExtId;

	if ((messId & 0xffff0000) == 0x05000000)
	{ //MSG CAN hello packet, 0x0500XXXX
		_LastReceiving = Hal::GetTickCount();
		int8_t i = 0;
		bool detected = false;
		for(; i < MaxContChargerFlatPack; i++)
		{
			if (_DetectedFlatPack2[i].Id == 0)
				break;
			if (memcmp(((uint8_t*)&(_DetectedFlatPack2[i].Id))+1, data+1, 6)==0)
			{
				detected = true;
				break;
			}
		}
		if (i < MaxContChargerFlatPack && !detected)
		{
			_TransmitState = CanTransmitStates::SendLogin;
			_DetectedFlatPack2[i].Id = *((uint64_t*)data);
		}

		return true;
	}

	if ((messId & 0xff00ffff) == 0x05004400)
	{ //MSG Login request / start-up notification, 0x05XX4400
		uint8_t id = (uint8_t)(messId >> 16) - 1;
		if (id >= MaxContChargerFlatPack)
			return false;
		if (memcmp(((uint8_t*)&(_DetectedFlatPack2[id].Id))+1, data, 6)!=0)
			return false;
		_DetectedFlatPack2[id].LoggedIn = false;
		_DetectedFlatPack2[id].CheckedDefaultVoltage = false;
		return true;
	}

	if ((messId & 0xff00ff00) == 0x05004000)
	{ //MSG Status, 0x05XX40YY
		uint8_t id = (uint8_t)(messId >> 16) - 1;
		if (id >= MaxContChargerFlatPack)
			return false;
		if (_DetectedFlatPack2[id].Id == 0)
			return false;
		_DetectedFlatPack2[id].IntakeTemp = data[0];
		_DetectedFlatPack2[id].CurrentOut = (data[1] << 8)  | data[2];
		_DetectedFlatPack2[id].VoltageOut = (data[3] << 8)  | data[4];
		_DetectedFlatPack2[id].VoltageInp = (data[5] << 8)  | data[6];
		_DetectedFlatPack2[id].ExhaustTemp = data[7];

		return true;
	}

	return false;
}

bool ChargerFlatPack2::IsOnline()
{
	return CanDevice::IsOnline();
}

