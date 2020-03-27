# GetIRQVector MMURTL system call
# Takes 2 parameters
# Call gate at 0xF8
# Returns int

.section .text
.type GetIRQVector, @function
.globl GetIRQVector
GetIRQVector:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xF8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
