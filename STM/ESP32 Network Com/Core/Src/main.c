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
#include "cmsis_os.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f7xx_hal.h"
#include "math.h"
#define PI 3.14159265
char webpage[] = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Display Webcam Stream</title><style>#container {margin: 0px auto;width: 500px;height: 300px;border: 10px #333 solid;}#videoElement {width: 500px;height: 300px;background-color: #666;}</style></head><body><div id=\"container\"><video autoplay=\"true\" id=\"videoElement\"></video></div><script>var video = document.querySelector(\"#videoElement\");if (navigator.mediaDevices.getUserMedia) {navigator.mediaDevices.getUserMedia({ video: true }).then(function (stream) {video.srcObject = stream;}).catch(function (err0r) {console.log(\"Something went wrong!\");});}</script>";
char webpage2[] = "<svg width=\"900\" height=\"305\"><rect width=\"1000\" height=\"700\" style=\"fill:rgb(253,253,253);stroke-width:3;stroke:rgb(253,253,253)\" /><circle cx=\"250\" cy=\"300\" r=\"250\" stroke=\"green\" stroke-width=\"15\"></circle><line x1=\"250\" y1=\"50\" x2=\"250\" y2=\"300\" style=\"stroke:rgb(0,255,0);stroke-width:2\" /></body></html>";
unsigned int l;
double teste;
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void config_esp8266(){

	WI_FI=2;
	printf("AT+RST\r\n");
	HAL_Delay(1000);
	
	WI_FI=2;
	printf("AT+CWMODE=2\r\n");
	HAL_Delay(100);

	WI_FI=2;
	printf("AT+CWSAP=\"PIPI\",\"12345678\",3,3\r\n");
	HAL_Delay(100);
	
	WI_FI=2;
	printf("AT+CIPMUX=1\r\n");
	HAL_Delay(100);

	WI_FI=2;
	printf("AT+CIPSERVER=1,80\r\n");
	HAL_Delay(100);

	WI_FI=2;
	printf("AT+CIPSTO=300\r\n");
	HAL_Delay(100);


}
void send(char webpage1[]){

	WI_FI=2;
	printf("AT+CIPSEND=0,%d\r\n",l);
	HAL_Delay(100);	
	WI_FI=2;
	printf("%s                    \r\n", webpage1);
	HAL_Delay(2000);	
}
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
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_UART4_Init();
  MX_ADC3_Init();
	MX_UART5_Init();
  /* USER CODE BEGIN 2 */
	MX_UART4_Init();								//UART 	-> communication with interface module
	init_UART3();										//UART3 -> serial Port for the tests
	init_UART4();										//prepare the serial port to recieve
	init_UART5();
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); //Timer 4 in PWM mode to controle the servos
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	
	HAL_ADC_Start(&hadc3);
	
	user_ticks = 0;									//reset the global variable that is used in ultrasom module
	htim4.Instance->CCR1 	= 	500;	//is pre-defined the positions of x-axis and y-axis
	cmd_servos = 0;
	WI_FI = 0;											//define wich is the UART that we are use
	angle_found = 500;							//define the first angle by default
	
	config_esp8266();
	flag_connect = 1;
	while(!flag_send){}
	HAL_Delay(200);
		
	l=sizeof(webpage);
	send(webpage);
	l=sizeof(webpage2);
	send(webpage2);

	flag_send=0;
	flag_connect = 0;
	flag_on =1;
		//HAL_UART_AbortReceive_IT(&huart5);
		
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init(); 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
    /* USER CODE END WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_UART4;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_HSI;
  PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
