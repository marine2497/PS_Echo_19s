/****************************************Copyright (c)**************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Ultrason_Cal.C
**创   建   人: lzb
**最后修改日期: 2018年9月20日
**描        述: 数据处理
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
********************************************************************************************************/
#include "board.h"
#include "Ultrason_Cal.h"
#include "Ultrason_Airth.h"
#include "Ultrason_rcv.h"
#include "string.h"
#include <math.h>	//fabs()
#include <stdlib.h> //abs()


/*注意*/
/*
LF= 0， Channel_One = 1
HF= 1， Channel_Two = 2
函数参数如果是Channel ，则需要用Channel_One/Channel_Two判断高低频通道
*/

FiltWave_Typedef   FiltWave[2] ={
									{
										 0,											
										 0,
										{0},
										 0
									},
									{	 
										 0,										
										 0,
										{0},
										 0
									}
								};
Cal_deep_Typedef   Caldeep[2] = {
									{
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0
									},
									{
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0,
										0
									}
								};


DepthInfo_Typedef  DepthInfo[2] = {	
									{
										0,
										0,
										{0},
										{0},
										0
//										0
//										1
									},
									{
										0,
										0,
										{0},
										{0},
										0
//										0
//										1
									}
								  }	;


Auto_Adjust_Typedef 	Autoadjust[2] ={
										{
										0,
										0,
										0,
										0,
										SEARCH_NONE,
										0,
										0
										},
										{
										0,
										0,
										0,
										0,
										SEARCH_NONE,
										0,
										0
										}
									};


judge_Parater_Typedef  JudgePara[2] ={
										{
										0,
										0,
										0,
										0,
										0,
										0},
										
										{
										0,
										0,
										0,
										0,
										0,
										0
										}
										
								    }	;

Echo_ProcCal_Typedef    Echo_ProcCal[2] = {
											{	0,
												0,
												0,
												0,
												0,
												0,
												0,
												{0},
												{0},
												{0}											
											},
											{
												0,
												0,
												0,
												0,
												0,
												0,
												0,
												{0},
												{0},
												{0},								
											}									
									  };
								
DeepTrend_Typedef   DeepTrend[2] ={
									{
										LONGSTEADYCNT,
										SHORTSTEADYCUN,
										0,
										0
									},
									{
										LONGSTEADYCNT,
										SHORTSTEADYCUN,
										0,
										0
									}
								};						
						
/*************************************************************************************************/
//	数组初始化-32位
/*************************************************************************************************/
void Intit_Arry(uint32_t *Arry,uint32_t num,uint32_t count)
{
	uint32_t iLoop;
	for(iLoop=0;iLoop<count;iLoop++)
	{
		Arry[iLoop] = num;
	}

}
/*************************************************************************************************/
//	数组初始化-8位
/*************************************************************************************************/
void Intit_Arry_u8(uint8_t *Arry,uint8_t num,uint8_t count)
{
	uint8_t iLoop;
	for(iLoop=0;iLoop<count;iLoop++)
	{
		Arry[iLoop] = num;
	}

}

/*************************************************************************************************/
//	模拟水深：正弦波
/*************************************************************************************************/
static int32_t deep_tick = 4.0;
static int16_t x ,y= 0;
static float deep_test;
void Simulation_Ware(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	int iLoop=0;
	float T = 50;
		char buff[30];

	if(Channel == Channel_Two)
	{
		
		x = 540;
		for(iLoop=0 ;iLoop<p_P_SYS_CB->UltraDat_Len; iLoop++ )
		{		
			//deep_test = 25+15*sin(x*3.1415/180)+2+0.1*sin(x*3.1415/180*50)+10*cos(x*3.1415/180*0.2);
			deep_test = 140+140*sin(x*3.1415/360)+1.5;
			if(deep_test<0)
				deep_test = 0.5;
			if(x<360*T)
				x += 2;
			else
				x = 0;
			deep_tick = deep_test*2*100000/1480/2;	
		
			Float_to_Str(buff,deep_test);
			
			Str_to_float(buff,&deep_test);
			
//			getCMD_ACK(	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
//						&deep_test,
//						Float_Type
//						);	
				*(p_P_SYS_CB->p_UltraDat + iLoop) = deep_test;
			
		}		
		
		
		
//		deep_test = 3+3*sin(x*3.1415/180)+2+0.1*sin(x*3.1415/180*50)+10*cos(x*3.1415/180*0.2);
//		deep_test = 4;
//		//deep_test = 140+140*sin(x*3.1415/360)+1.5;
//		if(deep_test<0)
//			deep_test = 0.5;
//		if(x<360*T)
//			x += 1;
//		else
//			x = 0;
//		deep_tick = deep_test*2*100000/1480/2;	
//		
////		for(iLoop=0 ;iLoop<200; iLoop++ )
////		{
////			if(iLoop<100)
////				*(p_P_SYS_CB->p_UltraDat + iLoop) = iLoop*2;
////			else
////				*(p_P_SYS_CB->p_UltraDat + iLoop) = 400-iLoop*2;	
////		}
	}
	else if(Channel == Channel_One)
	{
		deep_test = 3+3*sin(y*3.1415/180)+2+0.1*sin(y*3.1415/180*50)+10*cos(y*3.1415/180*0.2);
		deep_test = 4;
		//deep_test = 140+140*sin(y*3.1415/360)+1.5;
		if(deep_test<0)
			deep_test = 0.5;
		if(y<360*T) 
			y += 1;
		else 
			y = 0;
		deep_tick = deep_test*2*10000/1480*2;
		
//		for(iLoop=0 ;iLoop<50; iLoop++ )
//		{
//			if(iLoop<25)
//				*(p_P_SYS_CB->p_UltraDat + iLoop) = iLoop*10;
//			else
//				*(p_P_SYS_CB->p_UltraDat + iLoop) = 500-iLoop*10;
//		}
	}

	//模拟回波
//	if(Sys_CB.Sounder.forcedlockingecho.channel_two ==1)//Sys_CB.Parameters.alarmstatus == 0)
//	{
		for(iLoop=0 ;iLoop<p_P_SYS_CB->UltraDat_Len; iLoop++ )
		{
			if(*(p_P_SYS_CB->p_UltraDat + iLoop) >0) 
			{
				*(p_P_SYS_CB->p_UltraDat + deep_tick + iLoop) = *(p_P_SYS_CB->p_UltraDat + iLoop);
				//*(p_P_SYS_CB->p_UltraDat + 2*deep_tick + iLoop) = *(p_P_SYS_CB->p_UltraDat + iLoop);			
				if(iLoop>10 && *(p_P_SYS_CB->p_UltraDat + iLoop) >0 && *(p_P_SYS_CB->p_UltraDat + iLoop + 1) ==0)
					break;
			}
		}
//	}


//	for(iLoop=0 ;iLoop<p_P_SYS_CB->UltraDat_Len; iLoop++ )
//	{
//				deep_tick = 1*2*100000/1480/2;	
//			*(p_P_SYS_CB->p_UltraDat+deep_tick+iLoop) = 180;
//	}

}
/*************************************************************************************************/
//	1，分离数据
/*************************************************************************************************/
char Seperate_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	FiltWave_Typedef *pFilstWave = FiltWave + Channel -1;//包络特征结构体
	Cal_deep_Typedef *pCal_Deep  = Caldeep  + Channel -1;//待处理的水深值

	uint32_t begintime = 0;											    //起始时间;
	uint32_t endtime = 1;                                               //包络的结束点时间

	uint32_t totalstrenth = 0;                                          //总能量
	uint32_t totalwdth = 0;                                             //总脉宽
	uint32_t avestrenth = 0;                                            //平均能量

	uint32_t iLoop,jLoop=0;						                	
	uint8_t reonecnt =0;
	uint8_t threshold = 0;
	
