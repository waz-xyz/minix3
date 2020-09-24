.syntax unified
.global _longjmp
.global longjmp
.type _longjmp,%function
.type longjmp,%function

_longjmp:
longjmp:
	mov ip,r0
	movs r0,r1
	moveq r0,#1
	ldmia ip!, {v1,v2,v3,v4,v5,v6,sl,fp}
	ldmia ip!, {r2,lr}
	mov sp,r2

	.fpu vfp
	vldmia ip!, {d8-d15}
	.fpu softvfp
	.eabi_attribute Tag_FP_arch, 0
	.eabi_attribute Tag_ABI_HardFP_use, 0

	bx lr
