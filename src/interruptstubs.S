
.set IRQ_BASE, 0x20

.section .text

.extern HandleInterrupt


// Exceptions
.macro HandleException num
.global HandleException\num

HandleException\num:
    movb $\num, (interruptNumber)
    jmp int_routine

.endm

// Interrupt Requests
.macro HandleInterruptRequest num
.global HandleInterruptRequest\num

HandleInterruptRequest\num:
    movb $\num + IRQ_BASE, (interruptNumber)
    jmp int_routine

.endm

HandleInterruptRequest 0x16

int_routine:
    
    // Store register contents
    pushq %rbp
    pushq %rdi
    pushq %rsi

    pushq %rdx
    pushq %rcx
    pushq %rbx
    pushq %rax

    // Pass arguments and call interrupt handler
    mov %rax, %rdi
    xor %rsi, %rsi
    mov (interruptNumber), %sil
    call HandleInterrupt

    // Switch stack
    //movq %rax, %rsp

    // Restore register contents
    popq %rax
    popq %rbx
    popq %rcx
    popq %rdx

    popq %rsi
    popq %rdi
    popq %rbp

    //add $4, %rsp

.global IgnoreInterrupt
IgnoreInterrupt:

    iretq

.data
    interruptNumber: .byte 0
