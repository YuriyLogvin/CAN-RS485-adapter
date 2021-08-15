/*
 * SwitchesValues.h
 *
 *  Created on: 6 זמגע. 2020 נ.
 *      Author: banz
 */

#ifndef CANDEVICES_SWITCHESVALUES_H_
#define CANDEVICES_SWITCHESVALUES_H_


enum class SwitchesValues
{
	None = 0,
	Break = (1 << 1),
	Interlock = (1 << 2),
	Forward = (1 << 6),
	Backward = (1 << 7),
};


#endif /* CANDEVICES_SWITCHESVALUES_H_ */
