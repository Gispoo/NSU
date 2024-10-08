	.file	"main.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$48, %esp
	call	___main
	movl	$-1294967296, 16(%esp)
	movl	$0, 20(%esp)
	movl	$-1, 44(%esp)
	fld1
	fstpt	32(%esp)
	movl	$1, 24(%esp)
	movl	$0, 28(%esp)
	jmp	L2
L3:
	fildl	44(%esp)
	fildq	24(%esp)
	fld	%st(0)
	faddp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fdivrp	%st, %st(1)
	fldt	32(%esp)
	faddp	%st, %st(1)
	fstpt	32(%esp)
	negl	44(%esp)
	addl	$1, 24(%esp)
	adcl	$0, 28(%esp)
L2:
	movl	24(%esp), %eax
	movl	28(%esp), %edx
	cmpl	16(%esp), %eax
	movl	%edx, %eax
	sbbl	20(%esp), %eax
	jl	L3
	fldt	32(%esp)
	fnstcw	14(%esp)
	movzwl	14(%esp), %eax
	orb	$12, %ah
	movw	%ax, 12(%esp)
	fldcw	12(%esp)
	fistpl	8(%esp)
	fldcw	14(%esp)
	movl	8(%esp), %eax
	leave
	ret
	.ident	"GCC: (GNU) 13.1.0"