//	uint8_t max_value = 0;
	
	if(Channel == Channel_Two)
		threshold = (*p_P_SYS_CB->p_Sounder->p_threshold)*15;
	else
		threshold = (*p_P_SYS_CB->p_Sounder->p_threshold)*(*p_P_SYS_CB->p_Sounder->p_threshold)*(*p_P_SYS_CB->p_Sounder->p_threshold)+5;
	
	pFilstWave->chrctnum = 0;
	
	Intit_Arry(&pFilstWave->chrct[0],0,25*5);  //初始化临存变量数组
	
	if(*p_P_SYS_CB->p_Sounder->p_clearwidth >= Sys_CB.Parameters.draft) //抑制水深计算
		pCal_Deep->cltdeeptcik = (uint32_t)((*p_P_SYS_CB->p_Sounder->p_clearwidth - Sys_CB.Parameters.draft)*2/(p_P_SYS_CB->ADC_Samp_Time*1480));	
	else
		pCal_Deep->cltdeeptcik = 0;	//(uint32_t)((*p_P_SYS_CB->p_Sounder->p_clearwidth)*2/(p_P_SYS_CB->ADC_Samp_Time*1480));	
		
	*(p_P_SYS_CB->p_UltraDat + p_P_SYS_CB->UltraDat_Len-1) = 0;
	*(p_P_SYS_CB->p_UltraDat + p_P_SYS_CB->UltraDat_Len) = 0; //最后两点强制为零	
	
	for(iLoop=0 ;iLoop<p_P_SYS_CB->UltraDat_Len; iLoop++ )
	{

		
		/*阈值过滤*/
		if(*(p_P_SYS_CB->p_UltraDat + iLoop)>=threshold && iLoop!=p_P_SYS_CB->UltraDat_Len)
		{			
			//*(p_P_SYS_CB->p_UltraDat + iLoop) -= (*p_P_SYS_CB->p_Sounder->p_threshold*10);	// 回波阈值滤除(消除跳动)
			if(endtime != 0)
			{
				endtime = 0;
				begintime = iLoop;		
			}
	
			totalwdth    += 1;                                     	//脉宽累计宽度			
			totalstrenth += *(p_P_SYS_CB->p_UltraDat + iLoop);	  	//总能量
			avestrenth    = totalstrenth/totalwdth;                 //平均能量	
			
//			if(Channel == Channel_One) //低频波形加深灰度
//			{
//				*(p_P_SYS_CB->p_UltraDat + iLoop) *= 1.5;	
//				if(*(p_P_SYS_CB->p_UltraDat + iLoop)>255)
//					*(p_P_SYS_CB->p_UltraDat + iLoop) = 255;				
//			}			
//			
//			if(Channel == Channel_Two && totalwdth/10>120*3) 		//3倍最大脉宽 3*1.2ms
//				return 0;
//			else if(Channel == Channel_One && totalwdth/10>12*6)	//6倍最大脉宽-淤泥层 6*1.2ms
//				return 0;
			
			
		}
		else
		{
			if(*(p_P_SYS_CB->p_UltraDat + iLoop) != 0)
				*(p_P_SYS_CB->p_UltraDat + iLoop) = 0;			//回波阈值滤除
			if(totalstrenth!=0)	  								//找到幅值为0的点
			{
				endtime = iLoop;  								//保存结束点的时间值
				if(reonecnt == 0)                      			//找到第一个包络的末尾时间点，判断第一个包络的宽度。设想使用，待验证可行性。
				{
					if(totalwdth>4)							    //第一个包络未必是发射波，可能是小杂波
					{
						reonecnt = 1;
						pFilstWave->starttime = begintime;	         		//第一个发射波的起始时间点
						pCal_Deep->cltdeeptcik += pFilstWave->starttime; 	//保持一致
						pCal_Deep->basestrenth     = totalstrenth ;  //pFilstWave->chrct[1];	//发射波的总能量
						pCal_Deep->basepulse       = totalwdth ;     //pFilstWave->chrct[2];	//发射波的脉宽宽度
						pCal_Deep->baselavestrenth = avestrenth ;    //pFilstWave->chrct[3];	//发射波的平均能量						
					}
				}
				if(jLoop < 25 && reonecnt == 1 && totalwdth > pFilstWave->chrct[2+0*5]/10)//&& avestrenth > 30 )//保存回波
				{
					pFilstWave->chrct[0+jLoop*5] =  begintime;							//起始时间
					pFilstWave->chrct[1+jLoop*5] = 	totalstrenth;   					//总能量
					pFilstWave->chrct[2+jLoop*5] = 	totalwdth;						    //总脉宽
					pFilstWave->chrct[3+jLoop*5] = 	avestrenth;							//平均能量
					pFilstWave->chrct[4+jLoop*5] = 	endtime+2;							//包络结束时间值
					pFilstWave->chrctnum = jLoop ;    									//统计包络的个数			
		
					if((begintime >= pCal_Deep->cltdeeptcik))        					//时间点大于抑制水深点
					{   //在这里进行抑制水深起作用，但在包络筛选的进行水深抑制无效
						if(jLoop < 24)               //包络连续统计				
							jLoop++;
						else
							break;
					}
					else
						jLoop = 1; 	//去掉抑制水深之前的包络，保留发射波			
				}
				totalstrenth = 0;
				totalwdth 	 = 0;
				avestrenth 	 = 0;			
			}
		}
	}
	
	
	//printf("\r\n");
