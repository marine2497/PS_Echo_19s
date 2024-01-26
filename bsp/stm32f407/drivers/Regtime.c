/*************************************************************************************************
	0x08008000-0x0800C000,0x0800C000-0x08010000两块16K的Flash轮流格式化，其中
每块第一个的四字节为注册时间，其余每四字节记录一个10分钟的递加数字，当第一块
写完后，接着写第二块，第二块写完后，格式第一块，写第一块，每块格式时间250-500ms。
*************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "stm32f4xx.h"
#include "Regtime.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.h"
#include <rthw.h>

#define		Flash_Sector1_Regi_Addr		0x08008000							// 注册码地址
#define		Flash_Sector2_Regi_Addr		0x0800F000							// 注册码地址结束

#define		Regi_Num_Addr				((uint32_t *)Flash_Sector1_Regi_Addr)	// 注册时间1
uint32_t p_Cur_Regi_Addr;

const uint32_t *p_regtime0 = RegTime0_Addr;
const uint32_t *p_regtime1 = RegTime1_Addr;

/*************************************************************************************************
// 搜索Flash，返回当前已用时间，上电时用
*************************************************************************************************/
void Read_CurUseTime(void)		
{
	uint16_t i = 0;

	if(*p_regtime0 != 0xffffffff)
	{
		p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;//0x08008000
		for(i=1;i<3072;i++) //4096
		{
			if(*(p_regtime0+i) == 0xffffffff)
			{
				Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 4*i; //0x08009000
				if(i>1)
					Sys_CB.Time.Cur_Counter_10Min = *(p_regtime0+i-1);
				else
					Sys_CB.Time.Cur_Counter_10Min = 0;
				
				Sys_CB.Time.used_time = Sys_CB.Time.Cur_Counter_10Min/6;
				Sys_CB.Time.reg_time = *p_regtime0;
				break;
			}
		}
		if(i==3072) //4096
		{                            //0x0800C000
			Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr + 4; //Flash_Sector2_Addr + 4; //lzb 20191205
			Sys_CB.Time.Cur_Counter_10Min = 0;
			Sys_CB.Time.used_time = 0;
			Sys_CB.Time.reg_time = *p_regtime1 ;//*p_regtime0; //lzb 20191205 ,,隐患bug			
			p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;  //0x0800F000	
		}
		for(i=0;i<36;i++) //读取注册码		
		{		
			if(*(uint32_t *)(p_Cur_Regi_Addr) != 0xffffffff)
				Sys_CB.Regi.srt_regi[i] = *(uint8_t *)(p_Cur_Regi_Addr + 4*i);
			else
				Sys_CB.Regi.srt_regi[i] = '0';
		}	
//		printf("Line =%d   p_Cur_Addr =%x \r\n ",__LINE__, Sys_CB.Time.p_Cur_Addr);
	}
	else if(*p_regtime1!=0xffffffff)
	{
		p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;//0x0800F000		
		for(i=1;i<3072;i++) //4096
		{
			if(*(p_regtime1+i) == 0xffffffff)
			{                            //0x0800C000
				Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr + 4*i;
				if(i>1)
					Sys_CB.Time.Cur_Counter_10Min = *(p_regtime1+i-1);
				else
					Sys_CB.Time.Cur_Counter_10Min = 0;
				Sys_CB.Time.used_time = Sys_CB.Time.Cur_Counter_10Min/6;
				Sys_CB.Time.reg_time = *p_regtime1;
				break;
			}
		}
		if(i==3072)		//4096// 出错
		{
			Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 4;  //0x08009000
			Sys_CB.Time.Cur_Counter_10Min = 0;
			Sys_CB.Time.used_time = 0;
			Sys_CB.Time.reg_time = *p_regtime0;
			p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;
		}
		for(i=0;i<36;i++) //读取注册码		
		{		
			if(*(uint32_t *)(p_Cur_Regi_Addr) != 0xffffffff)
				Sys_CB.Regi.srt_regi[i] = *(uint8_t *)(p_Cur_Regi_Addr + 4*i);
			else
				Sys_CB.Regi.srt_regi[i] = '0';
		}	

//		printf("Line =%d   p_Cur_Addr =%x \r\n ",__LINE__, Sys_CB.Time.p_Cur_Addr);		
	}
	else		// 报错，未注册
	{
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 4;
		Sys_CB.Time.Cur_Counter_10Min = 0;
		Sys_CB.Time.reg_time = 0;
		Sys_CB.Time.reg_time = *p_regtime0;
		
		
	}
	
	
//	printf("regtime =%d   usetime=%d  p0 =%x  p1=%x \r\n",Sys_CB.Time.reg_time,Sys_CB.Time.Cur_Counter_10Min/6,(uint32_t)p_regtime0,(uint32_t)p_regtime1);
	if(Sys_CB.Time.used_time ==0 && Sys_CB.Time.reg_time > 60000) //首次给芯片下载固件，注册时间读取有误！
	{
		Sys_CB.Time.reg_time = 0;	//强制清零
	}
	

}
/*************************************************************************************************
写入更新时间值，每10分钟写入一次，写满一块后，自动切换至另一块，并将上一块格式化，轮流使用，
以其得到最长的Flash寿命，节约格式化时间
*************************************************************************************************/
int WriteAddr;

