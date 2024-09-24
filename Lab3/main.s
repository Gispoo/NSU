	.file	"main.cpp"
	.text
	.section	.text$_ZSt12setprecisioni,"x"
	.linkonce discard
	.globl	_ZSt12setprecisioni
	.def	_ZSt12setprecisioni;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZSt12setprecisioni
_ZSt12setprecisioni:
.LFB5994:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	16(%rbp), %eax
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB15004:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$144, %rsp
	.seh_stackalloc	144
	.seh_endprologue
	call	__main
	rdtsc
	salq	$32, %rdx
	orq	%rdx, %rax
	nop
	movq	%rax, -48(%rbp)
	movl	$3000000000, %eax
	movq	%rax, -56(%rbp)
	movl	$-1, -4(%rbp)
	fld1
	fstpt	-32(%rbp)
	movq	$1, -40(%rbp)
	jmp	.L5
.L6:
	fildl	-4(%rbp)
	fildq	-40(%rbp)
	fld	%st(0)
	faddp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fdivrp	%st, %st(1)
	fldt	-32(%rbp)
	faddp	%st, %st(1)
	fstpt	-32(%rbp)
	negl	-4(%rbp)
	addq	$1, -40(%rbp)
.L5:
	movq	-40(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jl	.L6
	movl	$10, %ecx
	call	_ZSt12setprecisioni
	movl	%eax, %edx
	movq	.refptr._ZSt4cout(%rip), %rax
	movq	%rax, %rcx
	call	_ZStlsIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_St13_Setprecision
	movq	%rax, %rcx
	fldt	-32(%rbp)
	fldt	.LC1(%rip)
	fmulp	%st, %st(1)
	fstpt	-96(%rbp)
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	_ZNSolsEe
	movq	%rax, %rcx
	movq	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%rip), %rax
	movq	%rax, %rdx
	call	_ZNSolsEPFRSoS_E
	rdtsc
	salq	$32, %rdx
	orq	%rdx, %rax
	nop
	movq	%rax, -64(%rbp)
	fldt	.LC2(%rip)
	fstpt	-80(%rbp)
	movq	-64(%rbp), %rax
	subq	-48(%rbp), %rax
	movq	%rax, %rdx
	movq	.refptr._ZSt4cout(%rip), %rax
	movq	%rax, %rcx
	call	_ZNSolsEy
	movq	%rax, %rcx
	movq	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%rip), %rax
	movq	%rax, %rdx
	call	_ZNSolsEPFRSoS_E
	movq	%rax, %rcx
	movq	-64(%rbp), %rax
	subq	-48(%rbp), %rax
	movq	%rax, -104(%rbp)
	fildq	-104(%rbp)
	testq	%rax, %rax
	jns	.L8
	fldt	.LC3(%rip)
	faddp	%st, %st(1)
.L8:
	fldt	-80(%rbp)
	fdivrp	%st, %st(1)
	fstpt	-96(%rbp)
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	_ZNSolsEe
	movl	$0, %eax
	addq	$144, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE:
	.byte	1
_ZNSt8__detail30__integer_to_chars_is_unsignedImEE:
	.byte	1
_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE:
	.byte	1
_ZNSt8__detail30__integer_to_chars_is_unsignedIoEE:
	.byte	1
	.align 16
.LC1:
	.long	0
	.long	-2147483648
	.long	16385
	.long	0
	.align 16
.LC2:
	.long	0
	.long	-94967296
	.long	16413
	.long	0
	.align 16
.LC3:
	.long	0
	.long	-2147483648
	.long	16447
	.long	0
	.ident	"GCC: (GNU) 13.1.0"
	.def	_ZStlsIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_St13_Setprecision;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEe;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEPFRSoS_E;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEy;	.scl	2;	.type	32;	.endef
	.section	.rdata$.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_, "dr"
	.globl	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	.linkonce	discard
.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_:
	.quad	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	.section	.rdata$.refptr._ZSt4cout, "dr"
	.globl	.refptr._ZSt4cout
	.linkonce	discard
.refptr._ZSt4cout:
	.quad	_ZSt4cout
