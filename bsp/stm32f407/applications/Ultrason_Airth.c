/****************************************Copyright (c)**************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Ultrason_Airth.C
**创   建   人: lzb
**最后修改日期: 2018年9月20日
**描        述: 自动测量参数设定
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
********************************************************************************************************/
#include "Ultrason_Airth.h"
#include "Ultrason_cal.h"
#include "Ultrason_rcv.h"
#include "TIME.h"
#include "DAC.h"
#include "UART_DEVICE.h"

/*************************************************************************************************/
//	档位判断
/*************************************************************************************************/
char Deep_To_Level(int deep)
{
	if(deep > 10000)
	return 6;
	else if(deep >5000)
	return 5;
	else if(deep >3000)
	return 4;
	else if(deep >1000)
	return 3;
	else if(deep>500)
	return 2;
	else 
	return 1;
}

/*************************************************************************************************/
//	高频参数设定
/*************************************************************************************************/
void Change_HF_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB, char ACK)
{
	//uint16_t i;
	//float tmp_float;
	//uint16_t tmp_int;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;

	switch(levelnum)
	{
		case 1:	// 0-6米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.2;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.3;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 2;
		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 2;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 3;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 5;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 4;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 5;
		break;

		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 6;
		
		break;
		case 7:	//220-300 以上

			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.5;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 7;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 7;
		break;
		
		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
		htim1.Instance->ARR  = tmp_freq;		// 直接更新寄存器
		htim1.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim1.Instance->RCR  = tmp_txwidth;		// 直接更新寄存器	
//		if(ACK==1)
//			Sounder_ALLChannel_getCMD_ACK(Channel_Two);
	}	
}
/*************************************************************************************************/
//	高频搜索水深参数
/*************************************************************************************************/
void Search_HFOK_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB)
{
	//uint16_t i;
	//float tmp_float;
	//uint16_t tmp_int;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;
	
	Sys_CB_Copy.Sounder.threshold.channel_two = 2;	
//	Sys_CB_Copy.Sounder.filter.channel_two = 0.5;	
	
	switch(levelnum)
	{
		case 1:	// 0-6米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.2;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.startgain.channel_two = 35;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.3;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.3;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.startgain.channel_two = 37;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;

		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 2;
			Sys_CB_Copy.Sounder.startgain.channel_two = 39;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 41;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 43;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;

		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 44;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;
		case 7:	//220-300 以上
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 45;
			//Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;
			//Sys_CB_Copy.Sounder.filter.channel_two = 10;
		break;
		
		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
//		Sys_CB.Grade.auto_grade = levelnum;
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
		htim1.Instance->ARR = tmp_freq;			// 直接更新寄存器
		htim1.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim1.Instance->RCR = tmp_txwidth;		// 直接更新寄存器	
		
//		Sounder_ALLChannel_getCMD_ACK(Channel_Two);
	}	
}
/*************************************************************************************************/
//	高频搜索水深参数
/*************************************************************************************************/
void Search_HFParameters(char levelnum, P_SYS_CB *p_P_SYS_CB)
{
	//uint16_t i;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;
	
	
	Sys_CB_Copy.Sounder.threshold.channel_two = 3;
	Sys_CB_Copy.Sounder.tvgs.channel_two = 25;	
	Sys_CB_Copy.Sounder.filter.channel_two = 0;	
	
	switch(levelnum)
	{
		case 1:	// 0-6米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.2;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.startgain.channel_two = 35;
			Sys_CB_Copy.Sounder.clearwidth.channel_two = 0.8;

		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.3;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 1;
			Sys_CB_Copy.Sounder.startgain.channel_two = 37;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;

		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 2;
			Sys_CB_Copy.Sounder.startgain.channel_two = 39;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 41;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 43;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;
		break;

		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 44;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;
		break;
		case 7:	//220-300 以上
			Sys_CB_Copy.Sounder.txwidth.channel_two = 0.4;
			Sys_CB_Copy.Sounder.power.channel_two.Power_Index = 6;
			Sys_CB_Copy.Sounder.startgain.channel_two = 45;
			Sys_CB_Copy.Sounder.clearwidth.channel_two =  0.8;
		break;

		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_two,Sys_CB_Copy.Sounder.txwidth.channel_two,&tmp_freq);
		htim1.Instance->ARR = tmp_freq;			// 直接更新寄存器
		htim1.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim1.Instance->RCR = tmp_txwidth;		// 直接更新寄存器	
	
//		Sounder_ALLChannel_getCMD_ACK(Channel_Two);  //1225+
	}	
}