#ifdef DEBUG_UART2
		printf("chrctnum=%d ", pFilstWave->chrctnum+1);
		printf("base_s=%d base_w=%d base_as=%d\r\n\r\n", pCal_Deep->basestrenth, pCal_Deep->basepulse, pCal_Deep->baselavestrenth);
#endif
	return pFilstWave->chrctnum;
}	

/*************************************************************************************************/
//	2，比较数据
/*************************************************************************************************/	
void Compare_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)	 //50  37   135 1M
{
	FiltWave_Typedef	*pFilstWave = FiltWave+ Channel -1;	           //待处理的包络特征值
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;			   //待处理的水深值
	Echo_ProcCal_Typedef *pEcho_ProcCal =  Echo_ProcCal+ Channel -1;     //待处理的能量特征值

	static char hfrecut = 0;
	static char lfrecut = 0;
	uint32_t Deepwspoint;	//水深包络起始点
	
	uint32_t iLoop;

	uint32_t echostrenindex;
	uint32_t echoaveindex;
	uint32_t echowdthindex;
	
	
	if(pEcho_ProcCal->strenthindex>19)//防止越界
	{
		pEcho_ProcCal->strenthindex = 19;
		pEcho_ProcCal->avestreindex = 19;
		pEcho_ProcCal->wdthindex    = 19; 
	}
	 echostrenindex = pEcho_ProcCal->strenthindex;          //当前统计的总能量的点数
	 echoaveindex   = pEcho_ProcCal->avestreindex;          //当期统计的平均能量的点数
	 echowdthindex  = pEcho_ProcCal->wdthindex;              //当前统计的脉宽的个数

	if(pFilstWave->chrctnum == 0)		//当前无水深包络
	{
		pCal_Deep->tick = pCal_Deep->muxminpirtick;
		pEcho_ProcCal->echonum = 0;
		if(Channel == Channel_Two)
		{
			hfrecut ++;
			if(hfrecut>=3)
			{			
				hfrecut =3;
				pCal_Deep->tick = 0;
			}
		}
		else
		{
			lfrecut ++;
			if(lfrecut>=3)
			{			
				lfrecut =3;
				pCal_Deep->tick = 0;
			}
		}		
	}	
	else if(pFilstWave->chrctnum > 0)  //至少获得一个包络
	{
		char lstrenth =0,lwdth=0,lavestrenth=0;
		uint32_t chrctstrenth 	 = 0;   //总能量
		uint32_t chrctwdth    	 = 0;   //脉宽
		uint32_t chrctavestrenth = 0;   //平均能量
		
		pCal_Deep->tick = 0;
		pEcho_ProcCal->echonum = pFilstWave->chrctnum+1;	
		
		if(Channel == Channel_Two)
			hfrecut = 0;
		else
			lfrecut = 0;
		
		for(iLoop = 1; iLoop <= pFilstWave->chrctnum; iLoop++)
		{
			if(pFilstWave->chrct[0+iLoop*5] >= pCal_Deep->filttick_begin && pFilstWave->chrct[0+iLoop*5] <= pCal_Deep->filttick_end)	//过滤小于抑制水深的包络于时间门外的包络
			{
				if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic ==1)
				{
					if((pFilstWave->chrct[1+iLoop*5] > chrctstrenth) )
					{
						chrctstrenth 	= pFilstWave->chrct[1+iLoop*5];
						lstrenth 		= iLoop;			
					}
					if((pFilstWave->chrct[2+iLoop*5] > chrctwdth) )
					{
						chrctwdth		= pFilstWave->chrct[2+iLoop*5];
						lwdth    		= iLoop;				
					}
					if((pFilstWave->chrct[3+iLoop*5] > chrctavestrenth) && pFilstWave->chrct[1+iLoop*5] > pCal_Deep->basepulse/2)
					{
						chrctavestrenth = pFilstWave->chrct[3+iLoop*5];
						lavestrenth     = iLoop;				
					}
					//浅水模式
					if(*p_P_SYS_CB->p_Sounder->p_shallowsurveymode && pFilstWave->chrct[2+iLoop*5]>pCal_Deep->basepulse/2)
					{
						chrctstrenth 	= pFilstWave->chrct[1+iLoop*5];
						lstrenth 		= iLoop;			
						chrctwdth		= pFilstWave->chrct[2+iLoop*5];
						lwdth    		= iLoop;				
						chrctavestrenth = pFilstWave->chrct[3+iLoop*5];
						lavestrenth     = iLoop;	
						break;
					}
				}
				else if((*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic == 2 || *p_P_SYS_CB->p_Sounder->p_forcedlockingecho) && Channel == Channel_One)	//低频参考高频回波
				{
					if(((pFilstWave->chrct[0+iLoop*5]-pFilstWave->starttime)*LF_Freq/HF_Freq) >= ((Caldeep[1].tick-FiltWave[1].starttime)*0.8) \
						&& ((pFilstWave->chrct[0+iLoop*5]-pFilstWave->starttime)*LF_Freq/HF_Freq) <= ((Caldeep[1].tick-FiltWave[1].starttime)*1.2) ) //30 tick = 1m
					{
						chrctstrenth 	= pFilstWave->chrct[1+iLoop*5];
						lstrenth 		= iLoop;			
						chrctwdth		= pFilstWave->chrct[2+iLoop*5];
						lwdth    		= iLoop;				
						chrctavestrenth = pFilstWave->chrct[3+iLoop*5];
						lavestrenth     = iLoop;	
						break;
					}
					else if(chrctstrenth == 0)	//没搜索成功
					{
						chrctstrenth 	= pFilstWave->chrct[1+0*5];
						lstrenth 		= 1;			
						chrctwdth		= pFilstWave->chrct[2+0*5];
						lwdth    		= 1;				
						chrctavestrenth = pFilstWave->chrct[3+0*5];
						lavestrenth     = 1;
						pFilstWave->chrct[0+1*5]= (Caldeep[1].tick-FiltWave[1].starttime+pFilstWave->starttime)*HF_Freq/LF_Freq;
						pFilstWave->chrct[4+1*5]= (Caldeep[1].endtick-FiltWave[1].starttime+pFilstWave->starttime)*HF_Freq/LF_Freq;
					}
				}
			}
		}
	
		if(Channel == Channel_Two)	//HF
		{
			if(lwdth == lavestrenth) 	//lwdth == lavestrenth 
			{
				pCal_Deep->tick    = pFilstWave->chrct[0+lwdth*5];
				pCal_Deep->endtick = pFilstWave->chrct[4+lwdth*5];				
				Deepwspoint 	   = lavestrenth;
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lwdth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lwdth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lwdth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
			}			
			else if(lstrenth == lavestrenth) 	//lstrenth == lavestrenth 
			{
				pCal_Deep->tick    = pFilstWave->chrct[0+lstrenth*5];
				pCal_Deep->endtick = pFilstWave->chrct[4+lstrenth*5];				
				Deepwspoint = lavestrenth;
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lstrenth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lstrenth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lstrenth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
			}
			else 
			{
				pCal_Deep->tick 	= pFilstWave->chrct[0+lavestrenth*5];
				pCal_Deep->endtick 	= pFilstWave->chrct[4+lavestrenth*5];
				Deepwspoint = lavestrenth;
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lavestrenth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lavestrenth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lavestrenth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
			}			
		}
		else
		{
			if(lstrenth==lwdth && lstrenth==lavestrenth) //三者都最大 
			{
				pCal_Deep->tick    = pFilstWave->chrct[0+lstrenth*5];
				pCal_Deep->endtick = pFilstWave->chrct[4+lstrenth*5];					
				Deepwspoint = lstrenth; 

				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lstrenth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lstrenth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lstrenth*5] ;	
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;
			}
			else if(lstrenth==lavestrenth) //能量与平均能量最大
			{
				pCal_Deep->tick    = pFilstWave->chrct[0+lstrenth*5];
				pCal_Deep->endtick = pFilstWave->chrct[4+lstrenth*5];					
				Deepwspoint = lstrenth; 
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lstrenth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lstrenth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lstrenth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
							
			}		
			else if(lstrenth==lwdth)  //能量与脉宽最大
			{
				pCal_Deep->tick    = pFilstWave->chrct[0+lstrenth*5];
				pCal_Deep->endtick = pFilstWave->chrct[4+lstrenth*5];					
				Deepwspoint = lstrenth; 
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lstrenth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lstrenth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lstrenth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
							
			}			
			else 
			{
				pCal_Deep->tick 	= pFilstWave->chrct[0+lwdth*5];	
				pCal_Deep->endtick 	= pFilstWave->chrct[4+lwdth*5]; 					
				Deepwspoint = lwdth;
				
				pEcho_ProcCal->strentharry[echostrenindex]  = pFilstWave->chrct[1+lwdth*5] ;
				pEcho_ProcCal->wdtharry[echowdthindex]      = pFilstWave->chrct[2+lwdth*5] ;
				pEcho_ProcCal->avestrearry[echoaveindex]    = pFilstWave->chrct[3+lwdth*5] ;
				echostrenindex++;
				echoaveindex++;
				echowdthindex++;				
			}
			
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB.Survey_Status.channel_two && Sys_CB.Survey_Status.channel_one && Channel == Channel_One) //如果是双频模式, 低频水深与高频水深进行对比
			{	
				//高频采样周期20us，低频50us
				if(((((pCal_Deep->tick)*LF_Freq/HF_Freq - pFilstWave->starttime) > ((Caldeep[1].tick-FiltWave[1].starttime)*1.5))  || \
				    (((pCal_Deep->tick)*LF_Freq/HF_Freq - pFilstWave->starttime) < ((Caldeep[1].tick-FiltWave[1].starttime)*0.8))) && \
					   Caldeep[1].tick>30) //如果大于高频水深点的1.8倍，则重新计算
				{
					for(iLoop = 1; iLoop <= pFilstWave->chrctnum; iLoop++)
					{
						if((pFilstWave->chrct[0+iLoop*5]-pFilstWave->starttime)*LF_Freq/HF_Freq >= Caldeep[1].tick-FiltWave[1].starttime-1) //25 tick = 0.74m
						{
							pCal_Deep->tick 	= pFilstWave->chrct[0+iLoop*5];	
							pCal_Deep->endtick 	= pFilstWave->chrct[4+iLoop*5]; 					
							
							pEcho_ProcCal->strentharry[echostrenindex-1]  = pFilstWave->chrct[1+iLoop*5] ;
							pEcho_ProcCal->wdtharry[echowdthindex-1]      = pFilstWave->chrct[2+iLoop*5] ;
							pEcho_ProcCal->avestrearry[echoaveindex-1]    = pFilstWave->chrct[3+iLoop*5] ; 	
							break;
						}
						else if((pFilstWave->chrct[0+iLoop*5]-pFilstWave->starttime)*LF_Freq/HF_Freq >= ((Caldeep[1].tick-FiltWave[1].starttime)*1.5)) 
						{
							pCal_Deep->tick 	= (Caldeep[1].tick- FiltWave[1].starttime+ pFilstWave->starttime)* HF_Freq/LF_Freq;	
							pCal_Deep->endtick 	= (Caldeep[1].endtick- FiltWave[1].starttime+ pFilstWave->starttime)* HF_Freq/LF_Freq;	
							break;
						}
					}				
				}		
			}		
		}
		
		if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Channel == Channel_Two)//寻找对比最终包络是否为2次回波，在此仅判断二次回波，三次机四次回波暂时不比较			
		{
			for(iLoop=1 ; iLoop<pFilstWave->chrctnum; iLoop++)	         //二次回波的 特征值为  总脉宽大于 40  平均能量大于8   (具体数据可以继续比较设置)		
			{
				if((pFilstWave->chrct[iLoop*5] > (pCal_Deep->tick /2-pCal_Deep->tick/10)) && (pFilstWave->chrct[iLoop*5] < (pCal_Deep->tick /2+pCal_Deep->tick/10))\
					&& (pFilstWave->chrct[3+iLoop*5] > (pFilstWave->chrct[3+Deepwspoint*5]-pFilstWave->chrct[3+Deepwspoint*5]/10))) 
				{
					pCal_Deep->tick = 	pFilstWave->chrct[iLoop*5];
					pCal_Deep->endtick = 	pFilstWave->chrct[4+iLoop*5];  //计算最终包络的  结束时间点	
					
					pEcho_ProcCal->strentharry[echostrenindex-1]  = pFilstWave->chrct[1+iLoop*5] ;
					pEcho_ProcCal->wdtharry[echowdthindex-1]      = pFilstWave->chrct[2+iLoop*5] ;
					pEcho_ProcCal->avestrearry[echoaveindex-1]    = pFilstWave->chrct[3+iLoop*5] ;					
					break;
				}			
			}
		}

		
		pEcho_ProcCal->strenthindex = echostrenindex;
		pEcho_ProcCal->avestreindex = echoaveindex;
		pEcho_ProcCal->wdthindex    = echowdthindex;
		
	}					
}

