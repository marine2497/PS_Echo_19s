#include <rtthread.h>
#include "board.h"
#include "stm32f4xx.h"
#include "adc.h"
#include "dac.h"
#include "iwdg.h"
#include "stm32f4xx_hal.h"
#include "UART_Protocol.H"
#include "Ultrason_rcv.H"
#include "Ultrason_Cal.h"
#include "math.h"
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;			// PWM驱动发射波
TIM_HandleTypeDef htim2;			// 控制ADC3采样频率，低频
TIM_HandleTypeDef htim3;			// 控制ADC1采样频率，高频
TIM_HandleTypeDef htim4;			// TVG 定时器
//TIM_HandleTypeDef htim5;			// 控制ADC2采样频率
TIM_HandleTypeDef htim6;			// TVG 定时器
TIM_HandleTypeDef htim7;			// 机器使用时间定时器
TIM_HandleTypeDef htim8;			// PWM驱动发射波
TIM_HandleTypeDef htim9;			// PWM驱动充电脉冲

/****************事件定义*******************/
struct rt_semaphore Used_time_sem;					// 十分钟事件

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_oc)
{
	if(htim_oc->Instance==TIM1)
	{
	__HAL_RCC_TIM1_CLK_ENABLE();
	}
	else if(htim_oc->Instance==TIM8)
	{
	__HAL_RCC_TIM8_CLK_ENABLE();
	}
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
	}
	else if(htim_base->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();
	}
	else if(htim_base->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
	}
	else if(htim_base->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();
	    /* TIM4 interrupt Init */
		HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(htim_base->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();
	}
	else if(htim_base->Instance==TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();
		/* TIM6 interrupt Init */
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
	else if(htim_base->Instance==TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();
		/* TIM7 interrupt Init */
		HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
	}
	else if(htim_base->Instance==TIM8)
	{
		__HAL_RCC_TIM8_CLK_ENABLE();
	}
	else if(htim_base->Instance==TIM9)
	{
		__HAL_RCC_TIM9_CLK_ENABLE();
	}
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if(htim->Instance==TIM1)
	{
		/* USER CODE BEGIN TIM1_MspPostInit 0 */

		/* USER CODE END TIM1_MspPostInit 0 */
		/**TIM1 GPIO Configuration    
		PE8     ------> TIM1_CH1N
		PA8     ------> TIM1_CH1 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USER CODE BEGIN TIM1_MspPostInit 1 */

		/* USER CODE END TIM1_MspPostInit 1 */
	}
	else if(htim->Instance==TIM8)
	{
		/* USER CODE BEGIN TIM8_MspPostInit 0 */

		/* USER CODE END TIM8_MspPostInit 0 */

		/**TIM8 GPIO Configuration    
		PB0     ------> TIM8_CH2N
		PC7     ------> TIM8_CH2 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* USER CODE BEGIN TIM8_MspPostInit 1 */

		/* USER CODE END TIM8_MspPostInit 1 */
	}
	else if(htim->Instance==TIM9)
	{
		/* USER CODE BEGIN TIM9_MspPostInit 0 */

		/* USER CODE END TIM9_MspPostInit 0 */

		/**TIM9 GPIO Configuration    
		PE6     ------> TIM9_CH2
		PE5     ------> TIM9_CH1 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
}
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef* htim_oc)
{
	if(htim_oc->Instance==TIM1)
	{
	__HAL_RCC_TIM1_CLK_DISABLE();
	}
	else if(htim_oc->Instance==TIM8)
	{
	__HAL_RCC_TIM8_CLK_DISABLE();
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM6)
	{
		__HAL_RCC_TIM6_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM7)
	{
		__HAL_RCC_TIM7_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM8)
	{
		__HAL_RCC_TIM8_CLK_DISABLE();
	}
	else if(htim_base->Instance==TIM9)
	{
		__HAL_RCC_TIM9_CLK_DISABLE();
	}
}
// 返回脉冲重复个数
uint16_t Freq_Width(uint16_t Freq,float txwidth,uint16_t *p_Freq)
{
	double tmp_float = Freq;
	int16_t tmp_int;
	
	tmp_float = 84000/tmp_float;
	tmp_int = (uint16_t)tmp_float;
	tmp_float = tmp_float - tmp_int;
	if(tmp_float>0.5)				// 四舍五入
		tmp_int	-= 2;				// 算出分频比
	else
		tmp_int	-= 1;
	if(tmp_int>0)
		*p_Freq = tmp_int;
	else
		*p_Freq = 0;
	
	tmp_float = tmp_int+1;
	tmp_float = tmp_float/84000;	// 算出真正的周期，单位为ms
	tmp_float = txwidth/tmp_float;
	tmp_int = (uint16_t)tmp_float;
	tmp_float = tmp_float - tmp_int;
	if(tmp_float>0.5)
		tmp_int	-= 2;				// 算出重复个数
	else
		tmp_int	-= 1;				// 算出重复个数
	if(tmp_int<=0)
		tmp_int = 0;
	return tmp_int;
}
/* TIM1 init function */
void MX_TIM1_Init(uint16_t Freq,float txwidth)				// 频率和脉宽
{
	uint16_t Freq_Num,Rep_Num;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	
	Rep_Num = Freq_Width(Freq,txwidth,&Freq_Num);
		
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = Freq_Num;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = Rep_Num;					// 高频发射冲宽个数
	HAL_TIM_OC_Init(&htim1);

	HAL_TIM_OnePulse_Init(&htim1, TIM_OPMODE_SINGLE);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = Freq_Num>>1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 10;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

	HAL_TIM_MspPostInit(&htim1);
}

/* TIM2 init function */
void MX_TIM2_Init(void)			// 控制ADC2采样频率
{
	TIM_MasterConfigTypeDef sMasterConfig;

	htim2.Instance = TIM2;
						
#ifdef LF_50us
		htim2.Init.Prescaler = 19;
#else	
		htim2.Init.Prescaler = 39;// 210KHz// 调试更改过
#endif
	
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 209;							// 10KHz采样率，低频段ADC采样定时器
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim2);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}
/* TIM3 init function */
void MX_TIM3_Init(void)			// 控制ADC1采样频率
{
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;							
#ifdef HF_20us
		htim3.Init.Prescaler = 7;
#else
		htim3.Init.Prescaler = 3;							    // 21MHz  
#endif
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 209;							// 100KHz采样率，高频段ADC采样定时器
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim3);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);
}

