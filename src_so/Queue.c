#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

#define true 1
#define false 0

int InitQueue(LQueue *lq)
{
    return InitDLlist(&lq->queue);
}

void QPush(LQueue *lq, ElementType element) // 入队列 尾插
{
    InsertTail(&lq->queue, element);
}

ElementType *QPop(LQueue *lq)
{
    if (lq->queue.len == 0)
    {
        printf("Queue is Empty!\n");
        return NULL;
    }
    lq->FontData = lq->queue.head->data; //
    RemoveByIndex(&lq->queue, 0);
    return &lq->FontData;
}

struct Node *GerFront(LQueue *lq)
{
    return lq->queue.head;
}

int IsQEmty(LQueue *lq)
{
    if (lq->queue.len == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    return 0;
}

void FreeQueue(LQueue *lq)
{
    FreeDLlist(&lq->queue);
}

int GetQueueLen(LQueue *q) // 获取队列长度
{
    return q->queue.len;
}

