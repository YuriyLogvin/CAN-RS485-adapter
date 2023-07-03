/*
 * ChillerSongZ.h
 *
 *  Created on: 25 лист. 2021 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CHILLERSONGZ_H_
#define CANDEVICES_CHILLERSONGZ_H_

#include "CanDevice.h"
#include "CanDevices/IChiller.h"


class ChillerSongZ : public CanDevice, public IChiller {
	int32_t _LastReceiving;
	int32_t _TransmitTicker;
	uint8_t _TransmitCounter;

public:
	ChillerSongZ();
	virtual ~ChillerSongZ();

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

	virtual bool IsOnline();

};

#endif /* CANDEVICES_CHILLERSONGZ_H_ */
