#include <libc.h>

#define NUM_ROWS 25
#define NUM_COLUMNS 80

char buff[24];

int pid;

int isWall(int i){
  if ((i%NUM_COLUMNS==0&&i>NUM_ROWS)||(i>(NUM_COLUMNS*(NUM_ROWS-1)))) {
    return 1; //esquerra i a baix
  }
  else if ((i>NUM_COLUMNS&&i<NUM_COLUMNS*2)||((i%NUM_COLUMNS-79)==0&&i>NUM_COLUMNS)){
    return 1; //a dalt i dreta
  }
  else return 0;
}
int isRoof(int i){
  if ((i>(NUM_COLUMNS*(NUM_ROWS-1)))||(i>NUM_COLUMNS&&i<NUM_COLUMNS*2)) return 1;
  else return 0;
}
int isLateral(int i){
  if ((i%NUM_COLUMNS==0&&i>NUM_ROWS)||((i%NUM_COLUMNS-79)==0&&i>NUM_COLUMNS)) return 1;
  else return 0;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     
     //provar d'escriure en la direccio de memoria etc
    
    char *buff;
    char c;
    short *matrix;

    matrix = alloc(); // = &ORIG matrix;

    if((int)matrix == -1) perror();

    buff = "\n ALLOC FET\n";
    if(write(1, buff, strlen(buff)) == -1) perror();

    for (int i = 0; i < NUM_COLUMNS*NUM_ROWS; ++i){
      if (isLateral(i)) *(matrix + i) = stos('+',0x03);
      else if (isRoof(i)) *(matrix + i) = stos('+',0x03);
      if (i==1500) *(matrix + i) = stos('O',0x02);
    }

    buff = "\n MATRIU FETA TETE\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
     
    if (dump_screen(matrix) == -1 ) perror();


    if (dealloc(matrix) == -1) perror();


  while(1) {

    if (get_key(&c) == 0){
      if(write(1, &c, sizeof(&c)) == -1) perror();

      matrix = alloc();
      if((int)matrix == -1) perror();
      for (int i = 0; i < NUM_COLUMNS*NUM_ROWS; ++i){
        *(matrix + i) = stos('.',0x03);
      }

      if (sem_init(0, 0) == -1) perror();

      //if (sem_wait(0) == -1) perror();
      //if (sem_signal(0) == -1) perror();

      if (sem_destroy(0) == -1) perror();

      if (createthread(dump_screen, matrix) == -1) perror();
      
      if (dealloc(matrix) == -1) perror();
    }
  }
}
