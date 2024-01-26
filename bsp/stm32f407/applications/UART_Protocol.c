/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: UART_Protocol.C
**��   ��   ��: 
**����޸�����: 2018��9��20��
**��        ��: ������ʼ��+�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.00
** �ա���: 2017��12��06��
** �衡��: ԭʼ�汾
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
//const DEVICE_INFO Device_Info  __attribute__((at(0X08007800))) = 		// ��λ�ڵڶ�������2K
//					{		
//						boot_ver_str,			// BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
//						sn_str,					// 15λ����ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						pn_str,					// Ӳ���汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						pid_str,				// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
//						hid_str,				// ϵͳ��Ϣ
//						brand_str,				// ��ƷƷ�ƣ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						model_str,				// ����,�ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						production_date_str,	// ��������, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						app_ver_str,			// �̼��汾��, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						gnssboard_str,			// �������ͣ��ַ����ԡ�\0����β�����Ȳ�����32��������\0��?
//					};

// ϵͳ���ƿ�
SYS_CB Sys_CB = {
//					// 1.1.1   INFO	�ڵ�(�ýڵ�֧��GET/SET��ȡҶ����Ϣ)
//					// DEVICE_INFO		Device_Info;
//					{		
//						boot_ver_str,			// BOOT�汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32��������\0����
//						sn_str,					// 15λ����ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						pn_str,					// Ӳ���汾�ţ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						pid_str,				// ��Ʒ�����룬�ַ����ԡ�\0����β�����Ȳ�����128(������\0��)
//						hid_str,				// ϵͳ��Ϣ
//						brand_str,				// ��ƷƷ�ƣ��ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						model_str,				// ����,�ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						production_date_str,	// ��������, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						app_ver_str,			// �̼��汾��, �ַ����ԡ�\0����β�����Ȳ�����32(������\0��)
//						gnssboard_str,			// �������ͣ��ַ����ԡ�\0����β�����Ȳ�����32��������\0��?
//					},
					Device_Info_Addr,
					// 1.1.2 TIME.�˽ڵ�����Ϊ�����ǵ�ע��ʱ���ʹ��ʱ�䣬����ѯ������֧��GET����
					// TIME 			Time;
					{
						{0},
						200,					// Ĭ��ע��ʱ��
						0,						// ʹ��ʱ��
						Flash_Sector2_Addr + 4,
						0
					},
					// 1.1.3 VOLTAGE����.�˽ڵ����Է��ص�ǰ��ص�ѹ����ӵ�Դ����ʵ��ѹ����VΪ��λ����֧��GET����
					// VOLTAGE 		Voltage;
					{
						12.5,
					},
					//1.1.8 DOGNUM.�˽ڵ���������������������ʱ�Ľ�������֧��SET����,����ʱ���ȷ���PENDING�����ʾ�ѽ��գ����ܺ���GETӦ��ĸ�ʽ����
					// DOGNUM 			Dogunm;
					{"0123456789"},
				
					//1.2 Parameters.�˽ڵ�����Ϊ�������ã������и�Ψһ��ͨ����Ҷ�ӽڵ㣬ֻ�����Ҷ�ӽڵ�������һ����䡣
					// PARAMETERS 		Parameters;
					{
						0.5,					// ��ˮ ��λΪ�� ȡֵΪ0.00-9.99 Ĭ�ϣ�0.50
						1480,					// ���� ��λ����/�� ȡֵΪ1300-1700 Ĭ�ϣ�1483
						0,						// ǳˮ����ˮ�� ��λΪ�� ȡֵΪ0.50-9.99 Ĭ�ϣ�1
						300,					// ��ˮ����ˮ�� ��λΪ�� ȡֵΪ1.00-600.00 Ĭ�ϣ�100
						0,						// �޻ز�������ʶ 0Ϊ�أ�1Ϊ�� Ĭ�ϣ�0���رգ�
						1,						// �ο�����ͨ���� ȡֵ1-16 Ĭ�ϣ�1
						5						// ��ʾ���������� ȡֵ1-10 Ĭ�ϣ�5									
					},
					//  1.1.4 REGI����.�˽ڵ���������ע������ǣ���֧��SET����
					// REGI Regi
					{"S23001111111111729048374639234567890",
					 "S23001111111111729048374639234567891",
					},
					// 1.1.5SURVEY_STATUS,�˽ڵ�����Ϊ��������͹رղ������1��ʾ������0��ʾ�رգ���֧��SET��GET���֧��ͬʱ�������е�ͨ����
					// SURVEY_STATUS
					{
						0,					// CHANNEL ONE
						1
					},						// CHANNEL TWO
					// 1.1.7 SURVEY_MODEL,�˽ڵ�����Ϊ���ø�ͨ���Ĳ���ģʽΪ�ֶ�����ģʽ���Զ�����ģʽ��0��ʾ�ֶ���1��ʾ�Զ���֧��SET���GET����,֧��ͬʱ�������е�ͨ����
					// SURVEY_MODEL
					{
						0,					// CHANNEL ONE
						1					// CHANNEL TWO
					},						
					// 1.2.2Calibration�ȶ԰�����У���������˽ڵ�����Ϊ�ȶ԰����ټ�����������֧��SET����
					// CALIBRATION 	Calibration;
					{5,0},
					//Sounder�����ǲ���.�˽ڵ�����Ϊÿ��ͨ���Ĳ����ǲ�������֧��GET,SET������Զ��ϴ����ϴ�ʱ��ʽͬGETӦ���ʽ��
					// SOUNDER			
					{
						{
							0.2,					// �������� ��λΪ���� ȡֵΪ 0.01-1.20 Ĭ�ϣ�0.2
							0.2
						},
						{
							0.4,					// ����ˮ�� ��λΪ�� ȡֵΪ0.2-20 Ĭ�ϣ�0.8;0.2-3.0 ����0.1   3.0-10.0 ����1  10-20 ����2
							0.4
						},
						{
							{
								1,					// ���� ȡֵ1-3 Ĭ�ϣ�2
								{2880,2400,1600,480,320,240,160,80}//{180,150,100,30,20,15,10,5}		// ���ʶ�Ӧ��ADCֵ
							},
							{
								1,					// ���� ȡֵ1-3 Ĭ�ϣ�2
								{2880,2400,1600,480,320,240,160,80}//{200,150,100,30,20,15,10,5}		// ���ʶ�Ӧ��ADCֵ
							},
						},
						{
							{
								3,					// ���� ȡֵ1-8 Ĭ�ϣ�3
								{800,1250,1700,2150,
								2600,3050,3500,4000}// ���� ��Ӧ��ADCֵ
							},
							{
								3,					// ���� ȡֵ1-8 Ĭ�ϣ�3
								{800,1250,1700,2150,
								2600,3050,3500,4000}// ���� ��Ӧ��ADCֵ
							},
						},
						{
							1,						// �ز���ֵ ȡֵ1-5 Ĭ�ϣ�3
							2
						},
						{
							20,						// TVG���� ȡֵ0-30 ��λΪlogR ����5 Ĭ�ϣ�20
							20
						},
						{
							0,					// ʱ���Ų��� ȡֵ0%-50%  0.0��ʾ�ر� Ĭ�ϣ�0.1
							0
						},
						{
							30,						// ��ʼ���� ȡֵΪ0-60 ��λΪdb����Ϊ5 Ĭ�ϣ�10
							30
						},
						{
							20,						// Ƶ��.��Ƶ��100KHZ-1000KHZ Ĭ�ϣ�200KHZ;��Ƶ��10KHZ-100KHZ   Ĭ�ϣ�20KHZ
							200
						},
						{
							0,						// ǳˮ����ģʽ.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
							0
						},
						{
							0,						// ǿ�������ز�.ȡֵ0��1  0���ر�  1������  Ĭ�ϣ�0
							0
						},
//						{
//							0,					// ǿ�������ز���Χ
//							0,
//							0,
//							0
//						},						
						{
							1,						// �ز��ж�.ȡֵ1��2��3  1����������ز�  2���ο���Ƶ�ز� 3���ײ�ز�.Ĭ�ϣ�1
							1
						}
					},
					{	
						5,						// grade ������λ,��λȡֵ0-7Ĭ��15�׵�
						0						// �Զ�����ʱȷ���ĵ�λ
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
//	�ַ�������������ʵ�ʿ�������
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
	if(i)			// dat Ϊ����ֵ
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
//	Dat_Fill�����
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
//	GET_ACK�����
/*************************************************************************************************/
void getCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type)
{
	uint16_t Num;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
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
//	SET_ACK�����
/*************************************************************************************************/
void setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type)
{
	uint16_t Num;
	CMD_Refresh = 1;
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
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
   
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
	
	Num = 0;
	Num += my_strncpy(UART1_TxBuf,Str,mystrlen(Str));
	Num += Dat_Fill(&UART1_TxBuf[Num],p_Dat,Dat_Type);
	UART1_TxBuf[Num++] = ',';
	UART1_TxBuf[Num++] = 'O';
	UART1_TxBuf[Num++] = 'K';
	UART1_TxBuf[Num++] = '*';
	UART1_TxBuf[Num] = do_crc(UART1_TxBuf,Num-1);			// crc,*������У��
	Num++;
	UART1_TxBuf[Num++] = '\r';
	UART1_TxBuf[Num++] = '\n';
    
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UART1_TxBuf,Num);
   
}


/*************************************************************************************************/
//	�������
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
					 if((tmp>='A')&&(tmp<='Z'))				// ���д�д��ĸ���Сд��ĸ
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
						// ALL GET ϵ������
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
						// ALL SET ϵ������
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
							//ת������ʾ
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

