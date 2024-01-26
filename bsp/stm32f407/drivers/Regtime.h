#ifndef __REGTIME_H
#define __REGTIME_H	
#include <rtthread.h>
#include "board.h"

#define 	Flash_SavePara_Addr	    0x080E1000							// ��ų�ˮ�����ټ��������ò����ĵ�ַ

#define		Flash_Sector2_Addr		0x08009000							// ע��ʱ���0ҳ��ַ
#define		Flash_Sector3_Addr		0x0800C000							// ע��ʱ���1ҳ��ַ
#define		Flash_Sector4_Addr		0x0800F000							// ע��ʱ���1ҳ��ַ��β��ַ
//#define		Flash_Sector2_Addr		0x08008000							// ע��ʱ���0ҳ��ַ
//#define		Flash_Sector3_Addr		0x0800C000							// ע��ʱ���1ҳ��ַ
//#define		Flash_Sector4_Addr		0x08010000							// ע��ʱ���1ҳ��ַ��β��ַ
#define		RegTime0_Addr			((uint32_t *)Flash_Sector2_Addr)	// ע��ʱ��0
#define		RegTime1_Addr			((uint32_t *)Flash_Sector3_Addr)	// ע��ʱ��1

#define		UNIQUEID_ADDRESS		0x1FFF7A10			 //��ȡstm32ID��λ��


//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  

/*************************************************************************************************
// ����Flash�����ص�ǰ����ʱ�䣬�ϵ�ʱ��
*************************************************************************************************/
void Read_CurUseTime(void);
/*************************************************************************************************
д�����ʱ��ֵ��ÿ10����д��һ�Σ�д��һ����Զ��л�����һ�飬������һ���ʽ��������ʹ�ã�
����õ����Flash��������Լ��ʽ��ʱ��
*************************************************************************************************/
void Regtime_write(uint32_t Addr,uint32_t Dat);
/*************************************************************************************************
 REGI���� ע��������,������ע��ʱ��ʱ��Ҫ���µİ��д���ݣ�ͬʱ����ʽ���ɵİ��
*************************************************************************************************/
void REGI_Process(uint32_t regtime);

void Read_Stm32ID(char *pTarget,uint32_t address);
char Dog_Key(char *key);

uint32_t GetSector(uint32_t Address);
void Save_Para_Process(uint32_t draft,uint32_t velocity);
void Read_Para_Process(void);

#endif 
