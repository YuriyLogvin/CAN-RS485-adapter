/*
 * Hal.h
 *
 *  Created on: Feb 25, 2017
 *      Author: banz
 */

#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>
#include "Stm32UsartDmaIt.h"

#define MODE_SNIFFER 1
#define MODE_TOS 2
#define MODE_CURTIS_SDO 3
#define MODE_CURTIS_PDO 4
#define MODE_KELLY 5

#ifdef DEBUG
//#define MODE MODE_CURTIS_SDO
#endif

class Hal {

public:
	static void Init(bool canSilentMode);
	static void Tick();

	static bool LedBlue();
	static void LedBlue(bool);
	static Stm32UsartDmaIt* UsartExt;

	static int32_t GetTickCount();
	static int32_t GetSpendTicks(int32_t fromTicks);
	static int32_t GetTicksDiff(int32_t fromTicks, int32_t toTicks);
	static short GetTicksInSecond();
	static short GetTicksInMilliSecond();
	static void Sleep(uint16_t sleepMs);

	static void UpdateWdt();

private:
};

#endif /* HAL_H_ */
