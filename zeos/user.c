#include <libc.h>
#include <list.h>

#define NUM_ROWS 25
#define NUM_COLUMNS 80

#define SEG 18 //retard moviment ia
#define SEG_ZEOS 18 //ticks per second zeos

#define EASY 1

#define BUFF_SIZE 64
#define NULL 0

char c;
extern int gettime();


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
int WIN = 0;
int dead_zone = 19;
int x_aliens = 3;
int y_aliens = 2;
int x_ship = NUM_ROWS-2;
int y_ship = NUM_COLUMNS/2;
int move_right = 1;
int move_left = 0;
int move_down = 0;
int last_shoot = 3;
int shoot_pointer = 1;
int fps = 0;
int pfd = 0;

int t0 = 0;
short * mat;

/*============= BUFFER CIRCULAR BAKA BAKA =============*/

short * frame_buff[BUFF_SIZE];
int Head = 0;
int Tail = 0;
int IsFull_Flag = 0;

int pending_frames() {
  if (Head < Tail) return (Head + BUFF_SIZE) - Tail;
  else if (Head > Tail) return Head-Tail;
  else {
      if (IsFull_Flag) return BUFF_SIZE;
      else return 0;
  }
}
void push_frame(short * mat){
  //the buff is not full
  if (IsFull_Flag == 0) { 
    frame_buff[Head] = mat;
    Head = (Head + 1)%BUFF_SIZE;
    if (Head == Tail) IsFull_Flag = 1;
  }
}
short * read_frame(){
  //the buffer is empty
  if ((Head == Tail) && (IsFull_Flag != 1)){
    return NULL;
  } 
  else {
    short * tmp = frame_buff[Tail];
    Tail = (Tail + 1)%BUFF_SIZE;
    IsFull_Flag = 0;
    return tmp;
  }
}

/*===================================================*/

int ticks_prveis = 0;
int total_frames = 0;

void compute_fps() {
    //int aux_frames = total_frames;
    int ticks = gettime();
    if (ticks-ticks_prveis >= SEG_ZEOS) { //ha passat 1seg
        ticks_prveis = ticks;
        fps = total_frames;
        total_frames = 0;
    }
}
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
      if ((i==x_aliens || i==x_aliens+4) && (j>=y_aliens && j%2==0 && j<52)) board[i][j].alien=1;
      if (i==x_aliens+2 && j>=y_aliens && j%2==1 && j<51) board[i][j].alien=1;
      // put SHIELDS
      if (((j>9&&j<9*2)||(j>9*3&&j<9*4)||(j>9*5&&j<9*6)||(j>9*7&&j<9*8))&&(i==dead_zone||i==dead_zone-1)) board[i][j].shield=1;
    }
  }
}
void board_to_screen(short * matrix){
  char cent;
  char unit;
  int a = 0; //offsets
  int b = 7; //offsets
  //Position x y
  *(matrix) = stos('x',0x6); *(matrix+1) = stos(':',0x7);
  itoa(y_ship/10, &cent); *(matrix+2) = stos(cent,0x6);
  itoa(y_ship%10, &unit); *(matrix+3) = stos(unit,0x6);
  *(matrix+5) = stos('y',0x6); *(matrix+6) = stos(':',0x7);
  *(matrix+7) = stos('2',0x6); *(matrix+8) = stos('2',0x6);
  //FPS
  compute_fps();
  *(matrix+10+a) = stos('f',0x4); *(matrix+11+a) = stos('p',0x4); 
  *(matrix+12+a) = stos('s',0x4); *(matrix+13+a) = stos(':',0x7);
  itoa(fps/10, &cent); *(matrix+14+a) = stos(cent,0x4);
  itoa((int)fps%10, &unit); *(matrix+15+a) = stos(unit,0x4);
  
  //PFD
  pfd = pending_frames();
  *(matrix+10+a+b) = stos('p',0xC); *(matrix+11+a+b) = stos('f',0xC); 
  *(matrix+12+a+b) = stos('d',0xC); *(matrix+13+a+b) = stos(':',0x7);
  itoa(pfd/10, &cent); *(matrix+14+a+b) = stos(cent,0xC);
  itoa(pfd%10, &unit); *(matrix+15+a+b) = stos(unit,0xC);
  
  //HEAD I TAIL
  int auxxx = 7;
  *(matrix+10+a+b+auxxx) = stos('h',0xC); *(matrix+11+a+b+auxxx) = stos('e',0xC);
  *(matrix+12+a+b+auxxx) = stos('a',0xC);
  *(matrix+13+a+b+auxxx) = stos('d',0xC); *(matrix+14+a+b+auxxx) = stos(':',0x7);
  itoa(Head/10, &cent); *(matrix+15+a+b+auxxx) = stos(cent,0xC);
  itoa(Head%10, &unit); *(matrix+16+a+b+auxxx) = stos(unit,0xC);
  
  int auxxx2 = 15;
  *(matrix+10+a+b+auxxx2) = stos('t',0xC); *(matrix+11+a+b+auxxx2) = stos('a',0xC); 
  *(matrix+12+a+b+auxxx2) = stos('i',0xC); *(matrix+13+a+b+auxxx2) = stos('l',0xC);
  *(matrix+14+a+b+auxxx2) = stos(':',0x7);
  itoa(Tail/10, &cent); *(matrix+15+a+b+auxxx2) = stos(cent,0xC);
  itoa(Tail%10, &unit); *(matrix+16+a+b+auxxx2) = stos(unit,0xC);
  
  
  //board
  for (int i=1; i<NUM_ROWS; ++i){
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
void thread_push() {
  if (sem_wait(0) == -1) perror();
  mat = alloc();
  board_to_screen(mat);
  push_frame(mat);
  if (sem_signal(0) == -1) perror();
}
void thread_dump(void *addr){
  while(1){
    //short mat;
    if (sem_wait(0) == -1) perror(); //EVITO RACE CONDITION BAKABAKA
    short * tmp = read_frame();
    if (tmp != NULL) {//maybe NULL 
        //fps_global = CalcAverageTick(gettime());
        if (dump_screen(tmp) == -1) perror(); //NO ES & 
        ++total_frames;
        if (dealloc(tmp) == -1) perror();
    }
    
    if (sem_signal(0) == -1) perror();
  }
  
  terminatethread();
}

void welcome() {
    if (sem_wait(0) == -1) perror();
    mat = alloc();
    
    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_COLUMNS; ++j) {
            *(mat + (NUM_COLUMNS*i)+j) = stos(' ', BLACKFLASBG );
        }
    }
    
    *(mat + (NUM_COLUMNS*6) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 32) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 36) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 40) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 44) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 48) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*6) + 49) = stos(' ', REDBG ); 
