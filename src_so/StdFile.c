#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StdFile.h"

#define true 1
#define false 0

int IsFileExist(const char *FilePath) // 是否存在
{
    FILE *file = fopen(FilePath, "r");
    if (file == NULL) // 打不开 失败
    {
        return false;
    }
    else
    {
        return true;
    }
    fclose(file);
}

int IsFileExist1(char *file, char *childfile) // 是否存在
{
    char temp[100];
    sprintf(temp, "./filetransfer/%s/%s", file, childfile);
    FILE *fp = fopen(temp, "r");
    printf("temp : %s\n", temp);
    if (fp == NULL)
    {
        printf("文件不存在\n");
        return 0;
    }

    if (fp == NULL) // 打不开 失败
    {
        return false;
    }
    else
    {
        return true;
    }
    fclose(fp);
}

char *LoadFromFile(const char *FilePath)
{
    FILE *file = fopen(FilePath, "r");
    if (file == NULL)
    {
        printf("loadFromFile open file error\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);  // 将光标指向文本文档末尾
    size_t size = ftell(file); // 一个字符占一个字节，故此光标位置即整个文本内的字节数

    char *str = (char *)malloc(size + 1); // 开空间加一给'\0'
    if (str == NULL)
    {
        printf("loadFromFile malloc error\n");
        return NULL;
    }

    fseek(file, 0, SEEK_SET); // 将光标指向开头
    fread(str, size, 1, file);
    str[size] = '\0'; // 给结尾手动填上‘\0’

    fclose(file);
    return str;
}

void RemoveCharIndex(char *a, int index) // weizhishanchu
{
    if (index < 0 || index >= strlen(a)) // shuzumanle
    {
        printf("InsertIndex invalid place!\n");
        return;
    }
    for (int i = index; i < strlen(a); i++)
    {
        a[i] = a[i + 1];
    }
}

void RemoveChar(char *a, char element) // an zhi shanchu
{
    int len = strlen(a);
    for (int i = 0; i < len; i++)
    {
        if (a[i] == element)
        {
            RemoveCharIndex(a, i);
            i--; // xianchu shandiao
        }
    }
}

DLlist *GetLineFromFile(const char *FilePath)
{
    FILE *file = fopen(FilePath, "r"); // 最小权限操作 需要多少操作多少
    if (file == NULL)
    {
        printf("file %s open fail!\n", FilePath); // 传的路径有问题
        return NULL;
    }

    DLlist *list = (DLlist *)malloc(sizeof(DLlist)); // 创一个链表
    if (list == NULL)
    {
        printf("GetLineFromFile malloc error!\n");
        return NULL;
    }
    InitDLlist(list); // 初始化

    char ContentTemp[100] = {0};                  // 省空间不知道字符串多长 所以先把文件都考进来 然后strlen +1 个是\N为了节省空间
    while (fgets(ContentTemp, 100, file) != NULL) // 有返回值的
    {

        char *Line = (char *)malloc(strlen(ContentTemp) + 1); // 之前的是局部变量所以要重新申请
        strcpy(Line, ContentTemp);                            // 拷贝过来 这样每一个字符串都可以往里面插了
        RemoveChar(Line, '\n');
        InsertTail(list, Line);
    }
    fclose(file);
    return list;
}

void WriteToFile(const char *FilePath, void *ptr, size_t size) // 输入字符串  字节大小
// 1、如果文件不存在就创建 2、对如果文件已经存在要不要清空
{
    FILE *file = fopen(FilePath, "w");
    if (file == NULL)
    {
        perror("file ");
        printf(" WriteToFile open file error!\n");
        return;
    }
    if (fwrite(ptr, size, 1, file) <= 0) // 返回值是写进去的内存大小   <=0说明写进去有问题
    {
        printf("WriteToFile error!\n");
    }
    fclose(file);
}

void WritrLineToFile(const char *FilePath, DLlist *list) // 覆盖掉重新写
{
    FILE *file = fopen(FilePath, "w");
    if (file == NULL)
    {
        printf("WritrLineToFile open file error!\n");
        return;
    }

    struct Node *TravelPointList = list->head; // 遍历
    while (TravelPointList != NULL)
    {
        fputs(TravelPointList->data, file); // 结构体一个字节一个字节塞进去  最后要整体读出来 ‘\0’ 结束
        fputc('\n', file);
        TravelPointList = TravelPointList->next;
    }

    fclose(file);
}

void AppendToFile(const char *FilePath, void *ptr, size_t size) // void *ptr 字符串
{
    FILE *file = fopen(FilePath, "a"); // 打开文件
    if (file == NULL)
    {
        printf("AppendToFile open file error!\n");
        return;
    }
    fwrite(ptr, size, 1, file); // 打开成功就追加写
    fclose(file);
}

void CopyFile(const char *SourcePath, const char *TargetPath)
{
    if (IsFileExist(SourcePath) == false) // 调用是否存在 打开失败
    {
        printf("the sourcefile is not exist or has no read permission"); // 打印权限不够
        return;
    }
    char *fileContent = LoadFromFile(SourcePath); // 把原文件全部拷出来

    if (IsFileExist(TargetPath) == true) // 如果新文件已经存在  要考虑改名
    {
        char Target[100] = {0};
        strcpy(Target, TargetPath);           // 新路径拷给他不能动
        char *fileName = strtok(Target, "."); // 按点切 切的是前面比如  name.txt   filename = name
        char *backName = strtok(NULL, ".");   // 按点切  切的是后面    backname = txt

        char NewPath[100] = {0};   // 找一个新的
        strcpy(NewPath, fileName); // 先放前缀 把name放进新的   此时 name
        strcat(NewPath, "_new.");  // 再把_new 拼接给他       此时 name _new
        strcat(NewPath, backName); // 再把原来的拼接给他       此时 name_new.txt
        if (IsFileExist(NewPath) == true)
        {
            CopyFile(SourcePath, NewPath); // 如果拼出来还有问题递归  老的不变 新的
            return;
        }
        WriteToFile(NewPath, fileContent, strlen(fileContent));
        free(fileContent);
        return;
    }

    WriteToFile(TargetPath, fileContent, strlen(fileContent)); // 写在内存里面
    free(fileContent);
}
