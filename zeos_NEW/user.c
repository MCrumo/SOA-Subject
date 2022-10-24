#include <libc.h>

char buff[24];

int pid;
extern int zeos_ticks;
extern int gettime();
extern void perror();

int __attribute__ ((__section__(".text.main"))) main(void) {

    gettime();
    
    char *boff;

    /*write*/
    if(write(1, "\n***** \n", 9) == -1) perror();
    char * buff;
    buff = "\n Test de write a user.c\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    if(write(1, "***** \n", 9) == -1) perror();
    

    /*gettime*/
    buff = "\n Test de gettime a user.c\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    itoa(gettime(), buff); 
    if(write(1, buff, strlen(buff)) == -1) perror();
    buff = " resultat de gettime())\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    buff = "***** \n\n";
    if(write(1, buff, strlen(buff)) == -1) perror();

    /*getpid brbr*/
    boff = "\n getpid() My PID: ";
    if(write(1, boff, strlen(boff)) == -1) perror();
    int pid = getpid();
    itoa(pid, boff);
    if(write(1, boff, strlen(boff)) == -1) perror();
    if(write(1, "\n ****** \n", 9) == -1) perror();
    
    while(1) {}
}
