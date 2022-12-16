#include <libc.h>

#define NUM_ROWS 25
#define NUM_COLUMNS 80

char c;
extern int gettime();


char buff[24];
int pid;

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

typedef struct {
  int alien;
  int alien_shoot;

  int shield;

  int ship;
  int ship_shoot;

  int wall;
} cell;

cell board[NUM_ROWS][NUM_COLUMNS];

short alien = 0x0A00 | 'w'; 
short ship =  0x0900 | 'x'; 
short shield = 0x0800 | 'o';
short alien_shoot = 0x0E00 | 't'; 
short ship_shoot = 0x0C00 | 'i';  
short wall = 0x1000 | 0;

int LOSE = 0;
int dead_zone = 19;
int x_aliens = 3;
int y_aliens = 2;
int x_ship = NUM_ROWS-2;
int y_ship = NUM_COLUMNS/2;
int move_right = 1;
int move_left = 0;
int move_down = 0;

int is_wall(int i, int j){
  return ((i==1)||(i==NUM_ROWS-1)||((j==0||j==1) && i!=0)||((j==NUM_COLUMNS-1||j==NUM_COLUMNS-2) && i!=0));
}

void init_board(cell board[NUM_ROWS][NUM_COLUMNS]){
  for (int i=0; i<NUM_ROWS; ++i){
    for (int j=0; j<NUM_COLUMNS; ++j){
      // put WALL
      if (is_wall(i,j)) board[i][j].wall=1;
      // put SHIP
      if (i==x_ship && j==y_ship){
        board[i][j].ship = 1;
        board[i-1][j].ship = 1;
        board[i][j-1].ship = 1;
        board[i][j+1].ship = 1;
        board[i][j-2].ship = 1;
        board[i][j+2].ship = 1;
      }
      // put ALIENS
      if ((i==x_aliens || i==x_aliens+2 || i==x_aliens+4) && (j>=y_aliens && j%2==0 && j<52)) board[i][j].alien=1;
      // put SHIELDS
      if (((j>9&&j<9*2)||(j>9*3&&j<9*4)||(j>9*5&&j<9*6)||(j>9*7&&j<9*8))&&(i==dead_zone||i==dead_zone-1)) board[i][j].shield=1;
      //trying shoots--------
      if (i==x_aliens+5 && j == 5) board[i][j].alien_shoot=1;
    }
  }
}

void board_to_screen(short* matrix){
  for (int i=0; i<NUM_ROWS; ++i){
    for (int j=0; j<NUM_COLUMNS; ++j){
      if (board[i][j].alien == 1) *(matrix + (NUM_COLUMNS*i) + j) = alien;
      else if (board[i][j].wall == 1) *(matrix + (NUM_COLUMNS*i) + j) = wall;
      else if (board[i][j].ship == 1) *(matrix + (NUM_COLUMNS*i) + j) = ship;
      else if (board[i][j].shield == 1) *(matrix + (NUM_COLUMNS*i) + j) = shield;
      else if (board[i][j].alien_shoot == 1) *(matrix + (NUM_COLUMNS*i) + j) = alien_shoot;
      else if (board[i][j].ship_shoot == 1) *(matrix + (NUM_COLUMNS*i) + j) = ship_shoot;
      else *(matrix + (NUM_COLUMNS*i) + j) = 0x0000;
    }
  }
}

void setup(){
  init_board(board);
  short* matrix = alloc();
  board_to_screen(matrix);
  dump_screen(matrix);
  dealloc(matrix);
}

void aliens_down(){
  for (int i = 22; i >= 3; --i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].alien == 1){
        board[i][j].alien = 0;
        if (board[i][j].ship_shoot!=1){
          board[i+1][j].alien = 1;
          if (board[i][j].shield == 1) board[i][j].shield = 0;
        }
      }
    }
  }
}
void aliens_right(){
  for (int i = 3; i < 22; ++i){
    for (int j = NUM_COLUMNS-3; j >= 2; --j){
      if (board[i][j].alien){
        if (board[i][j].shield == 1) board[i][j].shield = 0;
        board[i][j].alien = 0;
        if (board[i][j].ship_shoot!=1) board[i][j+1].alien = 1;
      }
    }
  }
}
void aliens_left(){
  for (int i = 3; i < 22; ++i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].alien){
        if (board[i][j].shield == 1) board[i][j].shield = 0;
        board[i][j].alien = 0;
        if (board[i][j].ship_shoot!=1) board[i][j-1].alien = 1;
      }
    }
  }
}
void ship_right(){
  if (y_ship < NUM_COLUMNS-5){
    board[x_ship][y_ship+3].ship = 1;
    board[x_ship-1][y_ship+1].ship = 1;
    board[x_ship][y_ship-2].ship = 0;
    board[x_ship-1][y_ship].ship = 0;
    ++y_ship;
  }
}
void ship_left(){
  if (y_ship > 4){
    board[x_ship][y_ship-3].ship = 1;
    board[x_ship-1][y_ship-1].ship = 1;
    board[x_ship][y_ship+2].ship = 0;
    board[x_ship-1][y_ship].ship = 0;
    --y_ship;
  }
}

