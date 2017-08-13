#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<termio.h>
#include<unistd.h>
#include<fcntl.h>

//windows can use getch()
int sh_getch(void)
{
  int cr;
  struct termios nts,ots;
  
  if(tcgetattr(0,&ots) < 0)
    return EOF;

  nts = ots;
  cfmakeraw(&nts);
  
  if(tcsetattr(0,TCSANOW,&nts) < 0)
    return EOF;

  cr = getchar();
  if(tcsetattr(0,TCSANOW,&ots) < 0)
    return EOF; 

  return cr; 
}
//windows can use kbhit()
int kbhit(void)
{
  struct termios oldt,newt;
  int ch;
  int oldf;
  tcgetattr(STDIN_FILENO,&oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,&newt);
  oldf = fcntl(STDIN_FILENO,F_GETFL,0);
  fcntl(STDIN_FILENO,F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO,TCSANOW,&oldt);  
  fcntl(STDIN_FILENO,F_SETFL,oldf);
  if(ch != EOF)
  {
    ungetc(ch,stdin);
    return 1;
  }
  return 0;
}


