	.file	"main.c"
	.text
	.def	__main;	.scl	2;	.type	32;	.endef
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB46:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	call	__main
	movl	$2999999999, %eax
.L2:
	subq	$1, %rax
	jne	.L2
	movl	$0, %eax
	addq	$40, %rsp
	ret
	.seh_endproc
	.ident	"GCC: (GNU) 13.1.0"
