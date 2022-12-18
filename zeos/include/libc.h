/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

extern int errno;

int write(int fd, char *buffer, int size);

void itoa(int a, char *b);

int strlen(char *a);

void perror();

int getpid();

int fork();

void exit();

int yield();

int get_stats(int pid, struct stats *st);

void *alloc();

int dealloc(void *address);

int createthread(void (*function)(void *param), void *param);

int terminatethread();

int dump_screen(void *address);

int get_key(char* c);

int sem_init(int n_sem, unsigned int value);

int sem_wait(int n_sem);

int sem_signal(int n_sem);

int sem_destroy(int n_sem);


short stos(char ch, char color);

void twrap(void (*function)(void *param), void *param);

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


#endif  /* __LIBC_H__ */
