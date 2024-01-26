/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Ultrason_Cal.C
**��   ��   ��: lzb
**����޸�����: 2018��9��20��
**��        ��: ���ݴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/
#include "board.h"
#include "Ultrason_Cal.h"
#include "Ultrason_Airth.h"
#include "Ultrason_rcv.h"
#include "string.h"
#include <math.h>	//fabs()
#include <stdlib.h> //abs()


/*ע��*/
/*
LF= 0�� Channel_One = 1
HF= 1�� Channel_Two = 2
�������������Channel ������Ҫ��Channel_One/Channel_Two�жϸߵ�Ƶͨ��
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
//	�����ʼ��-32λ
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
//	�����ʼ��-8λ
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
//	ģ��ˮ����Ҳ�
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

	//ģ��ز�
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
//	1����������
/*************************************************************************************************/
char Seperate_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	FiltWave_Typedef *pFilstWave = FiltWave + Channel -1;//���������ṹ��
	Cal_deep_Typedef *pCal_Deep  = Caldeep  + Channel -1;//�������ˮ��ֵ

	uint32_t begintime = 0;											    //��ʼʱ��;
	uint32_t endtime = 1;                                               //����Ľ�����ʱ��

	uint32_t totalstrenth = 0;                                          //������
	uint32_t totalwdth = 0;                                             //������
	uint32_t avestrenth = 0;                                            //ƽ������

	uint32_t iLoop,jLoop=0;						                	
	uint8_t reonecnt =0;
	uint8_t threshold = 0;
	
