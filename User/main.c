
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
void RCC_Configuration_t(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  ErrorStatus HSEStartUpStatus_t;
  HSEStartUpStatus_t = RCC_WaitForHSEStartUp();
  if (HSEStartUpStatus_t == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    /* PCLK2 = HCLK/2 */
    RCC_PCLK2Config(RCC_HCLK_Div1);
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
  }

  /* Enable GPIO and devices clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |   
                         RCC_APB2Periph_TIM1  | RCC_APB2Periph_USART1|  
                         RCC_APB2Periph_GPIOG | RCC_APB2Periph_ADC3  |
                         RCC_APB2Periph_AFIO  ,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3  | RCC_APB1Periph_SPI2  |
                         RCC_APB1Periph_TIM5  | RCC_APB1Periph_TIM4 |RCC_APB1Periph_I2C1,ENABLE);
}
void init_usart1(int rate)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = rate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);

  //使能串口1中断-接收数据完成中断
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启中断
  // 设置中断优先级-主函数中设置中断优先级分组
  NVIC_InitTypeDef NVIC_InitStrue;
  NVIC_InitStrue.NVIC_IRQChannel= USART1_IRQn;
  NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;//IRQ 通道使能
  NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级 1
  NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;//子优先级 1
  NVIC_Init(&NVIC_InitStrue);//中断优先级初始化
}

void uartsend_string(char *p,int n)
{
  for(int i=0;i<n;i++){
    USART_SendData(USART1,*p++);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
  }
}
void msleep(int num)
{
  for(int i=0;i<num;i++){
    for(int j=0;j<0x21FF;j++);
  }
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
    uartsend_string("task2",sizeof("task2")-1);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
static void vTask3(void *pvParameters)
{
  
}

static void AppTaskCreate (void);

/***************************************************************************
变量声明
***************************************************************************/

static TaskHandle_t xHandleTask1 = NULL;
static TaskHandle_t xHandleTask2 = NULL;
static TaskHandle_t xHandleTask3 = NULL;

static void AppTaskCreate (void)
{
  xTaskCreate( vTask1, "vTask1",512, NULL,1, &xHandleTask1);
  xTaskCreate( vTask2, "vTask2",512, NULL,1, &xHandleTask2);
  //xTaskCreate( vTask3, "vTask3",512, NULL,1, &xHandleTask3);
}

int main(void)
{
  RCC_Configuration_t();
  init_usart1(115200);
  
  /* 创建任务 */
  AppTaskCreate();
  /* 启动任务调度，开始执行任务 */
  vTaskStartScheduler();
}
