/****************************************Copyright (c)**************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: UART_Protocol.C
**创   建   人: 
**最后修改日期: 2018年9月20日
**描        述: 参数初始化+命令处理
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.00
** 日　期: 2017年12月06日
** 描　述: 原始版本
**
********************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "drv_usart.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.h"
#include "UART_Parameters.H"
#include "UART_DEVICE.H"
#include "UART_grade.H"
#include "Regtime.h"

 TRANSFORM       Transform = {0, 5};

uint8_t CMD_Refresh = 0;

//const DEVICE_INFO Device_Info  __attribute__((at(0X08007800)));
//const DEVICE_INFO Device_Info  __attribute__((at(0X08007800))) = 		// 定位在第二扇区后2K
//					{		
//						boot_ver_str,			// BOOT版本号，字符串以’\0’结尾，长度不大于32（包含’\0’）
//						sn_str,					// 15位机身号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						pn_str,					// 硬件版本号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						pid_str,				// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
//						hid_str,				// 系统信息
//						brand_str,				// 产品品牌，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						model_str,				// 机型,字符串以’\0’结尾，长度不大于32(包含’\0’)
//						production_date_str,	// 生产日期, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//						app_ver_str,			// 固件版本号, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//						gnssboard_str,			// 主板类型，字符串以’\0’结尾，长度不大于32，包含’\0’?
//					};

// 系统控制块
SYS_CB Sys_CB = {
//					// 1.1.1   INFO	节点(该节点支持GET/SET获取叶子信息)
//					// DEVICE_INFO		Device_Info;
//					{		
//						boot_ver_str,			// BOOT版本号，字符串以’\0’结尾，长度不大于32（包含’\0’）
//						sn_str,					// 15位机身号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						pn_str,					// 硬件版本号，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						pid_str,				// 产品附加码，字符串以’\0’结尾，长度不大于128(包含’\0’)
//						hid_str,				// 系统信息
//						brand_str,				// 产品品牌，字符串以’\0’结尾，长度不大于32(包含’\0’)
//						model_str,				// 机型,字符串以’\0’结尾，长度不大于32(包含’\0’)
//						production_date_str,	// 生产日期, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//						app_ver_str,			// 固件版本号, 字符串以’\0’结尾，长度不大于32(包含’\0’)
//						gnssboard_str,			// 主板类型，字符串以’\0’结尾，长度不大于32，包含’\0’?
//					},
					Device_Info_Addr,
					// 1.1.2 TIME.此节点属性为测深仪的注册时间和使用时间，仅查询，即仅支持GET命令
					// TIME 			Time;
					{
						{0},
						200,					// 默认注册时间
						0,						// 使用时间
						Flash_Sector2_Addr + 4,
						0
					},
					// 1.1.3 VOLTAGE属性.此节点属性返回当前电池电压或外接电源的真实电压，以V为单位。仅支持GET命令
					// VOLTAGE 		Voltage;
					{
						12.5,
					},
					//1.1.8 DOGNUM.此节点属性用于与自由行握手时的交互，仅支持SET命令,握手时可先发送PENDING命令，表示已接收，加密后以GET应答的格式返回
					// DOGNUM 			Dogunm;
					{"0123456789"},
				
					//1.2 Parameters.此节点属性为参数设置，都会有个唯一的通道号叶子节点，只有这个叶子节点才能组成一条语句。
					// PARAMETERS 		Parameters;
					{
						0.5,					// 吃水 单位为米 取值为0.00-9.99 默认：0.50
						1480,					// 声速 单位是米/秒 取值为1300-1700 默认：1483
						0,						// 浅水报警水深 单位为米 取值为0.50-9.99 默认：1
						300,					// 深水报警水深 单位为米 取值为1.00-600.00 默认：100
						0,						// 无回波报警标识 0为关，1为开 默认：0（关闭）
						1,						// 参考报警通道号 取值1-16 默认：1
						5						// 表示允许丢波个数 取值1-10 默认：5									
					},
					//  1.1.4 REGI属性.此节点属性用于注册测深仪，仅支持SET命令
					// REGI Regi
					{"S23001111111111729048374639234567890",
					 "S23001111111111729048374639234567891",
					},
					// 1.1.5SURVEY_STATUS,此节点属性为启动测深和关闭测深命令，1表示开启，0表示关闭，可支持SET和GET命令，支持同时设置所有的通道。
					// SURVEY_STATUS
					{
						0,					// CHANNEL ONE
						1
					},						// CHANNEL TWO
					// 1.1.7 SURVEY_MODEL,此节点属性为设置各通道的测量模式为手动测量模式或自动测量模式，0表示手动，1表示自动，支持SET命令及GET命令,支持同时设置所有的通道。
					// SURVEY_MODEL
					{
						0,					// CHANNEL ONE
						1					// CHANNEL TWO
					},						
					// 1.2.2Calibration比对板声速校正参数。此节点属性为比对板声速计算相关命令，仅支持SET命令
					// CALIBRATION 	Calibration;
					{5,0},
					//Sounder测深仪参数.此节点属性为每个通道的测深仪参数，可支持GET,SET命令，并自动上传，上传时格式同GET应答格式。
					// SOUNDER			
					{
						{
							0.2,					// 发射脉宽 单位为毫秒 取值为 0.01-1.20 默认：0.2
							0.2
						},
						{
							0.4,					// 抑制水深 单位为米 取值为0.2-20 默认：0.8;0.2-3.0 步进0.1   3.0-10.0 步进1  10-20 步进2
							0.4
						},
						{
							{
								1,					// 功率 取值1-3 默认：2
								{2880,2400,1600,480,320,240,160,80}//{180,150,100,30,20,15,10,5}		// 功率对应的ADC值
							},
							{
								1,					// 功率 取值1-3 默认：2
								{2880,2400,1600,480,320,240,160,80}//{200,150,100,30,20,15,10,5}		// 功率对应的ADC值
							},
						},
						{
							{
								3,					// 增益 取值1-8 默认：3
								{800,1250,1700,2150,
								2600,3050,3500,4000}// 增益 对应的ADC值
							},
							{
								3,					// 增益 取值1-8 默认：3
								{800,1250,1700,2150,
								2600,3050,3500,4000}// 增益 对应的ADC值
							},
						},
						{
							1,						// 回波阈值 取值1-5 默认：3
							2
						},
						{
							20,						// TVG曲线 取值0-30 单位为logR 步进5 默认：20
							20
						},
						{
							0,					// 时间门参数 取值0%-50%  0.0表示关闭 默认：0.1
							0
						},
						{
							30,						// 起始增益 取值为0-60 单位为db步进为5 默认：10
							30
						},
						{
							20,						// 频率.低频：100KHZ-1000KHZ 默认：200KHZ;低频：10KHZ-100KHZ   默认：20KHZ
							200
						},
						{
							0,						// 浅水测量模式.取值0或1  0：关闭  1：开启  默认：0
							0
						},
						{
							0,						// 强制锁定回波.取值0或1  0：关闭  1：开启  默认：0
							0
						},
//						{
//							0,					// 强制锁定回波范围
//							0,
//							0,
//							0
//						},						
						{
							1,						// 回波判断.取值1、2、3  1：最大能量回波  2：参考高频回波 3：底层回波.默认：1
							1
						}
					},
					{	
						5,						// grade 测量档位,档位取值0-7默认15米档
						0						// 自动测量时确定的档位
					}
				};	
SYS_CB_COPY	Sys_CB_Copy;
char Pid[str_len] = {0};
char App_ver[str_len] = {0};




char do_crc(char *src,uint32_t len)
{
	char  check_sum=0;
	uint32_t i=0;
	for(i=0;i<len;i++)
	{
		check_sum=check_sum^(uint8_t)(*(src+i));
	}
	return check_sum;
}
/*************************************************************************************************/
//	字符串拷贝，返回实际拷贝个数
/*************************************************************************************************/				
uint16_t my_strncpy(char *dst, const char *src, uint16_t n)
{
	uint16_t i = 0;
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;
        do
        {
            if ((*d++ = *s++) == 0)
            {
                break;
            }
			i++;
        } while (--n != 0);
    }
    return (i);
}
uint8_t Float_to_Str(char *p,float dat)
{
	uint16_t tmp_int;
	uint16_t tmp_point;
	char tmp_char;
	uint8_t i = 0;
	
	tmp_int = (uint16_t)dat;
	dat = (dat - tmp_int)*100;
	tmp_point = (uint16_t)dat;
	
	tmp_char = tmp_int/1000;						
	if(tmp_char||i)
	{
		*(p+i++) = tmp_char + '0';
	}
	tmp_char = (tmp_int%1000)/100;
	if(tmp_char||i)
	{
		*(p+i++) = tmp_char + '0';
	}
	tmp_char = (tmp_int%100)/10;
	if(tmp_char||i)
	{
		*(p+i++) = tmp_char + '0';
	}
	tmp_char = (tmp_int%10);
	*(p+i++) = tmp_char + '0';
	*(p+i++) = '.';
	tmp_char = (tmp_point/10);
	*(p+i++) = tmp_char + '0';
	tmp_char = (tmp_point%10);
	*(p+i++) = tmp_char + '0';
	return i;
}
uint8_t Str_to_float(char *p,float *dat)
{
	uint8_t i = 0,Point_Flag = 0;
	double	tmp = 0.1;
	float tmp_float = 0;
		
	while(((*(p+i)<='9')&&(*(p+i)>='0'))||(*(p+i)=='.'))
	{
		if(*(p+i)=='.')
		{
			Point_Flag = 1;
		}
		else
		{
			if(Point_Flag)
			{
				tmp_float = tmp_float + ( *(p+i) - '0')*tmp;
				tmp *= 0.1;
			}
			else
			{
				tmp_float = tmp_float*10 + ( *(p+i) - '0');
			}
		}
		i++;
	}
	*dat = tmp_float;
	if(*(p+i)=='\0')
	{
		if(*(p+i+1)=='|')
			i = i + 2;
	}
	i = i + 1;
	return i;
}
uint8_t Long_to_Str(char *p,uint32_t dat)
{
	uint8_t i = 0,j;
	char Buf[15];
	while(dat)
	{
		Buf[i++] = dat%10 + '0';
		dat = dat/10;
	}
	if(i)			// dat 为非零值
	{
		for(j=0;j<i;j++)
		{
			*(p+j) = Buf[i-j-1];
		}
	}
	else
	{
		*p = '0';
		i = 1;
	}
	return i;
}
uint8_t Str_to_Long(char *p,uint32_t *dat)
{
	uint8_t i = 0;
	uint32_t tmp_int = 0;

	while((*(p+i)<='9')&&(*(p+i)>='0'))
	{
		tmp_int = tmp_int*10 + (*(p+i)- '0');
		i++;
	}
	*dat = tmp_int;
	if(*(p+i)=='\0')
	{
		if(*(p+i+1)=='|')
			i = i + 2;
	}
	i = i + 1;
	return i;
}
uint8_t Str_to_Long_num(char *p,uint32_t *dat,char num)
{
	uint8_t i = 0;
	uint32_t tmp_int = 0;


//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)"[", 1);		
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)p, 8);	
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)"]", 1);	
	
	while((*(p+i)<='9')&&(*(p+i)>='0') && i<num)
	{
		tmp_int = tmp_int*10 + (*(p+i)- '0');
		i++;
	}
	*dat = tmp_int;
	if(*(p+i)=='\0')
	{
		if(*(p+i+1)=='|')
			i = i + 2;
	}
	i = i + 1;
	return i;
	
}


