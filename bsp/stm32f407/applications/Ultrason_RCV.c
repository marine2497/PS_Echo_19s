/****************************************Copyright (c)**************************************************
**                               	成都信息工程大学
**                                          
**
**                                    http://www.cuit.edu.cn
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: UltraSon_RCV.C
**创   建   人: 
**最后修改日期: 2017年12月06日
**描        述: 超声波数据处理
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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

/****************事件定义*******************/
struct rt_semaphore HFreqTranCMP_sem;				// 高频信号发送完毕事件
struct rt_semaphore LFreqTranCMP_sem;				// 低频信号发送完毕事件
struct rt_event HL_Pro_CMP_event;					// 高低频处理完毕

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
							&Sys_CB.Survey_Status.channel_two,	// 高频相关参数
							&Sys_CB.Survey_Model.channel_two,
							&p_Sounder_H,
							ADC1_Buf,					// 要处理的ADC采样数据
							2400,
							{
								21000,600,1200,2400,
								4730,8800,17000,21000
							},							// 当次采集点数,要处理的数据大小
							UART1_TxBufH,				// 串口发送缓存
							0,							// 实际要发送的数据长度
							0,							// 丢波数记录
							0,							// TVG数据位置指示
							TVG_DAC2_Dat,				// TVG_DAC数据表
							0.00002,					// 采样间隔时间，高频100KHz，即10us
							0,							// 水深记录
						};


#else  //HF_10us
	P_SYS_CB Sys_CB_H =	{
							&Sys_CB.Survey_Status.channel_two,	// 高频相关参数
							&Sys_CB.Survey_Model.channel_two,
							&p_Sounder_H,
							ADC1_Buf,					// 要处理的ADC采样数据
							2400,
							{
								42600,1200,2400,4800,
								9600,17200,34400,42600
							},						//  当次采集点数,要处理的数据大小
							UART1_TxBufH,				// 串口发送缓存
							0,							// 实际要发送的数据长度
							0,							// 丢波数记录
							0,							// TVG数据位置指示
							TVG_DAC2_Dat,				// TVG_DAC数据表
							0.00001,					// 采样间隔时间，高频100KHz，即10us
							0,							// 水深记录
						};

#endif

						
#ifdef LF_50us
	P_SYS_CB Sys_CB_L =	{
							&Sys_CB.Survey_Status.channel_one,	// 低频相关参数
							&Sys_CB.Survey_Model.channel_one,
							&p_Sounder_L,
							ADC3_Buf,
							960,
							{
								8400,240,480,960,
								1892,3520,6800,8400
							},						//  当次采集点数,要处理的数据大小
							UART1_TxBufL,
							0,
							0,							// 丢波数记录
							0,							// TVG数据位置指示
							TVG_DAC1_Dat,				// TVG_DAC数据表
							0.00005,//0.0001			// 采样间隔时间，低频10KHz，即100us
							0,
						};						
#else //LF_100us
	P_SYS_CB Sys_CB_L =	{
							&Sys_CB.Survey_Status.channel_one,	// 低频相关参数
							&Sys_CB.Survey_Model.channel_one,
							&p_Sounder_L,
							ADC3_Buf,
							240,
							{
								4260,120,240,480,
								860,1720,3440,4260
							},						//  当次采集点数,要处理的数据大小
							UART1_TxBufL,
							0,
							0,							// 丢波数记录
							0,							// TVG数据位置指示
							TVG_DAC1_Dat,				// TVG_DAC数据表
							0.0001,			// 采样间隔时间，低频10KHz，即100us				
							0,
						};	
#endif



