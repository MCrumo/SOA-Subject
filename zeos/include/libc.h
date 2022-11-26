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

int createthread(int (*function)(void *param), void *param);

int terminatethread();

int dump_screen(void *address);

#endif  /* __LIBC_H__ */
