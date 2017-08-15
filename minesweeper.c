#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include"game.h"

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
#else
	#define	LENGTH  9
	#define WIDE	9
	#define NUMBER  10
#endif

#define MIN(X,Y)  (X>Y?Y:X)
#define MAX(X,Y)  (X>Y?X:Y) 

#ifdef AUTO
	#define SPEED       500*1000
#else
	#define SPEED       500*1000
#endif

#ifdef AUTO
	_Bool AUTO_RANDOM     = false;
	_Bool AUTO_NOHUMAN   = false;
#endif	


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
#ifdef AUTO
			(*(fp+i))[j].question_status = false;
#endif
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
	int Loop_count;
	
	#ifndef AUTO
	 	Loop_count = 2;
	#else
		Loop_count = 1;
	#endif

	for(m = 0;m < Loop_count;m++)
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
					#ifdef AUTO
					else if((*(fp+i))[j].question_status)
					{
						printf("?");
					}
					#endif
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
			return 1;
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
							if(open_user_location(fp,&tmp))
							{
								return 1;
							}
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
/*AUTO FUNCTION*/
#ifdef AUTO
int random_open(Content_type (*fp)[LENGTH])
{
	Location_type lfp;
	int result;

	static _Bool auto_first_enter = true;
	
	do
	{
		lfp.x = rand()%LENGTH;
		lfp.y = rand()%WIDE;
	}while((*(fp+lfp.y))[lfp.x].open_status);
	if((result = open_user_location(fp,&lfp)) && auto_first_enter) 
	{
		init_mine(fp);
		random_open(fp);
	}
	auto_first_enter = false;
	
	AUTO_RANDOM = true;
	AUTO_NOHUMAN = false;
	
	return result;
}
_Bool cal_open(Content_type (*fp)[LENGTH],int all_number,int mine_number);

void nohuman_open(Content_type (*fp)[LENGTH])
{
	int i,j;
	int itmp,jtmp;
	int unopened_block = 0;
	int flaged_block = 0;
	int open_count_tmp = opened_count;
	Location_type lfp;

	for(i = 0;i < WIDE;i++)
	{	
		for(j = 0;j < LENGTH;j++)
		{	
			if((*(fp+i))[j].open_status)
			{
				if((*(fp+i))[j].content != empty && (*(fp+i))[j].content != mine)
				{
					for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
					{	
						for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
						{
							if(!(*(fp+itmp))[jtmp].open_status)	
							{
								if(!(*(fp+itmp))[jtmp].flag_status)	
								{
									unopened_block++;
								}
								else
								{
									flaged_block++;
								}
							}
						}
					}
					if((unopened_block + flaged_block) == (*(fp+i))[j].content)
					{
						for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
						{	
							for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
							{
								if((!(*(fp+itmp))[jtmp].open_status) &&(!(*(fp+itmp))[jtmp].flag_status))	
								{
									lfp.x = jtmp;
									lfp.y = itmp;
									flag_user_location(fp,&lfp);
								}
							}
						}
					}
					unopened_block = 0;
					flaged_block = 0;
				}
			}
		}
	}
	for(i = 0;i < WIDE;i++)
	{	
		for(j = 0;j < LENGTH;j++)
		{
			if((*(fp+i))[j].open_status)
			{
				lfp.x = j;
				lfp.y = i;
				open_user_location(fp,&lfp);
			}
		}
	}
	if(opened_count == open_count_tmp)
	{
		if(!cal_open(fp,2,1))
		{
			AUTO_NOHUMAN = true;
			AUTO_RANDOM = false;
		}
		else
		{	
			AUTO_NOHUMAN = false;
		}
	}
	else
	{
		AUTO_NOHUMAN = false;
	}
}

void cal_question_clear(Content_type (*fp)[LENGTH])
{
	int i,j;
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
		{
			(*(fp+i))[j].question_status = false;
		}
	}
}


/*算法考虑三种情况
2个里有1个
3个里有1个
3个里有2个
即all_number与mine_number组合有以上三种*/

