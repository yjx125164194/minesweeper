#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<time.h>
#include"game.h"

/*
 * 根据define的难度设定LENGTH,WIDE与雷的数量NUMBER
 */

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

/*
 * SPEED为print_block的刷新速度
 * TIMES为扫雷循环次数
 * 当未处于AUTO_MODE时，即单人游戏状态，TIMES为1
 */
 
#ifdef AUTO_MODE
	#define SPEED       500*1000
	#define TIMES	    200000
#else
	#define SPEED       500*1000
	#define TIMES       1
#endif

/*
 * 当处于AUTO_MODE时，定义下面全局变量
 * AUTO_MODE_RANDOM为是否开始随机open
 * AUTO_MODE_NOHUMAN为是否开始常规推理open&&flag
 * AUTO_FIRST_ENTER为是否为第一个open，用来处理第一个是雷或者第一个不是empty
 * random_times为随机open的次数
 */

#ifdef AUTO_MODE
	_Bool AUTO_MODE_RANDOM     = false;
	_Bool AUTO_MODE_NOHUMAN    = false;
	_Bool AUTO_FIRST_ENTER     = true;
	int   random_times 	   = 0;
	#define RIGHT_MAX 	   40
	#define BLOCK_MAX	   20
	#define SCAN_MAX           3
#endif	

int opened_count = 0;
int marked_count = 0;

int g_combine_result[120][8];
int g_combine_tmp[8];
int g_combine_count = 0;
int g_combine_end;
void combine(int m,int n)
{
	int i,j;
	for(i = m;i >= n;i--)
	{
		g_combine_tmp[n - 1] = i;
		if(n > 1)
		{
			combine(i-1,n-1);
		}
		else
		{
			for(j = 0;j < g_combine_end;j++)
			{
				g_combine_result[g_combine_count][j] = g_combine_tmp[j];
			}
			g_combine_count++;
		}
	}
}

int content_number(Content_type (*fp)[LENGTH],int i,int j)
{
	return ((*(fp+i))[j].content);
}

_Bool content_empty_status(Content_type (*fp)[LENGTH],int i,int j)
{
	if((*(fp+i))[j].content == empty)
	{
		return true;
	}
	else
	{
		return false;
	}
}

_Bool content_mine_status(Content_type (*fp)[LENGTH],int i,int j)
{
	if((*(fp+i))[j].content == mine)
	{
		return true;
	}
	else
	{
		return false;
	}
}

_Bool mine_status(Content_type (*fp)[LENGTH],int i,int j)
{
	return (*(fp+i))[j].mine_status;
}

_Bool open_status(Content_type (*fp)[LENGTH],int i,int j)
{
	return (*(fp+i))[j].open_status;
}

_Bool flag_status(Content_type (*fp)[LENGTH],int i,int j)
{
	return (*(fp+i))[j].flag_status;
}
/*
 * 初始化雷区
 * 第一个参数为雷区的二维数组
 */

void init_mine(Content_type (*fp)[LENGTH]) 
{
	int i,j,k;
	int x,y;
	int itmp,jtmp;
	// 将方块所有属性初始化
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
		{
			(*(fp+i))[j].content = empty;
			(*(fp+i))[j].mine_status = false;
			(*(fp+i))[j].open_status = false;
			(*(fp+i))[j].flag_status = false;
#ifdef AUTO_MODE
			(*(fp+i))[j].question_status = false;
#endif
		}
	}
	// 随机置放define定义中的雷的数量
	for(k = 0;k < NUMBER;k++)
	{
		x = rand()%WIDE;
		y = rand()%LENGTH;
		if(content_empty_status(fp,x,y))
		{
			(*(fp+x))[y].content = mine;
			(*(fp+x))[y].mine_status = true;
		}
		else
		{
			k--;
		}
	}
	//根据雷的分布，将剩余方块赋予数值（0~8）
	for(i = 0;i < WIDE;i++)
	{
		for(j = 0;j < LENGTH;j++)
		{
			if(!mine_status(fp,i,j))
			{
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{	
						if(mine_status(fp,itmp,jtmp))
						{
							(*(fp+i))[j].content++;
						}
					}
				}
			}
		}
	}
}

/*
 * 打印单格方块
 * 第一个参数为雷区的二维数组
 * 第二个参数为被打印的方块的纵坐标
 * 第三个参数为被打印的方块的横坐标
 */ 

