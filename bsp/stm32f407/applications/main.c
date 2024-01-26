/****************************************Copyright (c)**************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: main.c
**创   建   人: 
**最后修改日期: 2018年9月20日
**描        述: 主函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
********************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "string.h"
#include "ADC.h"
#include "DAC.h"
#include "TIME.h"
#include "Regtime.h"
#include "DEF.h"
//#include "LCDDRV.H"
#include "drv_usart.h"
#include "UART_Protocol.H"
#include "UART_Parameters.H"
#include "UART_DEVICE.H"
#include "Ultrason_tran.H"
#include "Ultrason_rcv.H"
#include <stdio.h>
#include "Ultrason_Cal.H"
#include "iwdg.h"

/*  变量分配4字节对齐 */
ALIGN(RT_ALIGN_SIZE)
/****************线程定义*******************/
static rt_uint8_t TX_stack[ 4096 ];
static struct rt_thread TX_thread;

static rt_uint8_t DatPro_H_stack[ 20480 ];
static struct rt_thread DatPro_H_thread;

static rt_uint8_t DatPro_L_stack[ 10240];
static struct rt_thread DatPro_L_thread;

static rt_uint8_t PC_Interface_stack[ 4096 ];
static struct rt_thread PC_Interface_thread;

static rt_uint8_t Used_Time_stack[ 4096 ];
static struct rt_thread Used_Time_thread;

static rt_uint8_t ALARM_stack[ 4096 ];
static struct rt_thread ALARM_thread;



/**************************************************************
* function name ：TX_thread_entry
* description   ：信号发射线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void TX_thread_entry(void* parameter)
{
	uint8_t *p_src = (uint8_t *)&Sys_CB.Parameters,*p_Dec = (uint8_t *)&Sys_CB_Copy.Parameters;
	memcpy(p_Dec,p_src,sizeof(Sys_CB_Copy));					//zgh
	
	MX_DAC_Init();	
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
//	MX_TIM5_Init();
	MX_TIM6_Init();
	MX_TIM9_Init();
	MX_TIM1_Init(Sys_CB.Sounder.frequency.channel_two,Sys_CB.Sounder.txwidth.channel_two);
	MX_TIM8_Init(Sys_CB.Sounder.frequency.channel_one,Sys_CB.Sounder.txwidth.channel_one);
	__HAL_TIM_MOE_ENABLE(&htim1);	  
	__HAL_TIM_MOE_ENABLE(&htim8);	  
	//初始化RTC
//	RTC_Init();
	Adc1_Init();
	Adc2_Init();
	Adc3_Init();
	ADC_DMA_Init();
	Get_PowerDcv();						//获取电源电压
		
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1229);	// 设置为0.75V，暂时不工作
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);	
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 1229);	// 设置为0.75V，暂时不工作
	HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

	L_Freq_Charge();					// 上电先充电
	H_Freq_Charge();

	while (1)
	{	
			
		Ultrason_Trans();
		if(CMD_Refresh)
		{
			CMD_Refresh = 0;
			memcpy(p_src,p_Dec,sizeof(Sys_CB_Copy));				// 更新参数
		}
	}
}
/**************************************************************
* function name ：DatPro_H_thread_entry
* description   ：高频数据处理线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void DatPro_H_thread_entry(void* parameter)
{
	while(1)
	{	
		rt_sem_take(&HFreqTranCMP_sem, RT_WAITING_FOREVER);				// 等待高频信号发送完毕信号量
		UltraSonDatPro(&Sys_CB_H,&Sys_CB.Grade);
//		rt_thread_delay(RT_TICK_PER_SECOND);
	}
}
/**************************************************************
* function name ：DatPro_L_thread_entry
* description   ：低频数据处理线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void DatPro_L_thread_entry(void* parameter)
{
	while(1)
	{
		rt_sem_take(&LFreqTranCMP_sem, RT_WAITING_FOREVER);				// 等待低频信号发送完毕信号量
		UltraSonDatPro(&Sys_CB_L,&Sys_CB.Grade);
	}
}
/**************************************************************
* function name ：PC_Interface_thread_entry
* description   ：数据交互，解析SIC命令线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void PC_Interface_thread_entry(void* parameter)
{
	HAL_UART_Receive_DMA(&huart1, UART1_RxDMABuf, UART1RX_Buf_Len);
	HAL_UART_Receive_DMA(&huart2, UART1_RxDMABuf, UART1RX_Buf_Len);	
	Jump_setCMD_Process("#sic,,set,device.jump,1");
	printf("SDE-19S App bulid time :%s\r\n", __TIME__);
	
	while(1)
	{
		CMD_Process( );
		rt_sem_take(&UART1RXCMP_sem, RT_WAITING_FOREVER);				// 试图持有信号量
		
	}
}
/**************************************************************
* function name ：Used_Time_thread_entry
* description   ：刷新使用时间线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void Used_Time_thread_entry(void* parameter)
{

	Read_CurUseTime();
	Read_Para_Process();
	MX_TIM7_Init();
	CMD_Refresh =1;
	
//	Sys_CB.Time.reg_time = 200;
//	REGI_Process(Sys_CB.Time.reg_time);			// 更新注册时间
	
	while(1)
	{
		Regtime_write(Sys_CB.Time.p_Cur_Addr, Sys_CB.Time.Cur_Counter_10Min);	// 记录10分钟的个数
		rt_sem_take(&Used_time_sem, RT_WAITING_FOREVER);						// 试图持有信号量	
	}
}
/**************************************************************
* function name ：ALARM_thread_entry
* description   ：报警处理线程
* input			：parameter
* output		：none
* return		：none
***************************************************************/
static void ALARM_thread_entry(void* parameter)
{
	int mun_i = 0;
	MX_IWDG_Init();	//初始化看门狗
	while(1)
	{

		if((Sys_CB.Survey_Status.channel_one)||(Sys_CB.Survey_Status.channel_two))
		{
			if((Sys_CB_H.Water_Deep>Sys_CB.Parameters.draft && Sys_CB_H.Water_Deep<Sys_CB.Parameters.minadepth) || (Sys_CB_L.Water_Deep>Sys_CB.Parameters.draft && Sys_CB_L.Water_Deep<Sys_CB.Parameters.minadepth))
			{// 浅水报警水深  响0.25秒停0.25秒，0.5秒一个周期，频率为2Hz
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_SET);				
				rt_thread_delay(50);//250ms	
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_RESET);	
				rt_thread_delay(50);//500ms	
				mun_i = mun_i+5;
			}

			else if(Sys_CB.Parameters.alarmstatus == 1 && (Echo_ProcCal[0].echonum == 0 || Echo_ProcCal[1].echonum == 0))
			{//	无回波报警  响1秒停4秒，5秒一个周期，频率为0.2Hz
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_SET);				
				rt_thread_delay(200);//1000ms	
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_RESET);			
				rt_thread_delay(800);//4000ms	
				mun_i = mun_i+50;
			}
			else if(Sys_CB.Voltage.voltage<11 && Sys_CB.Voltage.voltage>9)
			{//	低压报警
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_SET);				
				rt_thread_delay(400);//200ms	
				HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_RESET);			
				rt_thread_delay(400);//2000ms	
				mun_i = mun_i+40;
			}			
			else
			{
				rt_thread_delay(20);//100ms	
				mun_i = mun_i+1;
			}
			

		}
		else
		{		
			rt_thread_delay(20);		// 100ms
			mun_i = mun_i+1;
		}
		
		if(mun_i>150) //10s
		{
			mun_i = 0;
//			Read_CurUseTime();
			Uart2_DatOut();
		}		
		IWDG_Feed();	//喂狗
		
	}

}

