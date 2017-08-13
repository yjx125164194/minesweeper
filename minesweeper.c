#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include"game.h"

#define EASY

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

#define SPEED     500*1000

int opened_count = 0;
int marked_count = 0;

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
						if(!(*(fp+i))[j].flag_status)
						{
							printf("■");
						}
						else
						{
							printf("★");
						}
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
		printf("Length %d,Wide %d,Mine %d Opened %d\n",LENGTH,WIDE,NUMBER - marked_count,opened_count);
		usleep(SPEED);
	}
}

_Bool shift_user_location(Content_type (*fp)[LENGTH],Location_type *pUser,char ch)
{
	_Bool status = true;
	Location_type User_tmp;
	User_tmp = *pUser;
	
	//fprintf(stderr,"command %c,Location %d,%d\n",ch,pUser->x,pUser->y);
	if(status)
	{
		switch(ch)
		{
			case '8':
				if(pUser->y > 0)
				{
					pUser->y--;//UP 
				}
				else
				{
					return false;
				}
				break;
	            	case '5':
				if(pUser->y < WIDE-1) 
				{
					pUser->y++;//DOWN
				}
				else
				{
					return false;
				}
				break;
        	    	case '4':
				if(pUser->x > 0)
				{
					pUser->x--;//LEFT
				}
				else
				{
					return false;
				}
				break;
            		case '6':
				if(pUser->x < LENGTH-1) 
				{
					pUser->x++;//RIGHT
				}
				else
				{
					return false;
				}
				break;
          	  	default:break;
		}
		if((*(fp+pUser->y))[pUser->x].open_status)
		{
			if((*(fp+pUser->y))[pUser->x].content == empty
				|| (*(fp+pUser->y))[pUser->x].content == mine)
			{
				status = shift_user_location(fp,pUser,ch);
			}
		}
	}	
	
	if(!status)
	{
		*pUser = User_tmp;
		return false;
	}
	return true;
}

int open_user_location(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	int i,j;
	int result = 0;
	int flag_count = 0;
	Location_type tmp;
	
	if((*(fp+lfp->y))[lfp->x].open_status == false 
		&& (*(fp+lfp->y))[lfp->x].flag_status == false)	
	{
		(*(fp+lfp->y))[lfp->x].open_status = true;
		opened_count++;

		if((*(fp+lfp->y))[lfp->x].content == empty)
		{
			for(i = MAX(lfp->y-1,0);i <= MIN(lfp->y+1,WIDE-1);i++)
			{	
				for(j = MAX(lfp->x-1,0);j <= MIN(lfp->x+1,LENGTH-1);j++)
				{
					tmp.x = j;
					tmp.y = i;
					open_user_location(fp,&tmp);
				}
			}
		}
		else if((*(fp+lfp->y))[lfp->x].content == mine)
		{
			result = 1;
		}
	}
	else
	{
		if((*(fp+lfp->y))[lfp->x].content != empty 
		&& (*(fp+lfp->y))[lfp->x].content != mine)
		{
			for(i = MAX(lfp->y-1,0);i <= MIN(lfp->y+1,WIDE-1);i++)
			{	
				for(j = MAX(lfp->x-1,0);j <= MIN(lfp->x+1,LENGTH-1);j++)
				{
					if((*(fp+i))[j].flag_status)
					{
						flag_count++;
					} 
				}
			}
			if((*(fp+lfp->y))[lfp->x].content == flag_count)
			{
				for(i = MAX(lfp->y-1,0);i <= MIN(lfp->y+1,WIDE-1);i++)
				{	
					for(j = MAX(lfp->x-1,0);j <= MIN(lfp->x+1,LENGTH-1);j++)
					{
						if((*(fp+i))[j].open_status == false 
							&& (*(fp+i))[j].flag_status == false)	
						{
							tmp.x = j;
							tmp.y = i;
							open_user_location(fp,&tmp);
						}
					}
				}
			} 
		}
	}
	return result;
}

void flag_user_location(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	if(!(*(fp+lfp->y))[lfp->x].open_status)
	{
		if((*(fp+lfp->y))[lfp->x].flag_status)
		{
			marked_count--;
		}
		else
		{
			marked_count++;
		}
		(*(fp+lfp->y))[lfp->x].flag_status = !(*(fp+lfp->y))[lfp->x].flag_status;
	}
}
void check_first_enter(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	while((*(fp+lfp->y))[lfp->x].content == mine)
	{
		init_mine(fp);
	} 
}
int main(int argc,char *argv[])
{
	Location_type User;
	Location_type *pUser;
	Content_type Content[WIDE][LENGTH];
	Content_type (*pContent)[LENGTH];

	char ch;
	int result = 0;
	_Bool first_enter = true;

	User.y = WIDE/2;
	User.x = LENGTH/2;
	pContent = &Content[0];
	pUser = &User;
 
	srand((unsigned int) time(0));
	
	init_mine(pContent);	
	printf("Print any key to start the game\n");
	while(1)
	{
		if(kbhit())
		{
			switch(ch = sh_getch())
      			{
				case '8':
				case '5':
				case '4':
				case '6':
					shift_user_location(pContent,pUser,ch);
					break;
				case '0':
					if(first_enter)
					{
						check_first_enter(pContent,pUser);
						first_enter = false;	
					}
					result = open_user_location(pContent,pUser);
					break;
				case '.':
					flag_user_location(pContent,pUser);
					break;
				case 'q':
					exit(0);
				default:break;
			}
		}
		print_block(pContent,pUser);
		if(result)
		{
			printf("You open the mine and failed!\n");
			exit(0);
		}
		else if(opened_count == WIDE*LENGTH - NUMBER)
		{
			printf("You win the game!\n");
			exit(0);
		}
	}
	return 0;
}
