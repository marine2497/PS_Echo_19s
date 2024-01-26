#ifndef __ADC_H
#define __ADC_H	
#include <rtthread.h>
#include "board.h"

#define ADC1_DR_Address    ((uint32_t)0x4001204C)
#define	ADC_Num				5				// 5路ADC通道
#define	ADC1_DMA_Num		21000 //51200
#define	ADC2_DMA_Num		6
#define	ADC3_DMA_Num		8400 //5120

extern struct rt_semaphore ADC1_CMP_sem;					// ADC1转换完成事件
extern struct rt_semaphore ADC2_CMP_sem;					// ADC2转换完成事件
extern struct rt_semaphore ADC3_CMP_sem;					// ADC3转换完成事件

extern ADC_HandleTypeDef hadc1;				// ADC句柄
extern ADC_HandleTypeDef hadc2;				// ADC句柄
extern ADC_HandleTypeDef hadc3;				// ADC句柄

extern uint8_t ADC1_Buf[ADC1_DMA_Num];

extern uint8_t ADC3_Buf[ADC3_DMA_Num];			// 以100KHz的采样率，采集到100个数据刚好1ms


void Adc1_Init(void); 						//ADC通道初始化
void Adc2_Init(void); 						//ADC通道初始化
void Adc3_Init(void); 						//ADC通道初始化
void ADC_DMA_Init(void);

#endif 















