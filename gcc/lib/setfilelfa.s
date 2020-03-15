# SetFileLFA MMURTL system call
# Takes 2 parameters
# Call gate at 0x260
# Returns int

.section .text
.type SetFileLFA, @function
.globl SetFileLFA
SetFileLFA:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x260, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
