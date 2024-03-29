#ifndef  UART_PROTOCOL_H
#define  UART_PROTOCOL_H
#include <rtthread.h>
#include "board.h"
/*  变量分配4字节对齐 */
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

//#define	boot_ver_str			"20175625"			// BOOT版本号，字符串以’\0’结尾，长度不大于32（包含’\0’）
//#define sn_str					"S23001111111111"	// 15位机身号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define pn_str					"S23001111111111"	// 硬件版本号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define pid_str					"729048374639"		// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
//#define hid_str					"837364545"			// 系统信息
//#define brand_str				"SOUTH"				// 产品品牌，字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define model_str				"SDE230DF"			// 机型,字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define production_date_str		"20180109"			// 生产日期, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define app_ver_str				"2017.12.26"		// 固件版本号, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//#define gnssboard_str			"256145"			// 主板类型，字符串以’\0’结尾，长度不大于32，包含’\0’�

// 1.1.1   INFO	节点(该节点支持GET/SET获取叶子信息)
//typedef struct
//{
//	char boot_ver[sizeof(boot_ver_str)];				// BOOT版本号，字符串以’\0’结尾，长度不大于32（包含’\0’）
//	char sn[sizeof(sn_str)];							// 15位机身号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char pn[sizeof(pn_str)];							// 硬件版本号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char pid[sizeof(pid_str)];							// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
//	char hid[sizeof(hid_str)];							// 系统信息
//	char brand[sizeof(brand_str)];						// 产品品牌，字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char model[sizeof(model_str)];						// 机型,字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char production_date[sizeof(production_date_str)];	// 生产日期, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char app_ver[sizeof(app_ver_str)];					// 固件版本号, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//	char gnssboard[sizeof(gnssboard_str)];				// 主板类型，字符串以’\0’结尾，长度不大于32，包含’\0’
//}DEVICE_INFO;//
typedef struct
{
	const char boot_ver[str_len];						// BOOT版本号，字符串以’\0’结尾，长度不大于32（包含’\0’）
	const char sn[str_len];							// 15位机身号，字符串以’\0’结尾，长度不大于32(包含’\0’)
	const char pn[str_len];							// 硬件版本号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//	const char pid[str_len];							// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
	const char hid[str_len];							// 系统信息
	const char brand[str_len];						// 产品品牌，字符串以’\0’结尾，长度不大于32(包含’\0’)
	const char model[str_len];						// 机型,字符串以’\0’结尾，长度不大于32(包含’\0’)
	const char production_date[str_len];				// 生产日期, 字符串以’\0’结尾，长度不大于32(包含’\0’)
	const char app_ver[str_len];						// 固件版本号, 字符串以’\0’结尾，长度不大于32(包含’\0’)
	const char gnssboard[str_len];					// 主板类型，字符串以’\0’结尾，长度不大于32，包含’\0’	
	char pid[str_len];								// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
}DEVICE_INFO;
extern	char Pid[str_len];	
extern	char App_ver[str_len];	

