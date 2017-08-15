#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
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
	#define SPEED       100*1000
	#define TIMES	    10000	
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
#endif	

int opened_count = 0;
int marked_count = 0;

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
	//根据雷的分布，将剩余方块赋予数值（0~8）
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

/*
 * 打印单格方块
 * 第一个参数为雷区的二维数组
 * 第二个参数为被打印的方块的纵坐标
 * 第三个参数为被打印的方块的横坐标
 */ 

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
		system("clear");//清除屏幕	
		for(i = 0;i < WIDE;i++)
		{
			for(j = 0;j < LENGTH;j++)
			{	
				if(m == 0 || i != lfp->y || j != lfp->x)			
				{
					if(!(*(fp+i))[j].open_status)
					{
						if(0)
						{
						}
						#ifdef AUTO_MODE
						else if((*(fp+i))[j].question_status)
						{
							printf("?");//当处于AUTO_MODE下时，用来标记cal_open所赋予的怀疑状态
						}
						#endif
						else if((*(fp+i))[j].flag_status)
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
		if((*(fp+pUser->y))[pUser->x].open_status)
		{
			if((*(fp+pUser->y))[pUser->x].content == empty
				|| (*(fp+pUser->y))[pUser->x].content == mine)
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
	if((*(fp+lfp->y))[lfp->x].open_status == false 
		&& (*(fp+lfp->y))[lfp->x].flag_status == false)	
	{
		(*(fp+lfp->y))[lfp->x].open_status = true;
		opened_count++;
		//当打开的坐标为empty时，将周围八个坐标全部打开
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
		//当打开的坐标为雷时，直接返回1表示游戏结束
		else if((*(fp+lfp->y))[lfp->x].content == mine)
		{
			return 1;
		}
	}
	//当需要被打开的坐标为已打开状态，且为数字的情况下执行
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
			//如果周围flag标记的数量与自己的content相同，代表该数字周围雷已经被全部探查
			//则打开周围所有未打开且未标记的方块
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

/*
 * 标记一个方块（置雷）
 * 第一个参数为雷区的二维数组
 * 第二个参数为被标记的坐标
 */

void flag_user_location(Content_type (*fp)[LENGTH],Location_type *lfp)
{
	//当该坐标的方块被标记时，取消标记
	//当该坐标的方块未被标记时，将其标记
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
	while((*(fp+lfp->y))[lfp->x].content != empty)
#else
	while((*(fp+lfp->y))[lfp->x].content == mine)
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
		}while((*(fp+lfp.y))[lfp.x].open_status);
	}
	//当被打开的数量大于总被打开的4/5时，直接按顺序寻找未被打开且未被标记的坐标打开
	else
	{
		for(i = 0;i < WIDE;i++)
		{	
			for(j = 0;j < LENGTH;j++)
			{	
				if((!(*(fp+i))[j].open_status) && (!(*(fp+i))[j].flag_status))
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

_Bool cal_open(Content_type (*fp)[LENGTH],int all_number,int mine_number);

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
			if((*(fp+i))[j].open_status)
			{
				//在已经打开的前提，确定该方块内容是否为数字
				if((*(fp+i))[j].content != empty && (*(fp+i))[j].content != mine)
				{
					//当内容是数字时，确定周围8个未被打开的方块中
					//被标记的雷的数量与未被标记雷的数量
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
	//将所有可以标记雷的状态标记后
	//双击所有数字（该功能在open_user_location里实现）
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
	//若经历上述两个步骤，雷区所有opened_count没有变化，则调用cal_open
	if(opened_count == open_count_tmp)
	{
		//若调用cal_open系列函数，雷区opened_count依旧没有变化
		//则在main的主循环处，关闭该函数实现
		//同时打开随机打开函数的全局变量

		if((!cal_open(fp,2,1)) && (!cal_open(fp,3,1)) && (!cal_open(fp,3,2)))
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


/*算法考虑三种情况
2个里有1个
3个里有1个
3个里有2个
即all_number与mine_number组合有以上三种*/

/*
 * 推理算法函数
 * 当N个里有M个雷时，推理附近区域某个雷是雷还是空
 *
 * 第一个参数为雷区的二维数组
 * 第二个参数为N选M中的N
 * 第三个参数为N选M中的M
 *
 * 返回值为是否找到类似的位置并对其判定是或非雷，若有成功判定返回true
 *
 * 函数注释以2选1为例
 *
 * eg.
 * 当处于以下情况或类似情况时（其中某个1为边界）
 *      ■■■┇
 *      121┇
 * 可以推断2上为非雷的函数
 */


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
			
			//当某个方块为打开的情况时，该方块周围缺少mine_number个雷标记时执行
			//eg.
			//
			//某个1周围未有标记雷，即flag_count = 0，content = 1，mine_number = 1
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
			//当该方块周围有all_number个方块未被打开且未被标记时执行
			if(unopened_count == all_number && unopened_count != 0)
			{
				for(itmp = MAX(i-1,0);itmp <= MIN(i+1,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-1,0);jtmp <= MIN(j+1,LENGTH-1);jtmp++)
					{
						//将所有符合条件的方块附加内容，即标记疑问状态
						//同时赋值为all_number中选mine_number
						//eg.
						//被标记的方块有2个，这2个方块中有1个雷
						if((!(*(fp+itmp))[jtmp].open_status) && (!(*(fp+itmp))[jtmp].flag_status))	
						{
							(*(fp+itmp))[jtmp].question_status = true;
							(*(fp+itmp))[jtmp].all_number = all_number;
							(*(fp+itmp))[jtmp].mine_number = mine_number;
						}
					}
				}
				//对以方块为中心，周围5X5的范围内进行寻找数字
				for(itmp = MAX(i-2,0);itmp <= MIN(i+2,WIDE-1);itmp++)
				{	
					for(jtmp = MAX(j-2,0);jtmp <= MIN(j+2,LENGTH-1);jtmp++)
					{
						//当方块为已打开状态且为数字时
						if(((*(fp+itmp))[jtmp].open_status) && ((*(fp+i))[j].content != empty))
						{
							//初始化tmp
							tmp_question.x = LENGTH + 1;
							tmp_question.y = WIDE + 1;
							for(itmp2 = MAX(itmp-1,0);itmp2 <= MIN(itmp+1,WIDE-1);itmp2++)
							{	
								for(jtmp2 = MAX(jtmp-1,0);jtmp2 <= MIN(jtmp+1,LENGTH-1);jtmp2++)
								{
									//寻找未打开且未被标记的方块
									if((!(*(fp+itmp2))[jtmp2].open_status) 
									&& (!(*(fp+itmp2))[jtmp2].flag_status))	
									{
										unopened_count_next++;
										//若找到被疑问标记的，计数并将其参数赋予tmp
										if((*(fp+itmp2))[jtmp2].question_status)
										{
											tmp_question.x = jtmp2;
											tmp_question.y = itmp2;
											question_count++;	
										}	
										//若方块未被疑问标记，计数并详细记录坐标
										else
										{
											tmp_unquestion[unquestion_count].x = jtmp2;
											tmp_unquestion[unquestion_count].y = itmp2;
											unquestion_count++;
										}
									}
									//若被标记，则计数
									else if((*(fp+itmp2))[jtmp2].flag_status)
									{
										flag_count_next++;
									}
								}		
							}
							//若在5X5的某个数字中
							//找到了所有被疑问标记的方块
							//且除了这些被标记的疑问方块，还有未被疑问标记，未被标记的未打开方块时执行
							//
							//即符合推理条件
							if((tmp_question.x != LENGTH + 1)
  								&& (tmp_question.y != WIDE + 1)
  								&& (question_count == (*(fp+tmp_question.y))[tmp_question.x].all_number)
								&& (unopened_count_next > question_count))
							{
								//未被疑问标记方块中包含的雷的数量
								unmine_count =(*(fp+itmp))[jtmp].content 
									-(*(fp+tmp_question.y))[tmp_question.x].mine_number
									-flag_count_next;

								//未被疑问标记方块中无雷，全部打开
								if(unmine_count == 0)
								{
									while(unquestion_count)
									{
										open_user_location(fp,&tmp_unquestion[--unquestion_count]);
										result = true;
									}
								}
								//未被疑问标记方块中均为雷，全部标记
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
				}	
			}
			flag_count = 0;
			unopened_count = 0;
			cal_question_clear(fp);
		}
	}
	return result;
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
	
	#ifdef AUTO_MODE	
		printf("Print any key to start the game\n");
	#endif
	//非AUTO_MODE下TIMES为1，即用户可以玩1次
	//AUTO_MODE下TIMES在define中定义，用于多次扫雷求胜率
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
			//print_block(pContent,pUser);
			if(result)
			{
			//扫雷失败
			#ifndef AUTO_MODE
				printf("You open the mine and failed!\n");
			#else	
				lose++;
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
	return 0;
}
