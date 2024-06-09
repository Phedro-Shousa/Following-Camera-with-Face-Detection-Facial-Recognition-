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


//600 -> 960 -> 1320 -> 1680 -> 2040 
#define dist1 600
#define dist2 960
#define dist3 1320
#define dist4 1680
#define dist5 2040


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
	

	osThreadId myTask03Handle;
	osThreadId vTaskServoManageHandle;
	osThreadId vTaskReadSonar2Handle;
	osThreadId vTaskWirelessComunicationHandle;
	
	xSemaphoreHandle Sem_ActivateServo = 0;
	xQueueHandle Q_update_interface_handle = 0;
	osMutexId Mutex_SonarHandle;
	
/* USER CODE END Variables */
	osThreadId defaultTaskHandle;
	osThreadId myTask02Handle;
	osMutexId uartMutexHandle;
	xQueueHandle Q_sonar1_reading_handle = 0;
	xQueueHandle Q_sonar2_reading_handle = 0;
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Delay(uint16_t i);

void read_manual_commands(void);
distance read_distance1();
distance read_distance2();
void vTaskServoManage(void const * argument);
void vTaskReadSonar2(void const * argument);
void vTaskWirelessComunication(void const * argument);
void adaptative_control(struct distance str_reading_sonar1, struct distance str_reading_sonar2);
void sonar_func(void);
/* USER CODE END FunctionPrototypes */

