/****************************************Copyright (c)**************************************************
**                               	�ɶ���Ϣ���̴�ѧ
**                                          
**
**                                    http://www.cuit.edu.cn
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: UltraSon_RCV.C
**��   ��   ��: 
**����޸�����: 2017��12��06��
**��        ��: ���������ݴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "ADC.h"
#include "DAC.h"
#include "RTC.h"
#include "TIME.h"
#include "DEF.h"
#include "ADC.H"
#include "UART_Parameters.H"
#include "UART_DEVICE.H"
#include "Ultrason_tran.H"
#include "Ultrason_rcv.H"
#include "Ultrason_Cal.h"

/****************�¼�����*******************/
struct rt_semaphore HFreqTranCMP_sem;				// ��Ƶ�źŷ�������¼�
struct rt_semaphore LFreqTranCMP_sem;				// ��Ƶ�źŷ�������¼�
struct rt_event HL_Pro_CMP_event;					// �ߵ�Ƶ�������

P_SOUNDER	p_Sounder_H = {
	&Sys_CB.Sounder.txwidth.channel_two,
	&Sys_CB.Sounder.clearwidth.channel_two,
	&Sys_CB.Sounder.power.channel_two,
	&Sys_CB.Sounder.gain.channel_two,
	&Sys_CB.Sounder.threshold.channel_two,
	&Sys_CB.Sounder.tvgs.channel_two,
	&Sys_CB.Sounder.filter.channel_two,
	&Sys_CB.Sounder.startgain.channel_two,
	&Sys_CB.Sounder.frequency.channel_two,
	&Sys_CB.Sounder.shallowsurveymode.channel_two,
	&Sys_CB.Sounder.forcedlockingecho.channel_two,
	&Sys_CB.Sounder.indentifyarithmetic.channel_two
};
P_SOUNDER	p_Sounder_L = {
	&Sys_CB.Sounder.txwidth.channel_one,
	&Sys_CB.Sounder.clearwidth.channel_one,
	&Sys_CB.Sounder.power.channel_one,
	&Sys_CB.Sounder.gain.channel_one,
	&Sys_CB.Sounder.threshold.channel_one,
	&Sys_CB.Sounder.tvgs.channel_one,
	&Sys_CB.Sounder.filter.channel_one,
	&Sys_CB.Sounder.startgain.channel_one,
	&Sys_CB.Sounder.frequency.channel_one,
	&Sys_CB.Sounder.shallowsurveymode.channel_one,
	&Sys_CB.Sounder.forcedlockingecho.channel_one,
	&Sys_CB.Sounder.indentifyarithmetic.channel_one
};
	

#ifdef HF_20us
	P_SYS_CB Sys_CB_H =	{
							&Sys_CB.Survey_Status.channel_two,	// ��Ƶ��ز���
							&Sys_CB.Survey_Model.channel_two,
							&p_Sounder_H,
							ADC1_Buf,					// Ҫ�����ADC��������
							2400,
							{
								21000,600,1200,2400,
								4730,8800,17000,21000
							},							// ���βɼ�����,Ҫ��������ݴ�С
							UART1_TxBufH,				// ���ڷ��ͻ���
							0,							// ʵ��Ҫ���͵����ݳ���
							0,							// ��������¼
							0,							// TVG����λ��ָʾ
							TVG_DAC2_Dat,				// TVG_DAC���ݱ�
							0.00002,					// �������ʱ�䣬��Ƶ100KHz����10us
							0,							// ˮ���¼
						};


#else  //HF_10us
	P_SYS_CB Sys_CB_H =	{
							&Sys_CB.Survey_Status.channel_two,	// ��Ƶ��ز���
							&Sys_CB.Survey_Model.channel_two,
							&p_Sounder_H,
							ADC1_Buf,					// Ҫ�����ADC��������
							2400,
							{
								42600,1200,2400,4800,
								9600,17200,34400,42600
							},						//  ���βɼ�����,Ҫ��������ݴ�С
							UART1_TxBufH,				// ���ڷ��ͻ���
							0,							// ʵ��Ҫ���͵����ݳ���
							0,							// ��������¼
							0,							// TVG����λ��ָʾ
							TVG_DAC2_Dat,				// TVG_DAC���ݱ�
							0.00001,					// �������ʱ�䣬��Ƶ100KHz����10us
							0,							// ˮ���¼
						};

