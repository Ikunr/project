#include "MyString.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

void Print(MyString *obj);
int  IsEqual(MyString*obj1,MyString*obj2);
int IsContains(MyString *dest,MyString *src);
int Size(MyString *obj);
void RemoveString(MyString*dest,const char *str);  //前置声明，编译器从上往下翻译，防止编译器找不到报错
void InsertString(MyString *dest,const char*str,int index);


void Initialize(MyString *obj, const char *str)  //初始化结构体
{
       //将不可修改值的常量指针里面的参数str拷贝参数到结构体的string里面去
    obj->size = strlen(str);   //用strlen计算字符串长度给size赋值
    
    //new note
    obj->string = (char *)malloc(sizeof(char) * (obj->size+1));
    strcpy(obj->string,str);
    if(obj->string == NULL)
    {
        printf("MyString malloc error!\n");
        return;
    }
    
    obj->print = Print;
    obj->IsContains = IsContains;
    obj->isEqual = IsEqual;
    obj->StrinSize = Size;
    obj->insertString = InsertString;
    obj->removeString = RemoveString;
}

void FreeMyString(MyString *dest)
{
    if(dest->string != NULL)
    {
        free(dest->string);
        dest->string = NULL;
    }
}

void Print(MyString *obj)
{
    printf("%s\n",obj->string);
}

int IsEqual(MyString *obj1, MyString *obj2)
{
    if(strcmp(obj1 -> string,obj2 -> string) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int IsContains(MyString *dest, MyString *src)
{
    char *str = strstr(dest -> string,src -> string);
    if(str == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;
}

int Size(MyString *obj)
{  
    return obj->size;
}

void RemoveString(MyString *dest, const char *str)
{
    char* RMstr = strstr(dest->string,str);
    if(RMstr == NULL)
    {
        return;
    }
    else
    {
        char* destination = RMstr + strlen(str);
        while(*destination != '\0')
        {
            *RMstr = *destination;
            RMstr++;
            destination++;
        }
        *RMstr = '\0';
    }
}

void InsertString(MyString *dest, const char*str, int index)
{
    if(index < 0 || index > dest -> size)
    {
        return;
    }

    int new_str_len = dest->size + strlen(str) + 1;
    char *new_str = (char*)malloc(new_str_len);
    if(new_str == NULL)
    {
        printf("insertsString malloc error!\n");
    }

    strncpy(new_str,dest -> string,index);
    strncpy(new_str+index,str,strlen(str));
    strcpy(new_str+index+strlen(str),dest -> string+index); 

    free(dest->string);
    dest->string = new_str;
    dest->size = strlen(dest->string);
}
