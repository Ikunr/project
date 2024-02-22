#include <stdio.h>
#include <stdlib.h>
#include "DoubleLinkList.h"
#define true 1
#define false 0

int InitDLlist(DLlist *list) // 初始化 把里面的东西赋值
{
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    return 0;
}

struct Node *CreateNode(ElementType element)
{
    struct Node *newnode = (struct Node *)malloc(sizeof(struct Node));
    if (newnode == NULL)
    {
        printf(" CreateNode malloc error!\n");
        return NULL;
    }
    newnode->next = NULL;
    newnode->prev = NULL;
    newnode->data = element;
    return newnode;
}

void InsertTail(DLlist *list, ElementType element) // 尾插
{
    struct Node *newnode = CreateNode(element); // 调用上面的创造节点函数
    if (newnode == NULL)                        // 判断是否为空
    {
        printf("InsertTail create error!\n");
        return;
    }
    if (list->len == 0) // 先判断如果里面什么东西都没有
    {
        list->head = newnode; // 将头指向新的节点
        list->tail = newnode; // 将尾指向新的节点
    }
    else // 如果里面有元素
    {
        list->tail->next = newnode; // 将尾指针指向新的节点
        newnode->prev = list->tail; // 新节点的前驱指向尾指针的位置
        list->tail = newnode;       // 将尾指针赋值为新的节点
    }
    list->len++; // 总体长度+1

} // 找不到前面所以单链表需要头结点  找到前面的那一个
void InsertHead(DLlist *list, ElementType element) // 头插
{
    struct Node *newnode = CreateNode(element); // 之前相同步骤
    if (newnode == NULL)
    {
        printf("InsertTail create error!\n");
        return;
    }
    if (list->len == 0) // 相同 判断是否有元素如果没有直接插
    {
        list->head = newnode;
        list->tail = newnode;
    }
    else // 如果有元素执行以下程序
    {
        list->head->prev = newnode; // 将头指针的前驱指向新的节点
        newnode->next = list->head; // 将新节点的指针指向 头节点的地址
        list->head = newnode;       // 将头结点赋给新的节点 实现新的节点变成头实现头插
    }
    list->len++;
}

int FindFirstByElement(DLlist *list, ElementType element)
{
    int count = 0;                         // 找个计数器
    struct Node *TravelPoint = list->head; // 遍历指针
    while (TravelPoint != NULL)
    {
        if (TravelPoint->data == element) // 当指针里面的数据等于我们要找的数据
        {
            return count;
        }
        count++;
        TravelPoint = TravelPoint->next;
    }
    return -1;
}

void RemoveByIndex(DLlist *list, int index)
{
    if (index < 0 || index >= list->len)
    {
        printf("RemoveByIndex invaild place!\n");
        return;
    }
    if (index == 0) // 当删除的元素前面为空时候
    {
        if (list->len == 1) // 当只有一个元素的时候
        {
            free(list->head);
            list->head = NULL; // 头结点置空
            list->tail = NULL; // 尾节点置空
            list->len = 0;
            return;
        }
        struct Node *freeNode = list->head;
        list->head = list->head->next; // 头结点往后动
        list->head->prev = NULL;       // 前驱节点指向空
        free(freeNode);
        list->len--;
        return;
    }

    if (index == list->len - 1)
    {
        struct Node *freeNode = list->tail;
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        free(freeNode);
        list->len--;
        return;
    }
    struct Node *TravelPoint = list->head;
    while (index > 0)
    {
        TravelPoint = TravelPoint->next;
        index--;
    }
    struct Node *PrevNode = TravelPoint->prev; // 前面的那个指针给到 t的前面那个
    struct Node *NextNode = TravelPoint->next; // 后面的那个指针   给到后面的
    PrevNode->next = TravelPoint->next;        //
    NextNode->prev = TravelPoint->prev;
    free(TravelPoint);
    list->len--;
}

void RemoveByElement(DLlist *list, ElementType element)
{
    int index = FindFirstByElement(list, element);
    while (index != -1)
    {
        RemoveByIndex(list, index);
        index = FindFirstByElement(list, element);
    }
}
// void Travel(DLlist *list) // 需要走俩步 从前往后 从后往前 保证代码不会出错
// {
//     printf("sizeof len %d \n",list->len);
//     printf("Next :");
//     struct Node *TravelPoint=list->head;  //从前往后走
//     while(TravelPoint != NULL)
//     {
//         printf("%d ",TravelPoint->data);
//         TravelPoint=TravelPoint->next;     //一步一步走
//     }
//     printf("\n");
//     printf("Prew :");
//     TravelPoint=list->tail;    //从后往前走
//     while(TravelPoint != NULL)
//     {
//         printf("%d ",TravelPoint->data);
//         TravelPoint=TravelPoint->prev;
//     }
//     printf("\n");
// }

void FreeDLlist(DLlist *list)
{
    while (list->head != NULL)
    {
        struct Node *freeNode = list->head;
        list->head = list->head->next;
        free(freeNode);
    }
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
}

int GetListLen(DLlist *list)
{
    return list->len;
}
