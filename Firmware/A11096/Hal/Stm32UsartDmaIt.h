/*
 * Stm32UsartRecever.h
 *
 *  Created on: Feb 26, 2017
 *      Author: banz
 */

#ifndef STM32USARTDMAIT_H_
#define STM32USARTDMAIT_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "ReceiveBuffer.h"


class Stm32UsartDmaIt {
	friend void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
	friend void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
	friend void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
	uint16_t _SendBufSize;
	ReceiveBuffer* _ReceiveBuffer;
	uint16_t _Rbyte;
	uint8_t* _UsartSendBuff;
	UART_HandleTypeDef* _Huart;
	GPIO_TypeDef* _TxEnPort;
	uint16_t _TxEnPin;
	bool _InTxMode;
	volatile uint16_t _ErrorCount;

	void _Init(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin);
	void _TxCmplt();
	void _RxCmplt();

public:
	Stm32UsartDmaIt(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize);
	Stm32UsartDmaIt(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin);
	~Stm32UsartDmaIt();

	bool Send(uint8_t* data, uint16_t dataLen);
	bool Send(const char* format, ...);
	uint16_t Receive(uint8_t* data, uint16_t dataBufSize);
	bool Receive(uint8_t& data);

	bool Redy4Send();

	void RxStop();
	void RxStart();

	void Restart();
};

#endif /* STM32USARTDMA_H_ */
