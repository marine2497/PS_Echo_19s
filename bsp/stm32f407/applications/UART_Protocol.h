#ifndef  UART_PROTOCOL_H
#define  UART_PROTOCOL_H
#include <rtthread.h>
#include "board.h"
/*  ��������4�ֽڶ��� */
ALIGN(RT_ALIGN_SIZE)
//#define	GET						0
//#define	SET						1
//#define	ERR						2
#define	Byte_Type				0
#define	Int_Type				1
#define	Float_Type				2
#define	Str_Type				3

#define	Channel_One				1
#define	Channel_Two				2

#define	str_len					32
#define	Device_Info_Addr		((DEVICE_INFO *) 0x08007800)

//#define	boot_ver_str			"20175625"			// BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
//#define sn_str					"S23001111111111"	// 15λ����ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define pn_str					"S23001111111111"	// Ӳ���汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define pid_str					"729048374639"		// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
//#define hid_str					"837364545"			// ϵͳ��Ϣ
//#define brand_str				"SOUTH"				// ��ƷƷ�ƣ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define model_str				"SDE230DF"			// ����,�ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define production_date_str		"20180109"			// ��������, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define app_ver_str				"2017.12.26"		// �̼��汾��, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//#define gnssboard_str			"256145"			// �������ͣ��ַ����ԡ�\0����β�����Ȳ�����32��������\0���

// 1.1.1   INFO	�ڵ�(�ýڵ�֧��GET/SET��ȡҶ����Ϣ)
//typedef struct
//{
//	char boot_ver[sizeof(boot_ver_str)];				// BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
//	char sn[sizeof(sn_str)];							// 15λ����ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char pn[sizeof(pn_str)];							// Ӳ���汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char pid[sizeof(pid_str)];							// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
//	char hid[sizeof(hid_str)];							// ϵͳ��Ϣ
//	char brand[sizeof(brand_str)];						// ��ƷƷ�ƣ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char model[sizeof(model_str)];						// ����,�ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char production_date[sizeof(production_date_str)];	// ��������, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char app_ver[sizeof(app_ver_str)];					// �̼��汾��, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	char gnssboard[sizeof(gnssboard_str)];				// �������ͣ��ַ����ԡ�\0����β�����Ȳ�����32��������\0��
//}DEVICE_INFO;//
typedef struct
{
	const char boot_ver[str_len];						// BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
	const char sn[str_len];							// 15λ����ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
	const char pn[str_len];							// Ӳ���汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//	const char pid[str_len];							// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
	const char hid[str_len];							// ϵͳ��Ϣ
	const char brand[str_len];						// ��ƷƷ�ƣ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
	const char model[str_len];						// ����,�ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
	const char production_date[str_len];				// ��������, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
	const char app_ver[str_len];						// �̼��汾��, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
	const char gnssboard[str_len];					// �������ͣ��ַ����ԡ�\0����β�����Ȳ�����32��������\0��	
	char pid[str_len];								// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
}DEVICE_INFO;
extern	char Pid[str_len];	
extern	char App_ver[str_len];	

