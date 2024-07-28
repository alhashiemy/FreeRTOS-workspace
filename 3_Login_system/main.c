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


const char* Password = "1234";
const char* Login = "login success";
const char* LCD_clear = "\0";

void KPD_voidTask(void *pv_parameters);
void CLCD_voidTask(void *pv_parameters);
void LOGIN_voidTask(void *pv_parameters);


TaskHandle_t KPD_Task_handler  	= NULL;
TaskHandle_t CLCD_Task_handler 	= NULL;
TaskHandle_t LOGIN_Task_handler = NULL;


static xQueueHandle xQueue = NULL;

int main()
{
	Port_voidInit();
	CLCD_voidInit();
	
	/* CREATE QUEUE */
	xQueue = xQueueCreate(1, sizeof(char));
	
	/* Create the task, storing the handle. */
	
    xTaskCreate(KPD_voidTask, "KPD_Task", 100, (void*)0, 1, KPD_Task_handler ); 
	
	xTaskCreate(CLCD_voidTask, "CLCD_Task", 100, (void*)0, 2, CLCD_Task_handler );
	
	xTaskCreate(LOGIN_voidTask, "LOGIN_Task", 100, (void*)0, 3, LOGIN_Task_handler );

	
	vTaskStartScheduler();
	
	while(1)
	{	
	}
}


void KPD_voidTask(void *pv_parameters)
{
	char* lValueToSend = " ";
	
    // The variable must be initialized this way once before use!
	portTickType xLastWakeTime = xTaskGetTickCount();
    
	while(1)
	{
		
		if(KPD_u8GetNum(lValueToSend) == CLEAR)
		{
			/* GIVE Semaphore */
			xQueueSendToBack(xQueue, &lValueToSend, 0);
		}
        vTaskDelayUntil(&xLastWakeTime, (20 / portTICK_RATE_MS));

	}
}

void CLCD_voidTask(void *pv_parameters)
{
	char* lReceivedValue = NULL;
	
	
	while(1)
	{
		if (uxQueueMessagesWaiting(xQueue) != 0) {
			
			CLCD_u8SendString("Queue should have been empty!\n");
		}
		
		//Block task while semaphor is not taken.
		if(xQueueReceive(xQueue, &lReceivedValue, portMAX_DELAY) == pdPASS) {
			if(lReceivedValue[0] == '\0') {
				CLCD_voidClr();
			}
			else {
				
				CLCD_u8SendString(lReceivedValue);
			}
		}
	}
}

void LOGIN_voidTask(void *pv_parameters)
{
	u8 i, PasswordCounter;
	char* lPeekedValue = NULL;
	
	for(;;)
	{

		PasswordCounter = 0;
		for(i=0; i<4; i++)
		{
			xQueuePeek(xQueue, &lPeekedValue, portMAX_DELAY);
			
			if(lPeekedValue[0] == Password[i]) {
				
				PasswordCounter++;
			}
			vTaskDelay(10 / portTICK_RATE_MS);

		}
		/* check if login has been done */
		if(PasswordCounter == 4) {
			
			xQueueSendToBack(xQueue, &LCD_clear , 0);
			vTaskDelay(1 / portTICK_RATE_MS);

			
			/* login success */
			xQueueSendToBack(xQueue, &Login , 0);

		}
	}
}