void move_aliens(){
  if (move_down){
    aliens_down();
    ++x_aliens;
    if (x_aliens >= 18) LOSE = 1;
    move_down = 0;
    if (y_aliens >= 29) move_left = 1;
    else if (y_aliens <= 2) move_right = 1;
    else move_down = 1;
  }
  else if (move_right){
    aliens_right();
    ++y_aliens;
    if (y_aliens == 29){
      move_right = 0;
      move_down = 1;
    }
  }
  else if (move_left){
    aliens_left();
    --y_aliens;
    if (y_aliens == 2){
      move_left = 0;
      move_down = 1;
    }
  }
}

void move_shoots_aliens(){
  for (int i = 23; i >= 2; --i){
    for (int j = 2; j <= NUM_COLUMNS-3; ++j){
      if (board[i][j].alien_shoot){
        if (board[i][j].shield){ 
          board[i][j].shield = 0;
        }
        else if (i<NUM_ROWS-2 && (board[i][j].ship_shoot!=1||board[i+1][j].ship_shoot!=1)) board[i+1][j].alien_shoot = 1;
        if (board[i][j].ship) {
          LOSE = 1;
        }
        board[i][j].alien_shoot = 0;
      }
    }
  }
}

void move_shoots_ship(){
  for (int i = 2; i < 24; ++i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].ship_shoot){
        if (board[i][j].alien) {
          board[i][j].alien = 0; 
        }
        else if (!board[i-1][j].shield && (i>=3) && (board[i][j].alien_shoot!=1||board[i-1][j].alien_shoot!=1)) board[i-1][j].ship_shoot = 1;
        board[i][j].ship_shoot = 0;
      }
    }
  }
}
void cancel_shoots(){
  for (int i = 2; i < 24; ++i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].alien_shoot == 1 && board[i+1][j].ship_shoot == 1){
        board[i][j].alien_shoot = 0;
        board[i+1][j].ship_shoot = 0;
      }
    }
  }
}

void move_shoots(){
  cancel_shoots();
  move_shoots_aliens();
  move_shoots_ship();
}
void make_ship_shoot(){
  board[x_ship-2][y_ship].ship_shoot = 1;
}

void ia_shoot_alien(){
  int max_aliens_shoots = 3;
  int count;
  short is_alien[80];
  for (int m = 0; m < 80; ++m) is_alien[m] = 0;
  for (int i = 23; i >= 2; --i){
    for (int j = 2; j <= NUM_COLUMNS-3; ++j){
      if (board[i][j].alien && is_alien[j] == 0) is_alien[j] = i;
    }
  }
  for (int k = 0; k < 80 && count <= max_aliens_shoots; ++k){
    if (is_alien[k] > 0){
      if (gettime()%3 == 0) {
        board[is_alien[k]][k].alien_shoot = 1;
        ++count;
      }
    }
  }
}

void ia_move(){
  move_shoots();
  move_aliens();
  ia_shoot_alien();
}

void func_thread2(void *addr){
  if (dump_screen(addr) == -1) perror();
  if (dealloc(addr) == -1) perror();
  if (terminatethread() == -1) perror();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  
  setup();
  int t0 = gettime();
  //while(gettime()-t0 > 2){}
  while(1){
    if(get_key(&c) == 0 && !LOSE){
      if(c == 'a'){
        short*mat = alloc();
        ship_left();
        board_to_screen(mat);
        dump_screen(mat);
        dealloc(mat); 
      }
      else if (c == 'd'){
        short*mat = alloc();
        ship_right();
        board_to_screen(mat);
        dump_screen(mat);
        dealloc(mat);
      }
      else if (c == 's'){
        short*mat = alloc();
        make_ship_shoot();
        ia_move();
        board_to_screen(mat);
        dump_screen(mat);
        dealloc(mat);
      }
      else{
        if (LOSE == 1) write(1,"LOSE",4);
        short*mat = alloc();
        ia_move();
        board_to_screen(mat);
        dump_screen(mat);
        dealloc(mat);
      }
    }
  }
  /*
  while(1) {

    setup();
    

    while(!LOSE){
      if (gettime()-t0 > 1) {
        t0 = gettime();
        
      }
    }
  
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
      if (sem_wait(0) == -1) perror();

      if (createthread(func_thread2, (void*)matrix) == -1) perror();
      
      
    }
  }*/
}
