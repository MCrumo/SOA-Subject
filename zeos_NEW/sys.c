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
    int PID=-1; //Encara cal? mantinc, pero més tard en realitat ho edito...

    // Error si freequeue buida
    if(list_empty(&freequeue)) return -EAGAIN;

    // Agafem primer task struct
    struct list_head *first_elem = list_first(&freequeue);
    list_del(first_elem);
    struct task_struct *child_ts = list_head_to_task_struct(first_elem); //PCB del fill

    // Heretem dades sistema del pare al fill
    copy_data((void*)current(), (void*)child_ts, sizeof(union task_union));

    // Inicialitzo pagines fill
    allocate_DIR(child_ts); // Necessito "child_ts->dir_pages_baseAddr = "????

    // Busquem pagines per mapejar-hi les logiques del fill, error si no hi ha suficients lliures
    page_table_entry  *child_ts_PT = get_PT(child_ts);

    //CODI SET PAGES
    // Apartat d)
    int num_pagina[NUM_PAG_DATA];
    for(int i = 0; i < NUM_PAG_DATA; ++i) {
        // Alloc pag, -1 si incorrecte
        num_pagina[i] = alloc_frame();

        if(num_pagina[i] < 0){
            // Alliberem frames assignats
            for(int j = 0; j < i; j++) {
                free_frame(num_pagina[j]);
            }
            
            // PCB de nou a la freequeue
            list_add_tail(&child_ts->list, &freequeue);
            return -ENOMEM; //Retornem error
        }
    }

    // Apartat e)
    page_table_entry *father_PT = get_PT(current()); // e.i)

    // Page table entries for the system code and data and for the user code
    // e.ii.A) Código de sistema y usuario
    unsigned int father_page;
    for(int i = 0; i < NUM_PAG_KERNEL; ++i) {
        father_page = get_frame(father_PT, i);
        set_ss_pag(child_ts_PT, i, father_PT);
    }

    for(int i = 0; i < NUM_PAG_CODE; ++i) {
        father_page = get_frame(father_PT, PAG_LOG_INIT_CODE + i);
        set_ss_pag(child_ts_PT, PAG_LOG_INIT_CODE + i, father_PT);
    }

    // e.ii.B) Page table entries for the user data+stack have to point to new allocated pages which hold this region
    for(int i = 0; i < NUM_PAG_DATA; ++i) {
        set_ss_pag(child_ts_PT, PAG_LOG_INIT_DATA + i, num_pagina[i]);
    }

    // +espai al pare per copiar pagines de pare a fill amb TLB
    int SHARED_SPACE = NUM_PAG_KERNEL + NUM_PAG_CODE;
    int TOTAL_SPACE = NUM_PAG_CODE + NUM_PAG_KERNEL + NUM_PAG_DATA;

    for(int i = SHARED_SPACE; i < TOTAL_SPACE; ++i){
        unsigned int temp_page = i + PAG_LOG_INIT_DATA;
        set_ss_pag(father_PT, temp_page, num_pagina[i]);
        copy_data((void *) (i << 12), (void *) (temp_page << 12), PAGE_SIZE); // pag 4KB, shift 12bits
        del_ss_pag(father_PT, temp_page);
    }

    // FLUSH DE TLB AL FINNNN
    set_cr3(get_DIR(current()));

    // Apartat f) Assignem nou PID i incrementem contador general
    PID = pid_next++;
    child_ts->PID = PID;

    /* Apartat h)
    Representació stack actual per saber on fer les modificacions pertinents
        
        |    0   | <-- fake ebp
        |--------|
        |ret_fork| <-- [KERNEL_STACK_SIZE - 19]
        |--------|
        |@HANDLER|
        |--------| 
        | CTX SW | 11 pos (registers)
        |--------|
        | CTX HW | 5 pos (cpu state, stack pointer and ins pointer)
    
    */
    
    
    union task_union * child_tu = (union task_union *) child_ts;
    ((unsigned long *)KERNEL_ESP(child_tu))[-0x13] = (unsigned long) 0; // valor fake ebp
    ((unsigned long *)KERNEL_ESP(child_tu))[-0x12] = (unsigned long) ret_from_fork; // direcció de retorn
    child_ts->kernel_esp = &((unsigned long *)KERNEL_ESP(child_tu))[-0x13];

    // Apartat j) retornem PID fill
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
