/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

#define NUM_COLUMNS 80
#define NUM_ROWS    25
#define BUFF_SIZE 32

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);

void printcc(short cc);
void printcc_xy(Byte x, Byte y, short cc);

void write_to_buffer(char c);
int read_from_buffer(char* c);

#endif  /* __IO_H__ */
