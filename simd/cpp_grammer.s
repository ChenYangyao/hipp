	.file	"cpp_grammer.cpp"
	.text
	.p2align 4,,15
	.globl	_Z6addtwof
	.type	_Z6addtwof, @function
_Z6addtwof:
.LFB11125:
	.cfi_startproc
	vpxor	%xmm1, %xmm1, %xmm1
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	vmovq	%xmm1, %rax
	shrq	$32, %rax
	movq	%rax, 8(%rsp)
	vmovss	8(%rsp), %xmm2
	vaddss	.LC0(%rip), %xmm2, %xmm0
	vmovd	%xmm0, %eax
	vpinsrd	$1, %eax, %xmm1, %xmm0
	movq	24(%rsp), %rax
	xorq	%fs:40, %rax
	vaddps	%xmm0, %xmm0, %xmm0
	jne	.L6
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE11125:
	.size	_Z6addtwof, .-_Z6addtwof
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.type	_GLOBAL__sub_I__Z6addtwof, @function
_GLOBAL__sub_I__Z6addtwof:
.LFB11837:
	.cfi_startproc
	leaq	_ZStL8__ioinit(%rip), %rdi
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	_ZNSt8ios_base4InitC1Ev@PLT
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rdi
	leaq	__dso_handle(%rip), %rdx
	leaq	_ZStL8__ioinit(%rip), %rsi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	__cxa_atexit@PLT
	.cfi_endproc
.LFE11837:
	.size	_GLOBAL__sub_I__Z6addtwof, .-_GLOBAL__sub_I__Z6addtwof
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z6addtwof
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	1065353216
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
