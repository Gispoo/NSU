	.file	"main.c"
	.text
	.section	.text$_Z6printfPKcz,"x"
	.linkonce discard
	.globl	__Z6printfPKcz
	.def	__Z6printfPKcz;	.scl	2;	.type	32;	.endef
__Z6printfPKcz:
	pushl	%ebx
	subl	$24, %esp
	leal	36(%esp), %ebx
	movl	$1, (%esp)
	call	*__imp____acrt_iob_func
	movl	%ebx, 8(%esp)
	movl	32(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	___mingw_vfprintf
	addl	$24, %esp
	popl	%ebx
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC1:
	.ascii "%d\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	andl	$-16, %esp
	subl	$32, %esp
	call	___main
	movl	$1, %ecx
	movl	$0, %ebx
	fld1
	movl	$-1, %edx
L4:
	movl	%edx, 20(%esp)
	fildl	20(%esp)
	movd	%ecx, %xmm0
	movd	%ebx, %xmm1
	punpckldq	%xmm1, %xmm0
	movq	%xmm0, 24(%esp)
	fildq	24(%esp)
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
	jne	L4
	fstpt	4(%esp)
	movl	$LC1, (%esp)
	call	__Z6printfPKcz
	movl	$0, %eax
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
	.def	___mingw_vfprintf;	.scl	2;	.type	32;	.endef