/* USER CODE END 2 */
/**************************************************************
* function name ：main
* description   ：主函数
* input			：none
* output		：none
* return		：none
***************************************************************/
int main(void)
{
	rt_err_t result;
	MX_GPIO_Init();
	hw_usart_init();
	
#ifdef Unmanned_ship	
	my_strncpy(App_ver,"1.10.190715.Unmanned_ship",25); //定制无人船固件 	
#else 
	//AP-表示正常固件 //RD-表示固定注册时间
	my_strncpy(App_ver,"1.26.210629.D260-2RD",20); //固件日期在此修改
//my_strncpy(App_ver,"1.10.190625.D260RD",18); //注册码固定后三位301 403 505
#endif

	
	/* 初始化事件对象 */
	result = rt_sem_init(&ADC1_CMP_sem,
						"ADC1 Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&ADC2_CMP_sem,
						"ADC2 Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&ADC3_CMP_sem,
						"ADC3 Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }	
	result = rt_sem_init(&HFreqTranCMP_sem,
						"HFreqTran Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&LFreqTranCMP_sem,
						"LFreqTran Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&UART1TXCMP_sem,
						"UART TX Complete event",
						1,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&UART1RXCMP_sem,
						"UART RX Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }	
	result = rt_event_init(&HL_Pro_CMP_event, "event1", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
	result = rt_sem_init(&Used_time_sem,
						"Usedtime Complete event",
						0,
						RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -1;
    }
/*********************************************************************************/
	result = rt_thread_init(&TX_thread,
		"TX Process",
		TX_thread_entry, RT_NULL,
		(rt_uint8_t*)&TX_stack[0], sizeof(TX_stack), 14, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&TX_thread);
	}
	result = rt_thread_init(&DatPro_H_thread,
		"Data Process HFreq",
		DatPro_H_thread_entry, RT_NULL,
		(rt_uint8_t*)&DatPro_H_stack[0], sizeof(DatPro_H_stack), 15, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&DatPro_H_thread);
	}
	result = rt_thread_init(&DatPro_L_thread,
		"Data Process LFreq",
		DatPro_L_thread_entry, RT_NULL,
		(rt_uint8_t*)&DatPro_L_stack[0], sizeof(DatPro_L_stack), 16, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&DatPro_L_thread);
	}
	result = rt_thread_init(&PC_Interface_thread,
		"PC_Interface Process",
		PC_Interface_thread_entry, RT_NULL,
		(rt_uint8_t*)&PC_Interface_stack[0], sizeof(PC_Interface_stack), 13, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&PC_Interface_thread);
	}
	result = rt_thread_init(&Used_Time_thread,
		"Used Time Process",
		Used_Time_thread_entry, RT_NULL,
		(rt_uint8_t*)&Used_Time_stack[0], sizeof(Used_Time_stack), 17, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&Used_Time_thread);
	}
	result = rt_thread_init(&ALARM_thread,
		"ALARM Process",
		ALARM_thread_entry, RT_NULL,
		(rt_uint8_t*)&ALARM_stack[0], sizeof(ALARM_stack), 18, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&ALARM_thread);
	}	
}
