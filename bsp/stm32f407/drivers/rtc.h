#ifndef __RTC_H
#define __RTC_H	 
#include <rtthread.h>
#include "board.h"
 //////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//RTC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/5
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//�޸�˵��
//V1.1 20140726
//����:RTC_Get_Week����,���ڸ�����������Ϣ,�õ�������Ϣ.
////////////////////////////////////////////////////////////////////////////////// 
extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef sDate;
extern RTC_HandleTypeDef hrtc;  //RTC���	

extern float Lng_Value;
extern float Lat_Value;
//extern rt_device_t GPS_DEV;
extern uint8_t GPS_Dat_Valid;
extern uint8_t Time_Setted;

uint8_t RTC_Init(void);						//RTC��ʼ��

void UTCToBeijing(uint16_t *year,uint8_t *month,uint8_t *day,uint8_t *hour,uint8_t *minute,uint8_t *second);
uint8_t GPS_Get_Week(uint16_t year,uint8_t month,uint8_t day);
uint8_t GPS_Info_RCV(void);
void GPS_SetTime(void);
#endif

















