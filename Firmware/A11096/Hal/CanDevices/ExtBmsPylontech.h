/*
 * ExtBmsPylontech.h
 *
 *  Created on: 26 лип. 2022 р.
 *      Author: banz
 */

#ifndef CANDEVICES_EXTBMSPYLONTECH_H_
#define CANDEVICES_EXTBMSPYLONTECH_H_

#include <CanDevices/IExtBms.h>
#include "CanDevice.h"

#define BmsFuturesExtBmsPylontech

#ifdef BmsFuturesExtBmsPylontech

class ExtBmsPylontech : public CanDevice, public IExtBms {
	int32_t _RequestsTicks;
	uint8_t _TransmitStage;

	uint8_t _PackNumber;

public:
	ExtBmsPylontech();
	virtual ~ExtBmsPylontech();
protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
	virtual bool IsOnline();
};

#endif //BmsFuturesExtBmsPylontech
#endif /* CANDEVICES_EXTBMSPYLONTECH_H_ */