_Bool cal_open(Content_type (*fp)[LENGTH],int all_number,int mine_number)
{
	int i,j;
	int itmp,jtmp;
	int itmp2,jtmp2;
	
	int unopened_count = 0;
	int flag_count = 0;

	int question_count = 0;
	int unquestion_count = 0;
	int unopened_count_next = 0;
	int unmine_count = 0;
	int flag_count_next = 0;

	Location_type tmp_question;
	Location_type tmp_unquestion[8];

	_Bool result = false;

	for(i = 0;i < WIDE;i++)
	{	
		for(j = 0;j < LENGTH;j++)
		{	
			for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
			{	
				for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
				{
					if((*(fp+itmp))[jtmp].flag_status)
					{
						flag_count++;
					}
				}
			}
			
			if((*(fp+i))[j].open_status && (*(fp+i))[j].content == (mine_number + flag_count))
			{
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{
						if((!(*(fp+itmp))[jtmp].open_status) && (!(*(fp+itmp))[jtmp].flag_status))	
						{
							unopened_count++;
						}
					}
				}
			}
			
			if(unopened_count == all_number && unopened_count != 0)
			{
				
				fprintf(stderr,"flag_count     = %d|%d,%d\n",flag_count,i,j);
				fprintf(stderr,"content        = %d|%d,%d\n",(*(fp+i))[j].content,i,j);
				fprintf(stderr,"unopened_count = %d|%d,%d\n\n",flag_count,i,j);
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{
						if((!(*(fp+itmp))[jtmp].open_status) && (!(*(fp+itmp))[jtmp].flag_status))	
						{
							(*(fp+itmp))[jtmp].question_status = true;
							(*(fp+itmp))[jtmp].all_number = all_number;
							(*(fp+itmp))[jtmp].mine_number = mine_number;
						}
					}
				}/*
				for(itmp = MAX(i-2,0);itmp <= MIN(i+2,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-2,0);jtmp <= MIN(j+2,LENGTH-1);jtmp++)
					{
						if(((*(fp+itmp))[jtmp].open_status) && ((*(fp+i))[j].content != empty))
						{
							for(itmp2 = MAX(itmp-1,0);itmp2 <= MIN(itmp+1,WIDE-1);itmp2++)
							{	
								for(jtmp2 = MAX(jtmp-1,0);jtmp2 <= MIN(jtmp+1,LENGTH-1);jtmp2++)
								{
									if((!(*(fp+itmp2))[jtmp2].open_status) 
									&& (!(*(fp+itmp2))[jtmp2].flag_status))	
									{
										unopened_count_next++;
										if((*(fp+itmp2))[jtmp2].question_status)
										{
											tmp_question.x = jtmp2;
											tmp_question.y = itmp2;
											question_count++;	
										}	
										else
										{
											tmp_unquestion[unquestion_count].x = jtmp2;
											tmp_unquestion[unquestion_count].y = itmp2;
											unquestion_count++;
										}
									}
									else if((*(fp+itmp2))[jtmp2].flag_status)
									{
										flag_count_next++;
									}
								}		
							}
							if(question_count == (*(fp+tmp_question.y))[tmp_question.x].all_number
								&& unopened_count_next > question_count)
							{
								unmine_count =(*(fp+itmp))[jtmp].content 
									-(*(fp+tmp_question.y))[tmp_question.x].all_number
									-flag_count_next;
								if(unmine_count == 0)
								{
									while(unquestion_count)
									{
										open_user_location(fp,&tmp_unquestion[--unquestion_count]);
										result = true;
									}
								}
								else if(unmine_count == unquestion_count)
								{
									while(unquestion_count)
									{
										flag_user_location(fp,&tmp_unquestion[--unquestion_count]);
										result = true;
									}
								}
							}
						}
					unopened_count_next = 0;
					question_count = 0;
					unquestion_count = 0;
					unmine_count = 0;	
					flag_count_next = 0;
					}
				}*/	
			}
			flag_count = 0;
			unopened_count = 0;
			cal_question_clear(fp);
		}
	}
	return result;
}

#endif

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
	#ifdef AUTO	
		printf("Print any key to start the game\n");
	#endif
	while(1)
	{
	#ifndef AUTO
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
	#else
		if(!AUTO_RANDOM)
		{
			result = random_open(pContent);
		}
		if(!AUTO_NOHUMAN)
		{
			nohuman_open(pContent);
		}
	#endif
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
