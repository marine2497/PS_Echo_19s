#include "Cir_Queue.h"
#include <stdint.h>
#include <stdlib.h>
//置空队  
void InitQueue(cirqueue *q)  
{  
     q->front = q->rear = 0;  
     q->count = 0;  
}  

//判断队满  
uint8_t QueueFull(cirqueue *q)  
{  
     return (q->count == QueueSize);  
}  

//判断队空  
uint8_t QueueEmpty(cirqueue *q)  
{  
     return (q->count == 0);  
}  

//入队,若操作失败，返回非0值  
uint8_t EnCirQueue(cirqueue *q, datatype x)  
{  
	if(QueueFull(q)) //q满，终止程序  
		return 1;
	else
	{
		q->count++;  
		q->data[q->rear] = x;  
		q->rear = (q->rear + 1)%QueueSize; //循环队列设计，防止内存浪费 
		return 0;
	}		 
}  

//出队,通过*pDat传出数据，若数据无效，返回非0值
uint8_t DeCirQueue(cirqueue *q,datatype *pDat)  
{  
	if(QueueEmpty(q))		//q空，则终止程序，打印错误信息 
		return 1;
	else
	{
		*pDat = q->data[q->front];  
		q->count--;  
		q->front = (q->front + 1)%QueueSize;  
		return 0; 
	}		
}  

