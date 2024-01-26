/*
* File      : drv_usart.c
* This file is part of RT-Thread RTOS
* COPYRIGHT (C) 2015, RT-Thread Development Team
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License along
*  with this program; if not, write to the Free Software Foundation, Inc.,
*  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* Change Logs:
* Date           Author       Notes
* 2009-01-05     Bernard      the first version
* 2015-08-01     xiaonong     the first version for stm32f7xx
* 2016-01-15     ArdaFu       the first version for stm32f4xx with STM32 HAL
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>     // standard definition
#include "drv_usart.h"
#include "board.h"
#include <rthw.h>
#include <rtthread.h>
#include "Cir_Queue.h"
#include "adc.h"
/* Private variables ---------------------------------------------------------*/

volatile char uart1_flag=1;
volatile char uart2_flag=0;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/****************事件定义*******************/
struct rt_semaphore UART1TXCMP_sem;					// 串口发送完成事件
struct rt_semaphore UART1RXCMP_sem;					// 串口接收完成事件

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

//struct UART_rxDat UART1_rxbuf;			// DMA缓存	
cirqueue UART1_rxQ;						// 存放已收到的数据

uint8_t	UART1_RxBuf[UART1RX_Buf_Len]; 		// 接收循环队列缓存
uint8_t	UART1_RxDMABuf[UART1RX_Buf_Len]; 	// 接收DMA缓存

//struct UART_txCon UART1_txCon;
char UART1_TxBuf_tmep[200];	
char UART1_TxBuf[UART1TX_Buf_Len];		// 命令发送缓存
char UART1_TxBufH[UART1TXH_Buf_Len];	// 高频数据发送缓存
char UART1_TxBufL[UART1TXL_Buf_Len];	// 低频频数据发送缓存

/* USART1 init function */
void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	
}
/* USART2 init function */
void MX_USART2_UART_Init(void)
{	/*UART2*/
//	huart2.Instance = USART2;
//	huart2.Init.BaudRate = 115200;
//	huart2.Init.WordLength = UART_WORDLENGTH_8B;
//	huart2.Init.StopBits = UART_STOPBITS_1;
//	huart2.Init.Parity = UART_PARITY_NONE;
//	huart2.Init.Mode = UART_MODE_TX;	//只发送数据
//	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
//	HAL_UART_Init(&huart2); //初始化会使能
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);	
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
}
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((huart1.Instance->SR&0x40)==0);//循环发送,直到发送完毕   
	huart1.Instance->DR = (uint8_t) ch;      
	return ch;
}
#endif

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();	
	
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
	
	

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);	

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	  
	  
    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream5;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_rx);

    __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_tx);
	
    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
//  else if(huart->Instance==USART2)
//  {
//    /**USART2 GPIO Configuration    
//    PD5    ------> USART1_TX
//    PD6     ------> USART1_RX 
//    */
//	__HAL_RCC_USART2_CLK_ENABLE();
//	  
//    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
//    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	  
//  }  
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma_usart2_rx) ;
//    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
//    {
//      _Error_Handler(__FILE__, __LINE__);
//    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma_usart2_tx) ;
//    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
//    {
//      _Error_Handler(__FILE__, __LINE__);
//    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);
		
  /* USART2_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }	
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
//  else if(huart->Instance==USART2)
//  {
//  /* USER CODE BEGIN USART2_MspDeInit 0 */

//  /* USER CODE END USART2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_USART2_CLK_DISABLE();
//  
//    /**USART2 GPIO Configuration    
//    PD5     ------> USART2_TX
//    PD6     ------> USART2_RX 
//    */
//    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);

//  /* USER CODE BEGIN USART2_MspDeInit 1 */

//  /* USER CODE END USART2_MspDeInit 1 */
//  } 
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* USART2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }	
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

