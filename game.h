#include<stdbool.h>

#define HARD

#define AUTO_MODE
#define WIN7_MODE


typedef struct Location
{
    int x;
    int y;
}Location_type;

enum MineContent
{
	empty = 0,
	one,
	two,
	three,
	four,
	five,
	six,
	seven,
	eight,
	mine
};

typedef struct BlockContent
{
	_Bool mine_status;
	_Bool open_status;
	_Bool flag_status;
	enum MineContent content;
#ifdef AUTO_MODE
	_Bool question_status;
	int   all_number;
	int   mine_number;
#endif	
}Content_type;

extern int sh_getch(void);
extern int kbhit(void);
