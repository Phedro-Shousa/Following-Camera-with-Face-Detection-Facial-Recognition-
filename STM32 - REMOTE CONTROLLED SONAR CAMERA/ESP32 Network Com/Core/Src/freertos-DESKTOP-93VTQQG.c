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

#define MANUAL 				6
#define SONAR					7
#define ADAPT_CONTROL	8

#define size_of_queue 8

#include "tim.h"
#include "usart.h"
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

	int servo_angle_x 		= 	500;	
	int servo_angle_y 		= 	500;
	uint8_t aux_rotation 	= 		0;
	
	const float speedOfSound = 0.0343/2;
	
	struct database{
		float sonar1_dist;
		float sonar2_dist;
		int servo_xposition;
		uint8_t mode;
	};
	struct database acquired_data;
	
	//criar duas etruturas em que têm uma distancia e um 
	//indice de forma a que o tempo em que as duas leituras 
	//foram feitas sejam comparado e verificados se são
	//ou não iguais
	struct distance{
		float sonar_dist;
		uint8_t ID;
		float vec_distances[8];
		int index_of_vec;
	};
	struct distance str_reading_sonar1;
	struct distance str_reading_sonar2;
	

	osThreadId myTask03Handle;
	osThreadId vTaskServoManageHandle;
	osThreadId vTaskReadSonar2Handle;
	
	xSemaphoreHandle Sem_ActivateServo = 0;
	
	osMutexId Mutex_SonarHandle;
xQueueHandle Q_sonar1_reading_handle;
xQueueHandle Q_sonar2_reading_handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osMutexId uartMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Delay(uint16_t i);

void read_manual_commands(void);
void read_distance1(int ID);
void read_distance2(int ID);
void vTaskServoManage(void const * argument);
void vTaskReadSonar2(void const * argument);
void adaptative_control(void);
void sonar_func(void);
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
	osMutexDef(Mutex_Sonar);
  Mutex_SonarHandle = osMutexCreate(osMutex(Mutex_Sonar));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
		vSemaphoreCreateBinary(Sem_ActivateServo);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	Q_sonar1_reading_handle = xQueueCreate(10, sizeof(str_reading_sonar1));
	Q_sonar2_reading_handle = xQueueCreate(10, sizeof(str_reading_sonar2));
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
  osThreadDef(Taskatuationservo, vTaskServoManage, osPriorityAboveNormal, 0, 128);
  vTaskServoManageHandle = osThreadCreate(osThread(Taskatuationservo), NULL);
 
	osThreadDef(Taskreadsonar2, vTaskReadSonar2, osPriorityRealtime, 0, 128);
  vTaskReadSonar2Handle = osThreadCreate(osThread(Taskreadsonar2), NULL);

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
    osDelay(1);
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
    osDelay(1);
  }
  /* USER CODE END Thread2 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */



/* USER CODE BEGIN Header_vTaskServoManage */
/**
* @brief Function implementing the vTaskServoManage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskServoManage */
void vTaskServoManage(void const * argument)
{
  /* USER CODE BEGIN vTaskServoManage */
  /* Infinite loop */
  for(;;)
  {
   //code thread
		if(xSemaphoreTake(Sem_ActivateServo, portMAX_DELAY)){
		htim4.Instance->CCR1 = acquired_data.servo_xposition;
	}
		osDelay(125);
}
  /* USER CODE END vTaskReadSonar */
}



/* USER CODE BEGIN Header_vTaskReadSonar2 */
/**
* @brief Function implementing the vTaskReadSonar2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskServoManage */
void vTaskReadSonar2(void const * argument)
{
  /* USER CODE BEGIN vTaskReadSonar2 */
	
	int ID = 0;
  /* Infinite loop */
  for(;;)
  {
   //code thread
		
		if(acquired_data.mode == ADAPT_CONTROL){   //it´s just necessary in adaptative mode
			//read distance 2
			read_distance2(ID);
		
			//send Queue to analyse distances
			if((str_reading_sonar2.sonar_dist < 400) && (str_reading_sonar2.sonar_dist> 1)){
				if(! xQueueSend(Q_sonar2_reading_handle,&str_reading_sonar2,1000)){
					xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
					printf("Failed when try send a message!\r\n");
					xSemaphoreGive(uartMutexHandle);
				}
		
			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
				WI_FI=0;
				printf("distance 2: %f \r\n", str_reading_sonar2.sonar_dist);
			xSemaphoreGive(uartMutexHandle);
			}
		}
		osDelay(125);	
}
  /* USER CODE END vTaskReadSonar2 */
}