/*************************************************************************************************/
//	3，波形能量统计分析，用于自动测量
/*************************************************************************************************/
void PowerInfo_Compare(Echo_ProcCal_Typedef  *pechoprocal, uint8_t Channel)
{	
	uint32_t strindex;
	uint32_t aveindex ;
	uint32_t wdthindex;
	uint32_t iLoop;

	uint32_t value1 = 0;
	uint32_t value2 = 0; 
	uint32_t value3 = 0;

	
	strindex = pechoprocal->strenthindex;    //当前统计的总能量个数
	aveindex = pechoprocal->avestreindex;    //当前统计的平均能量个数
	wdthindex = pechoprocal->wdthindex;      //当前统计的脉宽的个数   

	for(iLoop = 0; iLoop < strindex; iLoop++)
		value1 +=  pechoprocal->strentharry[iLoop];
	value1 = value1/(strindex);

	for(iLoop = 0; iLoop < aveindex; iLoop++)
		value2 +=  pechoprocal->avestrearry[iLoop];
	value2 = value2/(aveindex);

	for(iLoop = 0; iLoop < wdthindex; iLoop++)
		value3 +=  pechoprocal->wdtharry[iLoop];
	value3 = value3/(wdthindex);

	pechoprocal->strenth 	= value1;
	pechoprocal->avestrenth = value2;
	pechoprocal->wdth 		= value3;

	
//#ifdef DEBUG_UART2
//		printf("index=%d ", wdthindex);
//		printf("ave-s=%d ", pechoprocal->strenth);
//		printf("ave-w=%d ", pechoprocal->wdth);	
//		printf("ave-as=%d ", pechoprocal->avestrenth);
//		printf("echonum=%d\r\n\r\n", pechoprocal->echonum);		
//#endif
	
	Intit_Arry(&pechoprocal->strentharry[0],0,20);
	Intit_Arry(&pechoprocal->wdtharry[0],0,20);
	Intit_Arry(&pechoprocal->avestrearry[0],0,20);

	pechoprocal->strenthindex = 0;
	pechoprocal->avestreindex = 0;
	pechoprocal->wdthindex = 0;		

}

