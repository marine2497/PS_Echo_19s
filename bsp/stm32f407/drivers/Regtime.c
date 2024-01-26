/*************************************************************************************************
	0x08008000-0x0800C000,0x0800C000-0x08010000����16K��Flash������ʽ��������
ÿ���һ�������ֽ�Ϊע��ʱ�䣬����ÿ���ֽڼ�¼һ��10���ӵĵݼ����֣�����һ��
д��󣬽���д�ڶ��飬�ڶ���д��󣬸�ʽ��һ�飬д��һ�飬ÿ���ʽʱ��250-500ms��
*************************************************************************************************/
#include <rtthread.h>
#include "board.h"
#include "stm32f4xx.h"
#include "Regtime.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.h"
#include <rthw.h>

#define		Flash_Sector1_Regi_Addr		0x08008000							// ע�����ַ
#define		Flash_Sector2_Regi_Addr		0x0800F000							// ע�����ַ����

#define		Regi_Num_Addr				((uint32_t *)Flash_Sector1_Regi_Addr)	// ע��ʱ��1
uint32_t p_Cur_Regi_Addr;

const uint32_t *p_regtime0 = RegTime0_Addr;
const uint32_t *p_regtime1 = RegTime1_Addr;

/*************************************************************************************************
// ����Flash�����ص�ǰ����ʱ�䣬�ϵ�ʱ��
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
			Sys_CB.Time.reg_time = *p_regtime1 ;//*p_regtime0; //lzb 20191205 ,,����bug			
			p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;  //0x0800F000	
		}
		for(i=0;i<36;i++) //��ȡע����		
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
		if(i==3072)		//4096// ����
		{
			Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 4;  //0x08009000
			Sys_CB.Time.Cur_Counter_10Min = 0;
			Sys_CB.Time.used_time = 0;
			Sys_CB.Time.reg_time = *p_regtime0;
			p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;
		}
		for(i=0;i<36;i++) //��ȡע����		
		{		
			if(*(uint32_t *)(p_Cur_Regi_Addr) != 0xffffffff)
				Sys_CB.Regi.srt_regi[i] = *(uint8_t *)(p_Cur_Regi_Addr + 4*i);
			else
				Sys_CB.Regi.srt_regi[i] = '0';
		}	

//		printf("Line =%d   p_Cur_Addr =%x \r\n ",__LINE__, Sys_CB.Time.p_Cur_Addr);		
	}
	else		// ����δע��
	{
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 4;
		Sys_CB.Time.Cur_Counter_10Min = 0;
		Sys_CB.Time.reg_time = 0;
		Sys_CB.Time.reg_time = *p_regtime0;
		
		
	}
	
	
//	printf("regtime =%d   usetime=%d  p0 =%x  p1=%x \r\n",Sys_CB.Time.reg_time,Sys_CB.Time.Cur_Counter_10Min/6,(uint32_t)p_regtime0,(uint32_t)p_regtime1);
	if(Sys_CB.Time.used_time ==0 && Sys_CB.Time.reg_time > 60000) //�״θ�оƬ���ع̼���ע��ʱ���ȡ����
	{
		Sys_CB.Time.reg_time = 0;	//ǿ������
	}
	

}
/*************************************************************************************************
д�����ʱ��ֵ��ÿ10����д��һ�Σ�д��һ����Զ��л�����һ�飬������һ���ʽ��������ʹ�ã�
����õ����Flash��������Լ��ʽ��ʱ��
*************************************************************************************************/
int WriteAddr;

