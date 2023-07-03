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
	virtual bool Send(uint8_t* data, uint16_t len) = 0;
	virtual bool IsBusy() = 0;
};

#endif /* IDATATRANSMITTER_H_ */
