#include <sqlite3.h>
#include <stdlib.h>

#include <stdio.h>
#include "StdSqlite.h"
#include <string.h>
#define true 1
#define false 0

struct StdSqlite
{
    sqlite3 *db;
};

/* 打开数据库 */
SQL *initSqlite(const char *filename)
{
    SQL *s = (SQL *)malloc(sizeof(SQL));
    if (s == NULL)
    {
        printf("initSqlite malloc error!\n");
        return NULL;
    }

    if (sqlite3_open(filename, &s->db) != SQLITE_OK)
    {
        printf("open %s error : %s\n", filename, sqlite3_errmsg(s->db));
        free(s);
        return NULL;
    }
    return s;
}
/* 创表 */
void createTable(SQL *s, const char *tableName, char **prolist, int row)
{
    char porperty[1024] = {0};
    for (int i = 0; i < row; i++) // 字符串拼接 小于行数
    {
        strcat(porperty, prolist[i * 2]); // 第一个第一个  id
        strcat(porperty, " ");
        strcat(porperty, prolist[i * 2 + 1]); // 第一行第二个  int

        if (i != row - 1)
            strcat(porperty, ",");
    }
    char sql[4096] = {0};
    sprintf(sql, "create table if not exists  %s(%s);", tableName, porperty);
    // printf("sql : %s\n",sql);
   
    if (sqlite3_exec(s->db, sql, NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("create table error : %s\n", sqlite3_errmsg(s->db));
    }
}
/* 删除整张表 */
void deleteTable(SQL *s, const char *tableName)
{
    char sql[4096] = {0};
    sprintf(sql, "drop table %s;", tableName);

    if (sqlite3_exec(s->db, sql, NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("create table error : %s\n", sqlite3_errmsg(s->db));
    }
}
/* 插入数据 */
void insertData(SQL *s, const char *tableName, char **values, int size)
{
    char valueList[1024] = {0};
    for (int i = 0; i < size; i++)
    {
        strcat(valueList, "'");
        strcat(valueList, values[i]);
        strcat(valueList, "'");
        if (i != size - 1)
            strcat(valueList, ",");
    }
    // printf("%s\n",valueList);

    char sql[4096] = {0};
    sprintf(sql, "insert into %s values(%s);", tableName, valueList);
    printf("sql : %s\n",sql);
    if (sqlite3_exec(s->db, sql, NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("insert data table error : %s\n", sqlite3_errmsg(s->db));
    }
}
/* 更新数据库中数据 */
void updateData(SQL *s, const char *tableName, const char *SetValue, const char *where)
{
    char sql[4096] = {0};
    sprintf(sql, "update %s set %s where %s;", tableName, SetValue, where);
    // printf("update : %s\n",sql);

    if (sqlite3_exec(s->db, sql, NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("updata table error : %s\n", sqlite3_errmsg(s->db));
    }
}
/* 获取数据库中所有数据 */
void getTableInfo(SQL *s, const char *tableName, char ***result, int *row, int *column)
{
    char sql[4096] = {0};
    sprintf(sql, "select *from %s;", tableName);
    if (sqlite3_get_table(s->db, sql, result, row, column, NULL) != SQLITE_OK)
    {
        printf("getTableInfo error: %s", sqlite3_errmsg(s->db));
    }
}
/* 释放二维数组 */
void freeInfoResult(char **result)
{
    sqlite3_free_table(result);
}
/* 判断数据库中是否有数据 元组 */
int GetTableLen(SQL *s, const char *tableName)
{
    char sql[4096] = {0};
    sprintf(sql, "select count(*) from %s;", tableName);
  
    char **result;
    int row, column;

    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("GetTableLen error: %s", sqlite3_errmsg(s->db));
    }

    int len = atoi(result[column]);
    sqlite3_free_table(result);

    return len;
}
/* 释放数据库 */
void freeSqlite(SQL *s)
{
    if (s == NULL)
        return;
    sqlite3_close(s->db);
    free(s);
}
/* 获取表中所有数据 */
char * getTableData(SQL *s, const char *tableName, const char *where)
{

    char sql[4096] = {0};
    sprintf(sql, "select %s from %s;", tableName, where);
    char *message = (char *)malloc(sizeof(char) * 128);
    if (!message)
    {
        printf("malloc error\n");
        return NULL;
    }
    memset(message, 0, sizeof(message) * 128);
    char **result;
    int row, column;
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("getTableData: %s", sqlite3_errmsg(s->db));
    }

    for (int i = 1; i <= row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            /* 使用字符串拷贝函数 */
            strcat(message, result[i * column + j]);
            printf("%s|", result[i * column + j]);
        }
        printf("\n");
    }
    freeInfoResult(result);
    printf("mess:%s\n", message);
    return message;
}
/* 获取数据句柄 */
sqlite3 *getDB(SQL *s)
{
    return s->db;
}
/* 判断账号和密码是否都存在 */
int getTableAccountPasswdIsEql(SQL *s, const char *tableName, char *account, char *passwd)
{

    char sql[4096] = {0};
    sprintf(sql, "select * from %s where Account = '%s' and Passwd = '%s';", tableName, account, passwd);

    char **result;
    int row, column;
    
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    { 
        printf("getTableAccountPasswdIsEql error: %s", sqlite3_errmsg(s->db));
    }

    int len = row;
    freeInfoResult(result);
    return len;
}
/* 判断账号是否重复登录 */
int getTableAccountIsReplyLogin(SQL *s, const char *tableName, char *account)
{
    char sql[4096] = {0};
    sprintf(sql, "select * from %s where Account = '%s' and Online = '1';", tableName, account);

    char **result;
    int row, column;
    
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    { 
        printf("getTableAccountPasswdIsEql error: %s", sqlite3_errmsg(s->db));
    }

    int len = row;
    freeInfoResult(result);
    return len;
}
/* 判断账号是否存在 */                             
int getTableAccountIsExit(SQL *s, const char *tableName, char *account)
{

    char sql[4096] = {0};
    sprintf(sql, "select * from %s where Account = '%s';", tableName, account);

    char **result;
    int row, column;
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("getTableAccountIsExit error: %s", sqlite3_errmsg(s->db));
    }

    int len = row;
    freeInfoResult(result);
    return len;
}
/* 判断是否是好友 */
int getTableNameIsExit(SQL *s, const char *tableName, char *name, char *toname)
{

    char sql[4096] = {0};
    sprintf(sql, "select * from %s where user_Name = '%s' and friend_Name = '%s';", tableName, name, toname);
    // printf("sql : %s\n", sql);

    char **result;
    int row, column;
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("getTableNameIsExit error: %s", sqlite3_errmsg(s->db));
    }

    int len = row;

    return len;
}
int GetTablclentfriend(const char *filename, const char *tableName, char *name)
{
    SQL *sq = initSqlite(filename);
    char sql[4096] = {0};
    sprintf(sql, "select * from %s where FriendName = '%s';", tableName, name);

    char **result;
    int row, column;
    if (sqlite3_get_table(sq->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("GetTablclentfriend error: %s", sqlite3_errmsg(sq->db));
    }

    int len = row;
    free(sq);
    freeInfoResult(result);
    return len;
}
/* 获取表中名字 */
char *findTableGetName(SQL *s, const char *tableName, char *account, char *passwd)
{
    char sql[4096] = {0};
    sprintf(sql, "select name from %s where Account = '%s' and Passwd = '%s';", tableName, account, passwd);

    char **result;
    int row, column;
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("findTableGetName error: %s", sqlite3_errmsg(s->db));
    }

    if (row > 0)
    {
        // printf("%s\n",result[1]);
        return result[1];
    }
    else
    {
        return NULL;
    }
}
/* 判断群聊是否存在 */
int getTableEmpty(SQL *s, const char *chatName)
{
    char sql[4096] = {0};
    sprintf(sql, "select *from [Group] where 群聊名称 = '%s';", chatName);
    // printf("sql : %s\n", sql);
    char **result;
    int row, column;
    if (sqlite3_get_table(s->db, sql, &result, &row, &column, NULL) != SQLITE_OK)
    {
        printf("GetTableEMpty error: %s",sqlite3_errmsg(s->db));
        return 0;
    }

    return row;
}
