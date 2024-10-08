	.file	"main.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	andl	$-16, %esp
	subl	$16, %esp
	call	___main
	movl	$1, %ecx
	movl	$0, %ebx
	fld1
	movl	$-1, %edx
L2:
	movl	%edx, 8(%esp)
	fildl	8(%esp)
	movd	%ecx, %xmm0
	movd	%ebx, %xmm1
	punpckldq	%xmm1, %xmm0
	movq	%xmm0, (%esp)
	fildq	(%esp)
	fadd	%st(0), %st
	fadds	LC0
	fdivrp	%st, %st(1)
	faddp	%st, %st(1)
	negl	%edx
	addl	$1, %ecx
	adcl	$0, %ebx
	movl	%ecx, %eax
	xorl	$-1294967296, %eax
	orl	%ebx, %eax
	jne	L2
	fnstcw	14(%esp)
	movzwl	14(%esp), %eax
	orb	$12, %ah
	movw	%ax, 12(%esp)
	fldcw	12(%esp)
	fistpl	8(%esp)
	fldcw	14(%esp)
	movl	8(%esp), %eax
	leal	-8(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC0:
	.long	1065353216
	.ident	"GCC: (GNU) 13.1.0"
