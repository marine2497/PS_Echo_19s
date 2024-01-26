/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: UART_Parameters.c
**��   ��   ��: 
**����޸�����: 2018��9��20��
**��        ��: Parameters�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "drv_usart.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.h"
#include "UART_DEVICE.h"
#include "TIME.h"
#include "DAC.h"
#include "Ultrason_rcv.H"
#include "Regtime.h"

/*************************************************************************************************/
//	Parameters Survey�����
/*************************************************************************************************/
uint16_t Survey_ACKDat_Fill(char *p)
{
	uint16_t i = 0;
	i += Float_to_Str(p+i,Sys_CB_Copy.Parameters.draft);
	*(p + i++) = '|';
	i += Float_to_Str(p+i,Sys_CB_Copy.Parameters.velocity);
	*(p + i++) = '|';
	i += Float_to_Str(p+i,Sys_CB_Copy.Parameters.minadepth);
	*(p + i++) = '|';
	i += Float_to_Str(p+i,Sys_CB_Copy.Parameters.maxadepth);
	*(p + i++) = '|';
	i += Long_to_Str(p+i,Sys_CB_Copy.Parameters.alarmstatus);
	*(p + i++) = '|';
	i += Long_to_Str(p+i,Sys_CB_Copy.Parameters.alarmchannel);
	*(p + i++) = '|';
	i += Long_to_Str(p+i,Sys_CB_Copy.Parameters.missreturn);	
	return i;
}
void Survey_ALLChannel_getCMD_ACK(void)
{
	uint16_t Num;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,PARAMETERS.SURVEY.CHANNEL_ALL,OK,",sizeof("@SIC,,GET,PARAMETERS.SURVEY.CHANNEL_ALL,OK,")-1);
	Num += Survey_ACKDat_Fill(&UART1_TxBuf[Num]);
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
void Survey_SetDat_Fill(char *p)
{
	float tmp_float;
	uint32_t tmp_int;
	uint16_t i = 0;
	// 1.draft
	i += Str_to_float((p+i),&tmp_float);
	Sys_CB_Copy.Parameters.draft = tmp_float;
	// 2.velocity
	i += Str_to_float((p+i),&tmp_float);
	Sys_CB_Copy.Parameters.velocity = tmp_float;
	// 3.minAdepth
	i += Str_to_float((p+i),&tmp_float);
	Sys_CB_Copy.Parameters.minadepth = tmp_float;
	// 4.maxAdepth
	i += Str_to_float((p+i),&tmp_float);
	Sys_CB_Copy.Parameters.maxadepth = tmp_float;
	// 5.AlarmStatus
	i += Str_to_Long((p+i),&tmp_int);		
	Sys_CB_Copy.Parameters.alarmstatus = tmp_int;
	// 6.AlarmChannel
	i += Str_to_Long((p+i),&tmp_int);
	Sys_CB_Copy.Parameters.alarmchannel = tmp_int;
	// 7.MissReturn
	i += Str_to_Long((p+i),&tmp_int);
	Sys_CB_Copy.Parameters.missreturn = tmp_int;
	
	Save_Para_Process((uint32_t)(Sys_CB_Copy.Parameters.draft*100),(uint32_t)(Sys_CB_Copy.Parameters.velocity*100));
}
void Survey_ALLChannel_setCMD_ACK(void)
{
	uint16_t Num;
	CMD_Refresh = 1;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,PARAMETERS.SURVEY.CHANNAL_ALL,",sizeof("@SIC,,SET,PARAMETERS.SURVEY.CHANNAL_ALL,")-1);
	Num += Survey_ACKDat_Fill(&UART1_TxBuf[Num]);
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
void Survey_getCMD_Process(char *p)
{
//	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_all",sizeof(".channel_all")-1)==0)
	{
		Survey_ALLChannel_getCMD_ACK();
	}
//	// (1).draft ��ˮ ��λΪ�� ȡֵΪ0.00-9.99 Ĭ�ϣ�0.50
//	else if(rt_strncmp((const char *)p,".draft",sizeof(".draft")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.DRAFT,OK,",
//						&Sys_CB.Parameters.draft,
//						Float_Type
//						);
//	}
//	// (2).velocity ���� ��λ����/�� ȡֵΪ1300-1700 Ĭ�ϣ�1483
//	else if(rt_strncmp((const char *)p,".velocity",sizeof(".velocity")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.VELOCITY,OK,",
//					&Sys_CB.Parameters.velocity,
//					Int_Type
//					);
//	}
//	// (3).minAdepthǳˮ����ˮ�� ��λΪ�� ȡֵΪ0.50-9.99 Ĭ�ϣ�1
//	else if(rt_strncmp((const char *)p,".minadepth",sizeof(".minadepth")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.MINADEPTH,OK,",
//					&Sys_CB.Parameters.minadepth,
//					Float_Type
//					);
//	}
//	// (4).maxAdepth��ˮ����ˮ�� ��λΪ�� ȡֵΪ1.00-600.00 Ĭ�ϣ�100
//	else if(rt_strncmp((const char *)p,".maxadepth",sizeof(".maxadepth")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.MAXADEPTH,OK,",
//					&Sys_CB.Parameters.maxadepth,
//					Float_Type
//					);
//	}
//	// (5).AlarmStatus�޻ز�������ʶ 0Ϊ�أ�1Ϊ�� Ĭ�ϣ�0���رգ�
//	else if(rt_strncmp((const char *)p,".alarmstatus",sizeof(".alarmstatus")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.ALARMSTATUS,OK,",
//					&Sys_CB.Parameters.alarmstatus,
//					Byte_Type
//					);
//}
//	// (6).AlarmChannel�ο�����ͨ���� ȡֵ1-16 Ĭ�ϣ�1
//	else if(rt_strncmp((const char *)p,".alarmchannel",sizeof(".alarmchannel")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.ALARMCHANNEL,OK,",
//					&Sys_CB.Parameters.alarmchannel,
//					Byte_Type
//					);
//}
//	// (7).MissReturn��ʾ���������� ȡֵ1-10 Ĭ�ϣ�5
//	else if(rt_strncmp((const char *)p,".missreturn",sizeof(".missreturn")-1)==0)
//	{
//		getCMD_ACK(	"@SIC,,GET,PARAMETERS.SURVEY.MISSRETURN,OK,",
//					&Sys_CB.Parameters.missreturn,
//					Byte_Type
//					);
//	}
}
void Survey_setCMD_Process(char *p)
{
	uint16_t Num;
//	float tmp_float;
//	uint16_t tmp_int;
	
	char High_status =0;
	char Low_status =0;
	
	Low_status = Sys_CB.Survey_Status.channel_one;
	High_status = Sys_CB.Survey_Status.channel_two;
	

	Sys_CB.Survey_Status.channel_one = 0;
	Sys_CB.Survey_Status.channel_two = 0;
	rt_thread_delay(1000);
	
	
	if(rt_strncmp((const char *)p,".channel_all,",sizeof(".channel_all,")-1)==0)
	{
		Num = sizeof(".channel_all,")-1;
		Survey_SetDat_Fill(p+Num);
		Survey_ALLChannel_setCMD_ACK( );
	}
	
	Sys_CB.Survey_Status.channel_one = Low_status;
	Sys_CB.Survey_Status.channel_two = High_status;
}
/*************************************************************************************************/
//	Calibration�����
/*************************************************************************************************/
void Calibration_setCMD_Process(char *p)
{
	if(rt_strncmp((const char *)p,".depth",sizeof(".depth")-1)==0)
	{
		if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
		{
		}
		else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
		{
		}
	}
	else if(rt_strncmp((const char *)p,".status",sizeof(".status")-1)==0)
	{
		if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
		{

		}
		else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
		{
		}
	}
	else
	{
		if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
		{
		}
		else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
		{
		}
	}
}
/*************************************************************************************************/
//	Sounder�����ǲ��������
/*************************************************************************************************/
uint16_t Sounder_ACKDat_Fill(char *p,uint8_t Channel)
{
	uint16_t i = 0;
	if(Channel==Channel_One)
	{
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.txwidth.channel_one);
		*(p+i++) = '|';
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.clearwidth.channel_one);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.power.channel_one.Power_Index);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.threshold.channel_one);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.tvgs.channel_one);
		*(p+i++) = '|';
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.filter.channel_one);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.startgain.channel_one);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.frequency.channel_one);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.shallowsurveymode.channel_one);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.forcedlockingecho.channel_one);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one);

	}
	else
	{
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.txwidth.channel_two);
		*(p+i++) = '|';
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.clearwidth.channel_two);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.power.channel_two.Power_Index);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.threshold.channel_two);
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.tvgs.channel_two);
		*(p+i++) = '|';
		i += Float_to_Str((p+i),Sys_CB_Copy.Sounder.filter.channel_two);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.startgain.channel_two);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.frequency.channel_two);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.shallowsurveymode.channel_two);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.forcedlockingecho.channel_two);		
		*(p+i++) = '|';
		i += Long_to_Str((p+i),Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two);	
	}
	return i;
}
void Sounder_ALLChannel_getCMD_ACK(uint8_t Channel)
{
	uint16_t Num;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	if(Channel==Channel_One)
	{
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,PARAMETERS.SOUNDER.CHANNEL_ONE,OK,",sizeof("@SIC,,GET,PARAMETERS.SOUNDER.CHANNEL_ONE,OK,")-1);
	}
	else if(Channel==Channel_Two)
	{
		Num += my_strncpy(UART1_TxBuf,"@SIC,,GET,PARAMETERS.SOUNDER.CHANNEL_TWO,OK,",sizeof("@SIC,,GET,PARAMETERS.SOUNDER.CHANNEL_TWO,OK,")-1);
	}
	Num += Sounder_ACKDat_Fill(&UART1_TxBuf[Num],Channel);
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
	if(uart1_flag)
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);
	}
	else if(uart2_flag && (Sys_CB_Copy.Survey_Model.channel_two ==1 || Sys_CB_Copy.Survey_Model.channel_one ==1)) //��ҳ�����Զ�����ʱ�����ϴ��Զ��������ݵ���ҳ
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);		
	}	
	else if(uart2_flag) //��ҳ�����Զ�����ʱ�����ϴ��Զ��������ݵ���ҳ
	{
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);		
	}
	else
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);		
	}	
}
void Sounder_SetDat_Fill(char *p,uint8_t Channel)
{
	float tmp_float;
	uint32_t tmp_int;
	uint16_t i = 0;
	if(Channel==Channel_One && Sys_CB_Copy.Survey_Model.channel_one == 0)  /*�ֶ�ģʽ*/
	{
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.txwidth.channel_one = tmp_float;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.clearwidth.channel_one = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.power.channel_one.Power_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.threshold.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.tvgs.channel_one = tmp_int;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.filter.channel_one = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.startgain.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.frequency.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.shallowsurveymode.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.forcedlockingecho.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one = tmp_int;
				
		/*�޶�Ƶ�ʷ�Χ*/
		if(Sys_CB_Copy.Sounder.frequency.channel_one > 25 || Sys_CB_Copy.Sounder.frequency.channel_one < 15)
			Sys_CB_Copy.Sounder.frequency.channel_one = 20;	
		
//		if(Sys_CB_Copy.Sounder.shallowsurveymode.channel_two==0)
//		{
//			if(Sys_CB_Copy.Sounder.startgain.channel_one < 30)
//				Sys_CB_Copy.Sounder.startgain.channel_one = 30;
//		}			
	}
	else if(Channel==Channel_One && Sys_CB_Copy.Survey_Model.channel_one==1)  /*�Զ�ģʽ*/
	{
		i += Str_to_float((p+i),&tmp_float);
//		Sys_CB_Copy.Sounder.txwidth.channel_one = tmp_float;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.clearwidth.channel_one = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.power.channel_one.Power_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.threshold.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.tvgs.channel_one = tmp_int;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.filter.channel_one = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.startgain.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.frequency.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.shallowsurveymode.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.forcedlockingecho.channel_one = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one = tmp_int;
				
		/*�޶�Ƶ�ʷ�Χ*/
		if(Sys_CB_Copy.Sounder.frequency.channel_one > 25 || Sys_CB_Copy.Sounder.frequency.channel_one < 15)
			Sys_CB_Copy.Sounder.frequency.channel_one = 20;		
			
	}	
	else if(Channel==Channel_Two && Sys_CB_Copy.Survey_Model.channel_two == 0)
	{
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.txwidth.channel_two = tmp_float;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.clearwidth.channel_two = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.power.channel_two.Power_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.threshold.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.tvgs.channel_two = tmp_int;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.filter.channel_two = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.startgain.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.frequency.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.shallowsurveymode.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.forcedlockingecho.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two = tmp_int;
		
		if(Sys_CB_Copy.Sounder.frequency.channel_two > 215 || Sys_CB_Copy.Sounder.frequency.channel_two < 190)
			Sys_CB_Copy.Sounder.frequency.channel_two = 200;	
//		if(Sys_CB_Copy.Sounder.shallowsurveymode.channel_two==0)
//		{
//			if(Sys_CB_Copy.Sounder.startgain.channel_two < 38)
//				Sys_CB_Copy.Sounder.startgain.channel_two = 38;
//		}			
	}
	else if(Channel==Channel_Two && Sys_CB_Copy.Survey_Model.channel_two == 1)
	{
		i += Str_to_float((p+i),&tmp_float);
//		Sys_CB_Copy.Sounder.txwidth.channel_two = tmp_float;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.clearwidth.channel_two = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.power.channel_two.Power_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.threshold.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.tvgs.channel_two = tmp_int;
		i += Str_to_float((p+i),&tmp_float);
		Sys_CB_Copy.Sounder.filter.channel_two = tmp_float;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.startgain.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
//		Sys_CB_Copy.Sounder.frequency.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.shallowsurveymode.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.forcedlockingecho.channel_two = tmp_int;
		i += Str_to_Long((p+i),&tmp_int);
		Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two = tmp_int;
		
		if(Sys_CB_Copy.Sounder.frequency.channel_two > 215 || Sys_CB_Copy.Sounder.frequency.channel_two < 190)
			Sys_CB_Copy.Sounder.frequency.channel_two = 200;	
				
	}	
}
void Sounder_ALLChannel_setCMD_ACK(uint8_t Channel)
{
	uint16_t Num;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	if(Channel==Channel_One)
	{
		Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,PARAMETERS.SOUNDER.CHANNAL_ONE,",sizeof("@SIC,,SET,PARAMETERS.SOUNDER.CHANNAL_ONE,")-1);
	}
	else if(Channel==Channel_Two)
	{
		Num += my_strncpy(UART1_TxBuf,"@SIC,,SET,PARAMETERS.SOUNDER.CHANNAL_TWO,",sizeof("@SIC,,SET,PARAMETERS.SOUNDER.CHANNAL_TWO,")-1);
	}
	Num += Sounder_ACKDat_Fill(&UART1_TxBuf[Num],Channel);
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
	CMD_Refresh = 1;
}
void Sounder_getCMD_Process(char *p)
{
	uint16_t Num;
	if(rt_strncmp((const char *)p,".channel_one",sizeof(".channel_one")-1)==0)
	{
		Sounder_ALLChannel_getCMD_ACK(Channel_One);
	}
	else if(rt_strncmp((const char *)p,".channel_two",sizeof(".channel_two")-1)==0)
	{
		Sounder_ALLChannel_getCMD_ACK(Channel_Two);
	}		
	//��1��.	txwidth�������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
	//#sic,,get,parameters.sounder.txwidth.channel_one\r\n
	//@SIC,,GET,PARAMETERS.SOUNDER.TXWIDTH.CHANNEL_ONE,OK,0.05* crc \r\n 
	
	else if(rt_strncmp((const char *)p,".txwidth",sizeof(".txwidth")-1)==0)
	{
		Num = sizeof(".txwidth")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.TXWIDTH.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.txwidth.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.TXWIDTH.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.txwidth.channel_two,
						Float_Type
						);
		}
	}
	//��2��.	clearwidth����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8
	//0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
	else if(rt_strncmp((const char *)p,".clearwidth",sizeof(".clearwidth")-1)==0)
	{
		Num = sizeof(".clearwidth")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.CLEARWIDTH.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.clearwidth.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.CLEARWIDTH.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.clearwidth.channel_two,
						Float_Type
						);
		}		
	}
	//��3��.	power���� ȡֵ1-8 Ĭ�ϣ�2
	else if(rt_strncmp((const char *)p,".power",sizeof(".power")-1)==0)
	{
		Num = sizeof(".power")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.POWER.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.power.channel_one.Power_Index,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.POWER.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.power.channel_two.Power_Index,
						Byte_Type
						);
		}		
	}
	//��4��.	gain���� ȡֵ1-8 Ĭ�ϣ�3
	else if(rt_strncmp((const char *)p,".gain",sizeof(".gain")-1)==0)
	{
		Num = sizeof(".gain")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.GAIN.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.GAIN.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index,
						Byte_Type
						);
		}		
	}
	//��5��.	threshold�ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
	else if(rt_strncmp((const char *)p,".threshold",sizeof(".threshold")-1)==0)
	{
		Num = sizeof(".threshold")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.THRESHOLD.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.threshold.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.THRESHOLD.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.threshold.channel_two,
						Byte_Type
						);
		}		
	}
	//��6��.	tvgs TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
	else if(rt_strncmp((const char *)p,".tvgs",sizeof(".tvgs")-1)==0)
	{
		Num = sizeof(".tvgs")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.TVGS.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.threshold.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.TVGS.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.threshold.channel_two,
						Byte_Type
						);
		}
	}
	//��7��.	filterʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
	else if(rt_strncmp((const char *)p,".filter",sizeof(".filter")-1)==0)
	{
		Num = sizeof(".filter")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FILTER.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.filter.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FILTER.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.filter.channel_two,
						Float_Type
						);
		}		
	}
	//��8��.	Startgain��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
	else if(rt_strncmp((const char *)p,".startgain",sizeof(".startgain")-1)==0)
	{
		Num = sizeof(".startgain")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.STARTGAIN.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.startgain.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.STARTGAIN.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.startgain.channel_two,
						Byte_Type
						);
		}
	}
	//��9��.	frequencyƵ��
	//��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ
	//��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
	else if(rt_strncmp((const char *)p,".frequency",sizeof(".frequency")-1)==0)
	{
		Num = sizeof(".frequency")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FREQUENCY.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.frequency.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FREQUENCY.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.frequency.channel_two,
						Byte_Type
						);
		}		
	}
	//��10��.	shallowSurveyModeǳˮ����ģʽ
	//ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	else if(rt_strncmp((const char *)p,".shallowsurveymode",sizeof(".shallowsurveymode")-1)==0)
	{
		Num = sizeof(".shallowsurveymode")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.SHALLOWSURVEMODE.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.shallowsurveymode.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.SHALLOWSURVEMODE.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.shallowsurveymode.channel_two,
						Byte_Type
						);
		}		
	}
	//��11��.	ForcedLockingEchoǿ�������ز�
	//ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	else if(rt_strncmp((const char *)p,".forcedlockingecho",sizeof(".forcedlockingecho")-1)==0)
	{
		Num = sizeof(".forcedlockingecho")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FORCELOCKINGECHO.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.forcedlockingecho.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.FORCELOCKINGECHO.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.forcedlockingecho.channel_two,
						Byte_Type
						);
		}			
	}
	//��12��.	indentifyArithmetic�ز��ж�
	//ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز� 
	//Ĭ�ϣ�1
	else if(rt_strncmp((const char *)p,".indentifyarithmetic",sizeof(".indentifyarithmetic")-1)==0)
	{
		Num = sizeof(".indentifyarithmetic")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one",sizeof(".channel_one")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.INDENTIFYARITHMETIC.CHANNEL_ONE,OK,",
						&Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two",sizeof(".channel_two")-1)==0)
		{
			getCMD_ACK(	"@SIC,,GET,PARAMETERS.SOUNDER.INDENTIFYARITHMETIC.CHANNEL_TWO,OK,",
						&Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two,
						Byte_Type
						);
		}		
	}
}
void Sounder_setCMD_Process(char *p)
{
	uint16_t Num,i;
	float tmp_float;
	uint32_t tmp_int;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;
	
	if(rt_strncmp((const char *)p,".channel_one,",sizeof(".channel_one,")-1)==0)
	{
		Num = sizeof(".channel_one,")-1;
		Sounder_SetDat_Fill(p+Num,Channel_One);
		Sounder_ALLChannel_setCMD_ACK(Channel_One);
		
		
		if((Sys_CB_Copy.Sounder.frequency.channel_one!=Sys_CB.Sounder.frequency.channel_one)||(Sys_CB_Copy.Sounder.txwidth.channel_one!=Sys_CB.Sounder.txwidth.channel_one))
		{
			if(Sys_CB_Copy.Sounder.txwidth.channel_one*100<10)
				Sys_CB_Copy.Sounder.txwidth.channel_one = 0.1;
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
			htim8.Instance->ARR  = tmp_freq;		// ֱ�Ӹ��¼Ĵ���
			htim8.Instance->CCR1 = tmp_freq>>1;		// ֱ�Ӹ��¼Ĵ���
			htim8.Instance->RCR  = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���
		}
//		if(Sys_CB_Copy.Sounder.startgain.channel_one!=Sys_CB.Sounder.startgain.channel_one)
//		{
//			for(i=0;i<TVG_DAC_Num;i++)
//			{
//				*(uint16_t* )(Sys_CB_L.p_TVG_Dat + i) = 68*Sys_CB_Copy.Sounder.startgain.channel_one;			// ��������
//			}
//		}
	}
	else if(rt_strncmp((const char *)p,".channel_two,",sizeof(".channel_two,")-1)==0)
	{
		Num = sizeof(".channel_two,")-1;
		Sounder_SetDat_Fill(p+Num,Channel_Two);
		Sounder_ALLChannel_setCMD_ACK(Channel_Two);
		if((Sys_CB_Copy.Sounder.frequency.channel_two!=Sys_CB.Sounder.frequency.channel_two)||(Sys_CB_Copy.Sounder.txwidth.channel_two!=Sys_CB.Sounder.txwidth.channel_two))
		{
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
			htim1.Instance->ARR = tmp_freq;			// ֱ�Ӹ��¼Ĵ���
			htim1.Instance->CCR1 = tmp_freq>>1;		// ֱ�Ӹ��¼Ĵ���
			htim1.Instance->RCR = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���		
		}
//		if(Sys_CB_Copy.Sounder.startgain.channel_two!=Sys_CB.Sounder.startgain.channel_two)
//		{
//			for(i=0;i<TVG_DAC_Num;i++)
//			{
//				*(uint16_t* )(Sys_CB_H.p_TVG_Dat + i) = 68*Sys_CB_Copy.Sounder.startgain.channel_two;			// ���ԣ���������
//			}
//		}
	}
	//��1��.	txwidth�������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
	//#sic,,set,parameters.sounder.txwidth.channel_one,0.05\r\n
	//@SIC,,SET,PARAMETERS.SOUNDER.TXWIDTH.CHANNEL_ONE,0.05,OK* crc \r\n
	else if(rt_strncmp((const char *)p,".txwidth",sizeof(".txwidth")-1)==0)
	{
		Num = sizeof(".txwidth")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.txwidth.channel_one = tmp_float;
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
			htim8.Instance->RCR = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.TXWIDTH.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.txwidth.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.txwidth.channel_two = tmp_float;
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
			htim1.Instance->RCR = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.TXWIDTH.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.txwidth.channel_two,
						Float_Type
						);
		}
	}
	//��2��.	clearwidth����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8
	//0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
	else if(rt_strncmp((const char *)p,".clearwidth",sizeof(".clearwidth")-1)==0)
	{
		Num = sizeof(".clearwidth")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.clearwidth.channel_one = tmp_float;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.CLEARWIDTH.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.clearwidth.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.clearwidth.channel_two = tmp_float;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.CLEARWIDTH.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.clearwidth.channel_two,
						Float_Type
						);
		}
	}
	//��3��.	power���� ȡֵ1-8 Ĭ�ϣ�2
	else if(rt_strncmp((const char *)p,".power",sizeof(".power")-1)==0)
	{
		Num = sizeof(".power")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.POWR.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.power.channel_one.Power_Index,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.POWER.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.power.channel_two.Power_Index,
						Byte_Type
						);
		}
	}
	//��4��.	gain���� ȡֵ0-8 Ĭ�ϣ�3
	else if(rt_strncmp((const char *)p,".gain",sizeof(".gain")-1)==0)
	{
		Num = sizeof(".gain")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index = tmp_int;

			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.GAIN.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index = tmp_int;

			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.GAIN.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.gain.channel_two.Gain_Index,
						Byte_Type
						);
		}
	}
	//��5��.	threshold�ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
	else if(rt_strncmp((const char *)p,".threshold",sizeof(".threshold")-1)==0)
	{
		Num = sizeof(".threshold")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.threshold.channel_one = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.THRESHLOD.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.threshold.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.threshold.channel_two = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.THRESHLOD.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.threshold.channel_two,
						Byte_Type
						);
		}
	}
	//��6��.	tvgs TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
	else if(rt_strncmp((const char *)p,".tvgs",sizeof(".tvgs")-1)==0)
	{
		Num = sizeof(".tvgs")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.tvgs.channel_one = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.TVGS.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.tvgs.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.tvgs.channel_two = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.TVGS.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.tvgs.channel_two,
						Byte_Type
						);
		}
	}
	//��7��.	filterʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
	else if(rt_strncmp((const char *)p,".filter",sizeof(".filter")-1)==0)
	{
		Num = sizeof(".filter")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.filter.channel_one = tmp_float;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FILTER.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.filter.channel_one,
						Float_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_float((p+Num),&tmp_float);
			Sys_CB_Copy.Sounder.filter.channel_two = tmp_float;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FILTER.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.filter.channel_two,
						Float_Type
						);
		}
	}
	//��8��.	Startgain��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
	else if(rt_strncmp((const char *)p,".startgain",sizeof(".startgain")-1)==0)
	{
		Num = sizeof(".startgain")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.startgain.channel_one = tmp_int;
			for(i=0;i<TVG_DAC_Num;i++)
			{
				//*(uint32_t* )(Sys_CB_L.p_TVG_Dat + i) = Sys_CB_Copy.Sounder.gain.channel_one.Gain_ADC_Value[Sys_CB_Copy.Sounder.gain.channel_one.Gain_Index];
				*(uint16_t* )(Sys_CB_L.p_TVG_Dat + i) = 68*tmp_int;			// ���ԣ���������
			}
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.STARTGAIN.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.startgain.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.startgain.channel_two = tmp_int;
			for(i=0;i<TVG_DAC_Num;i++)
			{			
				*(uint16_t* )(Sys_CB_H.p_TVG_Dat + i) = 68*tmp_int;			// ���ԣ���������
			}
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.STARTGAIN.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.startgain.channel_two,
						Byte_Type
						);
		}
	}
	//��9��.	frequencyƵ��
	//��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ
	//��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
	else if(rt_strncmp((const char *)p,".frequency",sizeof(".frequency")-1)==0)
	{
		Num = sizeof(".frequency")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.frequency.channel_one = tmp_int;
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
			htim8.Instance->ARR = tmp_freq;			// ֱ�Ӹ��¼Ĵ���
			htim8.Instance->CCR1 = tmp_freq>>1;		// ֱ�Ӹ��¼Ĵ���
			htim8.Instance->RCR = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FREQUENCY.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.frequency.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.frequency.channel_two = tmp_int;
			tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
			htim1.Instance->ARR = tmp_freq;			// ֱ�Ӹ��¼Ĵ���
			htim1.Instance->CCR1 = tmp_freq>>1;		// ֱ�Ӹ��¼Ĵ���
			htim1.Instance->RCR = tmp_txwidth;		// ֱ�Ӹ��¼Ĵ���
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FREQUENCY.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.frequency.channel_two,
						Byte_Type
						);
		}
	}
	//��10��.	shallowSurveyModeǳˮ����ģʽ
	//ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	else if(rt_strncmp((const char *)p,".shallowsurveymode",sizeof(".shallowsurveymode")-1)==0)
	{
		Num = sizeof(".shallowsurveymode")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.shallowsurveymode.channel_one = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.SHALLOWSURVEYMODE.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.shallowsurveymode.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.shallowsurveymode.channel_two = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.SHALLOWSURVEYMODE.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.shallowsurveymode.channel_two,
						Byte_Type
						);
		}
	}
	//��11��.	ForcedLockingEchoǿ�������ز�
	//ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	else if(rt_strncmp((const char *)p,".forcedlockingecho",sizeof("forcedlockingecho")-1)==0)
	{
		Num = sizeof(".forcedlockingecho")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.forcedlockingecho.channel_one = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FORCEDLOCKINGECHO.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.forcedlockingecho.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.forcedlockingecho.channel_two = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.FORCEDLOCKINGECHO.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.forcedlockingecho.channel_two,
						Byte_Type
						);
		}
	}
	//��12��.	indentifyArithmetic�ز��ж�
	//ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز� 
	//Ĭ�ϣ�1
	else if(rt_strncmp((const char *)p,".indentifyarithmetic",sizeof(".indentifyarithmetic")-1)==0)
	{
		Num = sizeof(".indentifyarithmetic")-1;
		if(rt_strncmp((const char *)(p+Num),".channel_one,",sizeof(".channel_one,")-1)==0)
		{
			Num += sizeof(".channel_one,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.INDENTIFYARITHMETIC.CHANNEL_ONE,",
						&Sys_CB_Copy.Sounder.indentifyarithmetic.channel_one,
						Byte_Type
						);
		}
		else if(rt_strncmp((const char *)(p+Num),".channel_two,",sizeof(".channel_two,")-1)==0)
		{
			Num += sizeof(".channel_two,")-1;
			Str_to_Long((p+Num),&tmp_int);
			Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two = tmp_int;
			setCMD_ACK(	"@SIC,,SET,PARAMETERS.SURVEY.INDENTIFYARITHMETIC.CHANNEL_TWO,",
						&Sys_CB_Copy.Sounder.indentifyarithmetic.channel_two,
						Byte_Type
						);
		}
	}
}
/*************************************************************************************************/
//	PARAMETERS GET����
/*************************************************************************************************/
void PARAMETERS_getCMD(char *p)
{
	uint16_t Num;
	// SURVEY ����
	if(rt_strncmp(p,".survey",sizeof(".survey")-1)==0)
	{
		Num = sizeof(".survey")-1;
		Survey_getCMD_Process(p+Num);
	}	
	// SOUNDER ����
	else if(rt_strncmp(p,".sounder",sizeof(".sounder")-1)==0)
	{
		Num = sizeof(".sounder")-1;
		Sounder_getCMD_Process(p+Num);
	}	
}
/*************************************************************************************************/
//	PARAMETERS SET����
/*************************************************************************************************/
void PARAMETERS_setCMD(char *p)
{
		uint16_t Num;
	// SURVEY ����
	
	
	if(rt_strncmp(p,".survey",sizeof(".survey")-1)==0)
	{
		Num = sizeof(".survey")-1;
		Survey_setCMD_Process(p+Num);
	}	
	// CALIBRATION ����
	else if(rt_strncmp(p,".calibration",sizeof(".calibration")-1)==0)
	{
		Num = sizeof(".calibration")-1;
		Calibration_setCMD_Process(p+Num);
	}
	// SOUNDER ����
	else if(rt_strncmp(p,".sounder",sizeof(".sounder")-1)==0)
	{
		Num = sizeof(".sounder")-1;
		Sounder_setCMD_Process(p+Num);
	}
}

//ת������
void Conversion_Display(char *p)
{
	uint16_t Num;
	
	if(rt_strncmp(p,"type",sizeof("type")-1)==0)
	{
		Num = sizeof("type,") - 1;
		if(*(p+Num) == '1')
        {
			Transform.Transform_Two = 1;
        }
		else if(*(p+Num) == '2')
        {
			Transform.Transform_Two = 2;
        }
        else if(*(p+Num) == '3')
        {
			Transform.Transform_Two = 3;
        }
        else if(*(p+Num) == '4')
        {
			Transform.Transform_Two = 4;
        }    
		else if(*(p+Num) == '5')
        {
			Transform.Transform_Two = 5;
        }   		
		Tran_setCMD_ACK("@SIC,,SET,DATA.TYPE,",&Transform.Transform_Two,Byte_Type);

	}
}
