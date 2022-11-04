#include <libc.h>

char buff[24];

int pid;
extern int zeos_ticks;
extern int gettime();
extern void perror();

int __attribute__ ((__section__(".text.main"))) main(void) {

    gettime();
    
    char *buff;

    /*write*/
    if(write(1, "\n***** \n", 9) == -1) perror();
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

    /**********GETPID********/
    buff = "\n [getpid] My PID: ";
    if(write(1, buff, strlen(buff)) == -1) perror();
    int pid = getpid();
    itoa(pid, buff);
    if(write(1, buff, strlen(buff)) == -1) perror();
    buff = "***** \n\n";
    if(write(1, buff, strlen(buff)) == -1) perror();

    /*********FORK**********/
    buff = "[fork] Forking... \n CHIDL PID: ";
    if(write(1, buff, strlen(buff)) == -1) perror();
    int pid_child = fork();
    itoa(pid_child, buff);
    if(write(1, buff, strlen(buff)) == -1) perror();

    buff = "\n\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    buff = "\n[fork] Forking AGAIN... \n CHIDL PID: ";
    if(write(1, buff, strlen(buff)) == -1) perror();
    pid_child = fork();
    itoa(pid_child, buff);
    if(write(1, buff, strlen(buff)) == -1) perror();


    buff = "\n\n[fork] Job is done. \n ***** \n\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
        
    while(1) {}
}