// 1.1.2 TIME.�˽ڵ�����Ϊ�����ǵ�ע��ʱ���ʹ��ʱ�䣬����ѯ������֧��GET����
typedef struct
{
	char str_reg_time[8];
	uint32_t reg_time;								// ע��ʱ��
	uint32_t used_time;								// ����ʱ���¼��ת����used_time�ַ���
	uint32_t p_Cur_Addr;							// ��¼д��Flash�ĵ�ַ
	uint32_t Cur_Counter_10Min;						// ��ǰ��¼��10���Ӹ���
}TIME;
// 1.1.3 VOLTAGE����.�˽ڵ����Է��ص�ǰ��ص�ѹ����ӵ�Դ����ʵ��ѹ����VΪ��λ����֧��GET����
typedef struct
{
	float voltage;								// �˽ڵ����Է��ص�ǰ��ص�ѹ����ӵ�Դ����ʵ��ѹ����VΪ��λ
}VOLTAGE;
// 1.1.4 REGI����.�˽ڵ���������ע������ǣ���֧��SET����
typedef struct
{
	char regi[40];									// �˽ڵ���������ע������ǣ���֧��SET����
	char srt_regi[40];
}REGI;
// 1.1.5SURVEY_STATUS,�˽ڵ�����Ϊ��������͹رղ������1��ʾ������0��ʾ�رգ���֧��SET��GET���֧��ͬʱ�������е�ͨ����
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_STATUS;
// 1.1.7 SURVEY_MODEL,�˽ڵ�����Ϊ���ø�ͨ���Ĳ���ģʽΪ�ֶ�����ģʽ���Զ�����ģʽ��0��ʾ�ֶ���1��ʾ�Զ���֧��SET���GET����,֧��ͬʱ�������е�ͨ����
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_MODEL;
//1.1.8 DOGNUM.�˽ڵ���������������������ʱ�Ľ�������֧��SET����,����ʱ���ȷ���PENDING�����ʾ�ѽ��գ����ܺ���GETӦ��ĸ�ʽ����
typedef struct
{
	char dognum[16];									
}DOGNUM;
//1.2 Parameters.�˽ڵ�����Ϊ�������ã������и�Ψһ��ͨ����Ҷ�ӽڵ㣬ֻ�����Ҷ�ӽڵ�������һ����䡣
//1.2 Parameters.�˽ڵ�����Ϊ�������ã������и�Ψһ��ͨ����Ҷ�ӽڵ㣬ֻ�����Ҷ�ӽڵ�������һ����䡣
typedef struct
{
	float draft;									// ��ˮ ��λΪ�� ȡֵΪ0.00-9.99 Ĭ�ϣ�0.50
	float velocity;								   // ���� ��λ����/�� ȡֵΪ1300-1700 Ĭ�ϣ�1483
	float minadepth;								// ǳˮ����ˮ�� ��λΪ�� ȡֵΪ0.50-9.99 Ĭ�ϣ�1
	float maxadepth;								// ��ˮ����ˮ�� ��λΪ�� ȡֵΪ1.00-600.00 Ĭ�ϣ�100
	uint8_t alarmstatus;							// �޻ز�������ʶ 0Ϊ�أ�1Ϊ�� Ĭ�ϣ�0���رգ�
	uint8_t alarmchannel;							// �ο�����ͨ���� ȡֵ1-16 Ĭ�ϣ�1
	uint8_t missreturn;								// ��ʾ���������� ȡֵ1-10 Ĭ�ϣ�5									
}PARAMETERS;
// 1.2.2Calibration�ȶ԰�����У���������˽ڵ�����Ϊ�ȶ԰����ټ�����������֧��SET����
typedef struct
{
	uint16_t DEPTH;									// �ȶ�ˮ��
	uint8_t STATUS;									// ��ʼ�ͽ����ȶ�״̬��1��ʾ������0��ʾ����
}CALIBRATION;
//Sounder�����ǲ���.�˽ڵ�����Ϊÿ��ͨ���Ĳ����ǲ�������֧��GET,SET������Զ��ϴ����ϴ�ʱ��ʽͬGETӦ���ʽ��
typedef struct
{
	uint8_t Power_Index;							// ���� ȡֵ1-3 Ĭ�ϣ�2
	const uint16_t Power_ADC_Value[8];				// ���ʶ�Ӧ��ADCֵ
}POWER_CFG;
typedef struct
{
	uint8_t Gain_Index;								// ���� ȡֵ1-8 Ĭ�ϣ�3
	uint16_t Gain_ADC_Value[8]; 					// ���� ��Ӧ��ADCֵ
}GAIN_CFG;
//////////////////////////////////////SOUNDER_cfg;
typedef struct	// �������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
{
	float	channel_one;
	float	channel_two;
}TXWIDTH;
typedef struct
{
	float 	channel_one;							// ����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8;0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
	float 	channel_two;							// ����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8;0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
}CLEARWIDTH;
typedef struct
{
	POWER_CFG	channel_one;						// ���� ȡֵ1-3 Ĭ�ϣ�2
	POWER_CFG	channel_two;						// ���� ȡֵ1-3 Ĭ�ϣ�2
}POWER;
typedef struct
{
	GAIN_CFG 	channel_one;						// ���� ȡֵ1-8 Ĭ�ϣ�3
	GAIN_CFG 	channel_two;						// ���� ȡֵ1-8 Ĭ�ϣ�3
}GAIN;
typedef struct
{
	uint8_t channel_one;							// �ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
	uint8_t channel_two;							// �ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
}THRESHOLD;
typedef struct
{
	uint8_t channel_one;							// TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
	uint8_t channel_two;							// TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
}TVGS;
typedef struct
{
	float 	channel_one;							// ʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
	float 	channel_two;							// ʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
}FILTER;
typedef struct
{
	uint8_t channel_one;							// ��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
	uint8_t channel_two;							// ��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
}STARTGAIN;
typedef struct
{
	uint8_t channel_one;							// Ƶ��.��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ;��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
	uint8_t channel_two;							// Ƶ��.��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ;��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
}FREQUENCY;
typedef struct
{
	uint8_t channel_one;							// ǳˮ����ģʽ.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	uint8_t channel_two;							// ǳˮ����ģʽ.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
}SHALLOWSURVEYMODE;
typedef struct
{
	uint8_t channel_one;							// ǿ�������ز�.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	uint8_t channel_two;							// ǿ�������ز�.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
}FORCEDLOCKINGECHO;

