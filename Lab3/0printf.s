	.file	"main.c"
	.text
	.section	.text$_Z6printfPKcz,"x"
	.linkonce discard
	.globl	__Z6printfPKcz
	.def	__Z6printfPKcz;	.scl	2;	.type	32;	.endef
__Z6printfPKcz:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	leal	12(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %ebx
	movl	$1, (%esp)
	movl	__imp____acrt_iob_func, %eax
	call	*%eax
	movl	%ebx, 8(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	___mingw_vfprintf
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	-4(%ebp), %ebx
	leave
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
	andl	$-16, %esp
	subl	$64, %esp
	call	___main
	movl	$-1294967296, 32(%esp)
	movl	$0, 36(%esp)
	movl	$-1, 60(%esp)
	fld1
	fstpt	48(%esp)
	movl	$1, 40(%esp)
	movl	$0, 44(%esp)
	jmp	L4
L5:
	fildl	60(%esp)
	fildq	40(%esp)
	fld	%st(0)
	faddp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fdivrp	%st, %st(1)
	fldt	48(%esp)
	faddp	%st, %st(1)
	fstpt	48(%esp)
	negl	60(%esp)
	addl	$1, 40(%esp)
	adcl	$0, 44(%esp)
L4:
	movl	40(%esp), %eax
	movl	44(%esp), %edx
	cmpl	32(%esp), %eax
	movl	%edx, %eax
	sbbl	36(%esp), %eax
	jl	L5
	fldt	48(%esp)
	fstpt	4(%esp)
	movl	$LC1, (%esp)
	call	__Z6printfPKcz
	fldt	48(%esp)
	fnstcw	30(%esp)
	movzwl	30(%esp), %eax
	orb	$12, %ah
	movw	%ax, 28(%esp)
	fldcw	28(%esp)
	fistpl	24(%esp)
	fldcw	30(%esp)
	movl	24(%esp), %eax
	leave
	ret
	.ident	"GCC: (GNU) 13.1.0"
	.def	___mingw_vfprintf;	.scl	2;	.type	32;	.endef
