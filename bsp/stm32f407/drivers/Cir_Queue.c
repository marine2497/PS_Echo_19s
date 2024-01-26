#include "Cir_Queue.h"
#include <stdint.h>
#include <stdlib.h>
//�ÿն�  
void InitQueue(cirqueue *q)  
{  
     q->front = q->rear = 0;  
     q->count = 0;  
}  

//�ж϶���  
uint8_t QueueFull(cirqueue *q)  
{  
     return (q->count == QueueSize);  
}  

//�ж϶ӿ�  
uint8_t QueueEmpty(cirqueue *q)  
{  
     return (q->count == 0);  
}  

//���,������ʧ�ܣ����ط�0ֵ  
uint8_t EnCirQueue(cirqueue *q, datatype x)  
{  
	if(QueueFull(q)) //q������ֹ����  
		return 1;
	else
	{
		q->count++;  
		q->data[q->rear] = x;  
		q->rear = (q->rear + 1)%QueueSize; //ѭ��������ƣ���ֹ�ڴ��˷� 
		return 0;
	}		 
}  

//����,ͨ��*pDat�������ݣ���������Ч�����ط�0ֵ
uint8_t DeCirQueue(cirqueue *q,datatype *pDat)  
{  
	if(QueueEmpty(q))		//q�գ�����ֹ���򣬴�ӡ������Ϣ 
		return 1;
	else
	{
		*pDat = q->data[q->front];  
		q->count--;  
		q->front = (q->front + 1)%QueueSize;  
		return 0; 
	}		
}  