#endif

						
#ifdef LF_50us
	P_SYS_CB Sys_CB_L =	{
							&Sys_CB.Survey_Status.channel_one,	// ��Ƶ��ز���
							&Sys_CB.Survey_Model.channel_one,
							&p_Sounder_L,
							ADC3_Buf,
							960,
							{
								8400,240,480,960,
								1892,3520,6800,8400
							},						//  ���βɼ�����,Ҫ��������ݴ�С
							UART1_TxBufL,
							0,
							0,							// ��������¼
							0,							// TVG����λ��ָʾ
							TVG_DAC1_Dat,				// TVG_DAC���ݱ�
							0.00005,//0.0001			// �������ʱ�䣬��Ƶ10KHz����100us
							0,
						};						
#else //LF_100us
	P_SYS_CB Sys_CB_L =	{
							&Sys_CB.Survey_Status.channel_one,	// ��Ƶ��ز���
							&Sys_CB.Survey_Model.channel_one,
							&p_Sounder_L,
							ADC3_Buf,
							240,
							{
								4260,120,240,480,
								860,1720,3440,4260
							},						//  ���βɼ�����,Ҫ��������ݴ�С
							UART1_TxBufL,
							0,
							0,							// ��������¼
							0,							// TVG����λ��ָʾ
							TVG_DAC1_Dat,				// TVG_DAC���ݱ�
							0.0001,			// �������ʱ�䣬��Ƶ10KHz����100us				
							0,
						};	
#endif



