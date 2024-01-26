#include <rtthread.h>
#include "board.h"
#include "stm32f4xx.h"
#include "adc.h"
#include "stm32f4xx_hal.h"
#include "time.h"
#include "UART_Protocol.H"
#include "Ultrason_Tran.h"

/****************事件定义*******************/
struct rt_semaphore ADC1_CMP_sem;					// ADC1转换完成事件
struct rt_semaphore ADC2_CMP_sem;					// ADC2转换完成事件
struct rt_semaphore ADC3_CMP_sem;					// ADC3转换完成事件

uint8_t ADC1_Buf[ADC1_DMA_Num];			// 以100KHz的采样率，采集到100个数据刚好1ms,最长512ms

uint8_t ADC3_Buf[ADC3_DMA_Num];			// 以10KHz的采样率，采集到10个数据刚好1ms,最长512ms

ADC_HandleTypeDef hadc1;			// ADC句柄
DMA_HandleTypeDef  hdma_adc1;		// hdma_adc1;

ADC_HandleTypeDef hadc2;			// ADC句柄

ADC_HandleTypeDef hadc3;			// ADC句柄
DMA_HandleTypeDef  hdma_adc3;		// hdma_adc2;

//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	if(hadc==&hadc1)
	{
		/* USER CODE BEGIN ADC1_MspInit 0 */

		/* USER CODE END ADC1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();

		/**ADC1 GPIO Configuration    
		PC5     ------> ADC1_IN15 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* 配置 DMA Stream */
		hdma_adc1.Instance = DMA2_Stream0;
		hdma_adc1.Init.Channel=DMA_CHANNEL_0;                      //通道选择
		hdma_adc1.Init.Direction=DMA_PERIPH_TO_MEMORY;             //存储器到外设
		hdma_adc1.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
		hdma_adc1.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
		hdma_adc1.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
		hdma_adc1.Init.MemDataAlignment=DMA_PDATAALIGN_BYTE;       //存储器数据长度:8位
		hdma_adc1.Init.Mode=DMA_NORMAL;                            //非循环模式
		hdma_adc1.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
		hdma_adc1.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
		hdma_adc1.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      

		HAL_DMA_Init(&hdma_adc1);

		/* 连接DMA */
		__HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
	}
	else if(hadc->Instance==ADC2)
	{
		/* USER CODE BEGIN ADC2_MspInit 0 */

		/* USER CODE END ADC2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_ADC2_CLK_ENABLE();

		/**ADC2 GPIO Configuration    
		PA1     ------> ADC2_IN1  DCV-MON
		PC0     ------> ADC2_IN10 P-ADC-L
		PC1     ------> ADC2_IN11 P-ADC-H
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		/* ADC2 interrupt Init */
		HAL_NVIC_SetPriority(ADC_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(ADC_IRQn);
	}
	else if(hadc->Instance==ADC3)
	{
		/* Peripheral clock enable */
		__HAL_RCC_ADC3_CLK_ENABLE();

		/**ADC3 GPIO Configuration    
		PA3     ------> ADC3_IN3 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC3 DMA Init */
		/* ADC3 Init */
		/* 配置 DMA Stream */
		hdma_adc3.Instance = DMA2_Stream1;
		hdma_adc3.Init.Channel=DMA_CHANNEL_2;                      //通道选择
		hdma_adc3.Init.Direction=DMA_PERIPH_TO_MEMORY;             //存储器到外设
		hdma_adc3.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
		hdma_adc3.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
		hdma_adc3.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
		hdma_adc3.Init.MemDataAlignment=DMA_PDATAALIGN_BYTE;       //存储器数据长度:8位
		hdma_adc3.Init.Mode=DMA_NORMAL;                            //非循环模式
		hdma_adc3.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
		hdma_adc3.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
		hdma_adc3.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL; 
		
		HAL_DMA_Init(&hdma_adc3);
		
		__HAL_LINKDMA(hadc,DMA_Handle,hdma_adc3);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance==ADC1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_ADC1_CLK_DISABLE();

		/**ADC1 GPIO Configuration    
		PC5     ------> ADC1_IN15 
		*/
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);

		/* ADC1 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	else if(hadc->Instance==ADC2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_ADC2_CLK_DISABLE();

		/**ADC2 GPIO Configuration    
		PA1     ------> ADC2_IN1
		PB0     ------> ADC2_IN8
		PB1     ------> ADC2_IN9 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);
	}
	else if(hadc->Instance==ADC3)
	{
		/* Peripheral clock disable */
		__HAL_RCC_ADC3_CLK_DISABLE();

		/**ADC3 GPIO Configuration    
		PA3     ------> ADC3_IN3 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

		/* ADC3 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
}

/**********************************************************ADC1*/
//初始化ADC															   
void  Adc1_Init(void)
{    
	ADC_ChannelConfTypeDef sConfig;
	
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;			// 4分频，DCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
    hadc1.Init.Resolution = ADC_RESOLUTION_8B;						// 8位模式
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;						// 右对齐
    hadc1.Init.ScanConvMode = DISABLE;								// 禁用扫描模式
    hadc1.Init.ContinuousConvMode = DISABLE;						// 关闭连续转换
    hadc1.Init.DiscontinuousConvMode = DISABLE;						// 禁止不连续采样模式
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;						// 关闭EOC中断
    hadc1.Init.NbrOfConversion = 1;                   				// 1个转换在规则序列中 也就是只转换规则序列1 
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;     // TIM3触发
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;// 使用软件触发
    hadc1.Init.DMAContinuousRequests = ENABLE;						// 关闭DMA请求
    HAL_ADC_Init(&hadc1);											// 初始化 
	
	sConfig.Channel = ADC_CHANNEL_15;                        		//通道
    sConfig.Rank = 1;                                       
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;        			//采样时间
    sConfig.Offset = 0;                 
    HAL_ADC_ConfigChannel(&hadc1,&sConfig);        					//通道配置
}
/**********************************************************ADC2*/
//初始化ADC															   
void  Adc2_Init(void)
{  
	ADC_ChannelConfTypeDef sConfig;
	ADC_InjectionConfTypeDef sConfigInjected;

    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;			// 4分频，DCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
    hadc2.Init.Resolution = ADC_RESOLUTION_12B;						// 8位模式
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;						// 右对齐
    hadc2.Init.ScanConvMode = ENABLE;								// 禁用扫描模式
    hadc2.Init.ContinuousConvMode = DISABLE;						// 关闭连续转换
    hadc2.Init.DiscontinuousConvMode = DISABLE;						// 禁止不连续采样模式
    hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;						// 关闭EOC中断
    hadc2.Init.NbrOfConversion = 1;                   				// 1个转换在规则序列中 也就是只转换规则序列1 
    hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;     			// 非软件触发
    hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;// 使用软件触发
    hadc2.Init.DMAContinuousRequests = DISABLE;						// 关闭DMA请求
    HAL_ADC_Init(&hadc2);											// 初始化 
	
	/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
	*/
	sConfig.Channel = ADC_CHANNEL_VBAT;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    /**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time 
    */
	sConfigInjected.InjectedChannel = ADC_CHANNEL_10;
	sConfigInjected.InjectedRank = 1;
	sConfigInjected.InjectedNbrOfConversion = 3;
	sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_3CYCLES;
	sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING;
	sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
	sConfigInjected.AutoInjectedConv = DISABLE;
	sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
	sConfigInjected.InjectedOffset = 0;
	HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

	/**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time 
	*/
	sConfigInjected.InjectedChannel = ADC_CHANNEL_11;
	sConfigInjected.InjectedRank = 2;
	HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

	/**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time 
	*/
	sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
	sConfigInjected.InjectedRank = 3;
	HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);
	
	HAL_ADCEx_InjectedStart_IT(&hadc2);
}
/**********************************************************ADC3*/
//初始化ADC															   
void  Adc3_Init(void)
{ 
	ADC_ChannelConfTypeDef sConfig;	
	
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;			// 4分频，DCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
    hadc3.Init.Resolution = ADC_RESOLUTION_8B;						// 8位模式
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;						// 右对齐
    hadc3.Init.ScanConvMode = DISABLE;								// 禁用扫描模式
    hadc3.Init.ContinuousConvMode = DISABLE;						// 关闭连续转换
    hadc3.Init.DiscontinuousConvMode = DISABLE;						// 禁止不连续采样模式
    hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;						// 关闭EOC中断
    hadc3.Init.NbrOfConversion = 1;                   				// 1个转换在规则序列中 也就是只转换规则序列1 
    hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;     // 非软件触发
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;// 使用软件触发
    hadc3.Init.DMAContinuousRequests = ENABLE;						// 关闭DMA请求
    HAL_ADC_Init(&hadc3);											// 初始化 
	
	/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
	*/
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc3, &sConfig);	
}