//	uint8_t max_value = 0;
	
	if(Channel == Channel_Two)
		threshold = (*p_P_SYS_CB->p_Sounder->p_threshold)*15;
	else
		threshold = (*p_P_SYS_CB->p_Sounder->p_threshold)*(*p_P_SYS_CB->p_Sounder->p_threshold)*(*p_P_SYS_CB->p_Sounder->p_threshold)+5;
	
	pFilstWave->chrctnum = 0;
	
	Intit_Arry(&pFilstWave->chrct[0],0,25*5);  //��ʼ���ٴ��������
	
	if(*p_P_SYS_CB->p_Sounder->p_clearwidth >= Sys_CB.Parameters.draft) //����ˮ�����
		pCal_Deep->cltdeeptcik = (uint32_t)((*p_P_SYS_CB->p_Sounder->p_clearwidth - Sys_CB.Parameters.draft)*2/(p_P_SYS_CB->ADC_Samp_Time*1480));	
	else
		pCal_Deep->cltdeeptcik = 0;	//(uint32_t)((*p_P_SYS_CB->p_Sounder->p_clearwidth)*2/(p_P_SYS_CB->ADC_Samp_Time*1480));	
		
	*(p_P_SYS_CB->p_UltraDat + p_P_SYS_CB->UltraDat_Len-1) = 0;
	*(p_P_SYS_CB->p_UltraDat + p_P_SYS_CB->UltraDat_Len) = 0; //�������ǿ��Ϊ��	
	
	for(iLoop=0 ;iLoop<p_P_SYS_CB->UltraDat_Len; iLoop++ )
	{

		
		/*��ֵ����*/
		if(*(p_P_SYS_CB->p_UltraDat + iLoop)>=threshold && iLoop!=p_P_SYS_CB->UltraDat_Len)
		{			
			//*(p_P_SYS_CB->p_UltraDat + iLoop) -= (*p_P_SYS_CB->p_Sounder->p_threshold*10);	// �ز���ֵ�˳�(��������)
			if(endtime != 0)
			{
				endtime = 0;
				begintime = iLoop;		
			}
	
			totalwdth    += 1;                                     	//�����ۼƿ��			
			totalstrenth += *(p_P_SYS_CB->p_UltraDat + iLoop);	  	//������
			avestrenth    = totalstrenth/totalwdth;                 //ƽ������	
			
//			if(Channel == Channel_One) //��Ƶ���μ���Ҷ�
//			{
//				*(p_P_SYS_CB->p_UltraDat + iLoop) *= 1.5;	
//				if(*(p_P_SYS_CB->p_UltraDat + iLoop)>255)
//					*(p_P_SYS_CB->p_UltraDat + iLoop) = 255;				
//			}			
//			
//			if(Channel == Channel_Two && totalwdth/10>120*3) 		//3��������� 3*1.2ms
//				return 0;
//			else if(Channel == Channel_One && totalwdth/10>12*6)	//6���������-����� 6*1.2ms
//				return 0;
			
			
		}
		else
		{
			if(*(p_P_SYS_CB->p_UltraDat + iLoop) != 0)
				*(p_P_SYS_CB->p_UltraDat + iLoop) = 0;			//�ز���ֵ�˳�
			if(totalstrenth!=0)	  								//�ҵ���ֵΪ0�ĵ�
			{
				endtime = iLoop;  								//����������ʱ��ֵ
				if(reonecnt == 0)                      			//�ҵ���һ�������ĩβʱ��㣬�жϵ�һ������Ŀ�ȡ�����ʹ�ã�����֤�����ԡ�
				{
					if(totalwdth>4)							    //��һ������δ���Ƿ��䲨��������С�Ӳ�
					{
						reonecnt = 1;
						pFilstWave->starttime = begintime;	         		//��һ�����䲨����ʼʱ���
						pCal_Deep->cltdeeptcik += pFilstWave->starttime; 	//����һ��
						pCal_Deep->basestrenth     = totalstrenth ;  //pFilstWave->chrct[1];	//���䲨��������
						pCal_Deep->basepulse       = totalwdth ;     //pFilstWave->chrct[2];	//���䲨��������
						pCal_Deep->baselavestrenth = avestrenth ;    //pFilstWave->chrct[3];	//���䲨��ƽ������						
					}
				}
				if(jLoop < 25 && reonecnt == 1 && totalwdth > pFilstWave->chrct[2+0*5]/10)//&& avestrenth > 30 )//����ز�
				{
					pFilstWave->chrct[0+jLoop*5] =  begintime;							//��ʼʱ��
					pFilstWave->chrct[1+jLoop*5] = 	totalstrenth;   					//������
					pFilstWave->chrct[2+jLoop*5] = 	totalwdth;						    //������
					pFilstWave->chrct[3+jLoop*5] = 	avestrenth;							//ƽ������
					pFilstWave->chrct[4+jLoop*5] = 	endtime+2;							//�������ʱ��ֵ
					pFilstWave->chrctnum = jLoop ;    									//ͳ�ư���ĸ���			
		
					if((begintime >= pCal_Deep->cltdeeptcik))        					//ʱ����������ˮ���
					{   //�������������ˮ�������ã����ڰ���ɸѡ�Ľ���ˮ��������Ч
						if(jLoop < 24)               //��������ͳ��				
							jLoop++;
						else
							break;
					}
					else
						jLoop = 1; 	//ȥ������ˮ��֮ǰ�İ��磬�������䲨			
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
//	2���Ƚ�����
/*************************************************************************************************/	
void Compare_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)	 //50  37   135 1M
{
	FiltWave_Typedef	*pFilstWave = FiltWave+ Channel -1;	           //������İ�������ֵ
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;			   //�������ˮ��ֵ
	Echo_ProcCal_Typedef *pEcho_ProcCal =  Echo_ProcCal+ Channel -1;     //���������������ֵ

	static char hfrecut = 0;
	static char lfrecut = 0;
	uint32_t Deepwspoint;	//ˮ�������ʼ��
	
	uint32_t iLoop;

	uint32_t echostrenindex;
	uint32_t echoaveindex;
	uint32_t echowdthindex;
	
	
	if(pEcho_ProcCal->strenthindex>19)//��ֹԽ��
	{
		pEcho_ProcCal->strenthindex = 19;
		pEcho_ProcCal->avestreindex = 19;
		pEcho_ProcCal->wdthindex    = 19; 
	}
	 echostrenindex = pEcho_ProcCal->strenthindex;          //��ǰͳ�Ƶ��������ĵ���
	 echoaveindex   = pEcho_ProcCal->avestreindex;          //����ͳ�Ƶ�ƽ�������ĵ���
	 echowdthindex  = pEcho_ProcCal->wdthindex;              //��ǰͳ�Ƶ�����ĸ���

	if(pFilstWave->chrctnum == 0)		//��ǰ��ˮ�����
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
	else if(pFilstWave->chrctnum > 0)  //���ٻ��һ������
	{
		char lstrenth =0,lwdth=0,lavestrenth=0;
		uint32_t chrctstrenth 	 = 0;   //������
		uint32_t chrctwdth    	 = 0;   //����
		uint32_t chrctavestrenth = 0;   //ƽ������
		
		pCal_Deep->tick = 0;
		pEcho_ProcCal->echonum = pFilstWave->chrctnum+1;	
		
		if(Channel == Channel_Two)
			hfrecut = 0;
		else
			lfrecut = 0;
		
		for(iLoop = 1; iLoop <= pFilstWave->chrctnum; iLoop++)
		{
			if(pFilstWave->chrct[0+iLoop*5] >= pCal_Deep->filttick_begin && pFilstWave->chrct[0+iLoop*5] <= pCal_Deep->filttick_end)	//����С������ˮ��İ�����ʱ������İ���
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
					//ǳˮģʽ
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
				else if((*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic == 2 || *p_P_SYS_CB->p_Sounder->p_forcedlockingecho) && Channel == Channel_One)	//��Ƶ�ο���Ƶ�ز�
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
					else if(chrctstrenth == 0)	//û�����ɹ�
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
			if(lstrenth==lwdth && lstrenth==lavestrenth) //���߶���� 
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
			else if(lstrenth==lavestrenth) //������ƽ���������
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
			else if(lstrenth==lwdth)  //�������������
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
			
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB.Survey_Status.channel_two && Sys_CB.Survey_Status.channel_one && Channel == Channel_One) //�����˫Ƶģʽ, ��Ƶˮ�����Ƶˮ����жԱ�
			{	
				//��Ƶ��������20us����Ƶ50us
				if(((((pCal_Deep->tick)*LF_Freq/HF_Freq - pFilstWave->starttime) > ((Caldeep[1].tick-FiltWave[1].starttime)*1.5))  || \
				    (((pCal_Deep->tick)*LF_Freq/HF_Freq - pFilstWave->starttime) < ((Caldeep[1].tick-FiltWave[1].starttime)*0.8))) && \
					   Caldeep[1].tick>30) //������ڸ�Ƶˮ����1.8���������¼���
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
		
		if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Channel == Channel_Two)//Ѱ�ҶԱ����հ����Ƿ�Ϊ2�λز����ڴ˽��ж϶��λز������λ��Ĵλز���ʱ���Ƚ�			
		{
			for(iLoop=1 ; iLoop<pFilstWave->chrctnum; iLoop++)	         //���λز��� ����ֵΪ  ��������� 40  ƽ����������8   (�������ݿ��Լ����Ƚ�����)		
			{
				if((pFilstWave->chrct[iLoop*5] > (pCal_Deep->tick /2-pCal_Deep->tick/10)) && (pFilstWave->chrct[iLoop*5] < (pCal_Deep->tick /2+pCal_Deep->tick/10))\
					&& (pFilstWave->chrct[3+iLoop*5] > (pFilstWave->chrct[3+Deepwspoint*5]-pFilstWave->chrct[3+Deepwspoint*5]/10))) 
				{
					pCal_Deep->tick = 	pFilstWave->chrct[iLoop*5];
					pCal_Deep->endtick = 	pFilstWave->chrct[4+iLoop*5];  //�������հ����  ����ʱ���	
					
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
//	3����������ͳ�Ʒ����������Զ�����
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

	
	strindex = pechoprocal->strenthindex;    //��ǰͳ�Ƶ�����������
	aveindex = pechoprocal->avestreindex;    //��ǰͳ�Ƶ�ƽ����������
	wdthindex = pechoprocal->wdthindex;      //��ǰͳ�Ƶ�����ĸ���   

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
//   4����������
/*************************************************************************************************/
void  Calculate_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{
	float value1 = 0.0;
	char  i = 0;
    int   Tmp = 0;
    
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;
	FiltWave_Typedef	*pFilstWave = FiltWave+ Channel -1;	
	
	if(Channel == Channel_One)
	{  //������С���ɷ�  ȷ�����ˮ�Χֵ
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
				if(MinMuxPirtick[LF][i] < MinMuxPirtick[LF][i+1]) //�ж��Ƿ�Ϊˮ���ǳ
					break; 
				if(i==LFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[LF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[LF][i+1];					
			}
			for(i=0; i<LFIndex-1; i++)
			{
				if(MinMuxPirtick[LF][i] > MinMuxPirtick[LF][i+1]) //�ж��Ƿ�Ϊˮ�����
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
				if(MinMuxPirtick[HF][i] < MinMuxPirtick[HF][i+1]) //�ж��Ƿ�Ϊˮ���ǳ
					break; 
				if(i==HFIndex-2 && pCal_Deep->muxminpirtick>MinMuxPirtick[HF][i+1]/4)
					pCal_Deep->muxminpirtick = MinMuxPirtick[HF][i+1];					
			}
			for(i=0; i<HFIndex-1; i++)
			{
				if(MinMuxPirtick[HF][i] > MinMuxPirtick[HF][i+1]) //�ж��Ƿ�Ϊˮ�����
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


	//��ɳ����У׼ 20190925
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
		if(Sys_CB.Survey_Status.channel_two && Sys_CB.Survey_Status.channel_one && Channel == Channel_One && pFilstWave->chrctnum)//˫Ƶģʽ���ֵ�Ƶˮ��������
		{
			if(Sys_CB_L.Water_Deep == Sys_CB.Parameters.draft && Sys_CB_H.Water_Deep != Sys_CB.Parameters.draft)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}
			//20190925 ���� 
			//�����Ƶ�����ڸ�Ƶ����������С�ڸ�Ƶ-5cm����֤��Ƶ���ȶ���
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB_L.Water_Deep <  Sys_CB_H.Water_Deep-0.05)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}	
			//�����Ƶ�����ڸ�Ƶ�������������ڸ�Ƶ*1.5����֤��Ƶ���ȶ���
			if(*p_P_SYS_CB->p_Sounder->p_indentifyarithmetic !=3 && Sys_CB_L.Water_Deep >  Sys_CB_H.Water_Deep*1.5)
			{	
				Sys_CB_L.Water_Deep = Sys_CB_H.Water_Deep;	
				pCal_Deep->deep = Sys_CB_H.Water_Deep;
			}
			//�����Ƶ�����ڸ�Ƶ�������������ڸ�Ƶ+10cm,С�ڸ�Ƶ+1m����֤��Ƶ���ȶ���
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
//	5��ʱ�����޼���
/*************************************************************************************************/
void Filt_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel)
{

	Cal_deep_Typedef *pCal_Deep = Caldeep + Channel -1;	
	
	uint32_t value1;	
	//uint8_t basetick =  7  ;        //140us = 0.104m
	//if(Channel == Channel_One)
	//	basetick = 3;				//150us = 0.111m
	
	if(pCal_Deep->muxminpirtick < 10  || ((uint32_t)(*p_P_SYS_CB->p_Sounder->p_filter*100))<10)	//ʱ����δ����
	{
		pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	 				//  ����ˮ�
		pCal_Deep->filttick_end   =   p_P_SYS_CB->UltraDat_Len_Buf[7]-1;		//  ��������ĩβ
	}
	else
	{
		//value1 = basetick * ((uint32_t)(*p_P_SYS_CB->p_Sounder->p_filter*100)); //HF 10%=15*10=1.04m// 20%=2.08m  
		value1 = pCal_Deep->tick * (*p_P_SYS_CB->p_Sounder->p_filter); //HF 10%=15*10=1.04m// 20%=2.08m  

		if(pCal_Deep->tick > pCal_Deep->muxminpirtick*0.8 && pCal_Deep->tick < pCal_Deep->muxminpirtick*1.2)
		{
			if(pCal_Deep->tick > value1+1)
				pCal_Deep->filttick_begin =   pCal_Deep->tick - value1;	//ʱ����ǰ��
			else 
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	
			
			if(pCal_Deep->filttick_begin  <   pCal_Deep->cltdeeptcik)  //��ʼʱ����С������ˮ��
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik;

			pCal_Deep->filttick_end       =   pCal_Deep->tick + value1;  	//ʱ���ź���		

//			if(Channel == Channel_One && (value1*2) < (1400/LF_Freq))  //1400us ~= 1m   //ʱ������СΪ1m ��ֹˮǳ������
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/LF_Freq);
//			
//			else if(Channel == Channel_Two && (value1*2) < (1400/HF_Freq))  //1400us ~= 1m
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/HF_Freq);
			if(Channel == Channel_One && pCal_Deep->filttick_begin > (200/LF_Freq))  //1400us ~= 1m   //�����·���ӳ�
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
				pCal_Deep->filttick_begin =   pCal_Deep->muxminpirtick - value1;	//ʱ����ǰ��
			else 
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik ;	
			
			if(pCal_Deep->filttick_begin  <   pCal_Deep->cltdeeptcik)
				pCal_Deep->filttick_begin =   pCal_Deep->cltdeeptcik;

			pCal_Deep->filttick_end       =   pCal_Deep->muxminpirtick + value1;  	//ʱ���ź���	
			
//			if(Channel == Channel_One && (value1*2) < (1400/LF_Freq))  //1400us ~= 1m  
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/LF_Freq);
//			
//			else if(Channel == Channel_Two && (value1*2) < (1400/HF_Freq))  //1400us ~= 1m
//				pCal_Deep->filttick_end = pCal_Deep->filttick_begin + (1400/HF_Freq);
			if(Channel == Channel_One && pCal_Deep->filttick_begin > (200/LF_Freq))  //1400us ~= 1m   //�����·���ӳ�
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
//	6���������
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
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
// 1����̬����
//$PSIC,ESI,��Ƶˮ�����ݣ���Ƶˮ�����ݣ�ע��ʱ�䣬����ʱ�䣬��ǰ��ѹ��������λ*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$PSIC,ESI,",sizeof("$PSIC,ESI,")-1);

	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//��Ƶˮ��
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.reg_time);//ע��ʱ��
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.used_time);	//ʹ��ʱ��
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB.Voltage.voltage);	//��ǰ��ѹ	
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Grade.grade);		//������λ

	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
	
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';		
	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);


}


/*1.1.1 SIC ���ݸ�ʽ��*/
void SIC_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char len = 0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
//DA00,��Ƶˮ�����ݣ���Ƶˮ������ m*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"@SIC,,GET,DATA.DEEP",sizeof("@SIC,,GET,DATA.DEEP")-1);

//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//��Ƶˮ��
	len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "O");	
    len += sprintf(&UART1_TxBuf[len], "K");	
    len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��
//    len += sprintf(&UART1_TxBuf[len], " ");	
//    len += sprintf(&UART1_TxBuf[len], "m");	
	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
//    UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*������У��
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
	
		//�ͷ��źţ���ֹ��������
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // ��Ƶ
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//

}


/*1.1.2 DES25���ݸ�ʽ:DA00124.45 m*/
void DES25_DatOut(P_SYS_CB *p_P_SYS_CB)
{
	char len = 0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
// 1����̬����
//DA00,��Ƶˮ�����ݣ���Ƶˮ������ m*CRC
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"DA00",sizeof("DA00")-1);

//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//��Ƶˮ��
//	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��
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
	
		//�ͷ��źţ���ֹ��������
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // ��Ƶ
		{	
			rt_event_send(&HL_Pro_CMP_event, (1 << 0));
		}	
		else
		{
			rt_event_send(&HL_Pro_CMP_event, (1 << 1));
		}
//	}//

}
/*1.1.3 DBT ���ݸ�ʽ$SSDTB,123.*/
void DBT_DatOut(P_SYS_CB *p_P_SYS_CB)
{ 
	char  len = 0;
    float Water_H_FDepth   = 0.0;
    float Water_H_FatDepth = 0.0;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
// 1����̬����
//DA00,��Ƶˮ�����ݣ���Ƶˮ������ m*CRC
	Water_H_FDepth = Sys_CB_H.Water_Deep*3.2808399f;
    Water_H_FatDepth = Water_H_FDepth / 6.0f;
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$SSDBT",sizeof("$SSDBT")-1);
//    len += sprintf(&UART1_TxBuf[len], ",");	
//	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//��Ƶˮ��
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Water_H_FDepth);//��Ƶˮ�Ӣ�ߣ�
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "f");	
    len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��ף�
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "M");	
    len += sprintf(&UART1_TxBuf[len], ",");
    len += Float_to_Str(&UART1_TxBuf[len],  Water_H_FatDepth);//��Ƶˮ�6Ӣ�ߣ�
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += sprintf(&UART1_TxBuf[len], "F");	
	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
 
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
    
	UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*������У��
	len++;
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 4)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 3)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//�ͷ��źţ���ֹ��������
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // ��Ƶ
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


	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$GPSSS",sizeof("$GPSSS")-1);
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��ף�
	
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  1480);//����
	
    len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  0.5);//��ˮ
	
	len += sprintf(&UART1_TxBuf[len], ",");	
    len += Float_to_Str(&UART1_TxBuf[len],  0.3);//��������

	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
 
	UART1_TxBuf[len] = do_crc(UART1_TxBuf,len-1);			// crc,*������У��
	len++;
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
//	if(Tran_Flag == 4)	
//		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
//	if(Tran_Flag == 3)
//	{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)UART1_TxBuf, len);
	
		//�ͷ��źţ���ֹ��������
		if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL) // ��Ƶ
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
	uint16_t First_Valid_Index = 0;							// ��¼���䲨��Ч�ĵ�һ��������
	uint16_t i;
	uint16_t  validratio = 0;	
	float tmp_float;
	
	Cal_deep_Typedef    *pCal_Deep  = Caldeep + Channel -1;
	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	*p_P_SYS_CB->p_UART_Dat       = '$';
	*(p_P_SYS_CB->p_UART_Dat + 1) = 'E';
	*(p_P_SYS_CB->p_UART_Dat + 2) = 'S';
	*(p_P_SYS_CB->p_UART_Dat + 3) = 'P';
	*(p_P_SYS_CB->p_UART_Dat + 4) = 'L';
	*(p_P_SYS_CB->p_UART_Dat + 5) = 'U';
	*(p_P_SYS_CB->p_UART_Dat + 6) = 'S';
	tmp_float = p_P_SYS_CB->Water_Deep;
	tmp_float = tmp_float*100;								// ����100��,��λΪcm	
	tmp_int = (uint16_t)tmp_float;
	
	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)				// ��Ƶ
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
	else													// ��Ƶ
	{
		*(p_P_SYS_CB->p_UART_Dat + 7) = 'H';
		*(p_P_SYS_CB->p_UART_Dat + 8) = 'F';
#ifdef HF_20us		
		*(p_P_SYS_CB->p_UART_Dat + 9) = 20;					// 10uS
#else
		*(p_P_SYS_CB->p_UART_Dat + 9) = 10;
#endif
	}

	*(p_P_SYS_CB->p_UART_Dat + 10) = tmp_int>>8;			// ˮ����ֽ�
	*(p_P_SYS_CB->p_UART_Dat + 11) = tmp_int;				// ˮ����ֽ�
	
	Envelope_Index = 18;									// �ó���ǰ�������������͵�ǰ�������ԭ������������4���ֽ�
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
			if(Envelope_Len==1)			// ֻ�ڵ�һ������ʱ���¼����
			{
				if(First_Valid_Index==0)
					First_Valid_Index = i;
				

				if(Channel == Channel_Two)
					EnvelopeStart_Index = i - First_Valid_Index;
				else
				{
					if(i - First_Valid_Index>0)
						EnvelopeStart_Index = i - First_Valid_Index - 4; //��ȥ����֤ˮ�����벨��һ��
					else
						EnvelopeStart_Index = i - First_Valid_Index;					
				}					
			}
			
			validratio++;
			
			if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
			{
				//�����ϴ��ز�����
				if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))//30%			// ��ֹԽ��
				{					
					break;
				}
			}
			else
			{
				//�����ϴ��ز�����
				if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))//30%			// ��ֹԽ��
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

	if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
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
//	7, ���ݷ��� 
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
	
	if(index <i_index) //20 //����Զ������Ŀ�����
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
		aveinput =aveinput/(index);	//һ��ͳ�����ڵ�ƽ��ˮ��ֵ
				
		for(iLoop = 0; iLoop<index ;iLoop++)
		{
			if(inputdep[iLoop] >= aveinput )
			{
				afteraveindex++;  //ͳ��һ�����ڴ���ƽ�����ݵĸ���
				afteravedeep +=inputdep[iLoop]; 					
			}
			else
			{
				beforeaveindex++;	//ͳ��һ������С��ƽ�����ݵĸ���
				beforeavedeep +=inputdep[iLoop]; 					
			}		
		}
		
		if(beforeaveindex >= afteraveindex)	 //����ƽ�����ݵĵ����϶�
		{
			aveoutput = beforeavedeep/beforeaveindex; 			
		}
		else
		{
			aveoutput = afteravedeep/afteraveindex;	
		}
		
		*pincount =0;	