/********************************************************************************************************
** 函数名称: UltraSonDatFill
** 功能描述: 超声波数据封装发送
**
** 输　入: 	UltraSonDat
**
** 输　出:  无
**
** 全局变量: 无
** 调用模块: 无
**
** 作　者: 
** 日　期: 2017年12月06日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
//$ESPLUS	HF/LF	0x0A	0x12	0x34	0xA5	0xA5
//                 
//   标识头 数据类型 1字节    2字节       2字节                    n字节
//   
//0xFF	0xFF	0xAA	0xBB	0x12	0x34	…….	0x43	0x10
//0xFF	0xFF	0xAA	0xBB	0x12	0x34	…….	0x43	0x10
//   ……

//一个包络信息  0xFF 0xFF  当前包络长度   两字节表示
//              0xAA 0xBB 当前包络第一个采样点距离起始点位置间隔数
//              0x12 0x34 …0x43 0x10  当前采样点幅值  一个字节两个点，每个点的幅值由0-15表示，单个点显示精度为 12位AD采样最高四位。两个点精度组合成一个字节
//*	crc	\r\n

//$ESPLUS   标识头
//HF/LF     高频或者低频的数据
//0x0A     采样点间隔周期
//0x12 0x34  当前测量水深值，与前面的HF|LF对应，单位cm
//0xA5 0xA5 当属数据包的总长度：当前位置开始(包括0xA5 0Xa5)到*前止(不包括*号)
//0xFF 0xFF  当前包络采样点个数
//0xAA 0xBB 当期包络距离原点间隔周期数
//0x12 0x34 …043 0x10   采样点采样值
//*
//Crc 异或校验
//\r\n  
void UltraSonDatFill(P_SYS_CB *p_P_SYS_CB)
{
	uint16_t tmp_int;
	uint16_t Pack_Len = 0,Envelope_Len = 0,Envelope_Index,EnvelopeStart_Index;
	uint16_t First_Valid_Index = 0;							// 记录发射波有效的第一个点坐标
	float tmp_float;
	uint32_t i;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	*p_P_SYS_CB->p_UART_Dat = '$';
	*(p_P_SYS_CB->p_UART_Dat + 1) = 'E';
	*(p_P_SYS_CB->p_UART_Dat + 2) = 'S';
	*(p_P_SYS_CB->p_UART_Dat + 3) = 'P';
	*(p_P_SYS_CB->p_UART_Dat + 4) = 'L';
	*(p_P_SYS_CB->p_UART_Dat + 5) = 'U';
	*(p_P_SYS_CB->p_UART_Dat + 6) = 'S';
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'L';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
		*(p_P_SYS_CB->p_UART_Dat + 9) = 100;		// 100uS
	}
	else										// 高频
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'H';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
		*(p_P_SYS_CB->p_UART_Dat + 9) = 10;		// 10uS
	}
	tmp_float = p_P_SYS_CB->Water_Deep;
	tmp_float = tmp_float*100;					// 扩大100倍,单位为cm
	tmp_int = (uint16_t)tmp_float;
	*(p_P_SYS_CB->p_UART_Dat + 10) = tmp_int>>8;			// 水深高字节
	*(p_P_SYS_CB->p_UART_Dat + 11) = tmp_int;				// 水深低字节
	
	Envelope_Index = 18;									// 让出当前包络采样点个数和当前包络距离原点间隔周期数共4个字节
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
			if(Envelope_Len==1)			// 只在第一个数的时候记录坐标
			{
				if(First_Valid_Index==0)
				{
					First_Valid_Index = i;
				}
				EnvelopeStart_Index = i - First_Valid_Index;
			}
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
			{
				if((Envelope_Index + (Envelope_Len>>1))>((UART1TXL_Buf_Len)-50))			// 防止越界
				{					
					break;
				}
			}
			else
			{
				if((Envelope_Index + (Envelope_Len>>1))>((UART1TXH_Buf_Len)-150))			// 防止越界
				{
					break;
				}
			}
		}
		else
		{
			if(Envelope_Len)
			{
				Envelope_Len >>= 1;				// 算出实际的数据字节数
				Envelope_Len += 4;				// 加上4个字节空间，即包字节数和偏移量
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// 当前包络采样点个数
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// 当前包络距离原点间隔周期数
				*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
				Envelope_Index += Envelope_Len;
				Envelope_Len = 0;
			}
		}
	}
	// 如果最后数据不为0，需要结尾
	if(Envelope_Len)
	{
		Envelope_Len >>= 1;				// 算出实际的数据字节数
		Envelope_Len += 4;				// 加上4个字节空间，即包字节数和偏移量
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// 当前包络采样点个数
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// 当前包络距离原点间隔周期数
		*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
		Envelope_Index += Envelope_Len;
		Envelope_Len = 0;
	}
	Pack_Len = Envelope_Index - 4;
	*(p_P_SYS_CB->p_UART_Dat + 12) = (Pack_Len-12)>>8;					// 包的总长度高字节
	*(p_P_SYS_CB->p_UART_Dat + 13) = (Pack_Len-12);						// 包的总长度低字节
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
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
	{	
		rt_event_send(&HL_Pro_CMP_event, (1 << 0));
	}	
	else
	{
		rt_event_send(&HL_Pro_CMP_event, (1 << 1));
	}
}



/********************************************************************************************************
** 函数名称: WaterDeep_Pro
** 功能描述: 超声波数据处理计算水深
**
** 输　入: 	p_P_SYS_CB
**
** 输　出:  实测水深
**
** 全局变量: 无
** 调用模块: 无
**
** 作　者: 
** 日　期: 2017年12月06日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8_t WaterDeep_Pro(P_SYS_CB *p_P_SYS_CB)
{
	uint16_t i = 0,i_jump;
	uint16_t UtrBack_Index = 0;			// 记录有效回波在采样中的位置
	uint16_t Utr_Start_Index;			// 记录发射波起始点
	uint8_t Up_Value = 0,UpFor_Count = 0;
	double	tmp;
	uint8_t DatPro_Sta = 0;
	
	if(*p_P_SYS_CB->p_Sounder->p_forcedlockingecho)
	{	// 如果强制锁定回波,则直接滤除前面的波形，即定深测量
		tmp = p_P_SYS_CB->Water_Deep/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
		i_jump = (uint16_t)tmp;
	}
	else
	{
		// 根据抑制水深,需要跳过的查找点数
		tmp = *p_P_SYS_CB->p_Sounder->p_clearwidth/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
		// 计算出实际的查找起始点
		i_jump = (uint16_t)2*tmp;
	}
	i = 0;
	Utr_Start_Index = 0;
	for(;i<p_P_SYS_CB->UltraDat_Len;i++)
	{
		if(*(p_P_SYS_CB->p_UltraDat + i)<(*p_P_SYS_CB->p_Sounder->p_threshold*10))
		{
			*(p_P_SYS_CB->p_UltraDat + i) = 0;	// 回波阈值滤除
		}
		else
		{
			*(p_P_SYS_CB->p_UltraDat + i) -= (*p_P_SYS_CB->p_Sounder->p_threshold*10);	// 回波阈值滤除
			switch(DatPro_Sta)
			{
				case 0:									// 搜索向上趋势
					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
					{
						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
						if(Utr_Start_Index==0)
						{
							Utr_Start_Index = i;			// 记录待定的发射波起始点,左对齐方式
						}
						UpFor_Count++;
						if(UpFor_Count>=Num_Forward)		// 连续3个增长点即认为有向上的趋势
						{
							i = Utr_Start_Index + i_jump;	// 跳过无需搜索的索引
							Up_Value = 0;
							DatPro_Sta = 1;				// 搜索最大值
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
				case 1:									// 搜索回波向上趋势
					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
					{
						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
						if(UtrBack_Index == 0)
							UtrBack_Index = i;
						UpFor_Count++;
						if(UpFor_Count>=Num_Forward)	// 连续3个增长点即认为有向上的趋势
						{
							DatPro_Sta = 2;				// 确定回波起始点，进行处理
						}
					}
					else
					{
						UpFor_Count = 0;
						Up_Value = 0;
						UtrBack_Index = 0;
					}
					break;
				case 2:										// 计算差值
					tmp = UtrBack_Index - Utr_Start_Index;
					tmp = (p_P_SYS_CB->ADC_Samp_Time)*tmp;						// 
					tmp = Sys_CB.Parameters.velocity*tmp/2 + Sys_CB.Parameters.draft;	// 加上吃水深度
					p_P_SYS_CB->Water_Deep = tmp;
					if(tmp<Sys_CB.Parameters.minadepth)
					{// 浅水报警水深 
					}
					else if(tmp>Sys_CB.Parameters.maxadepth)
					{// 深水报警水深
					}
					return 1;
				default:break;
			}
		}
	}
	p_P_SYS_CB->MissReturn_Count++;
	if(p_P_SYS_CB->MissReturn_Count==Sys_CB.Parameters.missreturn)		// 丢波个数报警,蜂鸣器
	{
		p_P_SYS_CB->MissReturn_Count = 0;
	}
	if(Sys_CB.Parameters.alarmstatus)
	{// 无回报报警
	}
	return	0;			// 错误返回
}

uint8_t AutoWaterDeep_Pro(P_SYS_CB *p_P_SYS_CB)
{
//	uint16_t int_tmp = 0x00U;
//	uint8_t Grade_tmp;
//	uint32_t i = 0;
//	static uint32_t j_Index;
//	uint32_t UltraSon_Index = 0;		// 记录有效回波在采样中的位置
//	uint8_t Up_Value = 0,Down_Value = 0xFF,UpFor_Count = 0,DownFor_Count = 0;
//	double	tmp;
//	uint8_t DatPro_Sta = 0;
//	if(p_P_SYS_CB->p_Sounder->p_forcedlockingecho)
//	{	// 如果强制锁定回波,则直接滤除前面的波形，即定深测量
//		tmp = p_P_SYS_CB->Water_Deep/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
//		i_jump = (uint16_t)tmp;
//	}
//	else
//	{
//		// 根据抑制水深,需要跳过的查找点数
//		tmp = *p_P_SYS_CB->p_Sounder->p_clearwidth/Sys_CB.Parameters.velocity/p_P_SYS_CB->ADC_Samp_Time;
//		// 计算出实际的查找起始点
//		i_jump = (uint16_t)tmp;
//	}
//	while(i<p_P_SYS_CB->UltraDat_Len)
//	{
//		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
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
//				*(p_P_SYS_CB->p_UltraDat + i) = 0;	// 回波阈值滤除
//			}
//			switch(DatPro_Sta)
//			{
//				case 0:									// 搜索向上趋势
//					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
//					{
//						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						UpFor_Count++;
//						if(UpFor_Count>4)				// 连续4个增长点即认为有向上的趋势
//						{
//							DatPro_Sta = 1;				// 搜索最大值
//						}
//					}
//					else
//					{
//						UpFor_Count = 0;
//						Up_Value = 0;
//					}
//					break;
//				case 1:									// 搜索最大值
//					if(*(p_P_SYS_CB->p_UltraDat + i)>Up_Value)
//					{
//						Up_Value = *(p_P_SYS_CB->p_UltraDat + i);
//					}
//					else
//					{
//						UltraSon_Index = i - 1;			// 记录可能出现的最大值
//						Down_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						DownFor_Count++;
//						DatPro_Sta = 2;			// 可能出现下降趋势
//					}
//					break;
//				case 2:										// 确定递减趋势
//					if(*(p_P_SYS_CB->p_UltraDat + i)<Down_Value)
//					{
//						Down_Value = *(p_P_SYS_CB->p_UltraDat + i);
//						DownFor_Count++;
//						if(DownFor_Count>4)
//						{
//							tmp = UltraSon_Index - p_P_SYS_CB->Ref_Start_Index;
//							tmp = p_P_SYS_CB->ADC_Samp_Time*tmp;						// 
//							tmp = Sys_CB.Parameters.velocity*tmp/2 + Sys_CB.Parameters.draft;	// 加上吃水深度
//							p_P_SYS_CB->Water_Deep = tmp;
//							if(tmp<Sys_CB.Parameters.minadepth)
//							{// 浅水报警水深 
//							}
//							else if(tmp>Sys_CB.Parameters.maxadepth)
//							{// 深水报警水深
//							}
//							// 自动确定档位
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
//								getCMD_ACK(	"@SIC,,GET,GRADE,",				// 上报档位
//									&Sys_CB.Grade.auto_grade,
//									Byte_Type
//									);
//							}
//							if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
//							{	
//								while((p_P_SYS_CB->UltraDat_Len - hadc2.DMA_Handle->Instance->NDTR)<int_tmp);
//								p_P_SYS_CB->UltraDat_Len = int_tmp;
//								HAL_TIM_Base_Stop(&htim3); 					// 停止TIM3计数
//								HAL_ADC_Stop_DMA(&hadc1);
//							}
//							else
//							{
//								while((p_P_SYS_CB->UltraDat_Len - hadc1.DMA_Handle->Instance->NDTR)<int_tmp);
//								p_P_SYS_CB->UltraDat_Len = int_tmp;
//								HAL_TIM_Base_Stop(&htim2); 					// 停止TIM2计数
//								HAL_ADC_Stop_DMA(&hadc2);
//							}
//							return 1;
//						}
//					}
//					else
//					{
//						DatPro_Sta = 3;									// 排除是否为干扰
//					}
//					break;
//				case 3:						// ????
//					if(*(p_P_SYS_CB->p_UltraDat + i)<Down_Value)
//					{
//						DatPro_Sta = 2;
//					}
//					else							// 失败，从新判断
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
//		// 无回波报警
//		if(p_P_SYS_CB->MissReturn_Count==Sys_CB.Parameters.missreturn)		// 丢波个数报警
//		{
//			p_P_SYS_CB->MissReturn_Count = 0;
//		}
//	}
	return	0;			// 错误返回
}
/********************************************************************************************************
** 函数名称: UltraSonDatPro
** 功能描述: 超声波数据处理
**
** 输　入: 	p_P_SYS_CB，p_Grade
**
** 输　出:  无
**
** 全局变量: 无
** 调用模块: 无
**
** 作　者: 
** 日　期: 2017年12月06日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void UltraSonDatPro(P_SYS_CB *p_P_SYS_CB,GRADE *p_Grade)
{
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+HF;
		
	#ifdef Unmanned_ship //上电自动测量
		*p_P_SYS_CB->p_Survey_Model = 1;
	#endif
	
		if(*p_P_SYS_CB->p_Survey_Model==0)		// 手动测量
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
	
					*p_P_SYS_CB->p_Sounder->p_shallowsurveymode = *Sys_CB_H.p_Sounder->p_shallowsurveymode;//潜水模式
					FiltWave_Excute(&Sys_CB_L, Channel_One);

			}
			else
			{
				rt_sem_take(&ADC1_CMP_sem, RT_WAITING_FOREVER);			 
				TVG_Channel_Two_Stop();

					FiltWave_Excute(&Sys_CB_H, Channel_Two);
	
			}
		}
		else if(*p_P_SYS_CB->p_Survey_Model==1)						// 自动测量
		{
			if(p_Grade->grade)
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[p_Grade->grade];
			else
				p_P_SYS_CB->UltraDat_Len = p_P_SYS_CB->UltraDat_Len_Buf[Sys_CB.Grade.auto_grade];			
			
			// 自动换挡，全域搜索
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
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