/* TIM5 init function */
//void MX_TIM5_Init(void)				// 控制ADC2采样频率
//{
//	TIM_MasterConfigTypeDef sMasterConfig;

//	htim5.Instance = TIM5;
//	htim5.Init.Prescaler = 399;						// 21KHz// 调试更改过
//	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
//	htim5.Init.Period = 209;							// 1KHz采样率，低频段ADC采样定时器
//	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//	HAL_TIM_Base_Init(&htim5);
//	  
//	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
//	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);
//	
//	HAL_TIM_Base_Start(&htim5); 										// 启动TIM5
//}

/* TIM4 init function */
void MX_TIM4_Init(void)				// TVG 定时器
{
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 19;//39;//83;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 209;//9999; 100us
	HAL_TIM_Base_Init(&htim4);
}

/* TIM6 init function */
void MX_TIM6_Init(void)				// TVG 定时器
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 19;//39;//83;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 209;//9999;
	HAL_TIM_Base_Init(&htim6);
}
/* TIM7 init function */
void MX_TIM7_Init(void)				// 机器使用时间定时器
{
	htim7.Instance = TIM7;
	
	htim7.Init.Prescaler = 33599;	// 分频至2.5KHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 24999;		// 定时间隔为10s

/*
	htim7.Init.Prescaler = 33599;	// 分频至2.5KHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 24;		// 定时间隔为0.01s 测试
*/	
	
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

/* TIM8 init function */
void MX_TIM8_Init(uint16_t Freq,float txwidth)				// 频率和脉宽
{
	uint16_t Freq_Num,Rep_Num;
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

	Rep_Num = Freq_Width(Freq,txwidth,&Freq_Num);
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 1;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = Freq_Num;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = Rep_Num;							// 低频频发射冲宽个数
	HAL_TIM_Base_Init(&htim8);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig);

	HAL_TIM_OC_Init(&htim8);

	HAL_TIM_OnePulse_Init(&htim8, TIM_OPMODE_SINGLE);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = Freq_Num>>1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_OC_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 10;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig);

	HAL_TIM_MspPostInit(&htim8);
}
/* TIM9 init function */
void MX_TIM9_Init(void)				// 高压充电定时器
{
	TIM_OC_InitTypeDef sConfigOC;

	htim9.Instance = TIM9;
	htim9.Init.Prescaler = 1679;	// 分频至100KHz
	htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim9.Init.Period = 999; //10		// 10ms周期 即1=10us
	HAL_TIM_Base_Init(&htim9);

	HAL_TIM_PWM_Init(&htim9);

	sConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
	sConfigOC.Pulse = 39;	//5								// 低频充电脉冲,最大值999,当前400us
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1);

	sConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
	sConfigOC.Pulse = 39;  //5;									// 高频充电脉冲,最大值999
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_2);

	HAL_TIM_MspPostInit(&htim9);
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_1);							
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_2);							
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static	uint8_t usedtime_counter = 0;
	if(htim==&htim4)//低频起始增益控制 100us = 7.4cm
	{
		if(Sys_CB_L.TVG_Dat_Index*7.5 < (Sys_CB_Copy.Sounder.txwidth.channel_one*100+50) \
			|| (Sys_CB_L.TVG_Dat_Index >= Caldeep[LF].filttick_begin && Sys_CB_L.TVG_Dat_Index <= Caldeep[LF].filttick_end) )		
		{
			if(Sys_CB_L.TVG_Dat_Index < 20 && Sys_CB_Copy.Sounder.startgain.channel_one < 20)
				hdac.Instance->DHR12R1 = (5*Sys_CB_Copy.Sounder.tvgs.channel_one*log10f(0.8*(Sys_CB_L.TVG_Dat_Index+1)))+58*20;					
			else
				hdac.Instance->DHR12R1 = (5*Sys_CB_Copy.Sounder.tvgs.channel_one*log10f(0.8*(Sys_CB_L.TVG_Dat_Index+1)))+58*Sys_CB_Copy.Sounder.startgain.channel_one;		
		}
		else
		{
			hdac.Instance->DHR12R1 = 0;		
		}
		hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1;
			
		
		if(Sys_CB_L.TVG_Dat_Index >= 4200) //100*4200*1480/3 = 300m
			Sys_CB_L.TVG_Dat_Index = 0;		
		else if(Sys_CB_L.TVG_Dat_Index >= 1)
			Sys_CB_L.TVG_Dat_Index++;		
	}
	else if(htim==&htim6)//高频起始增益控制 100us = 7.4cm
	{	
		if(Sys_CB_H.TVG_Dat_Index*7.5 < (Sys_CB_Copy.Sounder.txwidth.channel_two*100+10) \
			|| (Sys_CB_H.TVG_Dat_Index*2.5 >= Caldeep[HF].filttick_begin && Sys_CB_H.TVG_Dat_Index*2.5 <= Caldeep[HF].filttick_end) )
		{		
			if(Sys_CB_H.TVG_Dat_Index < 30 && Sys_CB_Copy.Sounder.startgain.channel_two < 13)
				hdac.Instance->DHR12R2 = (5*Sys_CB_Copy.Sounder.tvgs.channel_two*log10f(0.8*(Sys_CB_H.TVG_Dat_Index+1)))+43*17;		
			else
				hdac.Instance->DHR12R2 = (5*Sys_CB_Copy.Sounder.tvgs.channel_two*log10f(0.8*(Sys_CB_H.TVG_Dat_Index+1)))+43*Sys_CB_Copy.Sounder.startgain.channel_two;		
		}
		else
			hdac.Instance->DHR12R2 = 0;
		
		hdac.Instance->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG2;

		if(Sys_CB_H.TVG_Dat_Index >= 8400) //100*4200*1480/2= 300m
			Sys_CB_H.TVG_Dat_Index = 0;
		else if(Sys_CB_H.TVG_Dat_Index >= 1)
			Sys_CB_H.TVG_Dat_Index++;
	}
	else if(htim==&htim7)  //计时
	{	
		//开启测深，则开始计时
		if(((Sys_CB.Survey_Status.channel_one)||(Sys_CB.Survey_Status.channel_two))&& (Sys_CB.Time.reg_time > Sys_CB.Time.used_time))
		{
			if(usedtime_counter++ == 60)				// 10s*60 = 600s = 10min
			{
				usedtime_counter = 0;
				Sys_CB.Time.Cur_Counter_10Min++;
				Sys_CB.Time.used_time = Sys_CB.Time.Cur_Counter_10Min/6;
				rt_sem_release(&Used_time_sem);
			}
		}
	}
}
/**
* @brief This function handles TIM4 global interrupt.
*/
void TIM4_IRQHandler(void)
{
	/* USER CODE BEGIN TIM4_IRQn 0 */

	/* USER CODE END TIM4_IRQn 0 */
	HAL_TIM_IRQHandler(&htim4);
	/* USER CODE BEGIN TIM4_IRQn 1 */

	/* USER CODE END TIM4_IRQn 1 */
}
/**
* @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
*/
void TIM6_DAC_IRQHandler(void)
{
	/* USER CODE BEGIN TIM6_DAC_IRQn 0 */

	/* USER CODE END TIM6_DAC_IRQn 0 */
	HAL_TIM_IRQHandler(&htim6);
	/* USER CODE BEGIN TIM6_DAC_IRQn 1 */

	/* USER CODE END TIM6_DAC_IRQn 1 */
}
/**
* @brief This function handles TIM7 global interrupt.
*/
void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim7);
}
