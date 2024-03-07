#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "StdTcp.h"
#include "StdThread.h"
#include "ThreadPool.h"
#include "StdSqlite.h"
#include <string.h>
#include "StdFile.h"
#include <pthread.h>
#include <time.h>
#include <linux/types.h>
#include <linux/time_types.h>
#include <linux/types.h>
#include <stddef.h>
#include "Map.h"
#include "batter.h"
#include "super.h"
#include "DynamicArray.h"
#include "MyString.h"

#define true 1
#define false 0

#define NAME_BUFFER_SIZE 20
#define CONTENT_BUFFER_SIZE 1024
#define BUFFER_SIZE 128
#define LOCAL_IPADDRESS "127.0.0.1"
#define SERVER_PORT 9090
char * SERVER_IP = "172.25.216.59";


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
    START_PLAY_GAME,
    USER_OFFLINE, 
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


pthread_mutex_t choice_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t scanf_mainCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t scanf_cond = PTHREAD_COND_INITIALIZER;

extern int startPlayGame();

void start()
{
    printf("==================================\n");
    printf("=          1、私发消息            =\n");
    printf("=          2、群发消息            =\n");
    printf("=          3、群操作              =\n");
    printf("=          4、添加好友            =\n");
    printf("=          5、删除好友            =\n");
    printf("=          6、传送文件            =\n");
    printf("=          7、查看好友信息        =\n");
    printf("=          8、进入魔塔小游戏~     =\n");
    printf("=          \033[31m9、退出账号\033[0m            =\n");
    printf("==================================\n");
}

Msg msg;

char Name[NAME_BUFFER_SIZE]; // 全局变量
char ToName[NAME_BUFFER_SIZE];
char groupName[NAME_BUFFER_SIZE];
char Filecontent[CONTENT_BUFFER_SIZE];
size_t FileSize;

void *thread_handler(void *arg)
{

    TcpC *c = (TcpC *)arg;
    while (1)
    {
        /* 接收信息 */
        TcpClientRecv(c, &msg, sizeof(msg));

        if (msg.flag == PRIVATE_CHAT)                       /* 私聊 */
        {
            printf("收到了一条[%s]给你发了信息  信息内容是: %s\n", msg.Name, msg.content);
        }
        if (msg.flag == BROADCAST)                          /* 广播 */
        {
            printf("收到了一条[%s]给你发了信息  信息内容是: %s\n", msg.Name, msg.content);
        }
        if (msg.flag == GROUP_OPERATION)                    /* 群聊操作 */
        {
            if (msg.flag3 == PULL_GROUP)
            {
                if (msg.tf == GROUP_OR_FRIEND_NOT_EXIST)
                {
                    printf("该群聊不存在或者不是好友 \n");
                }
                else if (msg.tf == PULL_GROUP_APPLICATION)
                {
                    printf("收到好友拉群申请 名字:%s\n", msg.Name);
                    printf("输入102进行操作\n");
                    strncpy(Name, msg.Name, sizeof(Name)); 
                    strncpy(groupName, msg.Groupchat, sizeof(msg.Groupchat));
                }
            }
            if (msg.flag3 == GROUP_CHAT)
            {
                if (msg.tf == NOT_GROUP)
                {
                    printf("该群不存在请重新输入\n");
                }
                else
                {
                    printf("您收到了来自[%s]群聊的信息,信息内容为 [%s]!!\n",msg.Groupchat, msg.content);

                }
            }
        }
        if (msg.flag == ADD_FRIENDS)                        /* 添加好友 */
        {
            if (msg.tf == FRIEND_EXIST)             
            {
                printf("你已经有该好友了！\n");
            }
            else if (msg.tf == WHHETHER_ADD_FRIEND)
            {
                printf("收到好友申请 名字:%s\n", msg.Name);
                printf("输入100进行操作\n");
                strncpy(Name, msg.Name, sizeof(Name));  
            }
#if 0   
                // strncmp(msg.content, msg.Name, sizeof(msg));
                // printf("name1: %s\n", msg.content);

                // printf("11、同意\n");
                // printf("22、拒绝\n");

                // scanf("%d", &choice);
                // while ((cc = getchar()) != EOF && cc != '\n');
                // printf("如确定请重新输入一遍\n");
                // if (choice == 11)
                // {
                //     msg.flag = INSERT_FRIEND;
                //     TcpClientSend(c, &msg, sizeof(msg));
                //     printf("你已经和%s 成为好友请开始你们的聊天把！\n", msg.Name);
                //     continue;
                // }
                // else if (choice == 22)
                // {
                //     printf("该用户拒绝了你的好友申请\n");
                // }
                // else
                // {
                //     printf("vaild input\n");
                // }
#endif
        }
        if (msg.flag == FILE_TRANSFER)                      /* 文件传输 */
        {
            if (msg.tf == NOT_FRIEND_OR_FILE)
            {
                printf("该文件不存在或者不是你的好友请重新输入\n");
            }
            else
            {
                printf("收到好友%s文件申请\n", msg.Name);
                printf("请按101进行操作\n");
                // strcpy(Name, msg.Name);
                strcpy(ToName, msg.Name);
                strcpy(Filecontent, msg.Filecontent);
                FileSize = msg.fileSize;
            }
            // printf("name : %s toname : %s fileconten :%s filesize : %ld \n",Name,ToName,Filecontent,FileSize);
        }
        if (msg.flag == VIEWONLINE_INFOMATION)              /* 查看在线信息 */
        {            
            if (msg.tf == FIND_ONLINE_NUM)
            {
                printf("%s\n", msg.content);
            }
            else if (msg.tf == NOT_FRIEND_ONLINE)
            {
                printf("没有好友在线\n");
            }
            else if (msg.tf == FRIEND_ONLINE)
            {
                printf("好友 |%s| 在线\n", msg.content);
            }
        }
        if (msg.flag == AGREE_OR_REJEST_FILE_TRANSFER)      /* 文件传输 */
        {
            if (msg.tf == AGREE_FILE_TRANSFER)
            {
                printf("你的好友%s已经同意了你的文件申请\n", msg.Name);
            }
            else
            {
                printf("对方拒绝了你的文件申请\n");
            }
        }
    }
}


