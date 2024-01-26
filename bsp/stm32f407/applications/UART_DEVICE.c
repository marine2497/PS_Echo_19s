/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: UART_DEVICE.c
**��   ��   ��: 
**����޸�����: 2018��9��20��
**��        ��: DEVICE�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/
#include <rtthread.h>
#include "math.h"
#include "board.h"
#include "string.h"
#include "regtime.h"
#include "drv_usart.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.h"
#include "Ultrason_rcv.H"
#include "Decrypt2013.h"
#include "stm32f4xx_hal.h"
/*************************************************************************************************/
//	DEVICE�豸���������
/*************************************************************************************************/
/*************************************************************************************************/
// INFO�����
/*************************************************************************************************/
void Info_getCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,"\r\n",sizeof("\r\n")-1)==0)	// ������Ϣ
	{	
		//Read_Stm32ID(Sys_CB.Device_Info->pid,UNIQUEID_ADDRESS);		
		Read_Stm32ID(Pid,UNIQUEID_ADDRESS);
		
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���

//		my_strncpy(Pid, "13003500B0150472", strlen("13003500B0150472")); //�̶�pid
	
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,DEVICE.INFO,OK,",sizeof("@SIC,,GET,DEVICE.INFO,OK,")-1);
		
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->boot_ver,strlen(Sys_CB.Device_Info->boot_ver));
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->sn,strlen(Sys_CB.Device_Info->sn));
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->pn,strlen(Sys_CB.Device_Info->pn));
		UART1_TxBuf[Num++] = '|';	
//		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->pid,strlen(Sys_CB.Device_Info->pid));		
		Num += my_strncpy(&UART1_TxBuf[Num],Pid, strlen(Pid));
		UART1_TxBuf[Num++] = '|';	
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->hid,strlen(Sys_CB.Device_Info->hid));
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->brand,strlen(Sys_CB.Device_Info->brand));
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->model,strlen(Sys_CB.Device_Info->model));
//		Num += my_strncpy(&UART1_TxBuf[Num],"260D",4);
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->production_date,strlen(Sys_CB.Device_Info->production_date));
		UART1_TxBuf[Num++] = '|';
