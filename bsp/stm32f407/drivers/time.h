#ifndef __TIME_H__
#define __TIME_H__

#include <rtthread.h>
#include "board.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
//extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;


extern struct rt_semaphore Used_time_sem;					// 十分钟事件

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
//void MX_TIM5_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
void MX_TIM9_Init(void);


void MX_TIM1_Init(uint16_t Freq,float txwidth);				// 频率和脉宽
void MX_TIM8_Init(uint16_t Freq,float txwidth);				// 频率和脉宽
void MX_GPIO_Init(void);
uint16_t Freq_Width(uint16_t Freq,float txwidth,uint16_t *Freq_Num);

#endif
