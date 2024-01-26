#include "rtc.h"
#include <rtthread.h>
#include "board.h"
#include "UART_Protocol.h"

RTC_HandleTypeDef hrtc;// RTC���
float	Lng_Value = 0;
float	Lat_Value = 0;
uint8_t GPS_Dat_Valid = 0;			// ָʾGPS�����Ƿ���Ч
uint8_t Time_Setted = 0;

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       2,�����ʼ��ģʽʧ��;
uint8_t RTC_Init(void)
{      
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	
	hrtc.Instance=RTC;
    hrtc.Init.HourFormat=RTC_HOURFORMAT_24;//RTC����Ϊ24Сʱ��ʽ 
    hrtc.Init.AsynchPrediv=0X7F;           //RTC�첽��Ƶϵ��(1~0X7F)
    hrtc.Init.SynchPrediv=0XFF;            //RTCͬ����Ƶϵ��(0~7FFF)   
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
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 600, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);				// 10���ӻ���һ��
    return 0;
}
//RTC�ײ�������ʱ������
//�˺����ᱻHAL_RTC_Init()����
//hrtc:RTC���
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//ʹ�ܵ�Դʱ��PWR
	HAL_PWR_EnableBkUpAccess();//ȡ����������д����
    
    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//����ΪRTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTCʱ��ԴΪLSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTCʱ��ʹ��
}

//���������ݱ�
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};         
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�                                                                                                                                                                                 
uint8_t GPS_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{        
        unsigned int temp2;
        unsigned char yearH,yearL;
        
        yearH=year/100;        yearL=year%100; 
        // ���Ϊ21����,�������100  
        if (yearH>19)yearL+=100;
        // ����������ֻ��1900��֮���  
        temp2=yearL+yearL/4;
        temp2=temp2%7; 
        temp2=temp2+day+table_week[month-1];
        if (yearL%4==0&&month<3)temp2--;
        return(temp2%7);
}

//UTCʱ��ת��Ϊ����ʱ��
void UTCToBeijing(uint16_t *year,uint8_t *month,uint8_t *day,uint8_t *hour,uint8_t *minute,uint8_t *second)
{
    *hour += 8;

    if(*month==1||*month==3||*month==5||*month==7||*month==8||*month==10||*month==12)//1,3,5,7,8,9,12��ÿ��Ϊ31��
    {
        if(*hour >= 24) 
        {   
            *hour -= 24;*day += 1;//�������24Сʱ����ȥ24Сʱ�����ټ���һ��
            if(*day > 31){*day -= 31;*month += 1;}//�������31һ�죬��ȥ31�죬�����һ����
        }
    }
    else if(*month==4||*month==6||*month==9||*month==11)//4��6��9��11��ÿ��Ϊ30��
    {
        if(*hour >= 24) 
        {   
            *hour -= 24;*day += 1;//�������24Сʱ����ȥ24Сʱ�����ټ���һ��
            if(*day > 30){*day -= 30;*month += 1;}//�������30һ�죬��ȥ30�죬�����һ����
        }
    }
    else//ʣ��Ϊ2�£�����Ϊ29�죬ƽ��Ϊ28��
    {
        if(*hour >= 24)
        {
            *hour -= 24;*day += 1;
            if((*year%400 == 0)||(*year%4 == 0 && *year%100 != 0))//�ж��Ƿ�Ϊ���꣬����ܱ�400����������ܱ�4�����������ܱ�100����Ϊ����
            {if(*day > 29){*day -= 29;*month += 1;}}//Ϊ����
            else{if(*day > 28){*day -= 28;*month += 1;}}//Ϊƽ��
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
