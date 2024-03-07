#ifndef __STDSQLITE_H_
#define __STDSQLITE_H_

#include <sqlite3.h>

struct StdSqlite;

typedef struct StdSqlite SQL;

/* 打开数据库 */
SQL *initSqlite(const char *filename);
/* 创表 */
void createTable(SQL *s, const char *tableName, char **prolist, int row);
/* 删除整张表 */
void deleteTable(SQL *s, const char *tableName);                          
/* 插入数据 */
void insertData(SQL *s, const char *tableName, char **values, int size);
/* 判断账号是否重复登录 */
int getTableAccountIsReplyLogin(SQL *s, const char *tableName, char *account);
/* 更新数据库中数据 */
void updateData(SQL *s, const char *tableName, const char *SetValue, const char *where);
/* 获取数据库中所有数据 */
void getTableInfo(SQL *s, const char *tableName, char ***result, int *row, int *column);
/* 释放二维数组 */
void freeInfoResult(char **result);                                                    
/* 判断数据库中是否有数据 元组 */
int GetTableLen(SQL *s, const char *tableName);                                        
/* 释放数据库 */
void freeSqlite(SQL *s);
/* 获取表中所有数据 */
char * getTableData(SQL *s, const char *tableName, const char *where);
/* 获取数据句柄 */
sqlite3 *getDB(SQL *s);
/* 判断账号和密码是否都存在 */
int getTableAccountPasswdIsEql(SQL *s, const char *tableName, char *account, char *passwd); 
/* 获取表中名字 */
char *findTableGetName(SQL *s, const char *tableName, char *account, char *passwd);
/* 判断账号是否存在 */                             
int getTableAccountIsExit(SQL *s, const char *tableName, char *account); 
/* 判断是否是好友 */
int getTableNameIsExit(SQL *s, const char *tableName, char* name, char *toname);
/* 判断群聊是否存在 */
int getTableEmpty(SQL *s, const char *chatName);


int GetTablclentfriend(const char *filename, const char *tableName, char *name);


#endif