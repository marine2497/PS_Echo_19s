/****************************************Copyright (c)**************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Auto_Adjust.C
**创   建   人: lzb
**最后修改日期: 2018年9月20日
**描        述: 自动测量
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
********************************************************************************************************/
#include "Auto_Adjust.h"
#include "UART_Parameters.h"
#include "UART_DEVICE.H"
#include "Ultrason_cal.h"
#include "Ultrason_rcv.h"
#include "board.h"
#include "dac.h"

/*************************************************************************************************/
//	高频自动测量
/*************************************************************************************************/
void Auto_HF_Adjust(P_SYS_CB *p_P_SYS_CB)
{
	DepthInfo_Typedef    *pdepthinfo    = DepthInfo+HF;
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+HF;
	Echo_ProcCal_Typedef *pEcho_ProcCal = Echo_ProcCal+HF;
	Cal_deep_Typedef     *pCal_Deep     = Caldeep+HF; 

	uint32_t avedepth = pdepthinfo->outputavedep;
	
//#ifdef DEBUG_UART2						
//		printf("Auto_HF_avedeep=%d\r\n", avedepth);
//#endif	
	if(pautoadjust->processflag == 1)
	{
		pautoadjust->processflag =0;	
		if(pautoadjust->searchstatus == SEARCH_NONE)	// 初始化搜索过程
		{
			pautoadjust->searchlevel = 1;
			pautoadjust->searchstatus = SEARCH_READY;		
			Search_HFParameters(pautoadjust->searchlevel, p_P_SYS_CB);	
			
		}
		else if(pautoadjust->searchstatus == SEARCH_READY)   //继续搜索过程
		{				
			if(avedepth > Sys_CB.Parameters.draft*100)	 //有水深值
			{					
				pautoadjust->searchstatus = SEARCH_OK;
				
				if(avedepth > 23000)
				{
					pautoadjust->searchOklevel = 7;	
				}
				else if(avedepth > 11000)
				{
					pautoadjust->searchOklevel = 6;
				}			
				else if(avedepth > 5500)
				{
					pautoadjust->searchOklevel = 5;
				}
				else if(avedepth > 2900)//29m
				{
					pautoadjust->searchOklevel = 4;
				}
				else if(avedepth > 1450)//14.5m
				{
					pautoadjust->searchOklevel = 3;
				}	
				else if(avedepth > 750)//7.5m
				{
					pautoadjust->searchOklevel = 2;
				}
				else //if(avedepth > 50)//0.5m
				{
					pautoadjust->searchOklevel = 1;
				}
				Search_HFOK_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB);	
			} 
			else
			{
				pautoadjust->searchlevel++;
									
				if(pautoadjust->searchlevel>7)
						pautoadjust->searchlevel =1;
				
				Search_HFParameters(pautoadjust->searchlevel, p_P_SYS_CB);	
				
				//Sys_CB_Copy.Grade.grade = pautoadjust->searchlevel;
				//setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB_Copy.Grade.grade, Byte_Type);					
			}							
		}
		else if(pautoadjust->searchstatus == SEARCH_OK)
		{
			HF_Auto_Scope(pautoadjust->searchOklevel);  //根据水深选择相应的配置参数

			if(avedepth  >  pautoadjust->maxdepth)
			{
			 	pautoadjust->searchOklevel++;
				if(pautoadjust->searchOklevel>=7)
					pautoadjust->searchOklevel =7;
				Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
			}	
			else if(avedepth <  pautoadjust->mindepth)	
			{
				pautoadjust->searchOklevel--;
				if(pautoadjust->searchOklevel<=1) 
					pautoadjust->searchOklevel = 1;
				if( Sys_CB_Copy.Sounder.filter.channel_two > 0 && Sys_CB_Copy.Sounder.filter.channel_two < 0.3f) //水深变浅时间门应该增大
					Sys_CB_Copy.Sounder.filter.channel_two = 0.3f;
				Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
			}
			
			if(Sys_CB_Copy.Sounder.startgain.channel_two<5)
				Sys_CB_Copy.Sounder.startgain.channel_two=30;			

			
//			if(avedepth  <  400) //四米固定增益
//			{
//				Sys_CB_Copy.Sounder.startgain.channel_two = 35;
//				Sys_CB_Copy.Sounder.filter.channel_two = 0;
//			}		
//			else if(pEcho_ProcCal->avestrenth < 166|| pEcho_ProcCal->strenth < 2000 || pCal_Deep->basepulse < 5)  //增益递增条件			
			if(pEcho_ProcCal->avestrenth < 166|| pEcho_ProcCal->strenth < 2000 || pCal_Deep->basepulse < 5)  //增益递增条件
			{	//pCal_Deep->basepulse = 68 -->1m
				Sys_CB_Copy.Sounder.startgain.channel_two++;					
				if(Sys_CB_Copy.Sounder.startgain.channel_two>60)
					Sys_CB_Copy.Sounder.startgain.channel_two=60;
			
				Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);

				if(DeepTrend[HF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.5;
				else if(DeepTrend[HF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}				
				
			}
			else if(pEcho_ProcCal->avestrenth >170 ||  pEcho_ProcCal->wdth > 68 || pCal_Deep->basepulse>68 || pEcho_ProcCal->echonum>4)//增益递减条件
			{
				Sys_CB_Copy.Sounder.startgain.channel_two--;	
				if(Sys_CB_Copy.Sounder.startgain.channel_two<25)
					Sys_CB_Copy.Sounder.startgain.channel_two=25;	

				//判断是否需要上传自动测量参数
				if(pEcho_ProcCal->avestrenth >182 ||  pEcho_ProcCal->wdth > 68*2 || pCal_Deep->basepulse>68*2 || pEcho_ProcCal->echonum>6)
					Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				else
					Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				
				//根据水深稳定度设置时间门
				if(DeepTrend[HF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.5;
				else if(DeepTrend[HF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}									
			}	
			else 
			{
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}
				else
				{
					//当增益稳定时，根据水深稳定度 再减小时间门
					if(DeepTrend[HF].longsteadyflag == 1)				
						Sys_CB_Copy.Sounder.filter.channel_two = 0.4;
					else if(DeepTrend[HF].shortsteadyflag == 1)				
						Sys_CB_Copy.Sounder.filter.channel_two = 0.5;
					else
						Sys_CB_Copy.Sounder.filter.channel_two = 0.7;	
				}					
				
						
			}
			
			if(p_P_SYS_CB->Water_Deep == Sys_CB.Parameters.draft)
			{
				Sys_CB_Copy.Sounder.filter.channel_two = 0;			
			}		
			*p_P_SYS_CB->p_Sounder->p_filter = Sys_CB_Copy.Sounder.filter.channel_two;		//同步时间门参数						
		}
	}
}

/*************************************************************************************************/
//	低频自动测量
/*************************************************************************************************/
void Auto_LF_Adjust(P_SYS_CB *p_P_SYS_CB)
{
	DepthInfo_Typedef    *pdepthinfo    = DepthInfo+LF;
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+LF;
	Echo_ProcCal_Typedef *pEcho_ProcCal =  Echo_ProcCal+LF;
	Cal_deep_Typedef     *pCal_Deep  = Caldeep+LF; 
	
	uint32_t avedepth = pdepthinfo->outputavedep;
	
//#ifdef DEBUG_UART2						
//		printf("Auto_LF_avedeep=%d\r\n", avedepth);
//#endif
	if(pautoadjust->processflag == 1)
	{
		pautoadjust->processflag =0;

		if(pautoadjust->searchstatus == SEARCH_NONE)	// 初始化搜索过程
		{
			pautoadjust->searchlevel = 1;
			Search_LFParameters(pautoadjust->searchlevel, p_P_SYS_CB);
			pautoadjust->searchstatus = SEARCH_READY;	
		}
		else if(pautoadjust->searchstatus == SEARCH_READY)   //继续搜索过程
		{
			if(avedepth > 50+Sys_CB.Parameters.draft*100)	 //有水深值
			{
				pautoadjust->searchstatus = SEARCH_OK;
				if(avedepth > 23000)
				{
					pautoadjust->searchOklevel = 7;	
				}
				else if(avedepth > 11000)
				{
					pautoadjust->searchOklevel = 6;
				}			
				else if(avedepth > 5500)
				{
					pautoadjust->searchOklevel = 5;
				}
				else if(avedepth > 2900)//29m
				{
					pautoadjust->searchOklevel = 4;
				}
				else if(avedepth > 1450)//14.5m
				{
					pautoadjust->searchOklevel = 3;
				}	
				else if(avedepth > 750)//7.5m
				{
					pautoadjust->searchOklevel = 2;
				}
				else if(avedepth > 50)//0.5m
				{
					pautoadjust->searchOklevel = 1;
				}
				Search_LFOK_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB);	

			} 
			else
			{
				pautoadjust->searchlevel++;			
				if(pautoadjust->searchlevel>7)
					pautoadjust->searchlevel =1;				
				Search_LFParameters(pautoadjust->searchlevel, p_P_SYS_CB);	

			}							
		}
		else if(pautoadjust->searchstatus == SEARCH_OK)
		{
			LF_Auto_Scope(pautoadjust->searchOklevel);  //根据水深选择相应的配置参数

			
			if(avedepth  >  pautoadjust->maxdepth) //水深趋势变深
			{
			 	pautoadjust->searchOklevel++;
				if(pautoadjust->searchOklevel>=7)
					pautoadjust->searchOklevel =7;
				Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				
			}	
			else if(avedepth <  pautoadjust->mindepth)	//水深趋势变浅 应该增大时间门参数
			{
				pautoadjust->searchOklevel--;
				if(pautoadjust->searchOklevel<=1)
					pautoadjust->searchOklevel = 1;

				if( Sys_CB_Copy.Sounder.filter.channel_one > 0 && Sys_CB_Copy.Sounder.filter.channel_one < 0.3f)
						Sys_CB_Copy.Sounder.filter.channel_one = 0.3f;
				Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);

			}
			if(Sys_CB_Copy.Sounder.startgain.channel_one<5)
				Sys_CB_Copy.Sounder.startgain.channel_one=35;		
			
//			if(avedepth <  400)		
//			{
//				Sys_CB_Copy.Sounder.filter.channel_one = 0;
//				Sys_CB_Copy.Sounder.startgain.channel_one=40;
//			}
//			else
			if(pEcho_ProcCal->avestrenth >136 ||  pEcho_ProcCal->wdth > 80 || pCal_Deep->basepulse>50 || pEcho_ProcCal->echonum>5)//增益递减条件
			{
				//低频修改为先判断是否增益递减、再判断是否增益递增
				Sys_CB_Copy.Sounder.startgain.channel_one--;	
				if(Sys_CB_Copy.Sounder.startgain.channel_one<25)
					Sys_CB_Copy.Sounder.startgain.channel_one=25;
					
				//判断是否需要上传自动测量参数
				if(pEcho_ProcCal->avestrenth >150 ||  pEcho_ProcCal->wdth > 80*2 || pCal_Deep->basepulse>50*2 || pEcho_ProcCal->echonum>6)
					Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				else
					Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);	
				
				//根据水深稳定度设置时间门
				if(DeepTrend[LF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.5;
				else if(DeepTrend[LF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_one = 0;	
				
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}				
			}
				//pCal_Deep->basepulse = 30 -->1.11m
			else if(pEcho_ProcCal->avestrenth < 128 || pEcho_ProcCal->strenth < 500 || pCal_Deep->basepulse < 5) //增益递增条件
			{
				Sys_CB_Copy.Sounder.startgain.channel_one++;
				if(Sys_CB_Copy.Sounder.startgain.channel_one>=60)//48)
					Sys_CB_Copy.Sounder.startgain.channel_one=60;//48;
				
									
				if(DeepTrend[LF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.5;
				else if(DeepTrend[LF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}				
			}			
			else
			{
				if(avedepth<400) //水浅时时间门小于50%效果不好
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}
				else
				{
					//当增益稳定时，根据水深稳定度 再减小时间门
					if(DeepTrend[LF].longsteadyflag == 1)				
						Sys_CB_Copy.Sounder.filter.channel_one = 0.3;
					else if(DeepTrend[LF].shortsteadyflag == 1)				
						Sys_CB_Copy.Sounder.filter.channel_one = 0.4;
					else					
						Sys_CB_Copy.Sounder.filter.channel_one = 0.4;
				}					
		
			}
			if(p_P_SYS_CB->Water_Deep == Sys_CB.Parameters.draft)
			{
				Sys_CB_Copy.Sounder.filter.channel_one = 0;				
			}
			*p_P_SYS_CB->p_Sounder->p_filter = Sys_CB_Copy.Sounder.filter.channel_one;	//同步时间门参数	
		}
	}
}

/*************************************************************************************************/
//	自动换挡
/*************************************************************************************************/
static volatile char gradevalue = 0; //
void Auto_change_grade(uint32_t deep) 
{
	char lastgradevalue  = gradevalue;
	
	switch(gradevalue)
	{
		case 0:
			if(deep>Sys_CB.Parameters.draft*100 && deep <500)
				gradevalue =1;			
			else if(deep >650 && deep <1400)
				gradevalue =2;
			else if(deep >1300 && deep <2900)
				gradevalue =3;
			else if(deep >2700 && deep <5900)
				gradevalue =4;
			else if(deep >5800 && deep <11000)
				gradevalue =5;	
			else if(deep >10000 && deep <23000)
				gradevalue =6;
			else if(deep >22000 && deep <30000)
				gradevalue =7;	
			else if(deep==Sys_CB.Parameters.draft*100)   // 没水深时档位自动设成3档
				gradevalue =3;
			break; 		
		case 1:
			if(deep >650 && deep <1400)
				gradevalue =2;
			else if(deep >1300 && deep <2900)
				gradevalue =3;
			else if(deep >2700 && deep <5900)
				gradevalue =4;
			else if(deep >5800 && deep <11000)
				gradevalue =5;	
			else if(deep >11000 && deep <23000)
				gradevalue =6;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break; 
		case 2:
			if(deep>Sys_CB.Parameters.draft*100 && deep <500)
				gradevalue =1;
			else if(deep>1300 && deep<2900)
				gradevalue =3;
			else if(deep >2700 && deep <5900)
				gradevalue =4;
			else if(deep>5800 && deep<11000)
				gradevalue =5;
			else if(deep >11000 && deep <23000)
				gradevalue =6;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break;
			
		case 3:
			if(deep >Sys_CB.Parameters.draft*100 && deep <700)
				gradevalue =1;
			else if(deep>600 && deep<1200)
				gradevalue =2;
			else if(deep >2700 && deep <5900)
				gradevalue =4;
			else if(deep>5800 && deep<11000)
				gradevalue =5;
			else if(deep >11000 && deep <23000)
				gradevalue =6;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break;
			
		case 4:
			if(deep >Sys_CB.Parameters.draft*100 && deep <700)
				gradevalue =1;
			else if(deep>600 && deep<1400)
				gradevalue =2;
			else if(deep >1300 && deep <2600)
				gradevalue =3;
			else if(deep>5800 && deep<11000)
				gradevalue =5;
			else if(deep >11000 && deep <23000)
				gradevalue =6;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break;
			
		case 5:
			if(deep >Sys_CB.Parameters.draft*100 && deep <700)
				gradevalue =1;
			else if(deep>600 && deep<1400)
				gradevalue =2;
			else if(deep >1300 && deep <2700)
				gradevalue =3;
			else if(deep>2600 && deep<5600)
				gradevalue =4;
			else if(deep >11000 && deep <23000)
				gradevalue =6;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break;
		case 6:
			if(deep >Sys_CB.Parameters.draft*100 && deep <700)
				gradevalue =1;
			else if(deep>600 && deep<1400)
				gradevalue =2;
			else if(deep >1300 && deep <2700)
				gradevalue =3;
			else if(deep>2600 && deep<5800)
				gradevalue =4;
			else if(deep >5700 && deep<10000)
				gradevalue =5;
			else if(deep >23000 && deep <30000)
				gradevalue =7;				
			break;
		case 7:
			if(deep >Sys_CB.Parameters.draft*100 && deep <700)
				gradevalue =1;
			else if(deep>600 && deep<1400)
				gradevalue =2;
			else if(deep >1300 && deep <2700)
				gradevalue =3;
			else if(deep>2600 && deep<5800)
				gradevalue =4;
			else if(deep >5700 && deep<10000)
				gradevalue =5;
			else if(deep >11000 && deep <23000)
				gradevalue =6;			
			break;			
		default:
			break;
	}		

	Sys_CB.Grade.auto_grade = gradevalue;
	Sys_CB_Copy.Grade.auto_grade = gradevalue;
	if(lastgradevalue != gradevalue)
	{
		setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB.Grade.auto_grade, Byte_Type);	
	}	

}
