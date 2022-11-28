/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

/**************/
/** Screen  ***/
/**************/

// #define NUM_COLUMNS 80
// #define NUM_ROWS    25

Byte x, y=19;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  if (c=='\n')
  {
    x = 0;
    y=(y+1)%NUM_ROWS;
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | 0x0200;
	Word *screen = (Word *)0xb8000;
	screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1)%NUM_ROWS;
    }
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printc(c);
  x=cx;
  y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}

void printcc(short cc)
{
  char c = cc & 0x00FF;
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  if (c == '\n')
  {
    x = 0;
    y=(y+1)%NUM_ROWS;
  }
  else
  {
    //Word ch = (Word) (c & 0x00FF) | 0x0300; // (c & 0xFF00);
	Word *screen = (Word *)0xb8000;
	screen[(y * NUM_COLUMNS + x)] = cc;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1)%NUM_ROWS;
    }
  }
}

void printcc_xy(Byte mx, Byte my, short cc)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printcc(cc);
  x=cx;
  y=cy;
}


char circular_buff[BUFF_SIZE];
int Head = 0;
int Tail = 0;
int IsFull_Flag = 0;

void write_to_buffer(char c)
{
  if (IsFull_Flag == 0) { //the buff is not full
    circular_buff[Head] = c;
    Head = (Head + 1)%BUFF_SIZE;
    if (Head == Tail) IsFull_Flag = 1;
  }
}

int read_from_buffer(char* c)
{
  if ((Head == Tail)&&(IsFull_Flag != 1)) return -1; //the buffer us empty
  else {
    *c = circular_buff[Tail]; //AQUI DONA PG FAULT al accedir a *c
    Tail = (Tail + 1)%BUFF_SIZE;
    IsFull_Flag = 0;
    return 0;
  }
}