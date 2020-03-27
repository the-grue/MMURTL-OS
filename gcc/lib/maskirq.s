# MaskIRQ MMURTL system call
# Takes 1 parameter
# Call gate at 0xE0
# Returns int

.section .text
.type MaskIRQ, @function
.globl MaskIRQ
MaskIRQ:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0xE0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
