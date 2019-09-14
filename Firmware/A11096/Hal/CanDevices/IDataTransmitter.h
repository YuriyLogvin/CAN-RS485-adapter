/*
 * IDataTransmitter.h
 *
 *  Created on: Dec 2, 2017
 *      Author: banz
 */

#ifndef IDATATRANSMITTER_H_
#define IDATATRANSMITTER_H_

#include <stdint.h>

class IDataTransmitter {
public:
	virtual void Send(uint8_t* data, uint16_t len) = 0;
};

#endif /* IDATATRANSMITTER_H_ */
