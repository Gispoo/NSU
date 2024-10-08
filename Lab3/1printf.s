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
	subl	$48, %esp
	call	___main
	movl	$1, %ecx
	movl	$0, %ebx
	fld1
	fstpt	16(%esp)
	movl	$-1, %edx
	jmp	L4
L7:
	fstp	%st(0)
L4:
	movl	%edx, 28(%esp)
	fildl	28(%esp)
	movd	%ecx, %xmm0
	movd	%ebx, %xmm1
	punpckldq	%xmm1, %xmm0
	movq	%xmm0, 32(%esp)
	fildq	32(%esp)
	fadd	%st(0), %st
	fadds	LC0
	fdivrp	%st, %st(1)
	fldt	16(%esp)
	faddp	%st, %st(1)
	fld	%st(0)
	fstpt	16(%esp)
	negl	%edx
	addl	$1, %ecx
	adcl	$0, %ebx
	movl	%ecx, %eax
	xorl	$-1294967296, %eax
	orl	%ebx, %eax
	jne	L7
	fstpt	4(%esp)
	movl	$LC1, (%esp)
	call	__Z6printfPKcz
	fnstcw	46(%esp)
	movzwl	46(%esp), %eax
	orb	$12, %ah
	movw	%ax, 44(%esp)
	fldt	16(%esp)
	fldcw	44(%esp)
	fistpl	16(%esp)
	fldcw	46(%esp)
	movl	16(%esp), %eax
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
