#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <pthread.h>
#include <unistd.h>
#include "StdTcp.h"
#include "StdThread.h"
#include "DoubleLinkList.h"
#include "ThreadPool.h"
#include "StdSqlite.h"
#include "StdFile.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>

#define true 1
#define false 0
#define NAME_BUFFER_SIZE 20
#define CONTENT_BUFFER_SIZE 1024
#define BUFFER_SIZE 128
#define LOCAL_IPADDRESS "127.0.0.1"
#define SERVER_PORT 9090


enum OPERATION
{
    LOGIN = 1,
    REGISTER,
    PRIVATE_CHAT = 2,
    BROADCAST,
    GROUP_OPERATION,
    CREATE_GROUPS = 1,
    PULL_GROUP,
    GROUP_CHAT,
    ADD_FRIENDS = 5,
    DEL_FRIENDS,
    FILE_TRANSFER,
    VIEWONLINE_INFOMATION,
    USER_OFFLINE = 10, 
    INSERT_FRIEND,
    FIND_ONLINE_NUM = 1,
    FIND_ONLINE_FRIEND,
    AGREE_OR_REJEST_FILE_TRANSFER = 12,
    INSERT_GROUP,
};

enum SATATUSCODE 
{
    LOGIN_SUCCESS = 100,
    LOGIN_FAIL,   
    LOGIN_REPLAY,
    REGISTER_SUCCESS,
    REGISTER_FAIL,
    REGISTER_REPLAY,
    FRIEND_EXIST,
    ADD_FRIEND_SUCCESS,
    WHHETHER_ADD_FRIEND,
    NOT_FRIEND_ONLINE,
    FRIEND_ONLINE,
    NOT_GROUP,
    GROUP_OR_FRIEND_NOT_EXIST,
    PULL_GROUP_APPLICATION,
    AGREE_FRIEND = 101,
    AGREE_FILE_TRANSFER,
    AGREE_GROUP,
    NOT_FRIEND_OR_FILE,
};


typedef struct Message
{
    char toName[NAME_BUFFER_SIZE];
    char content[CONTENT_BUFFER_SIZE];
    char Account[NAME_BUFFER_SIZE];
    char Passwd[NAME_BUFFER_SIZE];
    char Name[NAME_BUFFER_SIZE];
    char fileName[NAME_BUFFER_SIZE];
    char Filecontent[CONTENT_BUFFER_SIZE];
    size_t fileSize;
    char IsOnline[NAME_BUFFER_SIZE];
    char Groupchat[NAME_BUFFER_SIZE];
    int flag;
    int tf;
    int loginflag2;
    int flag3;

} Msg;

/* 全局变量 */
char Name[NAME_BUFFER_SIZE]; 
char ToName[NAME_BUFFER_SIZE];
char Filecontent[CONTENT_BUFFER_SIZE];
size_t FileSize;
char FileName[CONTENT_BUFFER_SIZE];

typedef struct Client
{
    int sock;
    char name[NAME_BUFFER_SIZE];
} client;

typedef struct threadArg
{
    int acceptSock;
    SQL *sql;
    int epfd;

} threadArg;

client *CreateClient(int sock, const char *name)
{
    client *c = (client *)malloc(sizeof(client));
    if (c == NULL)
    {
        printf("CreateClient malloc error!\n");
        return NULL;
    }
    c->sock = sock;
    strcpy(c->name, name);
    return c;
}

void FreeClient(client *c)
{
    if (c != NULL)
    {
        free(c);
    }
}

DLlist list;