void vTaskDataAnalyse(void const * argument);
void vTaskReadSonar(void const * argument);

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
	Q_sonar1_reading_handle = xQueueCreate(10, sizeof(distance));
	Q_sonar2_reading_handle = xQueueCreate(10, sizeof(distance));
	Q_update_interface_handle = xQueueCreate(10, sizeof(distance));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, vTaskDataAnalyse, osPriorityAboveNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, vTaskReadSonar, osPriorityAboveNormal, 0, 1024);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(Taskatuationservo, vTaskServoManage, osPriorityAboveNormal, 0, 1024);
  vTaskServoManageHandle = osThreadCreate(osThread(Taskatuationservo), NULL);
 
	osThreadDef(Taskreadsonar2, vTaskReadSonar2, osPriorityAboveNormal, 0, 1024);
  vTaskReadSonar2Handle = osThreadCreate(osThread(Taskreadsonar2), NULL);
	
	osThreadDef(TaskComunication, vTaskWirelessComunication, osPriorityAboveNormal, 0, 1024);
  vTaskWirelessComunicationHandle = osThreadCreate(osThread(TaskComunication), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_vTaskDataAnalyse */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_vTaskDataAnalyse */
void vTaskDataAnalyse(void const * argument)
{
  /* USER CODE BEGIN vTaskDataAnalyse */
	operation_mode = MANUAL; //initialy the system start as MANUAL mode
	int message_receive =0;
	struct distance str_reading_sonar1;
	struct distance str_reading_sonar2;
	
  /* Infinite loop */
  for(;;)
  {
		acquired_data.mode = operation_mode;
		switch(acquired_data.mode){
			case MANUAL:
				read_manual_commands();
				break;
			case SONAR:
				if(xQueueReceive(Q_sonar1_reading_handle, &str_reading_sonar1 , 200)){  //if recieve a distance 
				message_receive = 1;
				}
				if(message_receive){
				xQueueSend(Q_update_interface_handle,&str_reading_sonar1,200);					//update de interface
				sonar_func();																														//update sonar positions
				message_receive = 0;
				
				//600 -> 960 -> 1320 -> 1680 -> 2040 
				float num = (30 - str_reading_sonar1.sonar_dist);
				
				if(num<0){
				num = 0;
				}else if(num > 30){
				num = 30;
				}
				xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
				if(acquired_data.servo_xposition > dist1 && acquired_data.servo_xposition < dist2){
				WI_FI=1;
				printf("*Y%f*", num );
				}else if(acquired_data.servo_xposition > dist2 && acquired_data.servo_xposition < dist3){
				WI_FI=1;
				printf("*U%f*", num );			
				}else if(acquired_data.servo_xposition > dist3 && acquired_data.servo_xposition < dist4){
				WI_FI=1;
				printf("*If*", num );
				}else if(acquired_data.servo_xposition > dist4 && acquired_data.servo_xposition < dist5){
				WI_FI=1;
				printf("*O%f*", num );
				}else if(acquired_data.servo_xposition < dist5){
				WI_FI=1;
				printf("*P%f*", num );
				}
				xSemaphoreGive(uartMutexHandle);
//				xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//				WI_FI=1;
//				printf("*P%f*", num);
//				xSemaphoreGive(uartMutexHandle);
				
				}
				break;
			case ADAPT_CONTROL:
				if((xQueueReceive(Q_sonar1_reading_handle, &str_reading_sonar1 , 200)) && (xQueueReceive(Q_sonar2_reading_handle, &str_reading_sonar2 , 1000))){
						xSemaphoreTake(Mutex_SonarHandle, portMAX_DELAY);
						adaptative_control(str_reading_sonar1, str_reading_sonar2);
						xSemaphoreGive(Mutex_SonarHandle);
				}
				break;
		}
		//osDelay(250);
  }
  /* USER CODE END vTaskDataAnalyse */
}

/* USER CODE BEGIN Header_vTaskReadSonar */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskReadSonar */
void vTaskReadSonar(void const * argument)
{
  /* USER CODE BEGIN vTaskReadSonar */
	//int index = 0;
	int ID = 0;
	struct distance str_reading_sonar1;
	struct distance str_reading_sonar2;
  /* Infinite loop */
  for(;;)
  {
	  acquired_data.mode = operation_mode;
   //code thread
		if(!(acquired_data.mode == MANUAL)){				//if mode is not manual
																						
			str_reading_sonar1 =  read_distance1();				//read distance of sonar1 
			
		if((str_reading_sonar1.sonar_dist < 400) && (str_reading_sonar1.sonar_dist > 1)){  
			if(acquired_data.mode != MANUAL){
			//send Queue to analyse distances
				if(! xQueueSend(Q_sonar1_reading_handle,&str_reading_sonar1,200)){
					xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
					WI_FI=0;
					printf("Failed when try send a message!\r\n");
					xSemaphoreGive(uartMutexHandle);
				}
			}
			
				//protection the validation of distances
				//and printing the distances that were read
				//xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
				//WI_FI=0;
				//printf("distance 1: %f \r\n", str_reading_sonar1.sonar_dist);
				//xSemaphoreGive(uartMutexHandle);			
			}
		}
		
		
				if(acquired_data.mode == ADAPT_CONTROL){   //it´s just necessary in adaptative mode
			//read distance 2
			str_reading_sonar2 = read_distance2();
		
			//send Queue to analyse distances
			if((str_reading_sonar2.sonar_dist < 400) && (str_reading_sonar2.sonar_dist> 1)){
				if(! xQueueSend(Q_sonar2_reading_handle,&str_reading_sonar2,200)){
					xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
					WI_FI=0;
					printf("Failed when try send a message!\r\n");
					xSemaphoreGive(uartMutexHandle);
				}
		
//			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//				WI_FI=0;
//				printf("distance 2: %f \r\n", str_reading_sonar2.sonar_dist);
//			xSemaphoreGive(uartMutexHandle);
			}
		}
		
		osDelay(200);
}
  /* USER CODE END vTaskReadSonar */
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
		//osDelay(250);
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
	struct distance str_reading_sonar2;
	int ID = 0;
  /* Infinite loop */
  for(;;)
  {
   //code thread
		
//		if(acquired_data.mode == ADAPT_CONTROL){   //it´s just necessary in adaptative mode
//			//read distance 2
//			read_distance2(str_reading_sonar2);
//		
//			//send Queue to analyse distances
//			if((str_reading_sonar2.sonar_dist < 400) && (str_reading_sonar2.sonar_dist> 1)){
//				if(! xQueueSend(Q_sonar2_reading_handle,&str_reading_sonar2,1000)){
//					xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//					WI_FI=0;
//					printf("Failed when try send a message!\r\n");
//					xSemaphoreGive(uartMutexHandle);
//				}
//		
////			xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
////				WI_FI=0;
////				printf("distance 2: %f \r\n", str_reading_sonar2.sonar_dist);
////			xSemaphoreGive(uartMutexHandle);
//			}
//		}
		osDelay(250);	
}
  /* USER CODE END vTaskReadSonar2 */
}

/* USER CODE BEGIN Header_vTaskWirelessComunication */
/**
* @brief Function implementing the vTaskWirelessComunication thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskWirelessComunication */
void vTaskWirelessComunication(void const * argument)
{
  /* USER CODE BEGIN vTaskReadSonar2 */
	struct distance str_reading_sonar1;
	int position, position_ant, inc;
	float x_antgraph,y_antgraph, x_graph, y_graph;
	x_graph=1;
	y_graph=0;
	inc = 0;
  /* Infinite loop */
  for(;;)
  {
   //code thread
			if(xQueueReceive(Q_update_interface_handle, &str_reading_sonar1 , 200)){
	
				//600 -> 960 -> 1320 -> 1680 -> 2040 
				float num = (30 - str_reading_sonar1.sonar_dist);
				
				if(num<0){
				num = 0;
				}else if(num > 30){
				num = 30;
				}
				
				xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
				WI_FI=1;
				printf("*P%f*", num);
				xSemaphoreGive(uartMutexHandle);
				
//				//in position 1
//				if((acquired_data.servo_xposition>dist1) && (acquired_data.servo_xposition<dist2) && position!=1){
//					position = 1;
//					y_graph = num;
//				}else if((acquired_data.servo_xposition>dist2) && (acquired_data.servo_xposition<dist3) && position!=2){
//					position = 2;
//					y_graph = num;
//				}else if((acquired_data.servo_xposition>dist3) && (acquired_data.servo_xposition<dist4) && position!=3){
//					position = 3;
//					y_graph = num;
//				}else if((acquired_data.servo_xposition>dist4) && (acquired_data.servo_xposition<dist5) && position!=4){
//					position = 4;
//					y_graph = num;
//				}else if((acquired_data.servo_xposition>dist5) && position!=5){
//					position = 5;
//					y_graph = num;
//				}
//					
//					if(position != position_ant){
//						if(x_graph < 6 && inc){
//							xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//							WI_FI=1;
//							printf("*PX%fY%f*",x_graph,y_graph);
//							xSemaphoreGive(uartMutexHandle);
//							x_graph++;
//						}else if(x_graph < 6 && !(inc)){
//							xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//							WI_FI=1;
//							printf("*PX%fY%f*",x_graph,y_graph);
//							xSemaphoreGive(uartMutexHandle);
//							x_graph--;
//						}
//						if((position == 5 && inc)|| (position == 1 && !(inc))){
//							inc = !(inc);
//						}
//					}else if( (position == position_ant) && (position == 1 || position == 5)){
//							xSemaphoreTake(uartMutexHandle, portMAX_DELAY);
//							WI_FI=1;
//							printf("*PC*");
//							xSemaphoreGive(uartMutexHandle);
//					}
//					
//					position_ant = position;
					
					
//				WI_FI=1;
//				printf("*P%f*",num);
//				if(acquired_data.servo_xposition > dist1 && acquired_data.servo_xposition < dist2){
//				WI_FI=1;
//				printf("*Y%f*", num );
//				}else if(acquired_data.servo_xposition > dist2 && acquired_data.servo_xposition < dist3){
//				WI_FI=1;
//				printf("*U%f*", num );			
//				}else if(acquired_data.servo_xposition > dist3 && acquired_data.servo_xposition < dist4){
//				WI_FI=1;
//				printf("*If*", num );
//				}else if(acquired_data.servo_xposition > dist4 && acquired_data.servo_xposition < dist5){
//				WI_FI=1;
//				printf("*O%f*", num );
//				}else if(acquired_data.servo_xposition < dist5){
//				WI_FI=1;
//				printf("*P%f*", num );
//				}
		}
}
  /* USER CODE END vTaskReadSonar2 */
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
		}while((HAL_GetTick() - startTick1) < 200);  //500ms
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
		}while((HAL_GetTick() - startTick2) < 200);  //500ms
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
		
		return str_reading_sonar2;
}

void adaptative_control(struct distance str_reading_sonar1, struct distance str_reading_sonar2){
	
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
