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
    

    pushl $write_return
    pushl %ebp
    mov %esp,%ebp


    sysenter
    
    # err al sys
write_return:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx
    cmpl $0, %eax
    jge write_no_error
    # no err
    negl %eax # abs(eax)
    movl %eax, errno
    movl -1, %eax
    
write_no_error:
    popl %ebp
    ret

ENTRY(gettime)
    push %ebp
    mov %esp,%ebp
    
    # Salvem context
    push %edx
    push %ecx
    
    movl $10, %eax
    
    push $gett_return
    push %ebp
    mov %esp,%ebp

    sysenter
    

gett_return:
    # err sys
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    cmp $0, %eax
    jge gett_no_error
    # no err
    neg %eax # abs(eax)
    mov %eax, errno
    mov -1, %eax
    
gett_no_error:
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

	push $gettt_return
	push %ebp
	mov %esp,%ebp

    # ######## ## ## #         ____  
	sysenter # ## ## ## # <3  |---   |\ |
    # ######## ## ## #        |____  | \| .

gettt_return:
	pop %ebp
	add $4, %esp
	pop %edx
	pop %ecx
	cmp $0, %eax
	jge gettt_no_error
	neg %eax 
	mov %eax, errno
	mov -1, %eax
gettt_no_error:
	pop %ebp
	ret
