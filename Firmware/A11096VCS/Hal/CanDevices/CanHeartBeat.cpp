/*
 * CanHeartBeat.cpp
 *
 *  Created on: 13 ρεπο. 2021 π.
 *      Author: banz
 */

#include <CanHeartBeat.h>
#include <sys/_stdint.h>
#include "Hal.h"

CanHeartBeat::CanHeartBeat(uint32_t destNodeId)
	: CanDevice(0,destNodeId,0,CanMode::Standart)
{
	_RequestsTicks = 0;
	_Flags = 0;
}

CanHeartBeat::~CanHeartBeat() {
	// TODO Auto-generated destructor stub
}

void CanHeartBeat::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 500)
		return;

	_TxData[0] = _Flags;
	_TxData[1] = 0;
	_TxData[2] = 0;
	_TxData[3] = 0;
	_TxData[4] = 0;
	_TxData[5] = 0;
	_TxData[6] = 0;
	_TxData[7] = 0;

	if (CanDevice::Transmit())
		_RequestsTicks = Hal::GetTickCount();;
}

bool CanHeartBeat::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	return false;
}

void CanHeartBeat::SetFlags(uint8_t flags)
{
	_Flags = flags;
}

