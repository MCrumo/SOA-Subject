/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

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

short stos(char ch, char color)
{  // Screen TO Short
  short col = color << 8;
  return ch | col;
}

void twrap(void (*function)(void *param), void *param)
{
  // EXECUTAR LA FUNCIO D'USUARI
  /*char *buff;
  buff = "\n DINS DE TWRAP\n";
  if(write(1, buff, strlen(buff)) == -1) perror();*/
    
    
  function(param);
  terminatethread();
}