/*************************************************************************************************/
//	高频水深档位变化判断
/*************************************************************************************************/
void HF_Auto_Scope(char level)
{
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+HF;
	switch(level)
	{
		case 1:
			pautoadjust->mindepth = 50;
			pautoadjust->maxdepth = 800;
		break;

		case 2:
			pautoadjust->mindepth = 780;
			pautoadjust->maxdepth = 1400;
		break;

		case 3:
			pautoadjust->mindepth = 1300;
			pautoadjust->maxdepth = 3000;
		break;

		case 4:
			pautoadjust->mindepth = 2900;
			pautoadjust->maxdepth = 6000;
		break;

		case 5:
			pautoadjust->mindepth = 5900;
			pautoadjust->maxdepth = 12000;
		break;
		case 6:
			pautoadjust->mindepth = 11000;
			pautoadjust->maxdepth = 24000;
		break;
		case 7:
			pautoadjust->mindepth = 23000;
			pautoadjust->maxdepth = 30000;
		break;		
		default :
		break;
	}	
}
/*************************************************************************************************/
//	低频搜索水深参数
/*************************************************************************************************/
void Change_LF_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB, char ACK)
{	
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;	
	
	switch(levelnum)
	{
		case 1:	// 0-6米
			// 8米档,以1500m/s来计算，10m，100KHz的采样率，单程约需采集667个数，最终以1200为界，12ms为处理周期
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.2;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 2;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.3;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 2;
		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.4;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 3;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 4;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 5;
		break;

		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 6;
		break;
		
		case 7:	//220-300 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 7;
		break;

		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
		htim8.Instance->ARR = tmp_freq;			// 直接更新寄存器
		htim8.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim8.Instance->RCR = tmp_txwidth;		// 直接更新寄存器			
//		if(ACK==1)
//			Sounder_ALLChannel_getCMD_ACK(Channel_One);
	}
}
/*************************************************************************************************/
//	低频搜索水深参数
/*************************************************************************************************/
void Search_LFOK_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB)
{	
	//uint16_t i;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;	
	
	Sys_CB_Copy.Sounder.threshold.channel_one = 2;
//	Sys_CB_Copy.Sounder.filter.channel_one = 0.5;
	
	switch(levelnum)
	{
		case 1:	// 0-6米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.2;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 2;
			Sys_CB_Copy.Sounder.startgain.channel_one = 35;
		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.3;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 37;
		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.4;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 38;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 40;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 41;
		break;
		
		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 42;
		break;
		
		case 7:	//220-300 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 4;
			Sys_CB_Copy.Sounder.startgain.channel_one = 45;
		break;

		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
//		Sys_CB.Grade.auto_grade = levelnum;
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
		htim8.Instance->ARR = tmp_freq;			// 直接更新寄存器
		htim8.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim8.Instance->RCR = tmp_txwidth;		// 直接更新寄存器	
			
//		Sounder_ALLChannel_getCMD_ACK(Channel_One);
	}
}
/*************************************************************************************************/
//	低频搜索水深参数
/*************************************************************************************************/
void Search_LFParameters(char levelnum, P_SYS_CB *p_P_SYS_CB)
{
	//uint16_t i;
	uint16_t tmp_freq;
	uint16_t tmp_txwidth;	
	
	Sys_CB_Copy.Sounder.threshold.channel_one = 3;
	Sys_CB_Copy.Sounder.tvgs.channel_one = 30;	
	Sys_CB_Copy.Sounder.filter.channel_one = 0;
	
	switch(levelnum)
	{
		case 1:	// 0-6米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.2;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 2;
			Sys_CB_Copy.Sounder.startgain.channel_one = 35;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;			
		break;

		case 2:	 //6-12米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.3;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 37;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		case 3:	 //12-30米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.4;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 38;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		case 4:	//30-50米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 40;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		case 5:	//50 - 100米
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 41;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		case 6:	//100-220 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 3;
			Sys_CB_Copy.Sounder.startgain.channel_one = 42;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;
		
		case 7:	//220-300 以上
			Sys_CB_Copy.Sounder.txwidth.channel_one = 0.6;
			Sys_CB_Copy.Sounder.power.channel_one.Power_Index = 4;
			Sys_CB_Copy.Sounder.startgain.channel_one = 45;
			Sys_CB_Copy.Sounder.clearwidth.channel_one = 0.8;
		break;

		default:
		break;
	}
	if(levelnum>=1 && levelnum<=7)
	{
//		Sys_CB.Grade.auto_grade = levelnum;
		tmp_txwidth = Freq_Width(Sys_CB_Copy.Sounder.frequency.channel_one,Sys_CB_Copy.Sounder.txwidth.channel_one,&tmp_freq);
		htim8.Instance->ARR = tmp_freq;			// 直接更新寄存器
		htim8.Instance->CCR1 = tmp_freq>>1;		// 直接更新寄存器
		htim8.Instance->RCR = tmp_txwidth;		// 直接更新寄存器	
//		Sounder_ALLChannel_getCMD_ACK(Channel_One);  //1225+		
	}	
}

