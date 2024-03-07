#include"super.h"
#include"DoubleLinkList.h"
#include"StdFile.h"
#include<string.h>
#define Map_Size 5 //定义地图大小
#define ture 1
#define false 0



int GetRandNumber(int max)
{
    
    return rand() % max;
}

void InitPlayer(struct Player* Player)
{
    Player->HP = 300;
    Player->attack = 999;
    Player->x = 0;
    Player->y = 0;
}


struct Monster * CreateMonster(const char* name,int HP,int attack)
{
    struct Monster *monster = (struct Monster *)malloc(sizeof(struct Monster));
    if(monster == NULL)
    {
        printf("Create Monster Error!\n");
        return NULL;
    }

    monster->HP = HP;
    monster->attack = attack;
    Initialize(&monster->name,name);//在MyString.c文件中用Initialize函数将怪兽的名字赋给怪兽

    monster->x = GetRandNumber(Map_Size);
    monster->y = GetRandNumber(Map_Size);


    return monster;
}

struct Treasure * CreateTreasure(const char* name,int value)
{
    struct Treasure *treasure = (struct Treasure *)malloc(sizeof(struct Treasure));
    if(treasure == NULL)
    {
        printf("Create treasure Error!\n");
        return NULL;
    }

    Initialize(&treasure->name,name);
    treasure->value = value;

    treasure->x = GetRandNumber(Map_Size);
    treasure->y = GetRandNumber(Map_Size);


    return treasure;
}

void InitMonster(struct DynamicArray *array)
{
    if(ArrayInit(array) == false)
    {
        return;
    }

    InsertArray(array,CreateMonster("纳什男爵",150,20));//在名为monsters动态数组结构体中插入CreateMonster函数创建的怪兽信息
    InsertArray(array,CreateMonster("巨龙",100,15));
    InsertArray(array,CreateMonster("食人魔1",20,10));
    InsertArray(array,CreateMonster("食人魔2",20,10));
    InsertArray(array,CreateMonster("食人魔3",20,10));
    InsertArray(array,CreateMonster("哥布林",10,5));
    InsertArray(array,CreateMonster("哥布林",10,5));
    InsertArray(array,CreateMonster("哥布林",10,5));
    InsertArray(array,CreateMonster("洪伟",1,1));
}



void InitTreasuers(struct DynamicArray *array)
{
    if(ArrayInit(array) == false)
    {
        return;
    }
    InsertArray(array,CreateTreasure("法拉利",100));
    InsertArray(array,CreateTreasure("美国美女",50));
    InsertArray(array,CreateTreasure("法国美女",50));
    InsertArray(array,CreateTreasure("日本美女",50));
    InsertArray(array,CreateTreasure("真香槟",20));
    InsertArray(array,CreateTreasure("假香槟",20));
    InsertArray(array,CreateTreasure("不真不假香槟",20));
    InsertArray(array,CreateTreasure("半真半假香槟",20));
    InsertArray(array,CreateTreasure("小香槟",20));
    
}

