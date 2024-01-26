/****************************************Copyright (c)**************************************************
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Ultrason_Tran.C
**��   ��   ��: 
**����޸�����: 2018��9��20��
**��        ��: �źŷ���
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
#include "ADC.h"
#include "DAC.h"
#include "RTC.h"
#include "TIME.h"
#include "DEF.h"
#include "UART_Protocol.H"
#include "UART_Parameters.H"
#include "UART_DEVICE.H"
#include "Ultrason_rcv.H"
#include "Ultrason_Cal.H"
#include "Ultrason_tran.H"
#include "string.h"

void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = RX_TX_L_Pin; 			
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  			//�������
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

	HAL_GPIO_WritePin(RX_TX_L_Port,RX_TX_L_Pin,GPIO_PIN_RESET);	

//	GPIO_InitStruct.Pin = Pon_CTL_Pin; 			
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  			//�������
//	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

//	HAL_GPIO_WritePin(Pon_CTL_Port,Pon_CTL_Pin,GPIO_PIN_RESET);			// Ĭ�Ϲرյ�Դ

	GPIO_InitStruct.Pin = RX_TX_H_Pin; 			
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  			//�������
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);

	HAL_GPIO_WritePin(RX_TX_H_Port,RX_TX_H_Pin,GPIO_PIN_RESET);	
/**********************************��棬�ܽŸ���**********************************************/
//	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;		// ��棬�ܽŸ���
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

//	GPIO_InitStruct.Pin = GPIO_PIN_5;		// ��棬�ܽŸ���
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
/**********************************************/
	
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);
	
	
	GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin; // LED
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  			//�������
	HAL_GPIO_Init(LED_Port,&GPIO_InitStruct);
	HAL_GPIO_WritePin(LED_Port,LED1_Pin,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(LED_Port,LED2_Pin,GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = Beep_Pin; // LED
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  			//�������
	HAL_GPIO_Init(Beep_Port,&GPIO_InitStruct);
	HAL_GPIO_WritePin(Beep_Port,Beep_Pin,GPIO_PIN_RESET);	


	#ifdef Unmanned_ship //�ϵ��Զ�����
		Sys_CB.Survey_Status.channel_one = 0;
		Sys_CB.Survey_Status.channel_two = 1;
	#endif	
}


void Uart2_DatOut()
{
	char len = 0;	
	int temp = 0 ;
	float temp_f = 0.0;
// 1����̬����
//$PSIC,ESI,��Ƶˮ�����ݣ���Ƶˮ�����ݣ�ע��ʱ�䣬����ʱ�䣬��ǰ��ѹ��������λ*CRC
	if((Sys_CB.Survey_Status.channel_one==0)&&(Sys_CB.Survey_Status.channel_two==0))
	{
		rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
				
		len = 0;
		memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
		len += my_strncpy(UART1_TxBuf,"$PSIC,ESI,",strlen("$PSIC,ESI,"));	
		len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_L.Water_Deep);//��Ƶˮ��
		len += sprintf(&UART1_TxBuf[len], ",");	
		
		len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB_H.Water_Deep);//��Ƶˮ��
		len += sprintf(&UART1_TxBuf[len], ",");	
		
		len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.reg_time);//ע��ʱ��
		len += sprintf(&UART1_TxBuf[len], ",");	
		
		len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Time.used_time);	//ʹ��ʱ��
		len += sprintf(&UART1_TxBuf[len], ",");
		
		len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB.Voltage.voltage); //��ǰ��ѹ	
		len += sprintf(&UART1_TxBuf[len], ",");
		
		len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Grade.grade);		//������λ
		
		len += my_strncpy(&UART1_TxBuf[len],"*",strlen("*"));
		
		memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
		my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
		len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
		
		UART1_TxBuf[len++] = '\r';
		UART1_TxBuf[len++] = '\n';		
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
		rt_thread_delay(1);		
	}
