#include<stdbool.h>

enum MineContent
{
	empty,
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

struct BlockContent
{
	_Bool mine_status;
	_Bool open_status;
	_Bool flag_status;
	enum MineContent content;
};

struct Location

{
    int x;
    int y;
};

typedef struct BlockContent Content_type;
typedef struct Location Location_type;

extern int sh_getch(void);
extern int kbhit(void);
