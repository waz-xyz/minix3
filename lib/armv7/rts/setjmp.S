.syntax unified
.global __setjmp
.global _setjmp
.global setjmp
.type __setjmp,%function
.type _setjmp,%function
.type setjmp,%function

__setjmp:
_setjmp:
setjmp:
	mov ip,r0
	stmia ip!,{v1,v2,v3,v4,v5,v6,sl,fp}
	mov r2,sp
	stmia ip!,{r2,lr}
	mov r0,#0

	.fpu vfp
	vstmia ip!, {d8-d15}
	.fpu softvfp
	.eabi_attribute Tag_FP_arch, 0
	.eabi_attribute Tag_ABI_HardFP_use, 0

	bx lr
