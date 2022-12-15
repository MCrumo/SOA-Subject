#include <libc.h>

#define NUM_ROWS 25
#define NUM_COLUMNS 80
char c;

/*
0x0 -> Black
0x1 -> Blue Dark
0x2 -> Green 
0x3 -> Cian
0x4 -> Red
0x5 -> Magenta
0x6 -> Orange
0x7 -> White
0x8 -> Grey
0x9 -> Magenta
0xA -> Green 
0xB -> Cian
0xC -> Red pastel 
0xD -> Pink
0xE -> Yellow
0xF -> Wite
0x1X-> UNDERLINED Blue Dark
0x2X-> Underlined Green
0x3X-> Underlined Cian
*/

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

typedef struct {
  int alien;
  int alien_shoot;

  int shield;

  int ship;
  int ship_shoot;

  int wall;
} cell;

cell board[NUM_ROWS][NUM_COLUMNS];
char alien = 'w';
char ship = 'x';
char shield = 'o';
char alien_shoot = 't';
char ship_shoot = 'i';
char wall = 'o';

int dead_zone = 20;
int x_aliens = 3;
int y_aliens = 2;

int is_wall(int i, int j){
  return (i==1 || (i==NUM_ROWS-1) || (j==0 && i!=0) || (j==NUM_COLUMNS-1 && i!=0));
}

void init_board(cell board[NUM_ROWS][NUM_COLUMNS]){
  for (int i=0; i<NUM_ROWS; ++i){
    for (int j=0; j<NUM_COLUMNS; ++j){
      // put WALL
      if (is_wall(i,j)) board[i][j].wall=1;
      // put SHIP
      if (i==NUM_ROWS-2 && j==NUM_COLUMNS/2){
        board[i][j].ship = 1;
        board[i-1][j].ship = 1;
        board[i][j-1].ship = 1;
        board[i][j+1].ship = 1;
        board[i][j-2].ship = 1;
        board[i][j+2].ship = 1;
      }
      // put ALIENS
      if ((i==x_aliens || i==x_aliens+2 || i==x_aliens+4) && (j>=y_aliens && j%2==0 && j<53)) board[i][j].alien=1;

    }
  }
}

void setup(){
  init_board(board);
  short* matrix = alloc();
  for (int i=0; i<NUM_ROWS; ++i){
    for (int j=0; j<NUM_COLUMNS; ++j){
      if (board[i][j].alien == 1) *(matrix + (NUM_COLUMNS*i) + j) = stos(alien, 0x2F);
      if (board[i][j].wall == 1) *(matrix + (NUM_COLUMNS*i) + j) = stos(wall, 0x3F);
      if (board[i][j].ship == 1) *(matrix + (NUM_COLUMNS*i) + j) = stos(ship, 0x4F);
    }
  }
  dump_screen(matrix);
  dealloc(matrix);
  
}

char buff[24];
int pid;

void func_thread2(void *addr){
  if (dump_screen(addr) == -1) perror();
  if (dealloc(addr) == -1) perror();
  if (terminatethread() == -1) perror();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{

  setup();

  while(1) {
  
    if (get_key(&c) == 0){
      if(write(1, &c, sizeof(&c)) == -1) perror();

      short* matrix;
      matrix = alloc();
      if((int)matrix == -1) perror();

      for (int i = 0; i < NUM_COLUMNS*NUM_ROWS; ++i){
        *(matrix + i) = stos('.',0x03);
      } 
      /*  80*i + x
      for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_COLUMNS; ++j) {
          matrix[i][j] = stos('.',0x03);
        }
      } */

      /* if (sem_init(0, 0) == -1) perror();
      if (sem_destroy(0) == -1) perror();
      if (sem_signal(0) == -1) perror();
      if (sem_wait(0) == -1) perror(); */

      if (createthread(func_thread2, (void*)matrix) == -1) perror();
      
    }
  }
}
