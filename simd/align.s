	.file	"align.cpp"
	.text
	.p2align 4,,15
	.globl	_Z4tempPfi
	.type	_Z4tempPfi, @function
_Z4tempPfi:
.LFB11212:
	.cfi_startproc
	testl	%esi, %esi
	jle	.L5
	subl	$1, %esi
	vxorps	%xmm2, %xmm2, %xmm2
	shrl	$3, %esi
	salq	$5, %rsi
	leaq	32(%rdi,%rsi), %rax
	.p2align 4,,10
	.p2align 3
.L3:
	vmovaps	(%rdi), %ymm0
	addq	$32, %rdi
	vcmpps	$29, %ymm2, %ymm0, %ymm1
	vandps	%ymm1, %ymm0, %ymm0
	vmovaps	%ymm0, -32(%rdi)
	cmpq	%rax, %rdi
	jne	.L3
	vzeroupper
.L5:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE11212:
	.size	_Z4tempPfi, .-_Z4tempPfi
	.p2align 4,,15
	.globl	_Z5temp1Pfi
	.type	_Z5temp1Pfi, @function
_Z5temp1Pfi:
.LFB11213:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	testl	%esi, %esi
	jle	.L11
	subl	$1, %esi
	vxorps	%xmm2, %xmm2, %xmm2
	shrl	$3, %esi
	salq	$5, %rsi
	leaq	32(%rdi,%rsi), %rax
	.p2align 4,,10
	.p2align 3
.L9:
	vmovaps	(%rdi), %ymm0
	addq	$32, %rdi
	vcmpps	$29, %ymm2, %ymm0, %ymm1
	vandps	%ymm1, %ymm0, %ymm0
	vmovaps	%ymm0, -32(%rdi)
	cmpq	%rax, %rdi
	jne	.L9
	vzeroupper
.L11:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE11213:
	.size	_Z5temp1Pfi, .-_Z5temp1Pfi
	.p2align 4,,15
	.globl	_Z5temp2Pfi
	.type	_Z5temp2Pfi, @function
_Z5temp2Pfi:
.LFB11214:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	testl	%esi, %esi
	jle	.L16
	subl	$1, %esi
	vxorps	%xmm2, %xmm2, %xmm2
	shrl	$3, %esi
	salq	$5, %rsi
	leaq	32(%rdi,%rsi), %rax
	.p2align 4,,10
	.p2align 3
.L14:
	vmovaps	(%rdi), %ymm1
	addq	$32, %rdi
	vcmpps	$29, %ymm2, %ymm1, %ymm0
	vandps	%ymm1, %ymm0, %ymm0
	vmovaps	%ymm0, -32(%rdi)
	cmpq	%rax, %rdi
	jne	.L14
	vzeroupper
.L16:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE11214:
	.size	_Z5temp2Pfi, .-_Z5temp2Pfi
	.section	.text._ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c,"axG",@progbits,_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c,comdat
	.p2align 4,,15
	.weak	_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c
	.type	_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c, @function
_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c:
.LFB11507:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdx, %r12
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	cmpq	%rsi, %rdi
	je	.L18
	leaq	4(%rdi), %rbx
	movl	%ecx, %ebp
	leaq	7(%rsp), %r13
	subq	%rbx, %rsi
	shrq	$2, %rsi
	leaq	4(%rdi,%rsi,4), %r14
	jmp	.L19
	.p2align 4,,10
	.p2align 3
.L24:
	addq	$4, %rbx
.L19:
	vxorpd	%xmm0, %xmm0, %xmm0
	vcvtss2sd	(%rdi), %xmm0, %xmm0
	movq	%r12, %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movl	$1, %edx
	movq	%rax, %rdi
	movq	%r13, %rsi
	movb	%bpl, 7(%rsp)
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	cmpq	%r14, %rbx
	movq	%rbx, %rdi
	jne	.L24
