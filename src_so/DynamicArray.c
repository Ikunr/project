#include<stdio.h>
#include<stdlib.h>
#include "DynamicArray.h"
#include "StdFile.h"
#include "DoubleLinkList.h"


#define ture 1
#define false 0

int ArrayInit(DMArray *array)  //数组的初始化
{
    //申请堆上的空间 size * 数据类型的大小
    array ->dp = (ElementType *)malloc(array ->size * sizeof(ElementType));
    //判断malloc申请是否成功
    if(array ->dp == NULL)
    {
        //申请失败后打印错误信息，返回false
        printf("Init error!");
        return false;
    }
    return ture;
}

void FreeArray(DMArray *array) //释放
{
    //判断指针不为空，因为释放空指针会出错
    if(array->dp != NULL)
    {
        free(array->dp);
        //放空指针之后要置为空
        array ->dp = NULL;
    }
}

int ReallocArray(DMArray *array) //扩容动态数组
{
    //保存原先内存的备份指针
    ElementType *temp = array ->dp;
    //给dp指针申请一块新的内存，大小为：数据类型*原先的空间个数*2
    array ->dp = (ElementType *)malloc(sizeof(ElementType) * array ->size * 2);
    //判断申请是否成功
    if(array ->dp ==NULL)
    {
    //申请失败后打印错误信息，返回false
        printf("ReallocArray Error!\n");
        return false;
    }
    //遍历原先的数组内存，将每一个元素都拷贝到新的内存中
    for(int i = 0; i < array->len;i++)
    {
        array ->dp[i] = temp[i];
    }
    //空间扩展到原先的2倍
    array ->size = array ->size * 2;
    //释放掉了原先的内存空间
    free(temp);
    return ture;
}

int InsertArray(DMArray *array, ElementType element)//插入数组元素
{
    //数组已满
   if(array ->len == array ->size)
    {
        //判断扩容是否失败
        if(ReallocArray(array) == false) 
        {
            //扩容失败打印错误信息，返回false
            printf("can not contain more elements!\n");
            return false;
        }
    }   

    //扩容成功或者数组没满，在数组的第len个位置附上我们的元素的值
    array ->dp[array ->len] = element;
    //计算数组长度的计数器加1
    array->len++;
    return  ture;
}