/*ˮ���ȶ��ж�-S*/	
		if(my_abs((int)(deep),(int)aveoutput) <=150)
		{
			JudgePara[Channel-1].water_Steady_flag = 1 ;	  //ˮ���ȶ�
		}
		else 
		{
			JudgePara[Channel-1].water_Steady_flag = 0 ; 	
		}
	
		if(JudgePara[Channel-1].water_Steady_flag == 1)
		{
			DeepTrend[Channel-1].longsteadycount++;	//�ۼ�ˮ���ȶ���ͳ��������
			DeepTrend[Channel-1].shortsteadycount++;	
			if(DeepTrend[Channel-1].longsteadycount >= LONGSTEADYCNT)
			{
				DeepTrend[Channel-1].longsteadyflag = 1;		   //�����ȶ��ı�־
				DeepTrend[Channel-1].longsteadycount = 0;				
			}
			if(DeepTrend[Channel-1].shortsteadycount >= SHORTSTEADYCUN)
			{
			 	DeepTrend[Channel-1].shortsteadyflag =1;		 //�����ȶ��ı�־
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
/*ˮ���ȶ��ж�-E*/		
		
		*outputdep = aveoutput;
		
		//8��  ����	�������
		PowerInfo_Compare(&Echo_ProcCal[Channel-1], Channel);


		Autoadjust[Channel-1].processflag  = 1;
//		Autoadjust[Channel-1].sendprocflag = 1;	

	}

	return outputdep[outputdex];	
}
/*************************************************************************************************/
//	�������ݴ���+ˮ�����
/*************************************************************************************************/
//char hlf_i = 0;
void FiltWave_Excute(P_SYS_CB *p_P_SYS_CB,  uint8_t Channel)
{	
	char ware;
	
//	hlf_i++;
//	if(hlf_i>= 4 + 8/(Sys_CB.Grade.grade+1)) //���ٷ���A5��������
//	{
//		hlf_i = 0;
//		Unmanned_ship_DatOut(p_P_SYS_CB);
//		
//	}	
	
	Unmanned_ship_DatOut(p_P_SYS_CB);
	
	//0��ģ������
//	Simulation_Ware(p_P_SYS_CB, Channel);
	
	//1��������ȡ����	
	ware = Seperate_Sampdata(p_P_SYS_CB, Channel);
	
	//DepthInfo[Channel-1].echocntnum = ware;
	
//	if(ware != 0)
//	{
		//2���Ƚ�����
		Compare_Sampdata(p_P_SYS_CB, Channel);
	
		//3����������
		Calculate_Sampdata(p_P_SYS_CB, Channel);

		//4��ʱ�����޼���
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
            GPSS_DatOut(p_P_SYS_CB);//�������ݵ�A5
            break;
        
        case 5:
            Combine_Sampdata(p_P_SYS_CB, Channel);
            break;
               
        default:
            break;
    }
	//5����ϡ���������
//	Combine_Sampdata(p_P_SYS_CB, Channel);
	



	
	//6, ���ݷ���    ˮ���жϣ�ֻ��ӳ��ǰ��ˮ�Χ������Ҫ�����ıȽ�
	DepthInfo_Compare((uint16_t)(p_P_SYS_CB->Water_Deep*100),\
					  DepthInfo[Channel-1].inputdep,\
					  &DepthInfo[Channel-1].outputavedep,\
	                  &DepthInfo[Channel-1].inputcount,\
	                  &DepthInfo[Channel-1].outputcount, Channel);
	
	
}
