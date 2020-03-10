
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

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

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
    mov %rsp, %rdi
    xor %rsi, %rsi
    mov (interruptNumber), %sil
    call HandleInterrupt

    // Switch stack
    movq %rax, %rsp

    // Restore register contents
    popq %rax
    popq %rbx
    popq %rcx
    popq %rdx

    popq %rsi
    popq %rdi
    popq %rbp

    //add $8, %rsp

.global IgnoreInterrupt
IgnoreInterrupt:

    iretq

.data
    interruptNumber: .byte 0
