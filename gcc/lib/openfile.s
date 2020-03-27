# OpenFile MMURTL system call
# Takes 5 parameters
# Call gate at 0x228
# Returns int

.section .text
.type OpenFile, @function
.globl OpenFile
OpenFile:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	lcall	$0x228, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
