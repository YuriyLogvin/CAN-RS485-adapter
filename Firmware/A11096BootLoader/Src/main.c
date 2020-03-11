/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "bootloader.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void XmLedEn(char val);
void XmLedAlt(void);
void XmWriteCom(unsigned char b);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_IWDG_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  if (HAL_GPIO_ReadPin(NeedBoot_GPIO_Port, NeedBoot_Pin) == GPIO_PIN_RESET)
	  Jump2Prog();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	//uint32_t ticks = HAL_GetTick();
	  if (Boot())
	  {
		  while (HAL_GPIO_ReadPin(NeedBoot_GPIO_Port, NeedBoot_Pin) != GPIO_PIN_SET)
			  XmLedEn(1); //Wait boot-switch removing
		  Jump2Prog();
	  }

	 // XmWriteCom('1');
	  /*XmWriteCom('2');
	  XmWriteCom('3');
	  XmWriteCom('\n');
	  XmWriteCom('\r');*/

	//uint32_t ticks = HAL_GetTick();
	//while (ticks + 10 > HAL_GetTick());

	  HAL_IWDG_Refresh(&hiwdg);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(GPIOB, LED_Pin|RS485_EN_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pins : LED_Pin CAN_S_Pin CAN_EN_Pin RS485_EN_Pin */
	  GPIO_InitStruct.Pin = LED_Pin|RS485_EN_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  /*Configure GPIO pin : NeedBoot_Pin */
		GPIO_InitStruct.Pin = NeedBoot_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(NeedBoot_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

uint32_t XmGetTickCount()
{
	return HAL_GetTick();
}

void XmLedEn(char val)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, val?GPIO_PIN_SET:GPIO_PIN_RESET);
}

void XmLedAlt(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

char XmReadCom(unsigned char* b, unsigned short readComTimeout)
{
	uint32_t ticks = HAL_GetTick();
	while (ticks + readComTimeout > HAL_GetTick())
	{
		if (HAL_OK == HAL_UART_Receive(&huart1, b, 1, 0))
			return 1;
	}

	return 0;
}

void XmWriteCom(unsigned char b)
{
	int i;
	HAL_GPIO_WritePin(GPIOB, RS485_EN_Pin, GPIO_PIN_SET);
	for (i = 0; i < 20; i++);
	HAL_UART_Transmit(&huart1, &b, 1, 100);
	while(!(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE)));
	for (i = 0; i < 20; i++);
	HAL_GPIO_WritePin(GPIOB, RS485_EN_Pin, GPIO_PIN_RESET);
}

void XmClearBuffers()
{
}

void wdt_reset()
{
  HAL_IWDG_Refresh(&hiwdg);
}

/*void Hal::Init(bool canSilentMode)
{
	UsartExt = new Stm32UsartDma(&huart1, 0x200, 0x100, RS485_EN_GPIO_Port, RS485_EN_Pin);

	CanDevice::Init(&hcan, CanDevice::Speeds::s125k, canSilentMode);

}

void Hal::Tick()
{
	CanDevice::Tick();
}

bool Hal::LedBlue()
{
	return (HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET);
}

void Hal::LedBlue(bool value)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, value?GPIO_PIN_SET:GPIO_PIN_RESET);
}

short Hal::GetTicksInSecond()
{
	return 1000;
}

short Hal::GetTicksInMilliSecond()
{
	return 1;
}

int32_t Hal::GetTickCount()
{
	return HAL_GetTick();
}

int32_t Hal::GetSpendTicks(int32_t fromTicks)
{
	int32_t nowT = GetTickCount();
	if (nowT >= fromTicks)
		return nowT - fromTicks;

	return (0xffffffff - fromTicks + nowT);
	//return embedded_get_spent_ms(fromTicks);
}

void Hal::Sleep(uint16_t sleepMs)
{
	int32_t t = Hal::GetTickCount();
	while (Hal::GetSpendTicks(t) < Hal::GetTicksInMilliSecond() * sleepMs);
}

void Hal::UpdateWdt()
{
	HAL_IWDG_Refresh(&hiwdg);*/


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