static volatile uint32_t  HFIndex=0;
static volatile uint32_t  LFIndex=0;
#ifdef	MINMUXPIR
static double MinMuxPirtick[2][10] ={{0},{0}};
#else
static uint32_t MinMuxPirtick[2][10] ={{0},{0}};
#endif
/*************************************************************************************************/
//   4，计算数据
/*************************************************************************************************/
void  Calculate_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	float value1 = 0.0;
	char  i = 0;
    int   Tmp = 0;
    
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;
	FiltWave_Typedef	*pFilstWave = FiltWave+ Channel -1;	
	
	if(Channel == Channel_One)
	{  //利用最小二成法  确定大概水深范围值
		MinMuxPirtick[LF][LFIndex++] = pCal_Deep->tick; 		
		if(LFIndex == 10)
		{			
			#ifdef MINMUXPIR
			   pCal_Deep->muxminpirtick=MinMuxPir(MinMuxPirtick[LF], LFIndex);
			#else
				pCal_Deep->muxminpirtick=AveCalOut(MinMuxPirtick[LF], LFIndex);
			#endif		

			for(i=0; i<LFIndex-1; i++)
			{
				if(MinMuxPirtick[LF][i] < MinMuxPirtick[LF][i+1]) //判断是否为水深变浅
					break; 
				if(i==LFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[LF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[LF][i+1];					
			}
			for(i=0; i<LFIndex-1; i++)
			{
				if(MinMuxPirtick[LF][i] > MinMuxPirtick[LF][i+1]) //判断是否为水深变深
					break; 
				if(i==LFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[LF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[LF][i+1];					
			}			
			LFIndex = 0;
			if(Sys_CB.Grade.grade == 0)
			{
				 if(((Sys_CB.Survey_Status.channel_one)&&(Sys_CB.Survey_Status.channel_two==0)))
					Auto_change_grade((uint32_t)Sys_CB.Parameters.velocity * (pCal_Deep->muxminpirtick) * p_P_SYS_CB->ADC_Samp_Time*50);		
			}			
		
		}
	}
	else 
	{
		MinMuxPirtick[HF][HFIndex++] = pCal_Deep->tick;
		if(HFIndex ==10)
		{		
			#ifdef MINMUXPIR
			   pCal_Deep->muxminpirtick = MinMuxPir(MinMuxPirtick[HF], HFIndex);
			#else
				pCal_Deep->muxminpirtick= AveCalOut(MinMuxPirtick[HF], HFIndex);
			#endif
			for(i=0; i<HFIndex-1; i++)
			{
				if(MinMuxPirtick[HF][i] < MinMuxPirtick[HF][i+1]) //判断是否为水深变浅
					break; 
				if(i==HFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[HF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[HF][i+1];					
			}
			for(i=0; i<HFIndex-1; i++)
			{
				if(MinMuxPirtick[HF][i] > MinMuxPirtick[HF][i+1]) //判断是否为水深变深
					break; 
				if(i==HFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[HF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[HF][i+1];					
			}			
			HFIndex = 0;
			if(Sys_CB.Grade.grade == 0 && Sys_CB.Survey_Status.channel_two)
				Auto_change_grade((uint32_t)Sys_CB.Parameters.velocity * (pCal_Deep->muxminpirtick) * p_P_SYS_CB->ADC_Samp_Time*50);	
		}
	}
     if(pCal_Deep->tick > pFilstWave->starttime)
     {
         Tmp = pCal_Deep->tick - pFilstWave->starttime;
     }
     else
     {
        Tmp = 0;
     }

//	value1 = Sys_CB.Parameters.velocity * (pCal_Deep->tick-pFilstWave->starttime) * p_P_SYS_CB->ADC_Samp_Time/2; // v(m/s)*t(us)
    value1 = Sys_CB.Parameters.velocity * Tmp * p_P_SYS_CB->ADC_Samp_Time/2; // v(m/s)*t(us)


	pCal_Deep->deep 	   = value1 + Sys_CB.Parameters.draft;
	p_P_SYS_CB->Water_Deep = value1 + Sys_CB.Parameters.draft;


	//长沙测试校准 20190925
//	if(Channel == Channel_One)
//	{
//		if(pCal_Deep->deep >13)
//		{
//			pCal_Deep->deep  = pCal_Deep->deep  - 0.15;
//			p_P_SYS_CB->Water_Deep = p_P_SYS_CB->Water_Deep  - 0.15;
//		}
//		if(pCal_Deep->deep <13)
//		{
//			pCal_Deep->deep  = pCal_Deep->deep  - 0.10;	
//			p_P_SYS_CB->Water_Deep = p_P_SYS_CB->Water_Deep  - 0.10;
//		}
//	}
	
		
	
	if(p_P_SYS_CB->Water_Deep > 300 || pCal_Deep->deep > 300 || p_P_SYS_CB->Water_Deep < Sys_CB.Parameters.draft || pCal_Deep->deep < Sys_CB.Parameters.draft)
		pCal_Deep->deep = p_P_SYS_CB->Water_Deep = 0;
/*	if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3)
	{
		if(Sys_CB.Survey_Status.channel_two && Sys_CB.Survey_Status.channel_one && Channel == Channel_One && pFilstWave->chrctnum)//双频模式保持低频水深连续性
		{
			if(Sys_CB_L.Water_Deep == Sys_CB.Parameters.draft && Sys_CB_H.Water_Deep != Sys_CB.Parameters.draft)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}
			//20190925 增加 
			//如果低频数据在高频附近跳动，小于高频-5cm，则保证低频的稳定性
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB_L.Water_Deep <  Sys_CB_H.Water_Deep-0.05)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}	
			//如果低频数据在高频附近跳动，大于高频*1.5，则保证低频的稳定性
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB_L.Water_Deep >  Sys_CB_H.Water_Deep*1.5)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}
			//如果低频数据在高频附近跳动，大于高频+10cm,小于高频+1m，则保证低频的稳定性
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB_L.Water_Deep >  Sys_CB_H.Water_Deep+0.1 && Sys_CB_L.Water_Deep <  Sys_CB_H.Water_Deep+2)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep+0.1;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep+0.1;
			}	
			
		}
	}
	*/
}

/*************************************************************************************************/
//	5，时间门限计算
/*************************************************************************************************/
void Filt_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{

	Cal_deep_Typedef *pCal_Deep = Caldeep + Channel -1;	
	
	uint32_t value1;	
	//uint8_t basetick =  7  ;        //140us = 0.104m
	//if(Channel == Channel_One)
	//	basetick = 3;				//150us = 0.111m
	
	if(pCal_Deep->muxminpirtick < 10  || ((uint32_t)(*p_P_SYS_CB->p_Sounder->p_filter*100))<10)	//时间门未开启
	{
		pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	 				//  抑制水深处
		pCal_Deep->filttick_end   =   p_P_SYS_CB->UltraDat_Len_Buf[7]-1;		//  计数周期末尾
	}
	else
	{
		//value1 = basetick * ((uint32_t)(*p_P_SYS_CB->p_Sounder->p_filter*100)); //HF 10%=15*10=1.04m// 20%=2.08m  
		value1 = pCal_Deep->tick * (*p_P_SYS_CB->p_Sounder->p_filter); //HF 10%=15*10=1.04m// 20%=2.08m  

		if(pCal_Deep->tick > pCal_Deep->muxminpirtick*0.8 && pCal_Deep->tick < pCal_Deep->muxminpirtick*1.2)
		{
			if(pCal_Deep->tick > value1+1)
				pCal_Deep->filttick_begin =   pCal_Deep->tick - value1;	//时间门前沿
			else 
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	
			
			if(pCal_Deep->filttick_begin  <   pCal_Deep->cltdeeptcik)  //起始时间门小于抑制水深
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik;

			pCal_Deep->filttick_end       =   pCal_Deep->tick + value1;  	//时间门后沿		

//			if(Channel == Channel_One && (value1*2) < (1400/LF_Freq))  //1400us ~= 1m   //时间门最小为1m 防止水浅的问题
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/LF_Freq);
//			
//			else if(Channel == Channel_Two && (value1*2) < (1400/HF_Freq))  //1400us ~= 1m
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/HF_Freq);
			if(Channel == Channel_One && pCal_Deep->filttick_begin > (200/LF_Freq))  //1400us ~= 1m   //增益电路有延迟
			{
				pCal_Deep->filttick_begin = pCal_Deep->filttick_begin - (200/LF_Freq);
				pCal_Deep->filttick_end = pCal_Deep->filttick_end - (200/LF_Freq);		
			}
			
			else if(Channel == Channel_Two && pCal_Deep->filttick_begin > (200/HF_Freq))  //1400us ~= 1m
			{
				pCal_Deep->filttick_begin = pCal_Deep->filttick_begin - (200/HF_Freq);
				pCal_Deep->filttick_end = pCal_Deep->filttick_end - (100/HF_Freq);				
			}			
		}
		else
		{			
			if(pCal_Deep->muxminpirtick > value1+1)
				pCal_Deep->filttick_begin =   pCal_Deep->muxminpirtick - value1;	//时间门前沿
			else 
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	
			
			if(pCal_Deep->filttick_begin  <   pCal_Deep->cltdeeptcik)
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik;

			pCal_Deep->filttick_end       =   pCal_Deep->muxminpirtick + value1;  	//时间门后沿	
			
//			if(Channel == Channel_One && (value1*2) < (1400/LF_Freq))  //1400us ~= 1m  
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/LF_Freq);
//			
//			else if(Channel == Channel_Two && (value1*2) < (1400/HF_Freq))  //1400us ~= 1m
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/HF_Freq);
			if(Channel == Channel_One && pCal_Deep->filttick_begin > (200/LF_Freq))  //1400us ~= 1m   //增益电路有延迟
			{
				pCal_Deep->filttick_begin = pCal_Deep->filttick_begin - (200/LF_Freq);
				pCal_Deep->filttick_end = pCal_Deep->filttick_end - (200/LF_Freq);		
			}
			
			else if(Channel == Channel_Two && pCal_Deep->filttick_begin > (200/HF_Freq))  //1400us ~= 1m
			{
				pCal_Deep->filttick_begin = pCal_Deep->filttick_begin - (200/HF_Freq);
				pCal_Deep->filttick_end = pCal_Deep->filttick_end - (100/HF_Freq);				
			}					
		}		
	}	

}

/*************************************************************************************************/
//	6，组合数据
/*************************************************************************************************/
unsigned char NMEACheckNum1(char *s)
{
	unsigned char sum = 0;

	if(*s == '$')
		s++;
	while(*s != '*' && *s)
	{
		sum ^= *s;
		s++;
	}
	return sum;
}


unsigned char NMEACheckNum2(char *s)
{
	unsigned char sum = 0;

	if(*s == '@')
		s++;
	while(*s != '*' && *s)
	{
		sum ^= *s;
		s++;
	}
	return sum;
}


void Unmanned_ship_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char len = 0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
// 1、动态数据
//$PSIC,ESI,低频水深数据，高频水深数据，注册时间，已用时间，当前电压，测量档位*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$PSIC,ESI,",sizeof("$PSIC,ESI,")-1);

	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//低频水深
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//高频水深
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.reg_time);//注册时间
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.used_time);	//使用时间
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB.Voltage.voltage);	//当前电压	
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Grade.grade);		//测量档位

	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
	
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';		
	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);


}