void Regtime_write(uint32_t Addr, uint32_t Dat)
{
	
	// 解锁FLASH
	HAL_FLASH_Unlock();
	if(Sys_CB.Time.p_Cur_Addr == Flash_Sector3_Addr)			// 边界 
	{

		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// 先擦除扇区3，确保写入前扇区3无数据  zgh
		
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr,Sys_CB.Time.reg_time); //0x0800C000
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr+4, Dat);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr+8;//0x0800c000 + 8	
		
		for(WriteAddr=0;WriteAddr<36;WriteAddr++) //写入注册码   //0x0800F000
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;//0x0800F000	
		
		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// 擦除第2块 擦除对应页 0x08008000 - 0x0800C000

	}
	else if(Sys_CB.Time.p_Cur_Addr == Flash_Sector4_Addr)// 边界 0x0800F000
	{

		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// 先擦除扇区2，确保写入前扇区2无数据  zgh		

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector2_Addr,Sys_CB.Time.reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector2_Addr+4, Dat);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr+8;
		
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //写入注册码
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;//0x08008000	
		
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// 擦除第3块  擦除对应页 0x0800C000 - 0x08010000
		

	}
	else
	{

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Addr, Dat);
		Sys_CB.Time.p_Cur_Addr += 4;	
		                                                                         
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //写入注册码
		{
	//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, p_Cur_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		
								
	}
	// 锁住FLASH
	HAL_FLASH_Lock();
}
/*************************************************************************************************
 REGI属性 注册码命令,当更新注册时间时，要想新的板块写数据，同时，格式化旧的板块
*************************************************************************************************/
void REGI_Process(uint32_t New_reg_time)
{
	WriteAddr = 0;	
	rt_base_t  levevl;
	

	// 解锁FLASH
	HAL_FLASH_Unlock();
	levevl = rt_hw_interrupt_disable();
	
	if(Sys_CB.Time.p_Cur_Addr<Flash_Sector3_Addr)		// 当前操作第一个板块
	{
	
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// 先擦除扇区3，确保写入前扇区3无数据  zgh

		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr,New_reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr + 4,Sys_CB.Time.Cur_Counter_10Min);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr + 8;
		Sys_CB.Time.reg_time = *p_regtime1;


		for(WriteAddr=0;WriteAddr<36;WriteAddr++) //写入注册码
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;
		
		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// 擦除对应页 0x08008000 - 0x0800C000

	}
	else
	{

		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// 先擦除扇区2，确保写入前扇区2无数据  zgh	

		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Addr, New_reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Addr + 4, Sys_CB.Time.Cur_Counter_10Min);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 8;
		Sys_CB.Time.reg_time = *p_regtime0;
		//2021-6-17 09:45:49
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //写入注册码
		{
	//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, p_Cur_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;	
		
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// 擦除对应页 0x0800C000 - 0x08010000
		


	}
	// 锁住FLASH
	rt_hw_interrupt_enable(levevl);
	HAL_FLASH_Lock();
	
}

//存储吃水和声速的函数  位于flash 最后一个块  ADDR_FLASH_SECTOR_11   11块
//具体地址  吃水：0x080E1000   //声速 0x080E1000+4

void Save_Para_Process(uint32_t draft,uint32_t velocity)
{
	rt_base_t  levevl;
	
	
	HAL_FLASH_Unlock();
	levevl = rt_hw_interrupt_disable();
	

	FLASH_Erase_Sector(FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE_3);// 先擦除扇区3，确保写入前扇区3无数据  zgh

	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_SavePara_Addr,draft);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_SavePara_Addr+4,velocity);
	


	// 锁住FLASH
	rt_hw_interrupt_enable(levevl);
	HAL_FLASH_Lock();
	
}
void Read_Para_Process()
{
	
	Sys_CB_Copy.Parameters.draft    = *(__IO uint32_t *)(Flash_SavePara_Addr) /100.0f;
    Sys_CB_Copy.Parameters.velocity = *(__IO uint32_t *)(Flash_SavePara_Addr+4)/100.0f;
    
    if((Sys_CB_Copy.Parameters.draft < 0.0f) || (Sys_CB_Copy.Parameters.draft > 9.99f))
    {
         Sys_CB_Copy.Parameters.draft = 0.0;
    }
    if((Sys_CB_Copy.Parameters.velocity < 1300) || (Sys_CB_Copy.Parameters.velocity > 1700))
    {
         Sys_CB_Copy.Parameters.velocity =  1480.0;
    }

	printf("draft =%.2f, velocity =%0.2f \r\n",Sys_CB_Copy.Parameters.draft,Sys_CB_Copy.Parameters.velocity);
}

