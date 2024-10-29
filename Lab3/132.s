	.file	"main.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	call	___main
	movl	$0, %eax
L2:
	addl	$1, %eax
	cmpl	$-1294967297, %eax
	jne	L2
	movl	$0, %eax
	leave
	ret
	.ident	"GCC: (GNU) 13.1.0"
