#ifndef __DEF_H__
#define __DEF_H__

#include <rtthread.h>
#include "board.h"
/***************************GPIOA*********************/
#define		RX_TX_L_Pin			GPIO_PIN_7
#define		RX_TX_L_Port		GPIOA
/***************************GPIOB*********************/
#define		Pon_CTL_Pin			GPIO_PIN_9
#define		Pon_CTL_Port		GPIOB
/***************************GPIOC*********************/
#define		RX_TX_H_Pin			GPIO_PIN_4
#define		RX_TX_H_Port		GPIOC
/***************************GPIOD*********************/
#define		LED1_Pin			GPIO_PIN_13
#define		LED2_Pin			GPIO_PIN_14
#define		LED_Port			GPIOD

/***************************GPIOE*********************/
#define		Beep_Pin			GPIO_PIN_2
#define		Beep_Port			GPIOE

#endif
