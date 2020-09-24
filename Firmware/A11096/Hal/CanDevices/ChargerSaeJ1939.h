/*
 * ChargerSaeJ1939.h
 *
 *  Created on: 22 ���. 2020 �.
 *      Author: banz
 */

#ifndef CANDEVICES_CHARGERSAEJ1939_H_
#define CANDEVICES_CHARGERSAEJ1939_H_

#include "CanDevice.h"

class ChargerSaeJ1939 : public CanDevice {
	uint16_t _VoltageNeed;
	uint16_t _CurrentNeed;
	uint16_t _VoltageHave;
	uint16_t _CurrentHave;
	uint8_t _ChargerFlags;
	uint8_t _ChargerEnable;
	int32_t _RequestsTicks;

public:
	ChargerSaeJ1939();
	virtual ~ChargerSaeJ1939();

	void SetVoltage(uint16_t);
	uint16_t GetVoltage();

	void SetCurrent(uint16_t);
	uint16_t GetCurrent();

	uint8_t GetChargerFlags();

	void TurnCharger(bool value);

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
};

#endif /* CANDEVICES_CHARGERSAEJ1939_H_ */