/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Auto_Adjust.C
**��   ��   ��: lzb
**����޸�����: 2018��9��20��
**��        ��: �Զ�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
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
//	��Ƶ�Զ�����
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
		if(pautoadjust->searchstatus == SEARCH_NONE)	// ��ʼ����������
		{
			pautoadjust->searchlevel = 1;
			pautoadjust->searchstatus = SEARCH_READY;		
			Search_HFParameters(pautoadjust->searchlevel, p_P_SYS_CB);	
			
		}
		else if(pautoadjust->searchstatus == SEARCH_READY)   //������������
		{				
			if(avedepth > Sys_CB.Parameters.draft*100)	 //��ˮ��ֵ
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
			HF_Auto_Scope(pautoadjust->searchOklevel);  //����ˮ��ѡ����Ӧ�����ò���

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
				if( Sys_CB_Copy.Sounder.filter.channel_two > 0 && Sys_CB_Copy.Sounder.filter.channel_two < 0.3f) //ˮ���ǳʱ����Ӧ������
					Sys_CB_Copy.Sounder.filter.channel_two = 0.3f;
				Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
			}
			
			if(Sys_CB_Copy.Sounder.startgain.channel_two<5)
				Sys_CB_Copy.Sounder.startgain.channel_two=30;			

			
//			if(avedepth  <  400) //���׹̶�����
//			{
//				Sys_CB_Copy.Sounder.startgain.channel_two = 35;
//				Sys_CB_Copy.Sounder.filter.channel_two = 0;
//			}		
//			else if(pEcho_ProcCal->avestrenth < 166|| pEcho_ProcCal->strenth < 2000 || pCal_Deep->basepulse < 5)  //�����������			
			if(pEcho_ProcCal->avestrenth < 166|| pEcho_ProcCal->strenth < 2000 || pCal_Deep->basepulse < 5)  //�����������
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
				
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}				
				
			}
			else if(pEcho_ProcCal->avestrenth >170 ||  pEcho_ProcCal->wdth > 68 || pCal_Deep->basepulse>68 || pEcho_ProcCal->echonum>4)//����ݼ�����
			{
				Sys_CB_Copy.Sounder.startgain.channel_two--;	
				if(Sys_CB_Copy.Sounder.startgain.channel_two<25)
					Sys_CB_Copy.Sounder.startgain.channel_two=25;	

				//�ж��Ƿ���Ҫ�ϴ��Զ���������
				if(pEcho_ProcCal->avestrenth >182 ||  pEcho_ProcCal->wdth > 68*2 || pCal_Deep->basepulse>68*2 || pEcho_ProcCal->echonum>6)
					Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				else
					Change_HF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				
				//����ˮ���ȶ�������ʱ����
				if(DeepTrend[HF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.5;
				else if(DeepTrend[HF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_two = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}									
			}	
			else 
			{
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_two = 0;
				}
				else
				{
					//�������ȶ�ʱ������ˮ���ȶ��� �ټ�Сʱ����
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
			*p_P_SYS_CB->p_Sounder->p_filter = Sys_CB_Copy.Sounder.filter.channel_two;		//ͬ��ʱ���Ų���						
		}
	}
}

/*************************************************************************************************/
//	��Ƶ�Զ�����
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

		if(pautoadjust->searchstatus == SEARCH_NONE)	// ��ʼ����������
		{
			pautoadjust->searchlevel = 1;
			Search_LFParameters(pautoadjust->searchlevel, p_P_SYS_CB);
			pautoadjust->searchstatus = SEARCH_READY;	
		}
		else if(pautoadjust->searchstatus == SEARCH_READY)   //������������
		{
			if(avedepth > 50+Sys_CB.Parameters.draft*100)	 //��ˮ��ֵ
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
			LF_Auto_Scope(pautoadjust->searchOklevel);  //����ˮ��ѡ����Ӧ�����ò���

			
			if(avedepth  >  pautoadjust->maxdepth) //ˮ�����Ʊ���
			{
			 	pautoadjust->searchOklevel++;
				if(pautoadjust->searchOklevel>=7)
					pautoadjust->searchOklevel =7;
				Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				
			}	
			else if(avedepth <  pautoadjust->mindepth)	//ˮ�����Ʊ�ǳ Ӧ������ʱ���Ų���
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
			if(pEcho_ProcCal->avestrenth >136 ||  pEcho_ProcCal->wdth > 80 || pCal_Deep->basepulse>50 || pEcho_ProcCal->echonum>5)//����ݼ�����
			{
				//��Ƶ�޸�Ϊ���ж��Ƿ�����ݼ������ж��Ƿ��������
				Sys_CB_Copy.Sounder.startgain.channel_one--;	
				if(Sys_CB_Copy.Sounder.startgain.channel_one<25)
					Sys_CB_Copy.Sounder.startgain.channel_one=25;
					
				//�ж��Ƿ���Ҫ�ϴ��Զ���������
				if(pEcho_ProcCal->avestrenth >150 ||  pEcho_ProcCal->wdth > 80*2 || pCal_Deep->basepulse>50*2 || pEcho_ProcCal->echonum>6)
					Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);
				else
					Change_LF_Parameters(pautoadjust->searchOklevel, p_P_SYS_CB, 0);	
				
				//����ˮ���ȶ�������ʱ����
				if(DeepTrend[LF].longsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.5;
				else if(DeepTrend[LF].shortsteadyflag == 1)				
					Sys_CB_Copy.Sounder.filter.channel_one = 0.6;
				else
					Sys_CB_Copy.Sounder.filter.channel_one = 0;	
				
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}				
			}
				//pCal_Deep->basepulse = 30 -->1.11m
			else if(pEcho_ProcCal->avestrenth < 128 || pEcho_ProcCal->strenth < 500 || pCal_Deep->basepulse < 5) //�����������
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
				
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}				
			}			
			else
			{
				if(avedepth<400) //ˮǳʱʱ����С��50%Ч������
				{
					Sys_CB_Copy.Sounder.filter.channel_one = 0;
				}
				else
				{
					//�������ȶ�ʱ������ˮ���ȶ��� �ټ�Сʱ����
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
			*p_P_SYS_CB->p_Sounder->p_filter = Sys_CB_Copy.Sounder.filter.channel_one;	//ͬ��ʱ���Ų���	
		}
	}
}

/*************************************************************************************************/
//	�Զ�����
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
			else if(deep==Sys_CB.Parameters.draft*100)   // ûˮ��ʱ��λ�Զ����3��
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
