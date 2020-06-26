/*
 * Stm32UsartRecever.h
 *
 *  Created on: Feb 26, 2017
 *      Author: banz
 */

#ifndef STM32USARTDMA_H_
#define STM32USARTDMA_H_

#ifndef STM32F105xC
#define STM32F105xC
#endif //STM32F105xC

#include <stdint.h>
#include "stm32f1xx_hal.h"


class Stm32UsartDma {
	friend void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
	friend void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
	uint16_t _RcvBufSize;
	uint16_t _SendBufSize;
	uint8_t* _UsartRecvBuff;
	uint8_t* _UsartSendBuff;
	volatile uint16_t  _UsartReceivePos;
	UART_HandleTypeDef* _Huart;
	GPIO_TypeDef* _TxEnPort;
	uint16_t _TxEnPin;
	bool _InTxMode;
	volatile uint16_t _ErrorCount;

	void _Init(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin);
	void _TxCmplt();

public:
	Stm32UsartDma(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize);
	Stm32UsartDma(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin);
	~Stm32UsartDma();

	bool Send(uint8_t* data, uint16_t dataLen);
	bool Send(const char* format, ...);
	uint16_t Receive(uint8_t* data, uint16_t dataBufSize);

	bool Redy4Send();

	void RxStop();
	void RxStart();

	uint16_t Deb();
	uint8_t Deb1();

	void Restart();
};

#endif /* STM32USARTDMA_H_ */
