#ifndef __DAC_H__
#define __DAC_H__

#include <rtthread.h>
#include "board.h"

#define	TVG_DAC_Num	20
extern uint16_t TVG_DAC1_Dat[TVG_DAC_Num];
extern uint16_t TVG_DAC2_Dat[TVG_DAC_Num];

extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_dac1;
extern DMA_HandleTypeDef hdma_dac2;
extern void TVG_Channel_One_Start(void);
extern void TVG_Channel_One_Stop(void);
extern void TVG_Channel_Two_Start(void);
extern void TVG_Channel_Two_Stop(void);
void MX_DAC_Init(void);

#endif
