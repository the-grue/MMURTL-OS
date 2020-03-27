# CopyDataR MMURTL system call
# Takes 3 parameters
# Call gate at 0x198
# Returns int

.section .text
.type CopyDataR, @function
.globl CopyDataR
CopyDataR:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x198, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