void HAL_UART1_IdleCpltCallback(void)
{
	uint32_t temp;
	uint32_t dmarequest = 0x00U;
	uint16_t Dat_Len;
	uint16_t i;
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		//        HAL_UART_DMAStop(&huart1);
		/* Stop UART DMA Rx request if ongoing */
		dmarequest = HAL_IS_BIT_SET(huart1.Instance->CR3, USART_CR3_DMAR);
		if((huart1.RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
		{
			CLEAR_BIT(huart1.Instance->CR3, USART_CR3_DMAR);

			/* Abort the UART DMA Rx channel */
			if(huart1.hdmarx != NULL)
			{
				HAL_DMA_Abort(huart1.hdmarx);
			}
			//UART_EndRxTransfer(&huart1);
			/* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
			CLEAR_BIT(huart1.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
			CLEAR_BIT(huart1.Instance->CR3, USART_CR3_EIE);

			/* At end of Rx process, restore huart->RxState to Ready */
			huart1.RxState = HAL_UART_STATE_READY;
		}	
		/***************************************************************/
		temp  = huart1.hdmarx->Instance->NDTR;
		Dat_Len =  UART1RX_Buf_Len - temp;
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
		huart1.hdmarx->State = HAL_DMA_STATE_READY;
		for(i=0;i<Dat_Len;i++)
		{
			EnCirQueue(&UART1_rxQ, UART1_RxDMABuf[i]);
		}
		rt_sem_release(&UART1RXCMP_sem);
		uart1_flag = 1;
		uart2_flag = 0;		
		HAL_UART_Receive_DMA(&huart1, UART1_RxDMABuf, UART1RX_Buf_Len);
	}
}
void HAL_UART2_IdleCpltCallback(void)
{
	uint32_t temp;
	uint32_t dmarequest = 0x00U;
	uint16_t Dat_Len;
	uint16_t i;
	if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		//        HAL_UART_DMAStop(&huart1);
		/* Stop UART DMA Rx request if ongoing */
		dmarequest = HAL_IS_BIT_SET(huart2.Instance->CR3, USART_CR3_DMAR);
		if((huart2.RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
		{
			CLEAR_BIT(huart2.Instance->CR3, USART_CR3_DMAR);

			/* Abort the UART DMA Rx channel */
			if(huart2.hdmarx != NULL)
			{
				HAL_DMA_Abort(huart2.hdmarx);
			}
			//UART_EndRxTransfer(&huart1);
			/* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
			CLEAR_BIT(huart2.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
			CLEAR_BIT(huart2.Instance->CR3, USART_CR3_EIE);

			/* At end of Rx process, restore huart->RxState to Ready */
			huart2.RxState = HAL_UART_STATE_READY;
		}	
		/***************************************************************/
		temp  = huart2.hdmarx->Instance->NDTR;
		Dat_Len =  UART1RX_Buf_Len - temp;
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
		huart2.hdmarx->State = HAL_DMA_STATE_READY;
		for(i=0;i<Dat_Len;i++)
		{
			EnCirQueue(&UART1_rxQ, UART1_RxDMABuf[i]);
		}
		rt_sem_release(&UART1RXCMP_sem);
		uart1_flag = 0;
		uart2_flag = 1;		
		HAL_UART_Receive_DMA(&huart2, UART1_RxDMABuf, UART1RX_Buf_Len);
	}
}
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */
    rt_interrupt_enter();

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */
    rt_interrupt_leave();
  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
* @brief This function handles DMA2 stream7 global interrupt.
*/
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */
    rt_interrupt_enter();

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */
    rt_interrupt_leave();
  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/**
* @brief This function handles DMA2 stream7 global interrupt.
*/
void DMA1_Stream6_IRQHandler(void) //uart2
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */
    rt_interrupt_enter();

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */
    rt_interrupt_leave();
  /* USER CODE END DMA2_Stream7_IRQn 1 */
}
void DMA1_Stream5_IRQHandler(void)//uart2
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */
    rt_interrupt_enter();

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */
    rt_interrupt_leave();
  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
	/* USER CODE BEGIN USART1_IRQn 0 */
	rt_interrupt_enter();

	/* USER CODE END USART1_IRQn 0 */		
	HAL_UART1_IdleCpltCallback();
	HAL_UART_IRQHandler(&huart1);

	/* USER CODE BEGIN USART1_IRQn 1 */
	rt_interrupt_leave();
	/* USER CODE END USART1_IRQn 1 */
}
void USART2_IRQHandler(void)
{
	/* USER CODE BEGIN USART1_IRQn 0 */
	rt_interrupt_enter();

	/* USER CODE END USART1_IRQn 0 */		
	HAL_UART2_IdleCpltCallback();
	HAL_UART_IRQHandler(&huart2);

	/* USER CODE BEGIN USART1_IRQn 1 */
	rt_interrupt_leave();
	/* USER CODE END USART1_IRQn 1 */
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart1)
	{
		rt_sem_release(&UART1TXCMP_sem);
	}
	else if(huart==&huart2)
	{
		rt_sem_release(&UART1TXCMP_sem);
	}	
}




/**
* @brief This function handles DMA2 stream2 global interrupt.
*/

int hw_usart_init(void)
{
	/* Initialize all configured peripherals */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();	
	
//	UART1_rxbuf.prxbuf = UART1_RxBuf;
	
	InitQueue(&UART1_rxQ);
	
	MX_DMA_Init();
	
	MX_USART1_UART_Init();
	
	MX_USART2_UART_Init();	
	
	return 0;
}
INIT_BOARD_EXPORT(hw_usart_init);


