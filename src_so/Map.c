#include"Map.h"




void InitBoard(char (*p)[Map_Size])//用数组指针指向二维数组
{
    for(int i = 0; i < Map_Size;i++)
    {
        for(int j = 0;j < Map_Size;j++)
        {
            p[i][j] = '-';
        }
    }
}


void PrintMap(char (*p)[Map_Size],struct Player *player,struct DynamicArray *monsent )
{
    InitBoard(p);

    p[player->y][player->x] = 'x';//
    
    for(int i =0 ;i<monsent->len;i++)
    {
        struct Monster *tempmonsent = (struct Monster *)monsent->dp[i];
        if(tempmonsent->HP != 0)
        p[tempmonsent->y][tempmonsent->x]='m';
    }

    printf("     ");
    for(int i = 0;i < Map_Size;i++)
    {
        printf(" %4d",i+1);
    }
    printf("\n");
    for(int i = 0; i < Map_Size;i++)
    {
        printf(" %4d",i+1);
        for(int j = 0; j < Map_Size;j++)
        {
            printf(" %4c",p[i][j]);//
        }
        printf("\n");
    }
}


