	.file	"kernel.cpp"
	.text
	.section .rdata,"dr"
	.align 8
_ZL12VIDEO_MEMORY:
	.quad	753664
.LC0:
	.ascii "0123456789abcdef\0"
	.align 8
_ZL7symbols:
	.quad	.LC0
.lcomm _ZL1x,4,4
.lcomm _ZL1y,4,4
	.text
	.globl	_Z5clearv
	.def	_Z5clearv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z5clearv
_Z5clearv:
.LFB0:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	$0, -4(%rbp)
.L3:
	cmpl	$1999, -4(%rbp)
	jg	.L2
	movl	-4(%rbp), %eax
	cltq
	addq	%rax, %rax
	addq	$753664, %rax
	movw	$1824, (%rax)
	addl	$1, -4(%rbp)
	jmp	.L3
.L2:
	movl	$0, _ZL1y(%rip)
	movl	_ZL1y(%rip), %eax
	movl	%eax, _ZL1x(%rip)
	nop
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z6printfPKc
	.def	_Z6printfPKc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z6printfPKc
_Z6printfPKc:
.LFB1:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$0, -4(%rbp)
.L10:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L11
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L6
	movl	$0, _ZL1x(%rip)
	movl	_ZL1y(%rip), %eax
	addl	$1, %eax
	movl	%eax, _ZL1y(%rip)
	jmp	.L7
.L6:
	movl	_ZL1y(%rip), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$4, %eax
	movl	%eax, %edx
	movl	_ZL1x(%rip), %eax
	addl	%edx, %eax
	movl	%eax, %eax
	addq	%rax, %rax
	addq	$753664, %rax
	movzwl	(%rax), %eax
	movb	$0, %al
	movl	%eax, %ecx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cbtw
	orl	%eax, %ecx
	movl	_ZL1y(%rip), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$4, %eax
	movl	%eax, %edx
	movl	_ZL1x(%rip), %eax
	addl	%edx, %eax
	movl	%eax, %eax
	addq	%rax, %rax
	addq	$753664, %rax
	movl	%ecx, %edx
	movw	%dx, (%rax)
	movl	_ZL1x(%rip), %eax
	addl	$1, %eax
	movl	%eax, _ZL1x(%rip)
.L7:
	movl	_ZL1x(%rip), %eax
	cmpl	$79, %eax
	jbe	.L8
	movl	$0, _ZL1x(%rip)
	movl	_ZL1y(%rip), %eax
	addl	$1, %eax
	movl	%eax, _ZL1y(%rip)
.L8:
	movl	_ZL1y(%rip), %eax
	cmpl	$24, %eax
	jbe	.L9
	call	_Z5clearv
.L9:
	addl	$1, -4(%rbp)
	jmp	.L10
.L11:
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC1:
	.ascii "hehe xd\0"
	.text
	.globl	_Z6printbv
	.def	_Z6printbv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z6printbv
_Z6printbv:
.LFB2:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	.LC1(%rip), %rcx
	call	_Z6printfPKc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC2:
	.ascii "THIS IS SOME TEXT\12\0"
.LC3:
	.ascii "THIS IS A NEW LINE\12\0"
.LC4:
	.ascii "THIS IS A THIRD LINE\12\0"
.LC5:
	.ascii "THIS IS A NUMBER IN HEX - \0"
	.text
	.globl	kernelMain
	.def	kernelMain;	.scl	2;	.type	32;	.endef
	.seh_proc	kernelMain
kernelMain:
.LFB3:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	call	_Z5clearv
	leaq	.LC2(%rip), %rcx
	call	_Z6printfPKc
	leaq	.LC3(%rip), %rcx
	call	_Z6printfPKc
	leaq	.LC4(%rip), %rcx
	call	_Z6printfPKc
	leaq	.LC5(%rip), %rcx
	call	_Z6printfPKc
	call	_Z6printbv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (GNU) 7.4.0"
