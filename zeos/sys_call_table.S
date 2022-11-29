#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)	
	.long sys_ni_syscall		//0
	.long sys_exit				//1
	.long sys_fork				//2
	.long sys_ni_syscall		//3
	.long sys_write				//4
	.long sys_ni_syscall		//5
	.long sys_ni_syscall		//6
	.long sys_ni_syscall		//7
	.long sys_ni_syscall		//8
	.long sys_ni_syscall		//9
	.long sys_gettime			//10
	.long sys_ni_syscall		//11
	.long sys_ni_syscall		//12
	.long sys_yield				//13
	.long sys_ni_syscall		//14
	.long sys_ni_syscall		//15
	.long sys_ni_syscall		//16
	.long sys_ni_syscall		//17
	.long sys_ni_syscall		//18
	.long sys_ni_syscall		//19
	.long sys_getpid			//20	
	.long sys_alloc				//21 alloc
	.long sys_dealloc	    	//22 dealloc
	.long sys_createthread		//23 createthread
	.long sys_terminatethread	//24 terminatethread
	.long sys_dump_screen		//25 dump_screen
	.long sys_get_key			//26 get_key
	.long sys_sem_init			//27 sem_init
	.long sys_sem_wait			//28 sem_wait
	.long sys_sem_signal		//29 sem_signal
	.long sys_sem_destroy		//30 sem_destroy
	.long sys_ni_syscall		//31
	.long sys_ni_syscall		//32
	.long sys_ni_syscall		//33
	.long sys_ni_syscall		//34
	.long sys_get_stats			//35
.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table)/4
