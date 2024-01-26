#ifndef __CIR_QUEUE_H__
#define __CIR_QUEUE_H__

#include <stdint.h>

#define QueueSize 500

typedef char datatype;

//���е�����Ԫ��  
typedef struct  
{  
     int front;  
     int rear;  
     int count;  //��������������¼Ԫ�ظ���  
     datatype data[QueueSize]; //��������  
}cirqueue;

//�ÿն�  
void InitQueue(cirqueue *q);
//�ж϶ӿ�  
uint8_t QueueEmpty(cirqueue *q);
//���,������ʧ�ܣ����ط�0ֵ  
uint8_t EnCirQueue(cirqueue *q, datatype x);
//����,ͨ��*pDat�������ݣ���������Ч�����ط�0ֵ
uint8_t DeCirQueue(cirqueue *q,datatype *pDat);

#endif
