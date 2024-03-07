#ifndef __DYNAMICARRAY_H_
#define __DYNAMICARRAY_H_

#define ElementType void * //魔塔是void*
struct DynamicArray
{
    ElementType * dp; //动态数组指针
    int size;         //堆上申请的单位空间的个数
    int len;          //元素的个数
};


typedef struct DynamicArray DMArray;

int ArrayInit(DMArray *array);
void FreeArray(DMArray *array);
int ReallocArray(DMArray *array);
int InsertArray(DMArray *array,ElementType element);//ElementType element是要插入的数据

#endif