/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hdma)		// DMA中断回调函数
{
//	HAL_TIM_Base_Stop(&htim4);			// 关闭TVG定时器
	if(hdma==&hadc1)
	{
		HAL_TIM_Base_Stop(&htim3); 		// 停止TIM3计数
//		if(Sys_CB.Grade.grade)			// 自动换挡，不需要发送
//		{
			rt_sem_release(&ADC1_CMP_sem);
//		}
	}
	else if(hdma==&hadc3)
	{
		HAL_TIM_Base_Stop(&htim2); 		// 停止TIM2计数
//		if(Sys_CB.Grade.grade)			// 自动换挡，不需要发送
//		{
			rt_sem_release(&ADC3_CMP_sem);
//		}
	}
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hdma)
{
	if(hdma==&hadc2)
	{
		HAL_ADCEx_InjectedStop_IT(&hadc2);
//		HAL_TIM_Base_Stop(&htim5);							// 关闭TIM5以停止ADC采样
		rt_sem_release(&ADC2_CMP_sem);
	}
}
/** 
  * Enable DMA controller clock
  */
void ADC_DMA_Init(void) 
{
	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	/* DMA2_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}
void DMA2_Stream0_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
	/* enter interrupt */
	rt_interrupt_enter();
	/* USER CODE END DMA2_Stream0_IRQn 0 */
	/* Transfer Complete Interrupt management ***********************************/
	HAL_DMA_IRQHandler(&hdma_adc1);
	/* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
	/* leave interrupt */
	rt_interrupt_leave();
	/* USER CODE END DMA2_Stream0_IRQn 1 */
}
void DMA2_Stream1_IRQHandler(void)
{
	
	/* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
	/* enter interrupt */
	rt_interrupt_enter();
	/* USER CODE END DMA2_Stream0_IRQn 0 */
	/* Transfer Complete Interrupt management ***********************************/
	HAL_DMA_IRQHandler(&hdma_adc3);
	
	/* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
	/* leave interrupt */
	rt_interrupt_leave();
	/* USER CODE END DMA2_Stream0_IRQn 1 */
}
/**
* @brief This function handles ADC1, ADC2 and ADC3 global interrupts.
*/
void ADC_IRQHandler(void)
{
	/* USER CODE BEGIN ADC_IRQn 0 */
	rt_interrupt_enter();
	/* USER CODE END ADC_IRQn 0 */
	HAL_ADC_IRQHandler(&hadc2);
	rt_interrupt_leave();
	/* USER CODE END ADC_IRQn 1 */
}
