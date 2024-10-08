	.file	"main.c"
	.text
	.def	__main;	.scl	2;	.type	32;	.endef
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB44:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$96, %rsp
	.seh_stackalloc	96
	.seh_endprologue
	call	__main
	movq	$3000000, -48(%rbp)
	fld1
	fchs
	fstpt	-16(%rbp)
	fld1
	fstpt	-32(%rbp)
	movq	$1, -40(%rbp)
	jmp	.L2
.L3:
	movq	-40(%rbp), %rax
	addq	%rax, %rax
	addq	$1, %rax
	movq	%rax, -64(%rbp)
	fildq	-64(%rbp)
	fldt	-16(%rbp)
	fdivp	%st, %st(1)
	fldt	-32(%rbp)
	faddp	%st, %st(1)
	fstpt	-32(%rbp)
	fldt	-16(%rbp)
	fchs
	fstpt	-64(%rbp)
	movq	-64(%rbp), %rax
	movl	-56(%rbp), %edx
	movq	%rax, -16(%rbp)
	movl	%edx, -8(%rbp)
	addq	$1, -40(%rbp)
.L2:
	movq	-40(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jl	.L3
	movl	$0, %eax
	addq	$96, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (GNU) 13.1.0"
