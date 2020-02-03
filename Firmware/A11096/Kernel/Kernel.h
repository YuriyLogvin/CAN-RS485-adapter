/*
 * Kernel.h
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

void BmsKernelInit();
void BmsKernelTick();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "IDataTransmitter.h"

class Kernel : public IDataTransmitter {

	static void _SendData();
	static void _ResponsePing();
	static void _ResponseRpm();
	static void _ResponseVoltage();
	static void _ResponseCurrent();
	static void _ProcessDataPacket();

public:
	Kernel() {};

	static void Init();
	static void Tick();

	virtual void Send(uint8_t* data, uint16_t len);
};

#endif //#ifdef __cplusplus
#endif /* KERNEL_H_ */
