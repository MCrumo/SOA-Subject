#include <libc.h>

char buff[24];

int pid;
extern int zeos_ticks;
extern int gettime();
extern void perror();

int __attribute__ ((__section__(".text.main"))) main(void) {

    gettime();
    
    char *buff;
    char *buff_espais;
    buff_espais = "\n===============\n\n";

    /*write____________________________________________________________________*/
    if(write(1, buff_espais, strlen(buff_espais)) == -1) perror();
    buff = "\n Test de write a user.c\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    if(write(1, buff_espais, strlen(buff_espais)) == -1) perror();

    /*gettime__________________________________________________________________*/
    buff = "\n Test de gettime a user.c\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    itoa(gettime(), buff); 
    if(write(1, buff, strlen(buff)) == -1) perror();
    buff = " resultat de gettime())\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    if(write(1, buff_espais, strlen(buff_espais)) == -1) perror();

    /*getpid___________________________________________________________________*/
    buff = "\n Test de getpid: El meu PID: ";
    if(write(1, buff, strlen(buff)) == -1) perror();
    int pid = getpid();
    itoa(pid, buff);
    if(write(1, buff, strlen(buff)) == -1) perror();
    if(write(1, buff_espais, strlen(buff_espais)) == -1) perror();
    
    /*test fork________________________________________________________________*/
    buff="\n Test de fork \n\n";
    if(write(1, buff, strlen(buff)) == -1) perror();
    
    int pid = fork();
    if(pid == 0){
        buff="Soc el procés fill, el meu PID: ";
        if(write(1, buff, strlen(buff)) == -1) perror();
        itoa(getpid(), buff);
        if(write(1, buff, strlen(buff)) == -1) perror();
        buff="\n";
        if(write(1, buff, strlen(buff)) == -1) perror();
        exit();
        
        buff="Procés fill finalitza amb PID: ";
        if(write(1, buff, strlen(buff)) == -1) perror();
        itoa(getpid(), buff);
        if(write(1, buff, strlen(buff)) == -1) perror();
        buff="\n";
        if(write(1, buff, strlen(buff)) == -1) perror();
    }
    else{
        buff="Soc el procés pare, el meu PID: ";
        if(write(1, buff, strlen(buff)) == -1) perror();
        itoa(getpid(), buff);
        if(write(1, buff, strlen(buff)) == -1) perror();
        buff="\n";
        if(write(1, buff, strlen(buff)) == -1) perror();
    }
    /*_______________________________fi testeig________________________________*/
    
    /*
     El fork ens dona page fault, no ho hem sabut arreglar, mirar-nos-ho post exams
     */
    
    while(1) {}
}
