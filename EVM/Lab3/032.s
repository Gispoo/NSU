	.file	"main.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	call	___main
	movl	$-1294967296, (%esp)
	movl	$0, 4(%esp)
	movl	$-1, 28(%esp)
	fld1
	fstpt	16(%esp)
	movl	$1, 8(%esp)
	movl	$0, 12(%esp)
	jmp	L2
L3:
	fildl	28(%esp)
	fildq	8(%esp)
	fld	%st(0)
	faddp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fdivrp	%st, %st(1)
	fldt	16(%esp)
	faddp	%st, %st(1)
	fstpt	16(%esp)
	negl	28(%esp)
	addl	$1, 8(%esp)
	adcl	$0, 12(%esp)
L2:
	movl	8(%esp), %eax
	movl	12(%esp), %edx
	cmpl	(%esp), %eax
	movl	%edx, %eax
	sbbl	4(%esp), %eax
	jl	L3
	movl	$0, %eax
	leave
	ret
	.ident	"GCC: (GNU) 13.1.0"
