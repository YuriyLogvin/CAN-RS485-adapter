/*
 * ChillerSongZ.cpp
 *
 *  Created on: 25 лист. 2021 р.
 *      Author: banz
 */

#include <ChillerSongZ.h>
#include <CanDevices/ChillerSongZ.h>
#include "hal.h"

ChillerSongZ::ChillerSongZ()
	: CanDevice(0,0,2000,CanMode::Extended),
	  IChiller()
{
	_LastReceiving = 0;
	_TransmitTicker = 0;
	_TransmitCounter = 0;
}

ChillerSongZ::~ChillerSongZ() {
	// TODO Auto-generated destructor stub
}


void ChillerSongZ::OnTick()
{
	if (Hal::GetSpendTicks(_TransmitTicker) < Hal::GetTicksInMilliSecond()*500)
		return; //таймаут между отправками

	//_TransmitState = CanTransmitStates::SendLogin;

	if (Hal::GetSpendTicks(_LastReceiving) > Hal::GetTicksInMilliSecond()*5000)
	{
		//_TransmitState = CanTransmitStates::Losted;
		//return;
	}

	uint8_t mode = 0;
	mode |= (uint8_t)_ControlModeNeed;
	mode |=  (uint8_t)_ChillerModeNeed << 1;
	mode |=  (uint8_t)_BatteryModeNeed << 4;
	mode |=  (uint8_t)_HvModeNeed << 7;

	_TxData[0] = _TempHaveMax / 10 + 40;
	_TxData[1] = _TempHaveMin / 10 + 40;;
	_TxData[2] = _VoltageHave >> 8;
	_TxData[3] = _VoltageHave;
	_TxData[4] = mode;
	_TxData[5] = _TempTarget / 10 + 40;
	_TxData[6] = 0xff;
	_TxData[7] = _TransmitCounter;

	_IdDest = 0X18FF45F3;
	if (CanDevice::Transmit())
	{
		_TransmitCounter++;
		_TransmitTicker = Hal::GetTickCount();
	}

}

bool ChillerSongZ::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	auto messId = rxHeader.ExtId;

	if (messId != 0x18FFF345)
		return false;

	_LastReceiving = Hal::GetTickCount();

	_TempIn = ((int16_t)(data[0]) - 40) * 10;
	_TempOut = ((int16_t)(data[1]) - 40) * 10;
	_COP = data[3];
	_ErrorCode = data[4];

	uint8_t mode = data[2];
	_ControlModeHave = (ControlModes)(mode & 0x1);
	_ChillerModeHave = (ChillerModes)((mode >> 1) & 0x7);
	_BatteryModeHave = (BatteryModes)((mode >> 4) & 0x1);
	_HvModeHave = (HvModes)((mode >> 7) & 0x1);

	return true;
}

bool ChillerSongZ::IsOnline()
{
	return CanDevice::IsOnline();
}
