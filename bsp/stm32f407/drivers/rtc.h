#ifndef __RTC_H
#define __RTC_H	 
#include <rtthread.h>
#include "board.h"
 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RTC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20140726
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
////////////////////////////////////////////////////////////////////////////////// 
extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef sDate;
extern RTC_HandleTypeDef hrtc;  //RTC句柄	

extern float Lng_Value;
extern float Lat_Value;
//extern rt_device_t GPS_DEV;
extern uint8_t GPS_Dat_Valid;
extern uint8_t Time_Setted;

uint8_t RTC_Init(void);						//RTC初始化

void UTCToBeijing(uint16_t *year,uint8_t *month,uint8_t *day,uint8_t *hour,uint8_t *minute,uint8_t *second);
uint8_t GPS_Get_Week(uint16_t year,uint8_t month,uint8_t day);
uint8_t GPS_Info_RCV(void);
void GPS_SetTime(void);
#endif

















