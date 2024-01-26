#ifndef __REGTIME_H
#define __REGTIME_H	
#include <rtthread.h>
#include "board.h"

#define 	Flash_SavePara_Addr	    0x080E1000							// 存放吃水和声速及其他配置参数的地址

#define		Flash_Sector2_Addr		0x08009000							// 注册时间第0页地址
#define		Flash_Sector3_Addr		0x0800C000							// 注册时间第1页地址
#define		Flash_Sector4_Addr		0x0800F000							// 注册时间第1页地址结尾地址
//#define		Flash_Sector2_Addr		0x08008000							// 注册时间第0页地址
//#define		Flash_Sector3_Addr		0x0800C000							// 注册时间第1页地址
//#define		Flash_Sector4_Addr		0x08010000							// 注册时间第1页地址结尾地址
#define		RegTime0_Addr			((uint32_t *)Flash_Sector2_Addr)	// 注册时间0
#define		RegTime1_Addr			((uint32_t *)Flash_Sector3_Addr)	// 注册时间1

#define		UNIQUEID_ADDRESS		0x1FFF7A10			 //获取stm32ID码位置


//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes  

/*************************************************************************************************
// 搜索Flash，返回当前已用时间，上电时用
*************************************************************************************************/
void Read_CurUseTime(void);
/*************************************************************************************************
写入更新时间值，每10分钟写入一次，写满一块后，自动切换至另一块，并将上一块格式化，轮流使用，
以其得到最长的Flash寿命，节约格式化时间
*************************************************************************************************/
void Regtime_write(uint32_t Addr,uint32_t Dat);
/*************************************************************************************************
 REGI属性 注册码命令,当更新注册时间时，要想新的板块写数据，同时，格式化旧的板块
*************************************************************************************************/
void REGI_Process(uint32_t regtime);

void Read_Stm32ID(char *pTarget,uint32_t address);
char Dog_Key(char *key);

uint32_t GetSector(uint32_t Address);
void Save_Para_Process(uint32_t draft,uint32_t velocity);
void Read_Para_Process(void);

#endif 
