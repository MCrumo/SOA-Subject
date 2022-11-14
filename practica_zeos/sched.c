/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

#include <interrupt.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;
struct list_head freequeue, readyqueue;
struct task_struct * idle_task; // Variable global idle_task
int quantum_count;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void update_sched_data_rr(void)
{
	--quantum_count;
}

void update_process_state_rr(struct task_struct *ts, struct list_head *dst)
{ 
	struct list_head * list_tmp = &ts->list;
	if(!(list_tmp->prev == NULL && list_tmp->next == NULL)){
		list_del(list_tmp);
	}
	if (dst) list_add_tail(list_tmp, dst);
}

int needs_sched_rr(void)
{
	if(quantum_count > 0) return 0;
	if(list_empty(&readyqueue)){
		quantum_count = current()->quantum;
		return 0;
	}
	return 1;
}

void sched_next_rr(void)
{
	struct task_struct *next;
	if(!list_empty(&readyqueue)){
		struct list_head *listh_first = list_first(&readyqueue);
		list_del(listh_first);
		next = list_head_to_task_struct(listh_first);
	}
	else next = idle_task;
	quantum_count = next->quantum;
	task_switch(next);
}

void schedule(){
	update_sched_data_rr();
	if(needs_sched_rr()) {
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}


void init_idle (void)
{
    // Creem un punter al primer element de la freequeue
    struct list_head *first_lh = list_first(&freequeue);

    // L'eliminem de la freequeue:
    list_del(first_lh);

    // Convertim de list_head a task_struct:
    struct task_struct *aux_ts = list_head_to_task_struct(first_lh);

    // PID = 0 perquè és el primer procés de idle
    aux_ts->PID = 0;

	aux_ts->quantum = QUANTUM;

    // Amb allocate_DIR inicialitzem el dir_pages_baseAaddr perquè serà on guardarem l'espai d'adreces del procés.
    allocate_DIR(aux_ts);

    // Inicializem el context d'execució pel procés per restaurar-lo quan és assignat a cpu i executa cpu_idle().
    // Guardem ebp i l'adreça de retorn.
    union task_union *ts_un = (union task_union*) aux_ts; // Task union corresponent al PCB de idle.
    ts_un -> stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; // @ret
    ts_un -> stack[KERNEL_STACK_SIZE - 2] = (unsigned long) 0;  // fake ebp
    ts_un -> task.kernel_esp = (unsigned int) &(ts_un->stack[KERNEL_STACK_SIZE - 2]); // mov ebp -> esp

    // idle_task = task_struct per accedir al procés idle ara inicialitzat
    idle_task = aux_ts;
}

void init_task1(void)
{
    /* The code of this process is implemented in user.c.
    This function is called from main() in system.c*/

    // Creem un punter al primer element de la freequeue
    struct list_head *t = list_first(&freequeue);

    // L'eliminem de la freequeue:
    list_del(t);

    // Convertim de list_head a task_struct:
    struct task_struct *ts_1 = list_head_to_task_struct(t);

    // PID = 1 perquè és el primer procés de init
    ts_1->PID = 1;

	ts_1->quantum = QUANTUM;

    // Amb allocate_DIR inicialitzem el dir_pages_baseAaddr perquè serà on guardarem l'espai d'adreces del procés.
    allocate_DIR(ts_1);

    // Punt 3 pg 52 del pdf
    set_user_pages(ts_1);

    // Actualitzem TSS per tal que apunti a la pila del sistema new_task
    // Com que utilitzem sysenter, hem de modificar també el MSR 0x175
    
    //DE MOMENT NO HO UTILITZEM
    //union task_union * ts_un1 = (union task_union*) ts_1; // get task_union de la pcb de init
    // A mm.c tenim el camps tss.esp0 i amb el define a sched.h de KERNEL_ESP l'utilitzem per modificar el camp
    tss.esp0 = KERNEL_ESP((union task_union *)ts_1); //esp0 = inici pila usuari
    // Modifiquem el registre SYSENTER_ESP_MSR
    writeMSR(0x175, (int) tss.esp0);

    // Utilitzant la funció set_cr3() assignem la seva pàgina de directori com a la pàgina actual de director del sistema o.o
    set_cr3(ts_1->dir_pages_baseAddr);
}

void inner_task_switch(union task_union *new_task){
	/*Update the pointer to the system stack to point to the stack of new_task. This step depends on the implemented mechanism to enter the system. In the case that the int assembly instruction is used to invoke the system code, TSS.esp0 must be modified to make it point to the stack of new_task. If the system code is invoked using sysenter, MSR number 0x175 must be also modified.*/
	tss.esp0 = KERNEL_ESP((union task_union *)new_task);
	writeMSR(0x175, (int) tss.esp0);

	/*Change the user address space by updating the current page directory: use the set_cr3
	funtion to set the cr3 register to point to the page directory of the new_task.*/
	set_cr3(get_DIR(&(new_task->task)));

	/*Store the current value of the EBP register in the PCB. EBP has the address of the current
	system stack where the inner_task_switch routine begins (the dynamic link).*/
	__asm__ __volatile__ ( 
		"mov %%ebp,%0"
		: "=g" (current()->kernel_esp) 
		:); 

	/*Change the current system stack by setting ESP register to point to the stored value in the
	new PCB.*/
	__asm__ __volatile__ (
		"mov %0, %%esp"
		: 
		: "g" (new_task->task.kernel_esp)); 

	/*Restore the EBP register from the stack.*/
	__asm__ __volatile__ (
		"pop %%ebp"
		: 
		:);

	/*Return to the routine that called this one using the instruction RET (usually task_switch,
	but...). */
	__asm__ __volatile__ (
		"ret"
		:
		:);
}


void init_sched()
{ 
  	//int val1 = add(0x42, 0x666);
  	//int val2 = addASM(0x42, 0x666);
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	for (short i = 0; i < NR_TASKS; ++i){
		list_add( &(task[i].task.list), &freequeue);
	}
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

