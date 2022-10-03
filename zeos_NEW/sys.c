/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}


int sys_gettime() {
	return zeos_ticks;
}

char buffer_ds[256];  //El podriem posar dins?

int sys_write(int fd, char * buffer, int size) {
    // Si el valor es menor que 0, hi ha hagut un error.
    int fd_error = check_fd(fd, ESCRIPTURA);
    if(fd_error < 0) return fd_error; // En cas d'error retornem el valor negatiu

    if(buffer == NULL) return -EFAULT;
    if(size < 0) return -EINVAL;

    int left_bytes, used_bytes;
    left_bytes = size;

    while(left_bytes > 256){ //256 és el tamany del buffer
        copy_from_user(buffer + (size - left_bytes), buffer_ds, 256);
        used_bytes = sys_write_console(buffer_ds, 256);
        
        buffer += 256;
        left_bytes -= used_bytes;
    }

    // Si queden menys de 256 bytes per llegir del buffer, els copia
    copy_from_user(buffer + (size - left_bytes), buffer_ds, left_bytes);
    used_bytes = sys_write_console(buffer_ds, left_bytes);
    left_bytes -= used_bytes;

    return (size - left_bytes);
}