//		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->app_ver,strlen(Sys_CB.Device_Info->app_ver));
		Num += my_strncpy(&UART1_TxBuf[Num],App_ver,strlen(App_ver));		
		UART1_TxBuf[Num++] = '|';
		Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Device_Info->gnssboard,strlen(Sys_CB.Device_Info->gnssboard));
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
			
		}
	}
	// BOOT_VER����: BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
	else if(rt_strncmp((const char *)p,".boot_ver",sizeof(".boot_ver")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.BOOT_VER,OK,",
					(void *)&Sys_CB.Device_Info->boot_ver,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".sn",sizeof(".sn")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.SN,OK,",
					(void *)&Sys_CB.Device_Info->sn,
					Str_Type
						);
	}
	else if(rt_strncmp((const char *)p,".pn",sizeof(".pn")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.PN,OK,",
					(void *)&Sys_CB.Device_Info->pn,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".pid",sizeof(".pid")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.PID,OK,",
					(void *)&Pid,
					Str_Type
					);					
	}
	else if(rt_strncmp((const char *)p,".hid",sizeof(".hid")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.HID,OK,",
					(void *)&Sys_CB.Device_Info->hid,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".brand",sizeof(".brand")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.BRAND,OK,",
					(void *)&Sys_CB.Device_Info->brand,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".model",sizeof(".model")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.MODEL,OK,",
					(void *)&Sys_CB.Device_Info->model,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".production_date",sizeof(".production_date")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.PRODUCTION_DATE,OK,",
					(void *)&Sys_CB.Device_Info->production_date,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".app_ver",sizeof(".app_ver")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.APP_VER,OK,",
					(void *)&App_ver,
					Str_Type
					);
	}
	else if(rt_strncmp((const char *)p,".gnssboard",sizeof(".gnssboard")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.INFO.GNSSBOARD,OK,",
					(void *)&Sys_CB.Device_Info->gnssboard,
					Str_Type
					);
	}
}
/*************************************************************************************************/
// TIME�����
/*************************************************************************************************/
void Time_getCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,"\r\n",sizeof("\r\n")-1)==0)	// ������Ϣ
	{
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,DEVICE.TIME,OK,",sizeof("@SIC,,GET,DEVICE.TIME,OK,")-1);
		Num += Long_to_Str(&UART1_TxBuf[Num],Sys_CB.Time.reg_time);
		UART1_TxBuf[Num++] = '|';
		Num += Long_to_Str(&UART1_TxBuf[Num],Sys_CB.Time.used_time);
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
		
		}
	}
	else if(rt_strncmp((const char *)p,".reg_time",sizeof(".reg_time")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.TIME.REG_TIME,OK,",
					&Sys_CB.Time.reg_time,
					Int_Type
					);
	}
	else if(rt_strncmp((const char *)p,".used_time",sizeof(".used_time")-1)==0)
	{
		getCMD_ACK(	"@SIC,,GET,DEVICE.TIME.UESD_TIME,OK,",
					&Sys_CB.Time.used_time,
					Int_Type
					);
	}
}
/*************************************************************************************************/
// VOLTAGE�����
/*************************************************************************************************/
void Voltage_getCMD_Process(void)
{
	getCMD_ACK("@SIC,,GET,DEVICE.VOLTAGE,OK,",
			&Sys_CB.Voltage.voltage,
			Float_Type
			);
}
/*************************************************************************************************/
// REGI�����
/*************************************************************************************************/
void Regi_setCMD_Process(char *p)
{
	uint16_t Num;
//	uint32_t regtime = 0;
	Str_to_Long_num(p,&Sys_CB.Time.reg_time,8);
	
//	Sys_CB.Time.reg_time = 100;
	
	REGI_Process(Sys_CB.Time.reg_time);			// ����ע��ʱ��

//	Str_to_Long_num(p,&regtime,8);
//	REGI_Process(regtime);			// ����ע��ʱ��	
	
	/*ע���ʹ��ʱ������-���Ի�-��ʹ�øù���ʱע�͵�*/
	/*
	Sys_CB.Time.Cur_Counter_10Min = 0;
	Regtime_write(Sys_CB.Time.p_Cur_Addr,Sys_CB.Time.Cur_Counter_10Min);
	*/
	Read_CurUseTime(); //ע�����Ҫ���¶�ȡʹ��ʱ�䣬����õ���ʹ��ʹ������
			

	CMD_Refresh = 1;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,DEVICE.REGI,",sizeof("@SIC,,SET,DEVICE.REGI,")-1);
	Num += my_strncpy(&UART1_TxBuf[Num],Sys_CB.Regi.regi,sizeof(Sys_CB.Regi.regi));
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
	if(uart1_flag)
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

	}
	else if(uart2_flag)
	{
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
	}
	

}
/*************************************************************************************************/
// SURVEY_STATUS�����
/*************************************************************************************************/
void Survey_Status_getCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_all",sizeof(".channel_all")-1)==0)
	{
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,DEVICE.SURVEY_STATUS.CHANNEL_ALL,OK,",sizeof("@SIC,,GET,DEVICE.SURVEY_STATUS.CHANNEL_ALL,OK,")-1);
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Status.channel_one + '0';
		UART1_TxBuf[Num++] = '|';
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Status.channel_two + '0';
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
			
		}
	}
	else if(rt_strncmp((const char *)p,".channel_one",sizeof(".channel_one")-1)==0)
	{
		getCMD_ACK("@SIC,,GET,DEVICE.SURVEY_STATUS.CHANNEL_ONE,OK,",
					&Sys_CB_Copy.Survey_Status.channel_one,
					Byte_Type
					);
	}
	else if(rt_strncmp((const char *)p,".channel_two",sizeof(".channel_two")-1)==0)
	{
		getCMD_ACK("@SIC,,GET,DEVICE.SURVEY_STATUS.CHANNEL_TWO,OK,",
					&Sys_CB_Copy.Survey_Status.channel_two,
					Byte_Type
					);
	}
}
void Survey_Status_setCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_all,",sizeof(".channel_all,")-1)==0)
	{
		Num = sizeof(".channel_all,")-1;
		if(*(p+Num+1)=='|')
		{
			if(*(p+Num)=='0')
			{
				Sys_CB_Copy.Survey_Status.channel_one = 0;
			}
			else if(*(p+Num)=='1')
			{
				Sys_CB_Copy.Survey_Status.channel_one = 0;
			}
			if(*(p+Num+2)=='0')
			{
				Sys_CB_Copy.Survey_Status.channel_two = 0;
			}
			else if(*(p+Num+2)=='1')
			{
				Sys_CB_Copy.Survey_Status.channel_two = 1;
			}
		}
		CMD_Refresh = 1;
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,DEVICE.SURVEY_STATUS.CHANNEL_ALL,",sizeof("@SIC,,SET,DEVICE.SURVEY_STATUS.CHANNEL_ALL,")-1);
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Status.channel_one + '0';
		UART1_TxBuf[Num++] = '|';
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Status.channel_two + '0';
		UART1_TxBuf[Num++] = ',';
		UART1_TxBuf[Num++] = 'O';
		UART1_TxBuf[Num++] = 'K';
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
	
		}
	}
	else if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
	{
		Num = sizeof(".channel_one,")-1;
		if(*(p+Num)=='0')
		{
			Sys_CB_Copy.Survey_Status.channel_one = 0;
		}
		else if(*(p+Num)=='1')
		{
			Sys_CB_Copy.Survey_Status.channel_one = 1;
		}
		setCMD_ACK("@SIC,,SET,DEVICE.SURVEY_STATUS.CHANNEL_ONE,",
				&Sys_CB_Copy.Survey_Status.channel_one,
				Byte_Type
				);
	}
	else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
	{
		Num = sizeof(".channel_two,")-1;
		if(*(p+Num)=='0')
		{
			Sys_CB_Copy.Survey_Status.channel_two = 0;
		}
		else if(*(p+Num)=='1')
		{
			Sys_CB_Copy.Survey_Status.channel_two = 1;
		}
		setCMD_ACK("@SIC,,SET,DEVICE.SURVEY_STATUS.CHANNEL_TWO,",
				&Sys_CB_Copy.Survey_Status.channel_two,
				Byte_Type
				);
	}
}
/*************************************************************************************************/
// SURVEY_MODEL�����
/*************************************************************************************************/
void Survey_Model_getCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_all",sizeof(".channel_all")-1)==0)
	{
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,DEVICE.SURVEY_MODEL.CHANNEL_ALL,OK,",sizeof("@SIC,,GET,DEVICE.SURVEY_MODEL.CHANNEL_ALL,OK,")-1);
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Model.channel_one + '0';
		UART1_TxBuf[Num++] = '|';
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Model.channel_two + '0';
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	

		}
	}
	else if(rt_strncmp((const char *)p,".channel_one",sizeof(".channel_one")-1)==0)
	{
		getCMD_ACK("@SIC,,GET,DEVICE.SURVEY_MODEL.CHANNEL_ONE,OK,",
				&Sys_CB_Copy.Survey_Model.channel_one,
				Byte_Type
				);
	}
	else if(rt_strncmp((const char *)p,".channel_two",sizeof(".channel_two")-1)==0)
	{
		getCMD_ACK("@SIC,,GET,DEVICE.SURVEY_MODEL.CHANNEL_TWO,OK,",
				&Sys_CB_Copy.Survey_Model.channel_two,
				Byte_Type
				);
	}
}
void Survey_Model_setCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_all,",sizeof(".channel_all,")-1)==0)
	{
		Num = sizeof(".channel_all,")-1;
		if(*(p+Num)=='0')
		{
			Sys_CB_Copy.Survey_Model.channel_one = 0;
			Sys_CB_Copy.Survey_Model.channel_two = 0;
		}
		else if(*(p+Num)=='1')
		{
			Sys_CB_Copy.Survey_Model.channel_one = 1;
			Sys_CB_Copy.Survey_Model.channel_two = 1;
		}
		CMD_Refresh = 1;
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,DEVICE.SURVEY_MODEL.CHANNEL_ALL,",sizeof("@SIC,,SET,DEVICE.SURVEY_MODEL.CHANNEL_ALL,")-1);
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Model.channel_one + '0';
		UART1_TxBuf[Num++] = '|';
		UART1_TxBuf[Num++] = Sys_CB_Copy.Survey_Model.channel_two + '0';
		UART1_TxBuf[Num++] = ',';
		UART1_TxBuf[Num++] = 'O';
		UART1_TxBuf[Num++] = 'K';
		UART1_TxBuf[Num++] = '*';
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
		
		}
	}
	else if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
	{
		Num = sizeof(".channel_one,")-1;
		if(*(p+Num)=='0')
		{
			Sys_CB_Copy.Survey_Model.channel_one = 0;
		}
		else if(*(p+Num)=='1')
		{
			Sys_CB_Copy.Survey_Model.channel_one = 1;
		}
		setCMD_ACK("@SIC,,SET,DEVICE.SURVEY_MODEL.CHANNEL_ONE,",
				&Sys_CB_Copy.Survey_Model.channel_one,
				Byte_Type
				);
	}
	else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
	{
		Num = sizeof(".channel_two,")-1;
		if(*(p+Num)=='0')
		{
			Sys_CB_Copy.Survey_Model.channel_two = 0;
		}
		else if(*(p+Num)=='1')
		{
			Sys_CB_Copy.Survey_Model.channel_two = 1;
//			if(Sys_CB_Copy.Grade.grade != 0 && Sys_CB_Copy.Grade.grade<3 && Sys_CB_H.Water_Deep == Sys_CB_Copy.Parameters.draft)
//			{
//				Sys_CB_Copy.Grade.grade = 3;
//				setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB_Copy.Grade.grade, Byte_Type);
//			}
		}
		setCMD_ACK("@SIC,,SET,DEVICE.SURVEY_MODEL.CHANNEL_TWO,",
				&Sys_CB_Copy.Survey_Model.channel_two,
				Byte_Type
				);
	}
}
/*************************************************************************************************/
//	Dognum�����
/*************************************************************************************************/
void Dognum_setCMD_Process(char *p)
{
	uint16_t Num;
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)p+1,10);	
//	
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
//	Num = 0;
//	Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,DEVICE.DOGNUM,PENDING*",sizeof("@SIC,,SET,DEVICE.DOGNUM,PENDING*")-1);
//	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
//	Num++;
//	UART1_TxBuf[Num++] = '\r';
//	UART1_TxBuf[Num++] = '\n';
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);	
	
	if(Dog_Key(p+1))
	{
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		Num = 0;
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,DEVICE.DOGNUM",sizeof("@SIC,,GET,DEVICE.DOGNUM")-1);
		UART1_TxBuf[Num++] = ',';
		UART1_TxBuf[Num++] = 'O';
		UART1_TxBuf[Num++] = 'K';
		UART1_TxBuf[Num++] = ',';			
		UART1_TxBuf[Num++] = *(p+1);
		UART1_TxBuf[Num++] = *(p+2);	
		UART1_TxBuf[Num++] = *(p+3);		
		UART1_TxBuf[Num++] = *(p+4);
		UART1_TxBuf[Num++] = *(p+5);	
		UART1_TxBuf[Num++] = *(p+6);		
		UART1_TxBuf[Num++] = *(p+7);
		UART1_TxBuf[Num++] = *(p+8);
		UART1_TxBuf[Num++] = *(p+9);
		UART1_TxBuf[Num++] = *(p+10);	
		UART1_TxBuf[Num++] = '*';			
		UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
		Num++;
		UART1_TxBuf[Num++] = '\r';
		UART1_TxBuf[Num++] = '\n';
		if(uart1_flag)
		{
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

		}
		else if(uart2_flag)
		{
			HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
		
		}
	}
	
}
/*************************************************************************************************/
//	jump�����
/*************************************************************************************************/
void Jump_setCMD_Process(char *p)
{
	uint16_t Num; 
	
	Read_Stm32ID(Pid,UNIQUEID_ADDRESS);
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,DEVICE.JUMP,",sizeof("@SIC,,SET,DEVICE.JUMP,")-1);
	UART1_TxBuf[Num++] = '1';
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
	if(uart1_flag)
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

	}
	else// if(uart2_flag)
	{
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	

	}
}

