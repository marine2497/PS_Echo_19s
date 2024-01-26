#ifndef __CIR_QUEUE_H__
#define __CIR_QUEUE_H__

#include <stdint.h>

#define QueueSize 500

typedef char datatype;

//队列的数据元素  
typedef struct  
{  
     int front;  
     int rear;  
     int count;  //计数器，用来记录元素个数  
     datatype data[QueueSize]; //数据内容  
}cirqueue;

//置空队  
void InitQueue(cirqueue *q);
//判断队空  
uint8_t QueueEmpty(cirqueue *q);
//入队,若操作失败，返回非0值  
uint8_t EnCirQueue(cirqueue *q, datatype x);
//出队,通过*pDat传出数据，若数据无效，返回非0值
uint8_t DeCirQueue(cirqueue *q,datatype *pDat);

#endif
