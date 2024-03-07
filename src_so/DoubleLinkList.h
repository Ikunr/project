#ifndef __DOUBLELINKLIST_H_ // 双链表
#define __DOUBLELINKLIST_H_
#define ElementType void *

struct Node
{
    ElementType data;
    struct Node *next; // 后驱点
    struct Node *prev; // 前驱点
};

typedef struct DoubleLinkList
{
    struct Node *head;
    struct Node *tail; // 尾指针从尾巴找到头
    int len;
} DLlist;


int InitDLlist(DLlist *list);
void InsertTail(DLlist *list, ElementType element); // 对链表进行操作  在链表后面加节点
void InsertHead(DLlist *list, ElementType element);
// 按位插入 自己写
int FindFirstByElement(DLlist *list, ElementType element); // 找到下标
void RemoveByIndex(DLlist *list, int index);
void RemoveByElement(DLlist *list, ElementType element);
void Travel(DLlist *list);
void FreeDLlist(DLlist *list);
int GetListLen(DLlist *list);
// 双向 提供一个冒泡排序

#endif