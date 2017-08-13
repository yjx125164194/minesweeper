#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include"game.h"

#define HARD

#ifdef HARD
	#define LENGTH	30
	#define WIDE	16
	#define NUMBER  99
#elif defined  MEDIUM
	#define	LENGTH  16
	#define WIDE	16
	#define NUMBER  40
#elif defined  EASY
	#define	LENGTH  9
	#define WIDE	9
	#define NUMBER  10
#endif

#define MIN(X,Y)  (X>Y?Y:X)
#define MAX(X,Y)  (X>Y?X:Y) 

#define SPEED     250*1000
void init_mine(Content_type (*fp)[LENGTH]) 
{
	int i,j,k;
	int x,y;
	int itmp,jtmp;
	// init block
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
		{
			(*(fp+i))[j].content = empty;
			(*(fp+i))[j].mine_status = false;
			(*(fp+i))[j].open_status = false;
			(*(fp+i))[j].flag_status = false;
		}
	}
	// set random mine
	for(k = 0;k < NUMBER;k++)
	{
		x = rand()%WIDE;
		y = rand()%LENGTH;
		if((*(fp+x))[y].content == empty)
		{
			(*(fp+x))[y].content = mine;
			(*(fp+x))[y].mine_status = true;
		}
		else
		{
			k--;
		}
	}
	//set the number of mine
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
		{
			if(!(*(fp+i))[j].mine_status)
			{
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{	
						if((*(fp+itmp))[jtmp].mine_status)
						{
							(*(fp+i))[j].content++;
						}
					}
				}
			}
		}
	}
}
void print_mine(Content_type (*fp)[LENGTH],int i,int j)
{
	switch((*(fp+i))[j].content)
	{
		case empty:
			printf(" ");
			break;
		case one:
			printf("1");
			break;
		case two:
			printf("2");
			break;
		case three:
			printf("3");
			break;
		case four:
			printf("4");
			break;
		case five:
			printf("5");
			break;
		case six:
			printf("6");
			break;
		case seven:
			printf("7");
			break;
		case eight:
			printf("8");
			break;
		case mine:
			printf("※");
			break;
	}
}
void print_block(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	int m;
	int i,j;
	for(m = 0;m < 2;m++)
	{
		system("clear");	
		for(i = 0;i < WIDE;i++)
		{
			for(j = 0;j < LENGTH;j++)
			{	
				if(m == 0 || i != lfp->y || j != lfp->x)			
				{
					if(!(*(fp+i))[j].open_status)
					{
						printf("■");
					}
					else
					{
						print_mine(fp,i,j);	
					}
				}
				else
				{
					printf("□");
				}
			}
		printf("\n");
		}
	usleep(SPEED);
	}
}

void shift_user_location(Location_type *pUser,char ch)
{
	switch(ch)
	{
		case '8':
			if(pUser->y > 0)
			{
				pUser->y--;//UP 
			}
			break;
            	case '5':
			if(pUser->y < WIDE-1) 
			{
				pUser->y++;//DOWN
			}
			break;
            	case '4':
			if(pUser->x > 0)
			{
				pUser->x--;//LEFT
			}
			break;
            	case '6':
			if(pUser->y < LENGTH-1) 
			{
				pUser->x++;//RIGHT
			}
			break;
            	default:break;
	}
}
int main(int argc,char *argv[])
{
	Content_type Content[WIDE][LENGTH];
	Content_type (*pContent)[LENGTH];
	Location_type User;
	Location_type *pUser;
	char ch;
	
	User.y = WIDE/2;
	User.x = LENGTH/2;
	pContent = &Content[0];
	pUser = &User;
 
	srand((unsigned int) time(0));
	
	init_mine(pContent);	
	printf("Print any key to start the game\n");
	while(1)
	{
		switch(ch = sh_getch())
      		{
			case '8':
			case '5':
			case '4':
			case '6':
				shift_user_location(pUser,ch);
				break;
			case '\n':
				//open_user_location(pContent,pUser);
				break;
			case ' ':
				//flag_user_location(pContent,pUser);
				break;
			case 'q':
				exit(0);
			default:break;
		}
		print_block(pContent,pUser);
	}
	return 0;
}
