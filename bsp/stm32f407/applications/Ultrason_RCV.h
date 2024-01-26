#ifndef  ULTRASON_RCV_H
#define  ULTRASON_RCV_H

#include "UART_Protocol.H"
#define	UltrasonDatProCMPH	1
#define	UltrasonDatProCMPL	2
#define	Num_Forward			3					// 递增趋势，连续Num_Forward即认为递增

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


// 高低频控制块相关指针
typedef struct
{
	float				*p_txwidth;				// 发射脉宽 单位为毫秒 取值为 0.01-1.20 默认：0.2
	float				*p_clearwidth;			// 抑制水深 单位为米 取值为0.2-20 默认：0.8;0.2-3.0 步进0.1   3.0-10.0 步进1  10-20 步进2
	POWER_CFG			*p_power;				// 功率 取值1-3 默认：2
	GAIN_CFG			*p_gain;				// 增益 取值1-8 默认：3
	uint8_t				*p_threshold;			// 回波阈值 取值1-5 默认：3
	uint8_t				*p_tvgs;				// TVG曲线 取值0-30 单位为logR 步进5 默认：20
	float				*p_filter;				// 时间门参数 取值0%-50%  0.0表示关闭 默认：0.1
	uint8_t				*p_startgain;			// 起始增益 取值为0-60 单位为db步进为5 默认：10
	uint8_t				*p_frequency;			// 频率.高频：100KHZ-1000KHZ 默认：200KHZ;低频：10KHZ-100KHZ   默认：20KHZ
	uint8_t				*p_shallowsurveymode;	// 浅水测量模式.取值0或1  0：关闭  1：开启  默认：0
	uint8_t				*p_forcedlockingecho;	// 强制锁定回波.取值0或1  0：关闭  1：开启  默认：0
	uint8_t				*p_indentifyarithmetic;	// 回波判断.取值1、2、3  1：最大能量回波  2：参考高频回波 3：底层回波.默认：1	
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

extern struct rt_semaphore HFreqTranCMP_sem;		// 高频信号发送完毕事件
extern struct rt_semaphore LFreqTranCMP_sem;		// 低频信号发送完毕事件
extern struct rt_event HL_Pro_CMP_event;			// 高低频处理完毕

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
