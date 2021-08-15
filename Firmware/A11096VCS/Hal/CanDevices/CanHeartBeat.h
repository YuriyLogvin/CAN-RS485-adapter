/*
 * CanHeartBeat.h
 *
 *  Created on: 13 ρεπο. 2021 π.
 *      Author: banz
 */

#ifndef CANDEVICES_CANHEARTBEAT_H_
#define CANDEVICES_CANHEARTBEAT_H_

#include "CanDevice.h"

class CanHeartBeat : public CanDevice {
	int32_t _RequestsTicks;
	uint8_t _Flags;
public:
	CanHeartBeat(uint32_t destNodeId);
	virtual ~CanHeartBeat();

	void SetFlags(uint8_t);

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
};

#endif /* CANDEVICES_CANHEARTBEAT_H_ */
