/*
 * CanEmulator.cpp
 *
 *  Created on: 5 трав. 2023 р.
 *      Author: banz
 */

#include <CanEmulator.h>
#include "hal.h"
#include "string.h"

CanEmulator::CanEmulator()
	: CanDevice(0, 0, 3000)
{
	_OnTickTicks = 0;
	_SendPoolCit = _SendPool.end();
}

CanEmulator::~CanEmulator() {
	// TODO Auto-generated destructor stub
}

void CanEmulator::OnTick()
{
	if (Hal::GetSpendTicks(_OnTickTicks) < Hal::GetTicksInMilliSecond()*5)
		return; //таймаут между отправками
	_OnTickTicks = Hal::GetTickCount();

	if (_SendPoolCit == _SendPool.end())
		_SendPoolCit = _SendPool.begin();

	if (_SendPoolCit == _SendPool.end())
		return;

	if (Hal::GetSpendTicks(_SendPoolCit->LastSendTime) < _SendPoolCit->Interval)
	{
		_SendPoolCit++;
		return;
	}

	_TxData[0] = _SendPoolCit->Data[0];
	_TxData[1] = _SendPoolCit->Data[1];
	_TxData[2] = _SendPoolCit->Data[2];
	_TxData[3] = _SendPoolCit->Data[3];
	_TxData[4] = _SendPoolCit->Data[4];
	_TxData[5] = _SendPoolCit->Data[5];
	_TxData[6] = _SendPoolCit->Data[6];
	_TxData[7] = _SendPoolCit->Data[7];

	_IdDest = _SendPoolCit->Id;
	if (CanDevice::Transmit())
	{
		_SendPoolCit->LastSendTime = Hal::GetTickCount();
		_SendPoolCit++;
	}

}

bool CanEmulator::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{

	Hal::LedBlue(true);

	uint32_t id = 0;

	for (auto cit = _FilteredIds.begin(); cit != _FilteredIds.end(); cit++)
		if (rxHeader.StdId == *cit)
		{
			id = rxHeader.StdId;
		}
		else if (rxHeader.ExtId == *cit)
		{
			id = rxHeader.ExtId;
		}

	if (id == 0)
		return false;

	ReceivedPacket& packet = _ReceivedPool[id];
	packet.Data.assign(data, data+8);
	volatile int32_t inter = packet.IntervalFromPrev = Hal::GetSpendTicks(packet.ReceivedTime);
	volatile int32_t rT = packet.ReceivedTime = Hal::GetTickCount();

	Hal::LedBlue(false);

	inter++;
	rT++;
	return true;
}

void CanEmulator::FilterAdd(uint32_t id)
{
	for (uint8_t i = 0; i < _FilteredIds.size(); i++)
		if (_FilteredIds[i] == id)
			return;
	_FilteredIds.push_back(id);
}

void CanEmulator::FilterDel(uint32_t id)
{
	for (auto cit = _FilteredIds.begin(); cit != _FilteredIds.end(); cit++)
		if (*cit == id)
		{
			_FilteredIds.erase(cit);
			return;
		}
}

//return interval from last packet
int32_t CanEmulator::GetData(uint32_t id, uint8_t data[], int32_t& intervalLastTransmits)
{
	auto d = _ReceivedPool.find(id);
	if (d == _ReceivedPool.end())
		return 0;

	memcpy(data, &(d->second.Data[0]), 8);
	intervalLastTransmits = d->second.IntervalFromPrev / Hal::GetTicksInMilliSecond();
	return Hal::GetSpendTicks(d->second.ReceivedTime) / Hal::GetTicksInMilliSecond();
}

void CanEmulator::SetData(uint32_t id, uint16_t transmitIntervalMs, uint8_t data[])
{
	uint8_t i = 0;
	for (; i < _SendPool.size(); i++)
	{
		if (_SendPool[i].Id == id)
			break;
	}
	if (i >= _SendPool.size())
		_SendPool.resize(i+1);

	_SendPool[i].Id = id;
	_SendPool[i].Interval = transmitIntervalMs * Hal::GetTicksInMilliSecond();
	_SendPool[i].LastSendTime = Hal::GetTickCount();
	_SendPool[i].Data.resize(8);
	memcpy(&(_SendPool[i].Data[0]), data, 8);

	_SendPoolCit = _SendPool.end();
}

void CanEmulator::DelData(uint32_t id)
{

	for (auto it = _SendPool.begin(); it != _SendPool.end(); it++)
		if (it->Id == id)
		{
			_SendPool.erase(it);
			_SendPoolCit = _SendPool.end();
			return;
		}

}