// 2����������
//$PSIC,GNP,���ͣ�SN��PID��ע���룬��ˮ��ȣ�����*CRC
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$PSIC,GNP,",strlen("$PSIC,GNP,"));

	len += my_strncpy(&UART1_TxBuf[len],  Sys_CB.Device_Info->model, strlen(Sys_CB.Device_Info->model));//����
	len += sprintf(&UART1_TxBuf[len], ",");		
	len += my_strncpy(&UART1_TxBuf[len],  Sys_CB.Device_Info->sn, strlen(Sys_CB.Device_Info->sn));//�����
	len += sprintf(&UART1_TxBuf[len], ",");		
	len += my_strncpy(&UART1_TxBuf[len],  Pid, strlen(Pid));//pid
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	len += my_strncpy(&UART1_TxBuf[len],  Sys_CB.Regi.srt_regi,  36);// 
	len += sprintf(&UART1_TxBuf[len], ",");
	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB.Parameters.draft);	//��ˮ���
	len += sprintf(&UART1_TxBuf[len], ",");	
	len += Float_to_Str(&UART1_TxBuf[len], Sys_CB.Parameters.velocity);	//����	

	len += my_strncpy(&UART1_TxBuf[len],"*",strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
		
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
    rt_thread_delay(1);

// 3����Ƶ����
//$PSIC,HFP,��������,����ģʽ,��������,����ˮ��,����,TVG����,��ʼ����,�ز���ֵ,ʱ������,ǳˮ����*CRC
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$PSIC,HFP,",strlen("$PSIC,HFP,"));
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Survey_Status.channel_two); //��������
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Survey_Model.channel_two); //����ģʽ
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp_f = Sys_CB.Sounder.txwidth.channel_two;
	if(temp_f>0.6f)
		temp_f = 0.6;
	len += Float_to_Str(&UART1_TxBuf[len],  temp_f); 
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp_f = Sys_CB.Sounder.clearwidth.channel_two;
	if(temp_f>0.3f && temp_f<0.5f)
		temp_f = 0.3;
	else if(temp_f>0.5f && temp_f<0.8f)
		temp_f = 0.5;	
	else if(temp_f>0.8f && temp_f<1)
		temp_f = 0.8;	
	else if(temp_f>=1 && temp_f<=10)
		temp_f = (int)temp_f;
	else if(temp_f>10)
		temp_f = 10;		
	len += Float_to_Str(&UART1_TxBuf[len],  temp_f); 
	len += sprintf(&UART1_TxBuf[len], ",");		
	
	temp = Sys_CB.Sounder.power.channel_two.Power_Index;
	if(temp>3)
		temp = 3;	
	len += Long_to_Str(&UART1_TxBuf[len],  temp); 
	len += sprintf(&UART1_TxBuf[len], ",");		

	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.tvgs.channel_two); 
	len += sprintf(&UART1_TxBuf[len], ",");		
	
	temp = Sys_CB.Sounder.startgain.channel_two - Sys_CB.Sounder.startgain.channel_two%10;
	if(temp <0) temp = 0;
	len += Long_to_Str(&UART1_TxBuf[len],  temp); 
	len += sprintf(&UART1_TxBuf[len], ",");	

	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.threshold.channel_two); 
	len += sprintf(&UART1_TxBuf[len], ",");	

	temp_f = Sys_CB.Sounder.filter.channel_two;
	if(temp_f>0.5f)
		temp_f = 0.5;	
	len += Float_to_Str(&UART1_TxBuf[len],  temp_f); 
	len += sprintf(&UART1_TxBuf[len], ",");	

	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.shallowsurveymode.channel_two); 

	len += my_strncpy(&UART1_TxBuf[len],"*", strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
	
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);
rt_thread_delay(1);	
// 4����Ƶ����
//$PSIC,LFP,��������,����ģʽ,��������,����ˮ��,����,TVG����,��ʼ����,�ز���ֵ,ʱ������*CRC	
	rt_sem_take(&UART1TXCMP_sem, RT_WAITING_FOREVER);		// ��ͼ�����ź���
		
	len = 0;
	memset(UART1_TxBuf, 0, sizeof(UART1_TxBuf));
	len += my_strncpy(UART1_TxBuf,"$PSIC,LFP,",strlen("$PSIC,LFP,"));
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Survey_Status.channel_one); //��������
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Survey_Model.channel_one); //����ģʽ
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp_f = Sys_CB.Sounder.txwidth.channel_one;
	if(temp_f>0.6f)
		temp_f = 0.6;	
	len += Float_to_Str(&UART1_TxBuf[len],  temp_f); //��������
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp_f = Sys_CB.Sounder.clearwidth.channel_one;
	if(temp_f>0.3f && temp_f<0.5f)
		temp_f = 0.3;
	else if(temp_f>0.5f && temp_f<0.8f)
		temp_f = 0.5;	
	else if(temp_f>0.8f && temp_f<1)
		temp_f = 0.8;	
	else if(temp_f>=1 && temp_f<=10)
		temp_f = (int)temp_f;
	else if(temp_f>10)
		temp_f = 10;	
	len += Float_to_Str(&UART1_TxBuf[len],  temp_f); //����ˮ�� 
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp = Sys_CB.Sounder.power.channel_one.Power_Index;
	if(temp>3)
		temp = 3;	
	len += Long_to_Str(&UART1_TxBuf[len],  temp); //����
	len += sprintf(&UART1_TxBuf[len], ",");		

	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.tvgs.channel_one); //TVG����
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	temp = Sys_CB.Sounder.startgain.channel_one - Sys_CB.Sounder.startgain.channel_one%10;
	if(temp <0) temp = 0;	
	len += Long_to_Str(&UART1_TxBuf[len],  temp); 								//��ʼ����
	len += sprintf(&UART1_TxBuf[len], ",");	

	len += Long_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.threshold.channel_one); //�ز���ֵ,
	len += sprintf(&UART1_TxBuf[len], ",");	
	
	len += Float_to_Str(&UART1_TxBuf[len],  Sys_CB.Sounder.filter.channel_one); //ʱ������*

	len += my_strncpy(&UART1_TxBuf[len],"*",strlen("*"));
	
	memset(UART1_TxBuf_tmep, 0, sizeof(UART1_TxBuf_tmep));		
	my_strncpy(UART1_TxBuf_tmep,&UART1_TxBuf[1], len-1);
	len += sprintf(&UART1_TxBuf[len], "%02x", NMEACheckNum1(UART1_TxBuf_tmep));
		
	UART1_TxBuf[len++] = '\r';
	UART1_TxBuf[len++] = '\n';	
	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)UART1_TxBuf, len);	
}

