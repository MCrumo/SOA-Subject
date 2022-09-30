#include <libc.h>

char buff[24];

int pid;
extern int zeos_ticks;
extern int gettime();
extern void perror();

int __attribute__ ((__section__(".text.main"))) main(void) {
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    gettime();
    
    /********WRITE********/
    if(write(1, "\n------- \n", 9) == -1) perror();
    char * mesg;
    mesg = "\n [write] Write from user.c\n";
    if(write(1, mesg, strlen(mesg)) == -1) perror();
    if(write(1, "------- \n", 9) == -1) perror();
    

    /*****GETTIME********/
    mesg = "\n [gettime] Testing gettime() from user.c: ";
    if(write(1, mesg, strlen(mesg)) == -1) perror();
    itoa(gettime(), mesg); 
    if(write(1, mesg, strlen(mesg)) == -1) perror();
    mesg = " (gettime result)\n";
    if(write(1, mesg, strlen(mesg)) == -1) perror();
    mesg = "------- \n\n";
    if(write(1, mesg, strlen(mesg)) == -1) perror();
    
    while(1) {}
}
