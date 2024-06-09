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
#include "adc.h"
#include <math.h>

#define PI 3.14159265
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


//600 -> 960 -> 1320 -> 1680 -> 2040 
#define dist1 600
#define dist2 960
#define dist3 1320
#define dist4 1750
#define dist5 2200


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
	typedef struct distance{
		float sonar_dist;
		uint8_t ID;
		float vec_distances[8];
		int index_of_vec;
	}distance;
	distance distance_str_size;
		int luminusity; 
	
	float dist_comunication_send=0;

	osMutexId Mutex_SonarHandle;
	osThreadId vTaskReadLdrHandle;
/* USER CODE END Variables */
osThreadId vTaskDataAnalysHandle;
osThreadId vTaskReadSonarHandle;
osThreadId vTaskServoManagHandle;
osThreadId vTaskWirelessCoHandle;
osMessageQId Q_sonar1_readingHandle;
osMessageQId Q_update_interfaceHandle;
osMessageQId Q_sonar2_readingHandle;
osMutexId uartMutexHandle;
osSemaphoreId Sem_ActivateServoHandle;
osSemaphoreId Sem_LuminusityHandle;
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Delay(uint16_t i);
void read_manual_commands(void);
distance read_distance1(void);
distance read_distance2(void);
void vTaskServoManage(void const * argument);
void vTaskReadSonar2(void const * argument);
void vTaskWirelessComunication(void const * argument);
void adaptative_control(distance str_reading_sonar1, distance str_reading_sonar2);
void sonar_func(void);
/* USER CODE END FunctionPrototypes */