void Regtime_write(uint32_t Addr, uint32_t Dat)
{
	
	// ����FLASH
	HAL_FLASH_Unlock();
	if(Sys_CB.Time.p_Cur_Addr == Flash_Sector3_Addr)			// �߽� 
	{

		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// �Ȳ�������3��ȷ��д��ǰ����3������  zgh
		
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr,Sys_CB.Time.reg_time); //0x0800C000
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr+4, Dat);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr+8;//0x0800c000 + 8	
		
		for(WriteAddr=0;WriteAddr<36;WriteAddr++) //д��ע����   //0x0800F000
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;//0x0800F000	
		
		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// ������2�� ������Ӧҳ 0x08008000 - 0x0800C000

	}
	else if(Sys_CB.Time.p_Cur_Addr == Flash_Sector4_Addr)// �߽� 0x0800F000
	{

		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// �Ȳ�������2��ȷ��д��ǰ����2������  zgh		

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector2_Addr,Sys_CB.Time.reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector2_Addr+4, Dat);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr+8;
		
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //д��ע����
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;//0x08008000	
		
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// ������3��  ������Ӧҳ 0x0800C000 - 0x08010000
		

	}
	else
	{

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Addr, Dat);
		Sys_CB.Time.p_Cur_Addr += 4;	
		                                                                         
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //д��ע����
		{
	//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, p_Cur_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		
								
	}
	// ��סFLASH
	HAL_FLASH_Lock();
}
/*************************************************************************************************
 REGI���� ע��������,������ע��ʱ��ʱ��Ҫ���µİ��д���ݣ�ͬʱ����ʽ���ɵİ��
*************************************************************************************************/
void REGI_Process(uint32_t New_reg_time)
{
	WriteAddr = 0;	
	rt_base_t  levevl;
	

	// ����FLASH
	HAL_FLASH_Unlock();
	levevl = rt_hw_interrupt_disable();
	
	if(Sys_CB.Time.p_Cur_Addr<Flash_Sector3_Addr)		// ��ǰ������һ�����
	{
	
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// �Ȳ�������3��ȷ��д��ǰ����3������  zgh

		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr,New_reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_Sector3_Addr + 4,Sys_CB.Time.Cur_Counter_10Min);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector3_Addr + 8;
		Sys_CB.Time.reg_time = *p_regtime1;


		for(WriteAddr=0;WriteAddr<36;WriteAddr++) //д��ע����
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector2_Regi_Addr;
		
		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// ������Ӧҳ 0x08008000 - 0x0800C000

	}
	else
	{

		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);// �Ȳ�������2��ȷ��д��ǰ����2������  zgh	

		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Addr, New_reg_time);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector2_Addr + 4, Sys_CB.Time.Cur_Counter_10Min);
		Sys_CB.Time.p_Cur_Addr = Flash_Sector2_Addr + 8;
		Sys_CB.Time.reg_time = *p_regtime0;
		//2021-6-17 09:45:49
		for(WriteAddr=0; WriteAddr<36; WriteAddr++) //д��ע����
		{
	//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, p_Cur_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Sector1_Regi_Addr+ WriteAddr*4, Sys_CB.Regi.srt_regi[WriteAddr]);
		}
		p_Cur_Regi_Addr = Flash_Sector1_Regi_Addr;	
		
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);// ������Ӧҳ 0x0800C000 - 0x08010000
		


	}
	// ��סFLASH
	rt_hw_interrupt_enable(levevl);
	HAL_FLASH_Lock();
	
}

//�洢��ˮ�����ٵĺ���  λ��flash ���һ����  ADDR_FLASH_SECTOR_11   11��
//�����ַ  ��ˮ��0x080E1000   //���� 0x080E1000+4

void Save_Para_Process(uint32_t draft,uint32_t velocity)
{
	rt_base_t  levevl;
	
	
	HAL_FLASH_Unlock();
	levevl = rt_hw_interrupt_disable();
	

	FLASH_Erase_Sector(FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE_3);// �Ȳ�������3��ȷ��д��ǰ����3������  zgh

	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_SavePara_Addr,draft);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Flash_SavePara_Addr+4,velocity);
	


	// ��סFLASH
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
	uint32_t  dst[5];	  // ��������
	uint32_t  uOddBit[6];   // ����λ
	uint32_t  uEvenBit[6];  // ż��λ 

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
				    && (dst[3] == uSource[8]) && (dst[4] == uSource[9]) )    // ���ȷʵ���Ϸ����
	{
	 	uOddBit[0] = uSource[1];      uEvenBit[0] = uSource[0];
		uOddBit[1] = uSource[3];	  uEvenBit[1] = uSource[2];
		uOddBit[2] = uSource[5];	  uEvenBit[2] = uSource[4];
		uOddBit[3] = uSource[7];	  uEvenBit[3] = uSource[6];
		uOddBit[4] = uSource[9];	  uEvenBit[4] = uSource[8];    // ����ż��λ	
		
		uOddBit[5] = uOddBit[0];
	    for( iLoop = 0; iLoop < 5; iLoop++ )
		{
			dst[iLoop] = ( ( 13 * uOddBit[iLoop] * (uOddBit[iLoop+1] +iLoop ) ) % 23 ) % 8 + 1; 
		} // ��������
		uSource[0] = dst[0];
		uSource[2] = dst[1];
		uSource[4] = dst[2];
		uSource[6] = dst[3];
		uSource[8] = dst[4];  // ���ص�ż��λ
					
		uEvenBit[5] = uEvenBit[0];  
		for(iLoop = 0; iLoop< 5; iLoop++ )
		{
			dst[iLoop] = ( ( 13 * uEvenBit[iLoop] * (uEvenBit[iLoop+1] + iLoop ) ) % 23 ) % 8 + 1; 
		} // ż������
		uSource[1] = dst[0];
		uSource[3] = dst[1];
		uSource[5] = dst[2];
		uSource[7] = dst[3];
		uSource[9] = dst[4];  // ���ص�����λ
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
	
//	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���		
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


