	.arch armv7a
	.syntax unified

	.section .data

        .global _brksize
        .align 2

_brksize:
        .word   endbss
