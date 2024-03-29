#include <asm.h>

ENTRY(syscall_sysenter)
	push %ecx
	push %edx
	push $SYSENTER_RETURN
	push %ebp
	mov %esp, %ebp
	sysenter
ENTRY(SYSENTER_RETURN)
	pop %ebp
	pop %edx
	pop %edx
	pop %ecx
	ret

/* int write(int fd, char *buffer, int size) */
ENTRY(write)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx;  // Save EBX, ESI and EDI if modified
	movl $4, %eax
	movl 0x8(%ebp), %ebx;	//fd
	movl 0xC(%ebp), %ecx;	//buffer
	movl 0x10(%ebp), %edx;	//size
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok	// if (eax < 0) -> 
	popl %ebp
	ret

/* Common code for negative return */
nok:
	neg %eax
	mov %eax, errno
	mov $-1, %eax
	popl %ebp
	ret

/* int gettime() */
ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp
	movl $10, %eax
	call syscall_sysenter
	popl %ebp
	ret

/* int getpid() */
ENTRY(getpid)
	pushl %ebp
	movl %esp, %ebp
	movl $20, %eax
	call syscall_sysenter
	popl %ebp
	ret

/* int fork() */
ENTRY(fork)
	pushl %ebp
	movl %esp, %ebp
	movl $2, %eax
	call syscall_sysenter
	test %eax, %eax
	js nok	// if (eax < 0) -->
	popl %ebp
	ret

/* void exit() */
ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp
	movl $1, %eax
	call syscall_sysenter
	popl %ebp
	ret

/* int yield() */
ENTRY(yield)
	pushl %ebp
	movl %esp, %ebp
	movl $13, %eax
	call syscall_sysenter
	popl %ebp
	ret

/* int get_stats(int pid, struct stats *st) */
ENTRY(get_stats)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx;  // Save EBX, ESI and EDI if modified
	movl $35, %eax
	movl 0x8(%ebp), %ebx;	//pid
	movl 0xC(%ebp), %ecx;	//st
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok	// if (eax < 0) -->
	popl %ebp
	ret

/* void *alloc() */
ENTRY(alloc)
	pushl %ebp
	movl %esp, %ebp
	movl $21, %eax
	call syscall_sysenter
	test %eax, %eax
	js nok;	// if (eax < 0) -->
	popl %ebp
	ret

/* int dealloc(void *address) */	
ENTRY(dealloc)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   // *address
	movl $22, %eax //entrada 22
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;	// if (eax < 0) -->
	popl %ebp
	ret

/* int createthread(int (*function)(void *param), void *param) */
ENTRY(createthread)			// PARAMETRES VEN PASSATS??
	pushl %ebp
	movl %esp, %ebp			//HACER SAVE DEL ECX
	pushl %ebx;  			// Save EBX, ESI and EDI if modified
	pushl %ecx
	pushl %edx
	movl $23, %eax 			//entrada 23
	
	movl $twrap, %ebx		//thread wrapper
	movl 0x8(%ebp), %ecx;	//int (*function)(void *param)
	movl 0xC(%ebp), %edx;	//void *param

	call syscall_sysenter
	popl %edx
	popl %ecx
	popl %ebx
	test %eax, %eax
	js nok					// if (eax < 0) -->
	popl %ebp
	ret

/* int terminatethread() */
ENTRY(terminatethread)
	pushl %ebp
	movl %esp, %ebp
	movl $24, %eax 			//entrada 24
	call syscall_sysenter
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret

/* int dump_screen(void *address) */	
ENTRY(dump_screen)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   // *address
	movl $25, %eax 			//entrada 25
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret

/* int get_key(char* c) */
ENTRY(get_key)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   // *address
	movl $26, %eax 			//entrada 26
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret

/* int sem_init (int n_sem, unsigned int value) */
ENTRY(sem_init)
	pushl %ebp
	movl %esp, %ebp			
	pushl %ebx;  			// Save EBX, ESI and EDI if modified
	pushl %ecx
	movl $27, %eax 			//entrada 27
	movl 0x8(%ebp), %ebx;	//int n_sem
	movl 0xC(%ebp), %ecx;	//int value
	call syscall_sysenter
	popl %ecx
	popl %ebx
	test %eax, %eax
	js nok					// if (eax < 0) -->
	popl %ebp
	ret

/* int sem_wait (int n_sem) */
ENTRY(sem_wait)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   //int n_sem
	movl $28, %eax 			//entrada 28
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret

/* int sem_signal (int n_sem) */
ENTRY(sem_signal)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   //int n_sem
	movl $29, %eax 			//entrada 29
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret

/* int sem_destroy (int n_sem) */
ENTRY(sem_destroy)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl 0x8(%ebp), %ebx;   //int n_sem
	movl $30, %eax 			//entrada 30
	call syscall_sysenter
	popl %ebx
	test %eax, %eax
	js nok;					// if (eax < 0) -->
	popl %ebp
	ret
