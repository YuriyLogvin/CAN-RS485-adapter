/*
 * IMotorController.h
 *
 *  Created on: 16 ñ³÷. 2020 ð.
 *      Author: banz
 */

#ifndef CANDEVICES_IMOTORCONTROLLER_H_
#define CANDEVICES_IMOTORCONTROLLER_H_

#include "stdint.h"

class IMotorController {
public:
	virtual int16_t Rpm() = 0;
	virtual int16_t Current() = 0; /*In 0.1A*/
	virtual int16_t Voltage() = 0; /*In 0.1V*/
	virtual int16_t TempConstroller() = 0; /*In 0.1 Deg*/
	virtual int16_t TempMotor() = 0; /*In 0.1 Deg*/
	virtual uint16_t Switches() = 0;

};

#endif /* CANDEVICES_IMOTORCONTROLLER_H_ */