/*upper limit  lower limit*/
//typedef struct
//{
//	uint16_t channel_one_upperlimit;							// ǿ�������ز�.����  Ĭ�ϣ�0
//	uint16_t channel_one_lowerlimit;							// ǿ�������ز�.����  Ĭ�ϣ�0
//	
//	uint16_t channel_two_upperlimit;							// ǿ�������ز�.����  Ĭ�ϣ�0
//	uint16_t channel_two_lowerlimit;							// ǿ�������ز�.����  Ĭ�ϣ�0	
//}FORCEDLOCKINGECHOVALNU;

typedef struct
{
	uint8_t channel_one;						// �ز��ж�.ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز�.Ĭ�ϣ�1
	uint8_t channel_two;						// �ز��ж�.ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز�.Ĭ�ϣ�1
}INDENTIFYARITHMETIC;
typedef struct
{
	TXWIDTH 			txwidth;								// �������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
	CLEARWIDTH 			clearwidth;								// ����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8;0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
	POWER				power;									// ���� ȡֵ1-3 Ĭ�ϣ�2
	GAIN 				gain;									// ���� ȡֵ1-8 Ĭ�ϣ�3
	THRESHOLD			threshold;								// �ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
	TVGS				tvgs;									// TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
	FILTER	 			filter;									// ʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
	STARTGAIN			startgain;								// ��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
	FREQUENCY			frequency;								// Ƶ��.��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ;��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
	SHALLOWSURVEYMODE	shallowsurveymode;						// ǳˮ����ģʽ.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
	FORCEDLOCKINGECHO	forcedlockingecho;						// ǿ�������ز�.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
//    FORCEDLOCKINGECHOVALNU forcedlockingechovalue;				// ǿ�������ز�.����������
	INDENTIFYARITHMETIC	indentifyarithmetic;					// �ز��ж�.ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز�.Ĭ�ϣ�1
}SOUNDER;
// 1.3 grade ������λ.�˽ڵ�Ϊ��λֵ��֧��SET������Զ��ϴ����ϴ�ʱ��ʽͬGETӦ���ʽ.��λȡֵ0-7Ĭ��15�׵�
typedef struct
{
	uint8_t grade;
	uint8_t auto_grade;
}GRADE;
// ϵͳ��Ϣ�����ÿ��ƿ�
typedef struct
{
	DEVICE_INFO		*Device_Info;
	TIME 			Time;
	VOLTAGE 		Voltage;
	DOGNUM 			Dogunm;	
	PARAMETERS 		Parameters;
	REGI 			Regi;
	SURVEY_STATUS 	Survey_Status;
	SURVEY_MODEL	Survey_Model;
	CALIBRATION 	Calibration;
	SOUNDER			Sounder;
	GRADE			Grade;
}SYS_CB;
// ϵͳ��Ϣ�����ÿ��ƿ�ؼ���������
typedef struct
{
	PARAMETERS 		Parameters;
	REGI 			Regi;
	SURVEY_STATUS 	Survey_Status;
	SURVEY_MODEL	Survey_Model;
	CALIBRATION 	Calibration;
	SOUNDER			Sounder;
	GRADE			Grade;
}SYS_CB_COPY;


typedef struct
{
	uint8_t Transform_One;
	uint8_t Transform_Two;
//    uint8_t Transform_Three;
//    uint8_t Transform_Four;
}TRANSFORM;
// ϵͳ���ƿ�
extern SYS_CB 		Sys_CB;									
extern SYS_CB_COPY	Sys_CB_Copy;
extern uint8_t CMD_Refresh;
extern TRANSFORM       Transform;
/*************************************************************************************************/
//	�ַ�������������ʵ�ʿ�������
/*************************************************************************************************/
char do_crc(char *src,uint32_t len);				
uint16_t my_strncpy(char *dst, const char *src, uint16_t n);
uint8_t Float_to_Str(char *p,float dat);
uint8_t Str_to_float(char *p,float *dat);
uint8_t Long_to_Str(char *p,uint32_t dat);
uint8_t Str_to_Long(char *p,uint32_t *dat);
uint8_t Str_to_Long_num(char *p,uint32_t *dat,char num);

void getCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void Tran_setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void Jump_setCMD_Process(char *p);
void CMD_Process(void);
void Tran_setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);

#endif
