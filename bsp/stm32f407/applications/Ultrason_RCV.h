#ifndef  ULTRASON_RCV_H
#define  ULTRASON_RCV_H

#include "UART_Protocol.H"
#define	UltrasonDatProCMPH	1
#define	UltrasonDatProCMPL	2
#define	Num_Forward			3					// �������ƣ�����Num_Forward����Ϊ����

//#define HF_20us
//#define LF_50us

#ifdef HF_20us
	#define HF_Freq 20  //20us 
#else
	#define HF_Freq 10  
#endif

#ifdef LF_50us
	#define LF_Freq 50  //50us
#else
	#define LF_Freq 100   
#endif


// �ߵ�Ƶ���ƿ����ָ��
typedef struct
{
	float				*p_txwidth;				// �������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
	float				*p_clearwidth;			// ����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8;0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
	POWER_CFG			*p_power;				// ���� ȡֵ1-3 Ĭ�ϣ�2
	GAIN_CFG			*p_gain;				// ���� ȡֵ1-8 Ĭ�ϣ�3
	uint8_t				*p_threshold;			// �ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
	uint8_t				*p_tvgs;				// TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
	float				*p_filter;				// ʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
	uint8_t				*p_startgain;			// ��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
	uint8_t				*p_frequency;			// Ƶ��.��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ;��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
	uint8_t				*p_shallowsurveymode;	// ǳˮ����ģʽ.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	uint8_t				*p_forcedlockingecho;	// ǿ�������ز�.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	uint8_t				*p_indentifyarithmetic;	// �ز��ж�.ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز�.Ĭ�ϣ�1	
}P_SOUNDER;
typedef struct
{
	uint8_t 			*p_Survey_Status;
	uint8_t				*p_Survey_Model;
	P_SOUNDER			*p_Sounder;
	uint8_t		 		*p_UltraDat;
	uint16_t			UltraDat_Len;
	const uint16_t 		UltraDat_Len_Buf[8];
	char 				*p_UART_Dat;
	uint16_t 			p_UART_Len;
	uint8_t 			MissReturn_Count;
	uint16_t	 		TVG_Dat_Index;
	uint16_t			*p_TVG_Dat;
	const float			ADC_Samp_Time;	
	float				Water_Deep;
}P_SYS_CB;

extern struct rt_semaphore HFreqTranCMP_sem;		// ��Ƶ�źŷ�������¼�
extern struct rt_semaphore LFreqTranCMP_sem;		// ��Ƶ�źŷ�������¼�
extern struct rt_event HL_Pro_CMP_event;			// �ߵ�Ƶ�������

extern P_SOUNDER	p_Sounder_H;
extern P_SOUNDER	p_Sounder_L;
extern P_SYS_CB 	Sys_CB_H;
extern P_SYS_CB 	Sys_CB_L;

uint8_t WaterDeep_Pro(P_SYS_CB *p_P_SYS_CB);
void UltraSonDatFill(P_SYS_CB *p_P_SYS_CB);
void UltraSonDatPro(P_SYS_CB *p_P_SYS_CB,GRADE *p_Grade);
	
extern void FiltWave_Excute(P_SYS_CB *p_P_SYS_CB,  uint8_t Channel);
extern char Seperate_Sampdata(P_SYS_CB *p_P_SYS_CB, uint8_t Channel);	

extern  void Auto_HF_Adjust(P_SYS_CB *p_P_SYS_CB);
extern  void Auto_LF_Adjust(P_SYS_CB *p_P_SYS_CB);

/****************************Airth*************************************/
extern char Deep_To_Level(int deep);

extern void Search_HFParameters(char levelnum, P_SYS_CB *p_P_SYS_CB);
extern void HF_Auto_Scope(char level);
extern void Search_HFOK_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB);
extern void Change_HF_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB, char ACK);

extern void Search_LFParameters(char levelnum, P_SYS_CB *p_P_SYS_CB);
extern void LF_Auto_Scope(char level);
extern void Search_LFOK_Parameters(char levelnum,P_SYS_CB *p_P_SYS_CB);
extern void Change_LF_Parameters(char levelnum, P_SYS_CB *p_P_SYS_CB, char ACK);

extern void Auto_change_grade(uint32_t deep); 

extern unsigned long MinMuxPir(double y[],int n);
extern unsigned int AveCalOut(unsigned int *pArry, char num);


#endif