void read_distance1(int ID){
	
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);	
		// Output 10 usec TRIG
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
		
		//Start timer of US1 
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
		//Wait for IC flag
		uint32_t startTick1 = HAL_GetTick();
		do
		{
			if(icFlag_1) break;
		}while((HAL_GetTick() - startTick1) < 500);  //500ms
		icFlag_1 = 0;
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);

		xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
		//Calculate distance in cm
		if(edge2Time_1 > edge1Time_1)
		{
			//acquired_data.sonar1_dist = ((edge2Time_1 - edge1Time_1) + 0.0f)*speedOfSound;
			str_reading_sonar1.sonar_dist = ((edge2Time_1 - edge1Time_1) + 0.0f)*speedOfSound;
			str_reading_sonar1.vec_distances[str_reading_sonar1.index_of_vec] = str_reading_sonar1.sonar_dist;
			str_reading_sonar1.index_of_vec++;
			str_reading_sonar1.index_of_vec &= 7;
		}
		else
		{
			//acquired_data.sonar1_dist = 0.0f;
			str_reading_sonar1.sonar_dist = 0.0f;	
		}
		xSemaphoreGive(Mutex_SonarHandle);
		
}


void read_distance2(int ID){
	
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);	
		// Output 10 usec TRIG
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
		
		//Start timer OF US2
		HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_1);
		//Wait for IC flag
		uint32_t startTick2 = HAL_GetTick();
		do
		{
			if(icFlag_2) break;
		}while((HAL_GetTick() - startTick2) < 500);  //500ms
		icFlag_2 = 0;
		HAL_TIM_IC_Stop_IT(&htim8, TIM_CHANNEL_1);
		
		
		//use of mutex to prevent the race conditions
		xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
		//Calculate distance in cm
		if(edge2Time_2 > edge1Time_2)
		{
		//acquired_data.sonar1_dist = ((edge2Time_1 - edge1Time_1) + 0.0f)*speedOfSound;
			str_reading_sonar2.sonar_dist = ((edge2Time_2 - edge1Time_2) + 0.0f)*speedOfSound;
			str_reading_sonar2.vec_distances[str_reading_sonar2.index_of_vec] = str_reading_sonar2.sonar_dist;
			str_reading_sonar2.index_of_vec++;
			str_reading_sonar2.index_of_vec &= 7;
		}
		else
		{
			str_reading_sonar2.sonar_dist = 0.0f;
		}
		xSemaphoreGive(Mutex_SonarHandle);
}

void adaptative_control(){
	
		// !!!!!servo_angle_x must be between 600-2400!!!!!
		int DiferenceBetweenDist=0;
		float actuation_value;
	
		if( (str_reading_sonar1.sonar_dist>30) &&  (str_reading_sonar2.sonar_dist>30)){
				actuation_value = 0.5;
		}else{
				actuation_value = 1;
		}
	
		DiferenceBetweenDist =  str_reading_sonar1.sonar_dist - str_reading_sonar2.sonar_dist;
	
		if( (DiferenceBetweenDist > 2) && (DiferenceBetweenDist < 10) ){ //obj closer to the sonar2
			//turn LEFT
			servo_angle_x = servo_angle_x + (100*actuation_value) ;
			if(servo_angle_x >= 2400) servo_angle_x = 2400;
		}else if( (DiferenceBetweenDist < -2)&&(DiferenceBetweenDist > -10)){ //obj closer to the sonar1
			//turn RIGHT
			servo_angle_x = servo_angle_x - (100*actuation_value);
			if(servo_angle_x <= 600) servo_angle_x = 600;
		}else if( DiferenceBetweenDist < -10 ){
			//turn RIGHT
			servo_angle_x = servo_angle_x - (200*actuation_value);
			if(servo_angle_x <= 600) servo_angle_x = 600;
		}else if(DiferenceBetweenDist > 10){
			//turn LEFT
			servo_angle_x = servo_angle_x + (200*actuation_value);
			if(servo_angle_x >= 2400) servo_angle_x = 2400;
		}
		
		acquired_data.servo_xposition = servo_angle_x;	
		xSemaphoreGive(Sem_ActivateServo);
		
}

void read_manual_commands(){

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
						if(servo_angle_x >= 2400) servo_angle_x = 2400;
						acquired_data.servo_xposition = servo_angle_x;	
						cmd_servos = ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServo);
			break;
		
		case	RIGHT_SERVO:
						servo_angle_x = servo_angle_x - 100;
						if(servo_angle_x <= 600) servo_angle_x = 600;
						acquired_data.servo_xposition = servo_angle_x;
						cmd_servos = ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServo);
			break;
	}
}

void sonar_func(){

			if((angle_found <= 2400) && !(aux_rotation) ){ //rotate to left
				angle_found = angle_found + 100;
			}else if(!(aux_rotation)){ 
				angle_found = 2400; 
				aux_rotation = 1; 
			}	
			if((angle_found >= 500) && (aux_rotation) ){	 //rotate to righ
				angle_found = angle_found - 100;
			}else if((aux_rotation)){ 
				angle_found = 500; 
				aux_rotation = 0;
			}
			acquired_data.servo_xposition = angle_found;
			htim4.Instance->CCR2 = 900;
			xSemaphoreGive(Sem_ActivateServo);
			
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
