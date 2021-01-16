
.set IRQ_BASE, 0x20

.section .text

.extern HandleInterrupt

.macro pushall
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdi
    pushq %rsi
    pushq %rsp
    pushq %rbp
    pushq %rdx
    pushq %rcx
    pushq %rbx
    pushq %rax
.endm

.macro popall
    popq %rax
    popq %rbx
    popq %rcx
    popq %rdx
    popq %rbp
    popq %rsp
    popq %rsi
    popq %rdi
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15
.endm


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
    pushall

    // Pass arguments and call interrupt handler
    mov %rsp, %rdi
    xor %rsi, %rsi
    mov (interruptNumber), %sil
    call HandleInterrupt

    // Switch stack
    movq %rax, %rsp

    // Restore register contents
    popall


.global IgnoreInterrupt
IgnoreInterrupt:

    iretq


.global SaveContext
SaveContext:

    // Push all registers on the stack
    // Stack after this line looks like:
    //  RIP
    //  R15
    //  ...
    //  RSP  (Old value, Offset 50)
    //  ...
    //  RAX
    // ----- <--- RSP now pointing here
    pushall

    // Adjust the saved stack pointer. InterruptContexts always
    // store an %rsp which has three quadwords on the stack
    // prior to the general-purpose regs, but since we don't
    // use cs or rflags we only have 1.
    sub $16, 50(%rsp)

    // Stack now matches the layout of InterruptContext
    // With the exception of cs and rflags at the end
    // Simply copy the first 136 bytes (17 qwords) and 
    // store cs and rflags manually.
    movq %rsp, %rsi
    movq $17, %rcx
    rep movsq

    // Store cs
    xor %rax, %rax
    movw %cs, %ax
    stosq

    // Store rflags
    pushf
    pop %rax
    stosq

    // Return 0
    popall
    xor %rax, %rax
    ret
    
.global RestoreContext
RestoreContext:

    // Load pointer to InterruptContext
    movq %rdi, %rsi

    // Switch stacks
    movq 50(%rsi), %rsp

    // rsp was saved with 3 quadwords on the stack (rip, cs, rflags).
    // Position rsp so we have 17 quadwords instead. (General purpose
    // registers plus rip, but no cs/rflags.)
    subq $112, %rsp

    // Copy the first 17 quadwords of InterruptContext onto the stack
    movq %rsp, %rdi
    movq $17, %rcx
    rep movsq

    // Restore the general purpose registers and rip
    popall
    ret

.data
    interruptNumber: .byte 0