uint16_t mystrlen(char *p)
{
	uint16_t i = 0;
	while(*(p+i)!='\0')
	{
		i++;
	}
	return i;
}
/*************************************************************************************************/
//	Dat_Fill命令处理
/*************************************************************************************************/
uint16_t Dat_Fill(char *dst,void *src,uint8_t Dat_Type)
{
	uint16_t i = 0;
	if(Dat_Type==Byte_Type)
	{
		i = Long_to_Str(dst,*(uint8_t*)src);
	}
	else if(Dat_Type==Int_Type)
	{
		i = Long_to_Str(dst,*(uint16_t*)src);
	}
	else if(Dat_Type==Float_Type)
	{
		i = Float_to_Str(dst,*(float *)src);
	}
	else if(Dat_Type==Str_Type)
	{
		i = my_strncpy(dst,(char *)src,mystrlen((char *)src));
	}
	return i;	
}
/*************************************************************************************************/
//	GET_ACK命令处理
/*************************************************************************************************/
void getCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type)
{
	uint16_t Num;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*不参与校验
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
	if(uart1_flag)
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);
	}
	else if(uart2_flag)
	{
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);	
		
	}
}
/*************************************************************************************************/
//	SET_ACK命令处理
/*************************************************************************************************/
void setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type)
{
	uint16_t Num;
	CMD_Refresh = 1;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*不参与校验
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
	if(uart1_flag)
	{
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);

	}
	else if(uart2_flag)
	{
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)UART1_TxBuf,Num);		
	}
}


