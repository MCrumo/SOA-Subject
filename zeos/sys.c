/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

#define MAX_SEM 8

int global_PID=1000;
int global_TID=0;

void * get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);
  
  /* Allocate pages for DATA+STACK */
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  for (pag=0; pag<NUM_PAG_DATA; pag++)
  {
    new_ph_pag=alloc_frame();
    if (new_ph_pag!=-1) /* One page allocated */
    {
      set_ss_pag(process_PT, PAG_LOG_INIT_DATA+pag, new_ph_pag);
    }
    else /* No more free pages left. Deallocate everything */
    {
      /* Deallocate allocated pages. Up to pag. */
      for (i=0; i<pag; i++)
      {
        free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
        del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
      }
      /* Deallocate task_struct */
      list_add_tail(lhcurrent, &freequeue);
      
      /* Return error */
      return -EAGAIN; 
    }
  }

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }
  /* Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    /* Map one child page to parent's address space. */
    set_ss_pag(parent_PT, pag+NUM_PAG_DATA, get_frame(process_PT, pag));
    copy_data((void*)(pag<<12), (void*)((pag+NUM_PAG_DATA)<<12), PAGE_SIZE);
    del_ss_pag(parent_PT, pag+NUM_PAG_DATA);
  }
  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.PID=++global_PID;
  uchild->task.state=ST_READY;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) 
{
  char localbuffer [TAM_BUFFER];
  int bytes_left;
  int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

void sys_exit()
{  
  int i;

  page_table_entry *process_PT = get_PT(current());

  // Deallocate all the propietary physical pages
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

unsigned int alloc_k()
{
  // get_frame(id_pl) retorna su pag asociada i 0 si esta vacia
  unsigned int fpage;
  int lpage;
  int found = 0;
  for (int i = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; i < TOTAL_PAGES && !found; ++i){
    fpage = get_frame(get_PT(current()), i);
    if (fpage == 0) {
      lpage = i;
      found = 1;
    }
  }
  
  if (found){
      int frame = alloc_frame();
      if (frame == -1) {
          return -ENOMEM;
      }
      
      set_ss_pag(get_PT(current()), lpage, frame);
      unsigned int logical_address = lpage << 12;
      
    return logical_address;
  }
  else {
    return -ENOMEM;
  }
}

void * sys_alloc()
{
  return (void *) alloc_k();
}

int dealloc_k(unsigned int lpage)
{
  //comprovar que no sigui de kernel etc i que estigui associada a una fisica
  if ((lpage < NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA) || (lpage >= TOTAL_PAGES)) {
    return -EACCES;
  }
    
  page_table_entry * PT = get_PT(current());
  unsigned int frame = get_frame(PT, lpage);
  if (frame == 0) { //get frame
    return -EFAULT;
  }
    
  free_frame(frame);
  del_ss_pag(PT, lpage);
  set_cr3(get_DIR(current())); //flush tlb
  
  return 0;
}

int sys_dealloc(void *address) 
{
  unsigned int lpage = (unsigned int)address >> 12;
  return dealloc_k(lpage);  
}

int sys_createthread(int (*twrap), int (*function)(void *param), void *param)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;

  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;
  lhcurrent = list_first(&freequeue);
  list_del(lhcurrent);

  uchild = (union task_union*)list_head_to_task_struct(lhcurrent);

  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));

  /* Hem de fer un alloc de pagina lliure, posar la pila de
   l'usuari i que torni per la pila de usr per executar la funcio*/
  unsigned long* user_stack = (unsigned long*)alloc_k();
  
  user_stack[1021] = NULL;
  user_stack[1022] = (unsigned long)function;
  user_stack[1023] = (unsigned long)param;
  //1023 ss; 1022 esp; 1021 palab de estado; 1020 cs; 1019 eip => on tenim el parametre; tocar esp i eip
  //uchild->stack[KERNEL_STACK_SIZE-2] = (unsigned long)&user_stack;      //esp Potser li hem de passar directament l'adreça user_stack[1021]
  uchild->stack[KERNEL_STACK_SIZE-2] = (unsigned long)&user_stack[1021];
  uchild->stack[KERNEL_STACK_SIZE-5] = (unsigned long)twrap;     //eip LA DEL WRAPPER

  uchild->task.register_esp = (unsigned long)&uchild->stack[KERNEL_STACK_SIZE-18];
  /*cambiar el contesto hw de este thread  modificar cont hw para que canduo vuelva a usr la pila 
  que utilizaras es a que acabo de alocatar i la instr que haras el la 1a instr de la funcion
  me alicao un pag para la pila de usr empilar parametro i poner un 0 (= dir de retorno)
  unsigned long * 1023 es la ultima y donde hay que comenzar a empilar
  *  QUINES DADES HEM DE COPIAR EXACTAMENT entre thread 1 i 2 ???
  *  com fer que EIP apunti a (*function) i SS a stak de |@ret|*func|*param|

  CREAR UN CAMP global_TID i TID a la task_union (shced.h) */
  uchild->task.TID=++global_TID; // TID

   /* HEM DE FER UN stats to 0 o NO perq es compartit? */
  //init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  //que el reg 
  
  return 0; 
}

