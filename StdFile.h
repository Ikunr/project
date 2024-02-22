#ifndef __STDFILE_H_
#define __STDFILE_H_
#include <stddef.h>
#include "DoubleLinkList.h"

int IsFileExist(const char *FilePath); // 是否存在

char *LoadFromFile(const char *FilePath);      // 文件全部读取出来放在字符串里面
DLlist *GetLineFromFile(const char *FilePath); // 按行输出我们的文本内容   账号密码字符串切割和这个函数
// 按行读   字符串连起来不知道每行多长  每行读完了确定完大小 需要链表指针
void WriteToFile(const char *FilePath, void *ptr, size_t size);  // 写在内存里面
void WritrLineToFile(const char *FilePath, DLlist *list);        // 一行一行的写
void AppendToFile(const char *FilePath, void *ptr, size_t size); // 追加函数
void CopyFile(const char *SourcePath, const char *TargetPath);   // 原路径 新目标路径拷贝文件
// 1.是否存在 打开失败不存在

int IsFileExist1(char *file, char *childfile);

#endif