int main(int argc, char const *argv[])
{
    Msg msg;

    SQL *s;

#if 0
    // if (argc != 3)
    // {
    //     printf("invalid nums!\n");
    //     return -1;
    // }
#endif

    TcpC *client = InitTcpClient(SERVER_IP, SERVER_PORT);

    if (client == NULL)
    {
        printf("InitTcpClient error!\n");
        return -1;
    }
    while (1)
    {

        printf("===================================\n");
        printf("=           欢迎来到MM聊天室        = \n");
        printf("===================================\n");
        printf("===================================\n");
        printf("=              1、登录             =\n");
        printf("=              2、注册             =\n");
        printf("===================================\n");
        printf("\n");
        /* 登录注册flag */
        scanf("%d", &msg.loginflag2); 
        while (getchar() != '\n');

        if (msg.loginflag2 == LOGIN)                    /* 登录 */
        {
            printf("请输入你的账号\n");
            scanf("%s", msg.Account);
            while (getchar() != '\n');

            printf("请输入你的密码\n");
            scanf("%s", msg.Passwd);
            while (getchar() != '\n');

            TcpClientSend(client, &msg, sizeof(msg)); // 把自己输得账号密码发过去
            msg.tf = 0;
            TcpClientRecv(client, &msg, sizeof(msg));
            sleep(1);        // 睡一会可能出现没有接到
            if (msg.tf == LOGIN_SUCCESS) // 判断是否登录成功
            {
                printf("登录成功\n");
                /* 如果不置位-1 服务器会一直给原先的sock发信息 */
                msg.loginflag2 = -1;
                break;
            }
            // else if (msg.tf == 2)
            // {
            //     printf("账号已被登录\n");
            //     continue;
            // }
            else if (msg.tf == LOGIN_FAIL)
            {
                printf("登录失败!\n");
                continue;
            }
        }
        if (msg.loginflag2 == REGISTER)                 /* 注册 */
        {
            printf("请输入你的昵称\n");
            scanf("%s", msg.Name);
            while (getchar() != '\n');

            printf("请输入你的账号\n");
            scanf("%s", msg.Account);
            while (getchar() != '\n');

            printf("请输入你的密码\n");
            scanf("%s", msg.Passwd);
            while (getchar() != '\n');

            TcpClientSend(client, &msg, sizeof(msg)); // 把自己输得账号密码发过去
            msg.tf = -1;
            TcpClientRecv(client, &msg, sizeof(msg));
            printf("msg tf : %d\n", msg.tf);
            if (msg.tf == REGISTER_FAIL)
            {
                printf("账号已存在请重新输入！\n");
            }
            else if (msg.tf == REGISTER_SUCCESS)
            {
                break;
            }
        }
    }
    /* 线程初始化 */
    Thread *t = InitThread(thread_handler, client);
    /* 将flag职位1  进行尾插 */
    msg.flag = 1;

    TcpClientSend(client, &msg, sizeof(msg));

    while (1)
    {
        start();
        scanf("%d", &msg.flag);
        while (getchar() != '\n');
        msg.flag += 1;
        if      (msg.flag == PRIVATE_CHAT)              /* 私聊 */
        {
            printf("请输入你需要发送的好友姓名：\n");
            scanf("%[^\n]", msg.toName);
            while (getchar() != '\n');

            printf("请输入你需要发送给好友的内容：\n");
            scanf("%[^\n]", msg.content);
            while (getchar() != '\n');
            TcpClientSend(client, &msg, sizeof(msg));
        }
        else if (msg.flag == BROADCAST)                 /* 广播 */
        {
            printf("请输入你要群发的内容：\n");
            scanf("%[^\n]", msg.content);
            while (getchar() != '\n');
            TcpClientSend(client, &msg, sizeof(msg));
        }
        else if (msg.flag == GROUP_OPERATION)           /* 群操作 */
        {
            printf("======================\n");
            printf("=     1、创建群聊      =\n");
            printf("=     2、拉好友进群聊   =\n");
            printf("=     3、群发信息      =\n");
            printf("======================\n");

            scanf("%d", &msg.flag3);
            while (getchar() != '\n');
            if (msg.flag3 == CREATE_GROUPS)             /* 创群 */
            {
                printf("请输入你要创群聊的名称\n");
                scanf("%s", msg.Groupchat);
                TcpClientSend(client, &msg, sizeof(msg));
            }
            if (msg.flag3 == PULL_GROUP)                /* 拉人 */
            {
                printf("请输入你要操作的群聊名称\n");
                scanf("%s", msg.Groupchat);
                while (getchar() != '\n');
                printf("请输入要拉进群的好友昵称\n");
                scanf("%[^\n]", msg.toName);
                while (getchar() != '\n');

                TcpClientSend(client, &msg, sizeof(msg));

            }
            if (msg.flag3 == GROUP_CHAT)                /* 群聊 */
            {
                printf("请输入你要在哪个群聊里进行群发\n");
                scanf("%s", msg.Groupchat);
                while (getchar() != '\n');
                printf("请输入你要群发的内容\n");
                scanf("%s", msg.content);
                while (getchar() != '\n');
                TcpClientSend(client, &msg, sizeof(msg));
            }
        }
        else if (msg.flag == ADD_FRIENDS)               /* 加好友 */
        {
            printf("请输入要加的好友昵称！\n");
            printf("msg name : %s\n", msg.Name);
            scanf("%[^\n]", msg.toName);
            while (getchar() != '\n');
            TcpClientSend(client, &msg, sizeof(msg));
        }
        else if (msg.flag == DEL_FRIENDS)               /* 删除好友 */
        {
            int choise;
            printf("请输入要删的好友昵称！\n");
            scanf("%[^\n]", msg.toName);
            while (getchar() != '\n');

            printf("确定要删除好友吗 1、确定  2、取消\n");
            scanf("%d", &choise);
            while (getchar() != '\n');
            if (choise == 2)
            {
                continue;
            }
            else if (choise == 1)
            {
                TcpClientSend(client, &msg, sizeof(msg));
            }
        }
        else if (msg.flag == FILE_TRANSFER)             /* 文件传输 */
        {
            printf("请输入要传送文件的好友昵称\n");
            scanf("%s", msg.toName);
            while (getchar() != '\n');

            printf("请输入要传输的文件的名称\n");
            scanf("%s", msg.fileName);
            while (getchar() != '\n');
            TcpClientSend(client, &msg, sizeof(msg));
        }
        else if (msg.flag == VIEWONLINE_INFOMATION)     /* 查看在线信息 */
        {
            printf("msg. flag :%d\n", msg.flag);
            printf("1.查看在线总人数人数 2.查看在线好友\n");
            int choice = 0;
            scanf("%d", &choice);
            while (getchar() != '\n');
            if (choice == 1)
            {
                msg.tf = FIND_ONLINE_NUM;
                TcpClientSend(client, &msg, sizeof(msg));
            }
            if (choice == 2)
            {
                msg.tf = FIND_ONLINE_FRIEND;
                TcpClientSend(client, &msg, sizeof(msg));
            }
        }
        else if (msg.flag == START_PLAY_GAME)
        {
            startPlayGame();
            continue;;
        }
        else if (msg.flag == USER_OFFLINE)              /* 退出 */
        {
            int choise;
            printf("确认是否退出\n");
            printf("\033[31m1、确定退出\033[0m\n");
            printf("2、取消\n");
            scanf("%d", &choise);

            if (choise == 2)
            {
                continue;
            }
            if (choise == 1) // 如果1确定退出
            {
                TcpClientSend(client, &msg, sizeof(msg)); // 给服务器发个信息
            }
            break;
        }
        else if (msg.flag == AGREE_FRIEND)              /* 同意好友 */
        {
            int choise;
            printf("1、同意\n");
            printf("2、拒绝\n");
            
            scanf("%d", &choise);
            while (getchar() != '\n');  // 清除输入缓冲         
            if(choise == 1)
            {
                msg.flag = INSERT_FRIEND;
                strncpy(msg.toName, Name, sizeof(Name));  
                printf("你已经和%s 成为好友请开始你们的聊天把！\n", msg.toName);
                TcpClientSend(client,&msg,sizeof(msg));
            }
            else if(choise == 2)
            {
                printf("该用户拒绝了你的好友申请\n");
            }
        }
        else if (msg.flag == AGREE_FILE_TRANSFER)       /* 同意文件传输 */
        {
            int choise;
            printf("1、同意\n");
            printf("2、拒绝\n");

            scanf("%d", &choise);
            while (getchar() != '\n'); // 清除输入缓冲
            strncpy(msg.toName, ToName, sizeof(Name));
            // printf("msgname:%s\n", msg.toName);

            if (choise == 1)
            {
                char file[1026] = {0};
                printf("文件新的名字\n");
                scanf("%s", msg.fileName);
                sprintf(file, "./filetransfer/%s/%s", msg.Name, msg.fileName); // 将上面定义的全局变量赋值完传过来
                // printf("msg.name : %s name : %s toname : %s fileName :  %s sizeof %ld  content : %s\n",msg.Name,Name,ToName,msg.fileName,FileSize,Filecontent);
                WriteToFile(file, Filecontent, FileSize);
                msg.flag = AGREE_OR_REJEST_FILE_TRANSFER;
                msg.tf = AGREE_FILE_TRANSFER; // 标志位
                printf("你已成功的接受了好友%s的文件传输请进行查看把~\n", msg.toName);
                TcpClientSend(client, &msg, sizeof(msg));
            }
            else
            {
                printf("你拒绝了好友的文件传输\n");
                msg.flag = AGREE_OR_REJEST_FILE_TRANSFER;
                TcpClientSend(client, &msg, sizeof(msg));
            }
        }
        else if (msg.flag == AGREE_GROUP)               /* 群聊操作 */
        {
            int choise;
            printf("1、同意\n");
            printf("2、拒绝\n");

            scanf("%d", &choise);
            while (getchar() != '\n'); // 清除输入缓冲

            if (choise == 1)
            {
                strncpy(msg.toName, Name, sizeof(Name)); 
                strncpy(msg.Groupchat, groupName, sizeof(groupName));
                msg.flag = INSERT_GROUP;
                TcpClientSend(client, &msg, sizeof(msg));
            }
            else
            {
                printf("你拒绝了加群申请\n");
            }
        }
    }
    ClearThread(t);
    return 0;
}

