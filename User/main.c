
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/***************************************************************************
变量声明
***************************************************************************/
portBASE_TYPE xStatus;
xQueueHandle xQueue;
static TaskHandle_t xHandleTask1 = NULL;
static TaskHandle_t xHandleTask2 = NULL;
static TaskHandle_t xHandleTask3 = NULL;
static TaskHandle_t xHandleTask4 = NULL;
static TaskHandle_t xHandleTask5 = NULL;
static TaskHandle_t xHandleTask6 = NULL;
/*FreeRTOSConfig.h 中的配置常量configUSE_IDLE_HOOK 必须定义为1，这样空
闲任务钩子函数才会被调用*/
unsigned long idlecount=0UL;
void vApplicationIdleHook(void)
{
  idlecount++;
  
}
static void vTask5_sendqueue(void *pvParameters)
{
  long data=0;
  for(;;){
    uartsend_string("task5\r\n",sizeof("task5\r\n")-1);
    xStatus=xQueueSendToBack(xQueue,&data,0);
    data++;
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask6_recvqueue(void *pvParameters)
{
  long data=0;
  for(;;){
    uartsend_string("task6 recv is\r\n",sizeof("task6 recv is\r\n")-1);
    xStatus=xQueueReceive(xQueue,&data,100/portTICK_RATE_MS);//参数2为保存数据的缓冲区，参数三是阻塞超时时间
    uartsend_number(data);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask4(void *pvParameters)
{
  for(;;){
    uartsend_string("task4",sizeof("task4")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask1(void *pvParameters)
{
  for(;;){
    uartsend_string("task1\r\n",sizeof("task1\r\n")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
    xTaskCreate( vTask4, "vTask4",512, NULL,1, &xHandleTask4);
  }
}
static void vTask2(void *pvParameters)
{
  while(1){
    uartsend_string(pvParameters,strlen(pvParameters));
    uartsend_string("\r\n",sizeof("\r\n")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
    vTaskDelete(xHandleTask4);
  }
}
static void vTask3(void *pvParameters)
{
  unsigned portBASE_TYPE uxpriority; 
  for(;;){
    //uartsend_string("task3\r\n",sizeof("task3\r\n")-1);
    //uartsend_number(idlecount);
    uxpriority=uxTaskPriorityGet(NULL);
    uartsend_string("task3 priority is",sizeof("task3 priority is")-1);
    uartsend_number(uxpriority);
    uartsend_string("\r\n",sizeof("\r\n")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
    vTaskPrioritySet(xHandleTask3,uxpriority+1);
    uxpriority=uxTaskPriorityGet(NULL);
    uartsend_string("task3 priority is",sizeof("task3 priority is")-1);
    uartsend_number(uxpriority);
    uartsend_string("\r\n",sizeof("\r\n")-1);
    vTaskPrioritySet(NULL,uxpriority-1);
  }
}

static void AppTaskCreate(void)
{
  xTaskCreate( vTask1, "vTask1",512, "task1",2, &xHandleTask1);
  xTaskCreate( vTask2, "vTask2",512, "this is task2",2, &xHandleTask2);
  //xTaskCreate( vTask3, "vTask3",512, NULL,1, &xHandleTask3);
  xTaskCreate( vTask5_sendqueue, "vTask5",512, NULL,1, &xHandleTask5);
  xTaskCreate( vTask6_recvqueue, "vTask6",512, NULL,1, &xHandleTask6);
}

int main(void)
{
  RCC_Configuration_t();
  init_usart1(115200);
  xQueue=xQueueCreate(5,sizeof(long));
  /* 创建任务 */
  AppTaskCreate();
  /* 启动任务调度，开始执行任务 */
  vTaskStartScheduler();
}
