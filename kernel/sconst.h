// Miscellaneous constants used in assembler code.

#ifndef	SCONST_H
#define	SCONST_H

#if (CHIP == INTEL)

#define	W	_WORD_SIZE	// Machine word size.

// Offsets in struct proc. They MUST match proc.h.
#define	P_STACKBASE	0
#if _WORD_SIZE == 2
#define	ESREG		P_STACKBASE
#else
#define	GSREG		P_STACKBASE
#define	FSREG		GSREG + 2       // 386 introduces FS and GS segments
#define	ESREG		FSREG + 2
#endif
#define	DSREG		ESREG + 2
#define	DIREG		DSREG + 2
#define	SIREG		DIREG + W
#define	BPREG		SIREG + W
#define	STREG		BPREG + W	// hole for another SP
#define	BXREG		STREG + W
#define	DXREG		BXREG + W
#define	CXREG		DXREG + W
#define	AXREG		CXREG + W
#define	RETADR		AXREG + W       // return address for save() call
#define	PCREG		RETADR + W
#define	CSREG		PCREG + W
#define	PSWREG		CSREG + W
#define	SPREG		PSWREG + W
#define	SSREG		SPREG + W
#define	P_STACKTOP	SSREG + W
#define	P_LDT_SEL	P_STACKTOP
#define	P_LDT		P_LDT_SEL + W

#if _WORD_SIZE == 2
#define	Msize		12		// size of a message in 16-bit words
#else
#define	Msize		9		// size of a message in 32-bit words
#endif

#elif (CHIP == ARM)

// Offsets in struct proc. They MUST match proc.h.

#define	PCREG_OFFSET	(15 * 4)
#define	PSWREG_OFFSET	(16 * 4)
#define	P_TTBASE_OFFSET	(17 * 4)
#define P_ASID_OFFSET   (18 * 4)
#define	P_NR_OFFSET	(19 * 4)

#define	MODE_SVC	0x13

#endif

#endif /* SCONST_H */
