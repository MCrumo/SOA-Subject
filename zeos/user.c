#include <libc.h>

#define NUM_ROWS 25
#define NUM_COLUMNS 80

char buff[24];

int pid;


short stos(char ch, char color){  //screen to short
  short col = color << 8;
  return ch | col;
} 


int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     
     //provar d'escriure en la direccio de memoria etc
    
    char *buff;

    short *matrix;

    matrix = alloc(); // = &ORIG matrix;

    if((int)matrix == -1) perror();

    buff = "\n ALLOC FET\n";
    if(write(1, buff, strlen(buff)) == -1) perror();

    for (int i = 0; i < NUM_COLUMNS*NUM_ROWS; ++i){
      if (i == 0 || i == (NUM_COLUMNS*NUM_ROWS)-1) *(matrix + i) = stos('O',0x01);
      else *(matrix + i) = stos('t',0x03);
    }

     buff = "\n MATRIU FETA TETE\n";
     if(write(1, buff, strlen(buff)) == -1) perror();
     
     dump_screen(matrix);
     if (dealloc(matrix) == -1) perror();

  while(1) { }
}