/*************************************************************************************************/
// DEVICE GET�����
/*************************************************************************************************/

uint8_t getinforesetcnt =0;
void DEVICE_getCMD(char *p)
{
	uint16_t Num;
	// INRO����
	if(rt_strncmp(p,".info",sizeof(".info")-1)==0)
	{
		Num = sizeof(".info")-1;
		Info_getCMD_Process(p+Num);
		getinforesetcnt++;
		if(getinforesetcnt>10)	//������ȡinfo  ��������
		{
			HAL_NVIC_SystemReset();
		}
	}
	// TIME����
	else if(rt_strncmp(p,".time",sizeof(".time")-1)==0)
	{
		getinforesetcnt = 0;
		Num = sizeof(".time")-1;
		Time_getCMD_Process(p+Num);
	}
	// VOLTAGE����
	else if(rt_strncmp(p,".voltage",sizeof(".voltage")-1)==0)
	{
		getinforesetcnt = 0;
		Voltage_getCMD_Process();
	}
	// SURVEY_STATUS��ȡ����
	else if(rt_strncmp(p,".survey_status",sizeof(".survey_status")-1)==0)
	{
		getinforesetcnt = 0;
		Num = sizeof(".survey_status")-1;
		Survey_Status_getCMD_Process(p+Num);
	}
	// SURVEY_MODEL��ȡ����
	else if(rt_strncmp(p,".survey_model",sizeof(".survey_model")-1)==0)
	{
		getinforesetcnt = 0;
		Num = sizeof(".survey_model")-1;
		Survey_Model_getCMD_Process(p+Num);
	}
}
/*************************************************************************************************/
// DEVICE SET�����
/*************************************************************************************************/
void DEVICE_setCMD(char *p)
{
	uint16_t Num, i;
	char SN[17] = {0};
//	char Reg_Num[36] = {0};
	char str_reg_time[8] = {0};
	char High_status =0;
	char Low_status =0;
	// REGI����
	if(rt_strncmp(p,".regi",sizeof(".regi")-1)==0)
	{
		Num = sizeof(".regi,")-1;
		rt_strncpy(Sys_CB.Regi.regi,p+Num,36);
		for(i=0; i<36; i++)
		{
			if((Sys_CB.Regi.regi[i]>='a')&&(Sys_CB.Regi.regi[i]<='z'))				// ���д�д��ĸ��ɴ�д��ĸ
			{
				Sys_CB.Regi.regi[i] = Sys_CB.Regi.regi[i] - 'a' + 'A';
			}
			Sys_CB.Regi.srt_regi[i] = Sys_CB.Regi.regi[i];
		}
		
		Low_status = Sys_CB.Survey_Status.channel_one;
		High_status = Sys_CB.Survey_Status.channel_two;
		
		Sys_CB.Survey_Status.channel_one = 0;
		Sys_CB.Survey_Status.channel_two = 0;
		rt_thread_delay(500);
			
		rt_strncpy(SN, Sys_CB.Device_Info->sn, 15);		
		SN[15] = '0';
		SN[16] = '0';  //�ѻ����޸�Ϊ17λ������λ��0
		
		
		rt_strncpy(str_reg_time, Sys_CB.Time.str_reg_time, 8);
		i = Decrypt(0, SN, &Pid[0], &Sys_CB.Regi.regi[0], str_reg_time, 0);
		if(i==0) //�ɹ�����
		{
			for(i=0; i<36; i++)		
				Sys_CB.Regi.srt_regi[i] = Sys_CB.Regi.regi[i];
					
			rt_strncpy( Sys_CB.Time.str_reg_time, str_reg_time, 8);	
			Regi_setCMD_Process(&Sys_CB.Time.str_reg_time[0]);
//			printf("str_reg_time = %s\r\n", Sys_CB.Time.str_reg_time);		
		}
        //2021��6��16��18:13:39
		else//�̶�ע��ʱ��
		{
			//ǰ��33λ�������������λΪ301 403 506 ����� -20190625
			rt_strncpy(str_reg_time, "00000000", 8);
			rt_strncpy(&str_reg_time[5], &Sys_CB.Regi.regi[33], 3);
			
			if( rt_strncmp(str_reg_time, "00000209", 8)==0 || \
				rt_strncmp(str_reg_time, "00000301", 8)==0 || \
			    rt_strncmp(str_reg_time, "00000403", 8)==0 || \
				rt_strncmp(str_reg_time, "00000506", 8)==0)
			{
				
				for(i=0; i<36; i++)		
					Sys_CB.Regi.srt_regi[i] = Sys_CB.Regi.regi[i];
				
				rt_strncpy( Sys_CB.Time.str_reg_time, str_reg_time, 8);	
				
//				printf("errregtime =%8s\r\n",&Sys_CB.Time.str_reg_time[0]);

				Regi_setCMD_Process(&Sys_CB.Time.str_reg_time[0]);
			}

		}
		
		Sys_CB.Survey_Status.channel_one = Low_status;
		Sys_CB.Survey_Status.channel_two = High_status;
	
	}
	// SURVEY_STATUS��������
	else if(rt_strncmp(p,".survey_status",sizeof(".survey_status")-1)==0)
	{
		Num = sizeof(".survey_status")-1;
		Survey_Status_setCMD_Process(p+Num);
	}
	// SURVEY_MODEL��������
	else if(rt_strncmp(p,".survey_model",sizeof(".survey_model")-1)==0)
	{
		Num = sizeof(".survey_model")-1;
		Survey_Model_setCMD_Process(p+Num);
	}
	// DOGNUM��������
	else if(rt_strncmp(p,".dognum",sizeof(".dognum")-1)==0)
	{
		Num = sizeof(".dognum")-1;
		Dognum_setCMD_Process(p+Num);
	}
	// jump��������
	else if(rt_strncmp(p,".jump",sizeof(".jump")-1)==0)
	{
		Num = sizeof(".jump")-1;
		Jump_setCMD_Process(p+Num);
	}
	// reset��������
	else if(rt_strncmp(p,".reset",sizeof(".reset")-1)==0)
	{
		HAL_NVIC_SystemReset();
	}	
}
