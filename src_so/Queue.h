#ifndef __QUEUE_H_ // 队列
#define __QUEUE_H_
#include "DoubleLinkList.h"

struct LinkQueue // 链式队列 先进先出   尾巴进头出   链表：尾插头删
{
    DLlist queue;         //
    ElementType FontData; // 头元素 每次出去都是头
};

typedef struct LinkQueue LQueue;
int InitQueue(LQueue *lq);                   // 初始化
void QPush(LQueue *lq, ElementType element); // 入队列 尾插
ElementType *QPop(LQueue *lq);
int IsQEmty(LQueue *lq);
struct Node *GerFront(LQueue *lq);
void FreeQueue(LQueue *lq);
int GetQueueLen(LQueue *q); // 获取队列长度


#endif
