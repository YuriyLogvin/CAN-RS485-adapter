/*
 * CanDevice.h
 *
 *  Created on: Sep 15, 2017
 *      Author: banz
 */

#ifndef CANDEVICE_H_
#define CANDEVICE_H_

#include "stm32f1xx_hal.h"

enum CanDeviceStates
{
	cdsNone,
	cdsWorking,
	cdsBusError,
	cdsRecieveTimeOut
};

enum class CanMode
{
	None,
	Standart,
	Extended
};

class CanDevice {
	//friend void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan);
	//friend void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);
	uint32_t _ExtIdFilter;
	int32_t _Ticks;
	uint16_t _RecieveTimeoutMs;
	CanDeviceStates _CanDeviceState;
	static CAN_HandleTypeDef* _hCan;
	static bool _CanSilentMode;
public:
	CanDevice(uint32_t extIdFilter, uint32_t idDest, uint16_t recieveTimeoutMs, CanMode canMode = CanMode::Extended);
	virtual ~CanDevice();

	enum class Speeds
	{
		s125k,
		s250k,
		s500k
	};

	static int32_t ReceivedPackets();

	static int16_t Errors(uint32_t& errBits);
	static void ResetErrors();

	static void Init(CAN_HandleTypeDef* hCan, Speeds speed, bool silentMode);
	static void Tick();


protected:
	uint32_t _IdDest;
	CanMode _CanMode;

	uint8_t 		_TxData[8];
	uint32_t 		_RxExtId;
	uint32_t 		_RxStdId;
	int32_t _LastReceivingTime;

	bool Transmit(uint8_t dataLen = 8);

	CanDeviceStates _GetState();

	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

	void _Reinit();

};

#endif /* CANDEVICE_H_ */