void print_mine(Content_type (*fp)[LENGTH],int i,int j)
{
	switch(content_number(fp,i,j))
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

/*
 * 打印整个雷区
 * 第一个参数为雷区的二维数组
 * 第二个参数为非AUTO_MODE下的用户坐标
 */

void print_block(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	int m;
	int i,j;
	int Loop_count;
	
	//当处于非AUTO_MODE模式时，打印函数可以呈现用户当前坐标的方块
	#ifndef AUTO_MODE
	 	Loop_count = 2;
	#else
		Loop_count = 1;
	#endif

	for(m = 0;m < Loop_count;m++)
	{
		#ifndef AUTO_MODE
			system("clear");//清除屏幕	
		#endif
		for(i = 0;i < WIDE;i++)
		{
			for(j = 0;j < LENGTH;j++)
			{	
				if(m == 0 || i != lfp->y || j != lfp->x)			
				{
					if(!open_status(fp,i,j))
					{
						if(0)
						{
						}
						#ifdef AUTO_MODE
						else if((*(fp+i))[j].question_status)
						{
							//当处于AUTO_MODE下时，用来标记cal_open所赋予的怀疑状态
							if((*(fp+i))[j].question_mine)
							{
								printf("♂");
							}
							else
							{
								printf("♀");
							}
						}
						#endif
						else if(flag_status(fp,i,j))
						{
							printf("★");//当被标记为时，打印星号
						}
						else
						{
							printf("■");//当未被标记时，打印实心方块
						}
					}
					else
					{
						print_mine(fp,i,j);//当方块打开时，调用print_mine函数打印该方块的被打开后的数值	
					}
				}
				else
				{
					printf("□");//当处于非AUTO_MODE时，通过交替打印实心与空心方块来显示用户当前坐标
				}
			}
			printf("\n");
		}
//在整个雷区下打印参数

#ifndef AUTO_MODE
		printf("Length %d,Wide %d,Mine %d Opened %d\n",LENGTH,WIDE,NUMBER - marked_count,opened_count);
#else
		printf("Mine %d Opened %d Random %d\n",NUMBER - marked_count,opened_count,random_times);
#endif		
		usleep(SPEED);
	}
}

/*
 * 移动用户坐标
 * 第一个参数为雷区的二维数组
 * 第二个参数为输入的原始坐标
 * 第三个参数为输入的方向
 *
 * 返回值代表是否移动成功
 */

_Bool shift_user_location(Content_type (*fp)[LENGTH],Location_type *pUser,char ch)
{
	_Bool status = true;
	Location_type User_tmp;
	User_tmp = *pUser;//备份原始坐标
	
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
		//当上下左右移动遇到被打开的方块且为empty的情况下，继续移动
		if(open_status(fp,pUser->y,pUser->x))
		{
			if(content_empty_status(fp,pUser->y,pUser->x)
				|| content_mine_status(fp,pUser->y,pUser->x))
			{
				status = shift_user_location(fp,pUser,ch);
			}
		}
	}	
	//当移动失败时，将原始坐标复原
	if(!status)
	{
		*pUser = User_tmp;
		return false;
	}
	return true;
}

/*
 * 打开某个位置
 * 第一个参数为雷区的二维数组
 * 第二个参数为需要打开的坐标
 *
 * 返回值为1代表了打开了雷，游戏失败
 */


