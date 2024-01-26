/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "dac.h"
#include "math.h"
#include "time.h"
#include "Ultrason_rcv.H"
/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

// TVG表，每10ms更新一次增益值，直到最大为止
uint16_t TVG_DAC1_Dat[TVG_DAC_Num]={1600,1280,1280,1280,1280,1280,1200,3500,819,819,819,819,819,819,819,819,819,819,819,819,};
uint16_t TVG_DAC2_Dat[TVG_DAC_Num]={400,750,819,819,819,819,1200,3500,819,819,819,819,819,819,819,819,819,819,819,819,};

/* DAC init function */
void MX_DAC_Init(void)
{
	hdac.Instance = DAC;
	HAL_DAC_Init(&hdac);
//	for(i=0;i<20;i++)
//	{
//		TVG_DAC1_Dat[i] = 200*i;
//		TVG_DAC2_Dat[19-i] = TVG_DAC1_Dat[i];
//	}
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hdac->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspInit 0 */

  /* USER CODE END DAC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DAC_CLK_ENABLE();
	  
    /**DAC GPIO Configuration    
    PA4     ------> DAC_OUT1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END DAC_MspInit 1 */
  }
}

void TVG_Channel_One_Start(void)
{
//	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
	Sys_CB_L.TVG_Dat_Index=0;
	htim4.Instance->CNT = 0;
	HAL_TIM_Base_Start_IT(&htim4);
//	hdac.Instance->DHR12R1 = *(uint32_t* )Sys_CB_L.p_TVG_Dat;
	hdac.Instance->DHR12R1 = (10*Sys_CB_Copy.Sounder.tvgs.channel_one*log10f(0.8*(Sys_CB_L.TVG_Dat_Index+1)))+50*Sys_CB_Copy.Sounder.startgain.channel_one;		
	
	hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1;
}
void TVG_Channel_One_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&htim4);
	Sys_CB_L.TVG_Dat_Index = 0;
	hdac.Instance->DHR12R1 = 0;
	hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1;
}
void TVG_Channel_Two_Start(void)
{
//	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
	Sys_CB_H.TVG_Dat_Index=0;
	htim6.Instance->CNT = 0;
	HAL_TIM_Base_Start_IT(&htim6);
//	hdac.Instance->DHR12R2 = *(uint32_t* )Sys_CB_H.p_TVG_Dat;
	hdac.Instance->DHR12R2 = (5*Sys_CB_Copy.Sounder.tvgs.channel_two*log10f(0.8*(Sys_CB_H.TVG_Dat_Index+1)))+43*Sys_CB_Copy.Sounder.startgain.channel_two;		
		
	hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG2;
}
void TVG_Channel_Two_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&htim6);
	Sys_CB_H.TVG_Dat_Index = 0;
	hdac.Instance->DHR12R2 = 0;
	hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG2;
}