int sys_terminatethread()
{
  //HEM D'ALLIVERAR EL alloc() que fem al createthread
  unsigned long* task_union = (unsigned long*)current();
  unsigned long user_stack = task_union[KERNEL_STACK_SIZE-2]; //adreca a la pila d'usuari
  unsigned int lpage = (unsigned int)user_stack >> 12;
  int isError = dealloc_k(lpage);
  if (isError < 0) return isError;
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  /* Restarts execution of the next process */
  sched_next_rr();

  return 0;
}

int sys_dump_screen(void *address)
{ 
  //address corresponding to an 80x25 matrix; is valid?
  if (!access_ok(VERIFY_WRITE, address, sizeof(void*))) return -EFAULT;
  /*unsigned int lid = (unsigned int)address >> 12;
  // Is a valid range of logPage id? 
  if ((lid < NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA) || (lid >= TOTAL_PAGES)){
    return -EACCES;
  }
  // Is this logPage assigned, so it can be liberated?
  else{
    unsigned int fpage = get_frame(get_PT(current()), lid);
    if (fpage == 0) return -EACCES;
  }*/

  int k = 0;

  for(Byte i = 0; i < NUM_ROWS; ++i){
    for(Byte j = 0; j < NUM_COLUMNS; ++j){
      printcc_xy(j, i, *((short*)address + k)); 
      ++k;
    }
  }
  
  return 0;  
}

int sys_get_key(char* c)
{
  if (!access_ok(VERIFY_WRITE, c, sizeof(char*))) return -EFAULT;
  int isSomethingToRead = read_from_buffer(c);
  if (isSomethingToRead == 0) return 0;
  else return -1;
}


/* _____SEMAPHORES_____ */

typedef struct {
  int isInit;
  int counter;
  struct list_head blocked; //sacar del ready, se encola en semaforo i hace un sched next
} semaphore;

semaphore list_sem[MAX_SEM];

int valid_sem(int n_sem, int isNew) 
{
  if ((n_sem >= MAX_SEM) || (n_sem < 0)) return -1;
  if (isNew){
    if (list_sem[n_sem].isInit != NULL) return -1; //is already initialized
  }
  else { //isNotNew
    if (list_sem[n_sem].isInit == 0) return -1; //is not initialized
  }
  return 0;
}

int sys_sem_init(int n_sem, unsigned int value) 
{
  if (valid_sem(n_sem, 1) != 0) return -1; 
  list_sem[n_sem].counter = value;
  INIT_LIST_HEAD(&list_sem[n_sem].blocked);
  list_sem[n_sem].isInit = 1;
  return 0;
}

int sys_sem_wait(int n_sem) 
{
  if (valid_sem(n_sem, 0) != 0) return -1;
  list_sem[n_sem].counter--;
  if (list_sem[n_sem].counter < 0){
    list_add_tail(&(current()->list), &list_sem[n_sem].blocked);
    sched_next_rr();
  }
  if (current()->destroyed == 1) {
    current()->destroyed = 0;
    return -1;
  }
  return 0;
}

int sys_sem_signal(int n_sem) 
{
  if (valid_sem(n_sem, 0) != 0) return -1;
  list_sem[n_sem].counter++;
  if (list_sem[n_sem].counter <= 0){
    struct list_head *lib = list_first(&list_sem[n_sem].blocked);
    list_del(lib);
    list_add_tail(lib, &readyqueue);
  }
  return 0;
}

int sys_sem_destroy(int n_sem) //todos los sem destroy a 1 , lo tenemos el en task struct
{
  if (valid_sem(n_sem, 0) != 0) return -1;
  //unblock if there are blocked processes
  while (!list_empty(&list_sem[n_sem].blocked)){
    struct list_head *lib = list_first(&list_sem[n_sem].blocked);
    list_del(lib);
    struct task_struct *l = list_head_to_task_struct(lib);
    l->destroyed = 1;
    list_add_tail(lib, &readyqueue);
  }
  list_sem[n_sem].isInit = 0;
  
  return 0;
}