/*1.1.1 SIC 数据格式：*/
void SIC_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char len = 0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
//DA00,低频水深数据，高频水深数据 m*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"@SIC,,GET,DATA.DEEP",sizeof("@SIC,,GET,DATA.DEEP")-1);

//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//低频水深
	len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "O");	
    len += sprintf(&UART1_TxBuf[len], "K");	
    len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//高频水深
//    len += sprintf(&UART1_TxBuf[len], " ");	
//    len += sprintf(&UART1_TxBuf[len], "m");	
	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
//    UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*不参与校验
//	len ++;
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum2(UART1_TxBuf_tmep));
	
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 1)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 2)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//释放信号，防止程序阻塞
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // 低频
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//

}


/*1.1.2 DES25数据格式:DA00124.45 m*/
void DES25_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char len = 0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
// 1、动态数据
//DA00,低频水深数据，高频水深数据 m*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"DA00",sizeof("DA00")-1);

//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//低频水深
//	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//高频水深
    len += sprintf(&UART1_TxBuf[len], " ");	
    len += sprintf(&UART1_TxBuf[len], "m");	
	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
	
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 1)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 1)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//释放信号，防止程序阻塞
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // 低频
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//

}
/*1.1.3 DBT 数据格式$SSDTB,123.*/
void DBT_DatOut(P_SYS_CB *p_P_SYS_CB)
{ 
	char  len = 0;
    float Water_H_FDepth   = 0.0;
    float Water_H_FatDepth = 0.0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
// 1、动态数据
//DA00,低频水深数据，高频水深数据 m*CRC
	Water_H_FDepth = Sys_CB_H.Water_Deep*3.2808399f;
    Water_H_FatDepth = Water_H_FDepth / 6.0f;
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$SSDBT",sizeof("$SSDBT")-1);
//    len += sprintf(&UART1_TxBuf[len], ",");	
//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//低频水深
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Water_H_FDepth);//高频水深（英尺）
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "f");	
    len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//高频水深（米）
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "M");	
    len += sprintf(&UART1_TxBuf[len], ",");
    len += Float_to_Str(&UART1_TxBuf[len],  Water_H_FatDepth);//高频水深（6英尺）
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "F");	
	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
 
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
    
	UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*不参与校验
	len++;
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 4)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 3)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//释放信号，防止程序阻塞
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // 低频
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//

}