void H_Freq_Charge(void)
{
	uint16_t ADC_Tmp;
	uint16_t WD_Count;

	// ͨ��,P-ADC-H
	HAL_ADCEx_InjectedStart_IT(&hadc2);
//	HAL_TIM_Base_Start(&htim5); 										// ����TIM5�Դ���ADC����;
	rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);						// ��ͼ�����ź���
	ADC_Tmp = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_2);	// ͨ��,P-ADC-H
	if(ADC_Tmp>Sys_CB.Sounder.power.channel_two.Power_ADC_Value[Sys_CB.Sounder.power.channel_two.Power_Index-1])// �����趨ֵ������Ҫ���
	{
		WD_Count = 10000;													//�500ms���ʱ��
		htim9.Instance->CCMR1 &= ~TIM_CCMR1_OC2M;
		htim9.Instance->CCMR1 |= TIM_OCMODE_PWM2<<8;							// ���
		do
		{
			HAL_ADCEx_InjectedStart_IT(&hadc2);
//			HAL_TIM_Base_Start(&htim5); 										// ����TIM5�Դ���ADC����;
			rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);						// ��ͼ�����ź���
			ADC_Tmp = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_2);		// ͨ��,P-ADC-H
		}while((ADC_Tmp>Sys_CB.Sounder.power.channel_two.Power_ADC_Value[Sys_CB.Sounder.power.channel_two.Power_Index-1])&&(WD_Count--));								// ָ����ѹ��С��0.29����������48V&
		htim9.Instance->CCMR1 &= ~TIM_CCMR1_OC2M;
		htim9.Instance->CCMR1 |= TIM_OCMODE_FORCED_ACTIVE<<8;					// ������
	}
}
void H_Freq_Trans(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
/*****************************************��Ƶ����*****************************************************************/
/*************************************TVG**********************************************/
	rt_thread_delay(1);	
	TVG_Channel_Two_Start();
	rt_thread_delay(1);
	
	Sys_CB_H.TVG_Dat_Index = 1;		//����TVG
/*************************************TVG**********************************************/
	//ͨ��,TO-ADC-H
	HAL_ADC_Start_DMA(&hadc1,(uint32_t* )ADC1_Buf,Sys_CB_H.UltraDat_Len); 	// ʹ��ָ����ADC1�����ת����������
	HAL_TIM_Base_Start(&htim3); 											// ����TIM3�Դ���ADC����
	htim1.Instance->CCER &= 0xfffa;
	htim1.Instance->CCER |= 0x0005;
	__HAL_TIM_MOE_ENABLE(&htim1);											// ������������
	HAL_TIM_Base_Start(&htim1); 		
	while(htim1.Instance->CR1&TIM_CR1_CEN);
	HAL_TIM_Base_Stop(&htim1); 												// �������
	__HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);

	GPIO_InitStruct.Pin = GPIO_PIN_8;										// ���ٷŵ�
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

	rt_sem_release(&HFreqTranCMP_sem);
}
void L_Freq_Charge(void)
{
	uint16_t ADC_Tmp;
	uint16_t WD_Count;
	
	HAL_ADCEx_InjectedStart_IT(&hadc2);
//	HAL_TIM_Base_Start(&htim5); 												// ����TIM5�Դ���ADC����;
	rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);								// ��ͼ�����ź���	
	ADC_Tmp = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_1);			// ͨ��,P-ADC-L
	if(ADC_Tmp>Sys_CB.Sounder.power.channel_one.Power_ADC_Value[Sys_CB.Sounder.power.channel_one.Power_Index-1])													// �����趨ֵ������Ҫ���
	{			
		WD_Count = 10000;															//�500ms���ʱ��
		htim9.Instance->CCMR1 &= ~TIM_CCMR1_OC1M;
		htim9.Instance->CCMR1 |= TIM_OCMODE_PWM2;								// ���
		do
		{
			HAL_ADCEx_InjectedStart_IT(&hadc2);
//			HAL_TIM_Base_Start(&htim5); 										// ����TIM5�Դ���ADC����;
			rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);						// ��ͼ�����ź���
			ADC_Tmp = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_1);	// ͨ��,P-ADC-L
		}while((ADC_Tmp>Sys_CB.Sounder.power.channel_one.Power_ADC_Value[Sys_CB.Sounder.power.channel_one.Power_Index-1])&&(WD_Count--));													// ָ����ѹ��С��0.29����������48V
		htim9.Instance->CCMR1 &= ~TIM_CCMR1_OC1M;
		htim9.Instance->CCMR1 |= TIM_OCMODE_FORCED_ACTIVE;;						// ������
	}
}
void L_Freq_Trans(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
/*****************************************��Ƶ����*****************************************************************/	
/*************************************TVG**********************************************/
	TVG_Channel_One_Start();
	rt_thread_delay(1);	
	
	Sys_CB_L.TVG_Dat_Index = 1; //����TVG
	///*************************************TVG**********************************************/
	//ͨ��,TO-ADC-L	
	HAL_ADC_Start_DMA(&hadc3,(uint32_t* )ADC3_Buf,Sys_CB_L.UltraDat_Len); 	// ʹ��ָ����ADC3�����ת����������
	HAL_TIM_Base_Start(&htim2);

	htim8.Instance->CCER &= 0xffaf;
	htim8.Instance->CCER |= 0x0050;
	__HAL_TIM_MOE_ENABLE(&htim8);											// ������������
	HAL_TIM_Base_Start(&htim8); 		
	while(htim8.Instance->CR1&TIM_CR1_CEN);
	HAL_TIM_Base_Stop(&htim8); 
	__HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim8);
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	rt_sem_release(&LFreqTranCMP_sem);
}

