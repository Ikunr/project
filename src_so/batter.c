#include"batter.h"
#include"Map.h"



void MakeMove(struct Player *player,char symbol)
{
    switch (symbol)
    {
    case 'w':
        player->y--;
        break;
    case 's':
        player->y++;
        break;
    case 'a':
        player->x--;
        break;
    case 'd':
        player->x++;
        break;
    default: break;
    }

    if(player->x < 0) player->x = 0;
    if(player->x >= Map_Size) player->x = Map_Size-1;
    if(player->y <0) player->y = 0;
    if(player->y>=Map_Size) player->y = Map_Size-1;
}

int Battle(struct Player *player,struct Monster* monster)
{
    system("clear");
    printf("您遭遇了|%s| 血量: |%d| 攻击: %d",monster->name.string,monster->HP,monster->attack);
    printf("****************************************\n");
    printf("                 开始战斗                \n");
    printf("****************************************\n");
    while(player->HP > 0 && monster > 0)
    {
        
        printf("您遭遇了|%s| 血量：|%d| 攻击力： |%d|\n" ,monster->name.string,monster->HP,monster->attack);
        printf("请选择你要执行的行为：\n");
        printf("1、普通攻击\n");
        printf("2、暴击\n");
        printf("3、逃跑\n");
        int choice;
        scanf("%d",&choice);
        switch (choice)
        {
            case 1:
                player->HP -= monster->attack;
                monster->HP -= player->attack;
                printf("|%s|对你造成了%d伤害 你当前的血量为|%d| \n",monster->name.string,monster->attack,player->HP-=monster->attack);
                printf("你对|%s|造成了%d伤害 \n",monster->name.string,player->attack);    
                break;

            case 2:
                if(GetRandNumber(2) == 1)
                {
                    printf("|%s|对你造成了%d伤害\n",monster->name.string,monster->attack);
                    printf("你对|%s|造成了%d伤害\n",monster->name.string,player->attack *2); 
                    player->HP -= monster->attack;
                    monster->HP -= player->attack * 2;
                }
                else
                {
                    printf("|%s|对你造成了%d伤害\n",monster->name.string,monster->attack);
                    printf("暴击失败！ 无法发动攻击！\n");
                    player->HP -= monster->attack;
                }
                break;

            case 3:
                if(GetRandNumber(2) == 1)
                {
                    printf("逃跑成功\n");
                    return ture;
                }
                else
                {
                    printf("逃跑失败\n");
                    printf("%s对你造成了%d伤害\n",monster->name.string,monster->attack);
                    player->HP -= monster->attack;
                }
                break;
                default:break;
        }
        if(player->HP <= 0)
    {
        printf(("你被|%s|击败了！\n"),monster->name.string);
        player->HP = 0;
        return false;
    }

    if(monster->HP <= 0)
    {
        printf(("你击败了|%s|!\n"),monster->name.string);
        monster->HP = 0;
        return ture;
    }
    }
        printf("*****************************end****************************\n");
       

    
}