void GPSS_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char  len = 0;


	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$GPSSS",sizeof("$GPSSS")-1);
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//高频水深（米）
	
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  1480);//声速
	
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  0.5);//吃水
	
	len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  0.3);//测量周期

	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
 
	UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*不参与校验
	len++;
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 4)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 3)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//释放信号，防止程序阻塞
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // 低频
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//


}




void Combine_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	uint16_t tmp_int;
	uint16_t Pack_Len = 0,Envelope_Len = 0,Envelope_Index,EnvelopeStart_Index;
	uint16_t First_Valid_Index = 0;							// 记录发射波有效的第一个点坐标
	uint16_t i;
	uint16_t  validratio = 0;	
	float tmp_float;
	
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	*p_P_SYS_CB->p_UART_Dat       = '$';
	*(p_P_SYS_CB->p_UART_Dat + 1) = 'E';
	*(p_P_SYS_CB->p_UART_Dat + 2) = 'S';
	*(p_P_SYS_CB->p_UART_Dat + 3) = 'P';
	*(p_P_SYS_CB->p_UART_Dat + 4) = 'L';
	*(p_P_SYS_CB->p_UART_Dat + 5) = 'U';
	*(p_P_SYS_CB->p_UART_Dat + 6) = 'S';
	tmp_float = p_P_SYS_CB->Water_Deep;
	tmp_float = tmp_float*100;								// 扩大100倍,单位为cm	
	tmp_int = (uint16_t)tmp_float;
	
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)				// 低频
	{

		*(p_P_SYS_CB->p_UART_Dat + 7) = 'L';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
#ifdef LF_50us
		*(p_P_SYS_CB->p_UART_Dat + 9) = 50;					
#else
		*(p_P_SYS_CB->p_UART_Dat + 9) = 100;			// 100uS
#endif		
//		tmp_int = tmp_int-8; //8cm 
	}
	else													// 高频
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'H';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
#ifdef HF_20us		
		*(p_P_SYS_CB->p_UART_Dat + 9) = 20;					// 10uS
#else
		*(p_P_SYS_CB->p_UART_Dat + 9) = 10;
#endif
	}

	*(p_P_SYS_CB->p_UART_Dat + 10) = tmp_int>>8;			// 水深高字节
	*(p_P_SYS_CB->p_UART_Dat + 11) = tmp_int;				// 水深低字节
	
	Envelope_Index = 18;									// 让出当前包络采样点个数和当前包络距离原点间隔周期数共4个字节
	//for(i = p_P_SYS_CB->Ref_Start_Index;i<p_P_SYS_CB->UltraDat_Len;i++)
	for(i = 1;i<p_P_SYS_CB->UltraDat_Len;i++)
	{

		if(*(p_P_SYS_CB->p_UltraDat + i)&0xf0)
		{
			//if(*(p_P_SYS_CB->p_UltraDat + i)>= 0xb0)
			//	*(p_P_SYS_CB->p_UltraDat + i) = 0xe0;
	
			
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
					First_Valid_Index = i;
				

				if(Channel == Channel_Two)
					EnvelopeStart_Index = i - First_Valid_Index;
				else
				{
					if(i - First_Valid_Index>0)
						EnvelopeStart_Index = i - First_Valid_Index - 4; //减去误差，保证水深线与波形一致
					else
						EnvelopeStart_Index = i - First_Valid_Index;					
				}					
			}
			
			validratio++;
			
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
			{
				//限制上传回波数量
				if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))//30%			// 防止越界
				{					
					break;
				}
			}
			else
			{
				//限制上传回波数量
				if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))//30%			// 防止越界
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

	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
	{	
		rt_event_send(&HL_Pro_CMP_event, (1 << 0));
	}	
	else
	{
		rt_event_send(&HL_Pro_CMP_event, (1 << 1));
	}
}
int my_abs(int a, int b)
{
	return a>b?(a-b):(b-a);
}