int open_user_location(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	int i,j;
	int result = 0;
	int flag_count = 0;
	Location_type tmp;
	
	//当该坐标未被打开且未被flag标记时执行
	if((!open_status(fp,lfp->y,lfp->x)) && (!(flag_status(fp,lfp->y,lfp->x))))
	{
		(*(fp+lfp->y))[lfp->x].open_status = true;
		opened_count++;
		//当打开的坐标为empty时，将周围八个坐标全部打开
		if(content_empty_status(fp,lfp->y,lfp->x))
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
		//当打开的坐标为雷时，直接返回1表示游戏结束
		else if(content_mine_status(fp,lfp->y,lfp->x))
		{
			return 1;
		}
	}
	//当需要被打开的坐标为已打开状态，且为数字的情况下执行
	else
	{
		if((!content_empty_status(fp,lfp->y,lfp->x))
		&& (!content_mine_status(fp,lfp->y,lfp->x)))
		{
			for(i = MAX(lfp->y-1,0);i <= MIN(lfp->y+1,WIDE-1);i++)
			{	
				for(j = MAX(lfp->x-1,0);j <= MIN(lfp->x+1,LENGTH-1);j++)
				{
					if(flag_status(fp,i,j))
					{
						flag_count++;
					} 
				}
			}
			//如果周围flag标记的数量与自己的content相同，代表该数字周围雷已经被全部探查
			//则打开周围所有未打开且未标记的方块
			if(content_number(fp,lfp->y,lfp->x) == flag_count)
			{
				for(i = MAX(lfp->y-1,0);i <= MIN(lfp->y+1,WIDE-1);i++)
				{	
					for(j = MAX(lfp->x-1,0);j <= MIN(lfp->x+1,LENGTH-1);j++)
					{
						if((!open_status(fp,i,j)) 
							&& (!flag_status(fp,i,j)))	
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

/*
 * 标记一个方块（置雷）
 * 第一个参数为雷区的二维数组
 * 第二个参数为被标记的坐标
 */

void flag_user_location(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	//当该坐标的方块被标记时，取消标记
	//当该坐标的方块未被标记时，将其标记
	if(!open_status(fp,lfp->y,lfp->x))
	{
		if(flag_status(fp,lfp->y,lfp->x))
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

/*
 * 检查是否为首次输入
 * 第一个参数为雷区的二维数组
 * 第二个参数为输入的坐标
 */

void check_first_enter(Content_type (*fp)[LENGTH],Location_type *lfp)
{

//当处于win7模式时
//即用户第一个打开的一定为empty否者重置雷区继续打开
//非win7模式为第一个不是雷

#ifdef WIN7_MODE
	while(!content_empty_status(fp,lfp->y,lfp->x))
#else
	while(content_mine_status(fp,lfp->y,lfp->x))
#endif
	{
		init_mine(fp);
	} 
}

/*
 * 接下来的部分为AUTO_MODE的实现函数
 *
 * 注：
 * 在整个程序中
 * 理论上AUTO_MODE下函数可以直接获取雷区的所有内容
 * 但为了实现自动扫雷
 * 在AUTO_MODE下所有函数
 * 只获取了用户肉眼可以获取的参数
 * 包括是否打开
 * 是否被标记
 * 打开不是雷的数字（包括空）
 *
 * NEXT IS AUTO_MODE FUNCTION
 * */
#ifdef AUTO_MODE

/*
 * 随机打开一个坐标
 * 第一个参数为雷区的二维数组
 * 
 * 返回值为1代表打开了雷
 */

int random_open(Content_type (*fp)[LENGTH])
{
	Location_type lfp;
	int i,j;
	//当被打开的数量小于总被打开的4/5时，随机打开某个坐标
	if(opened_count < (((WIDE * LENGTH) - NUMBER)*4/5))
	{
		do
		{
			lfp.x = rand()%LENGTH;
			lfp.y = rand()%WIDE;
		}while(open_status(fp,lfp.y,lfp.x));
	}
	//当被打开的数量大于总被打开的4/5时，直接按顺序寻找未被打开且未被标记的坐标打开
	else
	{
		for(i = 0;i < WIDE;i++)
		{	
			for(j = 0;j < LENGTH;j++)
			{	
				if((!open_status(fp,i,j)) && (!flag_status(fp,i,j)))
				{
					lfp.x = j;
					lfp.y = i;
					i = WIDE;
					j = LENGTH;
				}
			}
		}
	}
	//当第一次输入时，进行检测
	if(AUTO_FIRST_ENTER)
	{
		check_first_enter(fp,&lfp);
		AUTO_FIRST_ENTER = false;
	}	
	//当随机打开雷时，返回1表示游戏结束
	if(open_user_location(fp,&lfp))
	{
		return 1;
	}
	
	//当随机模式函数被执行一次后，揭下来需要执行无脑推理模式
	random_times++;
	AUTO_MODE_RANDOM = true;
	AUTO_MODE_NOHUMAN = false;
	
	return 0;
}

_Bool ana_open(Content_type (*fp)[LENGTH]);
/*
 * 按照目前的雷区
 * 无脑打开可以被打开的
 * 无脑标记可以被标记的
 *
 * 第一个参数为雷区的二维数组
 */
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
			//检查该方块是否为打开状态
			if(open_status(fp,i,j))
			{
				//在已经打开的前提，确定该方块内容是否为数字
				if((!content_empty_status(fp,i,j)) && (!content_mine_status(fp,i,j)))
				{
					//当内容是数字时，确定周围8个未被打开的方块中
					//被标记的雷的数量与未被标记雷的数量
					for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
					{	
						for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
						{
							if(!open_status(fp,itmp,jtmp))	
							{
								if(!flag_status(fp,itmp,jtmp))	
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
					//当发现
					//若将该方块周围未被标记雷的方块全部标记后
					//该方块周围雷数与自己的数字相同
					//即代表状态合理
					//此时将周围所有方块均标记为雷
					if((unopened_block + flaged_block) == (*(fp+i))[j].content)
					{
						for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
						{	
							for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
							{
								if((!open_status(fp,itmp,jtmp)) &&(!flag_status(fp,itmp,jtmp)))	
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
	//将所有可以标记雷的状态标记后
	//双击所有数字（该功能在open_user_location里实现）
	for(i = 0;i < WIDE;i++)
	{	
		for(j = 0;j < LENGTH;j++)
		{
			if(open_status(fp,i,j))
			{
				lfp.x = j;
				lfp.y = i;
				open_user_location(fp,&lfp);
			}
		}
	}
	//若经历上述两个步骤，雷区所有opened_count没有变化，则调用cal_open
	if(opened_count == open_count_tmp)
	{
		//若调用ana_open系列函数，雷区opened_count依旧没有变化
		//则在main的主循环处，关闭该函数实现
		//同时打开随机打开函数的全局变量
		if(!ana_open(fp))
		{
			AUTO_MODE_NOHUMAN = true;
			AUTO_MODE_RANDOM = false;
		}
		else
		{	
			AUTO_MODE_NOHUMAN = false;
		}
	}
	else
	{
		//若执行任何一步均打开了新的方块，下次循环依旧调用该函数。
		AUTO_MODE_NOHUMAN = false;
	}
}
/*
 *清理cal_open的所有疑问标记状态
 *第一个参数输入雷区的二维数组
 */

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
/*
 *判断输入的坐标数组中，是否有某个坐标在每一列都有
 *有的话则返回true，反之false
 *第一个参数输入坐标二维数组
 *第二个参数输入所有合法列中最小的长度
 *第三个参数输入合法列的数量
 */


_Bool location_equal(Location_type (*lfp)[RIGHT_MAX],int waitflag_n, int right_way)
{
	int i,j;
	int x,y;
	
	int right_num = 0;

	x = (*(lfp+waitflag_n))[0].x;
	y = (*(lfp+waitflag_n))[0].y;
	for(i = 0;i < right_way;i++)
	{
		for(j = 0;j < BLOCK_MAX;j++)
		{
			if ((*(lfp+j))[i].x == x && (*(lfp+j))[i].y == y)
			{
				right_num++;
				break;
			}
		}
	}
	if(right_num == right_way && right_way  !=  0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

_Bool ana_open(Content_type (*fp)[LENGTH])
{
	int i,j;
	int itmp,jtmp;
	int itmp2,jtmp2;

	Location_type empty[8];
	int empty_n;
	int mine_n;
	
	int combine_i;
	int combine_j;
	int tmp;
	
	int empty_tmp;
	int mine_tmp;	
	int unquestion_tmp;
	Location_type unquestion[8];
	
	_Bool right_status;
	_Bool right_exe;
	int right_way = 0;
	
	_Bool return_status = false;
	Location_type waitflag[BLOCK_MAX][RIGHT_MAX];
	Location_type waitopen[BLOCK_MAX][RIGHT_MAX];
	int waitflag_n[RIGHT_MAX];
	int waitopen_n[RIGHT_MAX];
	
	for(i = 0;i < WIDE;i++)
	{	
		for(j = 0;j < LENGTH;j++)
		{	
			empty_n = 0;
			mine_n = 0;
			right_way = 0;
			if(open_status(fp,i,j) && (!content_empty_status(fp,i,j)))
			{
				mine_n = content_number(fp,i,j);
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{
						if((!open_status(fp,itmp,jtmp)) && (!flag_status(fp,itmp,jtmp)))
						{
							empty[empty_n].x = jtmp;
							empty[empty_n++].y = itmp;
						}
						else if(flag_status(fp,itmp,jtmp))
						{
							mine_n--;
						}
					}
				}
			}
			g_combine_count = 0;
			g_combine_end = mine_n;
			
			for(itmp = 0;itmp < BLOCK_MAX;itmp++)
			{
				for(jtmp = 0;jtmp < RIGHT_MAX;jtmp++)
				{
					waitflag[itmp][jtmp].x = -1;
					waitflag[itmp][jtmp].y = -1;
					waitopen[itmp][jtmp].x = -1;
					waitopen[itmp][jtmp].y = -1;
				}
			}

			if(mine_n < empty_n)
			{
				combine(empty_n,mine_n);
				for(combine_i = 0;combine_i < g_combine_count;combine_i++)
				{
					right_status = true;
					right_exe = false;
					for(combine_j = 0;combine_j < mine_n;combine_j++)
					{
						tmp = g_combine_result[combine_i][combine_j];
						(*(fp+empty[tmp-1].y))[empty[tmp-1].x].question_status = true;
						(*(fp+empty[tmp-1].y))[empty[tmp-1].x].question_mine = true;
					}
					for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
					{	
						for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
						{
							if((!open_status(fp,itmp,jtmp)) && (!flag_status(fp,itmp,jtmp)))
							{
								if(!((*(fp+itmp))[jtmp].question_status))
								{
									(*(fp+itmp))[jtmp].question_status = true;
									(*(fp+itmp))[jtmp].question_mine = false;
								}
							}
						}
					}
					for(itmp = MAX(i-SCAN_MAX,0);itmp <= MIN(i+SCAN_MAX,WIDE-1);itmp++)
					{	
						for(jtmp = MAX(j-SCAN_MAX,0);jtmp <= MIN(j+SCAN_MAX,LENGTH-1);jtmp++)
						{
							if(open_status(fp,itmp,jtmp) && !content_empty_status(fp,itmp,jtmp) && right_status)
							{
								unquestion_tmp = 0;
								empty_tmp = 0;
								mine_tmp = 0;
								right_exe = true;
								for(itmp2 = MAX(itmp-1,0);itmp2 <= MIN(itmp+1,WIDE-1);itmp2++)
								{	
									for(jtmp2 = MAX(jtmp-1,0);jtmp2 <= MIN(jtmp+1,LENGTH-1);jtmp2++)
									{
										if((!open_status(fp,itmp2,jtmp2))
										 && (!flag_status(fp,itmp2,jtmp2)))
										{
											if(!(*(fp+itmp2))[jtmp2].question_status)
											{
												unquestion[unquestion_tmp].x = jtmp2;
												unquestion[unquestion_tmp].y = itmp2;
												unquestion_tmp++;
											}
											else if(!(*(fp+itmp2))[jtmp2].question_mine)
											{
												empty_tmp++;
											}
											else
											{
												mine_tmp++;
											}
										}
										else if(flag_status(fp,itmp2,jtmp2))
										{
											mine_tmp++;
										}

									}
								}
								if(content_number(fp,itmp,jtmp) < mine_tmp)
								{
									right_status = false;
								}
								else if(content_number(fp,itmp,jtmp) == mine_tmp)
								{
									for(itmp2 = 0;itmp2 < unquestion_tmp;itmp2++)
									{
										(*(fp+unquestion[itmp2].y))[unquestion[itmp2].x]
														.question_status = true;
										(*(fp+unquestion[itmp2].y))[unquestion[itmp2].x]
														.question_mine = false;
									}
								}
								else
								{
									if(unquestion_tmp == content_number(fp,itmp,jtmp) - mine_tmp)
									{
										for(itmp2 = 0;itmp2 < unquestion_tmp;itmp2++)
										{
											(*(fp+unquestion[itmp2].y))[unquestion[itmp2].x]
															.question_status = true;
											(*(fp+unquestion[itmp2].y))[unquestion[itmp2].x]
															.question_mine = true;
										}
									}
								}
							}
						}
					}
					if(right_status && right_exe)
					{
						waitflag_n[right_way] = 0;
						waitopen_n[right_way] = 0;
						for(itmp = MAX(i-SCAN_MAX,0);itmp <= MIN(i+SCAN_MAX,WIDE-1);itmp++)
						{	
							for(jtmp = MAX(j-SCAN_MAX,0);jtmp <= MIN(j+SCAN_MAX,LENGTH-1);jtmp++)
							{
								if((*(fp+itmp))[jtmp].question_status == true
								&& (*(fp+itmp))[jtmp].question_mine == true)
								{
									waitflag[waitflag_n[right_way]][right_way].x = jtmp;
									waitflag[waitflag_n[right_way]][right_way].y = itmp;
									waitflag_n[right_way]++;
								}
								else if((*(fp+itmp))[jtmp].question_status == true
								&& (*(fp+itmp))[jtmp].question_mine == false)
								{
									waitopen[waitopen_n[right_way]][right_way].x = jtmp;
									waitopen[waitopen_n[right_way]][right_way].y = itmp;
									waitopen_n[right_way]++;
								}

							}
						}
						if(right_way > RIGHT_MAX)
						{
							right_exe = false;
							cal_question_clear(fp);
							return false;
						}
						right_way++;
					}
					right_exe = false;
					cal_question_clear(fp);
				}
				for(itmp = 0;itmp < right_way;itmp++)
				{
					waitflag_n[0] = MIN(waitflag_n[0],waitflag_n[itmp]);
					waitopen_n[0] = MIN(waitopen_n[0],waitopen_n[itmp]);
				}
				if(right_way > 0 && right_way < RIGHT_MAX)
				{
					for(itmp = 0;itmp < waitflag_n[0];itmp++)
					{
						if(location_equal(waitflag,itmp,right_way))
						{
							flag_user_location(fp,&(waitflag[itmp][0]));
							return_status = true;
						}
					}
					for(itmp = 0;itmp < waitopen_n[0];itmp++)
					{
						if(location_equal(waitopen,itmp,right_way))
						{
							open_user_location(fp,&(waitopen[itmp][0]));
							return_status = true;
						}
					}
				}
				if(return_status)
				{
					return return_status;
				}
				right_way = 0;
			}
		}
	}
	return return_status;
	
}

#endif

/*
 * main函数
 */

int main(int argc,char *argv[])
{
	Location_type User;
	Location_type *pUser;
	Content_type Content[WIDE][LENGTH];
	Content_type (*pContent)[LENGTH];

	char ch;
	int result = 0;
	_Bool first_enter = true;
	int times = 0;
//定义运行时间
	clock_t mine_start;
	clock_t mine_stop;
	double mine_time;
#ifdef AUTO_MODE
	int win = 0;
	int lose = 0;
#endif
	User.y = WIDE/2;
	User.x = LENGTH/2;
	pContent = &Content[0];
	pUser = &User;
 	
	//重置随机种子
	srand((unsigned int) time(0));
	
	#ifndef AUTO_MODE	
		printf("Print any key to start the game\n");
	#endif
	//非AUTO_MODE下TIMES为1，即用户可以玩1次
	//AUTO_MODE下TIMES在define中定义，用于多次扫雷求胜率
	mine_start = clock();
	for(times = 0;times < TIMES;times++)
	{
		init_mine(pContent);	
		while(1)
		{
		#ifndef AUTO_MODE
			if(kbhit())
			{
				//8546为上下左右
				//0为打开方块
				//.为标记方块
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
		#else	
			//AUTO_MODE模式下，随机打开与无脑打开两个函数交替执行，执行规律在函数内定义
			if(!AUTO_MODE_RANDOM)
			{
				result = random_open(pContent);
			}
			if(!AUTO_MODE_NOHUMAN)
			{
				nohuman_open(pContent);
			}
		#endif
			//打印所有雷区
			if(result)
			{
			//扫雷失败
			#ifndef AUTO_MODE
				printf("You open the mine and failed!\n");
			#else	
				lose++;
				//print_block(pContent,pUser);
			#endif
				break;
			}
			else if(opened_count == WIDE*LENGTH - NUMBER)
			{
			//未被打开的方块与雷总数相同，扫雷成功
			#ifndef AUTO_MODE
				printf("You win the game!\n");
			#else
				win++;
			#endif
				break;
			}
		}
#ifdef AUTO_MODE
		//AUTO_MODE多次扫雷情况下重置参数
		random_times = 0;
		opened_count = 0;
		marked_count = 0;
		AUTO_FIRST_ENTER = true;
		printf("you win %d times and lose %d times,win percent = %.3f%%\n",win,lose,(float)(win)/(win+lose)*100);
#endif	
	}
	mine_stop = clock();
	mine_time = (double)(mine_stop - mine_start) / CLOCKS_PER_SEC;    
#ifndef AUTO_MODE   	
	printf( "you used %f seconds\n",mine_time);
#else
	
	printf("Finished!\nyou calculate %d times\nwin %d times\nlose %d times\nwin percent %.3f%%\n",
		times,win,lose,(float)(win)/(win+lose)*100);
	printf( "use %.5f seconds\naverage use %.5f seconds\none second can solve %.5f times\n",
		mine_time,mine_time/(double)times,(double)times/mine_time); 
#endif
	return 0;
}
