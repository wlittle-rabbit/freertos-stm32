
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
void uartsend_char(char a)
{
  USART_SendData(USART1,a);
  while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
}
void uartsend_string(char *p,int n)
{
  for(int i=0;i<n;i++){
    USART_SendData(USART1,*p++);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
  }
}
char int_to_char(int a)
{
  return a+'0';
}
int get_number_length(unsigned long n)
{
  int i=0;
  int t=1;
  if(n==0||n==1)
    return 1;
  while(n>t){
    t=t*10;
    i++;
  }
  return i;
}
/*10的num次方*/
int ten_x_y(int num)
{
  int result=1;
  for(int i=0;i<num;i++)
    result*=10;
  return result; 
}
void uartsend_number(unsigned long n)
{
  char c=0;
  int tmp1=n;
  int tmp2=0;
  int len=get_number_length(n);
  for(int i=0;i<len;i++){
    c=int_to_char(tmp1/ten_x_y(len-i-1));
    uartsend_char(c);
    tmp1=tmp1%(ten_x_y(len-i-1));
  }
}
void msleep(int num)
{
  for(int i=0;i<num;i++){
    for(int j=0;j<0x21FF;j++);
  }
}

