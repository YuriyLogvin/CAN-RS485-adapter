/*
 * StatusBits.h
 *
 *  Created on: 18 квіт. 2020 р.
 *      Author: banz
 */

#ifndef INTERFACEMETODS_STATUSBITS_H_
#define INTERFACEMETODS_STATUSBITS_H_


enum class StatusBits
{
	None = 0,
	ChargeIn = 	0x01,
	ChargeOut =	0x02,
	PowerIn = 	0x04,
	PowerOut =	0x08,
	HeaterOn =	0x10
};


#endif /* INTERFACEMETODS_STATUSBITS_H_ */
