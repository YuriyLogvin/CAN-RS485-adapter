/*
 * ICharger.h
 *
 *  Created on: 21 вер. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_ICHARGERCONTROLLER_H_
#define CANDEVICES_ICHARGERCONTROLLER_H_

#include <stdint.h>

class ICharger {
protected:
	uint16_t _VoltageNeed;
	uint16_t _CurrentNeed;
	uint16_t _VoltageHave;
	uint16_t _CurrentHave;
	uint16_t _VoltageOver;
	uint8_t _ChargerFlags;
	bool _ChargerEnable;
	int32_t _RequestsTicks;

public:
	ICharger();

	virtual void SetVoltage(uint16_t); /*In 0.1V*/
	virtual uint16_t GetVoltage();/*In 0.1V*/

	virtual void SetCurrent(uint16_t);/*In 0.1A*/
	virtual uint16_t GetCurrent();/*In 0.1A*/

	virtual void SetOverVoltage(uint16_t val);/*In 0.1V*/

	virtual uint8_t GetChargerFlags();

	virtual void TurnCharger(bool value);

	virtual bool IsOnline() = 0;

};

#endif /* CANDEVICES_ICHARGERCONTROLLER_H_ */
