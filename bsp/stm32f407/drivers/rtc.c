#include "rtc.h"
#include <rtthread.h>
#include "board.h"
#include "UART_Protocol.h"

RTC_HandleTypeDef hrtc;// RTC句柄
float	Lng_Value = 0;
float	Lat_Value = 0;
uint8_t GPS_Dat_Valid = 0;			// 指示GPS数据是否有效
uint8_t Time_Setted = 0;

//RTC初始化
//返回值:0,初始化成功;
//       2,进入初始化模式失败;
uint8_t RTC_Init(void)
{      
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	
	hrtc.Instance=RTC;
    hrtc.Init.HourFormat=RTC_HOURFORMAT_24;//RTC设置为24小时格式 
    hrtc.Init.AsynchPrediv=0X7F;           //RTC异步分频系数(1~0X7F)
    hrtc.Init.SynchPrediv=0XFF;            //RTC同步分频系数(0~7FFF)   
    hrtc.Init.OutPut=RTC_OUTPUT_DISABLE;     
    hrtc.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType=RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&hrtc)!=HAL_OK) return 2;
      
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x3338)
	{
		sTime.Hours = 0x0;
		sTime.Minutes = 0x0;
		sTime.Seconds = 0x0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

		sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		sDate.Month = RTC_MONTH_JANUARY;
		sDate.Date = 0x1;
		sDate.Year = 0x0;

		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x3338);
	}
    /**Enable the WakeUp 
    */
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 600, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);				// 10分钟唤醒一次
    return 0;
}
//RTC底层驱动，时钟配置
//此函数会被HAL_RTC_Init()调用
//hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}

//月修正数据表
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};         
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号                                                                                                                                                                                 
uint8_t GPS_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{        
        unsigned int temp2;
        unsigned char yearH,yearL;
        
        yearH=year/100;        yearL=year%100; 
        // 如果为21世纪,年份数加100  
        if (yearH>19)yearL+=100;
        // 所过闰年数只算1900年之后的  
        temp2=yearL+yearL/4;
        temp2=temp2%7; 
        temp2=temp2+day+table_week[month-1];
        if (yearL%4==0&&month<3)temp2--;
        return(temp2%7);
}

//UTC时间转换为北京时间
void UTCToBeijing(uint16_t *year,uint8_t *month,uint8_t *day,uint8_t *hour,uint8_t *minute,uint8_t *second)
{
    *hour += 8;

    if(*month==1||*month==3||*month==5||*month==7||*month==8||*month==10||*month==12)//1,3,5,7,8,9,12月每月为31天
    {
        if(*hour >= 24) 
        {   
            *hour -= 24;*day += 1;//如果超过24小时，减去24小时，后再加上一天
            if(*day > 31){*day -= 31;*month += 1;}//如果超过31一天，减去31天，后加上一个月
        }
    }
    else if(*month==4||*month==6||*month==9||*month==11)//4，6，9，11月每月为30天
    {
        if(*hour >= 24) 
        {   
            *hour -= 24;*day += 1;//如果超过24小时，减去24小时，后再加上一天
            if(*day > 30){*day -= 30;*month += 1;}//如果超过30一天，减去30天，后加上一个月
        }
    }
    else//剩下为2月，闰年为29天，平年为28天
    {
        if(*hour >= 24)
        {
            *hour -= 24;*day += 1;
            if((*year%400 == 0)||(*year%4 == 0 && *year%100 != 0))//判断是否为闰年，年号能被400整除或年号能被4整除，而不能被100整除为闰年
            {if(*day > 29){*day -= 29;*month += 1;}}//为闰年
            else{if(*day > 28){*day -= 28;*month += 1;}}//为平年
        }
    }
}
/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/
/**
  * @brief  Wake Up Timer callback.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	static uint8_t count = 0;
  /* Prevent unused argument(s) compilation warning */
	if(count++==6)
	{
		count = 0;
		Sys_CB.Time.used_time++;
	}
}

/**
* @brief This function handles RTC wake-up interrupt through EXTI line 22.
*/
void RTC_WKUP_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_WKUP_IRQn 0 */

  /* USER CODE END RTC_WKUP_IRQn 0 */
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_WKUP_IRQn 1 */

  /* USER CODE END RTC_WKUP_IRQn 1 */
}
