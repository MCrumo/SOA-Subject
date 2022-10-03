# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2

.globl write; .type write, @function; .align 0; write:
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

.globl gettime; .type gettime, @function; .align 0; gettime:
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
