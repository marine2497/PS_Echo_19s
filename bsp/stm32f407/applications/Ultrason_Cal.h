/**************************LZB**********************************/
#ifndef  ULTRASON_CAL_H
#define  ULTRASON_CAL_H

//#include "Ultrason_rcv.h"
#include "stdint.h"

#define		LONGSTEADYCNT            10
#define   	SHORTSTEADYCUN            3
#define		MINMUXPIR			      
#define		LF            0
#define   	HF            1


typedef enum AutoSearch_Status_Typdef
{
	SEARCH_NONE =0,
	SEARCH_READY = 1,
	SEARCH_OK =2,
}
AutoSearch_Status;

typedef struct _FiltWave_Typedef //���˲��νṹ��
{
	uint16_t num;
	char  chrctnum;     		//������� 
	uint32_t chrct[25*5];  		//������
	uint8_t starttime;
}
FiltWave_Typedef ;

typedef struct _Cal_deep_Typedef //ˮ�����ṹ��
{	
	uint16_t tick;
	uint16_t endtick;
	uint16_t muxminpirtick;
	float    deep;
	uint16_t cltdeeptcik;
	uint16_t errortick;			//������
	uint16_t filttick_begin;
	uint16_t filttick_end;
	uint16_t basepulse;	     	 //��������µ�������
	uint32_t basestrenth;	 	 //��������µ�����������
	uint16_t baselavestrenth;	 //��������µ�����ƽ������	
}
Cal_deep_Typedef ;

typedef struct _Depth_Info_Typedef //ˮ����Ϣ�ṹ��
{
	uint16_t inputcount ;
	uint16_t outputcount;
	uint16_t inputdep[20];
	uint16_t outputdep[20];
	uint16_t outputavedep;

//	uint32_t noechocnt;
//	uint32_t echocntnum;
}
DepthInfo_Typedef;

typedef struct _Auto_Adjust_Typdef //�Զ����������ṹ��
{
	char processflag;
	char sendprocflag;
	char searchlevel;
	char searchOklevel;
	AutoSearch_Status	searchstatus;

	uint32_t mindepth;
	uint32_t maxdepth;
}
Auto_Adjust_Typedef;

typedef struct _Echo_Proc_Typedef //�ز�����ṹ��
{
	uint32_t strenthindex;
	uint32_t avestreindex;
	uint32_t wdthindex;
	
	uint32_t strenth;
	uint32_t avestrenth;
	uint32_t wdth;

	uint32_t echonum;
	
	uint32_t strentharry[20];
	uint32_t avestrearry[20];
	uint32_t wdtharry[20];
}
Echo_ProcCal_Typedef;


typedef struct      		//ˮ�����ƽṹ��
{
	char longsteadyflag;  //ˮ�ʱ�������ȶ���־λ
	char shortsteadyflag; //ˮ���ʱ���ȶ���־λ
	char longsteadycount; //��ʱ���ȶ����ڸ���
	char shortsteadycount;//��ʱʱ�ȶ����ڸ���
}
DeepTrend_Typedef;


typedef struct _judge_Parater_Typedef	  //�����ж�
{
	char   water_Steady_flag;  //ˮ���ȶ���־λ
	char   water_DecCnt;      //ˮ���ȶ�����  +
	char   water_IncCnt;      // ˮ���ȶ����� -    
	char   auto_Search_Ok;    //ˮ�������ɹ�
	char   auto_Secarch_Cnt;   	
	char   noDeepCount;
}
judge_Parater_Typedef;

extern judge_Parater_Typedef   JudgePara[2];
extern Echo_ProcCal_Typedef    Echo_ProcCal[2];

extern DepthInfo_Typedef  DepthInfo[2];
extern FiltWave_Typedef   FiltWave[2];
extern Cal_deep_Typedef   Caldeep[2];
extern Auto_Adjust_Typedef 	Autoadjust[2];
extern DeepTrend_Typedef   DeepTrend[2];

extern char my_itoa(int n,char *s);
unsigned char NMEACheckNum1(char *s);

//void Seperate_Sampdata(P_SYS_CB *p_P_SYS_CB);
//void Compare_Sampdata(char chanl);
//void PowerInfo_Compare(Echo_ProcCal_Typedef  *pechoprocal);
//void Calculate_Sampdata(char chanl);
//void FiltWave_Excute(P_SYS_CB *p_P_SYS_CB,  uint8_t chanl);
//void Combine_Sampdata(char chanl);
//uint32_t  DepthInfo_Compare(uint32_t deep,uint32_t *inputdep,uint32_t *outputdep,uint32_t *pincount,uint32_t*poutcount,char chanl);

/**************************LZB**********************************/


/****************************auto_adjust*************************************/

/**************************LZB**********************************/


#endif
