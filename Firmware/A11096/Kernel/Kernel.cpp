/*
 * Kernel.cpp
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#include "Kernel.h"
#include "Hal.h"
#include "CanSniffer.h"

Kernel* _Kernel = NULL;

void BmsKernelInit()
{
	Hal::Init();

	_Kernel = new Kernel();

	Kernel::Init();

}

void BmsKernelTick()
{
	Hal::Tick();
	Kernel::Tick();
}

CanSniffer* _CanSniffer = 0;
void Kernel::Init()
{
	_CanSniffer = new CanSniffer(_Kernel);
}

int32_t _KernelTicks = 0;

void Kernel::Tick()
{
	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 1000)
		return;

	Hal::LedBlue(!Hal::LedBlue());

	Hal::UsartExt->Send("Kernel::Tick()\n\r");

	_KernelTicks = Hal::GetTickCount();
}

void Kernel::Send(uint8_t* data, uint16_t len)
{
	Hal::UsartExt->Send(data, len);
}

