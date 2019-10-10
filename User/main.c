
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*FreeRTOSConfig.h �е����ó���configUSE_IDLE_HOOK ���붨��Ϊ1��������
�������Ӻ����Żᱻ����*/
unsigned long idlecount=0UL;
void vApplicationIdleHook(void)
{
  idlecount++;
  
}
static void vTask1(void *pvParameters)
{
  for(;;){
    uartsend_string("task1",sizeof("task1")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask2(void *pvParameters)
{
  while(1){
    uartsend_string(pvParameters,strlen(pvParameters));
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask3(void *pvParameters)
{
  for(;;){
    uartsend_string("task3\r\n",sizeof("task3\r\n")-1);
    uartsend_number(idlecount);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

static void AppTaskCreate (void);

/***************************************************************************
��������
***************************************************************************/

static TaskHandle_t xHandleTask1 = NULL;
static TaskHandle_t xHandleTask2 = NULL;
static TaskHandle_t xHandleTask3 = NULL;

static void AppTaskCreate (void)
{
  xTaskCreate( vTask1, "vTask1",512, "task1",2, &xHandleTask1);
  xTaskCreate( vTask2, "vTask2",512, "this is task2",3, &xHandleTask2);
  xTaskCreate( vTask3, "vTask3",512, NULL,1, &xHandleTask3);
}

int main(void)
{
  RCC_Configuration_t();
  init_usart1(115200);
  
  /* �������� */
  AppTaskCreate();
  /* ����������ȣ���ʼִ������ */
  vTaskStartScheduler();
}
