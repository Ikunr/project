#ifndef __MAP_H_
#define __MAP_H

#include"super.h"
#define Map_Size 5 //定义地图大小
#define ture 1
#define false 0


void InitBoard(char (*p)[Map_Size]);
void PrintMap(char (*p)[Map_Size],struct Player *player,struct DynamicArray *monsent );



#endif