/*
 * InterfaceMetodsCharger.h
 *
 *  Created on: 13 трав. 2020 р.
 *      Author: banz
 */

#ifndef INTERFACEMETODS_INTERFACEMETODSCHARGER_H_
#define INTERFACEMETODS_INTERFACEMETODSCHARGER_H_

enum class ChargerStates
{
	None = 0,
	Error = 0x1,
	Plugged = 0x2,
	Charging = 0x4
};

enum class InterfaceMetodsCharger
{
	ProcessCharging = 3,
	StateGet = 4,
	VoltageGet = 5,
	CurrentGet = 6
};



#endif /* INTERFACEMETODS_INTERFACEMETODSCHARGER_H_ */
