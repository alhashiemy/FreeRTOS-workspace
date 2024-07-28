/*
 * Author : BASSAM_MAHMOUD_
 * Layer  : APP
 * SWC    : RTOS
 * Version: 1.0
 * Created: JUN/11/2024
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "BIT_MATH.h"
#include "STD_TYPES.h"

#include "PORT_interface.h"

#include "CLCD_interface.h"
#include "KPD_interface.h"



u8 APP_u8PinRead[2] = {0};

void KPD_voidTask(void *pv_parameters);
void CLCD_voidTask(void *pv_parameters);

TaskHandle_t *KPD_Task_handler=NULL;
TaskHandle_t *CLCD_Task_handler=NULL;
SemaphoreHandle_t xCLCD_Semaphore = NULL;


int main()
{
	Port_voidInit();
	CLCD_voidInit();
	
	xCLCD_Semaphore = xSemaphoreCreateBinary();
	
	/* Create the task, storing the handle. */
	
    xTaskCreate(KPD_voidTask, "KPD_Task", 100, APP_u8PinRead, 3, KPD_Task_handler ); 
	
	xTaskCreate(CLCD_voidTask, "CLCD_Task", 100, APP_u8PinRead, 3, CLCD_Task_handler );
	
	vTaskStartScheduler();
	
	while(1)
	{	
	}
}


void KPD_voidTask(void *pv_parameters)
{
	portTickType xLastWakeTime;
    
    // The variable must be initialized this way once before use!
    xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		
		if(KPD_u8GetNum(pv_parameters) == CLEAR)
		{
			/* GIVE Semaphore */
			xSemaphoreGive( xCLCD_Semaphore );
			
		}
        vTaskDelayUntil(&xLastWakeTime, (20 / portTICK_RATE_MS));

	}
}

void CLCD_voidTask(void *pv_parameters)
{
	while(1)
	{
		if(xSemaphoreTake(xCLCD_Semaphore, portMAX_DELAY));	//Block task while semaphor is not taken.
		{
			CLCD_u8SendString(pv_parameters);
		}
	}
}