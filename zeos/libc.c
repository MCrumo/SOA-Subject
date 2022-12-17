/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

#define BLACK       0x0
//DARK VERSIONS
#define DARKBLUE    0x1 //DARK + COLOR NAME
#define DARKGREEN   0x2 
#define DARKCYAN    0x3
#define DARKRED     0x4
#define DARKMAGENTA 0x5
#define DARKORANGE  0x6
#define DARKWHITE   0x7

//NORMAL VERSIONS
#define GREY        0x8 //COLOR NAME
#define BLUE        0x9 
#define GREEN       0xA
#define CYAN        0xB
#define RED         0xC
#define PINK        0xD
#define YELLOW      0xE
#define WHITE       0xF

//BACKGROUND COLOR
#define BLUEBG      0x10 //Blue background      
#define GREENBG     0x20 //Green background     
#define CYANBG      0x30 //Cyan background      //BACKGROUND COLOR + LETTER COLOR
#define REDBG       0x40 //Red background


int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror()
{
  char buffer[256];

  itoa(errno, buffer);

  write(1, buffer, strlen(buffer));
}

void twrap(void (*function)(void *param), void *param)
{
  function(param);
  terminatethread();
}

short stos(char ch, char color)
{  // Screen TO Short
  short col = color << 8;
  return ch | col;
}