/*************************************************************************************************/
//	低频水深档位判断
/*************************************************************************************************/
void LF_Auto_Scope(char level)
{
	Auto_Adjust_Typedef  *pautoadjust	= Autoadjust+LF;
	switch(level)
	{
		case 1:
			pautoadjust->mindepth = 50;
			pautoadjust->maxdepth = 800;
		break;

		case 2:
			pautoadjust->mindepth = 780;
			pautoadjust->maxdepth = 1400;
		break;

		case 3:
			pautoadjust->mindepth = 1300;
			pautoadjust->maxdepth = 3000;
		break;

		case 4:
			pautoadjust->mindepth = 2900;
			pautoadjust->maxdepth = 6000;
		break;

		case 5:
			pautoadjust->mindepth = 5900;
			pautoadjust->maxdepth = 12000;
		break;
		
		case 6:
			pautoadjust->mindepth = 11000;
			pautoadjust->maxdepth = 24000;
		break;
		
		case 7:
			pautoadjust->mindepth = 23000;
			pautoadjust->maxdepth = 30000;
		break;	
		
		default:
		break;
	}	
}

/*************************************************************************************************/
//	水深算法：最小二乘法
/*************************************************************************************************/
#define MULT_SET	4
static	double	a[MULT_SET],
				b[MULT_SET],
				s[MULT_SET],
				t[MULT_SET];
unsigned long 	MinMuxPir(double y[],int n)
{ 
	int i,j,k;
	double z,p,c,g,q,d1,d2;

	for (i=0; i<=MULT_SET; i++) 
		{a[i]=0.0;}


	z=0.0;

	for (i=0; i<=n-1; i++) 
		{z=z+i/(1.0*n);}

	b[0]=1.0; d1=1.0*n; p=0.0; c=0.0;

	for (i=0; i<=n-1; i++)
	  	{ p=p+(i-z); c=c+y[i];}

	c=c/d1; p=p/d1;

	a[0]=c*b[0];

//	if (m>1) { 
		t[1]=1.0; 
		t[0]=-p;
		d2=0.0; 
		c=0.0; 
		g=0.0;
		for (i=0; i<=n-1; i++) { 
			q=i-z-p; d2=d2+q*q;
		    c=c+y[i]*q;
		    g=g+(i-z)*q*q;
		}
		c=c/d2; p=g/d2; q=d2/d1;
		d1=d2;
		a[1]=c*t[1]; 
		a[0]=c*t[0]+a[0];
//	}

	for (j=2; j<MULT_SET; j++) { 
		s[j]=t[j-1];
	    s[j-1]=-p*t[j-1]+t[j-2];

	    if(j>=3) {
	    	for (k=j-2; k>=1; k--)
	        	{s[k]=-p*t[k]+t[k-1]-q*b[k];}
		}

	    s[0]=-p*t[0]-q*b[0];

	    d2=0.0; c=0.0; g=0.0;

	    for (i=0; i<=n-1; i++) { 
	    	q=s[j];
	        for (k=j-1; k>=0; k--)
	         	{q=q*(i-z)+s[k];}
	        d2=d2+q*q; 
	        c=c+y[i]*q;
	        g=g+(i-z)*q*q;
	    }

	    c=c/d2; p=g/d2; q=d2/d1;

	    d1=d2;
	    a[j]=c*s[j]; t[j]=s[j];

	    for (k=j-1; k>=0; k--) {
	    	a[k]=c*s[k]+a[k];
	        b[k]=t[k]; 
	        t[k]=s[k];
	    }
	}

	return (unsigned long)a[0];
}	
/*************************************************************************************************/
//	水深算法：平均法
/*************************************************************************************************/
unsigned int AveCalOut(unsigned int *pArry, char num)
{
	unsigned long ave1 =0;
	unsigned long beforeave =0;
	unsigned long afterave =0;
	unsigned int aveoutput =0;
	char beforenum =0;
	char afternum =0;
	char iLoop =0;
	for(iLoop= 0;iLoop<num;iLoop++)
	{
		ave1 += pArry[iLoop];		   
	}	
	ave1 = ave1/num;
	for(iLoop= 0;iLoop<num;iLoop++)
	{
		if(pArry[iLoop] >= ave1)
		{
			afterave += pArry[iLoop];
			afternum++;
		}
		else
		{
			beforeave += pArry[iLoop];
			beforenum++;
		}
	}
	if(afternum >= beforenum)
	{
		aveoutput = afterave/afternum;	
	}
	else
	{
		aveoutput = beforeave/beforenum;		
	}
	return aveoutput;
}




