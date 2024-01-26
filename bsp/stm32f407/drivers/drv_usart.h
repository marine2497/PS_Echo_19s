/*
 * File      : usart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2015-08-01     xiaonong     the first version for stm32f7xx
 * 2016-01-15     ArdaFu       the first version for stm32f4xx with STM32 HAL
 */

#ifndef __USART_H__
#define __USART_H__
#include <rtthread.h>
#include "stm32f4xx_hal.h"
#include "Cir_Queue.h"

#define UART1RX_Buf_Len		1024
#define UART1TX_Buf_Len		512

#define UART1TXH_Buf_Len	2500//5120
#define UART1TXL_Buf_Len	1024//3072

//struct UART_rxDat
//{
//	uint8_t *prxbuf;
//	uint16_t rx_len;
//};

/* Private variables ---------------------------------------------------------*/
extern struct rt_semaphore UART1TXCMP_sem;					// 串口发送完成事件
extern struct rt_semaphore UART1RXCMP_sem;					// 串口接收完成事件

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern cirqueue UART1_rxQ;							// 存放已收到的数据

//extern struct UART_rxDat UART1_rxbuf;				// DMA缓存
extern uint8_t	UART1_RxBuf[UART1RX_Buf_Len]; 		// 接收循环队列缓存
extern uint8_t	UART1_RxDMABuf[UART1RX_Buf_Len]; 	// 接收DMA缓存
extern char UART1_TxBuf[UART1TX_Buf_Len];			// 命令发送缓存
extern char UART1_TxBufH[UART1TXH_Buf_Len];			// 高频数据发送缓存
extern char UART1_TxBufL[UART1TXL_Buf_Len];			// 低频频数据发送缓存

extern char UART1_TxBuf_tmep[200];	


extern volatile char uart1_flag;
extern volatile char uart2_flag;

int hw_usart_init(void);

void MX_USART1_UART_Init(void);

#endif