/*************************************************************************************************/
//	7, 数据分析 
/*************************************************************************************************/

uint32_t  DepthInfo_Compare(uint16_t deep, uint16_t *inputdep, uint16_t *outputdep, uint16_t *pincount, uint16_t *poutcount, uint8_t Channel)
{
	uint32_t iLoop;
	unsigned long aveinput =0;
	unsigned long aveoutput =0;
	uint32_t index ;
	uint32_t outputdex;

	uint8_t beforeaveindex =0;
	uint8_t afteraveindex  =0;
	unsigned long  beforeavedeep =0;
	unsigned long  afteravedeep = 0;
	
	uint8_t i_index = 20;
	
	index = *pincount ;
	outputdex = *poutcount;
	
	if(Sys_CB.Grade.grade)
		i_index = 21/Sys_CB.Grade.grade;
	
	if(index <i_index) //20 //提高自动测量的快速性
	{
		index++;
		inputdep[index]  = deep;
		*pincount  = index;
	}	
	else 
	{
		
		for(iLoop =0; iLoop<index ;iLoop++)
		{
			aveinput += inputdep[iLoop];	
		}
		aveinput =aveinput/(index);	//一个统计周期的平均水深值
				
		for(iLoop = 0; iLoop<index ;iLoop++)
		{
			if(inputdep[iLoop] >= aveinput )
			{
				afteraveindex++;  //统计一个周期大于平均数据的个数
				afteravedeep +=inputdep[iLoop]; 					
			}
			else
			{
				beforeaveindex++;	//统计一个周期小于平均数据的个数
				beforeavedeep +=inputdep[iLoop]; 					
			}		
		}
		
		if(beforeaveindex >= afteraveindex)	 //高于平均数据的点数较多
		{
			aveoutput = beforeavedeep/beforeaveindex; 			
		}
		else
		{
			aveoutput = afteravedeep/afteraveindex;	
		}
		
		*pincount =0;	
/*水深稳定判断-S*/	
		if(my_abs((int)(deep),(int)aveoutput) <=150)
		{
			JudgePara[Channel-1].water_Steady_flag = 1 ;	  //水深稳定
		}
		else 
		{
			JudgePara[Channel-1].water_Steady_flag = 0 ; 	
		}
	
		if(JudgePara[Channel-1].water_Steady_flag == 1)
		{
			DeepTrend[Channel-1].longsteadycount++;	//累计水深稳定的统计周期数
			DeepTrend[Channel-1].shortsteadycount++;	
			if(DeepTrend[Channel-1].longsteadycount >= LONGSTEADYCNT)
			{
				DeepTrend[Channel-1].longsteadyflag = 1;		   //长期稳定的标志
				DeepTrend[Channel-1].longsteadycount = 0;				
			}
			if(DeepTrend[Channel-1].shortsteadycount >= SHORTSTEADYCUN)
			{
			 	DeepTrend[Channel-1].shortsteadyflag =1;		 //短期稳定的标志
				DeepTrend[Channel-1].shortsteadycount = 0;
			}			
		}
		else
		{
			DeepTrend[Channel].longsteadycount = 0;
			DeepTrend[Channel].shortsteadycount = 0;
			DeepTrend[Channel].longsteadyflag =  0;
			DeepTrend[Channel].shortsteadyflag = 0;				
		}
/*水深稳定判断-E*/		
		
		*outputdep = aveoutput;
		
		//8，  能量	包络分析
		PowerInfo_Compare(&Echo_ProcCal[Channel-1], Channel);


		Autoadjust[Channel-1].processflag  = 1;
//		Autoadjust[Channel-1].sendprocflag = 1;	

	}

	return outputdep[outputdex];	
}
/*************************************************************************************************/
//	采用数据处理+水深计算
/*************************************************************************************************/
//char hlf_i = 0;
void FiltWave_Excute(P_SYS_CB *p_P_SYS_CB,  uint8_t Channel)
{	
	char ware;
	
//	hlf_i++;
//	if(hlf_i>= 4 + 8/(Sys_CB.Grade.grade+1)) //减少发到A5的数据量
//	{
//		hlf_i = 0;
//		Unmanned_ship_DatOut(p_P_SYS_CB);
//		
//	}	
	
	Unmanned_ship_DatOut(p_P_SYS_CB);
	
	//0，模拟数据
//	Simulation_Ware(p_P_SYS_CB, Channel);
	
	//1，分离提取数据	
	ware = Seperate_Sampdata(p_P_SYS_CB, Channel);
	
	//DepthInfo[Channel-1].echocntnum = ware;
	
//	if(ware != 0)
//	{
		//2，比较数据
		Compare_Sampdata(p_P_SYS_CB, Channel);
	
		//3，计算数据
		Calculate_Sampdata(p_P_SYS_CB, Channel);

		//4，时间门限计算
		Filt_Sampdata(p_P_SYS_CB, Channel);	
//	}	
//	else
//	{
		Echo_ProcCal[Channel-1].echonum = ware;
//	}
	
	
	switch(Transform.Transform_Two)
    {
        case 1:
            DES25_DatOut(p_P_SYS_CB);
            break;
        
        case 2:
            SIC_DatOut(p_P_SYS_CB);
            break;
        
        case 3:
            DBT_DatOut(p_P_SYS_CB);
            break;
        
        case 4:
            GPSS_DatOut(p_P_SYS_CB);//发送数据到A5
            break;
        
        case 5:
            Combine_Sampdata(p_P_SYS_CB, Channel);
            break;
               
        default:
            break;
    }
	//5，组合、发送数据
//	Combine_Sampdata(p_P_SYS_CB, Channel);
	



	
	//6, 数据分析    水深判断，只反映当前的水深范围，不需要其他的比较
	DepthInfo_Compare((uint16_t)(p_P_SYS_CB->Water_Deep*100),\
					  DepthInfo[Channel-1].inputdep,\
					  &DepthInfo[Channel-1].outputavedep,\
	                  &DepthInfo[Channel-1].inputcount,\
	                  &DepthInfo[Channel-1].outputcount, Channel);
	
	
}
