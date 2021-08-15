/*
 * ChargerSaeJ1939.h
 *
 *  Created on: 22 лип. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CHARGERSAEJ1939_H_
#define CANDEVICES_CHARGERSAEJ1939_H_

#include "CanDevice.h"
#include "ICharger.h"

class ChargerSaeJ1939 : public CanDevice, public ICharger {
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

	virtual void SetVoltage(uint16_t) override;
	virtual uint16_t GetVoltage() override;

	virtual void SetOverVoltage(uint16_t val) override;

	virtual void SetCurrent(uint16_t) override;
	virtual uint16_t GetCurrent() override;

	virtual uint8_t GetChargerFlags() override;

	virtual void TurnCharger(bool value) override;

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
};

#endif /* CANDEVICES_CHARGERSAEJ1939_H_ */