uint8_t DC_index = 0;
double DV_temp;
uint16_t DC_MIN, DC_MAX = 0;

void Ultrason_Trans(void)
{
	/* �����¼���� */
    rt_uint32_t evt;
	uint16_t DCV_MON;

	if(Sys_CB.Time.reg_time > Sys_CB.Time.used_time)
	{		
		if((Sys_CB.Survey_Status.channel_one)||(Sys_CB.Survey_Status.channel_two))
		{			
			if((Sys_CB.Survey_Status.channel_one)&&(Sys_CB.Survey_Status.channel_two))
			{
//				/* ���߼��ķ�ʽ�����¼� */
				L_Freq_Trans();
				L_Freq_Charge();
				H_Freq_Trans();
				H_Freq_Charge();
				rt_event_recv(&HL_Pro_CMP_event, ((1 << 0) | (1 << 1)), \
								  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, \
								  RT_WAITING_FOREVER, &evt);
			}
			else if(Sys_CB.Survey_Status.channel_one)
			{
				L_Freq_Trans();
				L_Freq_Charge();
				rt_event_recv(&HL_Pro_CMP_event, (1 << 0), \
								  RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, \
								  RT_WAITING_FOREVER, &evt);
			}
			else if(Sys_CB.Survey_Status.channel_two)
			{
				H_Freq_Trans();
				H_Freq_Charge();
				rt_event_recv(&HL_Pro_CMP_event, (1 << 1), \
								  RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, \
								  RT_WAITING_FOREVER, &evt);
			}		
		}
		else
		{
			
		}
	}
	
	HAL_ADCEx_InjectedStart_IT(&hadc2);
	rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);						// ��ͼ�����ź���
	DCV_MON = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_3);	// ͨ��,DCV
	DV_temp += DCV_MON;
	DC_index = DC_index + 1;
	if(DC_index == 1)
	{
		DC_MAX = DCV_MON;
		DC_MIN = DCV_MON;		
	}
	if(DCV_MON>DC_MAX)
		DC_MAX = DCV_MON;
	else if(DCV_MON<DC_MIN)
		DC_MIN = DCV_MON;

	//printf("DCV_MON=%d-%.3f  ",DCV_MON, ((float)DCV_MON)/4096*2.5);
	if(DC_index>=102)
	{
		DC_index = 0;
		//DV_temp = DV_temp/10/256*2.5*11;										// �õ�����ѹ
		DV_temp = (DV_temp - DC_MAX - DC_MIN);
		//printf("\r\nDV_temp=%.3f ",DV_temp/18);			
		DV_temp = DV_temp/100;
		DV_temp = DV_temp/4096*2.5f;
		
//		Sys_CB.Voltage.voltage = DV_temp*1000;
		
		//����2  v = 26.32x^3 - 86.86x^2 + 108.7x - 36.75
//		Sys_CB.Voltage.voltage = 26.32*DV_temp*DV_temp*DV_temp - 86.86f*DV_temp*DV_temp + 108.7f*DV_temp - 37.05f;	

//		if(Sys_CB.Voltage.voltage<10.40f)
//			Sys_CB.Voltage.voltage += 0.25f;
		
		//�°��� 2020-1-5
//		if(DV_temp<=1.309f)
//		{
////       3         2
////10.82 x - 30.02 x + 39.26 x - 8.761
//			Sys_CB.Voltage.voltage = 10.82f*DV_temp*DV_temp*DV_temp - 30.02f*DV_temp*DV_temp + 39.26f*DV_temp - 8.761f;	
//			
//		}
//		else if(DV_temp>1.309f && DV_temp<=1.6f)
//		{
////       3        2
////8.835 x - 21.4 x + 26.58 x - 2.455
//			Sys_CB.Voltage.voltage = 8.835f*DV_temp*DV_temp*DV_temp - 21.4f*DV_temp*DV_temp + 26.58f*DV_temp - 2.455f;
//		}
//		else
//		{
////       3         2
////56.55 x - 253.5 x + 405.2 x - 209.6	
//			Sys_CB.Voltage.voltage = 56.55f*DV_temp*DV_temp*DV_temp  - 253.5f*DV_temp*DV_temp+ 405.2f*DV_temp - 209.6f;
//		}		


		//          3          2
		//-0.03797 x + 0.1742 x + 20.39 x + 0.1704
		Sys_CB.Voltage.voltage = 0.1742f*DV_temp*DV_temp -0.03797*DV_temp*DV_temp*DV_temp + 20.39f*DV_temp + 0.1704f;
//		Sys_CB.Voltage.voltage = 12.1;
        //20.70548434
		//Sys_CB.Voltage.voltage =  20.7055f*DV_temp;

	}

	rt_thread_delay(6);		
	//rt_thread_delay(7);

}


