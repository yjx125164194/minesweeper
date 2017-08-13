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
			(*(fp+i))[j].status = false;
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
			(*(fp+x))[y].status = true;
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
			if(!(*(fp+i))[j].status)
			{
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{	
						if((*(fp+itmp))[jtmp].status)
						{
							(*(fp+i))[j].content++;
						}
					}
				}
			}
		}
	}
}
void print_block(Content_type (*fp)[LENGTH])
{
	
}
void print_mine(Content_type (*fp)[LENGTH])
{
	system("clear");

	int i,j;
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
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
					printf("*");
					break;
			}
		}
		printf("\n");
	}
}
int main(int argc,char *argv[])
{
	Content_type Content[WIDE][LENGTH];
	Content_type (*pContent)[LENGTH];

	pContent = &Content[0];
	srand((unsigned int) time(0));
	
	init_mine(pContent);
	print_mine(pContent);
	return 0;
}
