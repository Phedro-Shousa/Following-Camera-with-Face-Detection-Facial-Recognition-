/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

#define MANUAL 				6
#define SONAR					7
#define ADAPT_CONTROL	8

uint8_t UART3Rx_Buffer[128];
uint8_t UART4Rx_Buffer[128];
uint8_t UART5Rx_Buffer[128];
uint8_t Rx_Buffer[128];
uint8_t cmd_servos;
uint8_t user_ticks;
int angle_found;
volatile uint8_t UART3Rx_index;
volatile uint8_t UART4Rx_index;
volatile uint8_t UART5Rx_index;
volatile uint8_t WI_FI;
volatile uint8_t operation_mode;
volatile uint8_t flag_send = 0;
volatile uint8_t flag_connect = 0;
volatile uint8_t flag_on = 0;
/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart3;

/* UART4 init function */
void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 4800;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
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
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**UART4 GPIO Configuration    
    PD0     ------> UART4_RX
    PD1     ------> UART4_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */
    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**UART5 GPIO Configuration    
    PB12     ------> UART5_RX
    PB13     ------> UART5_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
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

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();
  
    /**UART4 GPIO Configuration    
    PD0     ------> UART4_RX
    PD1     ------> UART4_TX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();
  
    /**UART5 GPIO Configuration    
    PB12     ------> UART5_RX
    PB13     ------> UART5_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
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
	if(WI_FI == 1){
		HAL_UART_Transmit(&huart4, (uint8_t*)&ch, 1, 100);
		return ch;
	}else if(WI_FI==0){
		HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100);
		return ch;
	}else if(WI_FI==2){
		HAL_UART_Transmit(&huart5, (uint8_t*)&ch, 1, 100);
		return ch;
	}
}

//implemantation of UART ISR
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){

	if (huart->Instance == USART3){ //current UART?
		WI_FI = 0;
		printf("%c", UART3Rx_Buffer[UART3Rx_index]);
		if(UART3Rx_Buffer[UART3Rx_index] == 'D'){
			cmd_servos = LEFT_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'S'){
			cmd_servos = DOWN_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'A'){
			cmd_servos = RIGHT_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'W'){
			cmd_servos = UP_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'F'){ 
			angle_found = 500;
			operation_mode = SONAR;
		}
		else if(UART3Rx_Buffer[UART3Rx_index] == 'G'){
			operation_mode = ADAPT_CONTROL;
		}	
		
		UART3Rx_index++;
		UART3Rx_index &= ~(1<<7); //keep index inside the limits
		
		// set the interrupt for UART3 Rx again
		HAL_UART_Receive_IT(&huart3, &UART3Rx_Buffer[UART3Rx_index], 1);
	}
		if (huart->Instance == UART4){ //current UART?
		//WI_FI = 0;
		//printf("%c", UART4Rx_Buffer[UART4Rx_index]);
		if(UART4Rx_Buffer[UART4Rx_index] == 'D'){
			cmd_servos = LEFT_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART4Rx_Buffer[UART4Rx_index] == 'S'){
			cmd_servos = DOWN_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART4Rx_Buffer[UART4Rx_index] == 'A'){
			cmd_servos = RIGHT_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART4Rx_Buffer[UART4Rx_index] == 'W'){
			cmd_servos = UP_SERVO;
			operation_mode = MANUAL;
		}
		else if(UART4Rx_Buffer[UART4Rx_index] == 'F'){ 
			angle_found = 500;
			operation_mode = SONAR;
		}
		else if(UART4Rx_Buffer[UART4Rx_index] == 'G'){
			operation_mode = ADAPT_CONTROL;
		}	
		
		UART4Rx_index++;
		UART4Rx_index &= ~(1<<7); //keep index inside the limits
		
		// set the interrupt for UART3 Rx again
		HAL_UART_Receive_IT(&huart4, &UART4Rx_Buffer[UART4Rx_index], 1);
	}
		if (huart->Instance == UART5){ //current UART?
		
		if(flag_connect && UART5Rx_index > 4 ){

			if(UART5Rx_Buffer[UART5Rx_index-4] == 'H' && UART5Rx_Buffer[UART5Rx_index-3] == 'T' && UART5Rx_Buffer[UART5Rx_index-2] == 'T'&& UART5Rx_Buffer[UART5Rx_index-1] == 'P' && UART5Rx_Buffer[UART5Rx_index] == '/'){
				flag_send = 1;
				flag_connect = 0;
			}
		}
			
			UART5Rx_index++;
			UART5Rx_index &= ~(1<<9); //keep index inside the limits

	
			// set the interrupt for UART3 Rx again
			if(!flag_on){
			HAL_UART_Receive_IT(&huart5, &UART5Rx_Buffer[UART5Rx_index], 1);
		}
		}

	
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

void init_UART4(){
	// set the interrupt for UART3 Rx
	HAL_UART_Receive_IT(&huart4, &UART4Rx_Buffer[UART4Rx_index], 1);
}

void init_UART5(){
	// set the interrupt for UART3 Rx
	HAL_UART_Receive_IT(&huart5, &UART5Rx_Buffer[UART5Rx_index], 1);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