// 1.1.2 TIME.此节点属性为测深仪的注册时间和使用时间，仅查询，即仅支持GET命令
typedef struct
{
	char str_reg_time[8];
	uint32_t reg_time;								// 注册时间
	uint32_t used_time;								// 工作时间记录，转化成used_time字符串
	uint32_t p_Cur_Addr;							// 记录写入Flash的地址
	uint32_t Cur_Counter_10Min;						// 当前记录的10分钟个数
}TIME;
// 1.1.3 VOLTAGE属性.此节点属性返回当前电池电压或外接电源的真实电压，以V为单位。仅支持GET命令
typedef struct
{
	float voltage;								// 此节点属性返回当前电池电压或外接电源的真实电压，以V为单位
}VOLTAGE;
// 1.1.4 REGI属性.此节点属性用于注册测深仪，仅支持SET命令
typedef struct
{
	char regi[40];									// 此节点属性用于注册测深仪，仅支持SET命令
	char srt_regi[40];
}REGI;
// 1.1.5SURVEY_STATUS,此节点属性为启动测深和关闭测深命令，1表示开启，0表示关闭，可支持SET和GET命令，支持同时设置所有的通道。
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_STATUS;
// 1.1.7 SURVEY_MODEL,此节点属性为设置各通道的测量模式为手动测量模式或自动测量模式，0表示手动，1表示自动，支持SET命令及GET命令,支持同时设置所有的通道。
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_MODEL;
//1.1.8 DOGNUM.此节点属性用于与自由行握手时的交互，仅支持SET命令,握手时可先发送PENDING命令，表示已接收，加密后以GET应答的格式返回
typedef struct
{
	char dognum[16];									
}DOGNUM;
//1.2 Parameters.此节点属性为参数设置，都会有个唯一的通道号叶子节点，只有这个叶子节点才能组成一条语句。
//1.2 Parameters.此节点属性为参数设置，都会有个唯一的通道号叶子节点，只有这个叶子节点才能组成一条语句。
typedef struct
{
	float draft;									// 吃水 单位为米 取值为0.00-9.99 默认：0.50
	float velocity;								   // 声速 单位是米/秒 取值为1300-1700 默认：1483
	float minadepth;								// 浅水报警水深 单位为米 取值为0.50-9.99 默认：1
	float maxadepth;								// 深水报警水深 单位为米 取值为1.00-600.00 默认：100
	uint8_t alarmstatus;							// 无回波报警标识 0为关，1为开 默认：0（关闭）
	uint8_t alarmchannel;							// 参考报警通道号 取值1-16 默认：1
	uint8_t missreturn;								// 表示允许丢波个数 取值1-10 默认：5									
}PARAMETERS;
// 1.2.2Calibration比对板声速校正参数。此节点属性为比对板声速计算相关命令，仅支持SET命令
typedef struct
{
	uint16_t DEPTH;									// 比对水深
	uint8_t STATUS;									// 开始和结束比对状态，1表示开启，0表示结束
}CALIBRATION;
//Sounder测深仪参数.此节点属性为每个通道的测深仪参数，可支持GET,SET命令，并自动上传，上传时格式同GET应答格式。
typedef struct
{
	uint8_t Power_Index;							// 功率 取值1-3 默认：2
	const uint16_t Power_ADC_Value[8];				// 功率对应的ADC值
}POWER_CFG;
typedef struct
{
	uint8_t Gain_Index;								// 增益 取值1-8 默认：3
	uint16_t Gain_ADC_Value[8]; 					// 增益 对应的ADC值
}GAIN_CFG;
//////////////////////////////////////SOUNDER_cfg;
typedef struct	// 发射脉宽 单位为毫秒 取值为 0.01-1.20 默认：0.2
{
	float	channel_one;
	float	channel_two;
}TXWIDTH;
typedef struct
{
	float 	channel_one;							// 抑制水深 单位为米 取值为0.2-20 默认：0.8;0.2-3.0 步进0.1   3.0-10.0 步进1  10-20 步进2
	float 	channel_two;							// 抑制水深 单位为米 取值为0.2-20 默认：0.8;0.2-3.0 步进0.1   3.0-10.0 步进1  10-20 步进2
}CLEARWIDTH;
typedef struct
{
	POWER_CFG	channel_one;						// 功率 取值1-3 默认：2
	POWER_CFG	channel_two;						// 功率 取值1-3 默认：2
}POWER;
typedef struct
{
	GAIN_CFG 	channel_one;						// 增益 取值1-8 默认：3
	GAIN_CFG 	channel_two;						// 增益 取值1-8 默认：3
}GAIN;
typedef struct
{
	uint8_t channel_one;							// 回波阈值 取值1-5 默认：3
	uint8_t channel_two;							// 回波阈值 取值1-5 默认：3
}THRESHOLD;
typedef struct
{
	uint8_t channel_one;							// TVG曲线 取值0-30 单位为logR 步进5 默认：20
	uint8_t channel_two;							// TVG曲线 取值0-30 单位为logR 步进5 默认：20
}TVGS;
typedef struct
{
	float 	channel_one;							// 时间门参数 取值0%-50%  0.0表示关闭 默认：0.1
	float 	channel_two;							// 时间门参数 取值0%-50%  0.0表示关闭 默认：0.1
}FILTER;
typedef struct
{
	uint8_t channel_one;							// 起始增益 取值为0-60 单位为db步进为5 默认：10
	uint8_t channel_two;							// 起始增益 取值为0-60 单位为db步进为5 默认：10
}STARTGAIN;
typedef struct
{
	uint8_t channel_one;							// 频率.高频：100KHZ-1000KHZ 默认：200KHZ;低频：10KHZ-100KHZ   默认：20KHZ
	uint8_t channel_two;							// 频率.高频：100KHZ-1000KHZ 默认：200KHZ;低频：10KHZ-100KHZ   默认：20KHZ
}FREQUENCY;
typedef struct
{
	uint8_t channel_one;							// 浅水测量模式.取值0或1  0：关闭  1：开启  默认：0
	uint8_t channel_two;							// 浅水测量模式.取值0或1  0：关闭  1：开启  默认：0
}SHALLOWSURVEYMODE;
typedef struct
{
	uint8_t channel_one;							// 强制锁定回波.取值0或1  0：关闭  1：开启  默认：0
	uint8_t channel_two;							// 强制锁定回波.取值0或1  0：关闭  1：开启  默认：0
}FORCEDLOCKINGECHO;

