/*
 * ChargerSaeJ1939.h
 *
 *  Created on: 22 ���. 2020 �.
 *      Author: banz
 */

#ifndef CANDEVICES_CHARGERSAEJ1939_H_
#define CANDEVICES_CHARGERSAEJ1939_H_

#include "CanDevice.h"
#include "CanDevices/ICharger.h"

class ChargerSaeJ1939 : public CanDevice, public ICharger {

public:
	ChargerSaeJ1939();
	virtual ~ChargerSaeJ1939();

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

	virtual bool IsOnline();

};

#endif /* CANDEVICES_CHARGERSAEJ1939_H_ */
