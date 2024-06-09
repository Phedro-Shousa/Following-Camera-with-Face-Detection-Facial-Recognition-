/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#define ZOMBIE_SERVO	0
#define RIGHT_SERVO		1
#define LEFT_SERVO		2
#define UP_SERVO			3
#define DOWN_SERVO		4
#define FIND_MODE			5

uint8_t UART3Rx_Buffer[128];
uint8_t UART1Rx_Buffer[128];
uint8_t Rx_Buffer[128];
uint8_t cmd_servos;
uint8_t user_ticks;
int angle_found;
volatile uint8_t UART3Rx_index;
volatile uint8_t UART1Rx_index;
volatile uint8_t WI_FI;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
//#############################   UART TESTS   #######################
//redifine the stdout
int fputc(int ch, FILE *f){
	if(WI_FI){
		HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 100);
		return ch;
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100);
		return ch;
	}
}

//implemantation of UART ISR
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	if (huart->Instance == USART3){ //current UART?
		WI_FI = 0;
		printf("%c", UART3Rx_Buffer[UART3Rx_index]);
		if(UART3Rx_Buffer[UART3Rx_index] == 'A'){
			cmd_servos = LEFT_SERVO;			
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'S'){
			cmd_servos = DOWN_SERVO;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'D'){
			cmd_servos = RIGHT_SERVO;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'W'){
			cmd_servos = UP_SERVO;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'F'){ //find
			cmd_servos = FIND_MODE;
			angle_found = 500;
		}		

			
		
		UART3Rx_index++;
		UART3Rx_index &= ~(1<<7); //keep index inside the limits
		
		// set the interrupt for UART3 Rx again
		HAL_UART_Receive_IT(&huart3, &UART3Rx_Buffer[UART3Rx_index], 1);
	}
	
//		if (huart->Instance == USART1){ //current UART?
//		WI_FI = 0;
//		printf("%c", UART1Rx_Buffer[UART1Rx_index]);
//		
//		UART1Rx_index++;
//		UART1Rx_index &= ~(1<<7); //keep index inside the limits
//		
//		// set the interrupt for UART3 Rx again
//		HAL_UART_Receive_IT(&huart1, &UART1Rx_Buffer[UART1Rx_index], 1);
//	}
	
}

int messageReceived(){
	static int last_index = 0;
	static int out_index = 0;
	int return_value = 0;
	while(last_index != UART3Rx_index){
		if(UART3Rx_Buffer[last_index] == '\n'){
			Rx_Buffer[out_index - 1] = '\0';
			return_value = out_index;
			out_index = 0;
		}
		else{
			Rx_Buffer[out_index] = UART3Rx_Buffer[last_index];
			out_index++;
			return_value = 0;
		}
		last_index++;
		last_index &= ~(1<<7);
	}
	return return_value;
}

void init_UART3(){
	// set the interrupt for UART3 Rx
	HAL_UART_Receive_IT(&huart3, &UART3Rx_Buffer[UART3Rx_index], 1);
}


//#################### UART WI FI ##############################
//redifine the stdout

void init_UART1(){
	// set the interrupt for UART3 Rx
	HAL_UART_Receive_IT(&huart1, &UART1Rx_Buffer[UART1Rx_index], 1);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
