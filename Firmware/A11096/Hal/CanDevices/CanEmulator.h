/*
 * CanEmulator.h
 *
 *  Created on: 5 трав. 2023 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANEMULATOR_H_
#define CANDEVICES_CANEMULATOR_H_

#include "CanDevice.h"
#include <map>
#include <vector>

class CanEmulator : public CanDevice
{
	struct ReceivedPacket
	{
		int32_t ReceivedTime;
		int32_t IntervalFromPrev;
		std::vector<uint8_t> Data;
	};
	std::map<uint32_t, ReceivedPacket> _ReceivedPool;

	struct SendPacket
	{
		uint32_t Id;
		int32_t Interval;
		int32_t LastSendTime;
		std::vector<uint8_t> Data;
	};
	std::vector<SendPacket> _SendPool;
	std::vector<SendPacket>::iterator _SendPoolCit;

	std::vector<uint32_t> _FilteredIds;

	int32_t _OnTickTicks;
protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
public:
	CanEmulator();
	virtual ~CanEmulator();

	void FilterAdd(uint32_t id);
	void FilterDel(uint32_t id);

	//return interval from last packet
	int32_t GetData(uint32_t id, uint8_t data[], int32_t& intervalLastTransmits);

	void SetData(uint32_t id, uint16_t transmitIntervalMs, uint8_t data[]);
	void DelData(uint32_t id);

};

#endif /* CANDEVICES_CANEMULATOR_H_ */
