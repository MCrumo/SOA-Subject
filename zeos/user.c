#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     
     //provar d'escriure en la direccio de memoria etc
     char *buff;
     buff = "\n Test de write a user.c\n";
     if(write(1, buff, strlen(buff)) == -1) perror();
     
     void * pag = alloc();
     if ((int)pag == -1) perror();
     
     int ret = dealloc(pag);
     if (ret == -1) perror();

  while(1) { }
}
