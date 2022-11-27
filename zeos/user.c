#include <libc.h>

char buff[24];

int pid;


short stos(char ch, char color){  //screen to short
  color = color << 8;
  return (short)((ch & 0x00FF) | (color & 0xFF00));
} 


int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     
     //provar d'escriure en la direccio de memoria etc

  /*
     char *buff;
     buff = "\n Test de write a user.c\n";
     if(write(1, buff, strlen(buff)) == -1) perror();

  */
    int NUM_ROWS = 25;
    int NUM_COLUMNS = 80;
    char *buff;

    int sizeof_parameter = 2;
    int *matrix;
     
/*
    for(short i = 0; i < NUM_ROWS; ++i){
      int act_row = sizeof_parameter * NUM_COLUMNS * i;
      buff = "\n anem DECLARANT tete\n";
     if(write(1, buff, strlen(buff)) == -1) perror();
      for(short j = 0; j < NUM_COLUMNS; ++j){
        *(matrix + act_row + (j * sizeof_parameter)) = stos('t',0x04); //(short)(('t' & 0x00FF) | (short)(0x03 << 8 )); // // @ini + FIL + COLact
      }
    }*/

    matrix = alloc(); // = &ORIG matrix;

    if((int)matrix == -1) perror();

    buff = "\n ALLOC FET\n";
    if(write(1, buff, strlen(buff)) == -1) perror();

    for (int i = 0; i < 25*80; i += 1){
      buff = "fem...\n";
      if(write(1, buff, strlen(buff)) == -1) perror();
      *(matrix + i) = 0x0361; //stos('t',0x04);
       
    }

     buff = "\n MATRIU FETA TETE\n";
     if(write(1, buff, strlen(buff)) == -1) perror();
     
     dump_screen(matrix);
     if (dealloc(matrix) == -1) perror();

  while(1) { }
}