void thread1(void const * argument);
void thread2(void const * argument);
void thread3(void const * argument);
void thread4(void const * argument);
void thread5(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of Sem_ActivateServo */
  osSemaphoreDef(Sem_ActivateServo);
  Sem_ActivateServoHandle = osSemaphoreCreate(osSemaphore(Sem_ActivateServo), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* definition and creation of Sem_ActivateServo */
  osSemaphoreDef(Sem_send_luminusity);
  Sem_LuminusityHandle = osSemaphoreCreate(osSemaphore(Sem_send_luminusity), 1);
	
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of Q_sonar1_reading */
  osMessageQDef(Q_sonar1_reading, 15, distance);
  Q_sonar1_readingHandle = osMessageCreate(osMessageQ(Q_sonar1_reading), NULL);

  /* definition and creation of Q_update_interface */
  osMessageQDef(Q_update_interface, 15, distance);
  Q_update_interfaceHandle = osMessageCreate(osMessageQ(Q_update_interface), NULL);

  /* definition and creation of Q_sonar2_reading */
  osMessageQDef(Q_sonar2_reading, 15, distance);
  Q_sonar2_readingHandle = osMessageCreate(osMessageQ(Q_sonar2_reading), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of vTaskDataAnalys */
  osThreadDef(vTaskDataAnalys, thread1, osPriorityNormal, 0, 1024);
  vTaskDataAnalysHandle = osThreadCreate(osThread(vTaskDataAnalys), NULL);

  /* definition and creation of vTaskReadSonar */
  osThreadDef(vTaskReadSonar, thread2, osPriorityAboveNormal, 0, 1024);
  vTaskReadSonarHandle = osThreadCreate(osThread(vTaskReadSonar), NULL);

  /* definition and creation of vTaskServoManag */
  osThreadDef(vTaskServoManag, thread3, osPriorityNormal, 0, 256);
  vTaskServoManagHandle = osThreadCreate(osThread(vTaskServoManag), NULL);

  /* definition and creation of vTaskWirelessCo */
  osThreadDef(vTaskWirelessCo, thread4, osPriorityNormal, 0, 512);
  vTaskWirelessCoHandle = osThreadCreate(osThread(vTaskWirelessCo), NULL);
	
	 /* definition and creation of vTaskWirelessCo */
  osThreadDef(vTaskReadLdr, thread5, osPriorityNormal, 0, 256);
  vTaskReadLdrHandle = osThreadCreate(osThread(vTaskReadLdr), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_thread1 */
/**
  * @brief  Function implementing the vTaskDataAnalys thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_thread1 */
void thread1(void const * argument)
{
  /* USER CODE BEGIN thread1 */
		operation_mode = MANUAL; //initialy the system start as MANUAL mode
		distance str_reading_sonar1;
		distance str_reading_sonar2;
  /* Infinite loop */
  for(;;)
  {
    acquired_data.mode = operation_mode;
		switch(acquired_data.mode){
			case MANUAL:
				read_manual_commands();
				//xSemaphoreGive(Sem_LuminusityHandle);
				break;
			
			case SONAR:
				if(xQueueReceive(Q_sonar1_readingHandle, &str_reading_sonar1 , 50)){ //if recieve a distance 
					xQueueSend(Q_update_interfaceHandle,&str_reading_sonar1,200);				//update de interface
					sonar_func();																												//update positions
				}
				break;
				
			case ADAPT_CONTROL:
				//if(ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY)){
					if((xQueueReceive(Q_sonar1_readingHandle, &str_reading_sonar1 , 50)) && (xQueueReceive(Q_sonar2_readingHandle, &str_reading_sonar2 , 50))){
						xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
							adaptative_control(str_reading_sonar1, str_reading_sonar2);
						xSemaphoreGive(Mutex_SonarHandle);
					//xSemaphoreGive(Sem_LuminusityHandle);
					}
				//}
				break;
		}
  }
  /* USER CODE END thread1 */
}

/* USER CODE BEGIN Header_thread2 */
/**
* @brief Function implementing the vTaskReadSonar thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread2 */
void thread2(void const * argument)
{
  /* USER CODE BEGIN thread2 */
		distance str_reading_sonar1;
		distance str_reading_sonar2;
  /* Infinite loop */
  for(;;)
  {
	  acquired_data.mode = operation_mode;
		//code thread
		if(!(acquired_data.mode == MANUAL)){				//if mode is not manual
			str_reading_sonar1 =  read_distance1();		//read distance of sonar1 
			
			if((str_reading_sonar1.sonar_dist < 400) && (str_reading_sonar1.sonar_dist > 1)){  
				if(acquired_data.mode != MANUAL){
				//send Queue to analyse distances
					xQueueSend(Q_sonar1_readingHandle,&str_reading_sonar1,50);
				}	
			}
		}
		
		
		if(acquired_data.mode == ADAPT_CONTROL){   //it´s just necessary in adaptative mode
			//read distance 2
			str_reading_sonar2 = read_distance2();
		
			//send Queue to analyse distances
			if((str_reading_sonar2.sonar_dist < 400) && (str_reading_sonar2.sonar_dist> 1)){
				xQueueSend(Q_sonar2_readingHandle,&str_reading_sonar2,50);
				//if the code arrive here means that the adaptativer control can be executed
				// because the system already have 2 new distances prepared
				//xTaskNotifyGive(vTaskDataAnalysHandle);
			}
		}
		if(acquired_data.mode == SONAR){
			//osDelay(500);
			vTaskDelay(500);
		}else{
			//osDelay(20);
			vTaskDelay(25);
		}
		
		}
  /* USER CODE END thread2 */
}

/* USER CODE BEGIN Header_thread3 */
/**
* @brief Function implementing the vTaskServoManag thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread3 */
void thread3(void const * argument)
{
  /* USER CODE BEGIN thread3 */
  /* Infinite loop */
  for(;;)
  {
		if(xSemaphoreTake(Sem_ActivateServoHandle, portMAX_DELAY)){
			htim4.Instance->CCR1 = acquired_data.servo_xposition;
		}
  }
  /* USER CODE END thread3 */
}

/* USER CODE BEGIN Header_thread4 */
/**
* @brief Function implementing the vTaskWirelessCo thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread4 */
void thread4(void const * argument)
{
  /* USER CODE BEGIN thread4 */
	distance str_reading_sonar1;
	int angulo, x , y;
	float num;
  /* Infinite loop */
  for(;;)
  {
		if(xQueueReceive(Q_update_interfaceHandle, &str_reading_sonar1 , 200)){
			//600 -> 960 -> 1320 -> 1680 -> 2040
			

			
			num = (50 - str_reading_sonar1.sonar_dist);
				
			if(num<0){
				num = 0;
			}else if(num > 50){
				num = 50;
			}
				
			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
			if(acquired_data.servo_xposition > dist1 && acquired_data.servo_xposition < dist2){
				WI_FI=1;
				printf("*Y%f**R*", num );
			}else if(acquired_data.servo_xposition > dist2 && acquired_data.servo_xposition < dist3){
				WI_FI=1;
				printf("*U%f**R*", num );			
			}else if(acquired_data.servo_xposition > dist3 && acquired_data.servo_xposition < dist4){
				WI_FI=1;
				printf("*I%f**R*", num );
			}else if(acquired_data.servo_xposition > dist4 && acquired_data.servo_xposition < dist5){
				WI_FI=1;
				printf("*O%f**R*", num );
			}else if(acquired_data.servo_xposition > dist5){
				WI_FI=1;
				printf("*P%f**R*", num );
			}
				xSemaphoreGive(uartMutexHandle);
			
			if(angulo >= 175 || angulo <= 5){
				xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
	//		x+=5;
			WI_FI=2;
			printf("AT+CIPSEND=0,%d\r\n",49);
			osDelay(100);
			
			WI_FI=2;
			printf("<circle cx=\"250\" cy=\"300\" r=\"250\"></circle>          ");
			osDelay(100);
			xSemaphoreGive(uartMutexHandle);
			}
angulo = ((180*(acquired_data.servo_xposition - 400))/2000);
if(angulo <= 90){
x = 250 - (str_reading_sonar1.sonar_dist*2.5 * cos((angulo*PI)/180));
}
else{x = 250 + (str_reading_sonar1.sonar_dist*2.5 * cos((angulo*PI)/180))*(-1);}

y =  300-(str_reading_sonar1.sonar_dist*2.5 * sin((angulo*PI)/180)) ;
			
			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
	//		x+=5;
			WI_FI=2;
			printf("AT+CIPSEND=0,%d\r\n",97);
			osDelay(100);
			
			WI_FI=2;
			printf("<line x1=\"%d\" y1=\"%d\" x2=\"250\" y2=\"300\" style=\"stroke:rgb(0,255,0);stroke-width:2\" />          ", x, y);
			osDelay(100);
			xSemaphoreGive(uartMutexHandle);
			
		}
  }
  /* USER CODE END thread4 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE BEGIN Header_thread5 */
/**
* @brief Function implementing the vTaskReadLDR thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread5 */
void thread5(void const * argument)
{
  /* USER CODE BEGIN thread5 */

  /* Infinite loop */
  for(;;)
  {
		if(xSemaphoreTake(Sem_LuminusityHandle, portMAX_DELAY)){
			HAL_ADC_Start(&hadc3);
			luminusity = HAL_ADC_GetValue(&hadc3);
			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
			WI_FI=1;
			printf("*R%d*", 4096 - luminusity);
			xSemaphoreGive(uartMutexHandle);
			osDelay(3000);
		}
  }
  /* USER CODE END thread5 */
}


distance read_distance1(){
	
		distance str_reading_sonar1;
	
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
		}while((HAL_GetTick() - startTick1) < 100);  //50ms
		icFlag_1 = 0;
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);

		xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
		//Calculate distance in cm
		if(edge2Time_1 > edge1Time_1)
		{
			str_reading_sonar1.sonar_dist = ((edge2Time_1 - edge1Time_1) + 0.0f)*speedOfSound;
		}
		else
		{
			//acquired_data.sonar1_dist = 0.0f;
			str_reading_sonar1.sonar_dist = 0.0f;	
		}
		xSemaphoreGive(Mutex_SonarHandle);
		
		return str_reading_sonar1;
		
}


distance read_distance2(){
	
		distance str_reading_sonar2;
	
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
		}while((HAL_GetTick() - startTick2) < 100);  //50ms
		icFlag_2 = 0;
		HAL_TIM_IC_Stop_IT(&htim8, TIM_CHANNEL_1);
		
		
		//use of mutex to prevent the race conditions
		xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
		//Calculate distance in cm
		if(edge2Time_2 > edge1Time_2)
		{
			str_reading_sonar2.sonar_dist = ((edge2Time_2 - edge1Time_2) + 0.0f)*speedOfSound;
		}
		else
		{
			str_reading_sonar2.sonar_dist = 0.0f;
		}
		xSemaphoreGive(Mutex_SonarHandle);
		
		return str_reading_sonar2;
}


void adaptative_control(distance str_reading_sonar1, distance str_reading_sonar2)
{
   	// !!!!!servo_angle_x must be between 600-2400!!!!!
    float DiferenceBetweenDist = 0;
    float actuation_value;

    if ((str_reading_sonar1.sonar_dist > 30) && (str_reading_sonar2.sonar_dist > 30))
    {
        actuation_value = 0.5;
    }
    else
    {
        actuation_value = 1;
    }

    DiferenceBetweenDist = str_reading_sonar1.sonar_dist - str_reading_sonar2.sonar_dist;

    if ((DiferenceBetweenDist > 1) && (DiferenceBetweenDist < 7))
    {
    	//obj closer to the sonar2
       	//turn LEFT
        servo_angle_x = servo_angle_x + (40 *actuation_value);
        if (servo_angle_x >= 2400) servo_angle_x = 2400;
    }
    else if ((DiferenceBetweenDist < -1) && (DiferenceBetweenDist > -7))
    {
    	//obj closer to the sonar1
       	//turn RIGHT
        servo_angle_x = servo_angle_x - (40 *actuation_value);
        if (servo_angle_x <= 600) servo_angle_x = 600;
    }
    else if (DiferenceBetweenDist < -7)
    {
       	//turn RIGHT
        servo_angle_x = servo_angle_x - (80 *actuation_value);
        if (servo_angle_x <= 600) servo_angle_x = 600;
    }
    else if (DiferenceBetweenDist > 7)
    {
       	//turn LEFT
        servo_angle_x = servo_angle_x + (80 *actuation_value);
        if (servo_angle_x >= 2400) servo_angle_x = 2400;
    }

    acquired_data.servo_xposition = servo_angle_x;
    xSemaphoreGive(Sem_ActivateServoHandle);

}

void read_manual_commands(){

		switch(cmd_servos){
		case	ZOMBIE_SERVO:
			break;
		
		case	UP_SERVO:
						servo_angle_y = servo_angle_y + 200;
						if(servo_angle_y > 1200) servo_angle_y = 1200;
				//		htim4.Instance->CCR2 = servo_angle_y;
						cmd_servos =ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServoHandle);
			break;
		
		case	DOWN_SERVO:
						servo_angle_y = servo_angle_y - 200;
						if(servo_angle_y < 600) servo_angle_y = 600;
				//		htim4.Instance->CCR2 = servo_angle_y;
						cmd_servos = ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServoHandle);
			break;
		
		case	LEFT_SERVO:	
						servo_angle_x = servo_angle_x + 100;
						if(servo_angle_x >= 2400) servo_angle_x = 2400;
						acquired_data.servo_xposition = servo_angle_x;	
						cmd_servos = ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServoHandle);
			break;
		
		case	RIGHT_SERVO:
						servo_angle_x = servo_angle_x - 100;
						if(servo_angle_x <= 600) servo_angle_x = 600;
						acquired_data.servo_xposition = servo_angle_x;
						cmd_servos = ZOMBIE_SERVO;
						xSemaphoreGive(Sem_ActivateServoHandle);
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
			xSemaphoreGive(Sem_ActivateServoHandle);
			
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
