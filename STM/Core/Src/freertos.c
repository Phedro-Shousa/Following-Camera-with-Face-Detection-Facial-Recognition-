/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#define ZOMBIE_SERVO	0
#define RIGHT_SERVO		1
#define LEFT_SERVO		2
#define UP_SERVO			3
#define DOWN_SERVO		4
#define FIND_MODE			5
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
/* USER CODE BEGIN Variables */

	uint32_t time, time_copy;
	float distance 				= 		0;
	int servo_angle_x 		= 	500;	
	int servo_angle_y 		= 	500;
	uint8_t aux_rotation 	= 		0;
	char byte_wifi[] = "AT\r\n";
	
//	user_ticks = 0;


//	cmd_servos = ZOMBIE_SERVO;

//	WI_FI = 0;
//	printf("STM INITIALIZED! \n\r");
//	angle_found = 500;
	
osThreadId myTask03Handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osMutexId uartMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
void Thread3(void const * argument);
/* USER CODE END FunctionPrototypes */

void thread1(void const * argument);
void Thread2(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of uartMutex */
  osMutexDef(uartMutex);
  uartMutexHandle = osMutexCreate(osMutex(uartMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, thread1, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, Thread2, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(myTask03, Thread3, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_thread1 */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_thread1 */
void thread1(void const * argument)
{
  /* USER CODE BEGIN thread1 */
  /* Infinite loop */
  for(;;)
  {
		//code thread
		switch(cmd_servos){
		case	ZOMBIE_SERVO:
			break;
		case	UP_SERVO:
						servo_angle_y = servo_angle_y + 100;
						if(servo_angle_x > 1000) servo_angle_x = 1000;
						htim4.Instance->CCR2 = servo_angle_y;
						cmd_servos =ZOMBIE_SERVO;
			
			break;
		case	DOWN_SERVO:
						servo_angle_y = servo_angle_y - 100;
						if(servo_angle_x < 800) servo_angle_x = 800;
						htim4.Instance->CCR2 = servo_angle_y;
						cmd_servos = ZOMBIE_SERVO;
			
			break;
		case	LEFT_SERVO:	
						servo_angle_x = servo_angle_x + 100;
						if(servo_angle_x > 2500) servo_angle_x = 2500;
						htim4.Instance->CCR1 = servo_angle_x;
						cmd_servos = ZOMBIE_SERVO;
			break;
		case	RIGHT_SERVO:
						servo_angle_x = servo_angle_x - 100;
							if(servo_angle_x < 500) servo_angle_x = 500;
						htim4.Instance->CCR1 = servo_angle_x;
					cmd_servos = ZOMBIE_SERVO;
			break;
		case	FIND_MODE:
			if((angle_found <= 2500) && !(aux_rotation) ){ //rotate to left
				angle_found = angle_found + 100;
			}else if(!(aux_rotation)){ 
				angle_found = 2500; 
				aux_rotation = 1; 
			}
			
			if((angle_found >= 500) && (aux_rotation) ){	 //rotate to righ
				angle_found = angle_found - 100;
			}else if((aux_rotation)){ 
				angle_found = 500; 
				aux_rotation = 0;
			}
			
			htim4.Instance->CCR1 = angle_found;
			htim4.Instance->CCR2 = 900;
		break;
	}
		xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
    //inside mutex
		xSemaphoreGive(uartMutexHandle);
		osDelay(100);
  }
  /* USER CODE END thread1 */
}

/* USER CODE BEGIN Header_Thread2 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Thread2 */
void Thread2(void const * argument)
{
  /* USER CODE BEGIN Thread2 */
  /* Infinite loop */
  for(;;)
  {
   //code thread
		xTaskNotify(myTask03Handle, 0x01, eSetBits);  //signal other threads(flag nº1)
		xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
    //inside mutex
		xSemaphoreGive(uartMutexHandle);
		osDelay(1000);
  }
  /* USER CODE END Thread2 */
}


void Thread3(void const * argument)
{
  /* USER CODE BEGIN Thread3 */
	uint32_t notifValue;
  /* Infinite loop */
  for(;;)
  {
   //code thread
		xTaskNotifyWait(pdFALSE, 0xFF, &notifValue, portMAX_DELAY);
		if((notifValue & 0x01) != 0x00) //check if notValue has 0x01
		{ //signal detected
		xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
    //inside mutex
		xSemaphoreGive(uartMutexHandle);
		}
			
  }
  /* USER CODE END Thread3 */
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
void Delay(uint16_t i){
	
		static uint32_t j=0, ij=0;
		for(ij=0;ij<i;ij++){
			for(j=0;j<1;j++);
		}
		
}

void read_distance(){
uint32_t time = 0;
uint32_t time_copy = 0;
float distance;
	
			Delay(1); //2us
			GPIOB->BSRR = GPIO_PIN_8;									//B08 --> 1
			Delay(5); //10us
			GPIOB->BSRR 
	=(uint32_t)GPIO_PIN_8<< 16;
			while((GPIOB->IDR & GPIO_PIN_9) == 0x00){}
			time=0;		
			while((GPIOB->IDR & GPIO_PIN_9) != 0x00){
				time++;
				Delay(1); //2us
			}
			time_copy = time;
			distance = (float)time_copy*0.041668*2;
			Delay(500);
			printf("distance: %f \n\r", distance);
	
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
