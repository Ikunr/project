#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "StdTcp.h"

// TCP服务器
struct TcpServer
{
    int sock;
};
// 初始化服务器
TcpS *InitTcpServer(const char *IP, short int port)
{
    TcpS *s = (TcpS *)malloc(sizeof(TcpS));
    if (s == NULL)
    {
        printf("InitTcpServer malloc error!\n");
        return NULL;
    }
    s->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock < 0)
    {
        perror("socket():");
        free(s);
        return NULL;
    }

    int on = 1;
    if (setsockopt(s->sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
    {
        perror("setsockopt");
        free(s);
        return NULL;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(IP);

    if (bind(s->sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind:");
        free(s);
        return NULL;
    }

    if (listen(s->sock, 10) < 0)
    {
        perror("listen:");
        free(s);
        return NULL;
    }

    return s;
}

// 将已经握手的客户端
int TcpServerAccept(TcpS *s)
{
    int acceptSock = 0;
    struct sockaddr_in addr;
    socklen_t len = 0;
    if ((acceptSock = accept(s->sock, (struct sockaddr *)&addr, &len)) < 0)
    {
        perror("accept:");
        return -1;
    }
    return acceptSock;
}

void TcpServerSend(int ClientSock, void *ptr, size_t size)
{
    if (send(ClientSock, ptr, size, 0) < 0)
    {
        perror("send");
    }
}

int TcpServerRecv(int ClientSock, void *ptr, size_t size)
{
    int ret = recv(ClientSock, ptr, size, 0);
    return ret; // 返回具体的错误码
}

int getTcpSock(TcpS *c)
{
    return c->sock;
}


void ClearTcpServer(TcpS *s)
{
    close(s->sock);
    free(s);
}

struct TcpClient
{
    int sock;
};

TcpC *InitTcpClient(const char *ServerIP, short int ServerPort)
{
    TcpC *c = (TcpC *)malloc(sizeof(TcpC));
    if (c == NULL)
    {
        printf("InitTcpServer malloc error!\n");
        return NULL;
    }
    c->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (c->sock < 0)
    {
        perror("socket():");
        free(c);
        return NULL;
    }

    struct sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(ServerPort);
    ServerAddr.sin_addr.s_addr = inet_addr(ServerIP);
    if (connect(c->sock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    {
        perror("connect:");
        free(c);
        return NULL;
    }
    return c;
}

void TcpClientSend(TcpC *c, void *ptr, size_t size)
{
    if (send(c->sock, ptr, size, 0) < 0)
    {
        perror("send");
    }
}

void TcpClientRecv(TcpC *c, void *ptr, size_t size)
{
    if (recv(c->sock, ptr, size, 0) < 0)
    {
        perror("recv");
    }
}

void ClearTcpClient(TcpC *c)
{
    close(c->sock);
    free(c);
}
