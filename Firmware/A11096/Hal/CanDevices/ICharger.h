/*
 * ICharger.h
 *
 *  Created on: 7 ρεπο. 2021 π.
 *      Author: banz
 */

#ifndef CANDEVICES_ICHARGER_H_
#define CANDEVICES_ICHARGER_H_

#include <stdint.h>

class ICharger {
public:
	virtual void SetVoltage(uint16_t val) = 0;
	virtual uint16_t GetVoltage() = 0;

	virtual void SetOverVoltage(uint16_t val) = 0;

	virtual void SetCurrent(uint16_t val) = 0;
	virtual uint16_t GetCurrent() = 0;

	virtual uint8_t GetChargerFlags() = 0;

	virtual void TurnCharger(bool val) = 0;

};

#endif /* CANDEVICES_ICHARGER_H_ */