char Dog_Key(char *key)
{
	char iLoop;
	char  uSource[10];
	uint32_t  src[6];
	uint32_t  dst[5];	  // 加密数组
	uint32_t  uOddBit[6];   // 奇数位
	uint32_t  uEvenBit[6];  // 偶数位 

	for(iLoop=0;iLoop<10;iLoop++) 
		uSource[iLoop] = *(key+iLoop)	; 

	for(iLoop=0; iLoop<10; iLoop++) 
		uSource[iLoop] -= 0x30;

	src[0] = uSource[0];
	src[1] = uSource[1];
	src[2] = uSource[2];
	src[3] = uSource[3];
	src[4] = uSource[4];

	src[5] = src[0];
	for( iLoop = 0; iLoop < 5; iLoop++ )
	{
		dst[iLoop] = ( ( 13 * src[iLoop] * (src[iLoop+1] + iLoop ) ) % 23 ) % 8 + 1; 
	} 

	if( (dst[0] == uSource[5]) && (dst[1] == uSource[6]) && (dst[2] == uSource[7]) 
				    && (dst[3] == uSource[8]) && (dst[4] == uSource[9]) )    // 如果确实是南方软件
	{
	 	uOddBit[0] = uSource[1];      uEvenBit[0] = uSource[0];
		uOddBit[1] = uSource[3];	  uEvenBit[1] = uSource[2];
		uOddBit[2] = uSource[5];	  uEvenBit[2] = uSource[4];
		uOddBit[3] = uSource[7];	  uEvenBit[3] = uSource[6];
		uOddBit[4] = uSource[9];	  uEvenBit[4] = uSource[8];    // 保存偶数位	
		
		uOddBit[5] = uOddBit[0];
	    for( iLoop = 0; iLoop < 5; iLoop++ )
		{
			dst[iLoop] = ( ( 13 * uOddBit[iLoop] * (uOddBit[iLoop+1] +iLoop ) ) % 23 ) % 8 + 1; 
		} // 奇数加密
		uSource[0] = dst[0];
		uSource[2] = dst[1];
		uSource[4] = dst[2];
		uSource[6] = dst[3];
		uSource[8] = dst[4];  // 返回到偶数位
					
		uEvenBit[5] = uEvenBit[0];  
		for(iLoop = 0; iLoop< 5; iLoop++ )
		{
			dst[iLoop] = ( ( 13 * uEvenBit[iLoop] * (uEvenBit[iLoop+1] + iLoop ) ) % 23 ) % 8 + 1; 
		} // 偶数加密
		uSource[1] = dst[0];
		uSource[3] = dst[1];
		uSource[5] = dst[2];
		uSource[7] = dst[3];
		uSource[9] = dst[4];  // 返回到奇数位
		for(iLoop=0; iLoop<10; iLoop++) 
			uSource[iLoop] += 0x30;
	
		for(iLoop=0;iLoop<10;iLoop++) 
			*(key+iLoop) = uSource[iLoop];
		*(key+10)= '\0';
		return(1);
	}
	else return(0);
}

void Read_Stm32ID(char *pTarget,uint32_t address)
{
	uint32_t  stm32idnum;
	char iLoop=0;
	char buff[16] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	stm32idnum = *(uint32_t*)(address);
	while(iLoop<8)
	{
		pTarget[iLoop] = buff[stm32idnum%16];
		iLoop++;
		stm32idnum = stm32idnum/16;			
	}	
	stm32idnum = *(uint32_t*)(address+0x04);
	while(iLoop<12)
	{
		pTarget[iLoop] = buff[stm32idnum%16];
		iLoop++;
		stm32idnum = stm32idnum/16;			
	}

	pTarget[12] =  Sys_CB.Device_Info->sn[11];
	pTarget[13] =  Sys_CB.Device_Info->sn[12];
	pTarget[14] =  Sys_CB.Device_Info->sn[13];
	pTarget[15] =  Sys_CB.Device_Info->sn[14];
	
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// 试图持有信号量		
//	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)"pid=",4);	
//	
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);
//	HAL_UART_Transmit_DMA(&huart1, pTarget,15);
}


/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)) //4000 - 8000
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)) //8000 - C000
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)) //C000 - 10000 
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else
  {
    sector = FLASH_SECTOR_11;  
  }
  return sector;
}


