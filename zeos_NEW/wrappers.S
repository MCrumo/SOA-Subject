#include <asm.h>

ENTRY(write)
    pushl %ebp
    mov %esp,%ebp
    
    # Salvem context
    pushl %edx
    pushl %ecx

    
    mov 0x08(%ebp), %ebx # fd
    mov 0x0c(%ebp), %ecx # buffer
    mov 0x10(%ebp), %edx # size
    

    movl $4, %eax # escrivim num corresponent
    

    pushl $sys_write_return
    pushl %ebp
    mov %esp,%ebp


    sysenter
    
    # err al sys
sys_write_return:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx
    cmpl $0, %eax
    jge sys_write_NOerror
    # no err
    negl %eax # abs(eax)
    movl %eax, errno
    movl -1, %eax
    
sys_write_NOerror:
    popl %ebp
    ret

ENTRY(gettime)
    push %ebp
    mov %esp,%ebp
    
    # Salvem context
    push %edx
    push %ecx
    
    movl $10, %eax
    
    push $gettime_return
    push %ebp
    mov %esp,%ebp

    sysenter
    

gettime_return:
    # err sys
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    cmp $0, %eax
    jge gettime_NOerror
    # no err
    neg %eax # abs(eax)
    mov %eax, errno
    mov -1, %eax
    
gettime_NOerror:
    pop %ebp
    ret


ENTRY(getpid)
    push %ebp
	mov %esp,%ebp
	push %edx
	push %ecx

	movl $20, %eax

	#push %ecx
	#push %edx

	push $getpid_return
	push %ebp
	mov %esp,%ebp

    # ######## ## ## #         ____  
	sysenter # ## ## ## # <3  |---   |\ |
    # ######## ## ## #        |____  | \| .

getpid_return:
	pop %ebp
	add $4, %esp
	pop %edx
	pop %ecx
	cmp $0, %eax
	jge getpid_NOerror
	neg %eax 
	mov %eax, errno
	mov -1, %eax
getpid_NOerror:
	pop %ebp
	ret

ENTRY(fork)
	pushl %ebp
	mov %esp,%ebp

	#push %edx
	#push %ecx

	movl $0x2, %eax

    pushl %edx
	pushl %ecx

	#push $sys_fork_ret
	#push %ebp
	#mov %esp,%ebp
    pushl %ebp
	movl %esp,%ebp

	# ######## ## ## #         ____  
	sysenter # ## ## ## # <3  |---   |\ |
    # ######## ## ## #        |____  | \| .

sys_fork_ret:
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge sys_fork_NOerror
	negl %eax
	movl %eax, errno
	movl -1, %eax
sys_fork_NOerror:
	popl %ebp
	ret

ENTRY(exit)
	pushl %ebp
	mov %esp,%ebp

    movl $0x1, %eax
	pushl %edx
	pushl %ecx

    pushl %ebp
	movl %esp,%ebp
    
	sysenter

sys_exit_ret:
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebp
	ret