void Get_PowerDcv(void)
{
	uint16_t DCV_MON;

	for(DC_index=0; DC_index<12; DC_index++)
	{
		HAL_ADCEx_InjectedStart_IT(&hadc2);
		rt_sem_take(&ADC2_CMP_sem, RT_WAITING_FOREVER);						// ��ͼ�����ź���
		DCV_MON = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_3);	// ͨ��,DCV
		DV_temp += DCV_MON;
		if(DC_index == 0)
		{
			DC_MAX = DCV_MON;
			DC_MIN = DCV_MON;		
		}
		if(DCV_MON>DC_MAX)
			DC_MAX = DCV_MON;
		if(DCV_MON<DC_MIN)
			DC_MIN = DCV_MON;
		rt_thread_delay(2);		
	}

		DV_temp = (DV_temp - DC_MAX - DC_MIN);			
		DV_temp = DV_temp/10;
	    DV_temp = DV_temp/4096*2.5;
			
//		Sys_CB.Voltage.voltage = DV_temp*1000;
//		if(DV_temp<=1.309f)
//		{
////       3         2
////10.82 x - 30.02 x + 39.26 x - 8.761
//			Sys_CB.Voltage.voltage = 10.82f*DV_temp*DV_temp*DV_temp - 30.02f*DV_temp*DV_temp + 39.26f*DV_temp - 8.761f;	
//			
//		}
//		else if(DV_temp>1.309f && DV_temp<=1.6f)
//		{
////       3        2
////8.835 x - 21.4 x + 26.58 x - 2.455
//			Sys_CB.Voltage.voltage = 8.835f*DV_temp*DV_temp*DV_temp - 21.4f*DV_temp*DV_temp + 26.58f*DV_temp - 2.455f;
//		}
//		else
//		{
////       3         2
////56.55 x - 253.5 x + 405.2 x - 209.6	
//			Sys_CB.Voltage.voltage = 56.55f*DV_temp*DV_temp*DV_temp  - 253.5f*DV_temp*DV_temp+ 405.2f*DV_temp - 209.6f;
//		}	
		Sys_CB.Voltage.voltage = 0.1742f*DV_temp*DV_temp -0.03797*DV_temp*DV_temp*DV_temp + 20.39f*DV_temp + 0.1704f;
//        Sys_CB.Voltage.voltage = 12.1;
        //20.70548434
		//Sys_CB.Voltage.voltage =  20.7055f*DV_temp;		
}


