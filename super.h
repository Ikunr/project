#ifndef __SUPER_H_
#define __SUPER_H_




#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"MyString.h"
#include"DynamicArray.h"

struct Player
{
    int HP;
    int attack;
    int x;
    int y;
};

struct Monster
{
    MyString name;
    int HP;
    int attack;
    int x;
    int y;
};

struct Treasure
{
    MyString name;
    int value;
    int x;
    int y;
};


void InitPlayer(struct Player* Player);
struct Monster * CreateMonster(const char* name,int HP,int attack);
struct Treasure * CreateTreasure(const char* name,int value);
void InitMonster(struct DynamicArray *array);
void InitTreasuers(struct DynamicArray *array);
int GetRandNumber(int max);


#endif