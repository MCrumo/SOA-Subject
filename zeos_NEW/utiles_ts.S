#include <asm.h>
#include <segment.h>

ENTRY(task_switch)
	push %ebp
	mov %esp, %ebp

	push %ebx
	push %esi
	push %edi

	push 8(%ebp)
	call inner_task_switch
	addl $4, %esp

	pop %edi
	pop %esi
	pop %ebx

	mov %ebp,%esp
	pop %ebp 
	ret