# SetIRQVector MMURTL system call
# Takes 2 parameters
# Call gate at 0xF0
# Returns int

.section .text
.type SetIRQVector, @function
.globl SetIRQVector
SetIRQVector:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xF0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
