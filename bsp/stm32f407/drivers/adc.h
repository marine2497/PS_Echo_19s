#ifndef __ADC_H
#define __ADC_H	
#include <rtthread.h>
#include "board.h"

#define ADC1_DR_Address    ((uint32_t)0x4001204C)
#define	ADC_Num				5				// 5·ADCͨ��
#define	ADC1_DMA_Num		21000 //51200
#define	ADC2_DMA_Num		6
#define	ADC3_DMA_Num		8400 //5120

extern struct rt_semaphore ADC1_CMP_sem;					// ADC1ת������¼�
extern struct rt_semaphore ADC2_CMP_sem;					// ADC2ת������¼�
extern struct rt_semaphore ADC3_CMP_sem;					// ADC3ת������¼�

extern ADC_HandleTypeDef hadc1;				// ADC���
extern ADC_HandleTypeDef hadc2;				// ADC���
extern ADC_HandleTypeDef hadc3;				// ADC���

extern uint8_t ADC1_Buf[ADC1_DMA_Num];

extern uint8_t ADC3_Buf[ADC3_DMA_Num];			// ��100KHz�Ĳ����ʣ��ɼ���100�����ݸպ�1ms


void Adc1_Init(void); 						//ADCͨ����ʼ��
void Adc2_Init(void); 						//ADCͨ����ʼ��
void Adc3_Init(void); 						//ADCͨ����ʼ��
void ADC_DMA_Init(void);

#endif 