int startPlayGame()
{
    srand(time(NULL));//使每时每刻的随机值都不一样
    system("clear");
    printf("***********************************************************\n");
    printf("****************欢迎来到魔塔时间！ 我的小baby*******************\n");
    printf("***********************************************************\n");


    char board[Map_Size][Map_Size];//用二维数组定义地图的大小
    InitBoard(board);

    struct Player player;//创建一个名为player、struct Player类型的变量
    InitPlayer(&player);//取这个名为player的地址在InitPlayer函数中给他赋值
    
    struct DynamicArray monsters = {NULL,10,0};//创建一个名为monsters的动态数组结构体，并将他动态数组指针置空、在堆上申请10个单位空间、0个元素
    InitMonster(&monsters);

    struct DynamicArray treasures = {NULL,10,0};
    InitMonster(&treasures);


    while (1)
    {
        system("clear");
        PrintMap(board,&player,&monsters);
        printf("你当前所在的位置是<%d,%d>\n",player.x+1,player.y+1);

        for(int i = 0; i < monsters.len;i++)
        {
            struct Monster *monster = (struct Monster *)monsters.dp[i];
            if(monster->HP > 0 && monster->x == player.x && monster->y == player.y)
            {
                if(Battle(&player,monster) == false)
                {
                    printf("Game over!\n");
                    break;
                }
            }
        }

        if(player.HP == 0)
        break;

        printf("请选择你要进行的移动(输入'w','s','a','d'): ");
        printf("\033[31m按q退出游戏哦~\033[0m\n");
        char choice;
        scanf("%c", &choice);
        MakeMove(&player,choice);

        int flage = 0;
        for(int i = 0; i < monsters.len;i++)
        {
            
            struct Monster *monster = (struct Monster *)monsters.dp[i];
            if(monster->HP > 0)
            {
                flage =1;
            }
        }
        if(flage == 0)
        {
            printf("你胜利了！\n");
            break;
        }
        if (choice == 'q')
        {
            break;
        }
    }


        for(int i = 0; i < monsters.len;i++)
        {
            struct Monster *monster = (struct Monster *)monsters.dp[i];
            free(monster->name.string);
            free(monster);
        }
        free(monsters.dp);
    return 0;
}