void Tran_setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type)
{
	uint16_t Num;
    
//    Sys_CB_Copy.Survey_Status.channel_two = 0;
//    Sys_CB_Copy.Survey_Status.channel_one = 0;
	CMD_Refresh = 1;
   
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量
	
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*不参与校验
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
    
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);
   
}


/*************************************************************************************************/
//	总命令处理
/*************************************************************************************************/
// Command Process
void CMD_Process(void)
{
	static char tmp;
    static uint16_t wReceive232Pos = 0;
	static uint16_t	wReceiveCMD_Pos = 0;
    static char bReceive232ElementArr[512];
    static uint8_t bReceiveState = 0;

	if(UART1_rxQ.count)
	{
		while(UART1_rxQ.count)
		{
			DeCirQueue(&UART1_rxQ,&tmp);
			switch(bReceiveState)
			{
				case 0:   // Receive FE_MSG
					if(tmp == '#')
					{
						bReceiveState = 1; 
					}
					break;
				case 1:
					if((tmp=='s')||(tmp=='S'))
					{
						bReceiveState = 2; 
					}
					else
					{
						bReceiveState = 0; 
					}
					break;
				case 2:
					if((tmp=='i')||(tmp=='I'))
					{
						bReceiveState = 3; 
					}
					else
					{
						bReceiveState = 0; 
					}
					break;
				case 3:
					if((tmp=='c')||(tmp=='C'))
					{
						bReceiveState = 4; 
					}
					else
					{
						bReceiveState = 0; 
					}
					break;
				case 4:
					if(tmp==',')
					{
						bReceiveState = 5; 
					}
					else
					{
						bReceiveState = 0; 
					}
					break;
				case 5:
					if(tmp==',')
					{
						bReceiveState = 6; 			// #sic,,
						wReceive232Pos = 0;
					}
					else
					{
					   bReceiveState = 0; 
					}
					break;
				case 6:   // Receive Header size = 6
					 if((tmp>='A')&&(tmp<='Z'))				// 所有大写字母变成小写字母
					 {
						 tmp = tmp - 'A' + 'a';
					 }
					 bReceive232ElementArr[wReceive232Pos] = tmp;
					 wReceive232Pos++;
					 if (tmp == '\r')
					 {
						 bReceiveState = 7;
					 }
					 break;
				 case 7:   // Receive LSB of the length
					if(tmp=='\n')
					{
						bReceive232ElementArr[wReceive232Pos] = tmp;
						wReceive232Pos++;
						// ALL GET 系列命令
						if(rt_strncmp((const char *)bReceive232ElementArr,"get,",sizeof("get,")-1)==0)		//
						{
							wReceiveCMD_Pos = sizeof("get,")-1;
							if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"device",sizeof("device")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("device")-1;
								DEVICE_getCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"parameters",sizeof("device")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("parameters")-1;
								PARAMETERS_getCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"grade",sizeof("grade")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("grade")-1;
								GRADE_getCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
						}
						// ALL SET 系列命令
						else if(rt_strncmp((const char *)bReceive232ElementArr,"set,",sizeof("set,")-1)==0)		//
						{
							wReceiveCMD_Pos = sizeof("set,")-1;
							if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"device",sizeof("device")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("device")-1;
								DEVICE_setCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"parameters",sizeof("parameters")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("parameters")-1;
								PARAMETERS_setCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"grade",sizeof("grade")-1)==0)		//
							{
								wReceiveCMD_Pos += sizeof("grade,")-1;
								GRADE_setCMD(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							//转串口显示
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"data",sizeof("data")-1)==0)	
							{
								wReceiveCMD_Pos += sizeof("data.")-1;
								Conversion_Display(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}
							else if(rt_strncmp((const char *)&bReceive232ElementArr[wReceiveCMD_Pos],"dognum",sizeof("dognum")-1)==0)	
							{
								wReceiveCMD_Pos += sizeof("dognum")-1;
								Dognum_setCMD_Process(&bReceive232ElementArr[wReceiveCMD_Pos]);
							}

						}
					}
					bReceiveState = 0; 
					break;
			}
		}
	} 
}