/*upper limit  lower limit*/
//typedef struct
//{
//	uint16_t channel_one_upperlimit;							// 强制锁定回波.上限  默认：0
//	uint16_t channel_one_lowerlimit;							// 强制锁定回波.下限  默认：0
//	
//	uint16_t channel_two_upperlimit;							// 强制锁定回波.上限  默认：0
//	uint16_t channel_two_lowerlimit;							// 强制锁定回波.下限  默认：0	
//}FORCEDLOCKINGECHOVALNU;

typedef struct
{
	uint8_t channel_one;						// 回波判断.取值1、2、3  1：最大能量回波  2：参考高频回波 3：底层回波.默认：1
	uint8_t channel_two;						// 回波判断.取值1、2、3  1：最大能量回波  2：参考高频回波 3：底层回波.默认：1
}INDENTIFYARITHMETIC;
typedef struct
{
	TXWIDTH 			txwidth;								// 发射脉宽 单位为毫秒 取值为 0.01-1.20 默认：0.2
	CLEARWIDTH 			clearwidth;								// 抑制水深 单位为米 取值为0.2-20 默认：0.8;0.2-3.0 步进0.1   3.0-10.0 步进1  10-20 步进2
	POWER				power;									// 功率 取值1-3 默认：2
	GAIN 				gain;									// 增益 取值1-8 默认：3
	THRESHOLD			threshold;								// 回波阈值 取值1-5 默认：3
	TVGS				tvgs;									// TVG曲线 取值0-30 单位为logR 步进5 默认：20
	FILTER	 			filter;									// 时间门参数 取值0%-50%  0.0表示关闭 默认：0.1
	STARTGAIN			startgain;								// 起始增益 取值为0-60 单位为db步进为5 默认：10
	FREQUENCY			frequency;								// 频率.高频：100KHZ-1000KHZ 默认：200KHZ;低频：10KHZ-100KHZ   默认：20KHZ
	SHALLOWSURVEYMODE	shallowsurveymode;						// 浅水测量模式.取值0或1  0：关闭  1：开启  默认：0
	FORCEDLOCKINGECHO	forcedlockingecho;						// 强制锁定回波.取值0或1  0：关闭  1：开启  默认：0
//    FORCEDLOCKINGECHOVALNU forcedlockingechovalue;				// 强制锁定回波.上限与下限
	INDENTIFYARITHMETIC	indentifyarithmetic;					// 回波判断.取值1、2、3  1：最大能量回波  2：参考高频回波 3：底层回波.默认：1
}SOUNDER;
// 1.3 grade 测量档位.此节点为档位值，支持SET命令，及自动上传，上传时格式同GET应答格式.档位取值0-7默认15米档
typedef struct
{
	uint8_t grade;
	uint8_t auto_grade;
}GRADE;
// 系统信息及配置控制块
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
// 系统信息及配置控制块关键变量拷贝
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
// 系统控制块
extern SYS_CB 		Sys_CB;									
extern SYS_CB_COPY	Sys_CB_Copy;
extern uint8_t CMD_Refresh;
extern TRANSFORM       Transform;
/*************************************************************************************************/
//	字符串拷贝，返回实际拷贝个数
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
