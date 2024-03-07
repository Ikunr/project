#include <sys/socket.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <stdio.h>

int main()
{                                                             // 区别与 tpc和udp   数据包可以自己开发 更灵活  ETH_P_ALL 每一个数据包    sock——raw
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // 原始套接字 ：实现来源于内核  每一个包

    while (1)
    {
        unsigned char str[1600] = {0};
        recv(sock, str, 1600, 0); // 接受消息

        unsigned char dst_mac[18] = {0};
        unsigned char src_mac[18] = {0};

        sprintf(dst_mac, "%x:%x:%x:%x:%x:%x", str[0], str[1], str[2], str[3], str[4], str[5]);
        sprintf(src_mac, "%x:%x:%x:%x:%x:%x", str[6], str[7], str[8], str[9], str[10], str[11]);

        printf("src mac :%s ----> dst mac : %s\n", src_mac, dst_mac);
    }

    return 0;
    
}