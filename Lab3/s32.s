	.file	"main.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.section	.text.startup,"x"
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	call	___main
	xorl	%eax, %eax
	leave
	ret
	.ident	"GCC: (GNU) 13.1.0"