.L18:
	movq	8(%rsp), %rcx
	xorq	%fs:40, %rcx
	movq	%r12, %rax
	jne	.L25
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L25:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE11507:
	.size	_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c, .-_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB11215:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	movl	$4194304, %esi
	movl	$32, %edi
	pushq	-8(%r10)
	pushq	%rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_escape 0x10,0xf,0x2,0x76,0x78
	.cfi_escape 0x10,0xe,0x2,0x76,0x70
	.cfi_escape 0x10,0xd,0x2,0x76,0x68
	.cfi_escape 0x10,0xc,0x2,0x76,0x60
	movl	$16807, %r12d
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x58,0x6
	pushq	%rbx
	subq	$32, %rsp
	.cfi_escape 0x10,0x3,0x2,0x76,0x50
	call	aligned_alloc@PLT
	vmovss	.LC0(%rip), %xmm0
	movq	%rax, %r14
	movq	%rax, %rbx
	leaq	4194300(%rax), %r15
	movq	%rax, %r13
	vmovss	.LC3(%rip), %xmm2
	movabsq	$8589934597, %rcx
	vxorps	%xmm3, %xmm3, %xmm3
	vmovss	.LC2(%rip), %xmm4
	jmp	.L27
	.p2align 4,,10
	.p2align 3
.L39:
	vxorps	%xmm0, %xmm0, %xmm0
	vcvtsi2ssq	%rax, %xmm0, %xmm0
	vaddss	%xmm3, %xmm0, %xmm0
	vmulss	%xmm4, %xmm0, %xmm0
	vucomiss	%xmm2, %xmm0
	jnb	.L37
.L30:
	addq	$4, %r13
.L27:
	vaddss	%xmm0, %xmm0, %xmm0
	cmpq	%r15, %r13
	vsubss	%xmm2, %xmm0, %xmm0
	vmovss	%xmm0, 0(%r13)
	je	.L38
	imulq	$16807, %r12, %rsi
	movq	%rsi, %rax
	movq	%rsi, %r12
	mulq	%rcx
	subq	%rdx, %r12
	shrq	%r12
	addq	%rdx, %r12
	shrq	$30, %r12
	movq	%r12, %rax
	salq	$31, %rax
	subq	%r12, %rax
	subq	%rax, %rsi
	movq	%rsi, %rax
	movq	%rsi, %r12
	subq	$1, %rax
	jns	.L39
	movq	%rax, %rdx
	vxorps	%xmm0, %xmm0, %xmm0
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	vcvtsi2ssq	%rdx, %xmm0, %xmm0
	vaddss	%xmm0, %xmm0, %xmm0
	vaddss	%xmm3, %xmm0, %xmm0
	vmulss	%xmm4, %xmm0, %xmm0
	vucomiss	%xmm2, %xmm0
	jb	.L30
.L37:
	vxorps	%xmm1, %xmm1, %xmm1
	vmovaps	%xmm2, %xmm0
	vmovss	%xmm4, -68(%rbp)
	movq	%rcx, -64(%rbp)
	vmovss	%xmm3, -56(%rbp)
	vmovss	%xmm2, -52(%rbp)
	call	nextafterf@PLT
	vmovss	-68(%rbp), %xmm4
	movq	-64(%rbp), %rcx
	vmovss	-56(%rbp), %xmm3
	vmovss	-52(%rbp), %xmm2
	jmp	.L30
	.p2align 4,,10
	.p2align 3
.L38:
	leaq	80(%r14), %r12
	leaq	_ZSt4cout(%rip), %rdx
	movl	$9, %ecx
	movq	%r14, %rdi
	movq	%r12, %rsi
	call	_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@PLT
	vxorps	%xmm2, %xmm2, %xmm2
	leaq	4194304(%r14), %rax
	.p2align 4,,10
	.p2align 3
.L33:
	vmovaps	(%rbx), %ymm0
	addq	$32, %rbx
	vcmpps	$29, %ymm2, %ymm0, %ymm1
	vandps	%ymm1, %ymm0, %ymm0
	vmovaps	%ymm0, -32(%rbx)
	cmpq	%rbx, %rax
	jne	.L33
	leaq	_ZSt4cout(%rip), %rdx
	movl	$9, %ecx
	movq	%r12, %rsi
	movq	%r14, %rdi
	vzeroupper
	call	_ZN5MYSTD6MYFILE11print_rangeIPfEERSoT_S4_S3_c
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@PLT
	movq	%r14, %rdi
	call	free@PLT
	addq	$32, %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r10
	.cfi_def_cfa 10, 0
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11215:
	.size	main, .-main
	.p2align 4,,15
	.type	_GLOBAL__sub_I__Z4tempPfi, @function
_GLOBAL__sub_I__Z4tempPfi:
.LFB11975:
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
.LFE11975:
	.size	_GLOBAL__sub_I__Z4tempPfi, .-_GLOBAL__sub_I__Z4tempPfi
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z4tempPfi
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	922962944
	.align 4
.LC2:
	.long	805306368
	.align 4
.LC3:
	.long	1065353216
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