*(mat + (NUM_COLUMNS*7) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*7) + 47) = stos(' ', REDBG ); 
*(mat + (NUM_COLUMNS*8) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*8) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*8) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 32) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 36) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 40) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 48) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*8) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*8) + 70) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*9) + 10) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*9) + 14) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*9) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*9) + 65) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*9) + 69) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*10) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 10) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 11) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 12) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 13) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 14) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 32) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 44) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 48) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*10) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 65) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 66) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 67) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 68) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 69) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*10) + 70) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*11) + 8) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 11) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 12) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 13) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 16) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 63) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 66) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 67) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 68) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 70) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*11) + 71) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*12) + 7) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 8) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 10) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 11) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 12) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 13) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 14) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 16) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 17) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 62) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 63) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 65) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 66) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 67) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 68) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 69) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 70) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 71) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*12) + 72) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*13) + 7) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 10) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 11) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 12) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 13) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 14) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 17) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 24) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 25) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 26) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 28) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 38) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 42) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 46) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 50) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 51) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 53) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 54) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 55) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*13) + 62) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 65) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 66) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 67) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 68) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 69) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 70) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*13) + 72) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*14) + 7) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 9) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 15) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 17) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 25) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 28) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 29) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 51) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 53) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*14) + 62) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 64) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 70) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*14) + 72) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*15) + 10) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 11) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 13) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 14) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 25) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 28) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 30) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 38) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 46) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 50) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 51) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 53) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 54) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 55) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*15) + 65) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 66) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 68) = stos(' ', GREENBG ); *(mat + (NUM_COLUMNS*15) + 69) = stos(' ', GREENBG ); 
*(mat + (NUM_COLUMNS*16) + 25) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 28) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 30) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 33) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 35) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 43) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 50) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*16) + 55) = stos(' ', REDBG ); 
*(mat + (NUM_COLUMNS*17) + 24) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 25) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 26) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 28) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 31) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 34) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 37) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 39) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 41) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 42) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 45) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 46) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 47) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 49) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 51) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 53) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 54) = stos(' ', REDBG ); *(mat + (NUM_COLUMNS*17) + 55) = stos(' ', REDBG );
    
    push_frame(mat);
    if (sem_signal(0) == -1) perror();
}