/********************************************************************************************************
** ��������: UltraSonDatFill
** ��������: ���������ݷ�װ����
**
** �䡡��: 	UltraSonDat
**
** �䡡��:  ��
**
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: 
** �ա���: 2017��12��06��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
//$ESPLUS	HF/LF	0x0A	0x12	0x34	0xA5	0xA5
//                 
//   ��ʶͷ �������� 1�ֽ�    2�ֽ�       2�ֽ�                    n�ֽ�
//   
//0xFF	0xFF	0xAA	0xBB	0x12	0x34	����.	0x43	0x10
//0xFF	0xFF	0xAA	0xBB	0x12	0x34	����.	0x43	0x10
//   ����

//һ��������Ϣ  0xFF 0xFF  ��ǰ���糤��   ���ֽڱ�ʾ
//              0xAA 0xBB ��ǰ�����һ�������������ʼ��λ�ü����
//              0x12 0x34 ��0x43 0x10  ��ǰ�������ֵ  һ���ֽ������㣬ÿ����ķ�ֵ��0-15��ʾ����������ʾ����Ϊ 12λAD���������λ�������㾫����ϳ�һ���ֽ�
//*	crc	\r\n

//$ESPLUS   ��ʶͷ
//HF/LF     ��Ƶ���ߵ�Ƶ������
//0x0A     ������������
//0x12 0x34  ��ǰ����ˮ��ֵ����ǰ���HF|LF��Ӧ����λcm
//0xA5 0xA5 �������ݰ����ܳ��ȣ���ǰλ�ÿ�ʼ(����0xA5 0Xa5)��*ǰֹ(������*��)
//0xFF 0xFF  ��ǰ������������
//0xAA 0xBB ���ڰ������ԭ����������
//0x12 0x34 ��043 0x10   ���������ֵ
//*
//Crc ���У��
//\r\n  
void UltraSonDatFill(P_SYS_CB *p_P_SYS_CB)
{
	uint16_t tmp_int;
	uint16_t Pack_Len = 0,Envelope_Len = 0,Envelope_Index,EnvelopeStart_Index;
	uint16_t First_Valid_Index = 0;							// ��¼���䲨��Ч�ĵ�һ��������
	float tmp_float;
	uint32_t i;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	*p_P_SYS_CB->p_UART_Dat = '$';
	*(p_P_SYS_CB->p_UART_Dat + 1) = 'E';
	*(p_P_SYS_CB->p_UART_Dat + 2) = 'S';
	*(p_P_SYS_CB->p_UART_Dat + 3) = 'P';
	*(p_P_SYS_CB->p_UART_Dat + 4) = 'L';
	*(p_P_SYS_CB->p_UART_Dat + 5) = 'U';
	*(p_P_SYS_CB->p_UART_Dat + 6) = 'S';
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'L';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
		*(p_P_SYS_CB->p_UART_Dat + 9) = 100;		// 100uS
	}
	else										// ��Ƶ
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'H';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
		*(p_P_SYS_CB->p_UART_Dat + 9) = 10;		// 10uS
	}
	tmp_float = p_P_SYS_CB->Water_Deep;
	tmp_float = tmp_float*100;					// ����100��,��λΪcm
	tmp_int = (uint16_t)tmp_float;
	*(p_P_SYS_CB->p_UART_Dat + 10) = tmp_int>>8;			// ˮ����ֽ�
	*(p_P_SYS_CB->p_UART_Dat + 11) = tmp_int;				// ˮ����ֽ�
	
	Envelope_Index = 18;									// �ó���ǰ�������������͵�ǰ�������ԭ������������4���ֽ�
	//for(i = p_P_SYS_CB->Ref_Start_Index;i<p_P_SYS_CB->UltraDat_Len;i++)
	for(i = 1;i<p_P_SYS_CB->UltraDat_Len;i++)
	{
		if(*(p_P_SYS_CB->p_UltraDat + i)&0xf0)
		{

			if((Envelope_Len%2)==0)
			{
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index + (Envelope_Len>>1)) = *(p_P_SYS_CB->p_UltraDat + i)&0xf0;
			}
			else
			{
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index + (Envelope_Len>>1)) |= *(p_P_SYS_CB->p_UltraDat + i)>>4;
			}
			Envelope_Len++;			
			if(Envelope_Len==1)			// ֻ�ڵ�һ������ʱ���¼����
			{
				if(First_Valid_Index==0)
				{
					First_Valid_Index = i;
				}
				EnvelopeStart_Index = i - First_Valid_Index;
			}
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
			{
				if((Envelope_Index + (Envelope_Len>>1))>((UART1TXL_Buf_Len)-50))			// ��ֹԽ��
				{					
					break;
				}
			}
			else
			{
				if((Envelope_Index + (Envelope_Len>>1))>((UART1TXH_Buf_Len)-150))			// ��ֹԽ��
				{
					break;
				}
			}
		}
		else
		{
			if(Envelope_Len)
			{
				Envelope_Len >>= 1;				// ���ʵ�ʵ������ֽ���
				Envelope_Len += 4;				// ����4���ֽڿռ䣬�����ֽ�����ƫ����
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// ��ǰ������������
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// ��ǰ�������ԭ����������
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
				Envelope_Index += Envelope_Len;
				Envelope_Len = 0;
			}
		}
	}
	// ���������ݲ�Ϊ0����Ҫ��β
	if(Envelope_Len)
	{
		Envelope_Len >>= 1;				// ���ʵ�ʵ������ֽ���
		Envelope_Len += 4;				// ����4���ֽڿռ䣬�����ֽ�����ƫ����
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// ��ǰ������������
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// ��ǰ�������ԭ����������
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
		Envelope_Index += Envelope_Len;
		Envelope_Len = 0;
	}
	Pack_Len = Envelope_Index - 4;
	*(p_P_SYS_CB->p_UART_Dat + 12) = (Pack_Len-12)>>8;					// �����ܳ��ȸ��ֽ�
	*(p_P_SYS_CB->p_UART_Dat + 13) = (Pack_Len-12);						// �����ܳ��ȵ��ֽ�
	*(p_P_SYS_CB->p_UART_Dat + Pack_Len) = '*';
	*(p_P_SYS_CB->p_UART_Dat + Pack_Len + 1) = do_crc(p_P_SYS_CB->p_UART_Dat,Pack_Len);
	*(p_P_SYS_CB->p_UART_Dat + Pack_Len + 2) = '\r';
	*(p_P_SYS_CB->p_UART_Dat + Pack_Len + 3) = '\n';
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)p_P_SYS_CB->p_UART_Dat,Pack_Len + 4);
//	for(i=0;i<20;i++)
//	{
//		ADC1_Buf[i] = 0xff;
//	}
//	HAL_UART_Transmit_DMA(&huart1,ADC1_Buf,2000);
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
	{	
		rt_event_send(&HL_Pro_CMP_event, (1 << 0));
	}	
	else
	{
		rt_event_send(&HL_Pro_CMP_event, (1 << 1));
	}
}



/********************************************************************************************************
** ��������: WaterDeep_Pro
** ��������: ���������ݴ������ˮ��
**
** �䡡��: 	p_P_SYS_CB
**
** �䡡��:  ʵ��ˮ��
**
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: 
** �ա���: 2017��12��06��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8_t WaterDeep_Pro(P_SYS_CB *p_P_SYS_CB)
{
	uint16_t i = 0,i_jump;
	uint16_t UtrBack_Index = 0;			// ��¼��Ч�ز��ڲ����е�λ��
	uint16_t Utr_Start_Index;			// ��¼���䲨��ʼ��
	uint8_t Up_Value = 0,UpFor_Count = 0;
	double	tmp;
	uint8_t DatPro_Sta = 0;
	
	if(*p_P_SYS_CB->p_Sounder->p_forcedlockingecho)
	{	// ���ǿ�������ز�,��ֱ���˳�ǰ��Ĳ��Σ����������
		tmp = p_P_SYS_CB->Water_Deep/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
		i_jump = (uint16_t)tmp;
	}
	else
	{
		// ��������ˮ��,��Ҫ�����Ĳ��ҵ���
		tmp = *p_P_SYS_CB->p_Sounder->p_clearwidth/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
		// �����ʵ�ʵĲ�����ʼ��
		i_jump = (uint16_t)2*tmp;
	}
	i = 0;
	Utr_Start_Index = 0;
	for(;i<p_P_SYS_CB->UltraDat_Len;i++)
	{
		if(*(p_P_SYS_CB->p_UltraDat + i)<(*p_P_SYS_CB->p_Sounder->p_threshold*10))
		{
			*(p_P_SYS_CB->p_UltraDat + i) = 0;	// �ز���ֵ�˳�
		}
		else
		{
			*(p_P_SYS_CB->p_UltraDat + i) -= (*p_P_SYS_CB->p_Sounder->p_threshold*10);	// �ز���ֵ�˳�
			switch(DatPro_Sta)
			{
				case 0:									// ������������
					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
					{
						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
						if(Utr_Start_Index==0)
						{
							Utr_Start_Index = i;			// ��¼�����ķ��䲨��ʼ��,����뷽ʽ
						}
						UpFor_Count++;
						if(UpFor_Count>=Num_Forward)		// ����3�������㼴��Ϊ�����ϵ�����
						{
							i = Utr_Start_Index + i_jump;	// ������������������
							Up_Value = 0;
							DatPro_Sta = 1;				// �������ֵ
							UtrBack_Index = 0;
							UpFor_Count = 0;
						}
					}
					else
					{
						UpFor_Count = 0;
						Up_Value = 0;
						Utr_Start_Index = 0;
					}
					break;
				case 1:									// �����ز���������
					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
					{
						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
						if(UtrBack_Index == 0)
							UtrBack_Index = i;
						UpFor_Count++;
						if(UpFor_Count>=Num_Forward)	// ����3�������㼴��Ϊ�����ϵ�����
						{
							DatPro_Sta = 2;				// ȷ���ز���ʼ�㣬���д���
						}
					}
					else
					{
						UpFor_Count = 0;
						Up_Value = 0;
						UtrBack_Index = 0;
					}
					break;
				case 2:										// �����ֵ
					tmp = UtrBack_Index - Utr_Start_Index;
					tmp = (p_P_SYS_CB->ADC_Samp_Time)*tmp;						// 
					tmp = Sys_CB.Parameters.velocity*tmp/2 + Sys_CB.Parameters.draft;	// ���ϳ�ˮ���
					p_P_SYS_CB->Water_Deep = tmp;
					if(tmp<Sys_CB.Parameters.minadepth)
					{// ǳˮ����ˮ�� 
					}
					else if(tmp>Sys_CB.Parameters.maxadepth)
					{// ��ˮ����ˮ��
					}
					return 1;
				default:break;
			}
		}
	}
	p_P_SYS_CB->MissReturn_Count++;
	if(p_P_SYS_CB->MissReturn_Count==Sys_CB.Parameters.missreturn)		// ������������,������
	{
		p_P_SYS_CB->MissReturn_Count = 0;
	}
	if(Sys_CB.Parameters.alarmstatus)
	{// �޻ر�����
	}
	return	0;			// ���󷵻�
}

uint8_t AutoWaterDeep_Pro(P_SYS_CB *p_P_SYS_CB)
{
//	uint16_t int_tmp = 0x00U;
//	uint8_t Grade_tmp;
//	uint32_t i = 0;
//	static uint32_t j_Index;
//	uint32_t UltraSon_Index = 0;		// ��¼��Ч�ز��ڲ����е�λ��
//	uint8_t Up_Value = 0,Down_Value = 0xFF,UpFor_Count = 0,DownFor_Count = 0;
//	double	tmp;
//	uint8_t DatPro_Sta = 0;
//	if(p_P_SYS_CB->p_Sounder->p_forcedlockingecho)
//	{	// ���ǿ�������ز�,��ֱ���˳�ǰ��Ĳ��Σ����������
//		tmp = p_P_SYS_CB->Water_Deep/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
//		i_jump = (uint16_t)tmp;
//	}
//	else
//	{
//		// ��������ˮ��,��Ҫ�����Ĳ��ҵ���
//		tmp = *p_P_SYS_CB->p_Sounder->p_clearwidth/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
//		// �����ʵ�ʵĲ�����ʼ��
//		i_jump = (uint16_t)tmp;
//	}
//	while(i<p_P_SYS_CB->UltraDat_Len)
//	{
//		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
//		{	
//			j_Index = p_P_SYS_CB->UltraDat_Len - hadc2.DMA_Handle->Instance->NDTR;
//		}
//		else
//		{
//			j_Index = p_P_SYS_CB->UltraDat_Len - hadc1.DMA_Handle->Instance->NDTR;
//		}
//		for(;i<j_Index;i++)
//		{
//			if(*(p_P_SYS_CB->p_UltraDat + i)<*p_P_SYS_CB->p_Sounder->p_threshold)
//			{
//				*(p_P_SYS_CB->p_UltraDat + i) = 0;	// �ز���ֵ�˳�
//			}
//			switch(DatPro_Sta)
//			{
//				case 0:									// ������������
//					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
//					{
//						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						UpFor_Count++;
//						if(UpFor_Count>4)				// ����4�������㼴��Ϊ�����ϵ�����
//						{
//							DatPro_Sta = 1;				// �������ֵ
//						}
//					}
//					else
//					{
//						UpFor_Count = 0;
//						Up_Value = 0;
//					}
//					break;
//				case 1:									// �������ֵ
//					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
//					{
//						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
//					}
//					else
//					{
//						UltraSon_Index = i - 1;			// ��¼���ܳ��ֵ����ֵ
//						Down_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						DownFor_Count++;
//						DatPro_Sta = 2;			// ���ܳ����½�����
//					}
//					break;
//				case 2:										// ȷ���ݼ�����
//					if(*(p_P_SYS_CB->p_UltraDat + i)<Down_Value)
//					{
//						Down_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						DownFor_Count++;
//						if(DownFor_Count>4)
//						{
//							tmp = UltraSon_Index - p_P_SYS_CB->Ref_Start_Index;
//							tmp = p_P_SYS_CB->ADC_Samp_Time*tmp;						// 
//							tmp = Sys_CB.Parameters.velocity*tmp/2 + Sys_CB.Parameters.draft;	// ���ϳ�ˮ���
//							p_P_SYS_CB->Water_Deep = tmp;
//							if(tmp<Sys_CB.Parameters.minadepth)
//							{// ǳˮ����ˮ�� 
//							}
//							else if(tmp>Sys_CB.Parameters.maxadepth)
//							{// ��ˮ����ˮ��
//							}
//							// �Զ�ȷ����λ
//							if(p_P_SYS_CB->Water_Deep<8)
//							{
//								Grade_tmp = 1;
//							}
//							else if(p_P_SYS_CB->Water_Deep<15)
//							{
//								Grade_tmp = 2;
//							}
//							else if(p_P_SYS_CB->Water_Deep<30)
//							{
//								Grade_tmp = 3;
//							}
//							else if(p_P_SYS_CB->Water_Deep<60)
//							{
//								Grade_tmp = 4;
//							}
//							else if(p_P_SYS_CB->Water_Deep<120)
//							{
//								Grade_tmp = 5;
//							}
//							else if(p_P_SYS_CB->Water_Deep<240)
//							{
//								Grade_tmp = 6;
//							}
//							else if(p_P_SYS_CB->Water_Deep<300)
//							{
//								Grade_tmp = 7;
//							}
//							int_tmp = p_P_SYS_CB->UltraDat_Len_Buf[Grade_tmp];
//							if(Grade_tmp!=Sys_CB.Grade.auto_grade)
//							{
//								Sys_CB.Grade.auto_grade = Grade_tmp;
//								getCMD_ACK(	"@SIC,,GET,GRADE,",				// �ϱ���λ
//									&Sys_CB.Grade.auto_grade,
//									Byte_Type
//									);
//							}
//							if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
//							{	
//								while((p_P_SYS_CB->UltraDat_Len - hadc2.DMA_Handle->Instance->NDTR)<int_tmp);
//								p_P_SYS_CB->UltraDat_Len = int_tmp;
//								HAL_TIM_Base_Stop(&htim3); 					// ֹͣTIM3����
//								HAL_ADC_Stop_DMA(&hadc1);
//							}
//							else
//							{
//								while((p_P_SYS_CB->UltraDat_Len - hadc1.DMA_Handle->Instance->NDTR)<int_tmp);
//								p_P_SYS_CB->UltraDat_Len = int_tmp;
//								HAL_TIM_Base_Stop(&htim2); 					// ֹͣTIM2����
//								HAL_ADC_Stop_DMA(&hadc2);
//							}
//							return 1;
//						}
//					}
//					else
//					{
//						DatPro_Sta = 3;									// �ų��Ƿ�Ϊ����
//					}
//					break;
//				case 3:						// ????
//					if(*(p_P_SYS_CB->p_UltraDat + i)<Down_Value)
//					{
//						DatPro_Sta = 2;
//					}
//					else							// ʧ�ܣ������ж�
//					{
//						Up_Value = UpFor_Count = DownFor_Count = 0;
//						Down_Value = 0xff;
//						DatPro_Sta = 0;
//					}
//					break;
//				default:break;
//			}
//		}
//		i = j_Index;
//		if(j_Index>p_P_SYS_CB->UltraDat_Len)
//			break;
//	}
//	p_P_SYS_CB->MissReturn_Count++;
//	if(Sys_CB.Parameters.alarmstatus)
//	{
//		// �޻ز�����
//		if(p_P_SYS_CB->MissReturn_Count==Sys_CB.Parameters.missreturn)		// ������������
//		{
//			p_P_SYS_CB->MissReturn_Count = 0;
//		}
//	}
	return	0;			// ���󷵻�
}
/********************************************************************************************************
** ��������: UltraSonDatPro
** ��������: ���������ݴ���
**
** �䡡��: 	p_P_SYS_CB��p_Grade
**
** �䡡��:  ��
**
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: 
** �ա���: 2017��12��06��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void UltraSonDatPro(P_SYS_CB *p_P_SYS_CB,GRADE *p_Grade)
{
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+HF;
		
	#ifdef Unmanned_ship //�ϵ��Զ�����
		*p_P_SYS_CB->p_Survey_Model = 1;
	#endif
	
		if(*p_P_SYS_CB->p_Survey_Model==0)		// �ֶ�����
		{
			if(pautoadjust->searchstatus != SEARCH_NONE && p_P_SYS_CB->p_UART_Dat==UART1_TxBufH)
				pautoadjust->searchstatus = SEARCH_NONE;
			 
			if(p_Grade->grade)
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[p_Grade->grade];
			else
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[Sys_CB.Grade.auto_grade];							
			

			switch(p_Grade->grade)
			{
				case 1:	
				
				case 2:		
					if(Sys_CB.Sounder.power.channel_one.Power_Index >= 4)
						Sys_CB.Sounder.power.channel_one.Power_Index = 4;					
					if(Sys_CB.Sounder.power.channel_two.Power_Index >= 4)
						Sys_CB.Sounder.power.channel_two.Power_Index = 4;
					break;
				case 3:	
					
				case 4:		
	
				case 5:	
					if(Sys_CB.Sounder.power.channel_one.Power_Index >= 6)
						Sys_CB.Sounder.power.channel_one.Power_Index = 6;					
					if(Sys_CB.Sounder.power.channel_two.Power_Index >= 6)
						Sys_CB.Sounder.power.channel_two.Power_Index = 6;
					break;						
				case 6:		
		
				case 7:	
					if(Sys_CB.Sounder.power.channel_one.Power_Index >= 8)
						Sys_CB.Sounder.power.channel_one.Power_Index = 8;					
					if(Sys_CB.Sounder.power.channel_two.Power_Index >= 8)
						Sys_CB.Sounder.power.channel_two.Power_Index = 8;
					break;
				default:break;														
			}

			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		 
			{
				rt_sem_take(&ADC3_CMP_sem, RT_WAITING_FOREVER); 
				TVG_Channel_One_Stop();
	
					*p_P_SYS_CB->p_Sounder->p_shallowsurveymode = *Sys_CB_H.p_Sounder->p_shallowsurveymode;//Ǳˮģʽ
					FiltWave_Excute(&Sys_CB_L, Channel_One);

			}
			else
			{
				rt_sem_take(&ADC1_CMP_sem, RT_WAITING_FOREVER);			 
				TVG_Channel_Two_Stop();

					FiltWave_Excute(&Sys_CB_H, Channel_Two);
	
			}
		}
		else if(*p_P_SYS_CB->p_Survey_Model==1)						// �Զ�����
		{
			if(p_Grade->grade)
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[p_Grade->grade];
			else
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[Sys_CB.Grade.auto_grade];			
			
			// �Զ�������ȫ������
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
			{
				rt_sem_take(&ADC3_CMP_sem, RT_WAITING_FOREVER);			 
				TVG_Channel_One_Stop();
				
				FiltWave_Excute(&Sys_CB_L, Channel_One);				
				Auto_LF_Adjust(&Sys_CB_L);							
			}
			else
			{
				rt_sem_take(&ADC1_CMP_sem, RT_WAITING_FOREVER);			 
				TVG_Channel_Two_Stop();
				
				FiltWave_Excute(&Sys_CB_H, Channel_Two);					
				Auto_HF_Adjust(&Sys_CB_H);					
			}
		}
//	}	
}

