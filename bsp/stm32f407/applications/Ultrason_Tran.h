#ifndef  ULTRASON_TRAN_H
#define  ULTRASON_TRAN_H

extern uint8_t Power_ADC_Sta;

void MX_GPIO_Init(void);
void L_Freq_Charge(void);
void H_Freq_Charge(void);
void Ultrason_Trans(void);
void Get_PowerDcv(void);

void Uart2_DatOut(void);

#endif