void setup(){
  sem_init(0, 1); //DECLARO SEMAFORO 0 EXCLUSION MUTUA
  createthread(thread_dump, &frame_buff);
  welcome(); //PROTEGIDA POR SEMÁFOROS
  while (gettime() < SEG_ZEOS*20); //Retardo
  init_board(board);
  thread_push();
}
void aliens_down(){
  int total_aliens = 0;
  for (int i = 22; i >= 3; --i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].alien == 1){
        ++total_aliens;
        board[i][j].alien = 0;
        if (board[i][j].ship_shoot!=1){
          board[i+1][j].alien = 1;
          if (board[i][j].shield == 1) board[i][j].shield = 0;
        }
      }
    }
  }
  if (total_aliens == 0) WIN = 1;
}
void aliens_right(){
  int total_aliens = 0;
  for (int i = 3; i < 22; ++i){
    for (int j = NUM_COLUMNS-3; j >= 2; --j){
      if (board[i][j].alien){
        ++total_aliens;
        if (board[i][j].shield == 1) board[i][j].shield = 0;
        board[i][j].alien = 0;
        //if (board[i][j].ship_shoot!=1) 
        board[i][j+1].alien = 1;
      }
    }
  }
  if (total_aliens == 0) WIN = 1;
}
void aliens_left(){
  int total_aliens = 0;
  for (int i = 3; i < 22; ++i){
    for (int j = 2; j <= NUM_COLUMNS-3;++j){
      if (board[i][j].alien){
        ++total_aliens;
        if (board[i][j].shield == 1) board[i][j].shield = 0;
        board[i][j].alien = 0;
        //if (board[i][j].ship_shoot!=1) 
        board[i][j-1].alien = 1;
      }
    }
  }
  if (total_aliens == 0) WIN = 1;
}
int shoot_colision(){
  if (board[x_ship][y_ship].alien_shoot) return 1;
  if (board[x_ship-1][y_ship].alien_shoot) return 1;
  if (board[x_ship][y_ship-1].alien_shoot) return 1;
  if (board[x_ship][y_ship-2].alien_shoot) return 1;
  if (board[x_ship][y_ship+1].alien_shoot) return 1;
  if (board[x_ship][y_ship+2].alien_shoot) return 1;
  else return 0;
}
void ship_right(){
  if (y_ship < NUM_COLUMNS-5){
    board[x_ship][y_ship+3].ship = 1;
    board[x_ship-1][y_ship+1].ship = 1;
    board[x_ship][y_ship-2].ship = 0;
    board[x_ship-1][y_ship].ship = 0;
    ++y_ship;
    if (shoot_colision()) LOSE = 1;
  }
}
void ship_left(){
  if (y_ship > 4){
    board[x_ship][y_ship-3].ship = 1;
    board[x_ship-1][y_ship-1].ship = 1;
    board[x_ship][y_ship+2].ship = 0;
    board[x_ship-1][y_ship].ship = 0;
    --y_ship;
    if (shoot_colision()) LOSE = 1;
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
  if (EASY && board[x_ship-2][y_ship].ship_shoot!=1) board[x_ship-2][y_ship].ship_shoot = 1;
  else {
    if (board[x_ship-3][y_ship].ship_shoot!=1&&board[x_ship-2][y_ship+1].ship_shoot!=1&&board[x_ship-2][y_ship-1].ship_shoot!=1){
      board[x_ship-2][y_ship].ship_shoot = 1;
    }
  }
}
void ia_shoot_alien(){
  int max_aliens_shoots = 3;
  int count = 0;
  int hard = 1;
  short is_alien[80];
  if (EASY){ 
    hard = 2;
    max_aliens_shoots = 1;
  }
  if(gettime()%hard == 0){
    for (int m = 0; m < 80; ++m) is_alien[m] = 0;
    for (int i = 23; i >= 2; --i){
      for (int j = 2; j <= NUM_COLUMNS-3; ++j){
        if (board[i][j].alien){
          if (is_alien[j] == 0) is_alien[j] = i;
        }
      }
    }
    for (int k = last_shoot; k < 80 && count < max_aliens_shoots; ++k){
      if (is_alien[k] > 0){
        board[is_alien[k]+1][k].alien_shoot = 1;
        ++count;
        ++shoot_pointer;
        last_shoot = y_aliens+(49-(50/shoot_pointer));
        shoot_pointer = shoot_pointer%5;
      }
    }
  }
}
void ia_move(){
  move_shoots();
  move_aliens();
  ia_shoot_alien();
}
void game_win() {
  if (sem_wait(0) == -1) perror();
  mat = alloc();
  board_to_screen(mat);
  int off = 0;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('Y', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('O', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('U', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos(' ', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('W', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('I', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('N', REDFLASHBG + WHITE);
  push_frame(mat);
  if (sem_signal(0) == -1) perror();
}
void game_over() {
  if (sem_wait(0) == -1) perror();
  mat = alloc();
  board_to_screen(mat);
  int off = 0;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('G', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('A', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('M', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('E', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos(' ', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('O', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('V', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('E', REDFLASHBG + WHITE); ++off;
  *(mat + (NUM_COLUMNS*13) + off + 36) = stos('R', REDFLASHBG + WHITE);
  push_frame(mat);
  if (sem_signal(0) == -1) perror();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  setup();

  while(1){

    while(!LOSE && !WIN){
      if(get_key(&c) == 0){
        if(c == 'a'){
          ship_left();
          thread_push();
        }
        else if (c == 'd'){
          ship_right();
          thread_push();
        }
        else if (c == 'w'){
          make_ship_shoot();
          thread_push();
        }
      }
      if (gettime()-t0 > SEG){
        ia_move();
        thread_push();
        t0 = gettime();
      }
    }
    if (LOSE) game_over();
    else if (WIN) game_win();
    while(1);
  }
}