void *thread_handler(void *arg)
{
    /* 强转 */
    threadArg *thread = (threadArg *)arg;

    /* 取出结构体里面的数据 */
    int sock = (thread->acceptSock);
    SQL *s = (SQL *)thread->sql;
    sqlite3 * sqldb = getDB(s);
    int epfd = thread->epfd;
    /* 定义sql语句的 大小*/
    char sql[128] = {0};
    memset(sql, 0, sizeof(sql));

    pthread_detach(pthread_self());
    Msg msg;

    int readBytes = 0;
    while (1)
    {
        readBytes = TcpServerRecv(sock, &msg, sizeof(msg));
        // printf("msg  : %d\n", msg.flag);
        if (readBytes < 0)
        {
            if (errno == EAGAIN)
            {                                                                                                                               
                printf("read end ...\n");
                break;
            }
            else
            {   
                /* 将文件句柄从红黑树中删除掉 */
                epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
                printf("read error\n");
                break;                
            } 
        }
        else if (readBytes == 0)
        {
            printf("客户端下线\n");

            /* 将文件句柄从红黑树中删除掉 */
            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
            break;
        }  
        else
        {
            switch (msg.loginflag2)
            {
            case LOGIN:         /* 登录 */
                /* 如果账号密码都相同返回1 不相同返回0 */
                int count = getTableAccountPasswdIsEql(s, "person", msg.Account, msg.Passwd);
                /* 登录的是相同账号 */
                // if (count == 1 && getTableAccountIsReplyLogin(s, "person", msg.Account) == 1)
                // {
                //     /* 发送标记 */
                //     msg.tf = 2;
                //     TcpServerSend(sock,&msg,sizeof(msg));
                //     break;
                // }
                // printf("count : %d\n", count);
                /* 如果 !=0 说明匹配到了  */
                if (count != 0)
                {
                    msg.tf = LOGIN_SUCCESS; // 将标志为0 发过去

                    /* 获取表中的名字 */
                    char *temp = findTableGetName(s, "person", msg.Account, msg.Passwd); // 接口获取表中名字
                    /* 将名字拷贝过去 */
                    strcpy(msg.Name, temp);
                    printf("用户[%s] 已上线\n", msg.Name);
                    TcpServerSend(sock, &msg, sizeof(msg)); // 将结构体里的一起发过去

                    strcpy(msg.IsOnline, "1");
                    char online[20] = {0};

                    strcpy(online,"Online = '1'");
                    char where[40] = {0};
                    sprintf(where, "Account = '%s'", msg.Account);

                    /* 更新表中数据 将账号的在线更新为1 */
                    updateData(s, "person", online, where);
                }
                /* 匹配数据库失败表里没有这个人 */
                else
                {
                    msg.tf = LOGIN_FAIL;
                    TcpServerSend(sock, &msg, sizeof(msg)); // 将结构体里的一起发过去
                    break;
                }
                break;

            case REGISTER:
            /* 注册*/
                if (msg.loginflag2 == REGISTER)
                {
                    /* 判断账号是否存在 */
                    /* 不存在 */
                    if (getTableAccountIsExit(s, "person", msg.Account) != 0)
                    {
                        msg.tf = REGISTER_FAIL;
                        TcpServerSend(sock, &msg, sizeof(msg));
                    }
                    /* 存在插入数据库中 */
                    else if (getTableAccountIsExit(s, "person", msg.Account) == 0)
                    {
                        strcpy(msg.IsOnline, "1");
                        msg.tf = REGISTER_SUCCESS;
                        TcpServerSend(sock, &msg, sizeof(msg));

                        char *value[] = {msg.Name, msg.Account, msg.Passwd, msg.IsOnline};
                        /* 插入到person的表中 */
                        insertData(s, "person", value, sizeof(value) / sizeof(value[0]));
                    }
                }
                break;
            default:
                break;
            }

            switch (msg.flag)
            {
            case 1:
                InsertTail(&list, CreateClient(sock, msg.Name));
                break;
                /* 私聊 */
            case PRIVATE_CHAT:
                struct Node *travelPrivateChatPoint = list.head;

                while (travelPrivateChatPoint != NULL)
                {
                    client *c = (client *)travelPrivateChatPoint->data;
                    if (strcmp(c->name, msg.toName) == 0)
                    {
                        TcpServerSend(c->sock, &msg, sizeof(msg));
                        break;
                    }
                    travelPrivateChatPoint = travelPrivateChatPoint->next;
                }
                break;
                /* 广播 */
            case BROADCAST:
                struct Node *travelBroadcastPoint = list.head;
                while (travelBroadcastPoint != NULL)
                {
                    client *c = (client *)travelBroadcastPoint->data;
                    if (strcmp(c->name, msg.Name) != 0 && strcmp(c->name, msg.Name) != 0)
                    {
                        TcpServerSend(c->sock, &msg, sizeof(msg));
                    }
                    travelBroadcastPoint = travelBroadcastPoint->next;
                }
                break;
                /* 群聊操作 */
            case GROUP_OPERATION:
                char *errmsg = NULL;

                if (msg.flag3 == CREATE_GROUPS)         // 创群
                {
                    printf("用户[%s]创了一个群，群名称是%s\n", msg.Name, msg.Groupchat);
                    /* 创群 */
                    char *prolist1[] = {"群聊名称", "text", "群人员", "text"};
                    createTable(s, "[Group]", prolist1, sizeof(prolist1) / sizeof(prolist1[0]) / 2);
                    
                    /* 拉自己进群 */
                    memset(sql, 0, sizeof(sql));
                    sprintf(sql, "insert into [Group] values('%s','%s');", msg.Groupchat, msg.Name);
                    if (sqlite3_exec(sqldb, sql, NULL, NULL, &errmsg) != SQLITE_OK)
                    {
                        printf("sqlite3_exec delete %s\n", errmsg);
                    }
                }
                if (msg.flag3 == PULL_GROUP) // 拉人
                {
                    int qwe = 0;

                    printf("用户[%s]拉了[%s]进群，群名称是%s\n", msg.Name, msg.toName, msg.Groupchat);

                    int ret = getTableEmpty(s, msg.Groupchat);
                    // printf("ret : %d\n", ret);
                    
                    qwe = getTableNameIsExit(s, "Friend", msg.Name, msg.toName);
                    // printf("qwe : %d\n", qwe);

                    /* 群名不存在或者不是好友 */
                    if (ret == 0 || qwe == 0)
                    {
                        msg.tf = GROUP_OR_FRIEND_NOT_EXIST;                           // 标志位重新置
                        struct Node *travelGroupPoint = list.head; // 遍历发送
                        while (travelGroupPoint != NULL)
                        {
                            client *c = (client *)travelGroupPoint->data;
                            if (strcmp(c->name, msg.Name) == 0)
                            {
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelGroupPoint = travelGroupPoint->next;
                        }
                    }
                    else
                    {
                        msg.tf = PULL_GROUP_APPLICATION;                           // 标志位重新置
                        struct Node *travelGroupPoint = list.head; // 遍历发送
                        while (travelGroupPoint != NULL)
                        {
                            client *c = (client *)travelGroupPoint->data;
                            if (strcmp(c->name, msg.toName) == 0 && strcmp(c->name, msg.Name) != 0)
                            {
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelGroupPoint = travelGroupPoint->next;
                        }
                    }
                }
                if (msg.flag3 == GROUP_CHAT) /* 群聊*/
                {
                    char **result;
                    int row, column;
                    /* 清空脏数据 */
                    memset(sql, 0, sizeof(sql));
                    /* 看是否有群聊 */
                    sprintf(sql, "select *from [Group] where 群聊名称 = '%s';", msg.Groupchat);
                    if (sqlite3_get_table(sqldb, sql, &result, &row, &column, NULL) != SQLITE_OK)
                    {
                        printf("getTableNameIsExit error: %s", sqlite3_errmsg(sqldb));
                    }
                    /* 当row 为0时没有群聊 */
                    if (row == 0)
                    {
                        msg.tf = NOT_GROUP;
                        struct Node *travelPointGroup = list.head;

                        while (travelPointGroup != NULL)
                        {
                            client *c = (client *)travelPointGroup->data;
                            if (strcmp(c->name, msg.Name) == 0)
                            {
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelPointGroup = travelPointGroup->next;
                        }
                    }
                    /* 否则有群聊 */
                    else
                    {
                        printf("用户[%s]再群[%s]群发一条信息[%s]\n", msg.Name, msg.Groupchat, msg.content);
                        memset(sql, 0, sizeof(sql));
                        sprintf(sql, "select 群人员 from [Group] where 群聊名称 = '%s';", msg.Groupchat);
                        // printf("sql : %s\n", sql);
                        if (sqlite3_get_table(sqldb, sql, &result, &row, &column, NULL) != SQLITE_OK)
                        {
                            printf("getTableInfo error: %s", sqlite3_errmsg(sqldb));
                        }                
                        else
                        {
                            /* 遍历二维数组 */
                            for (int idx = 1; idx <= row; idx++) 
                            {
                                struct Node *travelGroupChatPoint = list.head; 
                                while (travelGroupChatPoint != NULL)          
                                {
                                    client *c = (client *)travelGroupChatPoint->data;
                                    if (strcmp(c->name, result[idx]) == 0 && strcmp(c->name, msg.Name) != 0)
                                    {
                                        printf("result : %s\n", result[idx]);
                                        TcpServerSend(c->sock, &msg, sizeof(msg));
                                        break;
                                    }
                                    travelGroupChatPoint = travelGroupChatPoint->next;
                                }
                            }

                        } 
                    } 
                }
                break;
                /* 添加好友 */
            case ADD_FRIENDS:
                printf("用户[%s]给用户[%s]发送一个好友申请\n", msg.Name, msg.toName);
                /* 如果是好友了 发送标志位 */
                if (getTableNameIsExit(s, "Friend", msg.Name, msg.toName) != 0) // 如果不是好友发信息告知
                {

                    msg.tf = FRIEND_EXIST;

                    TcpServerSend(sock, &msg, sizeof(msg));
                    memset(&msg, 0, sizeof(msg));
                    break;
                }

                struct Node *travelAddFriendPoint = list.head; // 遍历发送
                while (travelAddFriendPoint != NULL)
                {
                    client *c = (client *)travelAddFriendPoint->data;
                    if (strcmp(c->name, msg.toName) == 0 && strcmp(c->name, msg.Name) != 0)
                    {
                        msg.tf = WHHETHER_ADD_FRIEND; // 标志位重新置
                        TcpServerSend(c->sock, &msg, sizeof(msg));
                        memset(&msg, 0, sizeof(msg));

                        break;
                    }
                    travelAddFriendPoint = travelAddFriendPoint->next;
                }
                break;
                /* 删除好友 */
            case DEL_FRIENDS:
                sprintf(sql, "delete from Friend where user_Name = '%s' and friend_Name = '%s';", msg.Name, msg.toName);
                errmsg = NULL;
                if (sqlite3_exec(sqldb, sql, NULL, NULL, &errmsg) != SQLITE_OK)
                {
                    printf("sqlite3_exec delete %s\n", errmsg);
                }
            
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "delete from Friend where user_Name = '%s' and friend_Name = '%s';", msg.toName, msg.Name);
                if (sqlite3_exec(sqldb, sql, NULL, NULL, &errmsg) != SQLITE_OK)
                {
                    printf("sqlite3_exec delete %s\n", errmsg);
                }

                break;
                /* 文件传输 */
            case FILE_TRANSFER:
                printf("用户[%s]给用户[%s]发送一个文件传输\n", msg.Name, msg.toName);

                    int ret = 0;
                    ret = getTableNameIsExit(s, "Friend", msg.Name, msg.toName);

                    /* 群名不存在或者不是好友 */
                    int qwe = IsFileExist1(msg.Name, msg.fileName);
                    // printf("qwe : %d, ret : %d\n", qwe, ret);

                    if (ret == 0 || qwe == 0)
                    {
                        msg.tf = NOT_FRIEND_OR_FILE;                           // 标志位重新置
                        struct Node *travelGroupPoint = list.head; // 遍历发送
                        while (travelGroupPoint != NULL)
                        {
                            client *c = (client *)travelGroupPoint->data;
                            if (strcmp(c->name, msg.Name) == 0)
                            {
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelGroupPoint = travelGroupPoint->next;
                        }
                    }
                    else
                    {
                        char file[1024] = {0};
                        sprintf(file, "./filetransfer/%s/%s", msg.Name, msg.fileName);

                        char *temp = LoadFromFile(file);
                        strcpy(msg.Filecontent, temp); // 将读出来的内存赋值到结构体里定义的
                        msg.fileSize = strlen(temp);

                        struct Node *travelGroupPoint = list.head; // 遍历发送
                        while (travelGroupPoint != NULL)
                        {
                            client *c = (client *)travelGroupPoint->data;
                            if (strcmp(c->name, msg.toName) == 0)
                            {
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelGroupPoint = travelGroupPoint->next;
                        }
                    }
                break;
            case VIEWONLINE_INFOMATION:
                ret = 0; 
                if (msg.tf == FIND_ONLINE_NUM)
                {
                    char **result;
                    int row, column;
                    memset(sql, 0, sizeof(sql));
                    sprintf(sql, "select *from person where Online = '1';");
                    char *message = (char *)malloc(sizeof(char) * 128 * row);
                    if (!message)
                    {
                        printf("malloc error\n");
                        return NULL;
                    }
                    memset(message, 0, sizeof(char) * 128 * row);

                    row = 0;
                    /* 获取表中数据 */
                    if (sqlite3_get_table(sqldb, sql, &result, &row, &column, NULL) != SQLITE_OK)
                    {
                        printf("GetTableData: %s\n", errmsg);
                    }
                    memset(msg.content, 0, sizeof(msg.content));

                    /* 遍历查询结果只拿第一列 */
                    for (int idx = 1; idx <= row; idx++)
                    {
                        strcat(msg.content, result[idx * column]);
                        strcat(msg.content, "\n");
                        // printf("%s\n", result[idx * column]); // 打印每行的第一列值
                    }
                    /* 发送给对应的客户端 */
                    struct Node *travelPointOnline = list.head;

                    while (travelPointOnline != NULL)
                    {
                        client *c = (client *)travelPointOnline->data;
                        if (strcmp(c->name, msg.Name) == 0)
                        {
                            // printf("msg : %d %d\n", msg.flag, msg.tf);
                            TcpServerSend(c->sock, &msg, sizeof(msg));
                            break;
                        }
                        travelPointOnline = travelPointOnline->next;
                    }
                break;
                }
                /* 查看在线好友 */
                else if (msg.tf == FIND_ONLINE_FRIEND)
                {
                    char **result1;
                    char **result2;
                    int row1;
                    int row2;
                    int column1;
                    int column2; 

                    memset(sql, 0, sizeof(sql));                    
                    /* 将表中的Online1的取出来 */
                    sprintf(sql, "select *from person where Online = '1';");

                    if (sqlite3_get_table(sqldb, sql, &result1, &row1, &column1, &errmsg) != SQLITE_OK)
                    {
                        printf("getTableInfo error: %s", errmsg);
                    }

                    memset(sql, 0, sizeof(sql));
                    sprintf(sql, "select * from Friend where user_Name = '%s';", msg.Name);
                    // printf("where : %s\n", sql);
                    if(sqlite3_get_table(sqldb, sql, &result2, &row2, &column2, &errmsg) != SQLITE_OK)
                    {
                        printf("error : %s\n", errmsg);
                    }
#if 0
                    /* 打印测试 */
                    for(int i = 1;i <= row1;i++)
                    {
                        for(int j = 0;j < column1; j++)
                        {
                            // return (result1[i* column1 + j]);
                            printf("result1 : %s|",result1[i* column1 + j]);
                        }
                        printf("\n");

                    }

                    for(int i = 1;i <= row2;i++)
                    {
                        for(int j = 0;j < column2; j++)
                        {
                            // return (result1[i* column1 + j]);
                            printf("result2 : %s|",result2[i* column2 + j]);
                        }
                        printf("\n");

                    }
                    printf("\n");
#endif
                    char **result3 = (char **)malloc(row1 * sizeof(char *)); // 为result3分配内存
                    for (int idx = 0; idx < row1; idx++)
                    {
                        result3[idx] = (char *)malloc(31 * sizeof(char));
                    }
                    /* 比较result1 的第一列 和 result2的第二列 拿出相同的说明好友在线 */
                    int k = 0;
                    /* 遍历result1的行 */
                    for (int idx = 1; idx <= row1; idx++) { 
                        /* 遍历result2的行 */
                        for (int jdx = 1; jdx <= row2; jdx++) 
                        { 
                            // 比较result1的第一列与result2的第二列
                            if (strcmp(result1[idx * column1], result2[jdx * column2 + 1]) == 0) 
                            {   
                                /* 如果相同，则保存到result3中 */
                                strcpy(result3[k], result1[idx * column1]);
                                k++;
                            }
                        }
                    }
                    if (k == 0)
                    {
                        printf("没有好友在线\n");
                        ret = 1;
                    }
                    else
                    {
                        for (int idx = 0; idx < k; idx++)
                        {
                            printf("好友%s在线\n", result3[idx]);
                            memset(msg.content, 0, sizeof(msg.content));
                            strcat(msg.content, result3[idx]);
                            strcat(msg.content, "\n");
                        }
                    }
                    // 释放result3数组的内存
                    for (int idx = 0; idx < row1; idx++)
                    {
                        free(result3[idx]);
                    }
                    free(result3);

                    if (ret == 1)
                    {
                        struct Node *travelPointOnlineFriend = list.head;
                        while (travelPointOnlineFriend != NULL)
                        {
                            client *c = (client *)travelPointOnlineFriend->data;
                            if (strcmp(c->name, msg.Name) == 0)
                            {
                                msg.tf = NOT_FRIEND_ONLINE;
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelPointOnlineFriend = travelPointOnlineFriend->next;
                        }
                    }
                    else if (ret == 0)
                    {
                        struct Node *travelPointOnlineFriend = list.head;
                        while (travelPointOnlineFriend != NULL)
                        {
                            client *c = (client *)travelPointOnlineFriend->data;
                            if (strcmp(c->name, msg.Name) == 0)
                            {
                                msg.tf = FRIEND_ONLINE;
                                TcpServerSend(c->sock, &msg, sizeof(msg));
                                break;
                            }
                            travelPointOnlineFriend = travelPointOnlineFriend->next;
                        }
                    }
                }
                break;

            case USER_OFFLINE:

                printf("用户[%s] 已下线\n", msg.Name);

                memset(sql, 0, sizeof(sql));
                sprintf(sql, "update person set Online = '0' where Account = '%s';", msg.Account);

                if (sqlite3_exec(sqldb, sql, NULL, NULL, &errmsg) != SQLITE_OK)
                {
                    printf("update Friend %s\n", errmsg);
                }

                /* 线程退出 */
                pthread_exit(NULL);
                /* 关闭通信句柄 */
                close(sock);
                msg.tf = 0;
                break;

            case INSERT_FRIEND:
                char *value1[] = {msg.Name, msg.toName};
                char *value2[] = {msg.toName, msg.Name};

                insertData(s, "friend", value1, sizeof(value1) / sizeof(value1[0]));
                insertData(s, "friend", value2, sizeof(value2) / sizeof(value2[0]));

                msg.tf = 0;
                break;
                
            case AGREE_OR_REJEST_FILE_TRANSFER:
                struct Node *travelFilePoint = list.head;
                while (travelFilePoint != NULL)
                {
                    client *c = (client *)travelFilePoint->data;
                    if (strcmp(c->name, msg.toName) == 0)
                    {
                        TcpServerSend(c->sock, &msg, sizeof(msg));
                        break;
                    }
                    travelFilePoint = travelFilePoint->next;
                }
                
            case INSERT_GROUP:
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "insert into [Group] values('%s','%s');", msg.Groupchat, msg.Name);
                if (sqlite3_exec(sqldb, sql, NULL, NULL, &errmsg) != SQLITE_OK)
                {
                    printf("sqlite3_exec delete %s\n", errmsg);
                }
                
                break;
            default:
                break;
            }
        }
        
    }

    /* 线程退出 */
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{

    threadArg arg;
    /* 清除脏数据 */
    memset(&arg, 0, sizeof(arg));

#if 0
    // if (argc != 3)
    // {
    //     printf("invalid nums!\n");
    //     return -1;
    // }
#endif

    InitDLlist(&list);

    /* 打开数据库 */
    arg.sql = initSqlite("person.db");
    char *prolistPerson[] = {"name", "text", "Account", "text", "Passwd", "text", "Online", "text"};

    /* 创建总人员表 */
    createTable(arg.sql, "person", prolistPerson, sizeof(prolistPerson) / sizeof(prolistPerson[0]) / 2);

    /* 创建好友表 */
    char *prolistFriend[] = {"user_Name", "text", "friend_Name", "text"};
    createTable(arg.sql, "Friend", prolistFriend, sizeof(prolistFriend) / sizeof(prolistFriend[0]) / 2);

    /* 初始化线程池 */
    ThreadP *threadpool = InitThreadPool(20, 10, 10);
    if (threadpool == NULL)
    {
        printf("server point is null!\n");
        return -1;
    }

    /* 初始化服务器 */
    // TcpS *tcpServer = InitTcpServer(argv[1], atoi(argv[2]));
    TcpS *tcpServer = InitTcpServer(" ", SERVER_PORT);

    if (tcpServer == NULL)
    {
        printf("InitTcpServer error!\n");
        return -1;
    }

    /* 创建epoll 红黑树 实例 */
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll create error");
        exit(-1);
    }
    int sockfd = getTcpSock(tcpServer);
    /* 将sockfd 添加到红黑树实例里面 */
    struct epoll_event event;
    /* 清除脏数据 */
    memset(&event, 0, sizeof(event));
    event.data.fd = sockfd;
    event.events = EPOLLIN;     // 读事件
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if (ret == -1)
    {
        perror("epoll ctl error");
        exit(-1);
    }

    /* 监听的数量 */
    int nums = 0;

    while (1)
    {
        struct epoll_event events[BUFFER_SIZE];
        /* 清除脏数据 */
        memset(events, 0, sizeof(events));

        nums = epoll_wait(epfd, events, BUFFER_SIZE, -1);
        if (nums == -1)
        {
            perror("epoll wait error");
            exit(-1);
        }  

        /* 
            程序执行到这个地方有两种情况:
                1. 超时
                2. 有监听数据来了
        */
        for (int idx = 0; idx < nums; idx++)
        {
            int fd = events[idx].data.fd;
            if (fd == sockfd)
            {
                /* 有连接 */
                int connfd = accept(sockfd, NULL, NULL);
                if (connfd == -1)
                {
                    perror("perror error");
                    exit(-1);
                }

                /* 将通信句柄fd 设置成非阻塞模式 */
                int flag = fcntl(connfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(connfd, F_SETFL, flag);


                struct epoll_event conn_event;
                memset(&conn_event, 0, sizeof(conn_event));
                conn_event.data.fd = connfd;
                /* 将默认的水平触发模式 改成 边沿触发模式(高低电平) */
                conn_event.events = EPOLLIN | EPOLLET;

                /* 将通信的句柄 添加到树结点中 */
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &conn_event);
                if (ret == -1)
                {
                    perror("epoll_ctl error");
                    continue;
                }
            }
            else
            {
                // int connfd = events[idx].data.fd;
                arg.acceptSock = fd;
                printf("client : %d\n", arg.acceptSock);
                arg.epfd = epfd;
                /* 结构体赋值 */
                ThreadP_AddTask(threadpool, thread_handler, (void *)&arg);
            }
        }
    }
    /* 释放数据库 */
    freeSqlite(arg.sql);

    return 0;
    
}
