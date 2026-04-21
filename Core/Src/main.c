/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);


/* Private user code ---------------------------------------------------------*/

void LCD_init(void)
{
	// set PA0-PA7 to correspond to D0-D7 on the display
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // enable GPIOA

	// set mode to 01b
    GPIOA->MODER &=   ~(GPIO_MODER_MODE0 |
    					GPIO_MODER_MODE1 |
						GPIO_MODER_MODE2 |
						GPIO_MODER_MODE3 |
						GPIO_MODER_MODE4 |
						GPIO_MODER_MODE5 |
						GPIO_MODER_MODE6 |
						GPIO_MODER_MODE7);

    GPIOA->MODER  |=   (GPIO_MODER_MODE0_0 |
						GPIO_MODER_MODE1_0 |
						GPIO_MODER_MODE2_0 |
						GPIO_MODER_MODE3_0 |
						GPIO_MODER_MODE4_0 |
						GPIO_MODER_MODE5_0 |
						GPIO_MODER_MODE6_0 |
						GPIO_MODER_MODE7_0);

    // PUPD to 00b
    GPIOA->PUPDR &=	  ~(GPIO_PUPDR_PUPD0 |
    					GPIO_PUPDR_PUPD1 |
						GPIO_PUPDR_PUPD2 |
						GPIO_PUPDR_PUPD3 |
						GPIO_PUPDR_PUPD4 |
						GPIO_PUPDR_PUPD5 |
						GPIO_PUPDR_PUPD6 |
						GPIO_PUPDR_PUPD7);

	// set PB0 to RS
	// set PB1 to RW
	// set PB2 to E
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);

    GPIOB->MODER &=   ~(GPIO_MODER_MODE0);

    GPIOB->MODER &=   ~(GPIO_MODER_MODE0 |
    					GPIO_MODER_MODE1 |
						GPIO_MODER_MODE2);

    GPIOB->MODER |=    (GPIO_MODER_MODE0 |
			  	  	  	GPIO_MODER_MODE1 |
					    GPIO_MODER_MODE2);

    GPIOB->PUPDR &=   ~(GPIO_PUPDR_PUPD0 |
						GPIO_PUPDR_PUPD1 |
						GPIO_PUPDR_PUPD2);

}

enum LCD_Pins {
	RS,
	RW,
	E,
	DB0,
	DB1,
	DB2,
	DB3,
	DB4,
	DB5,
	DB6,
	DB7
	,
};


void LCD_RS_set_0(void) {
	GPIOB->ODR &= ~(GPIO_PIN_0);
}

void LCD_RS_set_1(void) {
	GPIOB->ODR |= (GPIO_PIN_0);
}

void LCD_RW_set_0(void) {
	GPIOB->ODR &= ~(GPIO_PIN_1);
}

void LCD_RW_set_1(void) {
	GPIOB->ODR |= (GPIO_PIN_1);
}

void LCD_E_set_0(void) {
	GPIOB->ODR &= ~(GPIO_PIN_2);
}

void LCD_E_set_1(void) {
	GPIOB->ODR |= (GPIO_PIN_2);
}

void LCD_DR_set(uint8_t mask)
{
	// assumes that DR = GPIOA0-7

	uint8_t reset_mask = 0;
	for (int i = 0; i < 8; i++){
		reset_mask |= 1 << i;
	}

	GPIOA->ODR &= ~(reset_mask);
	GPIOA->ODR |= mask;
}


void LCD_command(uint8_t command)
{
	int delay_ms = 1;

	LCD_RS_set_0();
	LCD_RW_set_0();
	LCD_DR_set(command);
	HAL_Delay(delay_ms);

	LCD_E_set_1();
	HAL_Delay(delay_ms);
	LCD_E_set_0();

	HAL_Delay(delay_ms);
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  LCD_init();

  while (1)
  {

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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
